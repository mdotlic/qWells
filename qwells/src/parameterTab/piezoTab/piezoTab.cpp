#include "piezoTab.h"
#include "qWells.h"
#include "mainMenu.h"
#include "engine/data.h"
#include "parameterTab/tableSize.h"
#include "piezoTable.h"
#include "piezoDataTable.h"
#include "widgets/tables/imExFrame.h"
#include "widgets/tables/lineEdit.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QPushButton>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

PiezoTab::PiezoTab(QWells * qWells, TableSize * tableSize) : QFrame(), _qWells(qWells), _data(qWells->data()), _tableSize(tableSize)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab START\n";
#endif
   
   connect(_qWells,&QWells::modelLoaded,this,&PiezoTab::loadModel);
   connect(_qWells->tableSettings(),&TableSettings::setMaxRows,
           this,&PiezoTab::setMaxRowsShown);
   
   _piezoTabLayout = new QGridLayout;
   _piezoTabLayout->setAlignment(Qt::AlignTop);
   _piezoTabLayout->setColumnMinimumWidth(0,20);
   //_piezoTabLayout->setColumnStretch(0,1);
   _piezoTabLayout->setColumnMinimumWidth(2,20);
   //_piezoTabLayout->setColumnStretch(1,1);
   _piezoTabLayout->setColumnMinimumWidth(4,20);
   _piezoTabLayout->setColumnStretch(6,3);
   
   QLabel * piezoNameLabel = new QLabel(tr("DEFINE NEW PIEZOMETER"));
   _piezoTabLayout->addWidget(piezoNameLabel,0,3,Qt::AlignTop);
   
   ImportExportFrame * imExFrame = new ImportExportFrame();
   _piezoTabLayout->addWidget(imExFrame,1,1,Qt::AlignTop);
   _piezoDataLabel = new QLabel(tr("Default"));
   _piezoTabLayout->addWidget(_piezoDataLabel,0,5,Qt::AlignTop);
   _piezoDataLabel->hide();
   
   constructPiezoTable();
   connect(imExFrame->importButton(),SIGNAL(clicked()),
           _qWells->mainMenu(),SLOT(importPiezometer()));
   connect(imExFrame->exportButton(),SIGNAL(clicked()),
           this,SLOT(prepareToExportPiezometer()));
   
   _dataTablesFrame = new QFrame;
   _dataTablesLayout = new QHBoxLayout;
   _dataTablesLayout->setAlignment(Qt::AlignTop);
   _dataTablesLayout->setContentsMargins(0,0,0,0);
   _dataTablesFrame->setFixedWidth(_piezoTable->size().width()*1.5);
   _dataTablesFrame->setFrameShape(QFrame::NoFrame);
   _dataTablesFrame->setLayout(_dataTablesLayout);
   _piezoTabLayout->addWidget(_dataTablesFrame,1,5,Qt::AlignTop);
   
   _timeWarningLabel = new QLabel(QString(tr("WARNING : Time already exists!")));
   _timeWarningLabel->setAlignment(Qt::AlignCenter);
   _timeWarningLabel->setStyleSheet("color:red;");
   _timeWarningLabel->setFixedWidth(200);
   _piezoTabLayout->addWidget(_timeWarningLabel,2,5,Qt::AlignLeft);
   _timeWarningLabel->hide();
   _rowMoveLabel = new QLabel(QString(tr("WARNING : Time not in ascending order, row will be moved.")));
   _rowMoveLabel->setAlignment(Qt::AlignCenter);
   _rowMoveLabel->setStyleSheet("color:red;");
   _rowMoveLabel->setFixedWidth(400);
   _piezoTabLayout->addWidget(_rowMoveLabel,2,5,Qt::AlignLeft);
   _rowMoveLabel->hide();
   
   setLayout(_piezoTabLayout);
   
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab END\n";
#endif
}

void PiezoTab::constructPiezoTable(const bool modelExists)
{
   if (modelExists)
   {
      const int rows = _data->noOfPiezometers();
#ifdef QDEBUG_ALL
      qDebug()<<"PiezoTab::constructPiezoTable() START | loading new model | noOfPiezometers = "<<rows<<"\n";
#endif
      _piezoTable = new PiezoTable(_data,_tableSize,rows,true);
      for (int i=0;i<rows;i++) _piezoTable->loadData(i);
   }
   else _piezoTable = new PiezoTable(_data,_tableSize);
   _piezoTabLayout->addWidget(_piezoTable,1,3,Qt::AlignTop);
   connect(_piezoTable,&WellPiezoTable::rowMade,
           this,&PiezoTab::addDataTable,Qt::UniqueConnection);
   connect(_piezoTable,&WellPiezoTable::rowRemoved,
           this,&PiezoTab::removePiezometer,Qt::UniqueConnection);
   connect(_piezoTable,&QTableWidget::cellActivated,
           this,&PiezoTab::showCorrectDataTable,Qt::UniqueConnection);
   connect(_data,&Data::piezometerImported,
           this,&PiezoTab::prepareToLoadData,Qt::UniqueConnection);
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::constructPiezoTable() END\n";
#endif
}
 
