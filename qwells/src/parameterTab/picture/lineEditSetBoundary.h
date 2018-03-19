#ifndef LINEEDITBOUNDARY_H
#define LINEEDITBOUNDARY_H
#include <QLineEdit>
class LineEditSetBoundary: public QLineEdit
{
   Q_OBJECT
   public:
      LineEditSetBoundary(int row, QWidget * parent = 0);
      void focusInEvent(QFocusEvent*);
   signals:
      void boundInFocus(int);
   private:
      int _row = -1;
};
#endif
