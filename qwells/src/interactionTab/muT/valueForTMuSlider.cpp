#include "valueForTMuSlider.h"

#include "qWells.h"
#include "centralWindow.h"
#include "result/result.h"
#include "engine/data.h"
#include "widgets/qcustomplot/qwellsPlot.h"

#include <QLineEdit>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

ValueForTMuSlider::ValueForTMuSlider(Data * data, QWellsPlot * plot, Result * result, const int id) : QLineEdit(), _data(data), _plot(plot), _result(result), _id(id)
{
   connect(this,&QLineEdit::returnPressed,
           this,&ValueForTMuSlider::verifyAndSubmitData);
   connect(this,&QLineEdit::textChanged,
           this,&ValueForTMuSlider::setModified);
   setAlignment(Qt::AlignCenter);
}

void ValueForTMuSlider::setModified()
{
   _modified = true;
}

void ValueForTMuSlider::setMin(const double value)
{
   _min = value;
}

void ValueForTMuSlider::setMax(const double value)
{
   _max = value;
}

void ValueForTMuSlider::setValue(const int sliderValue)
{
   if (_sliderValue == sliderValue) return;
   _sliderValue = sliderValue;
   double d;
   if (sliderValue != 0) d = _min + sliderValue * (_max - _min)/_noOfSteps;
   else d = _min;
   _value = d;
   setText(QString(QString::number(_value,'g',3)));
   emit showRangeWarning(false);
   emit showNanWarning(false);
   if (_id == 0) _data->setBigT(d);
   else _data->setMu(d);
   _result->recalculateAll();
   _plot->setAllData(_data->timeInSMHD(),_result);
#ifdef QDEBUG_ALL
   qDebug()<<"ValueForTMuSlider::setValue() | value set to "<<d<<"\n";
#endif
}

void ValueForTMuSlider::setInitialValue()
{
   if (_id == 0) _value = _data->getModel().bigT();
   else _value = _data->getModel().mu();
   setText(QString(QString::number(_value,'g',4)));
   _sliderValue = int((_noOfSteps-1) * _value / (_max - _min));
   emit valueChanged(_sliderValue);
#ifdef QDEBUG_ALL
   if (_id)
   qDebug()<<"ValueForTMuSlider::setInitialValue() | MU value set to "<<_value<<"\n";
   else
   qDebug()<<"ValueForTMuSlider::setInitialValue() | T value set to "<<_value<<"\n";
#endif
}
   
void ValueForTMuSlider::setNoOfSteps(const int noOfSteps)
{
   _noOfSteps = noOfSteps;
}

void ValueForTMuSlider::verifyAndSubmitData()
{
   if (text().isEmpty()) return;
   
   bool ok;
   double d = text().toDouble(&ok);
   if (ok)
   {
      if ((d>=_min) && (d<=_max))
      {
         _value = d;
         _modified = false;
#ifdef QDEBUG_ALL
   qDebug()<<"ValueForTMuSlider::verifyAndSubmitData() START | send value to slider = "<< int(_noOfSteps * d / (_max - _min))<<"\n";
#endif
         _sliderValue = int((_noOfSteps-1) * d / (_max - _min));
         emit valueChanged(_sliderValue);
         emit showRangeWarning(false); 
         if (_id == 0) _data->setBigT(d);
         else _data->setMu(d);
         _result->recalculateAll();
         _plot->setAllData(_data->timeInSMHD(),_result); 
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
