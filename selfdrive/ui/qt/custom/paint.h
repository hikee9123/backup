#pragma once

#include <QStackedLayout>
#include <QWidget>


#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/custom/widgetNetImg.h"


class OnPaint : public QWidget 
{
  Q_OBJECT




public:
  explicit OnPaint(QWidget *parent);
  void updateState(const UIState &s);
  void drawHud(QPainter &p);

private:
  void    paintEvent(QPaintEvent *event) override;
  void    drawText(QPainter &p, int x, int y, const QString &text, QColor qColor = QColor(255,255,255,255), int nAlign = Qt::AlignCenter );
  int     get_time();

private:
  void   ui_main_navi( QPainter &p );

private:
  UIState  *state;
  UIScene  *scene;

 
  struct _PARAM_
  {

    
    cereal::NaviCustom::NaviData::Reader naviData;
 

  } m_param, m_old;
  

  struct _NDA
  {
     int roadLimitSpeed;
     int camLimitSpeed;
     int camLimitSpeedLeftDist;
     int sectionLimitSpeed;
     int sectionLeftDist;
  } m_nda[2];
  

private:
   NetworkImageWidget *icon_01;


  


// navi
private:
  float  interp( float xv, float xp[], float fp[], int N);
  int    get_param( const std::string &key );

// kegmen
private:



signals:
  void valueChanged();  
};
