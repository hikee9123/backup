#pragma once

#include <memory>

#include <QFrame>
#include <QMap>

#include "selfdrive/ui/ui.h"




class CSidebar  {
public:
  explicit CSidebar(QFrame* parent = 0);




public:
  void paintEvent(QPainter &p);
  void updateState(const UIState &s);


private:
  void   configFont(QPainter &p, const QString &family, int size, const QString &style);


private:
  QPixmap beterrry1_img, beterrry2_img;
  QString beterryValtage;

  const QRect battery_rc = QRect(30, 170, 260, 36);
};
