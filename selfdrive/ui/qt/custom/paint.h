#pragma once

#include <QStackedLayout>
#include <QWidget>


#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/custom/widgetNetImg.h"


class OnPaint : public QWidget 
{
  Q_OBJECT




public:
  explicit OnPaint(QWidget *parent, int width, int height );
  void updateState(const UIState &s);
  void drawHud(QPainter &p);

private:
  void    paintEvent(QPaintEvent *event) override;
  void    drawText(QPainter &p, int x, int y, int flags, const QString &text, const QColor color = QColor(255, 255, 255, 220) );
  int     get_time();

private:
  void   ui_main_navi( QPainter &p );

private:
  UIState  *state;
  UIScene  *scene;


  int m_width;
  int m_height;
 
  struct _PARAM_
  {

    
    //cereal::NaviCustom::NaviData::Reader naviData;
    cereal::CarStateCustom::Tpms::Reader tpmsData;
    cereal::UICustom::Community::Reader uiCustom;


    int  nIdx;
  } m_param;
  
  struct _STATUS_
  {
      std::string alertTextMsg1;
      std::string alertTextMsg2; 
      std::string alertTextMsg3;
  } alert;

  struct _NDA
  {
     int activeNDA;
     int camType;
     int roadLimitSpeed;
     int camLimitSpeed;
     int camLimitSpeedLeftDist;
     int cntIdx;
  } m_nda;
  

private:
   NetworkImageWidget *icon_01;
   //QPixmap img_tire_pressure;
   int  is_debug;
  
private:
  void   configFont(QPainter &p, const QString &family, int size, const QString &style);

// navi
private:
  float  interp( float xv, float xp[], float fp[], int N);
  int    get_param( const std::string &key );

// tpms
private:
  QColor   get_tpms_color(int tpms); 
  QString  get_tpms_text(int tpms);
  void     bb_draw_tpms(QPainter &p, int x, int y );
  void     ui_draw_debug1( QPainter &p );

signals:
  //void    valueChanged();  
};
