#include "printTab.h"

//#include "widgets/qcustomplot/qwellsPlot.h"
//#include "result/result.h"
//#include "engine/data.h"
#include "settings/plotSettings.h"
#include "qWells.h"

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QFileDialog>
#include <QMenu>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>

PrintTab::PrintTab(Data * data) : QFrame(), _data(data)
{
   _hasSTitle = false;
   _hasSData = false;
   QVBoxLayout * printLayout = new QVBoxLayout;
   
   QFrame * twoPlotFrame = new QFrame;
   QVBoxLayout * twoPlotLayout = new QVBoxLayout;
   QFrame * plotButtonFrame = new QFrame;
   QHBoxLayout * plotButtonLayout = new QHBoxLayout;

   _qplot = new QCustomPlot; // TODO change
   _qplot->xAxis->setLabel("Time");
   _qplot->yAxis->setLabel("Q [l/s]");
   _qplot->legend->setVisible(true);
   QFont legendFont = font();
   legendFont.setPointSize(10);
   _qplot->legend->setFont(legendFont);
   _qplot->legend->setSelectedFont(legendFont);
   _qplot->legend->setMaximumSize(1,1);
   _qplot->xAxis2->setVisible(true);
   _qplot->xAxis2->setTickLabels(false);
   _qplot->yAxis2->setVisible(true);
   _qplot->yAxis2->setTickLabels(true);

   _saveQButton = new QPushButton(tr("Export to pdf"));
   plotButtonLayout->addWidget(_qplot, Qt::AlignLeft);
   plotButtonLayout->addWidget(_saveQButton);
   plotButtonFrame->setLayout(plotButtonLayout);
   plotButtonFrame->setFrameShape(QFrame::StyledPanel);
   plotButtonFrame->setFrameShadow(QFrame::Sunken);
   twoPlotLayout->addWidget(plotButtonFrame);
   
   QFrame * plotButtonFrame1 = new QFrame;
   QHBoxLayout * plotButtonLayout1 = new QHBoxLayout;
   _splot = new QWellsPlot(_data->noOfWells(), _data->noOfPiezometers(), 
         data->qWells()->plotSettings(), data->staticLevel());
   _splot->setGraph("S [m]");
   _saveSButton = new QPushButton(tr("Export to pdf"));
   plotButtonLayout1->addWidget(_splot, Qt::AlignLeft);
   plotButtonLayout1->addWidget(_saveSButton);
   plotButtonFrame1->setLayout(plotButtonLayout1);
   plotButtonFrame1->setFrameShape(QFrame::StyledPanel);
   plotButtonFrame1->setFrameShadow(QFrame::Sunken);

   connect(_data->qWells()->plotSettings(), &PlotSettings::setZoomFactor, this, &PrintTab::setZoomFactor);

   connect(_data,&Data::timeUnitSet,_splot,&QWellsPlot::setTimeUnit);

   twoPlotLayout->addWidget(plotButtonFrame1);
   twoPlotFrame->setLayout(twoPlotLayout);
  // twoPlotFrame->setMidLineWidth(2);
  // twoPlotFrame->setFrameShape(QFrame::StyledPanel);
  // twoPlotFrame->setFrameShadow(QFrame::Sunken);
   
   printLayout->addWidget(twoPlotFrame,Qt::AlignCenter);
   setLayout(printLayout);
   
   connect(_qplot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
   connect(_qplot->xAxis, SIGNAL(rangeChanged(QCPRange)), _qplot->xAxis2, SLOT(setRange(QCPRange)));
   connect(_qplot->yAxis, SIGNAL(rangeChanged(QCPRange)), _qplot->yAxis2, SLOT(setRange(QCPRange)));
   connect(_qplot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));   
   _qplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);


   //connect(_saveQButton, SIGNAL(mySignal()), qApp, SLOT(aboutQt()));
   connect(_saveQButton, SIGNAL(clicked()), this, SLOT(exportQToPdf()));
   connect(_saveSButton, SIGNAL(clicked()), this, SLOT(exportSToPdf()));

   _qplot->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(_qplot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequestQ(QPoint)));
   _splot->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(_splot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequestS(QPoint)));
}

