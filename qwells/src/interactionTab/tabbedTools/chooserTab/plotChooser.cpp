#include "plotChooser.h"
#include "wellChooserFrame.h"
#include "piezoChooserFrame.h"

#include "qWells.h"
#include "centralWindow.h"
#include "result/result.h"
#include "engine/data.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "helperFunctions.h"

#include <QScrollArea>
#include <QFrame>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPair>
#include <QString>
#include <QColor>
#include <QIcon>
#include <QCheckBox>
#include <vector>

#include <QScrollArea>
#include <QPalette>
#include <QScrollBar>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

PlotChooser::PlotChooser(Data * data, QWellsPlot * plot, Result * result) : QFrame(), _data(data), _plot(plot), _result(result)
{
   QVBoxLayout * layoutForScroll = new QVBoxLayout;
   QScrollArea * scroll = new QScrollArea;
   
   QFrame * chooserFrame = new QFrame;
   QVBoxLayout * chooserLayout = new QVBoxLayout;
   chooserLayout->setContentsMargins(0,0,0,0);
   
   makeColors();
   
   _noOfWells = _data->noOfWells();
   _noOfPiezometers = _data->noOfPiezometers();
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser START | noOfWells = "<<_noOfWells<<" | noOfPiezometers = "<<_noOfPiezometers<<"\n";
#endif
   
   QFrame * wellChooserFrame = new QFrame;
   _wellChooserLayout = new QVBoxLayout;
   _wellChooserLayout->setContentsMargins(0,0,10,0);
   
   QLabel * chooseWells = new QLabel("CHOOSE WELLS TO DISPLAY:");
   chooseWells->setStyleSheet("margin: 5px;");
   _wellChooserLayout->addWidget(chooseWells);

   const int noOfWells = _noOfWells; //  addWell changes _noOfWells
   for (int i=0;i<noOfWells;i++) addWell(i);
   
   wellChooserFrame->setLayout(_wellChooserLayout);
   chooserLayout->addWidget(wellChooserFrame);
   
   QFrame * piezoChooserFrame = new QFrame;
   _piezoChooserLayout = new QVBoxLayout;
   _piezoChooserLayout->setContentsMargins(0,0,10,0);
   
   QLabel * choosePiezometers = new QLabel("CHOOSE PIEZOMETERS TO DISPLAY:");
   choosePiezometers->setStyleSheet("margin: 5px;");
   _piezoChooserLayout->addWidget(choosePiezometers);
   
   const int noOfPiezometers = _noOfPiezometers; // addPiezometer changes _noOfPiezometers
   for (int i=0;i<noOfPiezometers;i++) addPiezometer(i);
   
   piezoChooserFrame->setLayout(_piezoChooserLayout);
   chooserLayout->addWidget(piezoChooserFrame);
   
   QWidget * spacer = new QWidget;
   chooserLayout->addWidget(spacer);
   chooserLayout->setStretchFactor(spacer,1);
   
   //chooserLayout->addStretch();
   
   chooserFrame->setFrameShape(QFrame::NoFrame);
   chooserFrame->setLayout(chooserLayout);
   scroll->setWidget(chooserFrame);
   scroll->setWidgetResizable(true);
   scroll->setBackgroundRole(QPalette::AlternateBase);
   scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   scroll->setFrameShape(QFrame::StyledPanel);
   
   
   connect(_data,&Data::wellAdded,this,&PlotChooser::addWell);
   connect(_data,&Data::piezometerAdded,this,&PlotChooser::addPiezometer);
   connect(_data,&Data::wellRemoved,this,&PlotChooser::removeWell);
   connect(_data,&Data::piezometerRemoved,this,&PlotChooser::removePiezometer);
   connect(_data,&Data::wellImported,this,&PlotChooser::addWell);
   connect(_data,&Data::piezometerImported,this,&PlotChooser::addPiezometer);
   connect(_data,&Data::itemNameChanged,this,&PlotChooser::changeItemName);
   
   layoutForScroll->addWidget(scroll,Qt::AlignTop);
   layoutForScroll->addStretch();
   setLayout(layoutForScroll);
   
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser END\n";
#endif
}

