#include "mainMenu.h"
#include "qWells.h"
#include "settings/timeUnitSettings.h"
#include "settings/tableSettings.h"
#include "settings/plotSettings.h"
#include "dialogs/about.h"
#include "engine/data.h"
#include "io.h"
#include "result/finalParameters.h"
#include "result/staticLevel.h"
#include "centralWindow.h"
#include "interactionTab/interactionTab.h"
#include "interactionTab/tabbedTools/tabbedTools.h"
#include "interactionTab/tabbedTools/chooserTab/plotChooser.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QDebug>

MainMenu::MainMenu(QWells * qWells) : QMenuBar(qWells), _qWells(qWells)
{   
   createActions();
   createMenus();
}

bool MainMenu::maybeSave()
{
   if (_dataModified) 
   {
      QMessageBox::StandardButton ret;
      ret = QMessageBox::warning(this, tr("Application"),
            tr("The document has been modified.\n"
                         "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      if (ret == QMessageBox::Save) return save();
      //else if (ret == QMessageBox::Discard) QApplication::quit();
      else if (ret == QMessageBox::Cancel) return false;
   }
   return true;
}

void MainMenu::newFile()
{
   if (maybeSave()) 
   {
      _qWells->clearModel();
      _qWells->timeUnitSettings()->timeUnitIsSet(false);
      _qWells->setWindowTitle(_qWells->title());
   }
}

void MainMenu::open()
{
   if (maybeSave()) 
   {
      QString fileName = QFileDialog::getOpenFileName(this,
               tr("Import model"),".",tr("Model Files (*.wjci)"));
      if (!fileName.isEmpty()) 
      {
         _qWells->clearModel();
         loadFile(fileName);
         QString title = _qWells->title();
         _qWells->setWindowTitle(title.append(" - ").append(fileName));
         emit _qWells->modelLoaded();//FIXME dataModified sets to true!!!
      }
      //qDebug()<<"MainMenu::open() | file "<<fileName<<" opened\n";
   }
}

void MainMenu::openDat()
{
   if (maybeSave()) 
   {
      QString fileName = QFileDialog::getOpenFileName(this,
               tr("Import old .dat file"),".",tr("qWells Files (*.dat)"));
      if (!fileName.isEmpty()) 
      {
         _qWells->clearModel();
         loadOldDat(fileName);
         QString title = _qWells->title();
         _qWells->setWindowTitle(title.append(" - ").append(fileName));
         emit _qWells->modelLoaded();//FIXME dataModified sets to true!!!
      }
      //qDebug()<<"MainMenu::openDat() | file "<<fileName<<" opened\n";
   }
}

void MainMenu::writeResult()
{
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Write result"),".",tr("Result Files (*.rez)"));

   if(!fileName.endsWith(".rez"))
      fileName.append(".rez");
   QFile file(fileName);
   
   if (!file.open(QFile::WriteOnly | QFile::Text)) 
   {
      QMessageBox::warning(this, tr("qWells"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
      return;
   }
   try{
      io::writeOutput(fileName, _qWells->data()->wells(), 
         _qWells->data()->fictiveWells(),
         _qWells->data()->piezometers(), _qWells->data()->getModel(), 
         _qWells->data()->t(), _qWells->data()->timeInSMHD());
   }catch(const QString & err){
      _qWells->error(err);
   }
   _qWells->statusBar()->showMessage(tr("Result written"), 2000);
}

bool MainMenu::save()
{
   if (_currentFile.isEmpty()) 
   {
      return saveAs();
   } 
   else 
   {
      return saveFile(_currentFile);
   }
   return true;
}

bool MainMenu::saveAs()
{
   //QFileDialog * fileDialog(this);
   //fileDialog->setDefaultSuffix("wjci");
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Save model"),".",tr("Model Files (*.wjci)"));
   if (fileName.isEmpty()) return false;
   if(!fileName.endsWith(".wjci"))
      fileName.append(".wjci");
   return saveFile(fileName);
}
 
void MainMenu::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
       QMessageBox::warning(this, tr("Application"),
                               tr("Cannot read file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
       return;
    }
// 
//    QTextStream in(&file);
// #ifndef QT_NO_CURSOR
//    QApplication::setOverrideCursor(Qt::WaitCursor);
// #endif
//    textEdit->setPlainText(in.readAll());
// #ifndef QT_NO_CURSOR
//    QApplication::restoreOverrideCursor();
// #endif

    try{
       io::openFile(fileName, _qWells->data()->wells(), 
         _qWells->data()->piezometers(), _qWells->data()->getModel(), 
         _qWells->data()->getBoundary(), 
         _qWells->data()->getModel().setLineWidth());
    }catch(const QString & err){
       _qWells->error(err);  
       _qWells->data()->wells().clear();
       _qWells->data()->piezometers().clear();
       _qWells->data()->getBoundary().clear();
       return;
    }
    setCurrentFile(fileName);
    _qWells->statusBar()->showMessage(tr("File loaded"), 2000);
    _dataModified = false;
    _qWells->timeUnitSettings()->timeUnitIsSet(true);
}

void MainMenu::loadOldDat(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
       QMessageBox::warning(this, tr("Application"),
                               tr("Cannot read file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
       return;
    }

    try{
    io::readDat(fileName, _qWells->data()->wells(), 
         _qWells->data()->piezometers(), _qWells->data()->getModel(), 
         _qWells->data()->getBoundary(), _qWells->data()->t());
    }catch(const QString & err){
       _qWells->error(err);
       _qWells->clearModel();
    }
    _qWells->statusBar()->showMessage(tr("File loaded"), 2000);
    _dataModified = false;
    _qWells->timeUnitSettings()->timeUnitIsSet(true);
}

bool MainMenu::saveFile(const QString &fileName)
{
   QFile file(fileName);
   if (!file.open(QFile::WriteOnly | QFile::Text)) 
   {
      QMessageBox::warning(this, tr("qWells"),
                              tr("Cannot write file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
      return false;
   }
   

//    QTextStream out(&file);
// #ifndef QT_NO_CURSOR
//    QApplication::setOverrideCursor(Qt::WaitCursor);
// #endif
//    out << textEdit->toPlainText();
// #ifndef QT_NO_CURSOR
//    QApplication::restoreOverrideCursor();
// #endif

   try{
      io::saveFile(fileName, _qWells->data()->wells(), 
         _qWells->data()->piezometers(), _qWells->data()->getModel(), 
         _qWells->data()->getBoundary(), _qWells->centralWindow()->interactionTab()->tabbedTools()->chooserTab()->wellFrames(), 
         _qWells->centralWindow()->interactionTab()->tabbedTools()->chooserTab()->piezFrames(), _qWells->centralWindow()->interactionTab()->splot()->lineWidth());
   }catch(const QString & err){
      _qWells->error(err);
   }


   setCurrentFile(fileName);
   _qWells->statusBar()->showMessage(tr("File saved"), 2000);

   _dataModified = false;
   return true;
   //return true;
}

void MainMenu::setCurrentFile(const QString &fileName)
{
   _currentFile = fileName;
   setWindowModified(false);

   QString shownName = _currentFile;
   if (_currentFile.isEmpty()) shownName = "untitled.txt";
   _qWells->setWindowFilePath(shownName);
}

void MainMenu::readSettings()
{
//    QSettings settings("Trolltech", "Application Example");
//    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
//    QSize size = settings.value("size", QSize(400, 400)).toSize();
//    resize(size);
//    move(pos);
}

void MainMenu::writeSettings()
{
//    QSettings settings("Trolltech", "Application Example");
//    settings.setValue("pos", pos());
//    settings.setValue("size", size());
}

void MainMenu::importWell()
{
   QString fileName = QFileDialog::getOpenFileName(this,
         tr("Import well"),".",tr("Well Files (*.well)"));
   if (!fileName.isEmpty())
   {
      Well well;
      try{
         well=io::importWell(fileName, _qWells->data()->getModel().timeUnit());
      }catch(const QString & err){
         _qWells->error(err);
         return;
      }
      _qWells->data()->wells().push_back(well);
      _qWells->data()->notifyOfWellImport();
      _qWells->timeUnitSettings()->timeUnitIsSet(true);
   }
}

void MainMenu::importPiezometer()
{
   QString fileName = QFileDialog::getOpenFileName(this,
         tr("Import piezometer"),".",tr("Piezo Files (*.piez)"));
   if (!fileName.isEmpty())
   {
      Piezometer piez;
      try{
         piez=io::importPiez(fileName, _qWells->data()->getModel().timeUnit());
      }catch(const QString & err)
      {
         _qWells->error(err);
         return;
      }
      _qWells->data()->piezometers().push_back(piez);
      _qWells->data()->notifyOfPiezometerImport();
      _qWells->timeUnitSettings()->timeUnitIsSet(true);
   }
}

void MainMenu::exportWell(const size_t i)
{
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Export well"),_qWells->data()->wells()[i].name(),tr("Well Files (*.well)"));
   if(!fileName.endsWith(".well"))
      fileName.append(".well");

   if (fileName.isEmpty())
      return;
   try{
      io::exportWell(fileName, _qWells->data()->wells()[i], 
            _qWells->data()->getModel().timeUnit());
   }catch(const QString & err){
      _qWells->error(err);
   }
}

void MainMenu::exportPiezometer(const size_t i)
{
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Export piezometer"),_qWells->data()->piezometers()[i].name(),
               tr("Piezo Files (*.piez)"));
   
   if(!fileName.endsWith(".piez"))
      fileName.append(".piez");
   
   if (fileName.isEmpty())
      return;
      
   try{
      io::exportPiez(fileName, _qWells->data()->piezometers()[i], 
            _qWells->data()->getModel().timeUnit());
   }catch(const QString & err){
      _qWells->error(err);
   }
}

void MainMenu::setDataModified(const bool yesNo)
{
   _dataModified = yesNo;
   _qWells->timeUnitSettings()->timeUnitIsSet(yesNo);
}

// SETTINGS MENU:

void MainMenu::tableSettings()
{
   // NOT IMPLEMENTED YET
}

// HELP MENU :

void MainMenu::showHelp()
{
   
}

void MainMenu::showAbout()
{
   About about(_qWells);
}

void MainMenu::finalParameters()
{
   FinalParameters f(_qWells);   
}

void MainMenu::setStaticLevel()
{
   StaticLevel s(_qWells);
}

// PRIVATE

void MainMenu::createActions()
{
   _newAct = new QAction(QIcon::fromTheme("document-new",QIcon(":/resources/images/folder.png")), tr("&New"), this);
//    _newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
   _newAct->setShortcuts(QKeySequence::New);
   _newAct->setStatusTip(tr("Create a new file"));
   connect(_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

   _openAct = new QAction(QIcon::fromTheme("document-open", QIcon(":/resources/images/new_page.png")), tr("&Open..."), this);
   _openAct->setShortcuts(QKeySequence::Open);
   _openAct->setStatusTip(tr("Open an existing file"));
   connect(_openAct, SIGNAL(triggered()), this, SLOT(open()));
     
   _readDatAct = new QAction(QIcon(":/images/open.png"), tr("Load old &dat file..."), this);
   _readDatAct->setShortcut(QKeySequence(tr("Ctrl+D", "File|Open")));
   _readDatAct->setStatusTip(tr("Load an old .dat file"));
   connect(_readDatAct, SIGNAL(triggered()), this, SLOT(openDat()));
     
   _saveAct = new QAction(QIcon::fromTheme("document-save",QIcon(":/resources/images/save.png")), tr("&Save..."), this);
   _saveAct->setShortcuts(QKeySequence::Save);
   _saveAct->setStatusTip(tr("Save to a .wjci file"));
   connect(_saveAct, SIGNAL(triggered()), this, SLOT(save()));
     
   _saveAsAct = new QAction(QIcon::fromTheme("document-save-as"), tr("&Save As..."), this);
   _saveAsAct->setShortcuts(QKeySequence::SaveAs);
   _saveAsAct->setStatusTip(tr("Save as"));
   connect(_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));
     
   _quitAct = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit..."), this);
   _quitAct->setShortcuts(QKeySequence::Quit);
   _quitAct->setStatusTip(tr("Quit qWells"));
   connect(_quitAct, SIGNAL(triggered()), this, SLOT(quit()));

   _writeResultAct = new QAction(QIcon(":/resources/images/contract11.png"), tr("&Write to a file..."), this);
//   _saveAct->setShortcuts(QKeySequence::Save);
   _writeResultAct->setStatusTip(tr("Save to a .rez file"));
   connect(_writeResultAct, SIGNAL(triggered()), this, SLOT(writeResult()));

   _staticLevelAct = new QAction(tr("Set static level"), this);
   connect(_staticLevelAct, SIGNAL(triggered()), this, SLOT(setStaticLevel()));

   _finalParametersAct = new QAction(tr("See final parameters"), this);
   connect(_finalParametersAct, SIGNAL(triggered()), this, SLOT(finalParameters()));
   
   _forecastAct = new QAction(tr("Go to forecast"), this);
   connect(_forecastAct, SIGNAL(triggered()), this, SLOT(forecast()));
   
   _timeUnitSettingsAct = new QAction(QIcon(":/resources/images/clock.png"), tr("Time unit settings"), this);
   connect(_timeUnitSettingsAct, SIGNAL(triggered()), _qWells->timeUnitSettings(), SLOT(checkIfSet()));
   
   _tableSettingsAct = new QAction(/*QIcon::fromTheme("preferences-system",*/QIcon(":/resources/images/chart.png"), tr("Table settings"), this);
   connect(_tableSettingsAct, SIGNAL(triggered()), _qWells->tableSettings(), SLOT(startDialog()));

 //  _plotSettingsAct = new QAction(/*QIcon::fromTheme("applications-graphics",*/QIcon(":/resources/images/chart1.png"), tr("Plot settings"), this);
   //connect(_plotSettingsAct, SIGNAL(triggered()), _qWells->plotSettings(), SLOT(startDialog()));
   
   _helpAct = new QAction(QIcon::fromTheme("help-contents",QIcon(":/resources/images/help.png")), tr("qWells help"), this);
   connect(_helpAct, SIGNAL(triggered()), this, SLOT(showHelp()));
   
   _aboutAct = new QAction(QIcon::fromTheme("help-about",QIcon(":/resources/images/info.png")), tr("About"), this);
   connect(_aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));
   
   
}

void MainMenu::createMenus()
{
   _fileMenu = addMenu(tr("File"));
   _fileMenu->addAction(_newAct);
   _fileMenu->addAction(_openAct);
   _fileMenu->addAction(_readDatAct);
   _fileMenu->addAction(_saveAct);
   _fileMenu->addAction(_saveAsAct);
   _fileMenu->addSeparator();
   _fileMenu->addAction(_quitAct);
   _settingsMenu = addMenu(tr("Settings"));
   _settingsMenu->addAction(_timeUnitSettingsAct);
   _settingsMenu->addAction(_tableSettingsAct);
   //_settingsMenu->addAction(_plotSettingsAct);
   _resultMenu = addMenu(tr("Results"));
   _resultMenu->addAction(_writeResultAct);
   _resultMenu->addAction(_staticLevelAct);
   _resultMenu->addAction(_finalParametersAct);
   _resultMenu->addAction(_forecastAct);
   _helpMenu = addMenu(tr("Help"));
   _helpMenu->addAction(_helpAct);
   _helpMenu->addAction(_aboutAct);
}

void MainMenu::quit()
{
   if(maybeSave())
   {
      QApplication::quit();
   }
  /* if (_dataModified)
   {
      QMessageBox::StandardButton ret;
      ret = QMessageBox::warning(this, tr("Application"),
                                 tr("The document has been modified.\n"
                         "Do you want to save your changes?"),
                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
      if (ret == QMessageBox::Save)
      {
         save();
         QApplication::quit();
      }else if(ret == QMessageBox::Discard)
         QApplication::quit();
   }else{
      QMessageBox::StandardButton ret;
      ret = QMessageBox::warning(this, tr("QWells"),tr("Quit QWells?"),
            QMessageBox::Yes | QMessageBox::Cancel);
      if (ret == QMessageBox::Yes) QApplication::quit();
   }*/
}

void MainMenu::forecast()
{
   QMessageBox::StandardButton ret;
   ret = QMessageBox::warning(this, tr("QWells"),tr("This will erase all measured data for wells and piezometers. Do you want to save file before that?"),
            QMessageBox::Yes | QMessageBox::No);
   if (ret == QMessageBox::Yes)
   {
      saveAs();                  
   }
      
   ret = QMessageBox::warning(this, tr("QWells"),
         tr("Please save forecast file!"), QMessageBox::Ok);

   QString defaultName = _currentFile;
   defaultName.truncate(_currentFile.size()-5);
   

   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Save model"), defaultName.append("Forecast"),
               tr("Model Files (*.wjci)"));
   if (fileName.isEmpty()) 
      return;
   if(!fileName.endsWith(".wjci"))
      fileName.append(".wjci");

   int nwells=_qWells->data()->wells().size();
   for(int iwell=0;iwell<nwells;iwell++)
   {
      _qWells->data()->wells()[iwell].Sm().clear();
      _qWells->data()->wells()[iwell].q().clear();
      _qWells->centralWindow()->interactionTab()->tabbedTools()->chooserTab()->wellFrames()[iwell]->checkBox()->setChecked(false);
   }
   _qWells->data()->fictiveWells().clear();

   int npiez=_qWells->data()->piezometers().size();
   for(int iwell=0;iwell<npiez;iwell++)
   {
      _qWells->data()->piezometers()[iwell].Sm().clear();
      _qWells->centralWindow()->interactionTab()->tabbedTools()->chooserTab()->piezFrames()[iwell]->checkBox()->setChecked(false);
   }

   saveFile(fileName);

   _qWells->clearModel();
   loadFile(fileName);
   QString title = _qWells->title();
   _qWells->setWindowTitle(title.append(" - ").append(_currentFile).append("Forecast"));
   emit _qWells->modelLoaded();//FIXME dataModified sets to true!!!
   _qWells->timeUnitSettings()->timeUnitIsSet(false);

}
