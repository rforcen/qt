#include "wavegraph.h"

WaveGraph::WaveGraph(QWidget *parent) : QWidget(parent) {
  connect(&d, &D::dataChanged, [=]() { update(); });
}

void WaveGraph::paintEvent(QPaintEvent *) {
  QPainter painter(this);
  QPainterPath path;

  int w = painter.device()->width(), h = painter.device()->height();

//  painter.drawRect(0, 0, w, h);  // main frame
  painter.setPen(Qt::green);

  d.waveSet();  // done w/fastsin, double eval is too slow
  int np = d.sampleRate * d.nSec, nsec = np / w;

  short max = -SHRT_MAX, min = SHRT_MAX;

  for (int x = 0; x < np; x++) {
    int xw = x % d.waveBufferSize;

    if (xw == 0) d.waveGet();

    short yw = d.waveBuffer[xw];
    max = std::max<short>(max, yw);
    min = std::min<short>(min, yw);

    if (x % nsec == 0 && x != 0) {
      int xg = x / nsec - 1;

      path.moveTo(xg, (((int)min + SHRT_MAX) * h) / (2 * (int)SHRT_MAX));
      path.lineTo(xg, (((int)max + SHRT_MAX) * h) / (2 * (int)SHRT_MAX));

      max = -SHRT_MAX, min = SHRT_MAX;
    }
  }
  painter.drawPath(path);
}
