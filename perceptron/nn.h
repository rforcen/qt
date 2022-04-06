#ifndef NN_H
#define NN_H

#include <eigen.h>
#include <mnist.h>
#include <trainingResult.h>
#include <QObject>

#include <future>
#include <thread>

using namespace std;

class NN : public QObject {
  Q_OBJECT

 public:
  initializer_list<int> sizes;
  int nLayers = 0;
  mdMatrix w, deltaW, nw;
  mdVector b, deltaB, nb;

  MNIST mnist;
  mmdVector test;
  int miniBatchSize = 10;
  bool _abort = false;

 signals:
  void nnProgress(int epoch, double hitFactor);

 public:
  NN() {}

  // { neurons per layer } -> {784, 10, 7, 9, 10}, {784, 30, 10}
  NN(initializer_list<int> il) : sizes(il), nLayers(il.size()) { initWB(); }
  void initWB() {
    w = mdMatrix(nLayers - 1);  // input is no layer
    b = mdVector(nLayers - 1);

    auto iter = sizes.begin();

    for (int l = 0; l < nLayers - 1; l++, iter++) {
      int sz = *iter, sz1 = *(iter + 1);  // {784,10} {10,7} {7,9} {9,10}

      w[l] = mat::Random(sz1, sz);
      b[l] = vec::Random(sz1);
    }
    nw = deltaW = w;
    nb = deltaB = b;
  }

  ~NN() {}

  void abort(bool _abort = true) { this->_abort = _abort; }

  double SGD(mmdVector trainingData, int nEpochs,
             double eta) {  // Train the neural network using
                            // Stochastic Gradient Descent, eta= learning rate

    for (int epoch = 0; epoch < nEpochs; epoch++) {
      emit nnProgress(epoch, testFactor());

      int nt = shuffle(trainingData);
      for (int k = 0; k < nt; k += miniBatchSize) {
        setZeroM(nw);
        setZeroV(nb);

        for (int nBatch = 0; nBatch < miniBatchSize;
             nBatch++) {  // perform miniBatch
          backprop(trainingData[images][k + nBatch],
                   trainingData[labels][k + nBatch]);  // -> deltaW, deltaB

          nb += deltaB, nw += deltaW;
        }

        w -= mult(nw, eta / miniBatchSize);  // w-=nw*eta/mbs
        b -= mult(nb, eta / miniBatchSize);  // b-=nb*eta/mbs
      }
    }

    return testFactor();
  }

  void backprop(vec &in, vec &test) {  // generate deltaW, deltaB
    // feed fwd
    auto activation = in;
    vector<vec> activations, zs;

    activations.push_back(activation);

    for (int l = 0; l < w.rows(); l++) {
      vec z = w[l] * activation + b[l];
      zs.push_back(z);
      activation = z.unaryExpr(ptr_fun(sigmoid));  // sigmoid(z);
      activations.push_back(activation);
    }

    setZeroM(deltaW);
    setZeroV(deltaB);

    vec delta = cost_derivative(activations.back(), test)
                    .cwiseProduct(sigmoid_prime(zs.back()));

    deltaB[last(deltaB)] = delta;
    deltaW[last(deltaW)] =
        delta * activations[activations.size() - 2].transpose();

    for (int l = 2; l < nLayers; l++) {
      // delta= (w[-l+1] * delta) * sigmoid_prime(z)
      delta = (w[negx(w, l - 1)].transpose() * delta)
                  .cwiseProduct(
                      zs[zs.size() - l].unaryExpr(ptr_fun(sigmoid_prime)));

      deltaB[negx(deltaB, l)] = delta;
      deltaW[negx(deltaW, l)] =
          delta * activations[activations.size() - l - 1].transpose();
    }
  }

