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



#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
#include "selfdrive/ui/qt/offroad/settings.h"

#include "selfdrive/ui/qt/custom/jsonContrl.h"

#include "selfdrive/ui/ui.h"



// new CValueControl("EnableAutoEngage", "EnableAutoEngage", "0:Not used,1:Auto Engage/Cruise OFF,2:Auto Engage/Cruise ON", "../assets/offroad/icon_shell.png", 0, 2, 1);

/*
class CValueControl : public AbstractControl {
    Q_OBJECT

public:
    CValueControl(const QString& params, const QString& title, const QString& desc, const QString& icon, int min, int max, int unit = 1);

private:
    QPushButton btnplus;
    QPushButton btnminus;
    QLabel label;

    QString m_params;
    int     m_min;
    int     m_max;
    int     m_unit;

    void refresh();
};
*/


class CustomPanel : public QWidget {
  Q_OBJECT
public:
  explicit CustomPanel(SettingsWindow *parent);

protected:  
  void closeEvent(QCloseEvent *event) override;  

protected:
  virtual void showEvent(QShowEvent *event) override;
  virtual void hideEvent(QHideEvent *event) override;

signals:


private slots:  // 시그널과 연결되어 특정 이벤트에 응답할 때
  void offroadTransition( bool offroad  );
  void OnTimer();  

private:
  void  updateToggles( int bSave );

public:


private:
  QTimer *timer = nullptr;
  Params params;
  int    m_cmdIdx = 0;
  QJsonObject m_jsonobj;

private:
  std::unique_ptr<PubMaster> pm; 
  std::unique_ptr<SubMaster> sm;

public:
  int send(const char *name, MessageBuilder &msg);
  QStringList m_cars;

public:
   QJsonObject readJsonFile(const QString& fileName);
   void     writeJsonToFile(const QJsonObject& jsonObject, const QString& fileName);
   void     writeJson();
};




class CommunityTab : public ListWidget {
  Q_OBJECT
public:
  explicit CommunityTab(CustomPanel *parent, QJsonObject &jsonobj);


private:
  std::map<std::string, JsonControl*> toggles;


protected:
  virtual void showEvent(QShowEvent *event) override;
  virtual void hideEvent(QHideEvent *event) override;


protected:  

signals:

private slots:



private:


private:
  CustomPanel *m_pCustom = nullptr;
  QJsonObject &m_jsonobj;  

};



class NavigationTab : public ListWidget {
  Q_OBJECT
public:
  explicit NavigationTab(CustomPanel *parent, QJsonObject &jsonobj);


private:
  std::map<std::string, ParamControl*> toggles;


protected:


protected:  

signals:

private slots:



private:
  Params params;
  CustomPanel *m_pCustom = nullptr;
  QJsonObject &m_jsonobj;  
};



class UITab : public ListWidget {
  Q_OBJECT
public:
  explicit UITab(CustomPanel *parent, QJsonObject &jsonobj);


private:
  std::map<std::string, JsonControl*> toggles;





  void updateToggles( int bSave );

protected:
  virtual void showEvent(QShowEvent *event) override;
  virtual void hideEvent(QHideEvent *event) override;


protected:  
  void closeEvent(QCloseEvent *event) override;  

private slots:
  void offroadTransition( bool offroad  );

private:



private:
  CustomPanel *m_pCustom = nullptr;
  QJsonObject &m_jsonobj;  
  int  m_cmdIdx = 0;
};



class Debug : public ListWidget {
  Q_OBJECT
public:
  explicit Debug(CustomPanel *parent, QJsonObject &jsonobj);


private:
  std::map<std::string, JsonControl*> toggles;
  QJsonObject &m_jsonobj;  



  void updateToggles( int bSave );

protected:
  virtual void showEvent(QShowEvent *event) override;
  virtual void hideEvent(QHideEvent *event) override;


protected:  
  void closeEvent(QCloseEvent *event) override;  

signals:

private slots:
  void offroadTransition( bool offroad  );

private:



private:
  CustomPanel *m_pCustom = nullptr;
  int  m_cmdIdx = 0;
};
