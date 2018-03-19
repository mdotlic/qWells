#ifndef CENTRALWINDOW_H
#define CENTRALWINDOW_H

#include <QTabWidget>
class QWells;
class ParameterTab;
class InteractionTab;
class PrintTab;
class VisualizationTab;
class Result;

class CentralWindow : public QTabWidget
{
   Q_OBJECT
public:
   CentralWindow(QWells *);
   
   QWells * qWells() { return _qWells; }
   Result * result() { return _result; }
   InteractionTab * interactionTab() {return _interactionTab;}
   VisualizationTab * visualizationTab() {return _visualizationTab;}
   PrintTab * printTab() {return _printTab;}
   void initialize();
      
signals:
   void resultMade();
   
public slots:
   void handleLoadingSignals(int);
   void handleTabBarClicks(int);
   
private:
   QWells * _qWells;
   ParameterTab * _parameterTab;
   InteractionTab * _interactionTab;
   VisualizationTab * _visualizationTab;
   PrintTab * _printTab;
   bool _actOnLoad[2] = {false,false};
   bool _initializeOnInteraction = true;
   Result * _result=nullptr;
};
#endif
