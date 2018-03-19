#ifndef BOUNDARYADJUSTER_H
#define BOUNDARYADJUSTER_H

#include "widgets/tabTools/tabToolBase.h"
class Data;
class QWellsPlot;
class Result;
class DistanceSlider;
class MaxDistanceSlider;
class QVBoxLayout;
class QScrollArea;
#include <vector>

class BoundaryAdjuster : public TabToolBase
{
   Q_OBJECT
public:
   BoundaryAdjuster(Data*,QWellsPlot*,Result*);
   
   void constructBoundaryFrames();
   void disconnectWellDistance(int);
   void connectWellDistance(int);
   void appendRefWellName(const int);
   
   static constexpr int adjusterID = 0;
   
public slots:
   void setAdjuster(const int);
   void setDistance(int,double);
   void setMaxDistance(const double);
   void resendWithID(const bool);
   void disableAdjuster();
   
signals:
   void iAmVisible(int,bool);
   
private:
   Data * _data;
   QWellsPlot * _plot;
   Result * _result;
   int _noOfBoundaries;
   QVBoxLayout * _boundaryAdjusterLayout;
   std::vector<DistanceSlider*> _distanceFrames;
   MaxDistanceSlider * _maxDistanceFrame=nullptr;
   QScrollArea * scroll;
};
#endif // BOUNDARYADJUSTER_H
