#pragma once
#include <string>
#include <vector>

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QObject>

#include "common/params.h"
#include "selfdrive/ui/qt/widgets/controls.h"


// widget to toggle params
class JsonControl : public ToggleControl {
  Q_OBJECT

public:
  JsonControl(const QString &param, const QString &title, const QString &desc, const QString &icon, QWidget *parent, QJsonObject &jsonobj)
    : ToggleControl(title, desc, icon, false, parent), m_jsonobj(jsonobj)
    {
    key = param;//.toStdString();
    QObject::connect(this, &JsonControl::toggleFlipped, [=](bool state) {
      QString content("<body><h2 style=\"text-align: center;\">" + title + "</h2><br>"
                      "<p style=\"text-align: center; margin: 0 128px; font-size: 50px;\">" + getDescription() + "</p></body>");
      ConfirmationDialog dialog(content, "Enable", "Cancel", true, this);

      bool confirmed = store_confirm;
      if (!confirm || confirmed || !state || dialog.exec()) {
        //QJsonValue value = QJsonValue::fromVariant(state);
        m_jsonobj.insert(key, state);
        setIcon(state);
      } else {
        toggle.togglePosition();
      }
    });
  }

  void setConfirmation(bool _confirm, bool _store_confirm) {
    confirm = _confirm;
    store_confirm = _store_confirm;
  }

  void setActiveIcon(const QString &icon) {
    active_icon_pixmap = QPixmap(icon).scaledToWidth(80, Qt::SmoothTransformation);
  }

  void refresh() {
    
    if (m_jsonobj.contains(key)) {
      bool state =  m_jsonobj[key].toBool();
      if (state != toggle.on) {
        toggle.togglePosition();
        setIcon(state);
      }
    }
    
  }

  void showEvent(QShowEvent *event) override {
    refresh();
  }

private:
  void setIcon(bool state) {
    if (state && !active_icon_pixmap.isNull()) {
      icon_label->setPixmap(active_icon_pixmap);
    } else if (!icon_pixmap.isNull()) {
      icon_label->setPixmap(icon_pixmap);
    }
  }

  QString key;
  QJsonObject &m_jsonobj;
  QPixmap active_icon_pixmap;
  bool confirm = false;
  bool store_confirm = false;
};