#include "selfdrive/ui/qt/custom/paint.h"

#include <cmath>

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QConicalGradient>
#include <QPen>


#include "selfdrive/ui/qt/util.h"





// OnroadHud
OnPaint::OnPaint(QWidget *parent, int width, int height ) : QWidget(parent) 
{
  m_sm = std::make_unique<SubMaster, const std::initializer_list<const char *>>({
    "peripheralState", "pandaStates", "gpsLocationExternal",
    "naviCustom", "carStateCustom", "uICustom", //"carControlCustom",
  });


  m_width = width;
  m_height = height;

  state = uiState();
  scene = &(state->scene);

  QVBoxLayout *main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(11, UI_BORDER_SIZE, 11, 20);

  QHBoxLayout *top_layout = new QHBoxLayout;
  top_layout->addWidget(icon_01 = new NetworkImageWidget, 0, Qt::AlignTop);

  main_layout->addLayout(top_layout);
  //connect(this, &OnPaint::valueChanged, [=] { update(); });

  is_debug = Params().getBool("ShowDebugMessage");
  //img_tire_pressure = QPixmap("qt/custom/images/img_tire_pressure.png");

  m_param.nIdx = 0;  
}


float OnPaint::interp( float xv, float xp[], float fp[], int N)
{
	float dResult = 0; 
	int low, hi = 0;

	while ( (hi < N) && (xv > xp[hi]))
	{
		hi += 1;
	}
	low = hi - 1;
	if( low < 0 )
	{
		low = N-1;
		return fp[0];
	}

	if (hi == N && xv > xp[low])
	{
		return fp[N-1];
	}
	else
	{
		if( hi == 0 )
		{
			return fp[0];
		}
		else
		{
			dResult = (xv - xp[low]) * (fp[hi] - fp[low]) / (xp[hi] - xp[low]) + fp[low];
		}
	}
	return  dResult;
}

void OnPaint::configFont(QPainter &p, const QString &family, int size, const QString &style) 
{
  QFont f(family);
  f.setPixelSize(size);
  f.setStyleName(style);
  p.setFont(f);
}


void OnPaint::drawText1(QPainter &p, int x, int y, const QString &text, QColor qColor, int nAlign ) 
{
  QFontMetrics fm(p.font());
  QRect init_rect = fm.boundingRect(text);
  QRect real_rect = fm.boundingRect(init_rect, 0, text);

  if( nAlign == Qt::AlignCenter ) // Qt::AlignLeft )
  {
     real_rect.moveCenter({x, y - real_rect.height() / 2});
  }
  else  if( nAlign ==  Qt::AlignRight  )
  {
    real_rect.moveLeft( x );
  }
  else  if( nAlign ==  Qt::AlignLeft  )
  {
    real_rect.moveRight( x );
  }
  else
  {
    real_rect.moveTo(x, y - real_rect.height() / 2);
  }


  p.setPen( qColor );
  p.drawText(real_rect, nAlign, text);
}


void OnPaint::drawText2(QPainter &p, int x, int y, int flags, const QString &text, const QColor color) 
{
  QFontMetrics fm(p.font());
  QRect rect = fm.boundingRect(text);
  rect.adjust(-1, -1, 1, 1);
  p.setPen(color);
  p.drawText(QRect(x, y, rect.width()+1, rect.height()), flags, text);
}


int OnPaint::get_time()
{
  int iRet;
  struct timeval tv;
  int seconds = 0;

  iRet = gettimeofday(&tv, NULL);
  if (iRet == 0)
  {
    seconds = (int)tv.tv_sec;
  }
  return seconds;
}

int OnPaint::get_param( const std::string &key )
{
    auto str = QString::fromStdString(Params().get( key ));
    int value = str.toInt();

    return value;
}




