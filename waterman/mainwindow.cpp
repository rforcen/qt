#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_radius_valueChanged(int value) {
  ui->waterman->setRadius(value);
  setWindowTitle(QString("Waterman Polyhedron (%1) #faces:%2, #vertex:%3")
                     .arg(ui->radius->value())
                     .arg(ui->waterman->getnFaces())
                     .arg(ui->waterman->getnVertex()));
}
