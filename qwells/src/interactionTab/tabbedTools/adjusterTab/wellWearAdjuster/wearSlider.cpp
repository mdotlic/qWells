#include "wearSlider.h"

#include "wearValue.h"
#include "helperFunctions.h"
#include "engine/data.h"
#include "engine/well.h"
#include "widgets/minMaxDoubleValue.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "result/result.h"

#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QString>
#include <QSpacerItem>

#include <assert.h>
#include <QDebug>
#ifndef QDEBUG_ALL
#define QDEBUG_ALL
#endif

// sliderID = 0 for boundarySlider, sliderID = 1 for wellConditionSlider
WearSlider::WearSlider(Data * data, QWellsPlot * plot, Result * result, const int wellID) : QFrame(), _data(data), _wellID(wellID)
{
   QGridLayout * sliderFrameLayout = new QGridLayout;

   _minValueA = new MinMaxDoubleValue();
   _minValueB = new MinMaxDoubleValue();
   
   _maxValueA = new MinMaxDoubleValue();
   _maxValueB = new MinMaxDoubleValue();
   
   _noOfStepsA = new QLineEdit;
   _noOfStepsB = new QLineEdit;
   
   _sliderA = new QSlider(Qt::Horizontal);
   _sliderA->setSingleStep(1);
   _sliderA->setMinimum(0);
   _sliderA->setTickPosition(QSlider::NoTicks);
   _sliderB = new QSlider(Qt::Horizontal);
   _sliderB->setSingleStep(1);
   _sliderB->setMinimum(0);
   _sliderB->setTickPosition(QSlider::NoTicks);
   
   _valueA = new WearValue(_data,plot,result,wellID,0);
   _valueB = new WearValue(_data,plot,result,wellID,1);
      
   QLabel * minLabelA = new QLabel(QString("min"));
   QLabel * minLabelB = new QLabel(QString("min"));
   QLabel * maxLabelA = new QLabel(QString("max"));
   QLabel * maxLabelB = new QLabel(QString("max"));
   QLabel * stepLabelA = new QLabel(QString("steps"));
   QLabel * stepLabelB = new QLabel(QString("steps"));
   
   _nameLabel = new QLabel(data->wells()[wellID].name());
   _nameLabel->setStyleSheet("font: 14px bold;");
   //nameLabel->setAlignment(Qt::AlignCenter);
   
   _rangeWarningLabelA = new QLabel(QString("NOT IN RANGE!"));
   _rangeWarningLabelA->setAlignment(Qt::AlignCenter);
   _rangeWarningLabelA->setStyleSheet("color:rgb(204,0,0);font:9px bold");
   _rangeWarningLabelB = new QLabel(QString("not in range!"));
   _rangeWarningLabelB->setAlignment(Qt::AlignCenter);
   _rangeWarningLabelB->setStyleSheet("color:rgb(204,0,0);font:9px bold");
   _nanWarningLabelA = new QLabel(QString("Not A Number!"));
   _nanWarningLabelA->setAlignment(Qt::AlignCenter);
   _nanWarningLabelA->setStyleSheet("color:rgb(204,0,0);font:9px bold");
   _nanWarningLabelB = new QLabel(QString("Nan!"));
   _nanWarningLabelB->setAlignment(Qt::AlignCenter);
   _nanWarningLabelB->setStyleSheet("color:rgb(204,0,0);font:9px bold");
   QVBoxLayout * warningValueLayoutA = new QVBoxLayout;
   warningValueLayoutA->addWidget(_rangeWarningLabelA,Qt::AlignTop);
   warningValueLayoutA->addWidget(_nanWarningLabelA,Qt::AlignTop);
   QVBoxLayout * warningValueLayoutB = new QVBoxLayout;
   warningValueLayoutB->addWidget(_rangeWarningLabelB,Qt::AlignTop);
   warningValueLayoutB->addWidget(_nanWarningLabelB,Qt::AlignTop);
   const int w = minLabelA->sizeHint().width();
   const int h = _rangeWarningLabelA->sizeHint().height()+15;
   QSpacerItem * spacer = new QSpacerItem(w,h,QSizePolicy::Fixed,
                                          QSizePolicy::Fixed);
   
   // FILL LAYOUT:
   sliderFrameLayout->addWidget(_nameLabel,0,2,1,3,Qt::AlignHCenter);
   
   sliderFrameLayout->addWidget(minLabelA,1,0,Qt::AlignLeft);
   sliderFrameLayout->addWidget(maxLabelA,2,0,Qt::AlignLeft);
   sliderFrameLayout->addWidget(stepLabelA,3,0,Qt::AlignLeft);
   sliderFrameLayout->addItem(spacer,4,0);
   sliderFrameLayout->addWidget(_minValueA,1,1,Qt::AlignLeft);
   sliderFrameLayout->addWidget(_maxValueA,2,1,Qt::AlignLeft);
   sliderFrameLayout->addWidget(_noOfStepsA,3,1,Qt::AlignLeft);
   QLabel * aLabel = new QLabel("A");
   aLabel->setToolTip("Linear loss [s/m"+QString(QChar(0x00B2))+"]");
   sliderFrameLayout->addWidget(aLabel,1,2,Qt::AlignCenter);
   sliderFrameLayout->addWidget(_valueA,2,2,Qt::AlignCenter);
   sliderFrameLayout->addWidget(_sliderA,3,2,Qt::AlignCenter);
   sliderFrameLayout->addLayout(warningValueLayoutA,4,2,Qt::AlignTop);
   QLabel * bLabel = new QLabel("B");
   bLabel->setToolTip("Quadratic loss [s"+QString(QChar(0x00B2))+"/m"
         +QString(QChar(0x2075))+"]");
   sliderFrameLayout->addWidget(bLabel,1,4,Qt::AlignCenter);
   sliderFrameLayout->addWidget(_valueB,2,4,Qt::AlignCenter);
   sliderFrameLayout->addWidget(_sliderB,3,4,Qt::AlignCenter);
   sliderFrameLayout->addLayout(warningValueLayoutB,4,4,Qt::AlignTop);
   sliderFrameLayout->addWidget(_minValueB,1,5,Qt::AlignRight);
   sliderFrameLayout->addWidget(_maxValueB,2,5,Qt::AlignRight);
   sliderFrameLayout->addWidget(_noOfStepsB,3,5,Qt::AlignRight);
   sliderFrameLayout->addWidget(minLabelB,1,6,Qt::AlignRight);
   sliderFrameLayout->addWidget(maxLabelB,2,6,Qt::AlignRight);
   sliderFrameLayout->addWidget(stepLabelB,3,6,Qt::AlignRight);
   
   sliderFrameLayout->setColumnStretch(3,1);
   sliderFrameLayout->setColumnStretch(7,2);
   //sliderFrameLayout->setRowStretch(5,1);
   
   _rangeWarningLabelA->hide();
   _nanWarningLabelA->hide();
   _rangeWarningLabelB->hide();
   _nanWarningLabelB->hide();
   
   setLayout(sliderFrameLayout);
   
   const int boxSize = 45;
//    const QString fontStr = QString("font:8px;"); 
   QFont f;
   f.setFamily(_noOfStepsB->font().family());
   f.setPointSize(9);
   f.setStretch(95);  
   
   _noOfStepsA->setFixedWidth(boxSize);
   _minValueA->setFixedWidth(boxSize);
   _maxValueA->setFixedWidth(boxSize);
   _minValueA->setFont(f);
   _maxValueA->setFont(f);
   _noOfStepsA->setFont(f);
   
   _valueA->setFixedWidth(boxSize+15);
   
   _noOfStepsB->setFixedWidth(boxSize);
   _minValueB->setFixedWidth(boxSize);
   _maxValueB->setFixedWidth(boxSize);
   _minValueB->setFont(f);
   _maxValueB->setFont(f);
   _noOfStepsB->setFont(f);
   _valueB->setFixedWidth(boxSize+15);
   
   setFixedHeight(sizeHint().height());
   //setMaximumHeight(150);
//    _slider->setMinimumWidth(150);
//    _slider->setMinimumHeight(40);
//    _value->setFixedSize(80,40);
//    _value->setStyleSheet("font: 22px bold;");
//    setMaximumHeight(90);
   
   setConnections();
   setInitialization();
}

