#include "selfdrive/ui/qt/offroad/settings.h"

#include <cassert>
#include <cmath>
#include <string>
#include <tuple>
#include <vector>

#include <QDebug>
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


CustomPanel::CustomPanel(SettingsWindow *parent) : QWidget(parent) {
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



void CustomPanel::showEvent(QShowEvent *event)
{
  QWidget::setContentsMargins(0,0,0,0);
  printf("CustomPanel::showEvent \n" );  
  QWidget::showEvent( event );
}


////////////////////////////////////////////////////////////////////////////////////////////
//
//

CommunityPanel::CommunityPanel(CustomPanel *parent) : ListWidget(parent) {
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
    auto toggle = new ParamControl(param, title, desc, icon, this);

    bool locked = params.getBool((param + "Lock").toStdString());
    toggle->setEnabled(!locked);

    addItem(toggle);
    toggles[param.toStdString()] = toggle;
  }
}

void CommunityPanel::showEvent(QShowEvent *event) 
{
  printf("CommunityPanel::showEvent \n" );  
  updateToggles();
}


void CommunityPanel::hideEvent(QShowEvent *event)
{
  printf("CommunityPanel::hideEvent \n" );
  
  auto HapticFeedbackWhenSpeedCamera = toggles["HapticFeedbackWhenSpeedCamera"];
  auto UseExternalNaviRoutes = toggles["UseExternalNaviRoutes"];
  auto ShowDebugMessage = toggles["ShowDebugMessage"];

  MessageBuilder msg;

  auto community = msg.initEvent().initUICustom().initCommunity();
  
  community.setHapticFeedbackWhenSpeedCamera( HapticFeedbackWhenSpeedCamera  );
  community.setUseExternalNaviRoutes( UseExternalNaviRoutes );
  community.setShowDebugMessage( ShowDebugMessage );  // Float32;
  community.setCmdIdx( m_cmdIdx );

  m_cmdIdx++;
  pm.send("uICustom", msg);
}

void CommunityPanel::closeEvent(QCloseEvent *event) 
{
    printf("CommunityPanel::closeEvent \n" );
      // closeEvent 처리 코드
      // 사용자 정의 작업을 수행할 수 있습니다.

      // 예를 들어, 닫기를 취소하려면 event->ignore()를 호출할 수 있습니다.
      // event->ignore();

      // 부모 클래스의 closeEvent()를 호출하여 원래의 동작을 유지합니다.
      QWidget::closeEvent(event);
}

void CommunityPanel::updateToggles()
{

}



/*
typedef struct LiveNaviDataResult {
      float speedLimit;  // Float32;
      float speedLimitDistance;  // Float32;
      float remainTime;  // Float32;
      float roadCurvature;    // Float32;

      int   safetySign1;    // Camera
      int   safetySign2;    // Road

      int   turnInfo;    // Int32;
      int   distanceToTurn;    // Int32;      
      bool  mapValid;    // bool;
      int   mapEnable;    // bool;
      int   mapType;

      double  dArrivalDistance;    // unit:  M
      double  dArrivalTimeSec;    // unit: sec
      double  dEventSec;
      double  dHideTimeSec;

      long  tv_sec;
} LiveNaviDataResult;


      PubMaster pm({"liveNaviData"});

      MessageBuilder msg;

     LiveNaviDataResult  event;
     event.mapValid = 1;
     event.roadCurvature = m_message;

      auto framed = msg.initEvent().initLiveNaviData();
     
      framed.setId(log_msg.id());

      framed.setMapType( event.mapType  );
      framed.setTs( event.tv_sec );
      framed.setSpeedLimit( event.speedLimit );  // Float32;
      framed.setSpeedLimitDistance( event.speedLimitDistance );  // raw_target_speed_map_dist Float32;

      framed.setSafetySign1( event.safetySign1 ); // map_sign Float32;
      framed.setSafetySign2( event.safetySign2 ); 

      framed.setRoadCurvature( event.roadCurvature ); // road_curvature Float32;
      framed.setRemainTime( event.remainTime ); // road_curvature Float32;

      // Turn Info
      framed.setTurnInfo( event.turnInfo );
      framed.setDistanceToTurn( event.distanceToTurn );

      framed.setMapEnable( event.mapEnable );
      framed.setMapValid( event.mapValid );
      framed.setTrafficType( traffic_type );

      framed.setArrivalSec(  event.dArrivalTimeSec );
      framed.setArrivalDistance(  event.dArrivalDistance );

      pm.send("liveNaviData", msg);
*/