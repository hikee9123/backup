#include "selfdrive/ui/qt/custom/csidebar.h"

#include <QMouseEvent>

#include "selfdrive/ui/qt/util.h"

CSidebar::CSidebar(Sidebar *parent)  
{

}

void CSidebar::updateState(const UIState &s) 
{
  auto &sm = *(s.sm);

  float  fBatteryVoltage = 0.;
  auto pandaStates = sm["pandaStates"].getPandaStates();
  if (pandaStates.size() > 0) {
    fBatteryVoltage = pandaStates[0].getVoltage() * 0.01;
  }
  auto deviceState = sm["deviceState"].getDeviceState();
  float carBatteryCapacityWh = (int)deviceState.getCarBatteryCapacityUwh() * 0.0000001;

  beterryValtage.sprintf("%.1fV %.0fW/h", fBatteryVoltage, carBatteryCapacityWh);
}

void CSidebar::paintEvent(QPaintEvent *event) 
{
  p.setFont(InterFont(35));
  p.setPen(QColor(0xff, 0xff, 0xff));
  const QRect r = QRect(50, 267, 100, 50);
  p.drawText(r, Qt::AlignCenter, beterryValtage);    
}