void PlotChooser::makeColors()
{
   _colors.resize(8);
   _colors[0] = qMakePair(QString("black"),qMakePair(QIcon(":resources/images/colors/black.png"),QColor(Qt::black)));
   _colors[1] = qMakePair(QString("red"),qMakePair(QIcon(":resources/images/colors/red.png"),QColor(Qt::red)));
   _colors[2] = qMakePair(QString("blue"),qMakePair(QIcon(":resources/images/colors/blue.png"),QColor(Qt::blue)));
   _colors[3] = qMakePair(QString("green"),qMakePair(QIcon(":resources/images/colors/green.png"),QColor(Qt::green)));
   _colors[4] = qMakePair(QString("yellow"),qMakePair(QIcon(":resources/images/colors/yellow.png"),QColor(Qt::yellow)));
   _colors[5] = qMakePair(QString("cyan"),qMakePair(QIcon(":resources/images/colors/cyan.png"),QColor(Qt::cyan)));
   _colors[6] = qMakePair(QString("magenta"),qMakePair(QIcon(":resources/images/colors/magenta.png"),QColor(Qt::magenta)));
   _colors[7] = qMakePair(QString("gray"),qMakePair(QIcon(":resources/images/colors/gray.png"),QColor(Qt::gray)));
}

void PlotChooser::fixNameLabelWidths(const int newWidth)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::fixNameLabelWidths START | newMaxWidth = "<<newWidth<<"\n";
#endif
   
   if (_wellFrames.size()>0) 
      for (int i=0;i<_noOfWells;i++) _wellFrames[i]->setNameLabelWidth(newWidth);
   if (_piezoFrames.size()>0) 
      for (int i=0;i<_noOfPiezometers;i++) _piezoFrames[i]->setNameLabelWidth(newWidth);
   _maxNameWidth = newWidth;
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::fixNameLabelWidths START END\n";
#endif
}

void PlotChooser::addWell(const int wellID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::addWell START | wellID = "<<wellID<<"\n";
#endif
   if (wellID > 0) _wellChooserLayout->takeAt(_noOfWells+1);
   _wellFrames.push_back( new WellChooserFrame(this,wellID,_data->wells()[wellID].name(), _data));
   _wellChooserLayout->addWidget(_wellFrames[wellID],Qt::AlignLeft);
   _noOfWells = _wellFrames.size();
   _wellFrames[wellID]->checkNameLabelWidth(_maxNameWidth);
   _wellChooserLayout->addStretch();
   _plot->addWell();
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::addWell END | _wellFrames.size() = "<<_wellFrames.size()<<"\n";
#endif
}

void PlotChooser::addPiezometer(const int piezoID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::addPiezometer START | piezoID = "<<piezoID<<"\n";
#endif
   if (piezoID > 0) _piezoChooserLayout->takeAt(_noOfPiezometers+1);
   _piezoFrames.push_back( new PiezoChooserFrame(this,piezoID,_data->piezometers()[piezoID].name(), _data));
   _piezoChooserLayout->addWidget(_piezoFrames[piezoID],Qt::AlignLeft);
   _noOfPiezometers = _piezoFrames.size();
   _piezoFrames[piezoID]->checkNameLabelWidth(_maxNameWidth);
   _piezoChooserLayout->addStretch();
   _plot->addPiez();
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::addPiezometer END\n";
#endif
}

void PlotChooser::removeWell(const int wellID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::removeWell | removing wellID = "<<wellID<<"\n";
#endif
   if (_wellFrames[wellID]->checkBox()->isChecked())
   {
#ifdef QDEBUG_ALL
      qDebug()<<"PlotChooser::removeWell | removing plots\n";
#endif
      const int itemFlag = 0;
      if (_wellFrames[wellID]->lossCheckBox()->isChecked())
         removePlot(itemFlag,wellID);
      if (_wellFrames[wellID]->noLossCheckBox()->isChecked())
         removePlot(itemFlag,wellID,false,true);
      if (_wellFrames[wellID]->smCheckBox()->isChecked())
         removePlot(itemFlag,wellID,true,false);
   }
   _plot->deleteWell(wellID);
   delete _wellFrames[wellID];
   _wellFrames.erase(_wellFrames.begin() + wellID);
   _noOfWells = _wellFrames.size();
   for (int i=wellID;i<_noOfWells;i++) _wellFrames[i]->reduceIDbyOne();
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::removeWell | wellID = "<<wellID<<" removed | _noOfWells = "<<_noOfWells<<"\n";
#endif
}

