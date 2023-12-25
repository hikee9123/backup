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
  UIState *s = uiState();
  UIScene &scene = s->scene;

  // atom - battery
  float  batteryPercent = 90.0;


  
 QColor  color;


  QString beterryValtage;
  beterryValtage.sprintf("%.1f", fBatteryVoltage );

  if( fBatteryVoltage < 5 ) 
  {
      color = QColor( 100, 100, 100 );
      beterryValtage = "-";
  }
  else if( scene.started )  // 충전중.
  {
      if( fBatteryVoltage > 14.4 )     color = QColor( 0, 128, 255 );
      else if( fBatteryVoltage > 13.4 )     color = QColor( 0, 255, 0 );
      else if( fBatteryVoltage > 13.2 )   color = QColor( 255, 255, 0 );
      else if( fBatteryVoltage > 12.5 )   color = QColor( 255, 128, 64 );
      else  color = QColor( 255, 0, 0 );
  }
  else
  {
      if( fBatteryVoltage > 12.7 )     color = QColor( 0, 128, 255  );
      else if( fBatteryVoltage > 12.4 )     color = QColor( 0, 255, 0  );
      else if( fBatteryVoltage > 12.0 )   color = QColor( 255, 255, 0  );
      else if( fBatteryVoltage > 11.6 )   color = QColor( 255, 128, 64 );
      else if( fBatteryVoltage > 10.8)   color = QColor( 255, 0, 0 );
      else if( fBatteryVoltage > 5)   color = QColor( 128, 0, 0 );
      else  color = QColor( 255, 0, 0 );
  }


  const QRect  rect = battery_rc;

  if( fBatteryVoltage > 5 )
  {
    QRect  bq(rect.left() + 6, rect.top() + 5, int((rect.width() - 19) * batteryPercent * 0.01), rect.height() - 11 );
    QBrush bgBrush = color; //("#149948");
    p.fillRect(bq, bgBrush);
  }


  p.drawPixmap( rect.x(), rect.y(), beterrry1_img );

  p.setPen(Qt::black);
  configFont(p, "Open Sans", 25, "Regular");  
  p.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, beterryValtage);    
}
