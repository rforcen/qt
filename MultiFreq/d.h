#pragma once

#include <QApplication>
#include <QClipboard>
#include <QColor>
#include <QModelIndex>

#include <QTime>
#include <QVector>

#include <ChordTable.h>
#include <MusicFreq.h>
#include <WaveGen.h>
#include <audioout.h>

class D : public QObject {  // data depot
  Q_OBJECT

 public:
  static const int nWaves = 32;
  static const int nCols = 3;  // amp, hz, phase
  static const int sampleRate = 44100, nChannels = 2, bitsSample = 16;
  static const int MaxFreq = 22000;
  int nSec = 10;

  typedef enum { wAmp, wFreq, wPhase } WaveComponent;
  typedef QVector<double> DoubleVect;
  typedef QVector<DoubleVect> Xprop;

  DoubleVect zeroVect = DoubleVect(nWaves);

  Xprop xProp = {zeroVect, zeroVect,
                 zeroVect};  // 0..1 for each parameter(amp,freq,phase)
  double data[nCols][nWaves];

  QColor brush[nCols] = {Qt::red, Qt::yellow, Qt::blue};
  double range[nCols] = {100, MaxFreq, 360};
  const QStringList header = {"amp", "pitch(hz)", "phase"},
                    typeName = {"amp", "freq", "phase"};

  // audio
  AudioOut *audioOut = nullptr;
  TWave *wave = nullptr;
  static const int waveBufferSize = 4 * 1024;
  short waveBuffer[waveBufferSize];

  // chord table
  ChordTable chordTable;
  const int nColsChords = 6;
  int chordBaseOct = -1;
  double chordBaseAmp = 0.5;
  int index2item(QModelIndex index) {
    return nColsChords * index.row() + index.column();
  }
  int getChordsRows() { return chordTable.getnChords() / nColsChords; }
  void chordSelected(QModelIndex index) {
    auto notev = chordTable.noteNumber[index2item(index)];  // qvectror of notes
    auto nn = notev.size();
    for (int i = 0; i < nWaves; i++) {
      xProp[wAmp][i] = chordBaseAmp + rand01() / 2.;
      xProp[wFreq][i] = to01range(toProp(
          wFreq,
          MusicFreq::NoteOct2Freq(notev[i % nn] % 12, chordBaseOct + i / nn)));
      xProp[wPhase][i] = rand01();
    }
    update();
  }
  double to01range(double x) {
    while (x > 1) x /= 2.;
    return x;
  }

  // color mgr
  QColor interpolate(QColor c1, QColor c2, float frac) {
    if (frac > 1) frac = 1;
    auto r = (c2.redF() - c1.redF()) * frac + c1.redF(),
         g = (c2.greenF() - c1.greenF()) * frac + c1.greenF(),
         b = (c2.blueF() - c1.blueF()) * frac + c1.blueF();

    return QColor(r * 255., g * 255., b * 255.);
  }

  float brightness(QColor col) {
    return col.redF() * 0.299 + col.greenF() * 0.587 + col.blueF() * 0.114;
  }


 signals:
  void dataChanged();

 public:
  D(QObject *parent = nullptr) : QObject(parent) {
    wave = new TWave(nChannels, sampleRate);
    audioOut = new AudioOut(sampleRate, nChannels, bitsSample, this);

    srand(QTime::currentTime().msec());

    // audio support prep, gen
    connect(audioOut, &AudioOut::prepareWave, [=]() { waveSet(); });
    connect(audioOut, &AudioOut::dataRequest, [=](char *data, qint64 maxlen) {
      wave->gen((short *)data, maxlen / 2);
    });
  }

  ~D() {
    delete audioOut;
    delete wave;
  }

  WaveComponent findType(QString t) {
    return (WaveComponent)typeName.indexOf(t);
  }

  void update() {
    emit dataChanged();
    emit audioOut->prepareWave();
  }

  void waveSet() {
    auto amp = getData(wAmp), freq = getData(wFreq), phase = getData(wPhase);
    int inz = 0;
    for (int i = 0; i < nWaves; i++) {
      if (amp[i] != 0 && freq[i] != 0) {
        if (i != inz) {
          amp[inz] = amp[i];
          freq[inz] = freq[i];
          phase[inz] = phase[i];
        }
        inz++;
      }
    }
    wave->Set(amp, freq, phase, inz);
  }

  void waveGet() { wave->gen(waveBuffer, waveBufferSize); }

