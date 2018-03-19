#include "wellPiezoTab.h"
#include "wellsTab/wellsTab.h"
#include "piezoTab/piezoTab.h"
#include "tableSize.h"
#include "qWells.h"

#include <QTabWidget>

#include <QDebug>
//#include <QTabBar>

WellPiezoTab::WellPiezoTab(QWells * qWells, ParameterTab * parameterTab) : QTabWidget()
{   
   _tableSize = new TableSize(qWells->tableSettings());
   
   setTabPosition(QTabWidget::West);
   QString wellsString = QString("WELLS");
   QString piezoString = QString("PIEZOMETERS");
   _wellsTab = new WellsTab(qWells,parameterTab,_tableSize);
   _wellsTab->setFocusPolicy(Qt::ClickFocus);
   _piezoTab = new PiezoTab(qWells,_tableSize);
   _piezoTab->setFocusPolicy(Qt::ClickFocus);
   addTab(_wellsTab,wellsString);
   addTab(_piezoTab,piezoString);
//    qDebug()<<"WellPiezoTab | tab0 rect = "<<tabBar()->tabRect(0)<<" | tab1 rect = "<<tabBar()->tabRect(0)<<"\n";
//    const int vertSize = tabBar()->tabRect(0).height() + tabBar()->tabRect(1).height() + 1;
//    setMinimumHeight(vertSize);
//    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}