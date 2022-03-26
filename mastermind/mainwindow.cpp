#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  selectPerm(0);
}

void MainWindow::showPerms() {
  ui->check->setText(QString("check (%1)").arg(mm.matchPerms.size()));
}

void MainWindow::selectPerm(int row) {
  if (row >= 0 and row < (int)mm.matchPerms.size()) {
    for (int r = 0; r < ui->tabProbe->rowCount(); r++) {
      if (ui->tabProbe->item(r, 0) == nullptr) {
        int c = 0;
        for (auto i : mm.matchPerms[row])
          ui->tabProbe->setItem(r, c++,
                                new QTableWidgetItem(QString::number(i)));
        break;
      }
    }
  }
  showPerms();
}

// slots

void MainWindow::clear() {
  mm.matchPerms = mm.allPerms;
  ui->tabProbe->clearContents();
  ui->statusbar->clearMessage();
  selectPerm(0);
}

void MainWindow::check() {
  // load probes
  mm.probes.clear();
  for (int r = 0; r < ui->tabProbe->rowCount(); r++) {
    if (ui->tabProbe->item(r, 0) != nullptr) {
      vector<int> v;
      for (auto c = 0; c < 4; c++)
        v.push_back(ui->tabProbe->item(r, c)->text().toInt());
      auto tb = ui->tabProbe->item(r, 4), tw = ui->tabProbe->item(r, 5);
      int b = tb ? tb->text().toInt() : 0;
      int w = tw ? tw->text().toInt() : 0;
      mm.probes.push_back({v, b, w});
    }
  }

  mm.findMatches();

  if (mm.matchPerms.size() == 1) // found solution!
    ui->statusbar->showMessage(
        QString("solution is:%1").arg(toString(mm.matchPerms[0])));

  if (mm.matchPerms.size() == 0)
    ui->statusbar->showMessage("wrong probes");
  else
    selectPerm(0);
}

MainWindow::~MainWindow() { delete ui; }
