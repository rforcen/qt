#pragma once

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace ml;
using namespace std;
using namespace dnn;

#include <iostream>
#include <mnist.h>
#include <sys/stat.h>
#include <unistd.h>

// mnistmodel.o resource bindings (protbuff, caffe model)
#define BINSIZE(s) (size_t)(&s##_end - &s##_start)

extern const char _binary_lenet_iter_10000_caffemodel_end,
    _binary_lenet_iter_10000_caffemodel_start, _binary_lenet_prototxt_end,
    _binary_lenet_prototxt_start;
//

class NN {
public:
  NN() { loadMNIST(); }

  void train(string path) {
    this->path = path;
    learnFile = path + "/" + learnName;

    if (!loadTrain())
      doTrain();
  }

  // trained model based as resource
  void loadModel() {
    net = readNetFromCaffe(&_binary_lenet_prototxt_start,
                           BINSIZE(_binary_lenet_prototxt),
                           &_binary_lenet_iter_10000_caffemodel_start,
                           BINSIZE(_binary_lenet_iter_10000_caffemodel));

    net.setPreferableBackend(backendId);
    net.setPreferableTarget(targetId);
  }

  // insite conversion from (1,768, float) -> (28,28, uchar)
  void conv8UC1(Mat &m) {
    Mat img(28, 28, CV_8UC1);

    // convert to byte gray scale
    for (int i = 0; i < m.cols; i++)
      img.at<uchar>(i) = (uchar)(m.at<float>(i) * 127.f);
    m = img;
  }

  // test is Mat(CV_32FC1) -> CV_8UC1
  void eval(Mat img, int &res, double &confidence) {
    conv8UC1(img);

    Mat blob; // blob it
    blobFromImage(img, blob);

    net.setInput(blob);
    Mat prob = net.forward(); // evaluate NN

    // Get a class with a highest score
    Point p;
    minMaxLoc(prob, 0, &confidence, 0, &p);
    res = p.x;
  }

  int countErrors(Mat &testImg, Mat &testLabel) {
    int error = 0;

    for (int t = 0; t < testImg.rows; t++) {

      int res;
      double conf;

      eval(testImg.row(t), res, conf);

      if (res != posMax(testLabel.row(t)))
        ++error;
    }
    return error;
  }

  void testConfidence() {
    int error = countErrors(testImg, testLabel);
    error += countErrors(samples, responses);

    errorRate = (float)error / (testImg.rows + samples.rows);
    accuracy = 1 - errorRate;
  }

public:
  // caffe model
  Net net;
  int backendId = 0;
  int targetId = 0;

  MNIST mnist;
  string learnName = "mnist.nn", path = "", learnFile = "";

  Ptr<ANN_MLP> ann = nullptr;

  Mat samples, responses; // samples, response
  Mat testImg, testLabel;
  float errorRate, accuracy;

  float percTrain = 99; // 90% train, rest test

  int maxCount = 1500;
  double epsilon = 1e-6;
  double precision = 1e-6;

public:
  void loadMNIST() {
    samples = mnist.getTrainingImages();
    responses = mnist.getTrainingLabels();

    testImg = mnist.getTestImages();
    testLabel = mnist.getTestLabels();
  }

  bool loadTrain() {

    if (access((learnFile).c_str(), F_OK) == -1)
      return false;

    ann = ANN_MLP::load(learnFile);

    testAccuracy();
    return true;
  }

  int check(Mat mdigit) {
    Mat result;
    ann->predict(mdigit, result);
    return posMax(result);
  }

private:
  int posMax(Mat m) {
    float max = -1e32;
    int pmax = -1;

    for (int i = 0; i < m.cols; i++) {
      auto x = m.at<float>(i);
      if (x > max) {
        pmax = i;
        max = x;
      }
    }

    return pmax;
  }

  void printSomeLabels(int n = 10) {
    srand(time(0));
    for (int i = 0; i < n; i++) {
      int ix = rand() % testImg.cols;

      printf("label:%d\n", posMax(testLabel.row(ix)));
      mnist.printImg(testImg.row(ix));
    }
  }

  void testAccuracy() {

    // test accuracy
    Mat result;

    ann->predict(testImg, result); // apply NN to testSamples -> result

    int error = 0;
    for (int i = 0; i < result.rows; i++) {

      int prediction = posMax(result.row(i)),
          testResponse = posMax(testLabel.row(i));

      if (prediction != testResponse)
        error++;
    }

    errorRate = (float)error / testImg.rows;
    accuracy = 1 - errorRate;
  }

  bool
  doTrain() { // based on :
              // https://amin-ahmadi.com/2020/04/14/using-opencv-ann-mlp-to-train-a-model-on-iris-flower-dataset/

    // layers
    int nInput = samples.cols, nOutout = responses.cols;

    Mat layers = (Mat_<double>(3, 1) << nInput, 30, nOutout); // {784, 30, 10}

    ann = ANN_MLP::create();

    ann->setLayerSizes(layers); // define ann
    ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 0, 0);
    // maxCount: The maximum number of iterations or elements to compute.
    ann->setTermCriteria(TermCriteria(
        TermCriteria::MAX_ITER + TermCriteria::EPS, maxCount, epsilon));
    ann->setTrainMethod(ANN_MLP::BACKPROP, precision);

    // train data
    Ptr<TrainData> trainData =
        TrainData::create(samples, ROW_SAMPLE, responses);

    trainData->setTrainTestSplitRatio(percTrain / 100.0, true);

    bool ok = ann->train(trainData); // UPDATE_MODEL

    if (ok)
      ann->save(learnFile); // save nn file

    return ok;
  }
};
