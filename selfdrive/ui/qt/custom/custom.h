#pragma once

#include <map>
#include <string>

#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>


#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/widgets/controls.h"



class CommunityPanel : public ListWidget {
  Q_OBJECT
public:
  explicit CommunityPanel(CustomPanel *parent);
  void showEvent(QShowEvent *event) override;

private:
  Params params;
  std::map<std::string, ParamControl*> toggles;

  void updateToggles();
};


class CustomPanel : public ListWidget {
  Q_OBJECT
public:
  explicit CustomPanel(SettingsWindow *parent);

signals:


private slots:


private:
  Params params;

 private:
  QButtonGroup    *nav_btns;
  QStackedWidget  *panel_widget;
};