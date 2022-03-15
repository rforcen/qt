#include "qmandelbrot.h"
#include <QCoreApplication>

QMandelbrot::QMandelbrot(QWidget *parent) : QWidget{parent} {
  iters = mandel64.iters;
  dpr = devicePixelRatioF();
}

QMandelbrot::~QMandelbrot() {}

void QMandelbrot::mousePressEvent(QMouseEvent *event) {
  pStart = pEnd = event->pos();
  dragging = true;
}

// events

void QMandelbrot::mouseReleaseEvent(QMouseEvent *event) {

  addbk();

  dragging = false;

  pEnd = event->pos();

  if (pStart == pEnd) {
    const int c = 150;
    pStart -= QPoint(c, c);
    pEnd += QPoint(c, c);
  }

  auto rx = fabs((float)pStart.x() - pEnd.x()) / w,
       ry = fabs((float)pStart.y() - pEnd.y()) / h;
  auto ratio = abs(range);

  QPointF cp = pStart + (pEnd - pStart) / 2; // pstart..pend center point

  // re center fractal
  center = Complex64(center.real() + ratio * (w / 2 - cp.x()) / w,
                     center.imag() + ratio * (h / 2 - cp.y()) / h);
  range = Complex64(range.real() * rx, range.imag() * ry);

  refresh();
}

void QMandelbrot::mouseMoveEvent(QMouseEvent *event) {
  if (dragging) {
    pEnd = event->pos();
    update();
  }
}

void QMandelbrot::wheelEvent(QWheelEvent *event) {
  int wd = event->angleDelta().y();

  double scale = wd < 0 ? 1.1 : 0.9;
  double ratio = (range.imag() - range.real()) / 20;
  auto p = event->position();

  // re center fractal
  center = Complex64(center.real() + ratio * (w / 2 - p.x()) / w,
                     center.imag() + ratio * (h / 2 - p.y()) / h);

  range = complex<double>(range.real() * scale, range.imag() * scale);

  refresh();
}

void QMandelbrot::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
  case Qt::Key_Space:
    resetView();
    break;

  case Qt::Key_PageUp:
    iters = std::max(iters - 20, 200);
    refresh();
    break;
  case Qt::Key_PageDown:
    iters += 20;
    refresh();
    break;

  case Qt::Key_Plus:
    addbk();
    break;
  case Qt::Key_Minus:
    delbk();
    break;

  case Qt::Key_Escape:
    if (wnd)
      wnd->close();
  }
}

void QMandelbrot::resizeEvent(QResizeEvent *) { recalc(); }

void QMandelbrot::paintEvent(QPaintEvent *) {

  QPainter p(this);
  p.scale(1 / dpr, 1 / dpr);

  QImage img((uchar *)image.data(), wimg, himg, QImage::Format_ARGB32);

  p.drawImage(0, 0, img);

  if (dragging) {
    p.setPen(Qt::white);
    p.drawRect(QRect(pStart * dpr, pEnd * dpr));
  }
}

void QMandelbrot::recalc() {

  w = width();
  h = height();

  wimg = w * dpr;
  himg = h * dpr;

  Timer t0;

  genMandel(wimg, himg);

  if (sb)
    sb->showMessage(sformat("lap:%ldms, iters:%d, scale:%.2g size:(%d,%d)",
                            t0.lap(), iters, calcScale(), wimg, himg));
}

// slots
void QMandelbrot::SaveBk() {

  fileName = QFileDialog::getSaveFileName(this, "save bookmarks", fileName,
                                          "BMK File (*.bmk)");
  if (!fileName.isEmpty())
    writeBookMark(fileName);
}
void QMandelbrot::LoadBk() {
  fileName = QFileDialog::getOpenFileName(this, "load bookmarks", fileName,
                                          "BMK File (*.bmk)");
  if (!fileName.isEmpty()) {
    clearbk();
    readBookMark(fileName);
  }
}

void QMandelbrot::setEngine(int engine) {
  this->engine = (MandelEngine)engine;
  refresh();
}

void QMandelbrot::setIters(int iters) {
  this->iters = iters;
  refresh();
}

void QMandelbrot::resetView() {
  center = Complex64(0.5, 0);
  range = Complex64(-2, 2);
  iters = 200;
  refresh();
}

void QMandelbrot::addbk() {
  if (tb) {
    tb->insertRow(tb->rowCount());
    const char *fmt = "%'Lf";
    QVector<QString> sl = {sformat(fmt, center.real()),
                           sformat(fmt, center.imag()),
                           sformat(fmt, range.real()),
                           sformat(fmt, range.imag()), sformat("%d", iters)};
    int i = 0;
    for (auto &s : sl) {
      QTableWidgetItem *newItem = new QTableWidgetItem(s);
      if (i == 0) // only item 0 contains mandelbrot data
        newItem->setData(Qt::UserRole, getMandel());

      tb->setItem(tb->rowCount() - 1, i++, newItem);
    }
  }
}

void QMandelbrot::delbk() {
  if (tb->currentRow() >= 0) {
    delMandel(tb->item(tb->currentRow(), 0)->data(Qt::UserRole));
    tb->removeRow(tb->currentRow());
  }
}

void QMandelbrot::clearbk() {
  for (auto i = 0; i < tb->rowCount(); i++)
    delMandel(tb->item(i, 0)->data(Qt::UserRole));

  tb->clearContents();
  tb->setRowCount(0);
}

void QMandelbrot::bookMarkSel(int row, int) {
  QTableWidgetItem *ci = tb->item(row, 0);
  if (ci) {
    restoreMandel(ci->data(Qt::UserRole));
    refresh();
  }
}

void QMandelbrot::bookMarkSel(int row, int, int, int) {
  QTableWidgetItem *ci = tb->item(row, 0);
  if (ci) {
    restoreMandel(ci->data(Qt::UserRole));
    refresh();
  }
}

void QMandelbrot::Export() {

  auto fileName = QFileDialog::getSaveFileName(this, "export fractal", "",
                                               "png File (*.png)");
  if (!fileName.isEmpty()) {
    int res = expResol->value();
    genMandel(res, res);
    QImage img((uchar *)image.data(), res, res, QImage::Format_ARGB32);
    img.save(fileName, "PNG");
  }
}

//
