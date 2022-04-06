#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <d.h>
#include <riff.h>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

 private slots:
  void on_actionabout_triggered();
  void on_actioncopy_triggered();
  void on_actionpaste_triggered();
  void on_actionplay_triggered();
  void on_actionclear_triggered();
  void on_actionrandom_triggered();
  void on_actionexport_triggered();
  void on_actionsave_wave_triggered();
  void on_actionopen_wave_triggered();

 private:
  Ui::MainWindow *ui;
  QSettings *settings = nullptr;

 private:
  void loadSettings(), saveSettings();

  QString file2QString(QString fileName) {
    QFile fAbout(fileName);
    fAbout.open(QIODevice::ReadOnly);
    auto aboutTxt = fAbout.readAll();
    fAbout.close();
    return aboutTxt;
  }

  QString fileWAV, fileWDF;

  QString prevFile(QString file) {
    return (file.isEmpty()) ? QDir::homePath() : file;
  }
  char *s2cp(QString s) { return s.toLatin1().data(); }  // QString 2 char*
  void mess(QString s);
  void stopPlaying();
};

#endif  // MAINWINDOW_H