void WearSlider::setConnections()
{
//    connect(_minValueA,&QLineEdit::returnPressed,
//            this,&WearSlider::setValueAMin);
//    connect(_maxValueA,&QLineEdit::returnPressed,
//            this,&WearSlider::setValueAMax);
//    connect(_noOfStepsA,&QLineEdit::returnPressed,
//            this,&WearSlider::setNoOfStepsA);
//    connect(_minValueB,&QLineEdit::returnPressed,
//            this,&WearSlider::setValueBMin);
//    connect(_maxValueB,&QLineEdit::returnPressed,
//            this,&WearSlider::setValueBMax);
   connect(_minValueA,SIGNAL(valueChanged(double)),
           this,SLOT(setValueAMin(double)));
   connect(_maxValueA,SIGNAL(valueChanged(double)),
           this,SLOT(setValueAMax(double)));
   connect(_minValueB,SIGNAL(valueChanged(double)),
           this,SLOT(setValueBMin(double)));
   connect(_maxValueB,SIGNAL(valueChanged(double)),
           this,SLOT(setValueBMax(double)));
   connect(_noOfStepsB,&QLineEdit::returnPressed,
           this,&WearSlider::setNoOfStepsB);
   connect(_valueA,&WearValue::fixMin,
           this,&WearSlider::fixMin);
   connect(_valueA,&WearValue::fixMax,
           this,&WearSlider::fixMax);
   connect(_valueB,&WearValue::fixMin,
           this,&WearSlider::fixMin);
   connect(_valueB,&WearValue::fixMax,
           this,&WearSlider::fixMax);
   
   // slider with value
   connect(_sliderA,SIGNAL(valueChanged(int)),
           _valueA,SLOT(setValueFromSlider(int)));
   connect(_valueA,SIGNAL(valueChanged(int)),
           _sliderA,SLOT(setValue(int)));
   connect(_sliderB,SIGNAL(valueChanged(int)),
           _valueB,SLOT(setValueFromSlider(int)));
   connect(_valueB,SIGNAL(valueChanged(int)),
           _sliderB,SLOT(setValue(int)));
   
   // _value with "out of range" and "Nan" warnings
   connect(_valueA,SIGNAL(showRangeWarning(bool)),
           this,SLOT(showRangeWarningLabelA(bool)));
   connect(_valueA,SIGNAL(showNanWarning(bool)),
           this,SLOT(showNanWarningLabelA(bool)));
   connect(_valueB,SIGNAL(showRangeWarning(bool)),
           this,SLOT(showRangeWarningLabelB(bool)));
   connect(_valueB,SIGNAL(showNanWarning(bool)),
           this,SLOT(showNanWarningLabelB(bool)));
}