void PrintTab::setZoomFactor(const double zoomFactor, const int lineWidth)
{
   _splot->axisRect()->setRangeZoomFactor(zoomFactor);
   _qplot->axisRect()->setRangeZoomFactor(zoomFactor);
   for(int i=0;i<_splot->graphCount();i++)
   {
      QPen pen=_splot->graph(i)->pen();
      pen.setWidth(lineWidth);
      _splot->graph(i)->setPen(pen);
   }
   for(int j=0;j<_qplot->graphCount();j++)
   {
      QPen pen=_qplot->graph(j)->pen();
      pen.setWidth(lineWidth);
      _qplot->graph(j)->setPen(pen);
   }
   _splot->replot();
   _qplot->replot();
}

void PrintTab::contextMenuRequestQ(QPoint posQ)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Title", this, SLOT(titleDialog()));
   menu->addAction("Set range", this, SLOT(qminMaxDialog()));
   menu->addAction("Set ticks", this, SLOT(setQTicksSlot()));
   menu->addSeparator();
   menu->addAction("Plot settings", _data->qWells()->plotSettings(),
         SLOT(startDialog()));
   menu->popup(_qplot->mapToGlobal(posQ));
}

void PrintTab::contextMenuRequestS(QPoint posS)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Title", this, SLOT(titleDialogS()));
   menu->addAction("Show data", this, SLOT(dataDialog()));
   menu->addAction("Set range", this, SLOT(sminMaxDialog()));
   menu->addAction("Set ticks", _splot, SLOT(setTicksSlot()));
   menu->addSeparator();
   menu->addAction("Plot settings", _data->qWells()->plotSettings(),
         SLOT(startDialog()));
   menu->popup(_splot->mapToGlobal(posS));
}

void PrintTab::dataDialog()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Data");

   QGridLayout * layout = new QGridLayout;
   QLabel * tLabel = new QLabel("T");
   layout->addWidget(tLabel,0,0,1,1, Qt::AlignRight);
   QCheckBox * tcheck = new QCheckBox;
   layout->addWidget(tcheck,0,1,1,1);

   QLabel * muLabel = new QLabel(QString(QChar(0x03BC)));
   layout->addWidget(muLabel,1,0,1,1, Qt::AlignRight);
   QCheckBox * mucheck = new QCheckBox;
   layout->addWidget(mucheck,1,1,1,1);
   /*
   QLabel * aLabel = new QLabel("A");
   layout->addWidget(aLabel,2,0,1,1, Qt::AlignRight);
   QCheckBox * acheck = new QCheckBox;
   layout->addWidget(acheck,2,1,1,1);

   QLabel * bLabel = new QLabel("B");
   layout->addWidget(bLabel,3,0,1,1, Qt::AlignRight);
   QCheckBox * bcheck = new QCheckBox;
   layout->addWidget(bcheck,3,1,1,1);

   size_t nb=_data->getBoundary().nlines();
   std::vector<QCheckBox *> boundChecks(nb);
   QCheckBox * maxCheck;
   for(size_t i=0;i<nb;i++)
   {
      QLabel * boundLabel = new QLabel("Distance to line "+QString::number(i));
      layout->addWidget(boundLabel,i+4,0,1,1, Qt::AlignRight);
      boundChecks[i] = new QCheckBox;
      layout->addWidget(boundChecks[i],i+4,1,1,1);
   }
   
   if(_data->getBoundary()->maxDistance() != std::numeric_limits<double>::max())
   {
      QLabel * boundLabel = new QLabel("Maximal distance");
      layout->addWidget(boundLabel,layout->rowCount()+1,0,1,1, Qt::AlignRight);
      maxCheck = new QCheckBox;
      layout->addWidget(maxCheck,layout->rowCount()+1,1,1,1);
   }*/

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,layout->rowCount(),0,1,2,Qt::AlignCenter);
   dialog->setLayout(layout);

   if(dialog->exec() == QDialog::Accepted)
   {
      QString dataString;
      if(tcheck->isChecked())
      {
         dataString.append("T = ");
         dataString.append(QString::number(_data->getModel().bigT()));
         dataString.append("[m"+QString(QChar(0x00B2))+"/s], ");
      }
      if(mucheck->isChecked())
      {
         dataString.append(QString(QChar(0x03BC)));
         dataString.append(" = ");
         dataString.append(QString::number(_data->getModel().mu()));
         dataString.append("[-], ");
      }
      if(!_hasSTitle)
      {//then there is no title
         if(!_hasSData)
         {
            _sdata=new QCPPlotTitle(_splot, dataString);
            _sdata->setFont(QFont("Helvetica", -1, -1));
            _splot->plotLayout()->insertRow(0);
            _splot->plotLayout()->addElement(0, 0, _sdata);
            _hasSData=true;
         }else{
            _sdata->setText(dataString);
         }
      }else if(_hasSTitle)
      { 
         if(!_hasSData)
         {
            _sdata=new QCPPlotTitle(_splot, dataString);
            _sdata->setFont(QFont("Helvetica", -1, -1));
            _splot->plotLayout()->insertRow(1);
            _splot->plotLayout()->addElement(1, 0, _sdata);
            _hasSData=true;
         }else{
            _sdata->setText(dataString);
         }
      }
      _splot->replot();
   }
}

