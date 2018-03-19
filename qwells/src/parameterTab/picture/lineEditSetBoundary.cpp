#include "lineEditSetBoundary.h"

#include <QLineEdit>
#include <QFocusEvent>

LineEditSetBoundary::LineEditSetBoundary(int row, QWidget * parent) : QLineEdit(parent), _row(row) 
{ 
   setFocusPolicy(Qt::StrongFocus); // already done in QLineEdit's constructor?
}

void LineEditSetBoundary::focusInEvent(QFocusEvent * /* event*/)
{
   emit boundInFocus(_row);
}
