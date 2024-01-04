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


#include "selfdrive/ui/qt/custom/jsonContrl.h"

#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
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
  virtual void hideEvent(QHideEvent *event) override;

private slots:
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
  // std::unique_ptr<SubMaster> sm;

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

private slots:
  void offroadTransition( bool offroad  );

private:



private:
  CustomPanel *m_pCustom = nullptr;
  int  m_cmdIdx = 0;
};
