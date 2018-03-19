#include "qwellsPlot.h"
#include "qWells.h"
#include "settings/plotSettings.h"
#include <QDebug>
#include <limits>
#include <QMenu>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QStatusBar>

QWellsPlot::QWellsPlot(size_t nWells, size_t nPiez, PlotSettings * plotSettings,
      double * st): _plotSettings(plotSettings), _staticLevel(st)
{
   _legendShow=true;
   _wellPlot.resize(nWells, std::numeric_limits<size_t>::max());
   _wellSmPlot.resize(nWells, std::numeric_limits<size_t>::max());
   _wellWithoutLossPlot.resize(nWells, std::numeric_limits<size_t>::max());
   _piezPlot.resize(nPiez, std::numeric_limits<size_t>::max());
   _piezSmPlot.resize(nPiez, std::numeric_limits<size_t>::max());   
   connect(_plotSettings, &PlotSettings::setZoomFactor, this, &QWellsPlot::setZoomFactorSlot);
  // connect(_plotSettings, &PlotSettings::showLegend, this, &QWellsPlot::showLegendSlot);
   this->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
   connect(this, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showCoord(QMouseEvent*)));
}

void QWellsPlot::mouseDoubleClickEvent(QMouseEvent * event)
{
   emit _plotSettings->qWells()->doubleClickPlot(this->xAxis->pixelToCoord(event->pos().x()));
}

void QWellsPlot::addWell()
{
   _wellPlot.push_back(std::numeric_limits<size_t>::max());
   _wellSmPlot.push_back(std::numeric_limits<size_t>::max());
   _wellWithoutLossPlot.push_back(std::numeric_limits<size_t>::max());
}

void QWellsPlot::addPiez()
{
   _piezPlot.push_back(std::numeric_limits<size_t>::max());
   _piezSmPlot.push_back(std::numeric_limits<size_t>::max());
}

void QWellsPlot::deleteWell(const size_t iwell)
{
   _wellPlot.erase(_wellPlot.begin()+iwell);
   _wellSmPlot.erase(_wellSmPlot.begin()+iwell);
   _wellWithoutLossPlot.erase(_wellWithoutLossPlot.begin()+iwell);
}

void QWellsPlot::deletePiez(const size_t ipiez)
{
   _piezPlot.erase(_piezPlot.begin()+ipiez);
   _piezSmPlot.erase(_piezSmPlot.begin()+ipiez);
}


void QWellsPlot::changeGraphIds(const size_t iGraph)
{
   for(size_t i=0;i<_wellPlot.size();i++)
   {
      if(_wellPlot[i]==std::numeric_limits<size_t>::max())
         continue;
      else if(_wellPlot[i]>iGraph)
         _wellPlot[i]--;
   }
   for(size_t i=0;i<_wellSmPlot.size();i++)
   {
      if(_wellSmPlot[i]==std::numeric_limits<size_t>::max())
         continue;
      else if(_wellSmPlot[i]>iGraph)
         _wellSmPlot[i]--;
   }
   for(size_t i=0;i<_wellWithoutLossPlot.size();i++)
   {
      if(_wellWithoutLossPlot[i]==std::numeric_limits<size_t>::max())
         continue;
      else if(_wellWithoutLossPlot[i]>iGraph)
         _wellWithoutLossPlot[i]--;
   } 
   for(size_t i=0;i<_piezPlot.size();i++)
   {
      if(_piezPlot[i]==std::numeric_limits<size_t>::max())
         continue;
      else if(_piezPlot[i]>iGraph)
         _piezPlot[i]--;
   }
   for(size_t i=0;i<_piezSmPlot.size();i++)
   {
      if(_piezSmPlot[i]==std::numeric_limits<size_t>::max())
         continue;
      else if(_piezSmPlot[i]>iGraph)
         _piezSmPlot[i]--;
   }
}

void QWellsPlot::setGraph(const QString & name)
{
   this->xAxis->setRange(0.0, 0.0001);
   this->yAxis->setRange(0.0, 0.0001);
   this->yAxis2->setRange(0.0, 0.0001);
   this->xAxis->setLabel("Time");
   this->yAxis->setLabel(name);
   this->yAxis->setRangeReversed(true);
 //  this->yAxis->setNumberFormat("ebd");
 //  this->yAxis->setNumberPrecision(2);
   this->legend->setVisible(true);
   QFont legendFont = font();
   legendFont.setPointSize(10);
   this->legend->setFont(legendFont);
   this->legend->setSelectedFont(legendFont);
   this->legend->setMaximumSize(1,1);
   this->xAxis2->setVisible(true);
   this->xAxis2->setTickLabels(false);
   this->yAxis2->setVisible(true);
   this->yAxis2->setRangeReversed(true);
   this->yAxis2->setTickLabels(true);
   this->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom);
   this->axisRect()->setRangeZoomFactor(0.95);
   this->legend->setSelectableParts(QCPLegend::spItems);
  // make left and bottom axes always transfer their ranges to right and top axes:
    connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
    connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(setRangeStaticLevel(QCPRange)));
