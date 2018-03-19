#include "centralWindow.h"
#include "qWells.h"
#include "engine/superPos.h"
#include "engine/modelInformation.h"
#include "engine/data.h"
#include "parameterTab/parameterTab.h"
#include "parameterTab/wellPiezoTab.h"
#include "parameterTab/wellsTab/wellsTab.h"
#include "parameterTab/piezoTab/piezoTab.h"
#include "interactionTab/interactionTab.h"
#include "printTab/printTab.h"
#include "visualizationTab/visualizationTab.h"
#include "result/result.h"

#include <QTabWidget>
#include <QString>
#include <QSplitter>
#include <QStatusBar>

#include <QColor>
#include <QPalette>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

CentralWindow::CentralWindow(QWells * qWells) : QTabWidget(), _qWells(qWells)
{
   // initialize result, needed for initialization of T & mu:
   _result = new Result(0,0,_qWells->data());
   QString parameterString = QString("PARAMETERS");
   QString interactionString = QString("INTERACTION TAB");
   QString printString = QString("PRINT TAB");
   QString visualizationString = QString("VIZUALIZATION");
   _parameterTab = new ParameterTab(_qWells);
   _parameterTab->setFocusPolicy(Qt::ClickFocus);
   _interactionTab = new InteractionTab(_qWells->data(),_result);
   _interactionTab->setFocusPolicy(Qt::ClickFocus);
   _printTab = new PrintTab(_qWells->data());
   _printTab->setFocusPolicy(Qt::ClickFocus);
   _visualizationTab = new VisualizationTab(_qWells->data());
   _visualizationTab->setFocusPolicy(Qt::ClickFocus);
   addTab(_parameterTab,parameterString);
   addTab(_interactionTab,interactionString); 
   addTab(_printTab, printString);
   addTab(_visualizationTab, visualizationString);
   
   connect(_parameterTab->wellPiezoTab()->wellsTab(),&WellsTab::loadInteractionTab,
           this,&CentralWindow::handleLoadingSignals);
   connect(_parameterTab->wellPiezoTab()->piezoTab(),&PiezoTab::loadInteractionTab,
           this,&CentralWindow::handleLoadingSignals);
   connect(this,&QTabWidget::currentChanged,this,&CentralWindow::handleTabBarClicks);
   
}

// when both wellsTab and piezoTab notify this class that they have finished loading the model, interactionTab and printTab are reloaded too
void CentralWindow::handleLoadingSignals(int tabID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"CentralWindow::handleLoadingSignals() START | tabID = "<<tabID<<"\n";
#endif
   
   _actOnLoad[tabID] = true;
   if (_actOnLoad[0] && _actOnLoad[1])
   {
#ifdef QDEBUG_ALL
   qDebug()<<"CentralWindow::handleLoadingSignals() START | restarting interaction and print tabs\n";
#endif
      removeTab(1);
      delete _interactionTab;
      delete _visualizationTab;
//      removeTab(2);
      //delete _printTab;
      //_result->resizeResult();
      initialize();
      _initializeOnInteraction = true;
      _interactionTab = new InteractionTab(_qWells->data(), _result);
      _interactionTab->setFocusPolicy(Qt::ClickFocus);
      _interactionTab->splot()->setAllData(_qWells->data()->timeInSMHD(),_result);
      _qWells->data()->getModel().clearWellPiez();
      _visualizationTab = new VisualizationTab(_qWells->data());
      _visualizationTab->setFocusPolicy(Qt::ClickFocus);
      const size_t timeUnit = _qWells->data()->getModel().timeUnit();
      emit _qWells->data()->timeUnitSet(timeUnit);
      //_printTab = new PrintTab(_qWells->data());
      //_printTab->setFocusPolicy(Qt::ClickFocus);
      QString interactionString = QString("INTERACTION TAB");
      //QString printString = QString("PRINT TAB");
      insertTab(1,_interactionTab,interactionString); 
      //addTab(_printTab,printString);
      addTab(_visualizationTab, "VISUALIZATION");

      _actOnLoad[0] = false;
      _actOnLoad[1] = false;
   }
#ifdef QDEBUG_ALL
   qDebug()<<"CentralWindow::handleLoadingSignals() END\n";
#endif
}

void CentralWindow::initialize()
{
   QString qstr;
   if(!_qWells->data()->isEverythingSet())
   {
      setCurrentIndex(0);
      return;
   }
   _qWells->data()->t().clear();
   if(_qWells->data()->noOfWells()==0)
      _qWells->error(" There is no wells in the model. Calculation is not performed!");
   else if(_qWells->data()->getBoundary().type() == 
         std::numeric_limits<size_t>::max())
      _qWells->error(" Boundary type is not set. Calculation is not performed!");
   else if((_qWells->data()->getBoundary().type()==0 ||
            (_qWells->data()->getBoundary().refWell() != 
             std::numeric_limits<size_t>::max() && 
             _qWells->data()->getBoundary().alpha() != 
             std::numeric_limits<double>::max())) && 
         !(_qWells->data()->getBoundary().type()==3 && 
            _qWells->data()->getBoundary().beta() ==
            std::numeric_limits<double>::max() ))
   {
      try{
         SuperPos::makeTimes(_qWells->data()->wells(),
               _qWells->data()->piezometers(),_qWells->data()->t(), 
               _qWells->data()->timeInSMHD(),
               _qWells->data()->getModel().timeUnit());
      }catch(const QString& err){ 
         _qWells->warning(err);
      }

      _qWells->data()->fictiveWells().clear();
      try{
         SuperPos::makeFictiveWells(_qWells->data()->wells(), 
               _qWells->data()->getBoundary(), 
               _qWells->data()->fictiveWells());
      }catch(const QString& err){ 
         _qWells->warning(err); 
      }

      //delete _result;
      _result->resizeResult();
      _result->recalculateAll();
      if (!(_actOnLoad[0] && _actOnLoad[1]))
         _interactionTab->splot()->setAllData(_qWells->data()->timeInSMHD(),_result);
      //_qWells->data())
      //= new Result(_qWells->data()->noOfWells(), 
      // _qWells->data()->noOfPiezometers(), _qWells->data());
      //emit resultMade();
      _initializeOnInteraction = false;
      _printTab->removeGraphs();
   }else if(_qWells->data()->getBoundary().refWell() == 
         std::numeric_limits<size_t>::max())
      _qWells->error(" Referent well is not set. Calculation is not performed!");
   else if(_qWells->data()->getBoundary().alpha() == 
         std::numeric_limits<double>::max())
      _qWells->error(" Angle is not set. Calculation is not performed!");
   else if(_qWells->data()->getBoundary().type()==3 && 
         _qWells->data()->getBoundary().beta() ==
         std::numeric_limits<double>::max())
      _qWells->error(" Beta is not set. Calculation is not performed!");
   else
      _qWells->error(" Something is wrong!");
}

void CentralWindow::handleTabBarClicks(const int tabID)
{
   _qWells->statusBar()->showMessage(" ");
   if (tabID == 1 && _initializeOnInteraction) // interactionTab clicked
   {
      initialize();     
   }else if(tabID == 0 && !_initializeOnInteraction){
      _printTab->removeGraphs();
      _initializeOnInteraction = true;
   }else if(tabID == 2)
   {
      _printTab->refresh(_interactionTab->splot(), _result);
   }else if(tabID == 3)
   {
      if (_initializeOnInteraction)
         initialize();
      _visualizationTab->refresh();
   }else
      _printTab->removeGraphs();
}
