#ifndef DISTANCESLIDER_H
#define DISTANCESLIDER_H

#include <QFrame>
class Data;
class QWellsPlot;
class Result;
class MinMaxDoubleValue;
class ValueForTMuSlider;
class QLineEdit;
class QSlider;
class QLabel;
class DistanceValue;

class DistanceSlider : public QFrame
{
   Q_OBJECT
public:
   DistanceSlider(Data*,QWellsPlot*,Result*,const int);
   
   void setConnections();
   //void setMinMaxStep(const double,const double,const int);
   void setInitialization();
   void initialize(const double,const double,const int,const double);
   void setValueFromData(const double);
      
public slots:
   void fixMin();
   void fixMax();
   
private slots:
   void setValueMin(const double);
   void setValueMax(const double);
   void setNoOfSteps();
   void showRangeWarningLabel(const bool);
   void showNanWarningLabel(const bool);
   
private:
   Data * _data;
   QWellsPlot * _plot;
   Result * _result;
   int _distID;
   MinMaxDoubleValue * _minValue;
   MinMaxDoubleValue * _maxValue;
   QLineEdit * _noOfSteps;
   QSlider * _slider;
   DistanceValue * _value;
   QLabel * _rangeWarningLabel;
   QLabel * _nanWarningLabel;
   double _dMin,_dMax;
};
#endif // DISTANCESLIDER_H