void PrintTab::qminMaxDialog()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set range");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * xminLabel = new QLabel("Xmin:");
   layout->addWidget(xminLabel,0,0,1,1);
   QLineEdit * xminLineEdit = new QLineEdit(dialog);
   xminLineEdit->setText(QString::number(_qplot->xAxis->range().lower));
   layout->addWidget(xminLineEdit,0,1,1,1);
   
   QLabel * xmaxLabel = new QLabel("Xmax:");
   layout->addWidget(xmaxLabel,0,2,1,1);
   QLineEdit * xmaxLineEdit = new QLineEdit(dialog);
   xmaxLineEdit->setText(QString::number(_qplot->xAxis->range().upper));
   layout->addWidget(xmaxLineEdit,0,3,1,1);

   QLabel * yminLabel = new QLabel("Ymin:");
   layout->addWidget(yminLabel,1,0,1,1);
   QLineEdit * yminLineEdit = new QLineEdit(dialog);
   yminLineEdit->setText(QString::number(_qplot->yAxis->range().lower));
   layout->addWidget(yminLineEdit,1,1,1,1);

   QLabel * ymaxLabel = new QLabel("Ymax:");
   layout->addWidget(ymaxLabel,1,2,1,1);
   QLineEdit * ymaxLineEdit = new QLineEdit(dialog);
   ymaxLineEdit->setText(QString::number(_qplot->yAxis->range().upper));
   layout->addWidget(ymaxLineEdit,1,3,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      
      _qplot->xAxis->setRange(xminLineEdit->text().toDouble(), xmaxLineEdit->text().toDouble());
      _qplot->yAxis->setRange(yminLineEdit->text().toDouble(), ymaxLineEdit->text().toDouble());
      _qplot->replot();
   }
}

