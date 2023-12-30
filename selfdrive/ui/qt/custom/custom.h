#pragma once

#include <map>
#include <string>

#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QWidget>
#include <QTimer>


#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

#include "cereal/messaging/messaging.h"
#include "third_party/json11/json11.hpp"

#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/widgets/controls.h"

#include "selfdrive/ui/qt/offroad/settings.h"
#include "selfdrive/ui/qt/custom/jsonContrl.h"

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
   QJsonObject readJsonFile(const QString& fileName);
   void     writeJsonToFile(const QJsonObject& jsonObject, const QString& fileName);

};




class CommunityPanel : public ListWidget {
  Q_OBJECT
public:
  explicit CommunityPanel(CustomPanel *parent);


private:
  Params params;
  std::map<std::string, JsonControl*> toggles;


  QJsonObject m_jsondata;


  void updateToggles( int bSave );

protected:
  virtual void showEvent(QShowEvent *event) override;
  virtual void hideEvent(QHideEvent *event) override;


protected:  
  void closeEvent(QCloseEvent *event) override;  

private slots:
    void OnTimer();

private:
    QTimer *timer = nullptr;


private:
  CustomPanel *m_pCustom = nullptr;;
  int  m_cmdIdx = 0;
};

