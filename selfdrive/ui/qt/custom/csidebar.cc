#include "selfdrive/ui/qt/custom/csidebar.h"

#include <QMouseEvent>

#include "selfdrive/ui/qt/util.h"

CSidebar::CSidebar(QFrame *parent)  
{
  QRect  rect(160, 247, 76, 36);
  beterrry1_img = loadPixmap("qt/custom/images/battery.png", rect.size());
  beterrry2_img = loadPixmap("qt/custom/images/battery_charging.png", rect.size());
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
  // atom - battery
  float  batteryPercent = 50.0;

  const QRect  rect(160, 247, 76, 36);
  QRect  bq(rect.left() + 6, rect.top() + 5, int((rect.width() - 19) * batteryPercent * 0.01), rect.height() - 11 );
  QBrush bgBrush("#149948");
  p.fillRect(bq, bgBrush);

  if( fBatteryVoltage > 12.5 ) p.drawImage(rect, beterrry2_img );
  else p.drawImage(rect, beterrry1_img );

  p.setPen(Qt::white);
  configFont(p, "Open Sans", 25, "Regular");  
  p.drawText(rect, Qt::AlignCenter, beterryValtage);    
}
