#include "tMuSliderFrame.h"

#include "valueForTMuSlider.h"
#include "helperFunctions.h"
#include "engine/data.h"
#include "widgets/minMaxDoubleValue.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "result/result.h"

#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QString>
#include <QChar>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

// varID = 0 for T, varID = 1 for mu
TMuSliderFrame::TMuSliderFrame(Data * data, QWellsPlot * plot, Result * result, const int varID) : QFrame()
{
   QGridLayout * sliderFrameLayout = new QGridLayout;

   _minValue = new MinMaxDoubleValue();
   _maxValue = new MinMaxDoubleValue();
   _noOfSteps = new QLineEdit;
   
   _slider = new QSlider(Qt::Horizontal);
   _slider->setSingleStep(1);
   _slider->setMinimum(0);
   _slider->setTickPosition(QSlider::TicksBelow);
   
   _value = new ValueForTMuSlider(data,plot,result,varID); 
   
   QLabel * minLabel = new QLabel(QString("min"));
   QLabel * maxLabel = new QLabel(QString("max"));
   QLabel * stepLabel = new QLabel(QString("steps"));
   QString nameStr;
   if (varID == 0) nameStr = QString("T");
   else nameStr = QChar(0x03BC);
   QLabel * nameLabel = new QLabel(nameStr);
   nameLabel->setAlignment(Qt::AlignCenter);
   nameLabel->setStyleSheet("font: 18px bold;");
   if(varID == 0)
      nameLabel->setToolTip("<font size=12px>Transmissivity [m"+QString(QChar(0x00B2))+"/s]</font>");
   else
      nameLabel->setToolTip("<font size=12px>Storativity [-]</font>");
   const QString fontString = QString("font:12px;");
   _minValue->setStyleSheet(fontString);
   _maxValue->setStyleSheet(fontString);
   _noOfSteps->setStyleSheet(fontString);
   _rangeWarningLabel = new QLabel(QString("Value outside range!"));
   _rangeWarningLabel->setAlignment(Qt::AlignCenter);
   _rangeWarningLabel->setStyleSheet("color:rgb(204,0,0);font:bold");
   _nanWarningLabel = new QLabel(QString("Nan - Not a number!"));
   _nanWarningLabel->setAlignment(Qt::AlignCenter);
   _nanWarningLabel->setStyleSheet("color:rgb(204,0,0);font:bold");
   QVBoxLayout * warningLayout = new QVBoxLayout;
   warningLayout->addWidget(_rangeWarningLabel,Qt::AlignCenter);
   warningLayout->addWidget(_nanWarningLabel,Qt::AlignCenter);
   if (varID==0) // T
   {
      sliderFrameLayout->addWidget(minLabel,0,1,Qt::AlignLeft);
      sliderFrameLayout->addWidget(maxLabel,1,1,Qt::AlignLeft);
      sliderFrameLayout->addWidget(stepLabel,2,1,Qt::AlignLeft);
      sliderFrameLayout->addWidget(_minValue,0,2,Qt::AlignLeft);
      sliderFrameLayout->addWidget(_maxValue,1,2,Qt::AlignLeft);
      sliderFrameLayout->addWidget(_noOfSteps,2,2,Qt::AlignLeft);
      sliderFrameLayout->addLayout(warningLayout,0,4,Qt::AlignCenter);
      sliderFrameLayout->addWidget(_slider,1,3,2,2,Qt::AlignRight);
      sliderFrameLayout->addWidget(nameLabel,0,5,Qt::AlignCenter);
      sliderFrameLayout->addWidget(_value,1,5,2,1,Qt::AlignRight);
      sliderFrameLayout->setColumnStretch(3,1);
   }
   else // mu
   {
      sliderFrameLayout->addWidget(minLabel,0,5,Qt::AlignLeft);
      sliderFrameLayout->addWidget(maxLabel,1,5,Qt::AlignLeft);
      sliderFrameLayout->addWidget(stepLabel,2,5,Qt::AlignLeft);
      sliderFrameLayout->addWidget(_minValue,0,4,Qt::AlignRight);
      sliderFrameLayout->addWidget(_maxValue,1,4,Qt::AlignRight);
      sliderFrameLayout->addWidget(_noOfSteps,2,4,Qt::AlignRight);
      sliderFrameLayout->addLayout(warningLayout,0,2,Qt::AlignCenter);
      sliderFrameLayout->addWidget(_slider,1,2,2,2,Qt::AlignLeft);
      sliderFrameLayout->addWidget(nameLabel,0,1,Qt::AlignCenter);
      sliderFrameLayout->addWidget(_value,1,1,2,1,Qt::AlignLeft);
      sliderFrameLayout->setColumnStretch(3,1);
   }
   _rangeWarningLabel->hide();
   _nanWarningLabel->hide();
   
   setLayout(sliderFrameLayout);
   
   _minValue->setMinimumWidth(40);
   _minValue->setMaximumWidth(60);
   _maxValue->setMinimumWidth(40);
   _maxValue->setMaximumWidth(60);
   _noOfSteps->setMinimumWidth(40);
   _noOfSteps->setMaximumWidth(60);
   _slider->setMinimumHeight(40);
//    SLIDER WIDTH
   _slider->setMinimumWidth(120); // default but too wide on low res laptops
//    _slider->setMinimumWidth(40);
//    _slider->setMaximumWidth(150);
//    _slider->sizePolicy().setHorizontalPolicy(QSizePolicy::Maximum);
//    END SLIDER WIDTH
   _value->setFixedSize(90,40);
   _value->setStyleSheet("font: 20px bold;");
   setMaximumHeight(90);
   
   setConnections();
   initializeValues(varID);
}

