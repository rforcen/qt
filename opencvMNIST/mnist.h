#ifndef MNIST_H
#define MNIST_H

#include <QDataStream>
#include <QFile>
#include <QImage>
#include <QString>
#include <QVector>

#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

enum { tInput, tOutput }; // training/test order

/*
MNIST files

TRAINING SET IMAGE FILE (train-images-idx3-ubyte):

[offset] [type]          [value]          [description]
0000     32 bit integer  0x00000803(2051) magic number
0004     32 bit integer  60000            number of images
0008     32 bit integer  28               number of rows
0012     32 bit integer  28               number of columns
0016     unsigned byte   ??               pixel
0017     unsigned byte   ??               pixel
........
xxxx     unsigned byte   ??               pixel
Pixels are organized row-wise. Pixel values are 0 to 255. 0 means background
(white), 255 means foreground (black).

  TRAINING SET LABEL FILE (train-labels-idx1-ubyte):

  [offset] [type]          [value]          [description]
  0000     32 bit integer  0x00000801(2049) magic number (MSB first)
  0004     32 bit integer  60000            number of items
  0008     unsigned byte   ??               label
  0009     unsigned byte   ??               label
  ........
  xxxx     unsigned byte   ??               label
  The labels values are 0 to 9.



TEST SET LABEL FILE (t10k-labels-idx1-ubyte):

[offset] [type]          [value]          [description]
0000     32 bit integer  0x00000801(2049) magic number (MSB first)
0004     32 bit integer  10000            number of items
0008     unsigned byte   ??               label
0009     unsigned byte   ??               label
........
xxxx     unsigned byte   ??               label
The labels values are 0 to 9.

TEST SET IMAGE FILE (t10k-images-idx3-ubyte):

[offset] [type]          [value]          [description]
0000     32 bit integer  0x00000803(2051) magic number
0004     32 bit integer  10000            number of images
0008     32 bit integer  28               number of rows
0012     32 bit integer  28               number of columns
0016     unsigned byte   ??               pixel
0017     unsigned byte   ??               pixel
........
xxxx     unsigned byte   ??               pixel
Pixels are organized row-wise. Pixel values are 0 to 255. 0 means background
(white), 255 means foreground (black).

 */

class MNIST {
public:
  const QString path = "/home/asd/Code/qt/VSlibQt/MNIST/";

  class File {
  public:
    enum { tLabel = 2049, tImage = 2051 };
    int offset; // magic+nItems
    bool ok = false;
    QFile file;
    QDataStream ds;
    int magic, nItems, rows, cols, imgSize;
    byte *imgBuff = nullptr;
    QImage image;

  public:
    File() {}
    File(QString name) { open(name); }
    ~File() {
      close();
      if (imgBuff)
        delete[] imgBuff;
    }

    bool open(QString name) {
      file.setFileName(name);
      if (file.open(QIODevice::ReadOnly)) {
        ds.setDevice(&file);

        ds >> magic;
        ds >> nItems;

        switch (magic) {
        case tLabel:
          offset = 4 + 4;
          ok = true;
          break;
        case tImage:
          offset = 4 + 4 + 4 + 4;
          ok = true;
          ds >> rows;
          ds >> cols;
          imgSize = rows * cols;
          imgBuff = new byte[imgSize];
          image = QImage(cols, rows, QImage::Format_ARGB32);
        }
      }
      return ok;
    }

    void close() {
      if (ok && file.isOpen())
        file.close();
    }

    byte getByte(int item) {
      byte rd = (byte)0;

      if (magic == tLabel) {
        file.seek(offset + item);
        ds >> rd;
      }
      return rd;
    }

    QImage getImage(int item) {
      if (magic == tImage) {
        file.seek(offset + item * rows * cols);
        file.read((char *)imgBuff, imgSize);

        for (int i = 0; i < imgSize; i++) {
          int x = i % rows, y = i / cols;
          image.setPixel(x, y,
                         imgBuff[i] < (byte)127 ? 0xffffffff : 0xff000000);
        }
      }
      return image;
    }
    byte *readImage(int item) {
      if (magic == tImage) {
        file.seek(offset + item * rows * cols);
        file.read((char *)imgBuff, imgSize);
      }
      return imgBuff;
    }

    QString toString() {
      QString s;
      if (magic == tImage) {
        for (int i = 0; i < imgSize; i++)
          s += QString(imgBuff[i] > (byte)127 ? "*" : " ") +
               QString((i % rows == 0 && i != 0) ? "\n" : "");
      }
      return s;
    }

    QVector<double> toInput() {
      QVector<double> dv;
      if (magic == tImage) {
        for (int i = 0; i < imgSize; i++)
          dv << (imgBuff[i] > (byte)127 ? 1 : 0);
      }
      return dv;
    }

    Mat toInputVector(int ix) {
      readImage(ix);
      return toInputVector();
    }

