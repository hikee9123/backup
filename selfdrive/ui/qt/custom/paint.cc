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
  //img_tire_pressure = QPixmap("images/img_tire_pressure.png");

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

void OnPaint::drawText(QPainter &p, int x, int y, int flags, const QString &text, const QColor color) 
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


void OnPaint::paintEvent(QPaintEvent *event) 
{
  QPainter p(this);

}


void OnPaint::updateState(const UIState &s)
{
  SubMaster &sm = *(s.sm);
  //if (sm.frame % (UI_FREQ / 2) != 0) return;

  auto uiCustom = sm["uICustom"].getUICustom();

  m_param.uiCustom  = uiCustom.getCommunity();

  //if ( sm.updated("naviCustom") )
  //{
    auto navi_custom = sm["naviCustom"].getNaviCustom();
    auto naviData = navi_custom.getNaviData();
    //m_param.naviData = navi_custom.getNaviData();

    int activeNDA = naviData.getActive();
    int camType  = naviData.getCamType();
    int roadLimitSpeed = naviData.getRoadLimitSpeed();
    int camLimitSpeed = naviData.getCamLimitSpeed();
    int camLimitSpeedLeftDist = naviData.getCamLimitSpeedLeftDist();
    int cntIdx = naviData.getCntIdx();

    
    //int sectionLimitSpeed = naviData.getSectionLimitSpeed();
    //int sectionLeftDist = naviData.getSectionLeftDist();
    //int isNda2 = naviData.getIsNda2();

    //if( activeNDA >= 0 )
    //{
        m_nda.activeNDA = activeNDA;
        m_nda.camType = camType;
        m_nda.roadLimitSpeed = roadLimitSpeed;
        m_nda.camLimitSpeed = camLimitSpeed;
        m_nda.camLimitSpeedLeftDist = camLimitSpeedLeftDist;    
        m_nda.cntIdx = cntIdx;
    //}
  //}

  //const auto nav_instruction = sm["navInstruction"].getNavInstruction();
  //m_nda.camLimitSpeedLeftDist = nav_instruction.getManeuverDistance();


  //if ( sm.updated("carStateCustom") )
  //{
      auto carState_custom = sm["carStateCustom"].getCarStateCustom();
      m_param.tpmsData  = carState_custom.getTpms();

      // debug Message
      alert.alertTextMsg1 = carState_custom.getAlertTextMsg1();
      alert.alertTextMsg2 = carState_custom.getAlertTextMsg2();
      alert.alertTextMsg3 = carState_custom.getAlertTextMsg3();    
  //}

    m_param.nIdx++;
    if( m_param.nIdx > 1000 )
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

}


void OnPaint::ui_main_navi( QPainter &p ) 
{
  QString text4;

  int bb_x = 100;
  int bb_y = 430;

  int  nYPos = bb_y;
  int  nGap = 80; 

 
  //text4.sprintf("NDA = %d", m_nda.activeNDA );                p.drawText( bb_x, nYPos+=nGap, text4 );
  //text4.sprintf("rLS = %d", m_nda.roadLimitSpeed );           p.drawText( bb_x, nYPos+=nGap, text4 );
  //text4.sprintf("cLS = %d", m_nda.camLimitSpeed);             p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("%d, %d, %d", m_nda.camLimitSpeedLeftDist , m_nda.cntIdx, m_param.nIdx );    p.drawText( bb_x, nYPos+=nGap, text4 );



  // auto uiCustom = sm["uICustom"].getUICustom();
  text4.sprintf("HapticFeedback = %d", m_param.uiCustom.community.getHapticFeedbackWhenSpeedCamera() );           p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("UseExternal = %d",  m_param.uiCustom.community.getUseExternalNaviRoutes() );           p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("ShowDebug = %d",  m_param.uiCustom.community.getShowDebugMessage() );           p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("m_cmdIdx = %d",  m_param.uiCustom.community.getCmdIdx() );           p.drawText( bb_x, nYPos+=nGap, text4 );
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
    drawText( p, x   -margin, y+10,   Qt::AlignRight, get_tpms_text(fl), get_tpms_color(fl)  );
    drawText( p, x+w +margin, y+10,   Qt::AlignLeft,  get_tpms_text(fr), get_tpms_color(fr)  );

    drawText( p, x   -margin, y+h+20, Qt::AlignRight, get_tpms_text(rl), get_tpms_color(rl)  );
    drawText( p, x+w +margin, y+h+20, Qt::AlignLeft,  get_tpms_text(rr), get_tpms_color(rr)  );

    p.setPen( QColor(255, 255, 255, 255) );
}



void OnPaint::ui_draw_debug1( QPainter &p ) 
{
  QString text1 = QString::fromStdString(alert.alertTextMsg1);
  QString text2 = QString::fromStdString(alert.alertTextMsg2);
  QString text3 = QString::fromStdString(alert.alertTextMsg3);

  int bb_x = 250;
  int bb_y = 930;
  int bb_w = 1600;//width();

  QRect rc( bb_x, bb_y, bb_w, 90);

  p.setPen( QColor(255, 255, 255, 255) );
  p.setBrush(QColor(0, 0, 0, 100));
  p.drawRoundedRect(rc, 20, 20); 


  QTextOption  textOpt =  QTextOption( Qt::AlignLeft );
  configFont( p, "Open Sans",  40, "Regular");


  //text3 = "1234567890";
  p.drawText( QRect(bb_x, 0, bb_w, 42), text1, textOpt );
  p.drawText( QRect(bb_x, bb_y, bb_w, 42), text2, textOpt );
  p.drawText( QRect(bb_x, bb_y+45, bb_w, 42), text3, textOpt );
}