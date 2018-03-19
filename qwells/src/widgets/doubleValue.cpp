#include "doubleValue.h"
#include "engine/data.h"
#include "engine/boundary.h"
#include "qWells.h"

#include <QLineEdit>
#include <QString>
#include <QFocusEvent>
#include <math.h>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif
   
DoubleValue::DoubleValue(Data * data, const QString name) : QLineEdit(), _data(data), _name(name)
{
   connect(this,&QLineEdit::returnPressed,this,&DoubleValue::verifyAndSubmitData);
   connect(this,&QLineEdit::textChanged,this,&DoubleValue::setModified);
   setAlignment(Qt::AlignCenter);
}

DoubleValue::DoubleValue(Data * data, const QString name, const int lineNo) : QLineEdit(), _data(data), _name(name), _lineNo(lineNo)
{
   connect(this,&QLineEdit::returnPressed,this,&DoubleValue::verifyAndSubmitData);
   connect(this,&QLineEdit::textChanged,this,&DoubleValue::setModified);
   setAlignment(Qt::AlignCenter);
}

void DoubleValue::setModified(const QString &)
{
   _modified = true;
   setStyleSheet("color:red;");
}

void DoubleValue::enterData(const double d)
{
   const QString dString = QString::number(d);
   setText(dString);
   setStyleSheet("color:black;");
   _setValue = d;
   _modified = false;
}

void DoubleValue::verifyAndSubmitData()
{
   if (text().isEmpty()) return;
   
   bool ok;
   double d = text().toDouble(&ok);
   if (ok)
   {
      _setValue = d;
      _modified = false;
      setStyleSheet("color:black;");
      if (_name == "alpha") 
      {
#ifdef QDEBUG_ALL
         qDebug()<<"DoubleValue() | Setting alpha = "<<d<<"\n";
#endif
         _data->setAngle(0,d);
      }
      else if (_name == "beta")  
      {
#ifdef QDEBUG_ALL
         qDebug()<<"DoubleValue() | Setting beta = "<<d<<"\n";
#endif
         if(d>=180.0 || d<=0.0)
            _data->qWells()->error("beta must be bigger than 0 and less than 180!");
         else if (fmod(360,d)==0) _data->setAngle(1,d);
         else _data->qWells()->error("360/beta must be an integer!");  
      }
      else if (_name == "distance") 
      {
#ifdef QDEBUG_ALL
         qDebug()<<"DoubleValue() | Setting rho["<<_lineNo<<"] = "<<d<<"\n";
#endif      
         _data->setDistance(_lineNo,d);
         emit newDistanceSet(_lineNo);
      }
      else if (_name == "maxDistance") 
      {
#ifdef QDEBUG_ALL
         qDebug()<<"DoubleValue() | Setting maxDistance = "<<d<<"\n";
#endif      
         _data->setMaxDistance(d);
      }
      else { assert(false); }
      
      emit focusNextItem();
   }
}

void DoubleValue::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Escape)
   {
      if (_name == "distance")
         _setValue = _data->getBoundary().rho(_lineNo);
      setText(QString::number(_setValue));
      setStyleSheet("color:black;");
   }
   QLineEdit::keyPressEvent(event);   
}

void DoubleValue::focusInEvent(QFocusEvent * event)
{
   
   QLineEdit::focusInEvent(event);
   if (_modified) setStyleSheet("color:rgb(204,0,0);");
}

void DoubleValue::focusOutEvent(QFocusEvent * event)
{
   if (_modified)
   {
      
   }
   QLineEdit::focusOutEvent(event);
}
