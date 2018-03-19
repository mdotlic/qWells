#include "isoDataTab.h"
#include "../isoList.h"
#include "../visualizationTab.h"
#include "../visResult.h"
#include "../visPicture.h"
#include "engine/data.h"
#include "qWells.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QToolBar>
#include <QDialog>
#include <QDialogButtonBox>
#include <QCoreApplication>
#include <QTime>
#include <QAction>


IsoDataTab::IsoDataTab(Data * data, VisualizationTab * visualizationTab):
   _visualizationTab(visualizationTab), _data(data)
{
   QVBoxLayout * isoDataTabLayout = new QVBoxLayout;
   _useStatic = new QCheckBox("Static level");
   _useStatic->setDisabled(true);
   isoDataTabLayout->addWidget(_useStatic);
   QLabel * isoValue = new QLabel("Isoline values");
   isoDataTabLayout->addWidget(isoValue);
   _isoVal = new QLineEdit;
   QPushButton * automatic = new QPushButton(tr("Automatic"));
   QHBoxLayout * horLayout = new QHBoxLayout;
   horLayout->addWidget(_isoVal);
   horLayout->addWidget(automatic);
   isoDataTabLayout->addLayout(horLayout);
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
   isoDataTabLayout->addWidget(_list); 

   _showButton = new QPushButton(tr("Show"));
   _showButton->setEnabled(false);
   isoDataTabLayout->addWidget(_showButton);
   createActions();
   createToolBars();
   isoDataTabLayout->addWidget(_timeToolBar);
   QGridLayout * timeLayout = new QGridLayout;
   QLabel * timeLabel = new QLabel("Time");
   timeLayout->addWidget(timeLabel, 0, 0);
   _time = new QLabel(" ");
   timeLayout->addWidget(_time, 0, 1);
   _spinBox = new QSpinBox;
   timeLayout->addWidget(_spinBox, 0, 2);
   
   isoDataTabLayout->addLayout(timeLayout);
   
   isoDataTabLayout->addStretch();

   setLayout(isoDataTabLayout);

   disableTimeChange();

   connect(_data, SIGNAL(timeUnitSet(size_t)), this,
         SLOT(setTimeToolTip(size_t)));
   connect(_spinBox, SIGNAL(valueChanged(int)), this, SLOT(setTimeValue(int)));
   connect(_isoVal, SIGNAL(returnPressed()), this, SLOT(addToList()));
   connect(_showButton, SIGNAL(clicked()), this, SLOT(showResult()));
   connect(automatic, SIGNAL(clicked()), this, SLOT(automaticIsoValues()));
   connect(_list, SIGNAL(listChanged()), this, SLOT(listCh()));
   connect(_data->qWells(), SIGNAL(doubleClickPlot(double)), this,
         SLOT(setTime(double)));
   connect(_useStatic, SIGNAL(stateChanged(int)), this, 
         SLOT(staticChanged(int)));

}

void IsoDataTab::setTime(double time)
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

void IsoDataTab::setTimeToolTip(const size_t timeUnit)
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


}

void IsoDataTab::listCh()
{
   _showButton->setEnabled(true);
}

void IsoDataTab::staticChanged(int state)
{
   int count = _list->count();
   std::vector<double> novo(count);
   for(int i=0;i<count;i++)
   {
      if(state)//this is for static level
         novo[i]=(*_data->staticLevel())-_list->item(i)->text().toDouble();
      else//for drawdown
         novo[i]=(*_data->staticLevel())-_list->item(i)->text().toDouble();
   }
   for(int i=0;i<count;i++)
   {//delete whole list
      delete _list->takeItem(0);
   }
   for(int i=0;i<count;i++)
   {
      QListWidgetItem *item = new QListWidgetItem;
      QString number=QString::number(novo[i], 'f',2); 
      item->setData(Qt::DisplayRole, number.toDouble()); 
      _list->addItem(item);
   }
   _visualizationTab->visPicture()->repaint();
}

void IsoDataTab::automaticIsoValues()
{
   double minValue=_visualizationTab->visResult()->minVal();
   double maxValue=_visualizationTab->visResult()->maxVal();
   if(_useStatic->isChecked())
   {
      double oldMax = maxValue;
      maxValue = (*_data->staticLevel()) - minValue;
      minValue = (*_data->staticLevel()) - oldMax;
   }
   if(!_isoVal->isEnabled())
      return;

   QDialog dialog(this);
   dialog.setWindowTitle("Automatic isoline values");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);
   QLabel * minLabel = new QLabel("Minimal calculated drawdown is "+QString::number(minValue));
   QLabel * maxLabel = new QLabel("Maximal calculated drawdown is "+QString::number(maxValue));
   dialogLayout->addWidget(minLabel);
   dialogLayout->addWidget(maxLabel);
   QLabel * numberOfIsoLabel = new QLabel("Number of isolines");
   QLineEdit * num = new QLineEdit(&dialog);
   dialogLayout->addWidget(numberOfIsoLabel);
   dialogLayout->addWidget(num);


   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
   if (dialog.exec() == QDialog::Accepted) {
      int limit = _list->count();
      for(int i=0;i<limit;i++)
      {
         delete _list->takeItem(0);
      }
      int niso=num->text().toInt();

      for(int j=1;j<=niso;j++)
      {
         double original=minValue+j*(maxValue-minValue)/(niso+1);
         QListWidgetItem *item = new QListWidgetItem;
         QString number=QString::number(original, 'f',2); 
         item->setData(Qt::DisplayRole, number.toDouble()); 
         _list->addItem(item);
      }
      emit _list->listChanged();
   }
   
}