void OnPaint::updateState(const UIState &s)
{
  SubMaster &sm1 = *(s.sm);

  // 1.
  auto deviceState = sm1["deviceState"].getDeviceState();
  auto  maxCpuTemp = deviceState.getCpuTempC();  
  m_param.cpuPerc = deviceState.getCpuUsagePercent()[0];  
  m_param.cpuTemp = maxCpuTemp[0];


  // 1.
  auto radar_state = sm1["radarState"].getRadarState();  // radar
  m_param.lead_radar = radar_state.getLeadOne();

  // 1.
  auto car_state = sm1["carState"].getCarState();
  m_param.angleSteers = car_state.getSteeringAngleDeg();
  m_param.enginRpm =  car_state.getEngineRpm();


  // user message
  SubMaster &sm2 = *(m_sm);
  sm2.update(0);

  // 1.
  auto gps_ext = sm2["gpsLocationExternal"].getGpsLocationExternal();
  m_param.gpsAccuracyUblox = gps_ext.getAccuracy();
  m_param.altitudeUblox = gps_ext.getAltitude(); 


  // 1.
  auto peripheralState = sm2["peripheralState"].getPeripheralState();
  m_param.batteryVoltage = peripheralState.getVoltage() * 0.001;


  // 1.
  auto uiCustom = sm2["uICustom"].getUICustom();
  m_param.community  = uiCustom.getCommunity();
  is_debug = m_param.community.getShowDebugMessage();

  // 1.
  auto navi_custom = sm2["naviCustom"].getNaviCustom();
  auto naviData = navi_custom.getNaviData();
  int activeNDA = naviData.getActive();
  int camType  = naviData.getCamType();
  int roadLimitSpeed = naviData.getRoadLimitSpeed();
  int camLimitSpeed = naviData.getCamLimitSpeed();
  int camLimitSpeedLeftDist = naviData.getCamLimitSpeedLeftDist();
  int cntIdx = naviData.getCntIdx();

  m_nda.activeNDA = activeNDA;
  m_nda.camType = camType;
  m_nda.roadLimitSpeed = roadLimitSpeed;
  m_nda.camLimitSpeed = camLimitSpeed;
  m_nda.camLimitSpeedLeftDist = camLimitSpeedLeftDist;    
  m_nda.cntIdx = cntIdx;


  // 1.
  auto carState_custom = sm2["carStateCustom"].getCarStateCustom();
  m_param.tpmsData  = carState_custom.getTpms();

  // debug Message
  alert.alertTextMsg1 = carState_custom.getAlertTextMsg1();
  alert.alertTextMsg2 = carState_custom.getAlertTextMsg2();
  alert.alertTextMsg3 = carState_custom.getAlertTextMsg3();    

  m_param.electGearStep  = carState_custom.getElectGearStep();


    m_param.nIdx++;
    if( m_param.nIdx >= 10 )
        m_param.nIdx = 0;
}


void OnPaint::drawHud(QPainter &p)
{
  if( !is_debug ) return;
  ui_main_navi( p );



  if( true )
  {
    ui_draw_debug1( p );
  }

  // 2. tpms
  if( true )
  {
    const int x = 75;
    const int y = 800; 
    bb_draw_tpms( p, x, y);
  } 

  if( true )
  {
     bb_ui_draw_UI( p );
  }
}


void OnPaint::ui_main_navi( QPainter &p ) 
{
  QString text4;

  int bb_x = 10;
  int bb_y = 0;// 430;

  int  nYPos = bb_y;
  int  nGap = 80; 

 
 // text4.sprintf("rc = %d, %d", rect().right(), state->fb_w );                p.drawText( bb_x, nYPos+=nGap, text4 );
  //text4.sprintf("rLS = %d", m_nda.roadLimitSpeed );           p.drawText( bb_x, nYPos+=nGap, text4 );
  //text4.sprintf("cLS = %d", m_nda.camLimitSpeed);             p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("%d, %d, %d", m_nda.camLimitSpeedLeftDist , m_nda.cntIdx, m_param.nIdx );    p.drawText( bb_x, nYPos+=nGap, text4 );
 

  // auto uiCustom = sm["uICustom"].getUICustom();
  //text4.sprintf("batteryVoltage = %.1fV", m_param.batteryVoltage );           p.drawText( bb_x, nYPos+=nGap, text4 );
 // text4.sprintf("UseExternal = %d",  m_param.community.getUseExternalNaviRoutes() );           p.drawText( bb_x, nYPos+=nGap, text4 );
 // text4.sprintf("ShowDebug = %d",  is_debug );           p.drawText( bb_x, nYPos+=nGap, text4 );
  //text4.sprintf("m_cmdIdx = %d",  m_param.community.getCmdIdx() );           p.drawText( bb_x, nYPos+=nGap, text4 );
}


