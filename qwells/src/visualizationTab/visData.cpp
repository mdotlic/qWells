#include "visData.h"
#include "visualizationTab.h"
#include "visResult.h"
#include "visPicture.h"
#include "isoList.h"
#include "engine/data.h"
#include "linesTabs/isoDataTab.h"
#include "linesTabs/streamDataTab.h"
#include "linesTabs/tabs.h"
#include "qWells.h"
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <QMessageBox>

VisData::VisData(Data * data, VisualizationTab * vizualizationTab):_data(data), 
   _visualizationTab(vizualizationTab)
{  
   QGridLayout * visDataLayout = new QGridLayout;

   QLabel * xminLabel = new QLabel("Xmin");
   visDataLayout->addWidget(xminLabel, 0, 0);
   _xmin = new QLineEdit;
   _xmin->setText(QString::number(data->getModel().visXmin(), 'f', 2));
   visDataLayout->addWidget(_xmin, 0, 1);

   QLabel * xmaxLabel = new QLabel("Xmax");
   visDataLayout->addWidget(xmaxLabel, 0, 2);
   _xmax = new QLineEdit;
   _xmax->setText(QString::number(data->getModel().visXmax(), 'f', 2));
   visDataLayout->addWidget(_xmax, 0, 3);
   
   QLabel * yminLabel = new QLabel("Ymin");
   visDataLayout->addWidget(yminLabel, 1, 0);
   _ymin = new QLineEdit;
   _ymin->setText(QString::number(data->getModel().visYmin(), 'f', 2));
   visDataLayout->addWidget(_ymin, 1, 1);
   
   QLabel * ymaxLabel = new QLabel("Ymax");
   visDataLayout->addWidget(ymaxLabel, 1, 2);
   _ymax = new QLineEdit;
   _ymax->setText(QString::number(data->getModel().visYmax(), 'f', 2));
   visDataLayout->addWidget(_ymax, 1, 3);


   QLabel * divLabel = new QLabel("Division");
   visDataLayout->addWidget(divLabel, 2, 0);
   QLabel * xLabel = new QLabel("X");
   visDataLayout->addWidget(xLabel, 3, 0);
   _xdiv = new QLineEdit;
   _xdiv->setText(QString::number(data->getModel().visXdiv()));
   visDataLayout->addWidget(_xdiv, 3, 1);

   QLabel * yLabel = new QLabel("Y");
   visDataLayout->addWidget(yLabel, 3, 2);
   _ydiv = new QLineEdit;
   _ydiv->setText(QString::number(data->getModel().visYdiv()));
   visDataLayout->addWidget(_ydiv, 3, 3);

   QVBoxLayout * allLayout = new QVBoxLayout;
   allLayout->addLayout(visDataLayout);
   _calculateButton = new QPushButton(tr("Calculate"));
   allLayout->addWidget(_calculateButton);

   //_isoDataTab = new IsoDataTab(_data, _isolineTab);
   _tabs = new Tabs(_data, _visualizationTab);
   allLayout->addWidget(_tabs);

   //qDebug()<<" imas "<<_xmin->text().toDouble()<<" aaa "<<_xmax->text().toDouble()<<" "<<_ymin->text().toDouble()<<" "<< _ymax->text().toDouble()<<"\n";
  // _visualizationTab->visPicture()->updateMinMax(_xmin->text().toDouble(), 
  // _xmax->text().toDouble(),_ymin->text().toDouble(), _ymax->text().toDouble());
     
  /* QLabel * isoValue = new QLabel("Isoline values");
   allLayout->addWidget(isoValue);
   _isoVal = new QLineEdit;
   QPushButton * automatic = new QPushButton(tr("Automatic"));
   QHBoxLayout * horLayout = new QHBoxLayout;
   horLayout->addWidget(_isoVal);
   horLayout->addWidget(automatic);
   allLayout->addLayout(horLayout);
   _isoVal->setEnabled(false);
   _list = new IsoList;
   int niso=_data->getModel().isoValues().size();
   for(int j=0;j<niso;j++)
   {
      QListWidgetItem *item = new QListWidgetItem;
      item->setData(Qt::DisplayRole, _data->getModel().isoValues()[j]); 
      _list->addItem(item);
   }
   _list->setSortingEnabled(true);
   allLayout->addWidget(_list); 

   _showButton = new QPushButton(tr("Show"));
   _showButton->setEnabled(false);
   allLayout->addWidget(_showButton);
   createActions();
   createToolBars();
   allLayout->addWidget(_timeToolBar);
   QGridLayout * timeLayout = new QGridLayout;
   QLabel * timeLabel = new QLabel("Time");
   timeLayout->addWidget(timeLabel, 0, 0);
   _time = new QLabel(" ");
   timeLayout->addWidget(_time, 0, 1);
   _spinBox = new QSpinBox;
   timeLayout->addWidget(_spinBox, 0, 2);*/
   
  // allLayout->addLayout(timeLayout);
   
   allLayout->addStretch();

   setLayout(allLayout);


// connect(_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setTimeValue(int)));
   connect(_calculateButton, SIGNAL(clicked()), this, SLOT(calculate()));
   connect(_xmin, SIGNAL(textChanged(const QString)), this,
         SLOT(enableCalc(const QString)));
   connect(_xmax, SIGNAL(textChanged(const QString)), this,
         SLOT(enableCalc(const QString)));
   connect(_ymin, SIGNAL(textChanged(const QString)), this,
         SLOT(enableCalc(const QString)));
   connect(_ymax, SIGNAL(textChanged(const QString)), this,
         SLOT(enableCalc(const QString)));
   connect(_xdiv, SIGNAL(textChanged(const QString)), this,
         SLOT(enableCalc(const QString)));
   connect(_ydiv, SIGNAL(textChanged(const QString)), this,
         SLOT(enableCalc(const QString)));
 /*  connect(_isoVal, SIGNAL(returnPressed()), this, SLOT(addToList()));
   connect(_showButton, SIGNAL(clicked()), this, SLOT(showResult()));
   connect(automatic, SIGNAL(clicked()), this, SLOT(automaticIsoValues()));
   connect(_list, SIGNAL(listChanged()), this, SLOT(listCh()));*/

}