void PlotChooser::removePiezometer(const int piezoID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::removePiezometer | removing piezoID = "<<piezoID<<"\n";
#endif
   if (_piezoFrames[piezoID]->checkBox()->isChecked())
   {
      const int itemFlag = 1;
      removePlot(itemFlag,piezoID);
      if (_piezoFrames[piezoID]->smCheckBox()->isChecked())
         removePlot(itemFlag,piezoID,true,false);
   }
   _plot->deletePiez(piezoID);
   delete _piezoFrames[piezoID];
   _piezoFrames.erase(_piezoFrames.begin() + piezoID);
   _noOfPiezometers = _piezoFrames.size();
   for (int i=piezoID;i<_noOfPiezometers;i++) _piezoFrames[i]->reduceIDbyOne();
   //if (piezoID == 0) _piezoChooserLayout->insertStretch(0);
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::removePiezometer | piezoID = "<<piezoID<<" removed | _noOfPiezometers = "<<_noOfPiezometers<<"\n";
#endif
}

void PlotChooser::changeItemName(const int itemID, const int itemType, const QString & name)
{
   if (itemType == 0) 
   {
      _wellFrames[itemID]->setName(name);
      _plot->setWellPlotName(itemID,name);
   }
   else 
   {
      _piezoFrames[itemID]->setName(name);
      _plot->setPiezPlotName(itemID,name);
   }
}

// SLOTS FOR SIGNALS FROM wellChooserFrame AND piezoChooserFrame:
// add/remove plots:

void PlotChooser::calculateWell(const int wellID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::calculateWell | wellID = "<<wellID<<" _result = "<<_result<<"\n";
#endif
   _result->calculateWell(wellID);
}

void PlotChooser::clearWell(const int wellID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::clearWell | wellID = "<<wellID<<"\n";
#endif
   _result->clearIwell(wellID);
}

void PlotChooser::addPlot(const int itemFlag, const int itemID, const int colorID, const int styleID, const QString & name)
{
   if (itemFlag == 0) // itemID = wellID
   {
#ifdef QDEBUG_ALL
      qDebug()<<"********** ADDPLOT | itemFlag = "<<itemFlag<<" | itemID = "<<itemID<<"\n";
      qDebug()<<"********** ADDPLOT | colorName = "<<_colors[colorID].first<<" | color = "<<_colors[colorID].second<<"\n";
      qDebug()<<"********** ADDPLOT | styleID = "<<styleID<<"\n";
#endif
      _result->calculateWellLosses(itemID);
      _plot->addWellPlot(itemID,_colors[colorID].second.second,styleID,name);
      _plot->setDataOfPlot(_data->timeInSMHD(), qvecOperation::qvecPlus(_result->well(itemID), _result->wellLosses(itemID)), _plot->wellPlot(itemID));
      emit wellAdded(itemID);
   }
   else // item is a piezometer
   {
      _result->calculatePiez(itemID);
      _plot->addPiezPlot(itemID,_colors[colorID].second.second,styleID,name);
      _plot->setDataOfPlot(_data->timeInSMHD(),_result->piez(itemID),_plot->piezPlot(itemID));
   }
}

