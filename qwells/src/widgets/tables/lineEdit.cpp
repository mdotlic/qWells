#include "lineEdit.h"

#include <QLineEdit>
#include <QKeyEvent>
#include <QFocusEvent>
//#include <QMouseEvent>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

LineEdit::LineEdit(int row, int col, QWidget * parent) : QLineEdit(parent), _row(row), _col(col) 
{ 
   setFocusPolicy(Qt::StrongFocus); // already done in QLineEdit's constructor?
   connect(this,&QLineEdit::textChanged,this,&LineEdit::setModified);
   setStyleSheet("border:0px;");
}

void LineEdit::enterData(const QString & str)
{
   setText(str);
   setStyleSheet("color:black;border:0px;");
   _modified = false;
}

void LineEdit::setModified(const QString &)
{
   if (_modified) return;
   _modified = true;
   setStyleSheet("color:rgb(204,0,0);;border:0px;");
}

void LineEdit::keyPressEvent(QKeyEvent * event)
{
   
   if (event->key() == Qt::Key_Escape) emit getData(_row,_col);
   else if ((event->key() == Qt::Key_Return) || (event->key() == Qt::Key_Enter))
   {
      _modified = false;
      setStyleSheet("color:black;border:0px;");
   }
   else if (event->modifiers()==Qt::ControlModifier && event->key() == Qt::Key_V) 
   {
      emit pasteData();
      return;
   }
   QLineEdit::keyPressEvent(event);
}

void LineEdit::focusInEvent(QFocusEvent * event)
{
#ifdef QDEBUG_ALL
   qDebug()<<"LineEdit::focusInEvent | this cell = ("<<_row<<","<<_col<<")\n";
#endif
   emit inFocus(_row,_col);
   QLineEdit::focusInEvent(event);
   if (_modified) setStyleSheet("color:rgb(204,0,0);border:0px;");
  // else
    //  setStyleSheet("color:black;border:0px;");
}

void LineEdit::reduceRowByOne(const int row)
{
   if (row < _row) _row = _row - 1;
}

void LineEdit::increaseRowByOne(const int row)
{
//    if (row > _row) _row = _row + 1;
   if (row > _row) _row = row;
}

void LineEdit::setRow(const int row)
{
   _row = row;
}

// void LineEdit::focusOutEvent(QFocusEvent * event)
// {
//    setText(_setValue);
//    setStyleSheet("color:black;");
//    QLineEdit::focusOutEvent(event);
// }

// void LineEdit::mousePressEvent(QMouseEvent * event)
// {
//    emit mouseClicked(_row,_col);
//    return QLineEdit::mousePressEvent(event);
// }
