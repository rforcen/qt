#pragma once

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/video/tracking.hpp"

using namespace cv;
using namespace ml;
using namespace std;

class Model {
public:
  Mat img;
  Mat samples;   // Set of train samples. Contains points on image
  Mat responses; // Set of responses for train samples
  Mat weights;
  float shift = 0;

  Model() {
    const int WIDTH = 1024 * 2, HEIGHT = 1024 * 2;

    img = Mat::zeros(HEIGHT, WIDTH, CV_8UC3);
  }

private:
  bool doTrain() {
    Ptr<SVMSGD> svmsgd = SVMSGD::create();

    Ptr<TrainData> trainData =
        TrainData::create(samples, ROW_SAMPLE, responses);
    bool ok = svmsgd->train(trainData);

    if (ok && svmsgd->isTrained()) {
      weights = svmsgd->getWeights();
      shift = svmsgd->getShift();

      return true;
    }
    return false;
  }

  void fillSegments(vector<pair<Point, Point>> &segments, int width,
                    int height) {
    pair<Point, Point> currentSegment;

    currentSegment.first = Point(width, 0);
    currentSegment.second = Point(width, height);
    segments.push_back(currentSegment);

    currentSegment.first = Point(0, height);
    currentSegment.second = Point(width, height);
    segments.push_back(currentSegment);

    currentSegment.first = Point(0, 0);
    currentSegment.second = Point(width, 0);
    segments.push_back(currentSegment);

    currentSegment.first = Point(0, 0);
    currentSegment.second = Point(0, height);
    segments.push_back(currentSegment);
  }

  bool findCrossPointWithBorders(const pair<Point, Point> &segment,
                                 Point &crossPoint) {
    int x = 0;
    int y = 0;
    int xMin = min(segment.first.x, segment.second.x);
    int xMax = max(segment.first.x, segment.second.x);
    int yMin = min(segment.first.y, segment.second.y);
    int yMax = max(segment.first.y, segment.second.y);

    float w0 = weights.at<float>(0), w1 = weights.at<float>(1);

    CV_Assert(weights.type() == CV_32FC1);
    CV_Assert(xMin == xMax || yMin == yMax);

    if (xMin == xMax && w1 != 0) {
      x = xMin;
      y = static_cast<int>(floor(-(w0 * x + shift) / w1));
      if (y >= yMin && y <= yMax) {
        crossPoint.x = x;
        crossPoint.y = y;
        return true;
      }
    } else if (yMin == yMax && w0 != 0) {
      y = yMin;
      x = static_cast<int>(floor(-(w1 * y + shift) / w0));
      if (x >= xMin && x <= xMax) {
        crossPoint.x = x;
        crossPoint.y = y;
        return true;
      }
    }
    return false;
  }

  bool findPointsForLine(Point points[2], int width, int height) {
    if (weights.empty()) {
      return false;
    }

    int foundPointsCount = 0;
    vector<pair<Point, Point>> segments;
    fillSegments(segments, width, height);

    for (uint i = 0; i < segments.size(); i++) {
      if (findCrossPointWithBorders(segments[i], points[foundPointsCount]))
        foundPointsCount++;
      if (foundPointsCount >= 2)
        break;
    }

    return true;
  }

  void redraw(const Point points[2]) {
    img.setTo(0);
    Point center;
    int radius = 3;
    Scalar color;
    CV_Assert((samples.type() == CV_32FC1) && (responses.type() == CV_32FC1));
    for (int i = 0; i < samples.rows; i++) {
      center.x = static_cast<int>(samples.at<float>(i, 0));
      center.y = static_cast<int>(samples.at<float>(i, 1));
      color = (responses.at<float>(i) > 0) ? Scalar(128, 128, 0)
                                           : Scalar(0, 128, 128);
      circle(img, center, radius, color, 2);
    }
    line(img, points[0], points[1], Scalar(1, 255, 1));
  }

public:
  void addRandom(int n = 1000) {
    srand(time(NULL));
    Mat currentSample(1, 2, CV_32FC1);

    for (int i = 0; i < n; i++) {
      int x = rand() % img.cols, y = rand() % img.rows, r = rand() & 1 ? 1 : -1;
      currentSample.at<float>(0, 0) = (float)x;
      currentSample.at<float>(0, 1) = (float)y;
      samples.push_back(currentSample);
      responses.push_back(static_cast<float>(r));
    }
    if (doTrain()) {
      Point points[2];
      findPointsForLine(points, img.cols, img.rows);

      redraw(points);
    }
  }

  void addPointRetrainAndRedraw(int x, int y, int response) {
    Mat currentSample(1, 2, CV_32FC1);

    currentSample.at<float>(0, 0) = (float)x;
    currentSample.at<float>(0, 1) = (float)y;

    samples.push_back(currentSample);
    responses.push_back(static_cast<float>(response));

    if (doTrain()) {
      Point points[2];
      findPointsForLine(points, img.cols, img.rows);

      redraw(points);
    }
  }
};
