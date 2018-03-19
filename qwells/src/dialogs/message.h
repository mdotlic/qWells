#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
class QWells;
class QWidget;

class Message
{
public:
   Message(QWells*);
   void newError(const QString text = "Unknown error");
   void newWarning(const QString text = "Unknown error");
   int newModalWarning(QWidget*, const QString);
private:
   QWells * _qWells;
   QWidget * _modalParent=nullptr;
};
#endif // MESSAGE_H
   