void VisData::enableCalc(const QString)
{
   _calculateButton->setEnabled(true);
}


void VisData::calculate()
{
   _calculateButton->setEnabled(false);
   if(_xmin->text().toDouble()>=_xmax->text().toDouble())
   {
      QString text = "Calculation is not performed because Xmin >= Xmax !!!";
      QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("ERROR"),text,
                                            QMessageBox::Ok);
      newError->exec();
      return;
   }
   if(_ymin->text().toDouble()>=_ymax->text().toDouble())
   {
      QString text = "Calculation is not performed because Ymin >= Ymax !!!";
      QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("ERROR"),text,
                                            QMessageBox::Ok);
      newError->exec();
      return;
   }

   double xrange = _xmax->text().toDouble() - _xmin->text().toDouble();
   double yrange = _ymax->text().toDouble() - _ymin->text().toDouble();

   if(xrange/yrange>10 || yrange/xrange>10)
   {
      QString text = "Calculation is not performed because x-range is "+QString::number(xrange/yrange)+" y-range!!!";
      QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("ERROR"),text,
                                            QMessageBox::Ok);
      newError->exec();
      return;
   }

   _data->getModel().setVisXmin(_xmin->text().toDouble());
   _data->getModel().setVisXmax(_xmax->text().toDouble());
   _data->getModel().setVisYmin(_ymin->text().toDouble());
   _data->getModel().setVisYmax(_ymax->text().toDouble());
   _visualizationTab->visPicture()->updateMinMax(_xmin->text().toDouble(), 
   _xmax->text().toDouble(),_ymin->text().toDouble(), _ymax->text().toDouble());
   _tabs->isoDataTab()->isoVal()->setEnabled(false);
   _visualizationTab->visResult()->calculateResult(_xmin->text().toDouble(), _xmax->text().toDouble(),_ymin->text().toDouble(), _ymax->text().toDouble(),
         _xdiv->text().toInt(), _ydiv->text().toInt());
   _tabs->isoDataTab()->isoVal()->setEnabled(true);
   if(_tabs->isoDataTab()->list()->count()!=0)
      _tabs->isoDataTab()->showButton()->setEnabled(true);
   _tabs->streamDataTab()->checkAll();

   _data->getModel().pushVisData(_xmin->text().toDouble(), _xmax->text().toDouble(),_ymin->text().toDouble(), _ymax->text().toDouble(),
         _xdiv->text().toInt(), _ydiv->text().toInt());

   _tabs->isoDataTab()->disableTimeChange();
}

void VisData::refresh()
{
   _tabs->isoDataTab()->refresh();
   _tabs->streamDataTab()->refresh();
}
