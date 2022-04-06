#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  testVect();

  qsrand(QTime::currentTime().msecsSinceStartOfDay());

  nn = new NN({784, 30, 10});
  //  nn->readTrainingResult();  // w,b of python training
  nn->setTest(mnist.getTest());
  connect(nn, SIGNAL(nnProgress(int, double)), this,  // progress
          SLOT(onEpoch(int, double)));

  connect(ui->draw, SIGNAL(digitFinished(QImage)), this,
          SLOT(onDigitDraw(QImage)));

  ui->itemTrain->setMaximum(mnist.getTrainingItems());
  ui->itemTest->setMaximum(mnist.getTestItems());
}

MainWindow::~MainWindow() {
  if (tLearn.joinable()) {
    nn->abort();
    tLearn.join();
  }
  delete ui;
}

void MainWindow::on_itemTrain_valueChanged(int value) {
  auto realLabel = mnist.getTrainLabel(value);
  auto recogLabel = nn->testTrainItem(value);

  QImage img = mnist.getTrainImage(value);

  ui->image->setPixmap(QPixmap::fromImage(img));
  ui->lbl->setText(
      QString("train set: value=%1/result=%2 %3")
          .arg(realLabel)
          .arg(recogLabel)
          .arg(realLabel == recogLabel ? "match" : "ERROR in recognition"));
  ui->label->setText(mnist.tr_img.toString());
}

void MainWindow::on_itemTest_valueChanged(int value) {
  auto realLabel = mnist.getTestLabel(value);
  auto recogLabel = nn->testTestItem(value);

  QImage img = mnist.getTestImage(value);

  ui->image->setPixmap(QPixmap::fromImage(img));
  ui->lbl->setText(
      QString("test set: value=%1/result=%2 %3")
          .arg(realLabel)
          .arg(recogLabel)
          .arg(realLabel == recogLabel ? "match" : "ERROR in recognition"));
  ui->label->setText(mnist.testToString());
}

void MainWindow::on_actiontest_triggered() {
  statusBar()->showMessage(
      QString("success ratio=%1 %").arg(100. * nn->testFactor()), 5000);
}

void MainWindow::onEpoch(int epoch, double hitFactor) {
  statusBar()->showMessage(QString("epoch: %1, hitFactor %2%, lap:%3\"")
                               .arg(epoch)
                               .arg(100. * hitFactor)
                               .arg(time.elapsed() / 1000., 5, 'g', 2));
  time.start();
  //  QCoreApplication::processEvents();
}

void MainWindow::onDigitDraw(QImage img) {
  auto in = mnist.image2Vector(img);
  auto digit = nn->evaluate(in);
  ui->recTxt->setText(QString("recognized: %1").arg(digit));
}

void MainWindow::doLearn() {
  auto hf =
      nn->SGD_multithread(mnist.getTraining(), nEpochs, 3.0);  // nEpochs,  eta
  onEpoch(nEpochs, hf);
}

void MainWindow::on_actionlearn_triggered() {
  time.start();

  if (!tLearn.joinable())
    tLearn = std::thread(&MainWindow::doLearn, this);
  else {
    nn->abort();
    tLearn.join();
    on_actiontest_triggered();
  }
}

void MainWindow::on_actionsave_training_triggered() {
  nn->writeTraining("training.nn");
}

void MainWindow::on_actionread_training_triggered() {
  nn->readTraining("training.nn");
}
