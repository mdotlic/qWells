#ifndef QWELLS_H
#define QWELLS_H

#include <QMainWindow>
#include <QRect>
#include "dialogs/message.h"
class Data;
class MainMenu;
class CentralWindow;
class Settings;
class TableSettings;
class TimeUnitSettings;
class PlotSettings;
class QStatusBar;


class QWells : public QMainWindow
{
   Q_OBJECT
public:
   QWells();
   ~QWells();
   
   CentralWindow * centralWindow() { return _centralWindow; }
   MainMenu * mainMenu() { return _mainMenu; }
   QStatusBar * statusBar() { return _statusBar; }
   Data * data() { return _data; }
   
   Settings * settings() { return _settings; }
   TimeUnitSettings * timeUnitSettings();
   TableSettings * tableSettings();
   PlotSettings * plotSettings();
   QString & title() { return _title; }
   
   void error(const QString& text) { return _message->newError(text); }
   void warning(const QString& text) { return _message->newWarning(text); }
   int modalWarning(QWidget * parent,const QString & text) 
                         { return _message->newModalWarning(parent,text); }
   void clearModel();
   void dataHasBeenModified();
   void closeEvent (QCloseEvent*);
signals:
   void modelLoaded();
   void doubleClickPlot(double);
   
private:
   MainMenu * _mainMenu;
   CentralWindow * _centralWindow;
   Settings * _settings;
   QStatusBar * _statusBar;
   Data * _data;
   Message * _message;
   QString _title = "qWells";
};
#endif
