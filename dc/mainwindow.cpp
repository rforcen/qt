#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  for (auto &ps : presets)
    ui->cbPreset->addItem(QString::fromStdString(ps));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::selectPreset(int ps) { ui->dc->setPreset(ps); }

void MainWindow::setExpression(QString exp) { ui->dc->setExpression(exp); }

void MainWindow::recMessage(QString msg) { ui->statusbar->showMessage(msg); }
