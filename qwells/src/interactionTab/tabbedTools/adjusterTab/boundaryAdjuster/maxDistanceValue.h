#ifndef MAXDISTANCEVALUE_H
#define MAXDISTANCEVALUE_H

#include <QLineEdit>
class Data;
class QWellsPlot;
class Result;

class MaxDistanceValue : public QLineEdit
{
   Q_OBJECT
public:
   MaxDistanceValue(Data*,QWellsPlot*,Result*);
   
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
   void fixMin();
   void fixMax();
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
   int _distID;
   double _min = 0;
   double _max = std::numeric_limits<double>::max();
   double _value;
   bool _modified = false;
   bool _rangeFlagOn = false;
   bool _nanFlagOn = false;
   int _noOfSteps,_sliderValue;
};
#endif // MAXDISTANCEVALUE_H