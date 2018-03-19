#include "distanceSlider.h"

#include "qWells.h"
#include "centralWindow.h"
#include "engine/data.h"
#include "distanceValue.h"
#include "helperFunctions.h"
#include "widgets/minMaxDoubleValue.h"

#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QString>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

DistanceSlider::DistanceSlider(Data * data, QWellsPlot * plot, Result * result, const int distID) : QFrame(), _data(data), _plot(plot), _result(result), _distID(distID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"DistanceSlider START | _distID = "<<_distID<<"\n";
#endif
   setFocusPolicy(Qt::ClickFocus);
   
   QGridLayout * sliderFrameLayout = new QGridLayout;

   _minValue = new MinMaxDoubleValue();
   _maxValue = new MinMaxDoubleValue();
   _noOfSteps = new QLineEdit;

   _slider = new QSlider(Qt::Horizontal);
   _slider->setSingleStep(1);
   _slider->setMinimum(0);
   //_slider->setTickPosition(QSlider::TicksBelow);
   _slider->setTickPosition(QSlider::NoTicks);
   
   _value = new DistanceValue(data,plot,result,distID);
      
   QLabel * minLabel = new QLabel(QString("min"));
   QLabel * maxLabel = new QLabel(QString("max"));
   QLabel * stepLabel = new QLabel(QString("steps"));
   QLabel * nameLabel = new QLabel(QString("Line %1").arg(distID+1));
   nameLabel->setAlignment(Qt::AlignCenter);
   nameLabel->setStyleSheet("font: 12px bold;");
//    QVBoxLayout * sliderNameLayout = new QVBoxLayout;
//    sliderNameLayout->addWidget(nameLabel);
//    sliderNameLayout->addWidget(_slider);
   _rangeWarningLabel = new QLabel(QString("NOT IN RANGE!"));
   _rangeWarningLabel->setAlignment(Qt::AlignCenter);
   _rangeWarningLabel->setStyleSheet("color:rgb(204,0,0);font:9px bold");
   _nanWarningLabel = new QLabel(QString("NOT A NUMBER!"));
   _nanWarningLabel->setAlignment(Qt::AlignCenter);
   _nanWarningLabel->setStyleSheet("color:rgb(204,0,0);font:9px bold");
   QVBoxLayout * warningValueLayout = new QVBoxLayout;
   warningValueLayout->addWidget(_rangeWarningLabel,Qt::AlignCenter);
   warningValueLayout->addWidget(_nanWarningLabel,Qt::AlignCenter);
   
   sliderFrameLayout->addWidget(minLabel,0,0,Qt::AlignLeft);
   sliderFrameLayout->addWidget(maxLabel,1,0,Qt::AlignLeft);
   sliderFrameLayout->addWidget(stepLabel,2,0,Qt::AlignLeft);
   sliderFrameLayout->addWidget(_minValue,0,1,Qt::AlignLeft);
   sliderFrameLayout->addWidget(_maxValue,1,1,Qt::AlignLeft);
   sliderFrameLayout->addWidget(_noOfSteps,2,1,Qt::AlignLeft);
   sliderFrameLayout->addWidget(nameLabel,0,3,Qt::AlignCenter);
   sliderFrameLayout->addWidget(_slider,1,3,2,1,Qt::AlignCenter);
   //sliderFrameLayout->addLayout(sliderNameLayout,0,3,3,1);
   sliderFrameLayout->addLayout(warningValueLayout,0,4,Qt::AlignCenter);
   sliderFrameLayout->addWidget(_value,1,4,2,1,Qt::AlignCenter);
   //sliderFrameLayout->setColumnStretch(2,1);
   sliderFrameLayout->setColumnStretch(5,2);
   _rangeWarningLabel->hide();
   _nanWarningLabel->hide();
   
   setLayout(sliderFrameLayout);
   
   const int boxSize = 80;
   _noOfSteps->setFixedWidth(boxSize);
   _minValue->setFixedWidth(boxSize);
   _maxValue->setFixedWidth(boxSize);
   _value->setFixedWidth(boxSize);
   
   setFixedHeight(sizeHint().height());
   
   setConnections();
   setInitialization();
   
#ifdef QDEBUG_ALL
   qDebug()<<"DistanceSlider END | _distID = "<<_distID<<"\n";
#endif
}

