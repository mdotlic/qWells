#ifndef POTENTIALCOMBOBOX_H
#define POTENTIALCOMBOBOX_H

#include <QComboBox>

class PotentialComboBox : public QComboBox
{
   Q_OBJECT
public:
   PotentialComboBox(const int);
   
signals:
   void activatedWithID(int,int);
   
private slots:
   void sendWithID(int);
   
private:
   int _potentialID;
};
#endif // POTENTIALCOMBOBOX_H