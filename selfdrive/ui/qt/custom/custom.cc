#include "selfdrive/ui/qt/offroad/settings.h"

#include <cassert>
#include <cmath>
#include <string>
#include <tuple>
#include <vector>


#include <QTabWidget>



#include "common/params.h"
#include "common/watchdog.h"
#include "common/util.h"
#include "system/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/scrollview.h"
#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/util.h"
#include "selfdrive/ui/qt/qt_window.h"

#include "selfdrive/ui/qt/custom/custom.h"



CustomPanel::CustomPanel(SettingsWindow *parent) : QWidget(parent) 
{
  pm.reset(new PubMaster({"uICustom"}));

/*
    QObject::connect(uiState(), &UIState::offroadTransition, [=](bool offroad) {
        for (auto btn : findChildren<ButtonControl *>()) {
        btn->setEnabled(offroad);
        }
    });
*/
    QList<QPair<QString, QWidget *>> panels = {
        {tr("Community"), new CommunityPanel(this)},
        {tr("UI"), new QWidget(this)},
        {tr("Tuning"), new QWidget(this)},
        {tr("Navigation"), new QWidget(this)},
        {tr("Debug"), new QWidget(this)},
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
}


void CustomPanel::closeEvent(QCloseEvent *event)
{
  printf("CustomPanel::closeEvent \n" );  
  QWidget::closeEvent( event );
}

void CustomPanel::showEvent(QShowEvent *event)
{
  QWidget::setContentsMargins(0,0,0,0);
  printf("CustomPanel::showEvent \n" );  
  QWidget::showEvent( event );
}


int CustomPanel::send(const char *name, MessageBuilder &msg)
{
   return pm->send( name, msg );
}


QJsonObject CustomPanel::readJsonFile(const QString& filePath ) 
{
    QJsonObject jsonObject;


    QString json_str = QString::fromStdString(params.get(filePath.toStdString()));
    printf( "JSON file: %s  \n", json_str.toStdString().c_str() );

    if (json_str.isEmpty()) return jsonObject;

    QJsonDocument doc = QJsonDocument::fromJson(json_str.toUtf8());
    if (doc.isNull()) {
        printf( "Failed to parse the JSON document: %s  ", filePath.toStdString().c_str() );
        return jsonObject;  // Return an empty object in case of failure
    }  
    jsonObject = doc.object();
    return jsonObject;


  /*
    QString filePath = "/data/params/d/" + fileName;

    // JSON 파일 열기
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        printf( "Failed to open the JSON file: %s  \n", filePath.toStdString().c_str() );
        return jsonObject;  // Return an empty object in case of failure
    }

    // JSON 파일 내용 읽기
    QByteArray jsonData = file.readAll();
    file.close();

    // JSON 파싱
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        printf( "Failed to parse the JSON document: %s  ", filePath.toStdString().c_str() );
        return jsonObject;  // Return an empty object in case of failure
    }

    // JSON 객체 얻기
    jsonObject = doc.object();
    return jsonObject;
  */    
}

void CustomPanel::writeJsonToFile(const QJsonObject& jsonObject, const QString& fileName) 
{
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();  
    params.put( fileName.toStdString(), jsonData.toStdString() );

    //params.put( fileName.toStdString(), jsonDoc.toJson().toStdString());

  /*
    QString filePath = "/data/params/d/" + fileName; 
    // JSON 파일 열기
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        printf( "Failed to open the JSON file for writing: %s  ", filePath.toStdString().c_str() );
        return;
    }
  
    // JSON 객체를 문자열로 변환
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson();

    // 파일에 JSON 데이터 쓰기
    qint64 bytesWritten = file.write(jsonData);
    file.close();

    if (bytesWritten == -1) {
        printf( "Failed to write JSON data to the file: %s  ", filePath.toStdString().c_str() );
    } else {
        printf( "JSON data successfully written to the file: %s  ", filePath.toStdString().c_str() );
    }
  */
}

////////////////////////////////////////////////////////////////////////////////////////////
//
//

CommunityPanel::CommunityPanel(CustomPanel *parent) : ListWidget(parent) 
{
  m_pCustom = parent;

/*
  QString selected_car = QString::fromStdString(Params().get("SelectedCar"));
  auto changeCar = new ButtonControl(selected_car.length() ? selected_car : tr("Select your car"),
                    selected_car.length() ? tr("CHANGE") : tr("SELECT"), "");

  connect(changeCar, &ButtonControl::clicked, [=]() {
    QStringList items = get_list("/data/params/d/SupportedCars");
    QString selection = MultiOptionDialog::getSelection(tr("Select a car"), items, selected_car, this);
    if (!selection.isEmpty()) {
      Params().put("SelectedCar", selection.toStdString());
      qApp->exit(18);
      watchdog_kick(0);
    }
  });
  addItem(changeCar);
*/

  m_jsondata = m_pCustom->readJsonFile( "CustomCommunity" );

  auto str3 = QString::fromStdString( params.get( "ShowDebugMessage" ) );
  int ShowDebugMessage = str3.toInt();

  int levelnum = m_jsondata["ShowDebugMessage"].toInt();
  printf("ShowDebugMessage = %d  %d \n", levelnum, ShowDebugMessage );


  // param, title, desc, icon
  std::vector<std::tuple<QString, QString, QString, QString>> toggle_defs{
    {
      "HapticFeedbackWhenSpeedCamera",
      tr("Haptic feedback (speed-cam alert)"),
      tr("Haptic feedback when a speed camera is detected"),
      "../assets/offroad/icon_openpilot.png",
    },
    {
      "UseExternalNaviRoutes",
      tr("Use external navi routes"),
      "",
      "../assets/offroad/icon_openpilot.png",
    },
    {
      "ShowDebugMessage",
      tr("Show Debug Message"),
      "",
      "../assets/offroad/icon_shell.png",
    },
  };

  for (auto &[param, title, desc, icon] : toggle_defs) {
    auto toggle = new JsonControl(param, title, desc, icon, this, m_jsondata);
    //auto toggle = new ParamControl(param, title, desc, icon, this );


    //bool locked = m_jsondata[param].toBool();// params.getBool((param + "Lock").toStdString());
    //toggle->setEnabled(!locked);

    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }


  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &CommunityPanel::OnTimer);
  timer->start(2000);
}