  void play() { audioOut->switchPlay(); }

  double rand01() { return (double)rand() / RAND_MAX; }

  void random(int nw = nWaves, int minFreq = 0, int maxFreq = MaxFreq) {
    clear(false);  // don't update

    for (int t = 0; t < nCols; t++)
      for (int i = 0; i < nw; i++) {
        auto &x = xProp[t][i] = rand01();  // 0..1
        if (t == wFreq) x = toProp(wFreq, x * (maxFreq - minFreq) + minFreq);
      }

    update();
  }

  double prop2freq(double x, double r) {  // 0..1 -> 0..MaxFreq(hz)
    return (x < 0.5) ? 2000. * x : 2000. * 0.5 + (x - 0.5) * r * 2;
  }

  double getData(int col, int row) {
    auto x = xProp[col][row], r = range[col];
    auto xr = x * r;
    return (col == wFreq) ? prop2freq(x, r) : xr;
  }
  double getData(int type, double x) {
    auto r = range[type];
    auto xr = x * r;
    return (type == wFreq) ? prop2freq(x, r) : xr;
  }

  double *getData(int type) {
    auto d = data[type];
    memcpy(d, xProp[type].data(), nWaves * sizeof(double));
    for (int i = 0; i < nWaves; i++) d[i] = toSoundCompatible(type, d[i]);
    return d;
  }

  void setData(QModelIndex index, double value) {
    int col = index.column();

    if (value > range[col]) value = range[col];
    if (value < 0) value = 0;

    xProp[col][index.row()] = toProp(col, value);

    update();
  }
  void setData(int type, int row, double value) {
    xProp[type][row] = value;

    update();
  }

  double toSoundCompatible(
      int type, double x) {  // convert x to 16 bit amp freq in hz phase in rad
    double rv = 0;
    switch (type) {
      case wFreq:
        rv = getData(type, x);
        break;
      case wPhase:
        rv = getData(type, x) * M_PI / 180.;
        break;
      case wAmp:
        rv = 32000. * x;
        break;
    }
    return rv;
  }

  int nNonZero() {
    int rv = 0;
    for (int i = 0; i < nWaves; i++)
      if (xProp[wAmp][i] != 0 && xProp[wFreq][i] != 0) rv++;
    return rv != 0 ? rv : 1;
  }

  double waveEval(double t) {
    double y = 0;
    int nz = nNonZero();

    for (int i = 0; i < nWaves; i++)
      y += xProp[wAmp][i] * sin(getData(wFreq, i) * t +
                                toSoundCompatible(wPhase, xProp[wPhase][i]));
    return y /= nz;
  }

  double toProp(int type, double x) {  // convert x to 0..1 range
    double rv = 0;
    switch (type) {
      case wFreq:
        rv = (x < 1000) ? x / 2000.
                        : (x - 2000. * 0.5) / (range[wFreq] * 2) + 0.5;
        break;
      case wPhase:
      case wAmp:
        return rv = x / range[type];
        break;
    }
    return rv;
  }

  double getData(QModelIndex index) {
    return getData(index.column(), index.row());
  }

  void clear(bool upd = true) {
    for (int i = 0; i < nCols; i++) xProp[i] = zeroVect;
    if (upd) update();
  }

  void copy2Clip() {
    QString s;
    for (int i = 0; i < nWaves; i++)

      s += QString("%1\t%2\t%3\n")
               .arg(getData(wAmp, i))
               .arg(getData(wFreq, i))
               .arg(getData(wPhase, i));
    QApplication::clipboard()->setText(s);
  }

  void pasteFromClip() {
    QString s = QApplication::clipboard()->text();

    if (!s.isEmpty()) {
      clear();

      auto lines = s.split(QRegExp("[\n\r]"), QString::SkipEmptyParts);
      int row = 0;
      for (auto l : lines)
        if (row < nWaves) {
          auto sahp = l.split('\t', QString::SkipEmptyParts);
          if (sahp.size() == nCols) {
            bool ok1, ok2, ok3;
            double wv[nCols] = {sahp[0].toDouble(&ok1), sahp[1].toDouble(&ok2),
                                sahp[2].toDouble(&ok3)};
            if (ok1 && ok2 && ok3) {
              for (int col = 0; col < nCols; col++)
                xProp[col][row] = toProp(col, wv[col]);
              row++;
            }
          }
        }
      update();
    }
  }
};

extern D d;
Q_DECLARE_METATYPE(D::Xprop)  // settings support
