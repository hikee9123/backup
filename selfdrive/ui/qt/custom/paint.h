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
    cereal::CarStateCustom::Tpms::Reader tpmsData;
 

  } m_param, m_old;
  

  struct _NDA
  {
     int activeNDA;
     int camType;
     int roadLimitSpeed;
     int camLimitSpeed;
     int camLimitSpeedLeftDist;
  } m_nda;
  

private:
   NetworkImageWidget *icon_01;
   QPixmap img_tire_pressure;

  


// navi
private:
  float  interp( float xv, float xp[], float fp[], int N);
  int    get_param( const std::string &key );

// tpms
private:
  QColor   get_tpms_color(int tpms); 
  QString  get_tpms_text(int tpms);
  void     bb_draw_tpms(QPainter &p, int x, int y );


signals:
  void valueChanged();  
};