void CommunityPanel::OnTimer() 
{
  UIState *s = uiState();
  UIScene &scene = s->scene;

  updateToggles( false );
  if( scene.started )
  {
    timer->stop();
  }
}

void CommunityPanel::closeEvent(QCloseEvent *event) 
{
    printf("CommunityPanel::closeEvent \n" );

    timer->stop();
    delete timer;
    timer = nullptr;
      // closeEvent 처리 코드
      // 사용자 정의 작업을 수행할 수 있습니다.

      // 예를 들어, 닫기를 취소하려면 event->ignore()를 호출할 수 있습니다.
      // event->ignore();

      // 부모 클래스의 closeEvent()를 호출하여 원래의 동작을 유지합니다.
      QWidget::closeEvent(event);
}

void CommunityPanel::showEvent(QShowEvent *event) 
{
    printf("CommunityPanel::showEvent \n" );  
    QWidget::showEvent(event);
}




void CommunityPanel::hideEvent(QHideEvent *event)
{
  printf("CommunityPanel::hideEvent \n" );
  QWidget::hideEvent(event);

  updateToggles( true );
}


void CommunityPanel::updateToggles( int bSave )
{
  if( bSave )
  {
    m_pCustom->writeJsonToFile(  m_jsondata, "CustomCommunity" );   
  }

  int HapticFeedbackWhenSpeedCamera = m_jsondata["HapticFeedbackWhenSpeedCamera"].toBool();
  int UseExternalNaviRoutes = m_jsondata["UseExternalNaviRoutes" ].toBool();
  int ShowDebugMessage = m_jsondata["ShowDebugMessage" ].toBool();

  printf("HapticFeedbackWhenSpeedCamera =%d \n", HapticFeedbackWhenSpeedCamera);
  printf("UseExternalNaviRoutes =%d \n", UseExternalNaviRoutes);
  printf("ShowDebugMessage =%d \n", ShowDebugMessage);


  m_cmdIdx++;
  MessageBuilder msg;
  auto community = msg.initEvent().initUICustom().initCommunity();
  community.setHapticFeedbackWhenSpeedCamera( HapticFeedbackWhenSpeedCamera  );
  community.setUseExternalNaviRoutes( UseExternalNaviRoutes );
  community.setShowDebugMessage( ShowDebugMessage );  // Float32;
  community.setCmdIdx( m_cmdIdx );
  m_pCustom->send("uICustom", msg);
  //printf("uiCustom  send(%d)  = ShowDebugMessage %d", ret, ShowDebugMessage);
}