  double SGD_multithread(
      mmdVector trainingData, int nEpochs,
      double eta) {  // Train the neural network using
                     // Stochastic Gradient Descent, eta= learning rate

    abort(false);

    // multithread support
    static const int nTh = 8;  // # of threads
    mdMatrix deltabkW[nTh];    // w,b[nTh] arrays
    mdVector deltabkB[nTh];
    std::thread bkThread[nTh];  // threads

    for (int i = 0; i < nTh; i++) {  //  init deltabk(W,B) shapes
      deltabkB[i] = b;
      deltabkW[i] = w;
    }

    for (int epoch = 0; epoch < nEpochs && !_abort; epoch++) {
      emit nnProgress(epoch, testFactor());

      int nt = shuffle(trainingData);
      for (int k = 0; k < nt && !_abort; k += nTh) {
        setZeroM(nw);
        setZeroV(nb);

        for (int nBatch = 0; nBatch < nTh;
             nBatch++) {  // perform multithreaded miniBatch

          bkThread[nBatch] = std::thread(&NN::backprop_multithread, this,
                                         &trainingData[images][k + nBatch],
                                         &trainingData[labels][k + nBatch],
                                         &deltabkW[nBatch], &deltabkB[nBatch]);
        }

        for (int i = 0; i < nTh; i++) {  // wait completition & accumulate
          bkThread[i].join();
          nb += deltabkB[i], nw += deltabkW[i];
        }

        // update net (w,b)
        w -= mult(nw, eta / nTh);  // w-=nw*eta/mbs
        b -= mult(nb, eta / nTh);  // b-=nb*eta/mbs
      }
    }

    return testFactor();
  }

  void backprop_multithread(
      vec *in, vec *test, mdMatrix *_deltaW,
      mdVector *_deltaB) {  // <- VectorD in, test ->generate
                            // deltabkW[i], deltabkB[i]

    auto &deltaB = *_deltaB;  // makes it easier to address
    auto &deltaW = *_deltaW;

    // feed fwd
    auto activation = *in;
    vector<vec> activations, zs;

    activations.push_back(activation);

    for (int l = 0; l < w.rows(); l++) {
      vec z = w[l] * activation + b[l];
      zs.push_back(z);
      activation = z.unaryExpr(ptr_fun(sigmoid));  // sigmoid(z);
      activations.push_back(activation);
    }

    setZeroM(deltaW);
    setZeroV(deltaB);

    vec delta = cost_derivative(activations.back(), *test)
                    .cwiseProduct(sigmoid_prime(zs.back()));

    deltaB[last(deltaB)] = delta;
    deltaW[last(deltaW)] =
        delta * activations[activations.size() - 2].transpose();

    for (int l = 2; l < nLayers; l++) {
      // delta= (w[-l+1] * delta) * sigmoid_prime(z)
      delta = (w[negx(w, l - 1)].transpose() * delta)
                  .cwiseProduct(
                      zs[zs.size() - l].unaryExpr(ptr_fun(sigmoid_prime)));

      deltaB[negx(deltaB, l)] = delta;
      deltaW[negx(deltaW, l)] =
          delta * activations[activations.size() - l - 1].transpose();
    }
  }

  vec feedFWD(vec in) { return feedFWD(nLayers - 2, in); }
  vec feedFWD(int layer, vec in) {  // call w/nLayers-1
    return (layer >= 0) ? sigmoidDot(w[layer], feedFWD(layer - 1, in), b[layer])
                        : in;
  }

  vec feedFWDIter(vec in) {  // iterate version
    for (int layer = 0; layer < nLayers - 1; layer++)
      in = sigmoidDot(w[layer], in, b[layer]);
    return in;
  }

  void readTrainingResult() {  //  from weight, biases in TrainingResults
                               //  namespace (python calc. parameters)
    for (int l = 0; l < TrainingResults::weights.size(); l++) {
      auto w1 = TrainingResults::weights[l];
      for (int r = 0; r < w1.size(); r++)
        for (int c = 0; c < w1[r].size(); c++) w[l](r, c) = w1[r][c];
      auto b1 = TrainingResults::biases[l];
      for (int r = 0; r < b1.size(); r++) b[l](r) = b1[r];
    }
  }

  byte evaluate(vec in) {  // evaluate net and return label
    return pmax(feedFWD(nLayers - 2, in));
  }

  double testFactor(int nr = 0) {
    double s = 0;
    int rows = nr ? nr : test[images].rows();
    for (int k = 0; k < rows; k++)
      if (evaluate(test[images][k]) == pmax(test[labels][k])) s++;
    return s / (rows ? rows : 1);
  }

