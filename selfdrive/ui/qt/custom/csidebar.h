#pragma once

#include <memory>

#include <QFrame>
#include <QMap>

#include "selfdrive/ui/ui.h"




class CSidebar  {
  Q_OBJECT


public:
  explicit CSidebar(Sidebar* parent = 0);




public:
  void paintEvent(QPaintEvent *event) override;
  void updateState(const UIState &s);


private:
  QString beterryValtage;
};
