#include "minMaxDoubleValue.h"

#include <QLineEdit>
#include <QString>
#include <QKeyEvent>

#include <QDebug>

MinMaxDoubleValue::MinMaxDoubleValue() : QLineEdit()
{
   connect(this,&QLineEdit::returnPressed,
           this,&MinMaxDoubleValue::verifyAndSubmitData);
   connect(this,&QLineEdit::textChanged,
           this,&MinMaxDoubleValue::setModified);
}

void MinMaxDoubleValue::setModified(const QString &)
{
   _modified = true;
   setStyleSheet("color:red;");
}

void MinMaxDoubleValue::enterData(const double d)
{
   const QString dString = QString::number(d);
   setText(dString);
   setStyleSheet("color:black;");
   _setValue = d;
   _modified = false;
}

void MinMaxDoubleValue::verifyAndSubmitData()
{
   if (text().isEmpty()) return;
   
   bool ok;
   double d = text().toDouble(&ok);
   if (ok)
   {
      _setValue = d;
      _modified = false;
      setText(QString::number(_setValue,'g',4));
      setStyleSheet("color:black;");
      emit valueChanged(d);
   }
   else
   {
      qDebug()<<"MinMaxDoubleValue::verifyAndSubmitData | ERROR - conversion from QString to double failed\n";
      setStyleSheet("color:red;");
      //setText(QString::number(_setValue,'g',4));
   }
}

void MinMaxDoubleValue::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Escape)
   {
      setText(QString::number(_setValue,'g',4));
      setStyleSheet("color:black;");
   }
   QLineEdit::keyPressEvent(event);   
}

