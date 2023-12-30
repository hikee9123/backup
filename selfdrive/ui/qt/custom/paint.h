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
  void    drawText1(QPainter &p, int x, int y, const QString &text, QColor qColor = QColor(255,255,255,255), int nAlign = Qt::AlignCenter  );
  void    drawText2(QPainter &p, int x, int y, int flags, const QString &text, const QColor color = QColor(255, 255, 255, 220) );
  int     get_time();

private:
  void   ui_main_navi( QPainter &p );

private:
  UIState  *state;
  UIScene  *scene;

  std::unique_ptr<SubMaster> m_sm;

  int m_width;
  int m_height;
  int bbh_left = 0;
  int bbh_right = 0;
  const int bdr_s = 30;

  struct _PARAM_
  {

    cereal::RadarState::LeadData::Reader lead_radar;
    //cereal::CarState::Reader car_state;

    cereal::CarStateCustom::Tpms::Reader tpmsData;
    cereal::UICustom::Community::Reader community;
    cereal::UICustom::Ui::Reader ui;

    int   cpuPerc;
    float cpuTemp; 

    int   electGearStep;

    float  angleSteers;
    int   enginRpm = 0;

    float batteryVoltage;

    float altitudeUblox;    
    float gpsAccuracyUblox;


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
  QString gearGap( int gear_step, QColor &color );
// tpms
private:
  QColor   get_tpms_color(int tpms); 
  QString  get_tpms_text(int tpms);
  void     bb_draw_tpms(QPainter &p, int x, int y );
  void     ui_draw_debug1( QPainter &p );


// kegmen
private:
  int  bb_ui_draw_measure(QPainter &p,  const QString &bb_value, const QString &bb_uom, const QString &bb_label,
    int bb_x, int bb_y, int bb_uom_dx,
    QColor bb_valueColor, QColor bb_labelColor, QColor bb_uomColor,
    int bb_valueFontSize, int bb_labelFontSize, int bb_uomFontSize, int bb_uom_dy = 0 );

  void bb_ui_draw_measures_right(QPainter &p, int bb_x, int bb_y, int bb_w );
  void bb_ui_draw_measures_left(QPainter &p, int bb_x, int bb_y, int bb_w );

  QColor  get_color( int nVal, int nRed, int nYellow );
  QColor angleSteersColor( int angleSteers );

  void  bb_ui_draw_UI(QPainter &p);

signals:
  //void    valueChanged();  
};
