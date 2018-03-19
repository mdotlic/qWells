#include "wearValue.h"

#include "helperFunctions.h"
#include "engine/data.h"
#include "engine/well.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "result/result.h"

#include <QLineEdit>
#include <QKeyEvent>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

// _id = 0 for A, _id = 1 for B
WearValue::WearValue(Data * data, QWellsPlot * plot, Result * result, const int wellID, const int id) : QLineEdit(), _data(data), _plot(plot), _result(result), _wellID(wellID), _id(id)
{
   connect(this,&QLineEdit::returnPressed,
           this,&WearValue::verifyAndSubmitData);
   connect(this,&QLineEdit::textChanged,
           this,&WearValue::textModified);
   setAlignment(Qt::AlignCenter);
}

void WearValue::textModified()
{
   _modified = true;
}

void WearValue::setModified(const bool modified)
{
   _modified = modified;
   if (modified) setStyleSheet("color:red;");
   else setStyleSheet("color:black;");
}

void WearValue::setMin(const double minVal)
{
   if (minVal < _max) _min = minVal;
   else emit fixMin(_id);
}

void WearValue::setMax(const double maxVal)
{
   if (maxVal > _min) _max = maxVal;
   else emit fixMax(_id);
}

void WearValue::setNoOfSteps(const int noOfSteps)
{
   _noOfSteps = noOfSteps;
}

void WearValue::setValueFromData(const double value, const int sliderValue)
{
   _value = value;
   setText(QString::number(_value,'g',4));
   setModified(false);
   _sliderValue = sliderValue;
}

void WearValue::setValueFromSlider(const int sliderValue)
{
   if (_sliderValue == sliderValue) return;
   _sliderValue = sliderValue;
   const double value = _min + _sliderValue * (_max - _min)/_noOfSteps;
   _value = value;
   setText(QString::number(_value,'g',4));
   if (_id == 0) _data->setAFromSlider(_wellID,_value);
   else _data->setBFromSlider(_wellID,_value);
   _result->calculateWellLosses(_wellID);
   _plot->setChangedLosses(_data->timeInSMHD(),_result,_wellID);
   if (!_modified) return;
   _modified = true;
   setStyleSheet("color:black;");
   if (_nanFlagOn) emit showNanWarning(false);  
   if (_rangeFlagOn) emit showRangeWarning(false); 
}

void WearValue::verifyAndSubmitData()
{
   if (text().isEmpty()) return;
   
   bool ok;
   double d = text().toDouble(&ok);
   if (ok)
   {
      if ((d>=_min) && (d<=_max))
      {
         _value = d;
#ifdef QDEBUG_ALL
   qDebug()<<"WearValue::verifyAndSubmitData() START | send value to slider = "<< int(_noOfSteps * d / (_max - _min))<<"\n";
#endif
         _sliderValue = int((_noOfSteps-1) * d / (_max - _min));
         emit valueChanged(_sliderValue);
         if (_id == 0) _data->setAFromSlider(_wellID,_value);
         else _data->setBFromSlider(_wellID,_value);
         _result->calculateWellLosses(_wellID);
         _plot->setChangedLosses(_data->timeInSMHD(),_result,_wellID);
         emit showRangeWarning(false); 
         setModified(false);
      }
      else
      {
         setText(QString(QString::number(_value,'g',4)));
         emit showRangeWarning(true); 
         setModified(true);
         _rangeFlagOn = true;
      }
      emit showNanWarning(false);
      _nanFlagOn = false;
   }
   else 
   { 
      emit showRangeWarning(false);
      _rangeFlagOn = false;
      emit showNanWarning(true);
      _nanFlagOn = true;
      setText(QString(QString::number(_value)));
      setModified(true);
   }
}

void WearValue::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Escape)
   {
      setText(QString::number(_value,'g',4));
      setModified(false);
   }
   QLineEdit::keyPressEvent(event);   
}
