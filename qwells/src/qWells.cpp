#include "qWells.h"
#include "centralWindow.h"
#include "mainMenu.h"
#include "settings/settings.h"
#include "settings/tableSettings.h"
#include "settings/timeUnitSettings.h"
#include "settings/plotSettings.h"
#include "engine/data.h"
#include "dialogs/message.h"

#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QCloseEvent>
#include <QDebug>

QWells::QWells() : QMainWindow()
{
   
   _data = new Data(this);
   setWindowTitle(_title);
   
   QRect screenGeometry = QApplication::desktop()->screenGeometry();
   _settings = new Settings(this);
   _settings->getTableSettings()->setScreenGeometry(screenGeometry);
   
   _mainMenu = new MainMenu(this);
   setMenuBar(_mainMenu);
   _centralWindow = new CentralWindow(this);
   setCentralWidget(_centralWindow);
   _statusBar = new QStatusBar();
   setStatusBar(_statusBar);
   
   _message = new Message(this);
   
   show();
   int x = (screenGeometry.width() - size().width()) / 2;
   int y = (screenGeometry.height() - size().height()) / 2;
   move(x,y);
}

QWells::~QWells()
{
   delete _data;
   delete _settings;
   delete _message;
}

void QWells::clearModel()
{
   _data = new Data(this);
   delete _centralWindow;
   _centralWindow = new CentralWindow(this);
   QRect screenGeometry = QApplication::desktop()->screenGeometry();
   _settings->getTableSettings()->setScreenGeometry(screenGeometry);
   setCentralWidget(_centralWindow);
   _mainMenu->setDataModified(false);
}

void QWells::dataHasBeenModified()
{
   _mainMenu->setDataModified(true);
}

TimeUnitSettings * QWells::timeUnitSettings() 
{ return _settings->getTimeUnitSettings(); }
TableSettings * QWells::tableSettings() 
{ return _settings->getTableSettings(); }
PlotSettings * QWells::plotSettings()
{return _settings->getPlotSettings();}
   
void QWells::closeEvent (QCloseEvent * event)
{
   _mainMenu->quit();
   event->ignore();
}
