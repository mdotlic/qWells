#include "interactionTab.h"

#include "muT/tMuSliderFrame.h"
#include "muT/valueForTMuSlider.h"
#include "tabbedTools/tabbedTools.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "result/result.h"
#include "engine/data.h"
#include "qWells.h"

#include <QFrame>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

InteractionTab::InteractionTab(Data * data,Result * result) : QSplitter(Qt::Horizontal), _data(data)
{    
   QFrame * sPlotFrame = new QFrame;
   QVBoxLayout * sPlotLayout = new QVBoxLayout;
  //trebaju nam dva iscrtavanja
   _splot = new QWellsPlot(_data->noOfWells(), _data->noOfPiezometers(), 
         data->qWells()->plotSettings(), _data->staticLevel()); 
   _splot->setLineWidth(_data->getModel().lineWidth());
   _splot->setGraph("S [m]");
   connect(_data,&Data::timeUnitSet,_splot,&QWellsPlot::setTimeUnit);
   
   _tabbedTools = new TabbedTools(_data,_splot,result);
   addWidget(_tabbedTools);
   
   QFrame * interactionFrame = new QFrame;
   QVBoxLayout * interactionFrameLayout = new QVBoxLayout;
   
   QHBoxLayout * sliderLayout = new QHBoxLayout;
   _sliderT = new TMuSliderFrame(_data,_splot,result,0); // 0 = T
   sliderLayout->addWidget(_sliderT,Qt::AlignCenter);
   _sliderMu = new TMuSliderFrame(_data,_splot,result,1); // 1 = mu
   sliderLayout->addWidget(_sliderMu,Qt::AlignCenter);
   interactionFrameLayout->addLayout(sliderLayout);

   sPlotLayout->addWidget(_splot,Qt::AlignVCenter);
   sPlotFrame->setLayout(sPlotLayout);
   sPlotFrame->setMidLineWidth(2);
   sPlotFrame->setFrameShape(QFrame::StyledPanel);
   sPlotFrame->setFrameShadow(QFrame::Sunken);
   interactionFrameLayout->addWidget(sPlotFrame);
   interactionFrame->setLayout(interactionFrameLayout);
   
   addWidget(interactionFrame);
   
   setFixedHeight(sizeHint().height());

   /// test
//    QVector<double> x(10), y(10), y1(10), y2(10), y3(10), y4(10); 
//    for(size_t i=0;i<10;i++)
//    {
//       x[i]=i;
//       y[i]=i*i;
//       y1[i]=i*i*i/10.0;
//       y2[i]=y1[i]-10.0;
//       y3[i]=y1[i]+10.0;
//       y4[i]=y[i]+15.0;
//    }
//    _splot->addWellPlot(0, QColor(250, 10, 10, 120), 2, "kaoBunar1");
//    _splot->addWellSmPlot(0, QColor(250, 10, 10, 120), 2, "kaoBunar1");
//    _splot->addWellWithoutLossPlot(0, QColor(250, 250, 10, 120), 2, "kaoBunar1");
//    _splot->addPiezPlot(0, QColor(250, 10, 250, 250), 2, "kaoPiez1");
//    _splot->addPiezSmPlot(0, QColor(250, 10, 250, 250), 2, "kaoPiez1");
// 
//    _splot->setDataOfPlot(x,y,_splot->wellPlot(0));
//    _splot->setDataOfPlot(x,y1,_splot->wellSmPlot(0));
//    _splot->setDataOfPlot(x,y2,_splot->wellWithoutLossPlot(0));
//    _splot->setDataOfPlot(x,y3,_splot->piezPlot(0));
//    _splot->setDataOfPlot(x,y4,_splot->piezSmPlot(0));
//    qDebug()<<" hocu da uklonim "<<_splot->piezSmPlot(0)<<"\n";
   //_splot->removePiezSmPlot(0);

   /*
   _splot->addPlot(x,y,QColor(250, 10, 10, 120), "jedan", 0, 2);
   _splot->addPlot(x,y1,QColor(0, 10, 10, 120), "dva", 1, 0, 2);

   _splot->addPlot(x,y2,QColor(30, 40, 255, 150), "tri", 2, 3);
   _splot->addPlot(x,y3,QColor(30, 40, 255, 20), "trib", 3, 3);
   _splot->addPlot(x,y4,QColor(30, 40, 255, 255), "tric", 4, 3);
   _splot->removePlot(1);*/
   //_splot->savePdf("ddd");
   //_splot->setStyle(0, 3);//za qplot treba poslednji broj 2
   //_splot->setLineStyle(0,1);
   
}
