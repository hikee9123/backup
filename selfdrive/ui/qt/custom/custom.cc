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
        {tr("Tuning"), new QWidget(this)},
        {tr("UI"), new QWidget(this)},
        //{tr("Debug"), new QWidget(this)},
        //{tr("Navigation"), new QWidget(this)},
    };


    // 탭 위젯
    QTabWidget *tabWidget = new QTabWidget(this);
    // 탭 버튼에 대한 스타일시트 설정
    tabWidget->setStyleSheet(R"(
        QTabBar::tab {
            border: 2px solid #C4C4C3;
            border-bottom-color: #C2C7CB; /* 위쪽 선 색상 */
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            min-width: 50ex; /* 탭의 최소 너비 */
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
        tabWidget->addTab(panel, name);
    }

    // 탭 위젯을 전체 화면으로 표시
   // tabWidget->showMaximized();    


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);

    //tabWidget->move(100, 50);
   // resize(1024, 900);
   // setLayout(mainLayout);
    //show();


/*
    setStyleSheet(R"(
        * {
        color: white;
        font-size: 50px;
        }
        SettingsWindow {
        background-color: black;
        }
        QStackedWidget, ScrollView {
        background-color: #292929;
        border-radius: 30px;
        }
    )");
*/
}





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

void CommunityPanel::showEvent(QShowEvent *event) {
  updateToggles();
}

void CommunityPanel::updateToggles() {

}