void PrintTab::sminMaxDialog()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set range");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * xminLabel = new QLabel("Xmin:");
   layout->addWidget(xminLabel,0,0,1,1);
   QLineEdit * xminLineEdit = new QLineEdit(dialog);
   xminLineEdit->setText(QString::number(_splot->xAxis->range().lower));
   layout->addWidget(xminLineEdit,0,1,1,1);
   
   QLabel * xmaxLabel = new QLabel("Xmax:");
   layout->addWidget(xmaxLabel,0,2,1,1);
   QLineEdit * xmaxLineEdit = new QLineEdit(dialog);
   xmaxLineEdit->setText(QString::number(_splot->xAxis->range().upper));
   layout->addWidget(xmaxLineEdit,0,3,1,1);

   QLabel * yminLabel = new QLabel("Ymin:");
   layout->addWidget(yminLabel,1,0,1,1);
   QLineEdit * yminLineEdit = new QLineEdit(dialog);
   yminLineEdit->setText(QString::number(_splot->yAxis->range().lower));
   layout->addWidget(yminLineEdit,1,1,1,1);

   QLabel * ymaxLabel = new QLabel("Ymax:");
   layout->addWidget(ymaxLabel,1,2,1,1);
   QLineEdit * ymaxLineEdit = new QLineEdit(dialog);
   ymaxLineEdit->setText(QString::number(_splot->yAxis->range().upper));
   layout->addWidget(ymaxLineEdit,1,3,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      
      _splot->xAxis->setRange(xminLineEdit->text().toDouble(), xmaxLineEdit->text().toDouble());
      _splot->yAxis->setRange(yminLineEdit->text().toDouble(), ymaxLineEdit->text().toDouble());
      _splot->replot();
   }
}


void PrintTab::titleDialog()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set title");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * titleLabel = new QLabel("Title:");
   layout->addWidget(titleLabel,0,0,1,1);
   QLineEdit * titleLineEdit = new QLineEdit(dialog);
   if(_qplot->plotLayout()->rowCount()==2)
      titleLineEdit->setText(_qtitle->text());
   layout->addWidget(titleLineEdit,0,1,1,1);
   
   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      if(_splot->plotLayout()->rowCount()==1)
      {
         _qtitle= new QCPPlotTitle(_qplot, titleLineEdit->text());
         _qplot->plotLayout()->insertRow(0);
         _qplot->plotLayout()->addElement(0, 0, _qtitle);
      }else
         _qtitle->setText(titleLineEdit->text());
      _qplot->replot();
   }
}

void PrintTab::titleDialogS()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set title");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * titleLabel = new QLabel("Title:");
   layout->addWidget(titleLabel,0,0,1,1);
   QLineEdit * titleLineEdit = new QLineEdit(dialog);
   if(_hasSTitle)
      titleLineEdit->setText(_stitle->text());
   layout->addWidget(titleLineEdit,0,1,1,1);
   
   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      if(!_hasSTitle)
      {
         _stitle= new QCPPlotTitle(_splot, titleLineEdit->text());
         _splot->plotLayout()->insertRow(0);
         _splot->plotLayout()->addElement(0, 0, _stitle);
         _hasSTitle=true;
      }else
         _stitle->setText(titleLineEdit->text());
      _splot->replot();
   }
}


