#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  SphericalHarmonics sh;
  for (int i = 0; i < sh.getNCodes(); i++)
    ui->codes->addItem(QString::fromStdString(sh.getCode(i)));
}

MainWindow::~MainWindow() { delete ui; }
