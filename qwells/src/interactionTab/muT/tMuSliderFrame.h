#ifndef TMUSLIDERFRAME_H
#define TMUSLIDERFRAME_H

#include <QFrame>
class Data;
class QWellsPlot;
class Result;
class MinMaxDoubleValue;
class ValueForTMuSlider;
class QLineEdit;
class QSlider;
class QLabel;

class TMuSliderFrame : public QFrame
{
   Q_OBJECT
public:
   TMuSliderFrame(Data *,QWellsPlot*,Result*,const int);
   
   void setConnections();
   void initializeValues(const int);
   
   ValueForTMuSlider * value() { return _value; }
   
private slots:
   void setValueMin(const double);
   void setValueMax(const double);
   void setSliderMax();
   void setSliderValue(const double);
   void setValue(const int);
   void showRangeWarningLabel(const bool);
   void showNanWarningLabel(const bool);
   
private:
   MinMaxDoubleValue * _minValue;
   MinMaxDoubleValue * _maxValue;
   QLineEdit * _noOfSteps;
   QSlider * _slider;
   ValueForTMuSlider * _value;
   QLabel * _rangeWarningLabel;
   QLabel * _nanWarningLabel;
   int _iniMin,_iniMax;
};
#endif // DOUBLESLIDERFRAME_H