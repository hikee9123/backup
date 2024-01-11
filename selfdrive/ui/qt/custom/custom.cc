#include "selfdrive/ui/qt/offroad/settings.h"

#include <cassert>
#include <cmath>
#include <string>
#include <tuple>
#include <vector>


#include <QTabWidget>
#include <QObject>
#include <QJsonArray>


#include "common/params.h"
#include "common/watchdog.h"
#include "common/util.h"



#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/custom/custom.h"




CValueControl::CValueControl(const QString& param, const QString& title, const QString& desc, const QString& icon, int min, int max, int unit, QJsonObject &jsonobj  ) 
              : AbstractControl(title, desc, icon) , m_jsonobj(jsonobj)
{
    key = param;
    m_min = min;
    m_max = max;
    m_unit = unit;

    label.setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    label.setStyleSheet("color: #e0e879");
    hlayout->addWidget(&label);

    int state = min;
    if ( !m_jsonobj.contains(key) ) 
    {
      m_jsonobj.insert(key, state);
    }
    else
    {
      state  = m_jsonobj[key]; 
    }


    m_value = state;
    btnminus.setStyleSheet(R"(
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");
    btnplus.setStyleSheet(R"(
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    )");

    btnminus.setFixedSize(150, 100);
    btnplus.setFixedSize(150, 100);
    hlayout->addWidget(&btnminus);
    hlayout->addWidget(&btnplus);

    QObject::connect(&btnminus, &QPushButton::released, [=]() 
    {
        //auto str = m_jsonobj[key].toString(); 
        int value = m_value;// str.toInt();
        value = value - m_unit;
        if (value < m_min) {
            value = m_min;
        }
        else {
        }
        m_jsonobj[key] = value;
        m_value = value;
        refresh();
    });

    QObject::connect(&btnplus, &QPushButton::released, [=]() 
    {
        //auto str = m_jsonobj[key].toString();
        int value = m_value;// str.toInt();
        value = value + m_unit;
        if (value > m_max) {
            value = m_max;
        }
        else {
        }
        m_jsonobj[key] = value;
        m_value = value;
        refresh();
    });
    refresh();
}

void CValueControl::refresh()
{
    //auto str = m_jsonobj[key].toString();
    label.sprintf( "%d",m_value );// setText( str );
    btnminus.setText("－");
    btnplus.setText("＋");

    //m_value = str.toInt();
}


int  CValueControl::getValue()
{
  int  ret_code = m_value;

  return  ret_code;
}

void CValueControl::setValue( int value )
{
  m_jsonobj[key] = value;

  refresh();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//


CustomPanel::CustomPanel(SettingsWindow *parent) : QWidget(parent) 
{
  pm.reset( new PubMaster({"uICustom"}) );
  sm.reset( new SubMaster({"carStateCustom"}) );

  m_jsonobj = readJsonFile( "CustomParam" );

    QList<QPair<QString, QWidget *>> panels = {
        {tr("UI"), new UITab(this, m_jsonobj)},      
        {tr("Community"), new CommunityTab(this, m_jsonobj)},
        {tr("Tuning"), new QWidget(this)},
        {tr("Navigation"), new NavigationTab(this, m_jsonobj)},
        {tr("Debug"), new Debug(this,m_jsonobj)},
    };


    // 탭 위젯
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(R"(
        QTabBar::tab {
            border: 1px solid #C4C4C3;
            border-bottom-color: #C2C7CB; /* 위쪽 선 색상 */
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            min-width: 45ex; /* 탭의 최소 너비 */
            padding: 2px; /* 탭의 내부 여백 */
            margin-right: 1px; /* 탭 사이의 간격 조절 */
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #FAFAFA, stop: 0.4 #F4F4F4,
                                        stop: 0.5 #EDEDED, stop: 1.0 #FAFAFA);
            color: black; /* 글씨 색상 */
        }

        QTabBar::tab:selected {
            border-bottom-color: #B1B1B0; /* 선택된 탭의 위쪽 선 색상 */
            background: white; /* 선택된 탭의 배경 색상 */
            color: black; /* 선택된 탭의 글씨 색상 */
        }

        QTabBar::tab:!selected {
            margin-top: 2px; /* 선택되지 않은 탭의 위치 조절 */
            background: black; /* 선택되지 않은 탭의 배경 색상 */
            color: white; /* 선택되지 않은 탭의 글씨 색상 */
        }
    )");
    for (auto &[name, panel] : panels) {
      panel->setContentsMargins(50, 25, 50, 25);
      ScrollView *panel_frame = new ScrollView(panel, this);
      tabWidget->addTab(panel_frame, name);
    }

    // 탭 위젯을 전체 화면으로 표시
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);


    QObject::connect(uiState(), &UIState::offroadTransition, this, &CustomPanel::offroadTransition);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CustomPanel::OnTimer);
    timer->start(1000);    
}

