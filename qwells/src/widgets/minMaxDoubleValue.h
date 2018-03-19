#ifndef MINMAXDOUBLEVALUE_H
#define MINMAXDOUBLEVALUE_H

#include <QLineEdit>
class QString;
class QKeyEvent;

class MinMaxDoubleValue : public QLineEdit
{
   Q_OBJECT
public:
   MinMaxDoubleValue();
   void setModified(const QString &);
   void enterData(const double d);
   void verifyAndSubmitData();
   void keyPressEvent(QKeyEvent*);
   
signals:
   void valueChanged(double);
   
private:
   double _setValue;
   bool _modified = false;
};
#endif // MINMAXDOUBLEVALUE_H