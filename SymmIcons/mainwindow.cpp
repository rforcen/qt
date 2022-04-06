#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);
  ui->centralwidget->hide();

  // fill table w/presets
  for (auto i = 0; i < ui->si->si.getnPreset(); i++) {
    ui->presetTab->insertRow(ui->presetTab->rowCount());

    for (int col = 0; col < ui->presetTab->columnCount(); col++)
      ui->presetTab->setItem(
          ui->presetTab->rowCount() - 1, col,
          new QTableWidgetItem(QString("%1").arg(ui->si->si.getParam(i, col))));
  }

  // progress message
  connect(ui->si, &QSymmetricIcon::Progress, this,
          [this](QString msg) { ui->statusbar->showMessage(msg); });

  loadSettings();
}

MainWindow::~MainWindow() {
  saveSettings();
  delete ui;
}

// slots
void MainWindow::cellClicked(int row, int) {
  int cc = ui->presetTab->columnCount();
  double params[cc]; // collect params
  for (int col = 0; col < cc; col++)
    params[col] =
        ui->presetTab->item(row, col)->data(Qt::DisplayRole).toDouble();

  ui->si->setParams(params);
}

// settings

void MainWindow::loadSettings() {
  settings = new QSettings("voicesync", "SymmIcon");

  restoreState(settings->value("windowState").toByteArray());
  restoreGeometry(settings->value("geometry").toByteArray());
}

void MainWindow::saveSettings() {
  settings->setValue("geometry", saveGeometry());
  settings->setValue("windowState", saveState());

  delete settings;
}
