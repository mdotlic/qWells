#include "collapsingBar.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QString>
#include <QLabel>
#include <QMouseEvent>

CollapsingBar::CollapsingBar(QString title) : QFrame(), _title(title)
{
   QHBoxLayout * barLayout = new QHBoxLayout;
   
   _barArrow = new QLabel;
   _barArrow->setPixmap(QIcon(":/resources/images/tabClosed.png").pixmap(8,8));
   _barArrow->setFixedSize(8,8);
   
   _barLabel = new QLabel(_title);
   //_barLabel->setFixedHeight(5);
   
   barLayout->addWidget(_barArrow);
   barLayout->addWidget(_barLabel);
   barLayout->setContentsMargins(5,0,5,0);
   setLayout(barLayout);
   
   setFixedHeight(25);
   setFocusPolicy(Qt::ClickFocus);
   _barArrow->setFocusPolicy(Qt::ClickFocus);
   
   setFrameShape(QFrame::NoFrame);

   _isVisible = false;
   
}

void CollapsingBar::switchArrow(bool isVisible)
{
   if (isVisible) 
      _barArrow->setPixmap(QIcon(":/resources/images/tabOpen.png").pixmap(8,8));
   else 
      _barArrow->setPixmap(QIcon(":/resources/images/tabClosed.png").pixmap(8,8));
}

void CollapsingBar::mousePressEvent(QMouseEvent * event)
{
   if (event->button()==Qt::LeftButton)
   {
      _isVisible = !_isVisible;
      switchArrow(_isVisible);
      emit(switchVisibility(_isVisible));
      event->accept();
   }   
}

void CollapsingBar::collapse()
{
   _isVisible = false;
   switchArrow(_isVisible);
   emit(switchVisibility(_isVisible));
}