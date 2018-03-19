#ifndef TIMEUNITSETTINGS_H
#define TIMEUNITSETTINGS_H

#include <QObject>
#include <QMessageBox>
class QWells;

class TimeUnitSettings : public QObject
{
   Q_OBJECT
public:
   TimeUnitSettings(QWells*);
   
   void startDialog();
   
   void timeUnitIsSet(const bool);
   bool isTimeUnitSet() { return _timeUnitsSet; }
   
public slots:
   void checkIfSet();
   
private:
   QWells * _qWells;
   bool _timeUnitsSet;
};
#endif //  TIMEUNITSETTINGS_H