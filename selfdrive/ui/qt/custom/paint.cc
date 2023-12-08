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

  auto navi_custom = sm["naviCustom"].getNaviCustom();  
  m_param.naviData = navi_custom.getNaviData();

  //if (sm.updated("navInstruction")) {
  if (sm.valid("navInstruction")) {
      auto i = sm["navInstruction"].getNavInstruction();
      QString imageUrl = QString::fromStdString(i.getImageUrl());
      icon_01->requestImage(imageUrl);
      icon_01->setVisible(true);        
  }
  //}
}


void OnPaint::drawHud(QPainter &p)
{
  ui_main_navi( p );

}


void OnPaint::ui_main_navi( QPainter &p ) 
{
  QString text4;

  int bb_x = 100;
  int bb_y = 300;

  int  nYPos = bb_y;
  int  nGap = 80; 

  // p.setFont(InterFont(40, QFont::Normal));

  int activeNDA = m_param.naviData.getActive();
  int camType  = m_param.navData.getCamType();
  int roadLimitSpeed = m_param.naviData.getRoadLimitSpeed();
  int camLimitSpeed = m_param.naviData.getCamLimitSpeed();
  int camLimitSpeedLeftDist = m_param.naviData.getCamLimitSpeedLeftDist();
  int sectionLimitSpeed = m_param.naviData.getSectionLimitSpeed();
  int sectionLeftDist = m_param.naviData.getSectionLeftDist();
  int isNda2 = m_param.naviData.getIsNda2();

 QString roadName = QString::fromStdString( m_param.naviData.getCurrentRoadName().cStr() );
  
  // getCurrentRoadName();

  if( activeNDA == 1 )
  {
    m_nda[0].roadLimitSpeed = roadLimitSpeed;
    m_nda[0].camLimitSpeed = camLimitSpeed;
    m_nda[0].camLimitSpeedLeftDist = camLimitSpeedLeftDist;
    m_nda[0].sectionLimitSpeed = sectionLimitSpeed;
    m_nda[0].sectionLeftDist = sectionLeftDist;
  }
  if( isNda2 == 1 )
  {
    m_nda[1].roadLimitSpeed = roadLimitSpeed;
    m_nda[1].camLimitSpeed = camLimitSpeed;
    m_nda[1].camLimitSpeedLeftDist = camLimitSpeedLeftDist;
    m_nda[1].sectionLimitSpeed = sectionLimitSpeed;
    m_nda[1].sectionLeftDist = sectionLeftDist;
  }

  text4.sprintf("NDA = %d, %d", activeNDA, isNda2 );                                                p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("rLS = %d, %d", m_nda[0].roadLimitSpeed, m_nda[1].roadLimitSpeed );                  p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("cLS = %d, %d", m_nda[0].camLimitSpeed, m_nda[1].camLimitSpeed );                    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("cLSD = %d, %d", m_nda[0].camLimitSpeedLeftDist, m_nda[1].camLimitSpeedLeftDist );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("sLS = %d, %d", m_nda[0].sectionLimitSpeed , m_nda[1].sectionLimitSpeed );            p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("sLD = %d, %d", m_nda[0].sectionLeftDist, m_nda[1].sectionLeftDist );                p.drawText( bb_x, nYPos+=nGap, text4 );
     p.drawText( bb_x, nYPos+=nGap, roadName );
  

  
}