// void WearSlider::setMinMaxStep(const double dMin, const double dMax, const int steps)
void WearSlider::setInitialization()
{
   _dMinA = 0.0;
   _dMaxA = 1000.0; // WARNING must be > _dMinA
   const int stepsA = 1000;
   
   _dMinB = 0.0;
   _dMaxB = 10000.0; // WARNING must be > _dMinB
   const int stepsB = 1000;
   
   double valueA, valueB;
   size_t vectorSize;
   vectorSize = _data->wells()[_wellID].bigA().size();
   if (vectorSize > 0) valueA = _data->wells()[_wellID].bigA(0)._y;
   else valueA = 0.0;
   vectorSize = _data->wells()[_wellID].bigB().size();
   if (vectorSize > 0) valueB = _data->wells()[_wellID].bigB(0)._y;
   else valueB = 0.0;

   initialize(_dMinA,_dMaxA,stepsA,valueA,_dMinB,_dMaxB,stepsB,valueB);
}

void WearSlider::initialize(const double iniMinA, const double iniMaxA, const int noOfStepsA, const double valueA, const double iniMinB, const double iniMaxB, const int noOfStepsB, const double valueB)
{   
   // these three don't trigger signals because there is no Return press
   _minValueA->enterData(iniMinA);
   _maxValueA->enterData(iniMaxA);
   _noOfStepsA->setText(QString(QString::number(noOfStepsA)));
   // ... so we do this instead:
   _valueA->setMin(iniMinA);
   _valueA->setMax(iniMaxA);
   _valueA->setNoOfSteps(noOfStepsA);
   _sliderA->setMaximum(noOfStepsA);
   
   // same way for B:
   _minValueB->enterData(iniMinB);
   _maxValueB->enterData(iniMaxB);
   _noOfStepsB->setText(QString(QString::number(noOfStepsB)));
   _valueB->setMin(iniMinB);
   _valueB->setMax(iniMaxB);
   _valueB->setNoOfSteps(noOfStepsB);
   _sliderB->setMaximum(noOfStepsB);
   
   // set values and sliders - we don't want to write data back to Data so we avoid procedures that do so
   int sliderValue;
   sliderValue = noOfStepsA * (valueA - iniMinA) / (iniMaxA - iniMinA);
   _valueA->setValueFromData(valueA,sliderValue);
   _sliderA->setValue(sliderValue);
   
   sliderValue = noOfStepsB * (valueB - iniMinB) / (iniMaxB - iniMinB);
   _valueB->setValueFromData(valueB,sliderValue);
   _sliderB->setValue(sliderValue);
}