//   connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));
   //radi this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
   connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
   this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
         QCP::iSelectLegend | QCP::iSelectPlottables);

  // connect(this, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*,int)));
}
void QWellsPlot::selectionChanged()
{
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    this->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    this->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    this->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    this->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  for (int i=0; i<this->graphCount(); ++i)
  {
    QCPGraph *graph = this->graph(i);
    QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }

}

void QWellsPlot::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed
  
  if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    this->axisRect()->setRangeZoom(this->xAxis->orientation());
  else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    this->axisRect()->setRangeZoom(this->yAxis->orientation());
  else
    this->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void QWellsPlot::setStyle(const size_t iGraph, 
      const size_t style)
{
/* enum QCPGraph::LineStyle 
 lsNone --- data points are not connected with any lines (e.g. data only represented with symbols according to the scatter style, see setScatterStyle)
lsLine --- data points are connected by a straight line
lsStepLeft --- line is drawn as steps where the step height is the value of the left data point
lsStepRight --- line is drawn as steps where the step height is the value of the right data point
lsStepCenter --- line is drawn as steps where the step is in between two data points
lsImpulse --- each data point is represented by a line parallel to the value axis, which reaches from the data point to the zero-value-line*/
   this->graph(iGraph)->setLineStyle((QCPGraph::LineStyle)(style));
   //this->replot();
}

void QWellsPlot::setScatter (const size_t iGraph, const size_t style, 
      const QColor & color)
{
   /*enum QCPScatterStyle::ScatterShape
 ssNone -- no scatter symbols are drawn (e.g. in QCPGraph, data only represented with lines)
ssDot -- a single pixel (use ssDisc or ssCircle if you want a round shape with a certain radius)
ssCross -- a cross
ssPlus -- a plus
ssCircle -- a circle
ssDisc --a circle which is filled with the pen's color (not the brush as with ssCircle)
ssSquare -- a square
ssDiamond --a diamond
ssStar -- a star with eight arms, i.e. a combination of cross and plus
ssTriangle -- an equilateral triangle, standing on baseline
ssTriangleInverted -- an equilateral triangle, standing on corner
ssCrossSquare -- a square with a cross inside
ssPlusSquare -- a square with a plus inside
ssCrossCircle -- a circle with a cross inside
ssPlusCircle -- a circle with a plus inside
ssPeace -- a circle, with one vertical and two downward diagonal lines
ssPixmap -- a custom pixmap specified by setPixmap, centered on the data point coordinates
ssCustom -- custom painter operations are performed per scatter (As QPainterPath, see setCustomPath)*/
   this->graph(iGraph)->setLineStyle(QCPGraph::lsNone);
   QPen scatterStyle(color);
   //scatterStyle.setColor(color);  
   scatterStyle.setWidth(_lineWidth);
   this->graph(iGraph)->setPen(scatterStyle);

   this->graph(iGraph)->setScatterStyle((QCPScatterStyle::ScatterShape)(style));
   this->replot();
}

void QWellsPlot::setLineStyle (const QColor & color, const size_t iGraph, const size_t style)
{
  /* Qt::NoPen	0	no line at all. For example, QPainter::drawRect() fills but does not draw any boundary line.
    Qt::SolidLine	1	A plain line.
    Qt::DashLine	2	Dashes separated by a few pixels.
    Qt::DotLine	3	Dots separated by a few pixels.
    Qt::DashDotLine	4	Alternate dots and dashes.
    Qt::DashDotDotLine	5	One dash, two dots, one dash, two dots.*/
   QPen lineStyle((Qt::PenStyle) style);
   lineStyle.setColor(color);   
   lineStyle.setWidth(_lineWidth);
   this->graph(iGraph)->setPen(lineStyle);
   this->replot();
}

