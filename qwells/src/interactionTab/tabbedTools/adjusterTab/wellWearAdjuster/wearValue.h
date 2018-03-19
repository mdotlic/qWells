#ifndef WEARVALUE_H
#define WEARVALUE_H

#include <QLineEdit>
class Data;
class QWellsPlot;
class Result;
class QKeyEvent;

class WearValue : public QLineEdit
{
   Q_OBJECT
public:
   WearValue(Data*,QWellsPlot*,Result*,const int,const int);
   
   void setMin(const double);
   void setMax(const double);
   void setNoOfSteps(const int);
   void setValueFromData(const double,const int);
   void setModified(const bool);
   void keyPressEvent(QKeyEvent*);
   
   double min() { return _min; }
   double max() { return _max; }
   int noOfSteps() { return _noOfSteps; }

signals:
   void valueChanged(int);
   void fixMin(int);
   void fixMax(int);
   void showRangeWarning(bool);
   void showNanWarning(bool);
      
private slots:
   void textModified();
   void verifyAndSubmitData();
   void setValueFromSlider(const int);
   
private:
   Data * _data;
   QWellsPlot * _plot;
   Result * _result;
   int _wellID;
   int _id; // _id = 0 for A, _id = 1 for B
   double _min = 0.0;
   double _max = std::numeric_limits<double>::max();
   double _value;
   bool _modified = false;
   bool _rangeFlagOn = false;
   bool _nanFlagOn = false;
   int _noOfSteps,_sliderValue;
};
#endif // WEARVALUE_H