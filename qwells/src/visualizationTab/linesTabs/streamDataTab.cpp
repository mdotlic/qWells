#include "streamDataTab.h"
#include "engine/data.h"
#include "../visualizationTab.h"
#include "../visResult.h"
#include "../visPicture.h"
#include "qWells.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QDebug>

StreamDataTab::StreamDataTab(Data * data, VisualizationTab * visualizationTab):_data(data), _visualizationTab(visualizationTab)
{
   QVBoxLayout * allLayout = new QVBoxLayout;
   QGridLayout * streamLayout = new QGridLayout;
   QLabel * porosityLabel = new QLabel("Porosity");
   streamLayout->addWidget(porosityLabel, 0, 0);

   _porosity = new QLineEdit;
   _porosity->setText(QString::number(_data->getModel().porosity()));
   streamLayout->addWidget(_porosity, 0, 1);

   QLabel * thickLabel = new QLabel("Layer thickness");
   streamLayout->addWidget(thickLabel, 1, 0);

   _thickness = new QLineEdit;
   _thickness->setText(QString::number(_data->getModel().thickness()));
   streamLayout->addWidget(_thickness, 1, 1);
   allLayout->addLayout(streamLayout);
   _list = new QListWidget;
   
   int nwells = _data->noOfWells();  
   _checked.resize(nwells, false);
   for(int i=0;i<nwells;i++)
   {
      QListWidgetItem * item = new QListWidgetItem(_data->getWell(i).name(),
            _list);
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      item->setCheckState(Qt::Unchecked);
   }

   allLayout->addWidget(_list);

   QGridLayout * timeLayout = new QGridLayout;
   QLabel * timeLabel = new QLabel("Choose time");
   timeLayout->addWidget(timeLabel, 0, 0);
   _time = new QLabel(" ");
   timeLayout->addWidget(_time, 0, 1);
   _spinBox = new QSpinBox;
   _spinBox->setValue(_data->getModel().timeStream());
   timeLayout->addWidget(_spinBox, 0, 2);
   QLabel * noStreamLines = new QLabel("No. of streamlines per well");
   timeLayout->addWidget(noStreamLines, 1,0,1,2);
   _nspinBox = new QSpinBox;
   _nspinBox->setMinimum(2);
   _nspinBox->setValue(_data->getModel().noStreamLines());
   timeLayout->addWidget(_nspinBox, 1, 2);
   QLabel * length = new QLabel("Time length");
   timeLayout->addWidget(length, 2, 0);
   _timeLength = new QLineEdit;
   _timeLength->setText(QString::number(_data->getModel().timeLength()));
   timeLayout->addWidget(_timeLength, 2,1,1,2);
   QLabel * sample = new QLabel("Time sample");
   timeLayout->addWidget(sample, 3, 0);
   _timeSample = new QLineEdit;
   timeLayout->addWidget(_timeSample, 3,1,1,2);
   allLayout->addLayout(timeLayout);


   _streamShowButton = new QPushButton(tr("Show"));
   _streamShowButton->setEnabled(false);

   allLayout->addWidget(_streamShowButton);
   
   setLayout(allLayout);

   connect(_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setTimeValue(int)));
   connect(_nspinBox, SIGNAL(valueChanged(int)), this, SLOT(checkAllInt(int)));
   connect(_streamShowButton, SIGNAL(clicked()), 
         this, SLOT(streamShowResult()));

   connect(_list, SIGNAL(itemClicked(QListWidgetItem*)), this, 
         SLOT(listClick(QListWidgetItem*)));

   connect(_porosity, SIGNAL(textChanged(const QString)), this, 
         SLOT(checkAllString(const QString)));
   connect(_thickness, SIGNAL(textChanged(const QString)), this, 
         SLOT(checkAllString(const QString)));
   connect(_timeSample, SIGNAL(textChanged(const QString)), this, 
         SLOT(checkAllString(const QString)));
   connect(_timeLength, SIGNAL(textChanged(const QString)), this, 
         SLOT(checkAllString(const QString)));
   connect(_data->qWells(), SIGNAL(doubleClickPlot(double)), this,
         SLOT(setTime(double)));
   connect(_data, SIGNAL(timeUnitSet(size_t)), this,
         SLOT(setTimeToolTip(size_t)));

}

void StreamDataTab::setTime(double time)
{

   int times=_data->timeInSMHD().size();
   if(times<1)
      return;
   else if(times==1)
   {
      _spinBox->setValue(0);
      return;
   }
   for(int i=0;i<times-1;i++)
   {
      if(time<_data->timeInSMHD()[i])           
      {
         if((_data->timeInSMHD()[i]-time)<(_data->timeInSMHD()[i+1]-time))
            _spinBox->setValue(i);
         else
            _spinBox->setValue(i+1);
         return;         
      }
   }
   _spinBox->setValue(times-1);
}

