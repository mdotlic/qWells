#ifndef DOUBLEVALUE_H
#define DOUBLEVALUE_H

#include <QLineEdit>
#include <QString>
class Data;
class QKeyEvent;
class QFocusEvent;

class DoubleValue : public QLineEdit
{
   Q_OBJECT
public:
   DoubleValue(Data*,const QString);
   DoubleValue(Data*,const QString, const int);
   
   void enterData(const double);
   
   void keyPressEvent(QKeyEvent*);
   void focusInEvent(QFocusEvent*);
   void focusOutEvent(QFocusEvent*);
   
signals:
   void focusNextItem();
   void newDistanceSet(int);
   void newMaxDistanceSet();
   
public slots:
      void verifyAndSubmitData();
      void setModified(const QString&);
      
private:
   Data * _data;
   QString _name;
   int _lineNo = -1;
   bool _modified = false;
   double _setValue;
};
#endif // DOUBLEVALUE_H