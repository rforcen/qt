#ifndef TABLECHORDS_H
#define TABLECHORDS_H

#include <d.h>
#include <QAbstractTableModel>
#include <QTableView>

class ChordTableModel : public QAbstractTableModel {
 public:
  ChordTableModel(QTableView *parent) : QAbstractTableModel(parent) {}

  int rowCount(const QModelIndex &) const { return d.getChordsRows(); }
  int columnCount(const QModelIndex &) const { return d.nColsChords; }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
    auto ix = d.index2item(index);
    switch (role) {
      case Qt::DisplayRole:
        return d.chordTable.getChordName(ix);

      case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
      case Qt::FontRole:
        return QFont("Courier New", 9);
      case Qt::BackgroundRole:
        return QVariant(d.chordTable.getColor(ix));
      case Qt::ForegroundRole:
        return QVariant(QColor(Qt::white));
    }
    return QVariant();
  }

  QVariant headerData(int, Qt::Orientation, int) const { return QVariant(); }
};

class TableChords : public QTableView {
 private:
  ChordTableModel *model = nullptr;

 public:
  TableChords(QWidget *parent = nullptr) : QTableView(parent) {
    connect(this, &QTableView::clicked,
            [](QModelIndex index) { d.chordSelected(index); });
    init();
  }

  void init() {
    if (model) delete model;
    model = new ChordTableModel(this);
    setModel(model);
  }
};

#endif  // TABLECHORDS_H
