#ifndef STATICLEVEL_H
#define STATICLEVEL_H
#include <QDialog>
class QWells;
class QLineEdit;

class StaticLevel: public QDialog
{
   Q_OBJECT
   public:
      StaticLevel(QWells*);
   private:
      QLineEdit * _lineEdit;
   private slots:
      void staticSet(int);
};
#endif
