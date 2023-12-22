#pragma once

#include <map>
#include <string>

#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>


#include "cereal/messaging/messaging.h"

#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/widgets/controls.h"

#include "selfdrive/ui/qt/offroad/settings.h"


class CustomPanel : public QWidget {
  Q_OBJECT
public:
  explicit CustomPanel(SettingsWindow *parent);

protected:

signals:




private slots:


private:
  Params params;


 private:
 ;




class CommunityPanel : public ListWidget {
  Q_OBJECT
public:
  explicit CommunityPanel(CustomPanel *parent);
  void showEvent(QShowEvent *event) override;

private:
  Params params;
  std::map<std::string, ParamControl*> toggles;

  void updateToggles();

protected:
  virtual void showEvent(QShowEvent *event) override;
  virtual void hideEvent(QHideEvent *event) override;


protected:  
  void closeEvent(QCloseEvent *event) override;  

private:
  int  m_cmdIdx = 0;
}
};

