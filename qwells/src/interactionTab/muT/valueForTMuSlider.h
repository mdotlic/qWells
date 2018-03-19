#ifndef VALUEFORTMUSLIDER_H
#define VALUEFORTMUSLIDER_H

#include <QLineEdit>
class Data;
class QWellsPlot;
class Result;

class ValueForTMuSlider : public QLineEdit
{
   Q_OBJECT
public:
   ValueForTMuSlider(Data*,QWellsPlot*,Result*,const int);
   
   double min() { return _min; }
   double max() { return _max; }
   double value() { return _value; }
   void setInitialValue();
   void setNoOfSteps(const int);

signals:
   void valueChanged(int);
   void showRangeWarning(bool);
   void showNanWarning(bool);
   
public slots:
   void setMin(const double);
   void setMax(const double);
   void setValue(const int);
   
private slots:
   void setModified();
   void verifyAndSubmitData();
   
private:
   Data * _data;
   QWellsPlot * _plot;
   Result * _result;
   int _id; // _id = 0 for T, _id = 1 for mu
   double _min;
   double _max;
   double _value;
   bool _modified = false;
   int _noOfSteps,_sliderValue;
};
#endif // VALUEFORTMUSLIDER_H