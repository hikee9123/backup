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

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "selfdrive/ui/qt/widgets/controls.h"

#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
#include "selfdrive/ui/qt/offroad/settings.h"

#include "selfdrive/ui/ui.h"



class JsonControl : public ToggleControl {
  Q_OBJECT

public:
  JsonControl(const QString &param, const QString &title, const QString &desc, const QString &icon, QWidget *parent, QJsonObject &jsonobj) 
    : ToggleControl(title, desc, icon, false, parent),m_jsonobj(jsonobj) {
    key = param;
    QObject::connect(this, &JsonControl::toggleFlipped, [=](bool state) {
      QString content("<body><h2 style=\"text-align: center;\">" + title + "</h2><br>"
                      "<p style=\"text-align: center; margin: 0 128px; font-size: 50px;\">" + getDescription() + "</p></body>");
      ConfirmationDialog dialog(content, "Enable", "Cancel", true, this);

      bool confirmed = store_confirm;
      if (!confirm || confirmed || !state || dialog.exec()) {
        m_jsonobj.insert(key, state);
      } else {
        toggle.togglePosition();
      }
    });
  }

  void setConfirmation(bool _confirm, bool _store_confirm) {
    confirm = _confirm;
    store_confirm = _store_confirm;
  }



  void refresh() {
    if (m_jsonobj.contains(key)) {
      bool state =  m_jsonobj[key].toBool();
      if (state != toggle.on) {
        toggle.togglePosition();
      }
    }
    
  }

  void showEvent(QShowEvent *event) override {
    refresh();
  }

private:
  QString key;
  QJsonObject &m_jsonobj;
  bool confirm = false;
  bool store_confirm = false;
};


// new CValueControl("EnableAutoEngage", "EnableAutoEngage", "0:Not used,1:Auto Engage/Cruise OFF,2:Auto Engage/Cruise ON", "../assets/offroad/icon_shell.png", 0, 2, 1);
class CValueControl : public AbstractControl {
    Q_OBJECT

public:
    CValueControl(const QString& param, const QString& title, const QString& desc, const QString& icon, int min, int max, int unit, QJsonObject &jsonobj );

private:
    QPushButton btnplus;
    QPushButton btnminus;
    QLabel label;

    int     m_min;
    int     m_max;
    int     m_unit;

    int     m_value;


    QJsonObject &m_jsonobj;  


private:
  void refresh();

public:
    QString key;


 public:
    void setValue( int value );
    int  getValue();   
};



class MapboxToken : public AbstractControl {
  Q_OBJECT

public:
  MapboxToken() : AbstractControl("MapboxToken", "Put your MapboxToken", "")
  {
    btn.setStyleSheet(R"(
      padding: -10;
      border-radius: 35px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    edit.setStyleSheet(R"(
      background-color: grey;
      font-size: 55px;
      font-weight: 500;
      height: 120px;
    )");
    btn.setFixedSize(200, 100);
    hlayout->addWidget(&edit);
    hlayout->addWidget(&btn);

    QObject::connect(&btn, &QPushButton::clicked, [=]() {
      QString targetvalue = InputDialog::getText("MapboxToken", this, "Put your MapboxToken starting with pk.", false, 1, QString::fromStdString(params.get("MapboxToken")));
      if (targetvalue.length() > 0 && targetvalue != QString::fromStdString(params.get("MapboxToken"))) {
        params.put("MapboxToken", targetvalue.toStdString());
        refresh();
      }
    });
    refresh();   
  }

private:
  QPushButton btn;
  QLineEdit edit;
  Params params;

  void refresh()
  {
    auto strs = QString::fromStdString(params.get("MapboxToken"));
    edit.setText(QString::fromStdString(strs.toStdString()));
    btn.setText("SET");   
  }
};



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
  std::map<std::string, CValueControl*> m_valueCtrl;


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
