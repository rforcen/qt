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