// tpms by neokii
QColor OnPaint::get_tpms_color(int tpms) 
{
    if(tpms < 5 || tpms > 60) // N/A
        return QColor(125, 125, 125, 200);
    if(tpms < 30)
        return QColor(255, 90, 90, 200);
    return QColor(255, 255, 255, 200);
}

QString OnPaint::get_tpms_text(int tpms) 
{
    if(tpms < 5 || tpms > 200)
        return "-";

    QString str;
    str.sprintf("%d", tpms );
    return str;
}

void OnPaint::bb_draw_tpms(QPainter &p, int x, int y )
{
    int fl = m_param.tpmsData.getFl();
    int fr = m_param.tpmsData.getFr();
    int rl = m_param.tpmsData.getRl();
    int rr = m_param.tpmsData.getRr();

    const int w = 58;
    const int h = 126;
    const int margin = 45;



    p.setFont(InterFont(38, QFont::Bold));
    drawText2( p, x   -margin, y+10,   Qt::AlignRight, get_tpms_text(fl), get_tpms_color(fl)  );
    drawText2( p, x+w +margin, y+10,   Qt::AlignLeft,  get_tpms_text(fr), get_tpms_color(fr)  );

    drawText2( p, x   -margin, y+h+20, Qt::AlignRight, get_tpms_text(rl), get_tpms_color(rl)  );
    drawText2( p, x+w +margin, y+h+20, Qt::AlignLeft,  get_tpms_text(rr), get_tpms_color(rr)  );

    p.setPen( QColor(255, 255, 255, 255) );
}



void OnPaint::ui_draw_debug1( QPainter &p ) 
{
  QString text1 = QString::fromStdString(alert.alertTextMsg1);
  QString text2 = QString::fromStdString(alert.alertTextMsg2);
  QString text3 = QString::fromStdString(alert.alertTextMsg3);

  int bb_x = 250;
  int bb_y = 930;
  int bb_w = state->fb_w - 500;// 1600;//width();

  QRect rc( bb_x, bb_y, bb_w, 90);

  p.setPen( QColor(255, 255, 255, 255) );
  p.setBrush(QColor(0, 0, 0, 100));
  p.drawRoundedRect(rc, 20, 20); 


  QTextOption  textOpt =  QTextOption( Qt::AlignLeft );
  configFont( p, "Open Sans",  40, "Regular");


  p.drawText( QRect(bb_x, 0, bb_w, 42), text1, textOpt );
  p.drawText( QRect(bb_x, bb_y, bb_w, 42), text2, textOpt );
  p.drawText( QRect(bb_x, bb_y+45, bb_w, 42), text3, textOpt );
}




//BB START: functions added for the display of various items
int OnPaint::bb_ui_draw_measure(QPainter &p,  const QString &bb_value, const QString &bb_uom, const QString &bb_label,
    int bb_x, int bb_y, int bb_uom_dx,
    QColor bb_valueColor, QColor bb_labelColor, QColor bb_uomColor,
    int bb_valueFontSize, int bb_labelFontSize, int bb_uomFontSize )
{
 
  int dx = 0;
  int nLen = bb_uom.length();
  if (nLen > 0) {
    dx = (int)(bb_uomFontSize*2.5/2);
   }


  //print value
  configFont( p, "Open Sans",  bb_valueFontSize*2, "SemiBold");
  drawText1( p, bb_x-dx/2, bb_y+ (int)(bb_valueFontSize*2.5)+5,  bb_value, bb_valueColor );
  //print label
  configFont( p, "Open Sans",  bb_valueFontSize*1, "Regular");
  drawText1( p, bb_x, bb_y + (int)(bb_valueFontSize*2.5)+5 + (int)(bb_labelFontSize*2.5)+5,  bb_label, bb_labelColor);

  //print uom
  if (nLen > 0) {

    int rx =bb_x + bb_uom_dx + bb_valueFontSize -3;
    int ry = bb_y + (int)(bb_valueFontSize*2.5/2)+25;
    configFont( p, "Open Sans",  bb_uomFontSize*2, "Regular");

    p.save();
    p.translate( rx, ry);
    p.rotate( -90 );
    p.setPen( bb_uomColor ); //QColor(0xff, 0xff, 0xff, alpha));
    p.drawText( 0, 0, bb_uom);
    //drawText( p, 0, 0, bb_uom, bb_uomColor);
    p.restore();
  }
  return (int)((bb_valueFontSize + bb_labelFontSize)*2.5) + 5;
}


