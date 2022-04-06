#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  centralWidget()->hide();

  d.clear();
  loadSettings();

  // populate tool bar
  {
    auto tb = ui->mainToolBar;
    tb->insertWidget(ui->actionexport, ui->secs);
    tb->insertWidget(ui->actionrandom, ui->maxRndWaves);
    tb->insertWidget(ui->actionrandom, ui->minFreq);
    tb->insertWidget(ui->actionrandom, ui->maxFreq);

    ui->maxRndWaves->setMaximum(d.nWaves);  // spin max values
    ui->maxFreq->setMaximum(d.MaxFreq);
    ui->maxFreq->setMaximum(d.MaxFreq);
  }
}

MainWindow::~MainWindow() {
  stopPlaying();

  saveSettings();
  delete ui;
}

void MainWindow::loadSettings() {
  qRegisterMetaTypeStreamOperators<D::Xprop>("D::Xprop");

  settings = new QSettings("voicesync", "multifreq");

  restoreState(settings->value("windowState").toByteArray());
  restoreGeometry(settings->value("geometry").toByteArray());

  fileWAV = settings->value("fileWAV").toString();
  fileWDF = settings->value("fileWDF").toString();

  for (auto sb : findChildren<QSpinBox *>()) {  // restore all spinboxes
    bool ok;
    auto v = settings->value(sb->objectName()).toInt(&ok);
    if (ok) sb->setValue(v);
  }

  auto xp = settings->value("xProp").value<D::Xprop>();  // load wave params
  if (!xp.isEmpty()) d.xProp = xp;
}
void MainWindow::saveSettings() {
  settings->setValue("geometry", saveGeometry());
  settings->setValue("windowState", saveState());

  settings->setValue("fileWAV", fileWAV);
  settings->setValue("fileWDF", fileWDF);

  for (auto sb : findChildren<QSpinBox *>())  // save all spinboxes
    settings->setValue(sb->objectName(), sb->value());

  settings->setValue("xProp",
                     QVariant::fromValue(d.xProp));  // save all wave params

  delete settings;
}

void MainWindow::mess(QString s) { statusBar()->showMessage(s); }

void MainWindow::stopPlaying() {
  if (ui->actionplay->isChecked()) {
    d.audioOut->stop();
    ui->actionplay->setChecked(false);
  }
}

void MainWindow::on_actionabout_triggered() {
  QMessageBox::about(this, "", MainWindow::file2QString(":/about.html"));
}
void MainWindow::on_actioncopy_triggered() { d.copy2Clip(); }
void MainWindow::on_actionpaste_triggered() { d.pasteFromClip(); }
void MainWindow::on_actionplay_triggered() { d.play(); }
void MainWindow::on_actionclear_triggered() { d.clear(); }
void MainWindow::on_actionrandom_triggered() {
  d.random(ui->maxRndWaves->value(), ui->minFreq->value(),
           ui->maxFreq->value());
}
void MainWindow::on_actionexport_triggered() {
  stopPlaying();

  fileWAV = QFileDialog::getSaveFileName(
      this, tr("Write WAV file"), prevFile(fileWAV), tr("WAV File (*.wav)"));
  if (!fileWAV.isEmpty()) {
    d.waveSet();
    WaveFile wf;  // do all open, write, close
    if (wf.OpenForWrite(s2cp(fileWAV), d.sampleRate, d.bitsSample,
                        d.nChannels) != DDC_SUCCESS) {
      mess("Can't create " + fileWAV);
      return;
    }
    for (int i = 0; i < 2 * ui->secs->value() * d.sampleRate / d.waveBufferSize;
         i++) {
      d.waveGet();
      if (wf.WriteData(d.waveBuffer, d.waveBufferSize) != DDC_SUCCESS)
        mess("Can't write data to file " + fileWAV);
    }
    if (wf.Close() != DDC_SUCCESS)
      mess("Can't close file " + fileWAV);
    else
      mess("WAV file " + fileWAV + " created");
  }
}
void MainWindow::on_actionsave_wave_triggered() {
  stopPlaying();
  fileWDF =
      QFileDialog::getSaveFileName(this, tr("Write Wave Definition File"),
                                   prevFile(fileWDF), tr("WDF File (*.wdf)"));
  if (!fileWDF.isEmpty()) {
    QFile wdf(fileWDF);
    if (wdf.open(QIODevice::WriteOnly)) {
      QDataStream swdf(&wdf);
      swdf << d.xProp;
      wdf.close();
    }
  }
}
void MainWindow::on_actionopen_wave_triggered() {
  stopPlaying();
  fileWDF =
      QFileDialog::getOpenFileName(this, tr("Open Wave Definition File"),
                                   prevFile(fileWDF), tr("WDF File (*.wdf)"));
  if (!fileWDF.isEmpty()) {
    QFile wdf(fileWDF);
    if (wdf.open(QIODevice::ReadOnly)) {
      QDataStream swdf(&wdf);
      swdf >> d.xProp;
      wdf.close();

      d.update();
    }
  }
}