void PlotChooser::addNoLossPlot(const int wellID, const int colorID, const int styleID, const QString & name)
{
#ifdef QDEBUG_ALL
   qDebug()<<"********** ADDNOLOSSPLOT | itemID = "<<wellID<<"\n";
   qDebug()<<"********** ADDNOLOSSPLOT | colorName = "<<_colors[colorID].first<<" | color = "<<_colors[colorID].second.second<<"\n";
   qDebug()<<"********** ADDNOLOSSPLOT | styleID = "<<styleID<<"\n";
#endif
   _plot->addWellWithoutLossPlot(wellID,_colors[colorID].second.second,styleID,name);
   _plot->setDataOfPlot(_data->timeInSMHD(),_result->well(wellID),_plot->wellWithoutLossPlot(wellID));
}

void PlotChooser::addSmPlot(const int itemFlag, const int itemID, const int colorID, const int tickType, const QString & name)
{
   if (itemFlag == 0) 
   {
      _plot->addWellSmPlot(itemID,_colors[colorID].second.second,
                                           tickType,name);
      _plot->setDataOfPlot(conversion::point2FirstToQvec(_data->wells()[itemID].Sm(), _data->getModel().timeUnit()), conversion::point2SecondToQvec(_data->wells()[itemID].Sm()),_plot->wellSmPlot(itemID));
   }
   else 
   {
      _plot->addPiezSmPlot(itemID,_colors[colorID].second.second,tickType,name);
      _plot->setDataOfPlot(conversion::point2FirstToQvec(_data->piezometers()[itemID].Sm(), _data->getModel().timeUnit()), conversion::point2SecondToQvec(_data->piezometers()[itemID].Sm()),_plot->piezSmPlot(itemID));
   }
}
 
void PlotChooser::removePlot(const int itemFlag, const int itemID, const bool isSmPlot, const bool isNoLossPlot)
{
#ifdef QDEBUG_ALL
   qDebug()<<"********** REMOVEPLOT | itemFlag = "<<itemFlag<<" | itemID = "<<itemID<<" | a Sm Plot? = "<<isSmPlot<<" | a noLossPlot? = "<<isNoLossPlot<<"\n";
#endif
   if (itemFlag == 0) // well
   {
      if (isSmPlot) _plot->removeWellSmPlot(itemID);
      else if (isNoLossPlot) 
      {
         _plot->removeWellWithoutLossPlot(itemID);
      }
      else 
      {
         _plot->removeWellPlot(itemID);
         _result->clearIwellLosses(itemID);
         emit wellRemoved(itemID);
      }
   }
   else // piezometer
   {
      if (isSmPlot) _plot->removePiezSmPlot(itemID);
      else 
      {
         _result->clearIpiez(itemID);
         _plot->removePiezPlot(itemID);
      }
   }
}

// line and tick properties:
void PlotChooser::setLineProperties(const int itemFlag, const int itemID, const int colorID, const int styleID)
{
#ifdef QDEBUG_ALL
//    qDebug()<<"PlotChooser::setLineProperties START | itemFlag = "<<itemFlag<<" | itemID = "<<itemID<<" | color = ["<<_colors[colorID].first<<","<<_colors[colorID].second<<")\n";
#endif
   size_t graphID;
   if (itemFlag == 0) graphID = _plot->wellPlot(itemID);
   else graphID = _plot->piezPlot(itemID);
   _plot->setLineStyle(_colors[colorID].second.second,graphID,styleID);
}
  
void PlotChooser::setNoLossLineProperties(const int itemID, const int colorID, const int styleID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PlotChooser::setNoLossLineProperties START | itemID = "<<itemID<<" | color = ["<<_colors[colorID].first<<","<<_colors[colorID].second.second<<") | styleID = "<<styleID<<"\n";
#endif
   const size_t graphID = _plot->wellWithoutLossPlot(itemID);
   _plot->setLineStyle(_colors[colorID].second.second,graphID,styleID);
}

void PlotChooser::setSmTickType(const int itemFlag, const int itemID, const int tickID, const int colorID)
{
   size_t graphID;
   if (itemFlag == 0) graphID = _plot->wellSmPlot(itemID);
   else graphID = _plot->piezSmPlot(itemID);
   if(graphID!=std::numeric_limits<size_t>::max())
      _plot->setScatter(graphID,tickID,_colors[colorID].second.second);
}
