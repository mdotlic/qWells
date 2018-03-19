#ifndef MAXDISTANCESLIDER_H
#define MAXDISTANCESLIDER_H

#include <QFrame>
class Data;
class QWellsPlot;
class Result;
class MinMaxDoubleValue;
class QLineEdit;
class QSlider;
class QLabel;
class MaxDistanceValue;

class MaxDistanceSlider : public QFrame
{
   Q_OBJECT
public:
   MaxDistanceSlider(Data*,QWellsPlot*,Result*);
   
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
   MinMaxDoubleValue * _minValue;
   MinMaxDoubleValue * _maxValue;
   QLineEdit * _noOfSteps;
   QSlider * _slider;
   MaxDistanceValue * _value;
   QLabel * _rangeWarningLabel;
   QLabel * _nanWarningLabel;
   double _dMin,_dMax;
};
#endif // MAXDISTANCESLIDER_H