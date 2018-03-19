#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include <QString>
class QString;
class QWidget;
class QKeyEvent;
class QMouseEvent;

class LineEdit : public QLineEdit
{
   Q_OBJECT
public:
   LineEdit(int,int,QWidget * parent = 0);
   
   void enterData(const QString&);
   
   void keyPressEvent(QKeyEvent*);
   void focusInEvent(QFocusEvent*);
   //void focusOutEvent(QFocusEvent*);
   //void mousePressEvent(QMouseEvent *);
   void increaseRowByOne(const int);
   void setRow(const int);
   
   int getColumn() { return _col; }
   int getRow() { return _row; }
   
signals:
   //void mouseClicked(int,int);
   void getData(int,int);
   void deleteData(int,int);
   void inFocus(int,int);
   void pasteData();
   
public slots:
   void setModified(const QString &);
   void reduceRowByOne(const int);
   
private:
   int _row = -1;
   int _col = -1;
   bool _modified = false;
};
#endif // LINEEDIT_H