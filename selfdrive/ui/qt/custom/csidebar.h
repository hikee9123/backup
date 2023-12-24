#pragma once

#include <memory>

#include <QFrame>
#include <QMap>

#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/sidebar.h"



class CSidebar  {
  Q_OBJECT


public:
  explicit CSidebar(Sidebar* parent = 0);




public:
  void paintEvent(QPaintEvent *event) override;
  void updateState(const UIState &s);


private:
  QPixmap beterrry1_img, beterrry2_img;
  QString beterryValtage;

  const QRect home_btn = QRect(60, 860, 180, 180);
};
