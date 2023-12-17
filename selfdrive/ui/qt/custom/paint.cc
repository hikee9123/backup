#include "selfdrive/ui/qt/custom/paint.h"

#include <cmath>

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QConicalGradient>
#include <QPen>


#include "selfdrive/ui/qt/util.h"





// OnroadHud
OnPaint::OnPaint(QWidget *parent) : QWidget(parent) 
{
  state = uiState();
  scene = &(state->scene);

  QVBoxLayout *main_layout = new QVBoxLayout(this);
  main_layout->setContentsMargins(11, UI_BORDER_SIZE, 11, 20);

  QHBoxLayout *top_layout = new QHBoxLayout;
  top_layout->addWidget(icon_01 = new NetworkImageWidget, 0, Qt::AlignTop);

  main_layout->addLayout(top_layout);
  //connect(this, &OnPaint::valueChanged, [=] { update(); });


  img_tire_pressure = QPixmap("images/img_tire_pressure.png");  
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



void OnPaint::drawText(QPainter &p, int x, int y, const QString &text, QColor qColor, int nAlign ) 
{
  QFontMetrics fm(p.font());
  QRect init_rect = fm.boundingRect(text);
  QRect real_rect = fm.boundingRect(init_rect, 0, text);

  if( nAlign == Qt::AlignCenter ) // Qt::AlignLeft )
  {
     real_rect.moveCenter({x, y - real_rect.height() / 2});
  }
/*  
  else  if( nAlign ==  Qt::AlignRight  )
  {
    real_rect.moveLeft( x );
  }
  else  if( nAlign ==  Qt::AlignLeft  )
  {
    real_rect.moveRight( x );
  }
*/  
  else
  {
    real_rect.moveTo(x, y);
  }

  p.setPen( qColor ); //QColor(0xff, 0xff, 0xff, alpha));
  p.drawText(real_rect, nAlign, text);
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
  if (sm.frame % (UI_FREQ / 2) != 0) return;

  if ( sm.updated("naviCustom") )
  {
    auto navi_custom = sm["naviCustom"].getNaviCustom();  
    m_param.naviData = navi_custom.getNaviData();

    int activeNDA = m_param.naviData.getActive();
    int camType  = m_param.naviData.getCamType();
    int roadLimitSpeed = m_param.naviData.getRoadLimitSpeed();
    int camLimitSpeed = m_param.naviData.getCamLimitSpeed();
    int camLimitSpeedLeftDist = m_param.naviData.getCamLimitSpeedLeftDist();
    //int sectionLimitSpeed = m_param.naviData.getSectionLimitSpeed();
    //int sectionLeftDist = m_param.naviData.getSectionLeftDist();
    int isNda2 = m_param.naviData.getIsNda2();


    m_nda.activeNDA = activeNDA;
    m_nda.camType = camType;
    m_nda.roadLimitSpeed = roadLimitSpeed;
    m_nda.camLimitSpeed = camLimitSpeed;
    m_nda.camLimitSpeedLeftDist = camLimitSpeedLeftDist;    
  }

  if ( sm.updated("carStateCustom") )
  {
    auto carState_custom = sm["carStateCustom"].getCarStateCustom();
    m_param.tpmsData  = carState_custom.getTpms();
  }
}


void OnPaint::drawHud(QPainter &p)
{
  ui_main_navi( p );


  // 2. tpms
  if( true )
  {
    const int x = (UI_BORDER_SIZE * 2);
    const int y = height() - 211; 
    bb_draw_tpms( p, x, y);
  } 
}


void OnPaint::ui_main_navi( QPainter &p ) 
{
  QString text4;

  int bb_x = 100;
  int bb_y = 300;

  int  nYPos = bb_y;
  int  nGap = 80; 

 

  text4.sprintf("NDA = %d", m_nda.activeNDA );                p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("rLS = %d", m_nda.roadLimitSpeed );           p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("cLS = %d", m_nda.camLimitSpeed);             p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("cLSD = %d", m_nda.camLimitSpeedLeftDist);    p.drawText( bb_x, nYPos+=nGap, text4 );

/*
  int unit = m_param.tpmsData.getUnit();
  int fl = m_param.tpmsData.getFl();
  int fr = m_param.tpmsData.getFr();
  int rl = m_param.tpmsData.getRl();
  int rr = m_param.tpmsData.getRr();
  text4.sprintf("tpms = %d,%d,%d,%d,%d", unit, fl, fr, rl,rr ); p.drawText( bb_x, nYPos+=nGap, text4 );
*/
}



// tpms by neokii
QColor OnPaint::get_tpms_color(int tpms) 
{
    if(tpms < 5 || tpms > 60) // N/A
        return QColor(255, 255, 255, 200);
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

void OnPaint::bb_draw_tpms(QPainter &p, int viz_tpms_x, int viz_tpms_y )
{
    int fl = m_param.tpmsData.getFl();
    int fr = m_param.tpmsData.getFr();
    int rl = m_param.tpmsData.getRl();
    int rr = m_param.tpmsData.getRr();

    const int w = 58;
    const int h = 126;
    int x = viz_tpms_x;// bdr_s + 80;
    int y = viz_tpms_y - h;// s->fb_h - bdr_s - h - 60;

    const int margin = 30;

    p.setOpacity(0.8);
    p.drawPixmap(x, y, w, h, img_tire_pressure);

    p.setFont(InterFont(38, QFont::Bold));
    drawText( p, x  -margin, y+45,   get_tpms_text(fl), get_tpms_color(fl), Qt::AlignLeft  );
    drawText( p, x+w+margin, y+45,   get_tpms_text(fr), get_tpms_color(fr), Qt::AlignRight  );

    drawText( p, x  -margin, y+h-15, get_tpms_text(rl), get_tpms_color(rl), Qt::AlignLeft  );
    drawText( p, x+w+margin, y+h-15, get_tpms_text(rr), get_tpms_color(rr), Qt::AlignRight  );
}