void QWellsPlot::setWellPlotName(const size_t wellId, const QString & name)
{
   if(_wellPlot[wellId] != std::numeric_limits<size_t>::max())
      this->graph(_wellPlot[wellId])->setName(name);
   if(_wellSmPlot[wellId] != std::numeric_limits<size_t>::max())
      this->graph(_wellSmPlot[wellId])->setName(name+"_measured");
   if(_wellWithoutLossPlot[wellId] != std::numeric_limits<size_t>::max())
      this->graph(_wellWithoutLossPlot[wellId])->setName(name+"_without_well_losses");
   this->replot();
}

void QWellsPlot::setPiezPlotName(const size_t piezId, const QString & name)
{
   if(_piezPlot[piezId] != std::numeric_limits<size_t>::max())
      this->graph(_piezPlot[piezId])->setName(name);
   if(_piezSmPlot[piezId] != std::numeric_limits<size_t>::max())
      this->graph(_piezSmPlot[piezId])->setName(name+"_measured");
   this->replot();
}

void QWellsPlot::addWellPlot(const size_t wellId, const QColor & color, 
      const size_t style, const QString & name)
{
   _wellPlot[wellId]=this->graphCount();
   this->addGraph();
   this->setLineStyle(color, _wellPlot[wellId], style);
   this->graph()->setName(name);
   this->rescaleAxes();
}

void QWellsPlot::removeWellPlot(const size_t wellId)
{
   this->removeGraph(_wellPlot[wellId]);
   changeGraphIds(_wellPlot[wellId]);
   _wellPlot[wellId]= std::numeric_limits<size_t>::max();
   this->rescaleAxes();
   this->replot();
}

void QWellsPlot::addWellSmPlot(const size_t wellId, const QColor & color, 
      const size_t scatterStyle, const QString & name)
{
   _wellSmPlot[wellId]=this->graphCount();
   this->addGraph();
   this->setScatter(_wellSmPlot[wellId], scatterStyle, color);
   this->graph()->setName(name+"_measured");
   this->rescaleAxes();
}

void QWellsPlot::removeWellSmPlot(const size_t wellId)
{
   this->removeGraph(_wellSmPlot[wellId]);
   changeGraphIds(_wellSmPlot[wellId]);
   _wellSmPlot[wellId]= std::numeric_limits<size_t>::max();
   this->rescaleAxes();
   this->replot();
}

void QWellsPlot::addWellWithoutLossPlot(const size_t wellId, 
      const QColor & color, const size_t style, const QString & name)
{
   _wellWithoutLossPlot[wellId]=this->graphCount();
   this->addGraph();
   this->setLineStyle(color, _wellWithoutLossPlot[wellId], style);
   this->graph()->setName(name+"_without_well_losses");
   this->rescaleAxes();
}

void QWellsPlot::removeWellWithoutLossPlot(const size_t wellId)
{
   this->removeGraph(_wellWithoutLossPlot[wellId]);
   changeGraphIds(_wellWithoutLossPlot[wellId]);
   _wellWithoutLossPlot[wellId]= std::numeric_limits<size_t>::max();
   this->rescaleAxes();
   this->replot();
}

void QWellsPlot::addPiezPlot(const size_t piezId, const QColor & color, 
      const size_t style, const QString & name)
{
   _piezPlot[piezId]=this->graphCount();
   this->addGraph();
   this->setLineStyle(color, _piezPlot[piezId], style);
   this->graph()->setName(name);
   this->rescaleAxes();
}

void QWellsPlot::removePiezPlot(const size_t piezId)
{
   this->removeGraph(_piezPlot[piezId]);
   changeGraphIds(_piezPlot[piezId]);
   _piezPlot[piezId]= std::numeric_limits<size_t>::max();
   this->rescaleAxes();
   this->replot();
}

void QWellsPlot::addPiezSmPlot(const size_t piezId, const QColor & color, 
      const size_t scatterStyle, const QString & name)
{
   _piezSmPlot[piezId]=this->graphCount();
   this->addGraph();
   this->setScatter(_piezSmPlot[piezId], scatterStyle, color);
   this->graph()->setName(name+"_measured");
   this->rescaleAxes();
}

void QWellsPlot::removePiezSmPlot(const size_t piezId)
{
   this->removeGraph(_piezSmPlot[piezId]);
   changeGraphIds(_piezSmPlot[piezId]);
   _piezSmPlot[piezId]= std::numeric_limits<size_t>::max();
   this->replot();
   this->rescaleAxes();
}

void QWellsPlot::setDataOfPlot(const QVector<double> & x, 
      const QVector<double> & y, const size_t iGraph)
{
   this->graph(iGraph)->setData(x, y);
  // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
   this->graph(iGraph)->rescaleAxes(true);
   this->replot();
}

