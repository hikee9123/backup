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


CustomPanel::CustomPanel(SettingsWindow *parent) : ListWidget(parent) {
    setSpacing(50);

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

    //setWindowTitle("Tab Example");
    //setGeometry(100, 100, 1024, 900);

    // 탭 위젯
    QTabWidget *tabWidget = new QTabWidget(this);
    for (auto &[name, panel] : panels) {
         QVBoxLayout *layout1 = new QVBoxLayout(panel);
        layout1->addWidget(new QPushButton("Button 1-1" , panel));
        layout1->addWidget(new QPushButton("Button 1-2", panel));
        tabWidget->addTab(panel, name);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    mainLayout->setGeometry(0, 0, 1024, 900);
    setLayout(mainLayout);
    show();


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



    /*
    stackedWidget = new QStackedWidget();
    // Create two pages
    // 1.
    CommunityPanel *page1 = new CommunityPanel(this); // QWidget();
    page1->setStyleSheet("background-color: lightblue;");
    QPushButton *button1 = new QPushButton("Community");
    QObject::connect(button1, &QPushButton::clicked, [stackedWidget]() {
        stackedWidget->setCurrentIndex(1);
    });
    QVBoxLayout *layout1 = new QVBoxLayout(page1);
    layout1->addWidget(button1);

    // 2.
    QWidget *page2 = new QWidget();
    page2->setStyleSheet("background-color: lightgreen;");
    QPushButton *button2 = new QPushButton("Go to Page 1");
    QObject::connect(button2, &QPushButton::clicked, [stackedWidget]() {
        stackedWidget->setCurrentIndex(0);
    });
    QVBoxLayout *layout2 = new QVBoxLayout(page2);
    layout2->addWidget(button2);

    // Add pages to the stacked widget
    stackedWidget->addWidget(page1);
    stackedWidget->addWidget(page2);

    // Set the current page
    stackedWidget->setCurrentIndex(0);

    // Show the stacked widget
    stackedWidget->show();
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
