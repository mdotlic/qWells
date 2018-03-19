#include "adjuster.h"
#include "boundaryAdjuster.h"
#include "wellWearAdjuster.h"

#include "widgets/tabTools/collapsingBar.h"

#include <QFrame>
#include <QVBoxLayout>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

Adjuster::Adjuster(Data * data,QWellsPlot * plot,Result * result, PlotChooser * chooser) : QFrame()
{   
   setFrameShape(QFrame::NoFrame);
   _adjusterLayout = new QVBoxLayout;
   _adjusterLayout->setContentsMargins(0,5,0,5);
   _adjusterLayout->setAlignment(Qt::AlignTop);
   
   _boundaryAdjuster = new BoundaryAdjuster(data,plot,result);
   _adjusterLayout->addWidget(_boundaryAdjuster);
//    _adjusterLayout->setStretchFactor(_boundaryAdjuster,0.5);
   _adjusterLayout->setStretchFactor(_boundaryAdjuster,0);
   connect(_boundaryAdjuster,&BoundaryAdjuster::iAmVisible,
           this,&Adjuster::tabHandler);
   
   _wellWearAdjuster = new WellWearAdjuster(data,plot,result,chooser);
   _adjusterLayout->addWidget(_wellWearAdjuster);
   _adjusterLayout->setStretchFactor(_wellWearAdjuster,1); 
   connect(_wellWearAdjuster,&WellWearAdjuster::iAmVisible,
           this,&Adjuster::tabHandler); 
   
   QWidget * spacer = new QWidget;
   spacer->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::MinimumExpanding);
   _adjusterLayout->addWidget(spacer);
   
   setLayout(_adjusterLayout); 
}

void Adjuster::tabHandler(const int itemID, const bool showing)
{
   switch (itemID)
   {
      case 0:
         _showing[0] = showing;
         break;
      case 1:
         _showing[1] = showing;
         break;
      default:
         qDebug()<<"Adjuster::tabHandler | ERROR - unknown item\n";
         break;
   }
   if (!_showing[0] && !_showing[1])  adjustStretchFactors(0);
   else if (_showing[0] && !_showing[1])  adjustStretchFactors(1);
   else if (!_showing[0] && _showing[1])  adjustStretchFactors(2);
   else if (_showing[0] && _showing[1])  adjustStretchFactors(3);
}

//    flag = 0 (both items hidden)
//           1 (1st item shown)
//           2 (2nd item shown)
//           3 (both items shown)
void Adjuster::adjustStretchFactors(const int flag)
{
#ifdef QDEBUG_ALL
   qDebug()<<"Adjuster::adjustStretchFactors START | flag = "<<flag<<"\n";
#endif
   switch (flag)
   {
      case 0:
      {
         _adjusterLayout->setStretchFactor(_boundaryAdjuster,0);
         _adjusterLayout->setStretchFactor(_wellWearAdjuster,1);
         break;
      }
      case 1:
      {
         _adjusterLayout->setStretchFactor(_boundaryAdjuster,2);
         _adjusterLayout->setStretchFactor(_wellWearAdjuster,0);
         break;
      }
      case 2:
      {
         _adjusterLayout->setStretchFactor(_boundaryAdjuster,0);
         _adjusterLayout->setStretchFactor(_wellWearAdjuster,1);
         break;
      }
      case 3:
      {
         _adjusterLayout->setStretchFactor(_boundaryAdjuster,1);
         _adjusterLayout->setStretchFactor(_wellWearAdjuster,1);
         break;
      }
      default:
      {
         qDebug()<<"Adjuster::adjustStretchFactors | ERROR - unknown flag\n";
         assert(false);
         break;
      }
   }
}