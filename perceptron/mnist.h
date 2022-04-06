#ifndef MNIST_H
#define MNIST_H

#include <eigen.h>
#include <QDataStream>
#include <QFile>
#include <QImage>
#include <QString>
#include <QVector>

enum { labels, images };

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
typedef uchar byte;

class MNIST {
 public:
  const QString path = "/Users/asd/Documents/development/qt/VSlibQt/MNIST/";

  class File {
   public:
    enum { tLabel = 2049, tImage = 2051 };
    int offset;  // magic+nItems
    bool ok = false;
    QFile file;
    QDataStream ds;
    int magic, nItems, rows, cols, imgSize;
    byte* imgBuff = nullptr;
    QImage image;

   public:
    File() {}
    File(QString name) { open(name); }
    ~File() {
      close();
      if (imgBuff) delete[] imgBuff;
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
      if (ok && file.isOpen()) file.close();
    }

    byte getByte(int item) {
      byte rd = 0;

      if (magic == tLabel) {
        file.seek(offset + item);
        ds >> rd;
      }
      return rd;
    }

    QImage getImage(int item) {
      if (magic == tImage) {
        file.seek(offset + item * rows * cols);
        file.read((char*)imgBuff, imgSize);

        for (int i = 0; i < imgSize; i++) {
          int x = i % rows, y = i / cols;
          image.setPixel(x, y, imgBuff[i] < 127 ? 0xffffffff : 0xff000000);
        }
      }
      return image;
    }
    byte* readImage(int item) {
      if (magic == tImage) {
        file.seek(offset + item * rows * cols);
        file.read((char*)imgBuff, imgSize);
      }
      return imgBuff;
    }

    QString toString() {
      QString s;
      if (magic == tImage) {
        for (int i = 0; i < imgSize; i++)
          s += QString(imgBuff[i] > 127 ? "*" : " ") +
               QString((i % rows == 0 && i != 0) ? "\n" : "");
      }
      return s;
    }

    QVector<double> toInput() {
      QVector<double> dv;
      if (magic == tImage) {
        for (int i = 0; i < imgSize; i++) dv << (imgBuff[i] > 127 ? 1 : 0);
      }
      return dv;
    }

    vec toInputVector(int ix) {
      readImage(ix);
      return toInputVector();
    }

    vec toInputVector() {
      vec dv(imgSize);
      if (magic == tImage)
        for (int i = 0; i < imgSize; i++) dv[i] = (imgBuff[i] > 127 ? 1 : 0);
      return dv;
    }
  };

  File tr_img, tr_lbl;  // training
  File te_img, te_lbl;  // test

 public:
  MNIST() {
    tr_img.open(path + "train-images-idx3-ubyte");
    tr_lbl.open(path + "train-labels-idx1-ubyte");

    te_img.open(path + "t10k-images-idx3-ubyte");
    te_lbl.open(path + "t10k-labels-idx1-ubyte");
  }
  ~MNIST() {}

  // NN interface
  mdVector getTrainingImages() {  // get all images
    const int n = tr_img.nItems;
    mdVector imgs(n);
    for (int i = 0; i < n; i++) imgs[i] = tr_img.toInputVector(i);
    return imgs;
  }
  mdVector getTrainingLabels() {  //  all training labels
    const int n = tr_lbl.nItems;
    mdVector labels(n);
    for (int i = 0; i < n; i++) labels[i] = getTrainLabelVector(i);
    return labels;
  }
  mmdVector getTraining() {
    mmdVector training(2);

    training[labels] = getTrainingLabels();
    training[images] = getTrainingImages();
    return training;
  }
  mdVector getTestLabels() {
    const int n = te_lbl.nItems;
    mdVector labels(n);
    for (int i = 0; i < n; i++) labels[i] = getTestLabelVector(i);
    return labels;
  }
  mdVector getTestImages() {  // get all images
    const int n = te_img.nItems;
    mdVector imgs(n);
    for (int i = 0; i < n; i++) imgs[i] = te_img.toInputVector(i);
    return imgs;
  }

  mmdVector getTest() {
    mmdVector test(2);
    test[0] = getTestLabels();  // 0=labels
    test[1] = getTestImages();  // 1=images
    return test;
  }

  vec getTrainImageasInputVector(int ix = -1) {
    if (ix == -1) ix = qrand() % tr_img.nItems;
    return tr_img.toInputVector(ix);
  }
  vec getTestImageasInputVector(int ix = -1) {
    if (ix == -1) ix = qrand() % te_img.nItems;
    return te_img.toInputVector(ix);
  }
  vec getTrainLabelVector(int ix) {
    vec dv = vec::Zero(10);
    byte b = tr_lbl.getByte(ix);
    dv[b] = 1;  // b=2 -> y(0,0,1,0,0,...0)
    return dv;
  }
  vec getTestLabelVector(int ix) {
    vec dv = vec::Zero(10);
    byte b = te_lbl.getByte(ix);
    dv[b] = 1;  // b=2 -> y(0,0,1,0,0,...0)
    return dv;
  }

  byte getTrainLabel(int ix) { return tr_lbl.getByte(ix); }
  QVector<double> getTrainImageasInput(int ix) {
    tr_img.getImage(ix);
    return tr_img.toInput();
  }
  QVector<double> getTrainLabelasInput(int ix) {
    QVector<double> dv(10, 0);
    byte b = tr_lbl.getByte(ix);
    dv[b] = 1;  // b=2 -> y(0,0,1,0,0,...0)
    return dv;
  }

  QImage getTrainImage(int ix) { return tr_img.getImage(ix); }
  int getTrainingItems() { return tr_lbl.nItems; }
  QString trainToString() { return tr_img.toString(); }

  byte getTestLabel(int ix) { return te_lbl.getByte(ix); }
  QImage getTestImage(int ix) { return te_img.getImage(ix); }
  int getTestItems() { return te_lbl.nItems; }
  QString testToString() { return te_img.toString(); }

  vec image2Vector(QImage img) {
    vec v(784);
    for (int x = 0, i = 0; x < 28; x++)
      for (int y = 0; y < 28; y++, i++)
        v[i] = img.pixelColor(y,x) == Qt::black ? 1 : 0;
    return v;
  }
};

#endif  // MNIST_H
