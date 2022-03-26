#include "mainwindow.h"
#include "Timer.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPicture>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  QLocale curLocale(QLocale("en_US"));
  QLocale::setDefault(curLocale);
  std::setlocale(LC_ALL, "en_GB.utf8");

  ui->setupUi(this);

  ui->mandelbrot->setStatusBar(ui->statusbar);
  ui->mandelbrot->setBookMark(ui->bookmarkTable);
  ui->mandelbrot->setRootWidget(this);
  ui->mandelbrot->setExpResol(ui->exportResol);

  ui->mandelbrot->readBookMark();

  ui->mandelbrot->setFocus();

  loadSettings();
}

MainWindow::~MainWindow() {
  ui->mandelbrot->writeBookMark();
  saveSettings();
  delete ui;
}

// settings

void MainWindow::loadSettings() {
  settings = new QSettings("voicesync", "Mandelbrot");

  restoreState(settings->value("windowState").toByteArray());
  restoreGeometry(settings->value("geometry").toByteArray());
}

void MainWindow::saveSettings() {
  settings->setValue("geometry", saveGeometry());
  settings->setValue("windowState", saveState());

  delete settings;
}

// events
void MainWindow::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Space:
    ui->mandelbrot->resetView();
    break;

  case Qt::Key_PageUp:
    ui->mandelbrot->incIters(+20);
    break;
  case Qt::Key_PageDown:
    ui->mandelbrot->incIters(-20);
    break;

  case Qt::Key_Plus:
    ui->mandelbrot->addbk();
    break;
  case Qt::Key_Minus:
    ui->mandelbrot->delbk();
    break;

  case Qt::Key_Escape:
    close();
  }
}

// slots
void MainWindow::horzScr(int action) {
  switch (action) {
  case QAbstractSlider::SliderSingleStepAdd:
    ui->mandelbrot->horzScr(+10);
    break;
  case QAbstractSlider::SliderSingleStepSub:
    ui->mandelbrot->horzScr(-10);
    break;
  }

  ui->hsb->setValue(50);
}
void MainWindow::vertScr(int action) {
  switch (action) {
  case QAbstractSlider::SliderSingleStepAdd:
    ui->mandelbrot->vertScr(+10);
    break;
  case QAbstractSlider::SliderSingleStepSub:
    ui->mandelbrot->vertScr(-10);
    break;
  }
  ui->vsb->setValue(50);
}