  void setTest(const mmdVector &testData) { this->test = testData; }

  byte testTestItem(int ix) {
    auto in = mnist.getTestImageasInputVector(ix);
    return evaluate(in);
  }
  byte testTrainItem(int ix) {
    auto in = mnist.getTrainImageasInputVector(ix);
    return evaluate(in);
  }
  void testsgd() { SGD(mnist.getTraining(), 10, 3.0); }
  void testfeed() {
    auto in = mnist.getTrainImageasInputVector();

    vec oRec = feedFWD(in);
    cout << oRec;
  }

  bool writeTraining(QString fileName) {
    bool ok = false;

    filebuf fb;
    if ((ok = fb.open(fileName.toLocal8Bit().data(), ios::out))) {
      ostream ds(&fb);
      for (int r = 0; r < w.rows(); r++) ds << w[r];
      for (int r = 0; r < b.rows(); r++) ds << b[r];
      fb.close();
    }
    return ok;
  }
  bool readTraining(QString fileName) {
    bool ok = false;

    filebuf fb;
    if ((ok = fb.open(fileName.toLocal8Bit().data(), ios::in))) {
      initWB();
      istream ds(&fb);
      for (int r = 0; r < w.rows(); r++)
        for (int rr = 0; rr < w[r].rows(); rr++)
          for (int cc = 0; cc < w[r].cols(); cc++) ds >> w[r](rr, cc);
      for (int r = 0; r < b.rows(); r++)
        for (int rr = 0; rr < b[r].rows(); rr++) ds >> b[r](rr);
      fb.close();
    }
    return ok;
  }

 private:
  mdMatrix mult(mdMatrix m, double d) {  // m*=d
    for (int nm = 0; nm < m.rows(); nm++) m[nm] *= d;
    return m;
  }
  mdVector mult(mdVector v, double d) {  // v*=d
    for (int nm = 0; nm < v.rows(); nm++) v[nm] *= d;
    return v;
  }

  void setZeroM(mdMatrix &m) {  // set to zero,  to existing shape
    for (int i = 0; i < m.rows(); i++) m(i).fill(0);
  }
  void setZeroV(mdVector &v) {  // set to zero, to existing shape
    for (int i = 0; i < v.rows(); i++) v[i].fill(0);
  }
  int shuffle(mmdVector &traningData) {
    auto idx =
        rndRange(traningData[0].rows());  // generate a random set of indices
    for (auto i : {0, 1})
      for (int r = 0; r < traningData[i].rows(); r++)
        swap<vec>(traningData[i][r], traningData[i][idx[r]]);

    return idx.rows();
  }

  // Return the vector of partial derivatives partial C_x  / partial a for
  // the output activations
  vec cost_derivative(const vec &output_activations, const vec &y) {
    return output_activations - y;
  }
  vec sigmoidDot(const mat &m, const vec &in, const vec &b) {
    vec o = m * in + b;
    return o.unaryExpr(ptr_fun(sigmoid));
  }



  inline int last(const mdVector &v) { return v.rows() - 1; }
  inline int last(const mdMatrix &m) { return m.rows() - 1; }
  inline int negx(const mdVector &v, int ix) { return v.rows() - ix; }
  inline int negx(const mdMatrix &m, int ix) { return m.rows() - ix; }

  vec sigmoid(vec &z) {
    for (int r = 0; r < z.rows(); r++) z[r] = sigmoid(z[r]);
    return z;
  }
  vec sigmoid_prime(vec &z) {
    for (int r = 0; r < z.rows(); r++) z[r] = sigmoid_prime(z[r]);
    return z;
  }

 public:
  int pmax(const vec &v) {  //  postion of max coeff
    VectorXd::Index ix;
    v.maxCoeff(&ix);
    return ix;
  }

  static inline double sigmoid(double z) { return 1. / (1. + exp(-z)); }
  static inline double sigmoid_prime(
      double z) {  // Derivative of the sigmoid function
    double sg = sigmoid(z);
    return sg * (1. - sg);
  }
};

#endif  // NN_H