QColor OnPaint::get_color( int nVal, int nRed, int nYellow ) 
{
  QColor  lab_color =  QColor(255, 255, 255, 255);

      if(nVal > nRed) {
        lab_color = QColor(255, 0, 0, 200);
      } else if( nVal > nYellow) {
        lab_color = QColor(255, 188, 3, 200);
      }

  return lab_color;
}


QColor OnPaint::angleSteersColor( int angleSteers ) 
{
    QColor val_color = QColor(255, 255, 255, 200);

    if( (angleSteers < -30) || (angleSteers > 30) ) {
      val_color = QColor(255, 175, 3, 200);
    }
    if( (angleSteers < -55) || (angleSteers > 55) ) {
      val_color = QColor(255, 0, 0, 200);
    }

    return val_color;
}


void OnPaint::bb_ui_draw_measures_left(QPainter &p, int bb_x, int bb_y, int bb_w ) 
{
  int bb_rx = bb_x + (int)(bb_w/2);
  int bb_ry = bb_y;
  int bb_h = 5;
  QColor lab_color = QColor(255, 255, 255, 200);
  QColor uom_color = QColor(255, 255, 255, 200);
  int value_fontSize=25;
  int label_fontSize=15;
  int uom_fontSize = 15;
  int bb_uom_dx =  (int)(bb_w /2 - uom_fontSize*2.5) ;


  if( bbh_left > 5 )
  {
    QRect rc( bb_x, bb_y, bb_w, bbh_left);
    p.setPen(QPen(QColor(0xff, 0xff, 0xff, 100), 3));
    p.setBrush(QColor(0, 0, 0, 100));
    p.drawRoundedRect(rc, 20, 20);
    p.setPen(Qt::NoPen);  
  }


  QString val_str;
  QString uom_str;

  //add visual radar relative distance
  if( true )
  {
    QColor val_color = QColor(255, 255, 255, 200);

    if ( m_param.lead_radar.getStatus() ) {
      //show RED if less than 5 meters
      //show orange if less than 15 meters
      float d_rel2 = m_param.lead_radar.getDRel();
      
      if((int)(d_rel2) < 15) {
        val_color = QColor(255, 188, 3, 200);
      }
      if((int)(d_rel2) < 5) {
        val_color = QColor(255, 0, 0, 200);
      }
      // lead car relative distance is always in meters
      val_str.sprintf("%d", (int)d_rel2 );
    } else {
       val_str = "-";
    }

    auto lead_cam = (*state->sm)["modelV2"].getModelV2().getLeadsV3()[0];  // camera
    if (lead_cam.getProb() > 0.1) {
      float d_rel1 = lead_cam.getX()[0];
      uom_str.sprintf("%d", (int)d_rel1 );
    }
    else
    {
      uom_str = "m";
    }

    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "REL DIST",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //add visual radar relative speed
  if( false )
  {
    QColor val_color = QColor(255, 255, 255, 200);
    if ( m_param.lead_radar.getStatus() ) {
      float v_rel = m_param.lead_radar.getVRel();  

      if((int)(v_rel) < 0) {
        val_color = QColor(255, 188, 3, 200);
      }
      if((int)(v_rel) < -5) {
        val_color = QColor(255, 0, 0, 200);
      }
      // lead car relative speed is always in meters
      if (scene->is_metric) {
        val_str.sprintf("%d", (int)(v_rel * 3.6 + 0.5) );
      } else {
        val_str.sprintf("%d", (int)(v_rel * 2.2374144 + 0.5));
      }
    } else {
       val_str = "-";
    }
    if (scene->is_metric) {
      uom_str = "km/h";
    } else {
      uom_str = "mph";
    }
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "REL SPEED",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //add  steering angle
  if( true )
  {
    QColor val_color = QColor(0, 255, 0, 200);

    val_color = angleSteersColor( (int)(m_param.angleSteers) );

    // steering is in degrees
    val_str.sprintf("%.1f",m_param.angleSteers);

    // steering is in degrees des
    uom_str = "des";

    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "REAL STEER",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }


    
  if( true )
  {
    float fEngineRpm = m_param.enginRpm;
    int   electGearStep  = m_param.electGearStep;
  
    uom_color = QColor(255, 255, 255, 200);
    QColor val_color = QColor(255, 255, 255, 200);

    if (  fEngineRpm <= 0 )
    {
      val_str.sprintf("EV"); 
      val_color = QColor(0, 255, 0, 200);
    }
    else 
    {
      val_str.sprintf("%.0f", fEngineRpm); 
      if( fEngineRpm > 2000 ) val_color = QColor(255, 188, 3, 200);
      else if( fEngineRpm > 3000 ) val_color = QColor(255, 0, 0, 200);
    }
    
    uom_str.sprintf("%d", electGearStep );
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "ENGINE",
      bb_rx, bb_ry, bb_uom_dx,
      val_color, lab_color, uom_color,
      value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //finally draw the frame
  bb_h += 20;
  bbh_left = bb_h;
}


void OnPaint::bb_ui_draw_measures_right( QPainter &p, int bb_x, int bb_y, int bb_w ) 
{
  int bb_rx = bb_x + (int)(bb_w/2);
  int bb_ry = bb_y;
  int bb_h = 5;
  QColor lab_color = QColor(255, 255, 255, 200);
  QColor uom_color = QColor(255, 255, 255, 200);
  int value_fontSize=25;
  int label_fontSize=15;
  int uom_fontSize = 15;
  int bb_uom_dx =  (int)(bb_w /2 - uom_fontSize*2.5) ;


  if ( bbh_right > 5 )
  {
    QRect rc( bb_x, bb_y, bb_w, bbh_right);
    p.setPen(QPen(QColor(0xff, 0xff, 0xff, 100), 3)); 
    p.setBrush(QColor(0, 0, 0, 100));
    p.drawRoundedRect(rc, 20, 20); 
    p.setPen(Qt::NoPen);
  }


  QString val_str;
  QString uom_str;
  //add CPU temperature
  if( true ) 
  {
    if( m_param.cpuTemp > 100 )  m_param.cpuTemp = 0;

    QColor val_color = QColor(255, 255, 255, 200);

     val_color = get_color(  (int)m_param.cpuTemp, 92, 80 );
     lab_color = get_color(  (int)m_param.cpuPerc, 90, 60 );

      // temp is alway in C * 10
      val_str.sprintf("%.1f", m_param.cpuTemp );
      uom_str.sprintf("%d", m_param.cpuPerc);
      bb_h += bb_ui_draw_measure(p,  val_str, uom_str, "CPU TEMP",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );

    bb_ry = bb_y + bb_h;
  }

  
   //add battery voltage
   lab_color = QColor(255, 255, 255, 200);
  if( true )
  {
    QColor val_color = QColor(255, 255, 255, 200);

    val_color = get_color( (int) m_param.batteryVoltage, 15, 11 );


    val_str.sprintf("%.1f", m_param.batteryVoltage );
    uom_str = "V";
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "BAT V",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }



  //add grey panda GPS accuracy
    QColor val_color = QColor(255, 255, 255, 200);
    //show red/orange if gps accuracy is low
     val_color = get_color( (int)m_param.gpsAccuracyUblox, 5, 2 );

    // gps accuracy is always in meters
    if(m_param.gpsAccuracyUblox > 99 || m_param.gpsAccuracyUblox == 0) {
       val_str = "None";
    }else if(m_param.gpsAccuracyUblox > 9.99) {
      val_str.sprintf("%.1f", m_param.gpsAccuracyUblox );
    }
    else {
      val_str.sprintf("%.2f", m_param.gpsAccuracyUblox );
    }
    uom_str.sprintf("%.1f", m_param.altitudeUblox); 
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "GPS PREC",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;


  //finally draw the frame
  bb_h += 20;
  bbh_right = bb_h;
}


void OnPaint::bb_ui_draw_UI(QPainter &p)
{
  const int bb_dml_w = 180;
  const int bb_dml_x = (0 + bdr_s) + 20;
  const int bb_dml_y = (0 + bdr_s) + 430;

  const int bb_dmr_w = 180;
  const int bb_dmr_x = 0 + state->fb_w - bb_dmr_w - bdr_s;
  const int bb_dmr_y = (0 + bdr_s) + 220;


  // 1. kegman ui
  bb_ui_draw_measures_left(p, bb_dml_x, bb_dml_y, bb_dml_w);
  bb_ui_draw_measures_right(p, bb_dmr_x, bb_dmr_y, bb_dmr_w);
}
//BB END: functions added for the display of various itemsapType