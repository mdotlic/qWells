#ifndef WELLWEARADJUSTER_H
#define WELLWEARADJUSTER_H

#include "widgets/tabTools/tabToolBase.h"
#include <vector>
class Data;
class QWellsPlot;
class Result;
class PlotChooser;
class WearSlider;
class QVBoxLayout;
class QScrollArea;
class QLabel;

class WellWearAdjuster : public TabToolBase
{
   Q_OBJECT
public:
   WellWearAdjuster(Data*,QWellsPlot*,Result*,PlotChooser*);
   
   static constexpr int adjusterID = 1;
   
public slots:
   void addWell(const int);
   void removeWell(const int);
   void changeWellName(const int,const int,const QString&);
   void setBigA(const int);
   void setBigB(const int);
   void showAdjuster(const int);
   void hideAdjuster(const int);
   void resendWithID(const bool);
   
signals:
   void iAmVisible(int,bool);
   
private:
   Data * _data;
   QWellsPlot * _plot;
   Result * _result;
   PlotChooser * _chooser;
   int _noOfWells;
   QVBoxLayout * _wellWearAdjusterLayout;
   QVBoxLayout * _sliderFrameLayout;
   std::vector<WearSlider*> _wearFrames;
   QLabel * _infoLabel;
};
#endif // WELLWEARADJUSTER_H