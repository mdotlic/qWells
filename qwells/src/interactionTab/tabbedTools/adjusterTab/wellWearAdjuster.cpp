#include "wellWearAdjuster.h"
#include "wellWearAdjuster/wearSlider.h"

#include "engine/data.h"
#include "result/result.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "widgets/tabTools/tabToolBase.h"
#include "widgets/tabTools/collapsingBar.h"
#include "interactionTab/tabbedTools/chooserTab/plotChooser.h"
#include "interactionTab/tabbedTools/chooserTab/wellChooserFrame.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <vector>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

WellWearAdjuster::WellWearAdjuster(Data * data, QWellsPlot * plot, Result * result, PlotChooser * chooser) : TabToolBase("ADJUST WELL CONDITION"), _data(data), _plot(plot), _result(result), _chooser(chooser)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellWearAdjuster START\n";
#endif
   _wellWearAdjusterLayout = new QVBoxLayout;
   _wellWearAdjusterLayout->setContentsMargins(0,0,0,0);
   _wellWearAdjusterLayout->setAlignment(Qt::AlignTop);
      
   QFrame * wellWearAdjusterFrame = new QFrame;
   wellWearAdjusterFrame->setLayout(_wellWearAdjusterLayout);
      
   _noOfWells = _data->noOfWells();
   const int noOfWells = _noOfWells; // addWell changes _noOfWells
   for (int i=0;i<noOfWells;i++) addWell(i);
   
   connect(_data,&Data::wellAdded,
           this,&WellWearAdjuster::addWell);
   connect(_data,&Data::wellRemoved,
           this,&WellWearAdjuster::removeWell);
   connect(_data,&Data::wellImported,
           this,&WellWearAdjuster::addWell);
   connect(_chooser,&PlotChooser::wellAdded,
           this,&WellWearAdjuster::showAdjuster);
   connect(_chooser,&PlotChooser::wellRemoved,
           this,&WellWearAdjuster::hideAdjuster);
   connect(_data,&Data::itemNameChanged,
           this,&WellWearAdjuster::changeWellName);
   connect(_data,&Data::bigASet,this,&WellWearAdjuster::setBigA);
   connect(_data,&Data::bigBSet,this,&WellWearAdjuster::setBigB);
   
   QScrollArea * scroll = new QScrollArea;
   scroll->setBackgroundRole(QPalette::AlternateBase);
   scroll->setFrameShape(QFrame::StyledPanel);
   //scroll->sizePolicy().setVerticalPolicy(QSizePolicy::Fixed);
   layout()->addWidget(scroll,Qt::AlignTop);
   layout()->setContentsMargins(0,0,0,0);
   layout()->setSpacing(0);
   scroll->setWidget(wellWearAdjusterFrame);
   scroll->setWidgetResizable(true);
   scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
      
//    QColor bgColor = palette().color(QPalette::AlternateBase);
//    QString styleBase = "background-color: rgb(%1, %2, %3);";
//    QString style = styleBase.arg(bgColor.red()).arg(bgColor.green()).arg(bgColor.blue());
//    titleBar()->setStyleSheet(style);
   
   scroll->setVisible(false);
   connect(this->titleBar(),SIGNAL(switchVisibility(bool)),
           scroll,SLOT(setVisible(bool)));
   connect(this->titleBar(),SIGNAL(switchVisibility(bool)),
           this,SLOT(resendWithID(bool)));
#ifdef QDEBUG_ALL
   qDebug()<<"WellWearAdjuster END\n";
#endif
}

void WellWearAdjuster::addWell(const int wellID)
{
   _wearFrames.push_back(new WearSlider(_data,_plot,_result,wellID));
   _wellWearAdjusterLayout->addWidget(_wearFrames[wellID],Qt::AlignLeft);
   _wearFrames[wellID]->setFrameShape(QFrame::NoFrame);
   if(_chooser->wellFrames()[wellID]->checkBox()->isChecked())
      _wearFrames[wellID]->show();
   else
      _wearFrames[wellID]->hide();
   _noOfWells = _wearFrames.size();
}

void WellWearAdjuster::removeWell(const int wellID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellWearAdjuster::removeWell | removing wellID = "<<wellID<<"\n";
#endif
   auto it = _wearFrames.begin() + wellID;
   _wearFrames.erase(it);
   _noOfWells = _wearFrames.size();
#ifdef QDEBUG_ALL
   qDebug()<<"WellWearAdjuster::removeWell | wellID = "<<wellID<<" removed\n";
#endif
}

void WellWearAdjuster::changeWellName(const int wellID, const int id, 
      const QString & name)
{
   if(id==1)
      return;
   _wearFrames[wellID]->setName(name);
}

void WellWearAdjuster::setBigA(const int wellID)
{
   _wearFrames[wellID]->setAFromData(wellID);
}

void WellWearAdjuster::setBigB(const int wellID)
{
   _wearFrames[wellID]->setBFromData(wellID);
}

void WellWearAdjuster::showAdjuster(const int wellID)
{
   _wearFrames[wellID]->show();
}

void WellWearAdjuster::hideAdjuster(const int wellID)
{
   _wearFrames[wellID]->hide();
}

void WellWearAdjuster::resendWithID(const bool showing)
{
   emit iAmVisible(adjusterID,showing);
}