void StreamDataTab::setTimeToolTip(const size_t timeUnit)
{
   QString text;
   switch (timeUnit)
   {
      case 0:
         text = "Time unit : second";
         break;
      case 1:
         text = "Time unit : minute";
         break;
      case 2:
         text = "Time unit : hour";
         break;
      case 3:
         text = "Time unit : day";
         break;
      default:
         text = "ERROR";
   }
   _time->setToolTip(text);
   _timeLength->setToolTip(text);
   _timeSample->setToolTip(text);
}

void StreamDataTab::listClick(QListWidgetItem * item)
{
   int i=_list->row(item);
   bool old=_checked[i];
   _checked[i]=item->checkState();
   if(item->checkState())
   {
      if(!old)
      {
         checkAll();
      }
   }else
      checkAll();
}


void StreamDataTab::refresh()
{
   int max=_data->timeInSMHD().size();
   if(max==0)
      _spinBox->setMaximum(0);
   else
   {
      _spinBox->setMaximum(max-1);
      //_spinBox->setValue(_data->getModel().timeStream());
      _time->setText(QString::number(_data->timeInSMHD()[_spinBox->value()]));
   }  
   
   int limit = _list->count();
   for(int i=0;i<limit;i++)
   {
      delete _list->takeItem(0);
   }
   int nwells = _data->noOfWells();  

   _checked.resize(nwells, false);
   for(int i=0;i<nwells;i++)
   {
      QListWidgetItem * item = new QListWidgetItem(_data->getWell(i).name(),
            _list);
      item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
      if(_checked[i])
         item->setCheckState(Qt::Checked);
      else
         item->setCheckState(Qt::Unchecked);
   }
   checkAll();
}

void StreamDataTab::setTimeValue(const int value)
{
   _time->setText(QString::number(_data->timeInSMHD()[value]));
  checkAll(); 
}

void StreamDataTab::streamShowResult()
{
   _visualizationTab->visResult()->clearStreamLines();
   if(_porosity->text().isEmpty())
   {
      _data->qWells()->error("Porosity is not set. Streamlines can not be shown.");
      return;
   }
   if(_porosity->text().toDouble()<=0.0 || _porosity->text().toDouble()>=1.0)
   {
      _data->qWells()->error("Porosity must be beetwen 0 and 1. Streamlines can not be shown.");
      return;
   }
   if(_thickness->text().isEmpty())
   {
      _data->qWells()->error("Thickness of the layer is not set. Streamlines can not be shown.");
      return;
   }
   if(_thickness->text().toDouble()<=0.0)
   {
      _data->qWells()->error("Thickness of the layer must be positive. Streamlines can not be shown.");
      return;
   }


   for(size_t i=0;i<_checked.size();i++)
   {
      if(_checked[i])
            break;
      if(i==_checked.size()-1)
      {
         _data->qWells()->error("No well is checked. Streamlines can not be shown.");
         return;
      }
   }

   if(_timeLength->text().isEmpty())
   {
      _data->qWells()->error("Time length of the streamlines is not set. Streamlines can not be shown.");
   }
   if(_timeLength->text().toDouble()<=0.0)
   {
      _data->qWells()->error("Time length of the streamlines must be positive. Streamlines can not be shown.");
      return;
   }
   
   if(_timeSample->text().isEmpty())
   {
      _data->qWells()->error("Time sample of the streamlines is not set. Streamlines can not be shown.");
      return;
   }
   if(_timeSample->text().toDouble()<=0.0)
   {
      _data->qWells()->error("Time sample of the streamlines must be positive. Streamlines can not be shown.");
      return;
   }

   _data->getModel().pushStreamData(_porosity->text().toDouble(), _thickness->text().toDouble(), _timeLength->text().toDouble(), _spinBox->value(), _nspinBox->value());
   double K=_data->getModel().bigT()/_thickness->text().toDouble();
   for(int i=0; i<_list->count(); i++)
   {
      if(_list->item(i)->checkState())
         _visualizationTab->visResult()->calcStreamlines(i, _nspinBox->value(),
               K, _porosity->text().toDouble(), _spinBox->value(), 
               _timeLength->text().toDouble()*_data->convertTimeUnit(),
               _timeSample->text().toDouble()*_data->convertTimeUnit());
   }
   _visualizationTab->visPicture()->repaint();
   _streamShowButton->setEnabled(false);
}


void StreamDataTab::checkAll()
{
   if(_porosity->text().isEmpty() || _thickness->text().isEmpty() || 
         _timeLength->text().isEmpty() || 
         !_visualizationTab->visResult()->resultsExist())
      return;
   for(size_t i=0;i<_checked.size();i++)
   {
      if(_checked[i])
         break;
      if(i==_checked.size()-1)
         return;
   }
   _streamShowButton->setEnabled(true);
}

void StreamDataTab::checkAllInt(const int)
{
   checkAll();
}

void StreamDataTab::checkAllString(const QString)
{
   checkAll();
}
