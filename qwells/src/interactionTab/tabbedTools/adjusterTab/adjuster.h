#ifndef ADJUSTER_H
#define ADJUSTER_H

#include <QFrame>
class Data;
class QWellsPlot;
class Result;
class PlotChooser;
class BoundaryAdjuster;
class WellWearAdjuster;
class QVBoxLayout;

class Adjuster : public QFrame
{
   Q_OBJECT
public:
   Adjuster(Data*,QWellsPlot*,Result*,PlotChooser*);
   void adjustStretchFactors(const int);
   
public slots:
   void tabHandler(const int,const bool);
   
private:
   BoundaryAdjuster * _boundaryAdjuster;
   WellWearAdjuster * _wellWearAdjuster;
   QVBoxLayout * _adjusterLayout;
   bool _showing[2];
};
#endif // ADJUSTER_H