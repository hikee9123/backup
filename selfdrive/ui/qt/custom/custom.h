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

#include "selfdrive/ui/qt/offroad/settings.h"


class CustomPanel : public QWidget {
  Q_OBJECT
public:
  explicit CustomPanel(SettingsWindow *parent);
  void     setContentsMargins(int left, int top, int right, int bottom);

signals:


protected:
  void create(WId window = 0, bool initializeWindow = true, bool destroyOldWindow = true);
  void destroy(bool destroyWindow = true, bool destroySubWindows = true);

  virtual void showEvent(QShowEvent *event);

private slots:


private:
  Params params;

 private:

};




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

