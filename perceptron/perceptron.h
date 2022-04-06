#ifndef PERCEPTRON_H
#define PERCEPTRON_H
#include <math.h>
#include <mnist.h>
#include <QVector>

// A perceptron takes several binary inputs, x1,x2,…x1,x2,…, and produces a
// single binary output

class Perceptron {
 public:
  typedef uchar binary;
  typedef QVector<binary> Inputs, Outputs;
  typedef QVector<double> Weights;

 public:
  QVector<binary> inputs;

  /*
   * Rosenblatt proposed a simple rule to compute the output. He introduced
   * weights, w1,w2,…w1,w2,…, real numbers expressing the importance of the
   * respective inputs to the output. The neuron's output, 00 or 11, is
   * determined by whether the weighted sum ∑jwjxj∑jwjxj is less than or greater
   * than some threshold value.
   */
  Weights weights;
  double threshold, bias;

  void setBias(double bias) {
    this->bias = bias;
    threshold = -bias;
  }

  void setInputs(QVector<binary> inputs) { this->inputs = inputs; }

  // output={0 if w⋅x+b≤0,  1 if w⋅x+b>0}
  binary output() {
    double wx = 0;
    for (int i = 0; i < inputs.size(); i++) wx += inputs[i] * weights[i];
    return (wx + bias <= 0 ? 0 : 1);
  }
  binary output(Inputs inputs) {
    this->inputs = inputs;
    return output();
  }

  Perceptron();
  Perceptron(Weights weights, double bias) {
    this->weights = weights;
    this->bias = bias;
  }
};

class NN : public Perceptron {
 public:
  typedef QVector<Perceptron> Layer;
  Layer hidden, output;
};

// Perceptron samples
// NAND gate: Perceptron w/2 inputs and one output
class NAND : public Perceptron {
 public:
  Perceptron* p = new Perceptron({-2, -2}, 3);
  NAND() {}
  binary output(Inputs in) { return p->output(in); }
};

// binary sum
class BinSum : public NAND {
 public:
  BinSum() : NAND() {}
  QVector<binary> output(Inputs inputs) {
    QVector<binary> outs(5);

    class Links {
     public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuninitialized"
      Links() : i0(i0), i1(i1) {}
#pragma clang diagnostic pop

      Links(binary& _i0, binary& _i1) : i0(_i0), i1(_i1) {}
      binary &i0, &i1;
    };

    QVector<Links> connectMap = {
        Links(inputs[0], inputs[1]), Links(inputs[0], outs[0]),
        Links(inputs[1], outs[0]), Links(outs[0], outs[0]),
        Links(outs[1], outs[2])};

    for (int i = 0; i < connectMap.size(); i++)
      outs[i] = NAND::output({connectMap[i].i0, connectMap[i].i1});
    return {outs[4], outs[3]};
  }
};

// Sigmoid neurons are similar to perceptrons, but modified so that small
// changes in their weights and bias cause only a small change in their output.

class SigmoidNetwork {
 public:
  typedef QVector<double> dv;
  typedef enum { tInput, tHidden, tOutput } LayerType;

  static double rand01() { return (double)qrand() / RAND_MAX; }

  class Neuron {
   public:
    Neuron() {}
    Neuron(int nInput) { weights = dv(nInput, rand01()); }
    double bias = rand01(), output = 0;
    dv weights;
  };

  class Layer {
   public:
    Layer() {}

    Layer(LayerType type, int n, int nw)
        : neurons(QVector<Neuron>(n)), type(type) {
      for (auto nn : neurons) nn.weights = dv(nw, rand01());
      initWB();
    }
    int size() { return neurons.size(); }
    void initWB() {
      for (auto nn : neurons) {
        nn.bias = rand01();
        for (auto w : nn.weights) w = rand01();
      }
    }

    void doHidden(const dv& input) {  // input -> hidden
      for (auto nn : neurons) {       // calc all output of hidden layer
        double wx = nn.bias;
        for (int i = 0; i < input.size(); i++) wx += input[i] * nn.weights[i];
        nn.output = sigmoid(wx);
      }
    }

    void doOutput(const Layer& hidden) {  // hidden -> output
      for (auto nn : neurons) {
        double wx = nn.bias;
        for (int i = 0; i < hidden.neurons.size(); i++)
          wx += hidden.neurons[i].output * nn.weights[i];
        nn.output = sigmoid(wx);
      }
    }

    // Sigmoid neurons are similar to perceptrons, but modified so that small
    // changes in their weights and bias cause only a small change in their
    // output. That's the crucial fact which will allow a network of sigmoid
    // neurons to learn.

    double sigmoid(double z) { return 1 / (1 + exp(-z)); }

   public:
    QVector<Neuron> neurons;
    LayerType type;
  };

  Layer hidden, outputs;
  double sCost = 0;

 public:
  SigmoidNetwork() {}
  SigmoidNetwork(int nInput, int nHidden, int nOutput) {
    hidden = Layer(tHidden, nHidden, nInput);
    outputs = Layer(tOutput, nOutput, nHidden);
  }
  // To quantify how well we're achieving this goal we define a
  // cost function. Sometimes referred to as a loss or
  // objective function. We'll call CC the quadratic cost
  // function; it's also sometimes known as the mean squared
  // error or just MSE
  // C(w,b)≡1/2n∑‖y(x)−a‖^2.

  //  cost(w,b) = 1/2n * (for all training inputs(x) |y(x)-a|^2)
  // a  is the vector of outputs from the network when x is input
  // y(x) desired output from the NN
  // |v|=dist(q,p) = sqrt( sum( (q(i)-p(i))^2 ) )

  void startTrain() { sCost = 0; }
  void train(const dv& input, const dv& desiredOut) {
    hidden.doHidden(input);
    outputs.doOutput(hidden);

    sCost += costTrain(desiredOut);
  }
  double getMSE() { return cost(sCost); }

  // we want to find a set of weights and biases which make the cost as small as
  // possible. We'll do that using an algorithm known as gradient descent.
  // wk→w′k=wk−η/m∑j∂CXj/∂wk
  // bl→b′l=bl−η/m∑j∂CXj/∂bl

 private:
  inline double sqr(double x) { return x * x; }

  // MSE between NN output & testOutput.
  // cost once train() check agains outputs.neurons[i].output
  double costTrain(const dv& testOutput) {
    double ss = 0;
    for (int i = 0; i < testOutput.size(); i++)
      ss += sqr(testOutput[i] - outputs.neurons[i].output);
    return ss / 2;
  }
  double cost(int nTrains) { return (1. / nTrains) * sCost; }
};

class NumberHandwritting : public SigmoidNetwork {
  MNIST mnist;

 public:
  // stochastic gradient descent works by randomly picking out a small number mm
  // of randomly chosen training inputs -> m
  //  sum(i=0; i<m; ++)  train( set[i], test[i] )
  // gradient C = 1/m * getMSE()
  // learning rate η is a small, positive parameter Δv=−η∇C

  NumberHandwritting()
      : SigmoidNetwork(784, 15,
                       10) {  // input 784=28x28, hidden=15, output = 0..9
  }

  void learn() {
    int m = 10;  // random set size

    startTrain();

    for (int i = 0; i < m; i++) {
      int nit = qrand() % mnist.getTrainingItems();

      auto inImg = mnist.getTrainImageasInput(nit);
      auto inLbl = mnist.getTrainLabelasInput(nit);

      train(inImg, inLbl);
    }

    double dc = getMSE();
  }
};

#endif  // PERCEPTRON_H
