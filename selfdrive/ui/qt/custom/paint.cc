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



  //connect(this, &OnPaint::valueChanged, [=] { update(); });
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
  SubMaster &sm = *(s.sm);
  if (sm.frame % (UI_FREQ / 2) != 0) return;

  auto navi_custom = sm["naviCustom"].getNaviCustom();  
  m_param.naviData = navi_custom.getNaviData();

  update(); 
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




void OnPaint::paintEvent(QPaintEvent *event) 
{
  QPainter p(this);
  p.save(); 
  ui_main_navi( p );
  p.restore();   
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

  p.setPen( qColor ); //QColor(0xff, 0xff, 0xff, alpha));
  p.drawText(real_rect, nAlign, text);
}



void OnPaint::ui_main_navi( QPainter &p ) 
{

  QString text4;

  int bb_x = 250;
  int bb_y = 300;

  int  nYPos = bb_y;
  int  nGap = 80; 
  configFont( p, "Open Sans",  80, "Regular");

  int activeNDA = m_param.naviData.getActive();
  int roadLimitSpeed = m_param.naviData.getRoadLimitSpeed();
  int camLimitSpeed = m_param.naviData.getCamLimitSpeed();
  int camLimitSpeedLeftDist = m_param.naviData.getCamLimitSpeedLeftDist();
  int sectionLimitSpeed = m_param.naviData.getSectionLimitSpeed();
  int sectionLeftDist = m_param.naviData.getSectionLeftDist();
  int isNda2 = m_param.naviData.getIsNda2();


  text4.sprintf("activeNDA = %d", activeNDA );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("roadLimitSpeed = %d", roadLimitSpeed );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("camLimitSpeed = %d", camLimitSpeed );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("camLimitSpeedLeftDist = %d", camLimitSpeedLeftDist );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("sectionLimitSpeed = %d", sectionLimitSpeed );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("sectionLeftDist = %d", sectionLeftDist );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("sectionLeftDist = %d", isNda2 );    p.drawText( bb_x, nYPos+=nGap, text4 );
}