void PiezoTab::loadModel()
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::loadModel() START\n";
#endif
   const int noOfOldPiezometers = _dataTables.size();
   for (int i=noOfOldPiezometers-1;i==0;i--) delete _dataTables[i];
   _dataTables.resize(0);
   delete _piezoTable;
   //_tableSize->resetAll();
   const int noOfPiezometers = _data->noOfPiezometers();
   constructPiezoTable(true);
   for (int i=0;i<noOfPiezometers;i++)
   {
      _auxTable.resize(0);
      _auxValidTable.resize(0);
      _data->constructAuxPiezoDataTable(_auxTable,_auxValidTable,i);
      const int rows = _auxTable.size();
      addDataTable(i,true,rows);
   }
   _piezoTable->setCurrentCell(noOfPiezometers,0);
   emit loadBoundaryWidget();
   emit loadInteractionTab(1);
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::loadModel() END\n";
#endif   
}

void PiezoTab::prepareToExportPiezometer()
{
   const int piezoID = _piezoTable->currentRow();
   if (piezoID != _piezoTable->rowCount()-1) _qWells->mainMenu()->exportPiezometer(piezoID);
}

void PiezoTab::prepareToLoadData(const int piezoID, const int noOfDataRows)
{
   _piezoTable->makeNewRow(piezoID,true,true); // row,doInsertRow,loadRow
   _auxTable.resize(0);
   _auxValidTable.resize(0);
   _data->constructAuxPiezoDataTable(_auxTable,_auxValidTable,piezoID);
   addDataTable(piezoID,true,noOfDataRows);
}

void PiezoTab::addDataTable(const int piezoNo, const bool modelExists, const int rows)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::addDataTable START | wellNo = "<<piezoNo<<" (with "<<rows<<" rows when "<<modelExists<<" =  true)\n";
#endif
   PiezoDataTable * newDataTable;
   if (modelExists)
   {
      newDataTable = new PiezoDataTable(this,_data,_tableSize,piezoNo,rows,true);
      newDataTable->loadTable(_auxTable,_auxValidTable);
   }
   else newDataTable = new PiezoDataTable(this,_data,_tableSize,piezoNo,rows);
   _dataTablesLayout->addWidget(newDataTable,Qt::AlignTop);
   _dataTables.push_back(newDataTable);
   newDataTable->hide();
   _piezoDataLabel->hide();
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::addDataTable END | noOfDataTables = "<<_dataTables.size()<<"\n";
#endif 
}

void PiezoTab::removePiezometer(const int tableNo)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::removePiezometer START | removing piezoID = "<<tableNo<<" from Data and deleting its DataTable\n";
#endif
   _data->removePiezometer(tableNo);
   _dataTables.erase(_dataTables.begin() + tableNo);
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::removePiezometer END\n";
#endif
}

void PiezoTab::showCorrectDataTable(const int piezoNo, const int)
{
   const int noOfPiezometers = _piezoTable->rowCount()-1;
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::showCorrectDataTable() START | noOfPiezometers = "<<noOfPiezometers<<"\n";
#endif
   if (_dataTables.size() == 0) return;
   for (int n=0;n<noOfPiezometers;n++)
   {
      if (n == piezoNo) 
      {
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTab::showCorrectDataTable() | show piezometer with ID = "<<piezoNo<<"\n";
#endif
         _dataTables[n]->show();
         const QString piezoName = static_cast<LineEdit*>(_piezoTable->cellWidget(n,0))->text();
         const QString labelText = QString("ENTER DATA FOR PIEZOMETER ").append(piezoName);
         _piezoDataLabel->setText(labelText);
         _piezoDataLabel->show();
      }
      else 
      {
         _dataTables[n]->hide();
      }
   }
   if (piezoNo>=noOfPiezometers) _piezoDataLabel->hide();
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::showCorrectDataTable() END\n";
#endif
}

void PiezoTab::setMaxRowsShown(const int)
{
   _piezoTable->adjustTableSize();
   const int noOfPiezometers = _dataTables.size();
   for (int i=0;i<noOfPiezometers;i++) _dataTables[i]->adjustTableSize();
}