void PrintTab::selectionChanged()
{
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (_qplot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || _qplot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      _qplot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || _qplot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    _qplot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    _qplot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (_qplot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || _qplot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      _qplot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || _qplot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    _qplot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    _qplot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
}

void PrintTab::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (_qplot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    _qplot->axisRect()->setRangeZoom(_qplot->xAxis->orientation());
  else if (_qplot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    _qplot->axisRect()->setRangeZoom(_qplot->yAxis->orientation());
  else
    _qplot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void PrintTab::exportQToPdf()
{
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Export to pdf"),".",tr("PDF Files (*.pdf)"));
   if(!fileName.endsWith(".pdf"))
      fileName.append(".pdf");
//   fileName.append("pdf");
   _qplot->savePdf(fileName);
}

void PrintTab::exportSToPdf()
{
   QString fileName = QFileDialog::getSaveFileName(this,
               tr("Export to pdf"),".",tr("PDF Files (*.pdf)"));
   if(!fileName.endsWith(".pdf"))
      fileName.append(".pdf");
//   fileName.append("pdf");
   _splot->savePdf(fileName);
}


void PrintTab::removeGraphs()
{
   size_t ngraphs=_splot->graphCount();
   for(int i=ngraphs-1;i>=0;i--)
   {
      _splot->removeGraph(i);
   }   
   ngraphs=_qplot->graphCount();
   for(int i=ngraphs-1;i>=0;i--)
   {
      _qplot->removeGraph(i);
   }
}

void PrintTab::setQGraphs()
{
   if(_qplot->graphCount()!=0)
      return;
   _qplot->xAxis->setRange(0.0, 0.0001);
   _qplot->yAxis->setRange(0.0, 0.0001);
   size_t nwell=_splot->wellPlot().size();
   size_t igraph=0;
   if(_data->getModel().timeUnit()==0)
      _qplot->xAxis->setLabel("Time [s]");
   else if(_data->getModel().timeUnit()==1)
      _qplot->xAxis->setLabel("Time [min]");
   else if(_data->getModel().timeUnit()==2)
      _qplot->xAxis->setLabel("Time [h]");
   else if(_data->getModel().timeUnit()==3)
      _qplot->xAxis->setLabel("Time [days]");
   for(size_t iwell=0;iwell<nwell;iwell++)
   {
      if(_splot->wellPlot(iwell)==std::numeric_limits<size_t>::max())
         continue;
      else
      {
         _qplot->addGraph();
         _qplot->graph(igraph)->setPen(_splot->graph(_splot->wellPlot(iwell))->pen());
         //_qplot->graph(igraph)->setLineStyle(_splot->graph(_splot->wellPlot(iwell))->lineStyle());
         //qDebug()<<" sta ti je problem "<<_splot->graph(igraph)->name()<<" jer imas da ti je "<<igraph<<" a moja ideja "<<_splot->graph(_splot->wellPlot(iwell))->name()<<"\n";
         _qplot->graph(igraph)->setName(_splot->graph(_splot->wellPlot(iwell))->name());
         _qplot->graph(igraph)->setLineStyle((QCPGraph::LineStyle) 2);
         QVector<double> x(conversion::point2FirstToQvec(_data->wells()[iwell].q(), _data->getModel().timeUnit()));
         QVector<double> y(conversion::point2SecondToQvec(_data->wells()[iwell].q()));
         x.push_back(_data->timeInSMHD().back());
         y.push_back(y.back());
         _qplot->graph(igraph)->setData(x,y);
         _qplot->graph(igraph)->rescaleAxes(true);
         igraph++;
      }
   }
   _qplot->rescaleAxes();
   _qplot->xAxis->setRangeUpper(_splot->xAxis->range().upper);
   _qplot->replot();
}

void PrintTab::setGraphs(QWellsPlot * splot)
{
   if(_splot->graphCount()!=0)
      return;
   _splot->xAxis->setRange(0.0, 0.0001);
   _splot->yAxis->setRange(0.0, 0.0001);
   size_t ngraph=splot->graphCount();
   _splot->setWellPlot(splot->wellPlot());
   _splot->setWellSmPlot(splot->wellSmPlot());
   _splot->setWellWithoutLossPlot(splot->wellWithoutLossPlot());
   _splot->setPiezPlot(splot->piezPlot());
   _splot->setPiezSmPlot(splot->piezSmPlot());
   

   for(size_t igraph=0;igraph<ngraph;igraph++)
   {
      //size_t igraph=splot->wellPlot(iwell);
      //if(igraph==std::numeric_limits<size_t>::max())
      // continue;
      //else
      //{
      _splot->addGraph();
      _splot->graph(igraph)->setPen(splot->graph(igraph)->pen());
      _splot->graph(igraph)->setLineStyle(splot->graph(igraph)->lineStyle());
      _splot->graph(igraph)->setScatterStyle(splot->graph(igraph)->scatterStyle());
      _splot->graph(igraph)->setName(splot->graph(igraph)->name());
      //}
   }
   size_t nwells=splot->nwells();
   size_t npiez=splot->npiez();
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      if(_splot->wellSmPlot(iwell)==std::numeric_limits<size_t>::max())
         continue;
      else
         _splot->setDataOfPlot(conversion::point2FirstToQvec(_data->wells()[iwell].Sm(), _data->getModel().timeUnit()), conversion::point2SecondToQvec(_data->wells()[iwell].Sm()), _splot->wellSmPlot(iwell));
   }
   for(size_t ipiez=0;ipiez<npiez;ipiez++)
   {
      if(_splot->piezSmPlot(ipiez)==std::numeric_limits<size_t>::max())
         continue;
      else
      {
         _splot->setDataOfPlot(conversion::point2FirstToQvec(_data->piezometers()[ipiez].Sm(), _data->getModel().timeUnit()), conversion::point2SecondToQvec(_data->piezometers()[ipiez].Sm()), _splot->piezSmPlot(ipiez));
      }
   }
}

void PrintTab::setQTicksSlot()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set ticks step");
QGridLayout * layout = new QGridLayout;
   
   QLabel * xTickStep = new QLabel("X tick step:");
   layout->addWidget(xTickStep,0,0,1,1);
   QLineEdit * xLineEdit = new QLineEdit(dialog);
   xLineEdit->setText(QString::number(_qplot->xAxis->tickStep()));
   layout->addWidget(xLineEdit,0,1,1,1);
   
   QLabel * yTickStep = new QLabel("Y tick step:");
   layout->addWidget(yTickStep,1,0,1,1);
   QLineEdit * yLineEdit = new QLineEdit(dialog);
   yLineEdit->setText(QString::number(_qplot->yAxis->tickStep()));
   layout->addWidget(yLineEdit,1,1,1,1);

   QLabel * automCheck = new QLabel("Auto ticks");
   layout->addWidget(automCheck,2,0,1,1);
   QCheckBox * autoTicks = new QCheckBox;
   layout->addWidget(autoTicks,2,1,1,1);



   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel, Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));
   layout->addWidget(buttonBox,3,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      if(autoTicks->isChecked())
      {
         _qplot->xAxis->setAutoTickStep(true);
         _qplot->xAxis2->setAutoTickStep(true);
         _qplot->yAxis->setAutoTickStep(true);
         _qplot->yAxis2->setAutoTickStep(true);
      }else{
         _qplot->xAxis->setAutoTickStep(false);
         _qplot->xAxis2->setAutoTickStep(false);
         _qplot->yAxis->setAutoTickStep(false);
         _qplot->yAxis2->setAutoTickStep(false);
      }
      _qplot->xAxis->setTickStep(xLineEdit->text().toDouble());
      _qplot->xAxis2->setTickStep(xLineEdit->text().toDouble());
      _qplot->yAxis->setTickStep(yLineEdit->text().toDouble());
      _qplot->yAxis2->setTickStep(yLineEdit->text().toDouble());
      _qplot->replot();
   }
}



/*void PrintTab::exportToPdf();
{
   _qplot->savePdf("ddd");
}*/

/*void PrintTab::refresh(QWellsPlot * splot)
{
   //_splot->setData(splot->data(), true);
   qDebug()<<" evo ga osvezavanje "<<_splot<<" dok je "<<splot<<"\n";
   size_t ngraph=splot->graphCount();
   size_t nwells=splot->nwells();
   _splot->wellPlot().resize(nwells);
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      size_t igraph=splot->wellPlot(iwell);
      _splot->setWellPlot(iwell, igraph);
      if(igraph==std::numeric_limits<size_t>::max())
         continue;
      else
      {
         _splot->addGraph();
         _splot->graph(igraph)->setPen(splot->graph(igraph)->pen());
         _splot->graph(igraph)->setName(splot->graph(igraph)->name());
            //WellPlot(iwell, splot->graph(splot->wellPlot(iwell)));
      }
   }
   _splot->setAllData();
   //for(size_t igraph=0;igraph<ngraph;igraph++)
   //{
   //   _splot->addGraph();
   //   _splot->graph(igraph)->setData(splot->graph(igraph)->data(), true);
   //}
}*/

