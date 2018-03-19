#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>
class QWells;
class QDialog;
class TableSettings;
class TimeUnitSettings;
class PlotSettings;

class Settings : public QObject
{
   Q_OBJECT
public:
   Settings(QWells*);
   ~Settings();

   // dialog stuff:
   void startDialog();
   
   // settings stuff;
   
   TimeUnitSettings * getTimeUnitSettings() { return _timeUnitSettings; }
   TableSettings * getTableSettings() { return _tableSettings; }
   PlotSettings * getPlotSettings() { return _plotSettings; }
   QString getFileLocation() { return _fileLocation; }

public slots:
   void updateFileLocation(int);
   
private:
   QDialog * _dialog;
   TableSettings * _tableSettings;
   TimeUnitSettings * _timeUnitSettings;
   PlotSettings * _plotSettings;
   
   QString _fileLocation;
};
#endif // SETTINGS_H
