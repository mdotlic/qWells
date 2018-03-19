#ifndef ISOLIST_H
#include <QListWidget>

class IsoList : public QListWidget
{
   Q_OBJECT
public:
   IsoList();

   void keyPressEvent(QKeyEvent * event);
  
signals:
   void listChanged();
private slots:
   void listItemClick(QListWidgetItem * item);
private:
   int _clicked =-1;
};
#endif
