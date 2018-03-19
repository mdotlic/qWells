#include "parameterTab.h"

#include "qWells.h"
#include "wellPiezoTab.h"
#include "wellsTab/wellsTab.h"
#include "piezoTab/piezoTab.h"
#include "boundaryWidget.h"

#include <QSplitter>

// #include <QList>
// #include <QSize>

#include <QDebug>

ParameterTab::ParameterTab(QWells * qWells) : QSplitter(Qt::Vertical)
{
   _wellPiezoTab = new WellPiezoTab(qWells,this);
   addWidget(_wellPiezoTab);
   _boundaryWidget = new BoundaryWidget(_wellPiezoTab->wellsTab()->wellsTable(), _wellPiezoTab->piezoTab()->piezoTable());
//    connect(_wellPiezoTab->wellsTab(),&WellsTab::loadBoundaryWidget,
//            this,&ParameterTab::loadBoundaryWidget);
   connect(_wellPiezoTab->piezoTab(),&PiezoTab::loadBoundaryWidget,
           this,&ParameterTab::loadBoundaryWidget);
   addWidget(_boundaryWidget);
   
/*   const int iniHeight = _wellPiezoTab.size().height();
   QList<int> halfSizes;
   halfSizes<<halfHeight<<halfHeight;
   setSizes(halfSizes);*/
//    const int halfHeight = sizeHint().height()/2;
//    qDebug()<<" wellpiezoTab.sizeHint() = "<<_wellPiezoTab->sizeHint()<<"\n";
//    QList<int> halfSizes;
//    halfSizes<<halfHeight<<halfHeight;
//    setSizes(halfSizes);
//    qDebug()<<" wellpiezoTab.size() = "<<_wellPiezoTab->size()<<"\n";
   //_boundaryWidget->setMinimumHeight(_wellPiezoTab->size().height());
}

void ParameterTab::loadBoundaryWidget()
{
   delete _boundaryWidget;
   const bool modelExists = true;
   _boundaryWidget = new BoundaryWidget(_wellPiezoTab->wellsTab()->wellsTable(),  _wellPiezoTab->piezoTab()->piezoTable(), modelExists);
   addWidget(_boundaryWidget);
   const int halfHeight = size().height();
   QList<int> halfSizes;
   halfSizes<<halfHeight<<halfHeight;
   setSizes(halfSizes);
}
