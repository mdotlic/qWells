#ifndef MAINMENU_H
#define MAINMENU_H

//#include <QToolBar>
#include <QMenuBar>
class QPlainTextEdit;
class QWells;
class QMenu;
class QToolBar;
class QAction;
class QComboBox;

class MainMenu : public QMenuBar
{
   Q_OBJECT
public:
   MainMenu(QWells *);
   
   void setDataModified(const bool);

   void exportWell(const size_t i);
   void exportPiezometer(const size_t i);
   void loadFile(const QString &fileName);
public slots:
   void importWell();
   void importPiezometer();
   void quit();
   
private slots:
   void newFile();
   void open();
   void openDat();
   bool save();
   bool saveAs();
   void writeResult();
   void tableSettings();
   void finalParameters();
   void showHelp();
   void showAbout();
   void setStaticLevel();
   void forecast();


private:
   QWells * _qWells;
   
   void createActions();
   void createMenus();
   void readSettings();
   void writeSettings();
   bool maybeSave();
   //void loadFile(const QString &fileName);
   void loadOldDat(const QString &fileName);
   bool saveFile(const QString &fileName);
   void setCurrentFile(const QString &fileName);

   QPlainTextEdit *textEdit;
   QString _currentFile;
   bool _dataModified = false;

   QMenu * _fileMenu; 
   QMenu * _resultMenu;
   QMenu * _settingsMenu;
   QMenu * _helpMenu;
   QAction * _newAct; 
   QAction * _openAct; 
   QAction * _readDatAct; 
   QAction * _saveAct; 
   QAction * _saveAsAct; 
   QAction * _quitAct; 
   QAction * _writeResultAct;
   QAction * _staticLevelAct;
   QAction * _finalParametersAct;
   QAction * _forecastAct;
   QAction * _timeUnitSettingsAct;
   QAction * _tableSettingsAct;
   //QAction * _plotSettingsAct;
   QAction * _helpAct;
   QAction * _aboutAct;
};
#endif