void CustomPanel::offroadTransition( bool offroad  )
{
   updateToggles( false );
}

void CustomPanel::OnTimer() 
{
  UIState *s = uiState();
  UIScene &scene = s->scene;

  sm->update(0);
  if( scene.started )
  {
    updateToggles( false );
    if( m_cmdIdx > 10 )
      timer->stop();
  }
}


void CustomPanel::updateToggles( int bSave )
{
  MessageBuilder msg;

  m_cmdIdx++;
  auto custom = msg.initEvent().initUICustom();  
  auto debug = custom.initDebug();

  int idx1 = m_jsonobj["debug1"].toBool();
  int idx2 = m_jsonobj["debug2"].toBool();
  int idx3 = m_jsonobj["debug3"].toBool();
  int idx4 = m_jsonobj["debug4"].toBool();
  int idx5 = m_jsonobj["debug5"].toBool();

  debug.setCmdIdx( m_cmdIdx );    
  debug.setIdx1( idx1 );
  debug.setIdx2( idx2);
  debug.setIdx3( idx3 );
  debug.setIdx4( idx4 );
  debug.setIdx5( idx5 );


  auto comunity = custom.initCommunity();
  int cruiseMode = m_jsonobj["CruiseMode"].toInt();
  comunity.setCmdIdx( m_cmdIdx );
  comunity.setCruiseMode( cruiseMode );


  auto ui = custom.initUserInterface();
  int bDebug = m_jsonobj["ShowDebugMessage"].toBool();
  int tpms = m_jsonobj["tpms"].toBool();
  int kegman = m_jsonobj["kegman"].toBool();
  int ndebug = m_jsonobj["debug"].toBool();

  ui.setCmdIdx( m_cmdIdx );  
  ui.setShowDebugMessage( bDebug );
  ui.setTpms( tpms );
  ui.setKegman( kegman );
  ui.setDebug( ndebug );

  send("uICustom", msg);
}



void CustomPanel::closeEvent(QCloseEvent *event)
{
  timer->stop();
  delete timer;
  timer = nullptr;

  QWidget::closeEvent( event );
}

void CustomPanel::showEvent(QShowEvent *event)
{
  QWidget::setContentsMargins(0,0,0,0);
  QWidget::showEvent( event );

  int  nCarCnt = m_cars.size();
  if( nCarCnt > 0 ) return;

  sm->update(0);
  auto carState_custom = (*sm)["carStateCustom"].getCarStateCustom();
  auto carSupport = carState_custom.getSupportedCars();
  int  nCnt = carSupport.size();

  printf("SupportedCars = suport = %d  carcnt = %d \n", nCnt, nCarCnt );
  if( nCnt <= 0 )
  {
      QJsonArray surportCar = m_jsonobj["SurportCars"].toArray();
      for (const auto& item : surportCar) {
            m_cars.append(item.toString());
      }      
  }
  else
  {  
    for (int i = 0; i<nCnt; i++) {
      QString car = QString::fromStdString( carSupport[i] );
      m_cars.append( car );
      //printf("%s \n", car.toStdString().c_str());
    }
  }
}