void IsoDataTab::addToList()
{
   QListWidgetItem *item = new QListWidgetItem;
   item->setData(Qt::DisplayRole, _isoVal->text().toDouble());
   _list->addItem(item);
   _isoVal->setText("");
   emit _list->listChanged();
}

void IsoDataTab::showResult()
{ 
   _visualizationTab->visResult()->clear();
   int count = _list->count();
   _listNumbers.resize(count);
   for(int i=0;i<count;i++)
   {
      if(_useStatic->isChecked())
         _listNumbers[i]=(*_data->staticLevel())-_list->item(i)->text().toDouble();
      else
         _listNumbers[i]=_list->item(i)->text().toDouble();
   }
   _visualizationTab->visResult()->calcMarchingSquares(_listNumbers);
   _visualizationTab->visPicture()->repaint();
   _showButton->setEnabled(false);
   _data->getModel().pushIsolineValue(_listNumbers);
   enableTimeChange();
}

void IsoDataTab::setTimeValue(int value)
{
   _time->setText(QString::number(_data->timeInSMHD()[value]));
}

void IsoDataTab::first()
{
   if(_data->timeInSMHD().size()!=0)
   {
      _spinBox->setValue(0);
      _visualizationTab->visPicture()->repaint();
   }
}
void IsoDataTab::previous()
{
   int value=_spinBox->value();
   if(value!=0)
   {
      _spinBox->setValue(value-1);
      _visualizationTab->visPicture()->repaint();
   }
}
void IsoDataTab::play()
{
   double division=_data->t().back() /
      _visualizationTab->visPicture()->playDuration();
   int ntimes=_data->t().size();
   for(int i=0;i<ntimes-1;i++)
   {
      double timestep=(_data->t()[i+1] - _data->t()[i])/division;
      QTime moveOn = QTime::currentTime().addMSecs(1000*timestep);
      while(QTime::currentTime()<moveOn)
      {
         QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
      }
      _spinBox->setValue(i);
      _visualizationTab->visPicture()->repaint();
      //_visualizationTab->visPicture()->image()->save(QString::number(i)+".png");  //za izbacivanje svake slike pojedinacno
   }
   _spinBox->setValue(ntimes-1);
   _visualizationTab->visPicture()->repaint();
 /*  QTime moveOn = QTime::currentTime().addMSecs(1000);
   qDebug()<<" pre starta \n";
   while(QTime::currentTime()<moveOn)
   {
      QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
   }*/
}
void IsoDataTab::next()
{
   int value=_spinBox->value();
   if(value<_data->timeInSMHD().size()-1)
   {
      _spinBox->setValue(value+1);
      _visualizationTab->visPicture()->repaint();
   }
}
void IsoDataTab::last()
{
   if(_data->timeInSMHD().size()!=0)
   {
      _spinBox->setValue(_data->timeInSMHD().size()-1);
      _visualizationTab->visPicture()->repaint();
   }
}

void IsoDataTab::createActions()
{
   _firstAct = new QAction(QIcon(":/resources/images/go-first-view.png"), tr("&First"), this);
   connect(_firstAct, SIGNAL(triggered()), this, SLOT(first()));

   _prevAct = new QAction(QIcon(":/resources/images/go-first-5.png"), tr("&Previous"), this);
   connect(_prevAct, SIGNAL(triggered()), this, SLOT(previous()));

   _playAct = new QAction(QIcon(":/resources/images/go-next-view.png"), tr("&Play"), this);
   connect(_playAct, SIGNAL(triggered()), this, SLOT(play()));

   _nextAct = new QAction(QIcon(":/resources/images/go-last-4.png"), tr("&Next"), this);
   connect(_nextAct, SIGNAL(triggered()), this, SLOT(next()));

   _lastAct = new QAction(QIcon(":/resources/images/go-last-view.png"), tr("&Last"), this);
   connect(_lastAct, SIGNAL(triggered()), this, SLOT(last()));
}

void IsoDataTab::createToolBars()
{
   _timeToolBar = new QToolBar;
   _timeToolBar->addAction(_firstAct);
   _timeToolBar->addAction(_prevAct);
   _timeToolBar->addAction(_playAct);
   _timeToolBar->addAction(_nextAct);
   _timeToolBar->addAction(_lastAct);
}

void IsoDataTab::refresh()
{
   int max=_data->timeInSMHD().size();
   if(max==0)
      _spinBox->setMaximum(0);
   else
   {
      _spinBox->setMaximum(max-1);
      _time->setText(QString::number(_data->timeInSMHD()[_spinBox->value()]));
   }
   /*_xmin->setText(QString::number(_data->getModel().picXmin()));
   _xmax->setText(QString::number(_data->getModel().picXmax()));
   _ymin->setText(QString::number(_data->getModel().picYmin()));
   _ymax->setText(QString::number(_data->getModel().picYmax()));*/
}

int IsoDataTab::timeStep()
{
   return _spinBox->value();
}

QString IsoDataTab::isolineVal(const int i)
{

   QString str;
   if(_useStatic->isChecked())
      str= QString::number((*_data->staticLevel())-_listNumbers[i], 'f', 2);
   else
      str= QString::number(_listNumbers[i], 'f', 2);
   return str;
}

void IsoDataTab::disableTimeChange()
{
   _timeToolBar->setEnabled(false);
   _spinBox->setEnabled(false);
}

void IsoDataTab::enableTimeChange()
{
   _timeToolBar->setEnabled(true);
   _spinBox->setEnabled(true);
}

void IsoDataTab::enableUseStatic(bool b)
{
   _useStatic->setDisabled(!b);
}