void TMuSliderFrame::setConnections()
{
//    connect(_minValue,&MinMaxDoubleValue::valueChanged,
//            this,&TMuSliderFrame::setValueMin);
//    connect(_maxValue,&MinMaxDoubleValue::valueChanged,
//            this,&TMuSliderFrame::setValueMax);
   connect(_minValue,SIGNAL(valueChanged(double)),
           this,SLOT(setValueMin(double)));
   connect(_maxValue,SIGNAL(valueChanged(double)),
           this,SLOT(setValueMax(double)));
   connect(_noOfSteps,&QLineEdit::returnPressed,
           this,&TMuSliderFrame::setSliderMax);
   
   // slider with value
   connect(_slider,SIGNAL(valueChanged(int)),
           _value,SLOT(setValue(int)));
   connect(_value,SIGNAL(valueChanged(int)),
           _slider,SLOT(setValue(int)));
   
   // _value with "out of range" and "Nan" warnings
   connect(_value,SIGNAL(showRangeWarning(bool)),
           this,SLOT(showRangeWarningLabel(bool)));
   connect(_value,SIGNAL(showNanWarning(bool)),
           this,SLOT(showNanWarningLabel(bool)));
}

void TMuSliderFrame::initializeValues(const int varID)
{
   double iniMin,iniMax;
   int noOfSteps;
   
   if (varID == 0) // T slider
   {
      iniMin = 0.001;
      iniMax = 1;
      noOfSteps = 1000;
   }
   else if (varID == 1) // mu slider
   {
      iniMin = 0.000001;
      iniMax = 1;
      noOfSteps = 1000;      
   }
   else
   {
      qDebug()<<"TMuSliderFrame::initializeValues | ERROR - incorrect slider id (not 0 or 1)! Exiting.\n";
      assert(false);
   }
   
   _value->setMin(iniMin);
   _value->setMax(iniMax);
   
   _minValue->enterData(iniMin);
   _maxValue->enterData(iniMax);
   
   _noOfSteps->setText(QString(QString::number(noOfSteps)));
   _slider->setMaximum(noOfSteps);
   //_slider->setValue(1);
   _value->setNoOfSteps(noOfSteps);
   // Here, we can't call 
   //_value->setValue(_slider->value());
   _value->setInitialValue();
   // because it contains _data->setMu(), which sets the model to modified state, causing prompt to save upon application start. Instead, setInitialValue is written in _value just for this reason.
}

void TMuSliderFrame::setValueMin(const double dVal)
{
   if (dVal <= _value->max())
   {
      _value->setMin(dVal);
      _minValue->setText(QString(QString::number(dVal,'g',4)));
#ifdef QDEBUG_ALL
   qDebug()<<"TMuSliderFrame::setValueMin | set min value of _value to "<<dVal<<"\n";
#endif
      _minValue->setStyleSheet("color:black;");
   }
   else _minValue->setStyleSheet("color:red;");
}

void TMuSliderFrame::setValueMax(const double dVal)
{
   if (dVal >= _value->min())
   {
      _value->setMax(dVal);
      _maxValue->setText(QString(QString::number(dVal,'g',4)));
#ifdef QDEBUG_ALL
   qDebug()<<"TMuSliderFrame::setValueMax | set max value of _value to "<<dVal<<"\n";
#endif
      _maxValue->setStyleSheet("color:black;");
   }
   else _maxValue->setStyleSheet("color:red;");
}

void TMuSliderFrame::setSliderMax()
{
   int noOfSteps;
   const bool ok = conversion::convertQStringToInt(_noOfSteps->text(),noOfSteps);
   if (!ok)
   { 
      qDebug()<<"TMuSliderFrame::setSliderMax | ERROR - string to int conversion failed\n";
      _noOfSteps->setText(QString(QString::number(1000,'g',4)));
      _noOfSteps->setStyleSheet("color:red;");
      return;
   }
   _slider->setMaximum(noOfSteps);
   _value->setNoOfSteps(noOfSteps); // _value keeps noOfSteps!
   _noOfSteps->setStyleSheet("color:black;");
}

void TMuSliderFrame::setSliderValue(const double dVal)
{
#ifdef QDEBUG_ALL
   qDebug()<<"TMuSliderFrame::setSliderValue() START | _slider->maximum() = "<<_slider->maximum()<<" | dVal = "<<dVal<<" | _value->max() = "<<_value->max()<<" | _value->min() = "<<_value->min()<<"\n";
#endif
   _slider->setValue(int(_slider->maximum() * dVal / (_value->max() - _value->min())));
#ifdef QDEBUG_ALL
   qDebug()<<"TMuSliderFrame::setSliderValue() START | _slider value set to "<<int(_slider->maximum() * dVal / (_value->max() - _value->min()))<<"\n";
#endif
}

void TMuSliderFrame::setValue(const int iVal)
{
#ifdef QDEBUG_ALL
   qDebug()<<"TMuSliderFrame::setValue() START | _value->min() = "<<_value->min()<<" | _value->max() = "<<_value->max()<<" | iVal = "<<iVal<<" | _slider->max() = "<<_slider->maximum()<<"\n";
#endif
   const double d = _value->min() + iVal * (_value->max() - _value->min())/_slider->maximum();
   _value->setValue(d);
#ifdef QDEBUG_ALL
   qDebug()<<"TMuSliderFrame::setValue() END | _value = "<<d<<"\n";
#endif
}

void TMuSliderFrame::showRangeWarningLabel(const bool show)
{
   if (show) _rangeWarningLabel->show();
   else _rangeWarningLabel->hide();
}

void TMuSliderFrame::showNanWarningLabel(const bool show)
{
   if (show) _nanWarningLabel->show();
   else _nanWarningLabel->hide();
}