void CustomPanel::hideEvent(QHideEvent *event)
{
  QWidget::hideEvent(event);

  updateToggles( false );

  writeJson();
}

int CustomPanel::send(const char *name, MessageBuilder &msg)
{
   return pm->send( name, msg );
}

void CustomPanel::writeJson()
{
   writeJsonToFile( m_jsonobj, "CustomParam" );
}



QJsonObject CustomPanel::readJsonFile(const QString& filePath ) 
{
    QJsonObject jsonObject;


    QString json_str = QString::fromStdString(params.get(filePath.toStdString()));

    if (json_str.isEmpty()) return jsonObject;

    QJsonDocument doc = QJsonDocument::fromJson(json_str.toUtf8());
    if (doc.isNull()) {
        printf( "Failed to parse the JSON document: %s  ", filePath.toStdString().c_str() );
        return jsonObject;  // Return an empty object in case of failure
    }  
    jsonObject = doc.object();
    return jsonObject;
}

void CustomPanel::writeJsonToFile(const QJsonObject& jsonObject, const QString& fileName) 
{
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();  
    params.put( fileName.toStdString(), jsonData.toStdString() );
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//

CommunityTab::CommunityTab(CustomPanel *parent, QJsonObject &jsonobj) : ListWidget(parent) , m_jsonobj(jsonobj)
{
  m_pCustom = parent;


  // param, title, desc, icon, min, max, unit
  std::vector<std::tuple<QString, QString, QString, QString, int, int, int>> value_defs{
    {
      "CruiseMode",
      tr("Cruise mode"),
      "0:Not used,1:Auto Speed control",
      "../assets/offroad/icon_shell.png",
      0,15,1
    },    
  };

  for (auto &[param, title, desc, icon, min,max,unit] : value_defs) {
    auto value =  new CValueControl( param, title, desc, icon, min, max, unit, m_jsonobj);
    addItem(value);
    m_valueCtrl[ param.toStdString() ] = value;
  }



  QString selected_car = QString::fromStdString(Params().get("SelectedCar"));
  auto changeCar = new ButtonControl(selected_car.length() ? selected_car : tr("Select your car"),
                    selected_car.length() ? tr("CHANGE") : tr("SELECT"), "");

  QObject::connect( changeCar, &ButtonControl::clicked, [=]() {
    QStringList items = m_pCustom->m_cars;


      QJsonArray jsonArray;
      foreach (const QString &item, items) {
        jsonArray.append(item);
      }
      m_jsonobj["SurportCars"] = jsonArray;

    QString selection = MultiOptionDialog::getSelection(tr("Select a car"), items, selected_car, this);
    if ( !selection.isEmpty() ) 
    {
      Params().put("SelectedCar", selection.toStdString());
    }
  });
  addItem(changeCar);


  setStyleSheet(R"(
    * {
      color: white;
      outline: none;
      font-family: Inter;
    }
    Updater {
      color: white;
      background-color: black;
    }
  )");  
}

void CommunityTab::showEvent(QShowEvent *event) 
{
    QWidget::showEvent(event);
}


void CommunityTab::hideEvent(QHideEvent *event)
{
  QWidget::hideEvent(event);
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//



NavigationTab::NavigationTab(CustomPanel *parent, QJsonObject &jsonobj) : ListWidget(parent), m_jsonobj(jsonobj)
{
  m_pCustom = parent;

  // param, title, desc, icon
  std::vector<std::tuple<QString, QString, QString, QString>> toggle_defs{
    {
      "UseExternalNaviRoutes",
      tr("Use external navi routes"),
      "",
      "../assets/offroad/icon_openpilot.png",
    },
  };

  for (auto &[param, title, desc, icon] : toggle_defs) {
    auto toggle = new ParamControl(param, title, desc, icon, this);

    bool locked = params.getBool((param + "Lock").toStdString());
    toggle->setEnabled(!locked);

    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }


   addItem( new MapboxToken() );

}


////////////////////////////////////////////////////////////////////////////////////////////
//
//

UITab::UITab(CustomPanel *parent, QJsonObject &jsonobj) : ListWidget(parent), m_jsonobj(jsonobj)
{
  m_pCustom = parent;


  // param, title, desc, icon
  std::vector<std::tuple<QString, QString, QString, QString>> toggle_defs{
    {
      "ShowDebugMessage",
      "Show Debug Message",
      "",
      "../assets/offroad/icon_shell.png",
    },
    {
      "tpms",
      "Show tpms",
      "",
      "../assets/offroad/icon_shell.png",
    },
    {
      "kegman",
      "Show kegman",
      "",
      "../assets/offroad/icon_shell.png",
    },
    {
      "debug",
      "Show debug",
      "",
      "../assets/offroad/icon_shell.png",
    },
  };

  for (auto &[param, title, desc, icon] : toggle_defs) {
    auto toggle = new JsonControl(param, title, desc, icon, this, m_jsonobj);

    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }

  QObject::connect(uiState(), &UIState::offroadTransition, this, &UITab::offroadTransition);
  connect(toggles["ShowDebugMessage"], &ToggleControl::toggleFlipped, [=]() {
    updateToggles( false );
  });    
}

void UITab::offroadTransition( bool offroad  )
{
  if( !offroad )
  {
    updateToggles( false );
  }
}


void UITab::closeEvent(QCloseEvent *event) 
{
    QWidget::closeEvent(event);
}

void UITab::showEvent(QShowEvent *event) 
{
    QWidget::showEvent(event);
}



void UITab::hideEvent(QHideEvent *event)
{
  QWidget::hideEvent(event);

  updateToggles( true );
}


void UITab::updateToggles( int bSave )
{
  if( bSave )
  {
    m_pCustom->writeJson();
  }

  int bDebug = m_jsonobj["ShowDebugMessage"].toBool();
  auto tpms_mode_toggle = toggles["tpms"];
  auto kegman_mode_toggle = toggles["kegman"];
  auto debug_mode_toggle = toggles["debug"];

  tpms_mode_toggle->setEnabled(bDebug);
  kegman_mode_toggle->setEnabled(bDebug);
  debug_mode_toggle->setEnabled(bDebug);
}



////////////////////////////////////////////////////////////////////////////////////////////
//
//

Debug::Debug(CustomPanel *parent, QJsonObject &jsonobj) : ListWidget(parent), m_jsonobj(jsonobj)
{
  m_pCustom = parent;


  // param, title, desc, icon
  std::vector<std::tuple<QString, QString, QString, QString>> toggle_defs{
    {
      "debug1",
      tr("debug1"),
      "",
      "../assets/offroad/icon_shell.png",
    },
    {
      "debug2",
      tr("debug2"),
      "",
      "../assets/offroad/icon_shell.png",
    },
    {
      "debug3",
      tr("debug3"),
      "",
      "../assets/offroad/icon_shell.png",
    },
    {
      "debug4",
      tr("debug4"),
      "",
      "../assets/offroad/icon_shell.png",
    },
    {
      "debug5",
      tr("debug5"),
      "",
      "../assets/offroad/icon_shell.png",
    },
  };

  for (auto &[param, title, desc, icon] : toggle_defs) {
    auto toggle = new JsonControl(param, title, desc, icon, this, m_jsonobj);

    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }

  QObject::connect(uiState(), &UIState::offroadTransition, this, &Debug::offroadTransition);  
}

void Debug::offroadTransition( bool offroad  )
{
  if( !offroad )
  {
    updateToggles( false );
  }
}


void Debug::closeEvent(QCloseEvent *event) 
{
    QWidget::closeEvent(event);
}

void Debug::showEvent(QShowEvent *event) 
{
    QWidget::showEvent(event);
}

void Debug::hideEvent(QHideEvent *event)
{
  QWidget::hideEvent(event);

  updateToggles( true );
}


void Debug::updateToggles( int bSave )
{
  if( bSave )
  {
    m_pCustom->writeJson();
  }

}
