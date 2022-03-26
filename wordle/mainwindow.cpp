#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QMessageBox>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  QObject::connect(ui->listAll, &QListView::clicked, this,
                   &MainWindow::dictSel);
  QObject::connect(ui->listMatch, &QListView::clicked, this,
                   &MainWindow::dictSel);

  loadDict();
}

void MainWindow::loadDict() { // load dict

  wordle.loadDict();

  // load dict ->listAll
  {
    QStandardItemModel *model = new QStandardItemModel();
    ui->listAll->setModel(model);
    for (auto &w : wordle.dict)
      model->appendRow(new QStandardItem(w));
  }

  ui->lDict->setText(QString("Dictionary (%1)").arg(wordle.dict.size()));
}

void MainWindow::changeLetters(int nl) {
  clear();
  wordle.changeNLetters(nl);
  loadDict();
}

void MainWindow::dictSel(const QModelIndex &index) {
  QString w = index.data().toString();
  for (int r = 0; r < ui->tab->rowCount(); r++) {
    if (ui->tab->item(r, 0) == nullptr) {
      ui->tab->setItem(r, 0, new QTableWidgetItem(w));
      break;
    }
  }
}

void MainWindow::clear() {
  ui->tab->clearContents();
  ui->listMatch->clear();
  ui->statusbar->clearMessage();
  wordle.reset();
}

void MainWindow::check(void) {
  // load fp vector from tab
  wordle.resetProbes();

  for (int r = 0; r < ui->tab->rowCount(); r++) {
    if (ui->tab->item(r, 0) == nullptr or ui->tab->item(r, 0)->text().isEmpty())
      break;
    auto w = ui->tab->item(r, 0)->text(), h = ui->tab->item(r, 1)->text();
    if (w.isEmpty() || w.size() != wordle.nletters ||
        h.size() != wordle.nletters)
      break;
    wordle.addProbe(w, h);
  }

  wordle.check(); // check -> matchWords

  ui->listMatch->clear();
  ui->listMatch->addItems(wordle.matchWords);

  ui->lMatch->setText(QString("Match (%1)").arg(wordle.matchWords.size()));
  ui->tab->setItem(
      wordle.probes.size() - 1, 2,
      new QTableWidgetItem(QString("%1").arg(wordle.matchWords.size())));

  if (wordle.matchWords.size() == 1)
    ui->statusbar->showMessage(
        QString("Solution is:%1").arg(wordle.matchWords[0]));
}

MainWindow::~MainWindow() { delete ui; }
