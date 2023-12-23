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
#include "third_party/json11/json11.hpp"

#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/widgets/controls.h"

#include "selfdrive/ui/qt/offroad/settings.h"


class CustomPanel : public QWidget {
  Q_OBJECT
public:
  explicit CustomPanel(SettingsWindow *parent);

protected:  
  void closeEvent(QCloseEvent *event) override;  

signals:

protected:
  virtual void showEvent(QShowEvent *event) override;


private slots:


private:
  Params params;

private:
  std::unique_ptr<PubMaster> pm; 

public:
  int send(const char *name, MessageBuilder &msg);


public:
   void  save_json_to_file(const json11::Json::object& log_j, const std::string& filename);
   json11::Json::object load_json_from_file(const std::string& filename) ;
};




class CommunityPanel : public ListWidget {
  Q_OBJECT
public:
  explicit CommunityPanel(CustomPanel *parent);


private:
  Params params;
  std::map<std::string, ParamControl*> toggles;


  json11::Json::object m_jsondata;


  void updateToggles();

protected:
  virtual void showEvent(QShowEvent *event) override;
  virtual void hideEvent(QHideEvent *event) override;


protected:  
  void closeEvent(QCloseEvent *event) override;  

private:
  CustomPanel *m_pCustom = nullptr;;
  int  m_cmdIdx = 0;
};

