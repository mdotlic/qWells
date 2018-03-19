#include "boundaryAdjuster.h"

// #include "qWells.h" // change bg color of titleBar()

#include "boundaryAdjuster/distanceSlider.h"
#include "boundaryAdjuster/maxDistanceSlider.h"
#include "engine/data.h"
#include "engine/boundary.h"
#include "result/result.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "widgets/tabTools/tabToolBase.h"
#include "widgets/tabTools/collapsingBar.h"

#include <QFrame>
#include <QScrollArea>
#include <QVBoxLayout>
#include <vector>

#include <QLabel>
#include <QPalette>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

BoundaryAdjuster::BoundaryAdjuster(Data * data, QWellsPlot * plot, Result * result) : TabToolBase("ADJUST DISTANCES"), _data(data), _plot(plot), _result(result)
{   
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster START\n";
#endif
         
   _boundaryAdjusterLayout = new QVBoxLayout;
   _boundaryAdjusterLayout->setContentsMargins(0,0,0,0);
   _boundaryAdjusterLayout->setAlignment(Qt::AlignTop);
      
   QFrame * boundaryAdjusterFrame = new QFrame;
   
   boundaryAdjusterFrame->setLayout(_boundaryAdjusterLayout);
//    layout()->addWidget(boundaryAdjusterFrame);
   
   connect(_data,&Data::refWellSet,this,&BoundaryAdjuster::setAdjuster);
   connect(_data,&Data::noBoundarySet,this,&BoundaryAdjuster::disableAdjuster);
   connect(_data,&Data::distanceSet,this,&BoundaryAdjuster::setDistance);
   connect(_data,&Data::maxDistanceSet,
           this,&BoundaryAdjuster::setMaxDistance);
//    connect(_data,&Data::clearBoundaryAdjuster,this,&BoundaryAdjuster::clear);
   
   QScrollArea * scroll = new QScrollArea;
//    scroll->setPalette(this->palette());
   scroll->setBackgroundRole(QPalette::AlternateBase);
   scroll->setFrameShape(QFrame::StyledPanel);
   scroll->sizePolicy().setVerticalPolicy(QSizePolicy::Maximum);
   layout()->addWidget(scroll,Qt::AlignTop);
   layout()->setContentsMargins(0,0,0,0);
   layout()->setSpacing(0);
   scroll->setWidget(boundaryAdjusterFrame);
   scroll->setWidgetResizable(true);
   scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
   //scroll->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);

   scroll->setVisible(false);
   connect(this->titleBar(),SIGNAL(switchVisibility(bool)),
           scroll,SLOT(setVisible(bool)));
   connect(this->titleBar(),SIGNAL(switchVisibility(bool)),
           this,SLOT(resendWithID(bool)));
      
   const int refWellID = _data->getBoundary().refWell();
   setAdjuster(refWellID);
   
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster END\n";
#endif
}

void BoundaryAdjuster::constructBoundaryFrames()
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::constructBoundaryFrames() START\n";
#endif
   if (_distanceFrames.size()>0) _distanceFrames.clear();
   
   _distanceFrames.resize(_noOfBoundaries);
   for (int i=0;i<_noOfBoundaries;i++)
   {
      _distanceFrames[i] =  new DistanceSlider(_data,_plot,_result,i);
      _boundaryAdjusterLayout->addWidget(_distanceFrames[i]);
   }
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::constructBoundaryFrames() END | layoutCount = "<<_boundaryAdjusterLayout->count()<<"\n";
#endif
}

void BoundaryAdjuster::setDistance(const int distID, const double distance)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::setDistance() START | distID = "<<distID<<"\n";
#endif
   _distanceFrames[distID]->setValueFromData(distance);
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::setDistance() END | distID = "<<distID<<" | distance set to "<<distance<<"\n";
#endif
}

void BoundaryAdjuster::setMaxDistance(const double maxDistance)
{
   _maxDistanceFrame->setValueFromData(maxDistance);
}

void BoundaryAdjuster::setAdjuster(const int refWellID)
{
   if (_distanceFrames.size() > 0) 
   {
      const int noOfFrames = _distanceFrames.size();
      for (int i=0;i<noOfFrames;i++) 
      {
         delete _distanceFrames[i];
      }
      _distanceFrames.clear();
      //_maxDistanceFrame = nullptr;//FIXME 14.02.2017
      delete _maxDistanceFrame;
      _maxDistanceFrame = nullptr;
   }
   if ((size_t)refWellID == std::numeric_limits<size_t>::max())
   {
#ifdef QDEBUG_ALL
      qDebug()<<"BoundaryAdjuster::setAdjuster START | referent well is not set, DISABLING\n";
#endif
      setEnabled(false);
      return;
   }
   setEnabled(true);
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::setAdjuster() START | refWellID = "<<refWellID<<"\n";
#endif
   _noOfBoundaries = _data->getBoundary().nlines();
   if (_noOfBoundaries>0) 
   {
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::setAdjuster() | noOfBoundaries = "<<_noOfBoundaries<<"\n";
#endif
      constructBoundaryFrames();
      const size_t boundaryType = _data->getBoundary().type();
      if ((boundaryType >= 2) && (boundaryType != 3))
      {
         _maxDistanceFrame = new MaxDistanceSlider(_data,_plot,_result);
         _boundaryAdjusterLayout->addWidget(_maxDistanceFrame);
      }
      appendRefWellName(refWellID);
   }
   //else setEnabled(false); // should never happen as setAdjuster is triggered by refWellSet which is activated when referent well is chosen, which can't be done if _noBoundaries = 0 (case of No Lines)
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::setAdjuter() END\n";
#endif
}

void BoundaryAdjuster::appendRefWellName(const int wellID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster\n";
#endif
   const QString toRefWell = QString(" FROM ").append(_data->wells()[wellID].name());
   appendToTitle(toRefWell);
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryAdjuster::appendRefWellName | new label : DISTANCE"<<toRefWell<<"\n";
#endif
}

void BoundaryAdjuster::resendWithID(const bool showing)
{
   emit iAmVisible(adjusterID,showing);
}

void BoundaryAdjuster::disableAdjuster()
{
   setEnabled(false);
}

// void BoundaryAdjuster::clear()
// {
//    
// }