void DistanceSlider::setConnections()
{
//    connect(_minValue,&QLineEdit::returnPressed,
//            this,&DistanceSlider::setValueMin);
//    connect(_maxValue,&QLineEdit::returnPressed,
//            this,&DistanceSlider::setValueMax);
   connect(_minValue,SIGNAL(valueChanged(double)),
           this,SLOT(setValueMin(double)));
   connect(_maxValue,SIGNAL(valueChanged(double)),
           this,SLOT(setValueMax(double)));
   connect(_noOfSteps,&QLineEdit::returnPressed,
           this,&DistanceSlider::setNoOfSteps);
   connect(_value,&DistanceValue::fixMin,
           this,&DistanceSlider::fixMin);
   connect(_value,&DistanceValue::fixMax,
           this,&DistanceSlider::fixMax);
   
   // slider with value
   connect(_slider,SIGNAL(valueChanged(int)),
           _value,SLOT(setValueFromSlider(int)));
   connect(_value,SIGNAL(valueChanged(int)),
           _slider,SLOT(setValue(int)));
   
   // _value with "out of range" and "Nan" warnings
   connect(_value,SIGNAL(showRangeWarning(bool)),
           this,SLOT(showRangeWarningLabel(bool)));
   connect(_value,SIGNAL(showNanWarning(bool)),
           this,SLOT(showNanWarningLabel(bool)));
}

// void DistanceSlider::setMinMaxStep(const double dMin, const double dMax, const int steps)
void DistanceSlider::setInitialization()
{
   _dMin = -1000.0;
   _dMax = 1000.0;
   const int steps = 200;
      
   const double valueInData = _data->getBoundary().rho(_distID);
   if (valueInData < _dMin) _dMin = valueInData;
   else if ((valueInData > _dMax) && 
         (valueInData != std::numeric_limits<double>::max())) 
      _dMax = valueInData;
   double value;
   if (valueInData != std::numeric_limits<double>::max())
      value = valueInData;
   else value = 100.0;
      
   initialize(_dMin,_dMax,steps,value);
}

void DistanceSlider::initialize(const double iniMin, const double iniMax, const int noOfSteps, const double value)
{   
#ifdef QDEBUG_ALL
   qDebug()<<"DistanceSlider::initialize START | iniMin = "<<iniMin<<" | iniMax = "<<iniMax<<" | noOfSteps = "<<noOfSteps<<" | value = "<<value<<"\n";
#endif
   // these three don't trigger signals because there is no Return press
   _minValue->enterData(iniMin);
   _maxValue->enterData(iniMax);
   _noOfSteps->setText(QString(QString::number(noOfSteps)));
   // ... so we do this instead:
   _value->setMin(iniMin);
   _value->setMax(iniMax);
   _value->setNoOfSteps(noOfSteps);
   _slider->setMaximum(noOfSteps);
   // set value and slider - we don't want to write data back to Data so we avoid procedures that do so
   const int sliderValue = noOfSteps * (value - iniMin) / (iniMax - iniMin);
   _value->setValueFromData(value,sliderValue);
   _slider->setValue(sliderValue);
#ifdef QDEBUG_ALL
   qDebug()<<"DistanceSlider::initialize END\n";
#endif
}

void DistanceSlider::setValueMin(const double dVal)
{
   _value->setMin(dVal);
   if (_value->min() == dVal) _dMin = dVal;
}

void DistanceSlider::setValueMax(const double dVal)
{
   _value->setMax(dVal);
   if (_value->max() == dVal) _dMax = dVal;
}

void DistanceSlider::fixMin()
{
   _minValue->enterData(_dMin); // FIXME should also change value min?
}

void DistanceSlider::fixMax()
{
   _maxValue->enterData(_dMax); // FIXME should also change value min?
}

void DistanceSlider::setNoOfSteps()
{
   int noOfSteps;
   const bool ok = conversion::convertQStringToInt(_noOfSteps->text(),noOfSteps);
   if (!ok) { qDebug()<<" string to int fail\n"; assert(false); }
   _slider->setMaximum(noOfSteps);
   _value->setNoOfSteps(noOfSteps);
}

void DistanceSlider::showRangeWarningLabel(const bool show)
{
   if (show) _rangeWarningLabel->show();
   else _rangeWarningLabel->hide();
}

void DistanceSlider::showNanWarningLabel(const bool show)
{
   if (show) _nanWarningLabel->show();
   else _nanWarningLabel->hide();
}

void DistanceSlider::setValueFromData(const double value)
{
   if (value < _value->min())
   {
      _dMin = value;
      fixMin();
      _value->setMin(value);
   }
   else if (value > _value->max())
   {
      _dMax = value;
      fixMax();
      _value->setMax(value);
   }
   const int sliderValue = _value->noOfSteps() * (value - _value->min()) / (_value->max() - _value->min());
   _value->setValueFromData(value,sliderValue);
   _slider->setValue(sliderValue);
#ifdef QDEBUG_ALL
   qDebug()<<"DistanceSlider::setValueFromData END | distance "<<_distID<<" set to = "<<value<<" | sliderValue = "<<sliderValue<<"\n";
#endif
}
   
