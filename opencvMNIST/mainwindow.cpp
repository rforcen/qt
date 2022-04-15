#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <thread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(this, &MainWindow::message, this, &MainWindow::shMessage);
  connect(ui->digit, &MousePainter::digitFinished, this,
          &MainWindow::onDigitFinished);

  nn.loadModel();
}

MainWindow::~MainWindow() { delete ui; }

// slots

void MainWindow::shMessage(QString s) { ui->statusbar->showMessage(s); }

void MainWindow::onDigitFinished(QImage &img) {
  Mat mdigit = QImageMono2Mat(img);
  //  int predict = nn.check(mdigit);
  int predict;
  double confidence;
  nn.eval(mdigit, predict, confidence);
  shMessage(
      QString("Prediction:%1, confidence:%2").arg(predict).arg(confidence));
}

void MainWindow::on_ckeck_clicked() {

  shMessage("evaluating NN, please wait...");

  std::thread([this]() {
    nn.testConfidence();
    emit message(QString("accuracy: %1%").arg(nn.accuracy * 100));
  }).detach();
}
