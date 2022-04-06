#ifndef TABLEWAVE_H
#define TABLEWAVE_H

#include <d.h>
#include <QAbstractTableModel>
#include <QTableView>

class WaveTableModel : public QAbstractTableModel {
 public:
  WaveTableModel(QTableView *parent) : QAbstractTableModel(parent) {}

  int rowCount(const QModelIndex &) const { return d.nWaves; }
  int columnCount(const QModelIndex &) const { return d.nCols; }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
    switch (role) {
      case Qt::EditRole:
      case Qt::DisplayRole: {
        auto xp = d.getData(index);
        return (xp != 0) ? QVariant(QString("%1").arg(xp, 7, 'f', 1))
                         : QVariant();
      }

      case Qt::TextAlignmentRole:
        return Qt::AlignRight;
      case Qt::FontRole:
        return QFont("Courier New",12,QFont::Bold);
      case Qt::BackgroundColorRole:
        return MusicFreq::Freq2Color(d.getData(D::wFreq, index.row()));
      case Qt::ForegroundRole:
        return d.brightness(MusicFreq::Freq2Color(
                   d.getData(D::wFreq, index.row()))) > 0.6
                   ? QColor(Qt::black)
                   : QColor(Qt::white);
    }
    return QVariant();
  }
  bool setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
      bool ok;
      double val = value.toDouble(&ok);
      if (ok) d.setData(index, val);
    }
    return true;
  }

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const {
    switch (orientation) {
      case Qt::Horizontal:
        if (role == Qt::DisplayRole) return d.header[section];
        break;
      case Qt::Vertical:
        if (role == Qt::DisplayRole) return QString::number(section + 1);
        break;
    }
    return QVariant();
  }

  Qt::ItemFlags flags(const QModelIndex &index) const {
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  }
};

class TableWave : public QTableView {
  Q_OBJECT

 private:
  WaveTableModel *model = nullptr;

 public slots:
  void onDataChanged() { init(); }

 public:
  TableWave(QWidget *parent = nullptr) : QTableView(parent) {
    connect(&d, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
  }

  void init() {
    if (model) delete model;
    model = new WaveTableModel(this);
    setModel(model);
  }
};

#endif  // TABLEWAVE_H