void QWellsPlot::setTimeUnit(const size_t timeUnit)
{
   if(timeUnit==0)
      this->xAxis->setLabel("Time [s]");
   else if(timeUnit==1)
      this->xAxis->setLabel("Time [min]");
   else if(timeUnit==2)
      this->xAxis->setLabel("Time [h]");
   else if(timeUnit==3)
      this->xAxis->setLabel("Time [days]");
}

void QWellsPlot::setZoomFactorSlot(const double zoomFactor, 
      const int lineWidth)
{
   _lineWidth=lineWidth;
   this->axisRect()->setRangeZoomFactor(zoomFactor);
   for(int i=0;i<this->graphCount();i++)
   {
      QPen pen=this->graph(i)->pen();
      pen.setWidth(lineWidth);
      this->graph(i)->setPen(pen);
   }
   this->replot();
}

void QWellsPlot::showLegendSlot()
{
   this->legend->setVisible(true);
   this->replot();
   _legendShow=true;
}

void QWellsPlot::hideLegendSlot()
{
   this->legend->setVisible(false);
   this->replot();
   _legendShow=false;
}

void QWellsPlot::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   if(_legendShow)
      menu->addAction("Hide legend", this, SLOT(hideLegendSlot()));
   else
      menu->addAction("Show legend", this, SLOT(showLegendSlot()));
   menu->addAction("Set range", this, SLOT(minMaxDialog()));
   menu->addAction("Set ticks", this, SLOT(setTicksSlot()));
   menu->addSeparator();
   menu->addAction("Plot settings", _plotSettings, SLOT(startDialog()));
   menu->popup(this->mapToGlobal(pos));
}

void QWellsPlot::minMaxDialog()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set range");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * xminLabel = new QLabel("Xmin:");
   layout->addWidget(xminLabel,0,0,1,1);
   QLineEdit * xminLineEdit = new QLineEdit(dialog);
   xminLineEdit->setText(QString::number(this->xAxis->range().lower));
   layout->addWidget(xminLineEdit,0,1,1,1);
   
   QLabel * xmaxLabel = new QLabel("Xmax:");
   layout->addWidget(xmaxLabel,0,2,1,1);
   QLineEdit * xmaxLineEdit = new QLineEdit(dialog);
   xmaxLineEdit->setText(QString::number(this->xAxis->range().upper));
   layout->addWidget(xmaxLineEdit,0,3,1,1);

   QLabel * yminLabel = new QLabel("Ymin:");
   layout->addWidget(yminLabel,1,0,1,1);
   QLineEdit * yminLineEdit = new QLineEdit(dialog);
   yminLineEdit->setText(QString::number(this->yAxis->range().lower));
   layout->addWidget(yminLineEdit,1,1,1,1);

   QLabel * ymaxLabel = new QLabel("Ymax:");
   layout->addWidget(ymaxLabel,1,2,1,1);
   QLineEdit * ymaxLineEdit = new QLineEdit(dialog);
   ymaxLineEdit->setText(QString::number(this->yAxis->range().upper));
   layout->addWidget(ymaxLineEdit,1,3,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      
      this->xAxis->setRange(xminLineEdit->text().toDouble(), xmaxLineEdit->text().toDouble());
      this->yAxis->setRange(yminLineEdit->text().toDouble(), ymaxLineEdit->text().toDouble());
      this->replot();
   }

}

