#include "selfdrive/ui/qt/custom/csidebar.h"

#include <QMouseEvent>

#include "selfdrive/ui/qt/util.h"


void CSidebar::configFont(QPainter &p, const QString &family, int size, const QString &style) 
{
  QFont f(family);
  f.setPixelSize(size);
  f.setStyleName(style);
  p.setFont(f);
}


CSidebar::CSidebar(QFrame *parent)  
{
  beterrry1_img = loadPixmap("qt/custom/images/battery.png", battery_rc.size());
  beterrry2_img = loadPixmap("qt/custom/images/battery_charging.png", battery_rc.size());
}

void CSidebar::updateState(const UIState &s) 
{
  auto &sm = *(s.sm);

  float  fBatteryVoltage = 0.;
  auto pandaStates = sm["pandaStates"].getPandaStates();
  if (pandaStates.size() > 0) {
    fBatteryVoltage = pandaStates[0].getVoltage() * 0.001;
  }
  //auto deviceState = sm["deviceState"].getDeviceState();
 // float carBatteryCapacityWh = (int)deviceState.getCarBatteryCapacityUwh() * 0.0000001;

  beterryValtage.sprintf("%.1fV", fBatteryVoltage );
}

void CSidebar::paintEvent(QPainter &p) 
{
  // atom - battery
  float  batteryPercent = 50.0;


  const QRect  rect = battery_rc;
  QRect  bq(rect.left() + 6, rect.top() + 5, int((rect.width() - 19) * batteryPercent * 0.01), rect.height() - 11 );
  QBrush bgBrush("#149948");
  p.fillRect(bq, bgBrush);

  if( fBatteryVoltage > 12.5 ) p.drawPixmap(rect.x(), rect.y(), beterrry2_img );
  else p.drawPixmap( rect.x(), rect.y(), beterrry1_img );

  p.setPen(Qt::white);
  configFont(p, "Open Sans", 25, "Regular");  
  p.drawText(rect, Qt::AlignCenter, beterryValtage);    
}
