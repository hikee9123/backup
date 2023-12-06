#pragma once

#include <QStackedLayout>
#include <QWidget>


#include "selfdrive/ui/ui.h"



class OnPaint : public QWidget 
{
  Q_OBJECT




public:
  explicit OnPaint(QWidget *parent);
  void updateState(const UIState &s);

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

    
    cereal::Event::naviCustom::Reader naviData;
 

  } m_param, m_old;
  

private:



  


// navi
private:
  float  interp( float xv, float xp[], float fp[], int N);
  int    get_param( const std::string &key );

// kegmen
private:



signals:
  void valueChanged();  
};