void QWellsPlot::setTicksSlot()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set ticks step");
QGridLayout * layout = new QGridLayout;
   
   QLabel * xTickStep = new QLabel("X tick step:");
   layout->addWidget(xTickStep,0,0,1,1);
   QLineEdit * xLineEdit = new QLineEdit(dialog);
   xLineEdit->setText(QString::number(this->xAxis->tickStep()));
   layout->addWidget(xLineEdit,0,1,1,1);
   
   QLabel * yTickStep = new QLabel("Y drawdown tick step:");
   layout->addWidget(yTickStep,1,0,1,1);
   QLineEdit * yLineEdit = new QLineEdit(dialog);
   yLineEdit->setText(QString::number(this->yAxis->tickStep()));
   layout->addWidget(yLineEdit,1,1,1,1);

   QLabel * y2TickStep = new QLabel("Y static level tick step:");
   layout->addWidget(y2TickStep,2,0,1,1);
   QLineEdit * y2LineEdit = new QLineEdit(dialog);
   y2LineEdit->setText(QString::number(this->yAxis2->tickStep()));
   layout->addWidget(y2LineEdit,2,1,1,1);

   QLabel * automCheck = new QLabel("Auto ticks");
   layout->addWidget(automCheck,3,0,1,1);
   QCheckBox * autoTicks = new QCheckBox;
   layout->addWidget(autoTicks,3,1,1,1);



   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel, Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));
   layout->addWidget(buttonBox,4,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      if(autoTicks->isChecked())
      {
         this->xAxis->setAutoTickStep(true);
         this->xAxis2->setAutoTickStep(true);
         this->yAxis->setAutoTickStep(true);
         this->yAxis2->setAutoTickStep(true);
      }else{
         this->xAxis->setAutoTickStep(false);
         this->xAxis2->setAutoTickStep(false);
         this->yAxis->setAutoTickStep(false);
         this->yAxis2->setAutoTickStep(false);
      }
      this->xAxis->setTickStep(xLineEdit->text().toDouble());
      this->xAxis2->setTickStep(xLineEdit->text().toDouble());
      this->yAxis->setTickStep(yLineEdit->text().toDouble());
      this->yAxis2->setTickStep(y2LineEdit->text().toDouble());
      this->replot();
   }
}

void QWellsPlot::setRangeStaticLevel(QCPRange range)
{
   if(_useStaticLevel)
   {
      this->yAxis2->setLabel("h[m a.s.l.]");
      QCPRange nov;
      nov.lower = (*_staticLevel)-range.lower;
      nov.upper = (*_staticLevel)-range.upper;

      this->yAxis2->setRange(nov);
      this->yAxis2->setRangeReversed(false);
      this->replot();
   }
   else
   {
      this->yAxis2->setLabel("");
      this->yAxis2->setRange(range);
      this->yAxis2->setRangeReversed(true);
      this->replot();
   }
}

void QWellsPlot::showCoord(QMouseEvent * event) 
{
   double x = this->xAxis->pixelToCoord(event->pos().x());
   double y = this->yAxis->pixelToCoord(event->pos().y());
   QString str = "("+QString::number(x)+";"+QString::number(y)+")";
   qDebug()<<_plotSettings->qWells()->statusBar()<<"\n";
   _plotSettings->qWells()->statusBar()->showMessage(str);
}


/*
template<class Result>
void QWellsPlot::setAllData(const QVector<double> & t, const Result * result)
{
   int nwells=_wellPlot.size();
   int npiez=_piezPlot.size();
   for(int iwell=0;iwell<nwells;iwell++)
   {
      if(_wellPlot[iwell]==std::numeric_limits<size_t>::max())
         continue;
      else
         setDataOfPlot(t,result->well(iwell),_wellPlot[iwell]);
   }
   for(int iwell=0;iwell<nwells;iwell++)
   {
      if(_wellWithoutLossPlot[iwell]==std::numeric_limits<size_t>::max())
         continue;
      else
         setDataOfPlot(t,result->wellLosses(iwell),_wellWithoutLossPlot[iwell]);
   }
   for(int ipiez=0;ipiez<npiez;ipiez++)
   {
      if(_wellWithoutLossPlot[ipiez]==std::numeric_limits<size_t>::max())
         continue;
      else
         setDataOfPlot(t,result->piez(ipiez),_piezPlot[ipiez]);
   }
}*/

/*void QWellsPlot::addPlot(const QVector<double> & x, 
      const QVector<double> & y, const QColor & color, const QString & name,
      const size_t iGraph, const size_t style, const size_t scatter)
{
   this->addGraph();
   qDebug()<<" velicina je "<<this->graphCount()<<"\n";
   if(style==0)//then there is no line but only thicks
      this->setScatter(iGraph, scatter);
   else
      this->setLineStyle(color, iGraph, style);
   
//   this->graph(iGraph)->setPen(QPen(color));

   

   this->xAxis2->setVisible(true);
   this->xAxis2->setTickLabels(false);
   this->yAxis2->setVisible(true);
   this->yAxis2->setTickLabels(true);
  // make left and bottom axes always transfer their ranges to right and top axes:
   connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
   connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));

   this->graph()->setName(name);
  // pass data points to graphs:
   this->graph(iGraph)->setData(x, y);
  // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
  this->graph(iGraph)->rescaleAxes();
  // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
  //_plot->graph(1)->rescaleAxes(true);
  // Note: we could have also just called customPlot->rescaleAxes(); instead
  // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
  this->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}*/

/*void QWellsPlot::removePlot(const size_t iGraph)
{
   this->removeGraph(iGraph);
}*/