void WearSlider::setValueAMin(const double dVal)
{
   _valueA->setMin(dVal);
   if (_valueA->min() == dVal) _dMinA = dVal;
}

void WearSlider::setValueBMin(const double dVal)
{
   _valueB->setMin(dVal);
   if (_valueB->min() == dVal) _dMinB = dVal;
}

void WearSlider::setValueAMax(const double dVal)
{
   _valueA->setMax(dVal);
   if (_valueA->max() == dVal) _dMaxA = dVal;
}

void WearSlider::setValueBMax(const double dVal)
{
   _valueB->setMax(dVal);
   if (_valueB->max() == dVal) _dMaxB = dVal;
}

void WearSlider::fixMin(const int valueID)
{
   if (valueID == 0)
      _minValueA->enterData(_dMinA);
   else // valueID = 1
      _minValueB->enterData(_dMinB);
}

void WearSlider::fixMax(const int valueID)
{
   if (valueID == 0)
      _maxValueA->enterData(_dMaxA);
   else // valueID = 1
      _maxValueB->enterData(_dMaxB);
}

void WearSlider::setNoOfStepsA()
{
   int noOfSteps;
   const bool ok = conversion::convertQStringToInt(_noOfStepsA->text(),noOfSteps);
   if (!ok) { qDebug()<<" string to int fail\n"; assert(false); }
   _sliderA->setMaximum(noOfSteps);
   _valueA->setNoOfSteps(noOfSteps);
}

void WearSlider::setNoOfStepsB()
{
   int noOfSteps;
   const bool ok = conversion::convertQStringToInt(_noOfStepsB->text(),noOfSteps);
   if (!ok) { qDebug()<<" string to int fail\n"; assert(false); }
   _sliderB->setMaximum(noOfSteps);
   _valueB->setNoOfSteps(noOfSteps);
}

void WearSlider::showRangeWarningLabelA(const bool show)
{
   if (show) _rangeWarningLabelA->show();
   else _rangeWarningLabelA->hide();
}

void WearSlider::showRangeWarningLabelB(const bool show)
{
   if (show) _rangeWarningLabelB->show();
   else _rangeWarningLabelB->hide();
}

void WearSlider::showNanWarningLabelA(const bool show)
{
   if (show) _nanWarningLabelA->show();
   else _nanWarningLabelA->hide();
}

void WearSlider::showNanWarningLabelB(const bool show)
{
   if (show) _nanWarningLabelB->show();
   else _nanWarningLabelB->hide();
}

void WearSlider::setName(const QString & name)
{
   _nameLabel->setText(name);
}

void WearSlider::setAFromData(const int wellID)
{
   const double valueA = _data->wells()[wellID].bigA(0)._y;
   if (valueA < _valueA->min())
   {
      _dMinA = valueA;
      fixMin(0);
      _valueA->setMin(valueA);
   }
   else if (valueA > _valueA->max())
   {
      _dMaxA = valueA;
      fixMax(0);
      _valueA->setMax(valueA);
   }
   const int sliderValue = _valueA->noOfSteps() * (valueA - _valueA->min()) / (_valueA->max() - _valueA->min());
   _valueA->setValueFromData(valueA,sliderValue);
   _sliderA->setValue(sliderValue);
}

void WearSlider::setBFromData(const int wellID)
{
   const double valueB = _data->wells()[wellID].bigB(0)._y;
   if (valueB < _valueB->min())
   {
      _dMinB = valueB;
      fixMin(1);
      _valueB->setMin(valueB);
   }
   else if (valueB > _valueB->max())
   {
      _dMaxB = valueB;
      fixMax(1);
      _valueB->setMax(valueB);
   }
   const int sliderValue = _valueB->noOfSteps() * (valueB - _valueB->min()) / (_valueB->max() - _valueB->min());
   _valueB->setValueFromData(valueB,sliderValue);
   _sliderB->setValue(sliderValue);
}
