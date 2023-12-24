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

int CSidebar::updateState(const UIState &s) 
{
  SubMaster &sm = *(s.sm);
  if (sm.frame % (UI_FREQ) != 0) return 0;

  //auto &sm = *(s.sm);

  frame_cnt++;
  auto pandaStates = sm["pandaStates"].getPandaStates();
  if (pandaStates.size() > 0) {
    fBatteryVoltage = pandaStates[0].getVoltage() * 0.001;
  }
  //auto deviceState = sm["deviceState"].getDeviceState();
 // float carBatteryCapacityWh = (int)deviceState.getCarBatteryCapacityUwh() * 0.0000001;

  return 1;
}

void CSidebar::paintEvent(QPainter &p) 
{
  // atom - battery
  float  batteryPercent = 100.0;


  int nB = (125 + (fBatteryVoltage - 12.0)*150);
  if( nB  < 0 ) nB = 0;
  else if( nB > 255 ) nB = 255;

  int nR = (255-nB);


  QString beterryValtage;
  beterryValtage.sprintf("%.1f", fBatteryVoltage );

  if( fBatteryVoltage <= 0)
  {
    nB = 255;
    nR = 255;
    beterryValtage = "-";
  }

  const QRect  rect = battery_rc;
  QRect  bq(rect.left() + 6, rect.top() + 5, int((rect.width() - 19) * batteryPercent * 0.01), rect.height() - 11 );
  QBrush bgBrush = QColor( nR, nB, nB); //("#149948");
  p.fillRect(bq, bgBrush);




  p.drawPixmap( rect.x(), rect.y(), beterrry1_img );

  p.setPen(Qt::white);
  configFont(p, "Open Sans", 25, "Regular");  
  p.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, beterryValtage);    
}