    Mat toInputVector() {
      Mat dv(1, imgSize, CV_32FC1);

      if (magic == tImage)
        for (int i = 0; i < imgSize; i++)
          dv.at<float>(i) = imgBuff[i] > (byte)127 ? 1.f : 0.f;
      return dv;
    }
  };

  File tr_img, tr_lbl; // training
  File te_img, te_lbl; // test

public:
  MNIST() {
    tr_img.open(path + "train-images-idx3-ubyte");
    tr_lbl.open(path + "train-labels-idx1-ubyte");

    te_img.open(path + "t10k-images-idx3-ubyte");
    te_lbl.open(path + "t10k-labels-idx1-ubyte");
  }
  ~MNIST() {}

  // NN interface

  // train
  Mat getTrainingImages() { // get all images
    Mat imgs;
    for (int i = 0; i < tr_img.nItems; i++)
      imgs.push_back(tr_img.toInputVector(i));
    return imgs;
  }

  Mat getTrainingLabels() { //  all training labels
    Mat labels;
    for (int i = 0; i < tr_lbl.nItems; i++)
      labels.push_back(getTrainLabelVector(i));

    return labels;
  }
  Mat getTraining() {
    Mat training;

    training.push_back(getTrainingLabels());
    training.push_back(getTrainingImages());

    return training;
  }

  // test
  Mat getTestLabels() {
    Mat labels;
    for (int i = 0; i < te_lbl.nItems; i++)
      labels.push_back(getTestLabelVector(i));
    return labels;
  }
  Mat getTestImages() { // get all images
    Mat imgs;
    for (int i = 0; i < te_img.nItems; i++)
      imgs.push_back(te_img.toInputVector(i));
    return imgs;
  }

  Mat getTest() {
    Mat test;
    test.push_back(getTestLabels()); // tOutput=labels
    test.push_back(getTestImages()); // tInput=images
    return test;
  }

  Mat getTrainImageasInputVector(int ix = -1) {
    if (ix == -1)
      ix = rand() % tr_img.nItems;
    return tr_img.toInputVector(ix);
  }
  Mat getTestImageasInputVector(int ix = -1) {
    if (ix == -1)
      ix = rand() % te_img.nItems;
    return te_img.toInputVector(ix);
  }

  float getTrainLabelValue(int i) { return (float)tr_lbl.getByte(i); }
  float getTestLabelValue(int i) { return (float)te_lbl.getByte(i); }

  Mat getTrainLabelVector(int ix) {
    Mat dv(1, 10, CV_32FC1, Scalar(0));
    byte b = tr_lbl.getByte(ix);
    dv.at<float>((int)b) = (float)1; // b=2 -> y(0,0,1,0,0,...0)
    return dv;
  }
  Mat getTestLabelVector(int ix) {
    Mat dv(1, 10, CV_32FC1, Scalar(0));
    byte b = te_lbl.getByte(ix);
    dv.at<float>((int)b) = (float)1; // b=2 -> y(0,0,1,0,0,...0)
    return dv;
  }

  Mat getImage(const Mat imgs, int ni) { return Mat(imgs, Range(ni, ni + 1)); }

  void printImg(const Mat img) {
    for (int i = 0; i < 28; i++) {
      for (int j = 0; j < 28; j++)
        if (img.at<float>(i * 28 + j) == 0)
          printf(" ");
        else
          printf("·");
      printf("\n");
    }
  }

  void printImg(const Mat imgs, int ni) { printImg(getImage(imgs, ni)); }

  byte getTrainLabel(int ix) { return tr_lbl.getByte(ix); }
  QVector<double> getTrainImageasInput(int ix) {
    tr_img.getImage(ix);
    return tr_img.toInput();
  }
  QVector<double> getTrainLabelasInput(int ix) {
    QVector<double> dv(10, 0);
    byte b = tr_lbl.getByte(ix);
    dv[(int)b] = 1; // b=2 -> y(0,0,1,0,0,...0)
    return dv;
  }

  QImage getTrainImage(int ix) { return tr_img.getImage(ix); }
  int getTrainingItems() { return tr_lbl.nItems; }
  QString trainToString() { return tr_img.toString(); }

  byte getTestLabel(int ix) { return te_lbl.getByte(ix); }
  QImage getTestImage(int ix) { return te_img.getImage(ix); }
  int getTestItems() { return te_lbl.nItems; }
  QString testToString() { return te_img.toString(); }

  Mat image2Vector(QImage img) {
    Mat v(1, 784, CV_8UC1, Scalar(0));
    for (int x = 0, i = 0; x < 28; x++)
      for (int y = 0; y < 28; y++, i++)
        v.at<byte>(i) = (byte)(img.pixelColor(y, x) == Qt::black ? 1 : 0);
    return v;
  }
};

#endif // MNIST_H
