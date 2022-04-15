#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QImage>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPicture>

#include <nn.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void on_ckeck_clicked();

private:
  Ui::MainWindow *ui;

  NN nn;

  QImage Mat2QImage(const cv::Mat3b &src) {
    QImage dest(src.cols, src.rows, QImage::Format_ARGB32);

    for (int y = 0; y < src.rows; ++y) {
      const cv::Vec3b *srcrow = src[y];
      QRgb *destrow = (QRgb *)dest.scanLine(y);
      for (int x = 0; x < src.cols; ++x)
        destrow[x] = qRgba(srcrow[x][2], srcrow[x][1], srcrow[x][0], 255);
    }

    return dest;
  }

  Mat QImage2Mat(QImage const &src) {
    Mat tmp(src.height(), src.width(), CV_8UC3, (uchar *)src.bits(),
            src.bytesPerLine());
    Mat result; // deep copy just in case (my lack of knowledge with open cv)
    cvtColor(tmp, result, COLOR_BGR2RGB);
    return result;
  }
  Mat QImageMono2Mat(QImage const &img) {
    Mat mimg(1, img.height() * img.width(), CV_32FC1);
    for (int x = 0, i = 0; x < 28; x++)
      for (int y = 0; y < 28; y++, i++)
        mimg.at<float>(i) = img.pixelColor(y, x) == Qt::black ? 1 : 0;
    return mimg;
  }

signals:
  void message(QString);
public slots:
  void shMessage(QString s);
  void onDigitFinished(QImage &img);
};
#endif // MAINWINDOW_H
