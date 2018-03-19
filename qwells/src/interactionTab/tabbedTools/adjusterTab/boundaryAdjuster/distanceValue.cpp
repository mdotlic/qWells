#include "distanceValue.h"

#include "engine/data.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "result/result.h"

#include <QLineEdit>
#include <QKeyEvent>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

DistanceValue::DistanceValue(Data * data, QWellsPlot * plot, Result * result, const int distID) : QLineEdit(), _data(data), _plot(plot), _result(result), _distID(distID)
{
   connect(this,&QLineEdit::returnPressed,
           this,&DistanceValue::verifyAndSubmitData);
   connect(this,&QLineEdit::textChanged,
           this,&DistanceValue::textModified);
   setAlignment(Qt::AlignCenter);
}

void DistanceValue::textModified()
{
   _modified = true;
}

void DistanceValue::setModified(const bool modified)
{
   _modified = modified;
   if (modified) setStyleSheet("color:red;");
   else setStyleSheet("color:black;");
}

void DistanceValue::setMin(const double minVal)
{
   if (minVal < _max) _min = minVal;
   else emit fixMin();
}

void DistanceValue::setMax(const double maxVal)
{
   if (maxVal > _min) _max = maxVal;
   else emit fixMax();
}

void DistanceValue::setNoOfSteps(const int noOfSteps)
{
   _noOfSteps = noOfSteps;
}

void DistanceValue::setValueFromData(const double value, const int sliderValue)
{
   _value = value;
   setText(QString::number(_value,'g',4));
   setModified(false);
   _sliderValue = sliderValue;
}

void DistanceValue::setValueFromSlider(const int sliderValue)
{
#ifdef QDEBUG_ALL
   qDebug()<<"DistanceValue::setValueFromSlider START | sliderValue = "<<sliderValue<<" | _sliderValue = "<<_sliderValue<<" | noOfSteps = "<<_noOfSteps<<" | min = "<<_min<<" | max = "<<_max<<"\n";
#endif
   if (_sliderValue == sliderValue) return;
   _sliderValue = sliderValue;
   const double value = _min + _sliderValue * (_max - _min)/_noOfSteps;
   _value = value;
   setText(QString::number(_value,'g',4));
   _data->setDistanceFromSlider(_distID,value);
   _data->fictiveWells().resize(0);
   SuperPos::makeFictiveWells(_data->wells(),_data->getBoundary(),_data->fictiveWells());
   _result->recalculateAll();
   _plot->setAllData(_data->timeInSMHD(),_result);
   if (!_modified) return;
   _modified = true;
   setStyleSheet("color:black;");
   if (_nanFlagOn) emit showNanWarning(false);  
   if (_rangeFlagOn) emit showRangeWarning(false); 
}

void DistanceValue::verifyAndSubmitData()
{
   if (text().isEmpty()) return;
   
   bool ok;
   double d = text().toDouble(&ok);
   if (ok)
   {
      if ((d>=_min) && (d<=_max))
      {
         _value = d;
         _data->setDistanceFromSlider(_distID,d);
         _data->fictiveWells().resize(0);
         SuperPos::makeFictiveWells(_data->wells(),_data->getBoundary(),_data->fictiveWells());
         _result->recalculateAll();
         _plot->setAllData(_data->timeInSMHD(),_result);
         _modified = false;
#ifdef QDEBUG_ALL
   qDebug()<<"DistanceValue::verifyAndSubmitData() START | send value to slider = "<< int(_noOfSteps * d / (_max - _min))<<"\n";
#endif
         _sliderValue = int((_noOfSteps-1) * d / (_max - _min));
         emit valueChanged(_sliderValue);
         emit showRangeWarning(false);
         emit showNanWarning(false);
      }
      else
      {
         setText(QString(QString::number(_value)));
         emit showRangeWarning(true);         
      }
      emit showNanWarning(false);
   }
   else 
   { 
      emit showRangeWarning(false);
      emit showNanWarning(true);
      setText(QString(QString::number(_value)));
   }
}

void DistanceValue::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Escape)
   {
      setText(QString::number(_value,'g',4));
      setModified(false);
   }
   QLineEdit::keyPressEvent(event);   
}
