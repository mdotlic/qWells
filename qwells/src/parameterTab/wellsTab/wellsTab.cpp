#include "wellsTab.h"
#include "wellsTable.h"
#include "wellsDataTable.h"

#include "qWells.h"
#include "mainMenu.h"
#include "engine/data.h"
#include "parameterTab/parameterTab.h"
#include "parameterTab/tableSize.h"
#include "parameterTab/boundaryWidget.h"
#include "parameterTab/boundaryWidget/boundaryDataFrame.h"
#include "widgets/tables/imExFrame.h"
#include "widgets/tables/lineEdit.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QHeaderView>
#include <QPushButton>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

WellsTab::WellsTab(QWells * qWells, ParameterTab * parameterTab, TableSize * tableSize) : QFrame(), _qWells(qWells), _parameterTab(parameterTab), _data(qWells->data()), _tableSize(tableSize)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab START\n";
#endif
   
   connect(_qWells,&QWells::modelLoaded,this,&WellsTab::loadModel);
   connect(_data,&Data::bigAModified,
           this,&WellsTab::setBigAModified,Qt::UniqueConnection);
   connect(_data,&Data::bigBModified,
           this,&WellsTab::setBigBModified,Qt::UniqueConnection);
   connect(_data,&Data::initialBigASet,
           this,&WellsTab::setInitialBigA,Qt::UniqueConnection);
   connect(_data,&Data::initialBigBSet,
           this,&WellsTab::setInitialBigB,Qt::UniqueConnection);
   connect(_qWells->tableSettings(),&TableSettings::setMaxRows,
           this,&WellsTab::setMaxRowsShown);
   
   _wellsTabLayout = new QGridLayout;
   _wellsTabLayout->setAlignment(Qt::AlignTop);
   _wellsTabLayout->setColumnMinimumWidth(0,20);
   _wellsTabLayout->setColumnMinimumWidth(2,20);
   _wellsTabLayout->setColumnMinimumWidth(4,20);
   _wellsTabLayout->setColumnStretch(6,2);
   
   QLabel * wellsNameLabel = new QLabel(tr("DEFINE NEW WELL"));
   _wellsTabLayout->addWidget(wellsNameLabel,0,3,Qt::AlignTop);
   
   ImportExportFrame * imExFrame = new ImportExportFrame();
   _wellsTabLayout->addWidget(imExFrame,1,1,Qt::AlignTop);
   _wellsDataLabel = new QLabel(tr("Default"));
   _wellsTabLayout->addWidget(_wellsDataLabel,0,5,Qt::AlignTop);
   _wellsDataLabel->hide();
   
   constructWellsTable();
   connect(imExFrame->importButton(),SIGNAL(clicked()),
           _qWells->mainMenu(),SLOT(importWell()));
   connect(imExFrame->exportButton(),SIGNAL(clicked()),
           this,SLOT(prepareToExportWell()));
   
   _dataTablesFrame = new QFrame;
   _dataTablesLayout = new QHBoxLayout;
   _dataTablesLayout->setAlignment(Qt::AlignTop);
   _dataTablesLayout->setContentsMargins(0,0,0,0);
   //_dataTablesFrame->setFixedWidth(_wellsTable->size().width()*1.5);
   _dataTablesFrame->setFrameShape(QFrame::NoFrame);
   _dataTablesFrame->setLayout(_dataTablesLayout);
   _wellsTabLayout->addWidget(_dataTablesFrame,1,5,Qt::AlignTop);
   
   _timeWarningLabel = new QLabel(QString(tr("WARNING : Time already exists!")));
   _timeWarningLabel->setAlignment(Qt::AlignCenter);
   _timeWarningLabel->setStyleSheet("color:red;");
   _timeWarningLabel->setFixedWidth(200);
   _wellsTabLayout->addWidget(_timeWarningLabel,2,5,Qt::AlignLeft);
   _timeWarningLabel->hide();
   _rowMoveLabel = new QLabel(QString(tr("WARNING : Time not in ascending order, row will be moved.")));
   _rowMoveLabel->setAlignment(Qt::AlignCenter);
   _rowMoveLabel->setStyleSheet("color:red;");
   _rowMoveLabel->setFixedWidth(400);
   _wellsTabLayout->addWidget(_rowMoveLabel,2,5,Qt::AlignLeft);
   _rowMoveLabel->hide();
   
   setLayout(_wellsTabLayout);
   
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab END\n";
#endif
}

void WellsTab::constructWellsTable(const bool modelExists)
{
   if (modelExists)
   {
      const int rows = _data->noOfWells();
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::constructWellsTable() START | loading new model | noOfWells = "<<rows<<"\n";
#endif
      _wellsTable = new WellsTable(_data,_tableSize,rows,true);
      for (int i=0;i<rows;i++) _wellsTable->loadData(i);
   }
   else _wellsTable = new WellsTable(_data,_tableSize);
   _wellsTabLayout->addWidget(_wellsTable,1,3,Qt::AlignTop);
   connect(_wellsTable,&WellPiezoTable::rowMade,
           this,&WellsTab::addDataTable,Qt::UniqueConnection);
   connect(_wellsTable,&WellPiezoTable::rowRemoved,
           this,&WellsTab::removeWell,Qt::UniqueConnection);
   connect(_wellsTable,&QTableWidget::cellActivated,
           this,&WellsTab::showCorrectDataTable,Qt::UniqueConnection);
   connect(_data,&Data::wellImported,
           this,&WellsTab::prepareToLoadData,Qt::UniqueConnection);
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::constructWellsTable() END\n";
#endif
}

void WellsTab::loadModel()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::loadModel() START\n";
#endif
   const int noOfOldWells = _dataTables.size();
   for (int i=noOfOldWells-1;i==0;i--) delete _dataTables[i];
   _dataTables.resize(0);
   delete _wellsTable;
   //_tableSize->resetAll();
   const int noOfWells = _data->noOfWells();
   constructWellsTable(true);
   for (int i=0;i<noOfWells;i++)
   {
      _auxTable.resize(0);
      _auxValidTable.resize(0);
      _data->constructAuxWellDataTable(_auxTable,_auxValidTable,i);
      const int rows = _auxTable.size();
      addDataTable(i,true,rows);
   }
   _wellsTable->setCurrentCell(noOfWells,0);
   emit loadInteractionTab(0);
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::loadModel() END\n";
#endif
}

void WellsTab::prepareToExportWell()
{
   const int wellID = _wellsTable->currentRow();
   if (wellID != _wellsTable->rowCount()-1) _qWells->mainMenu()->exportWell(wellID);
}

void WellsTab::prepareToLoadData(const int wellID, const int noOfDataRows)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::preparingToLoadData() START | wellID = "<<wellID<<" noOfDataRows = "<<noOfDataRows<<"\n";
#endif
   _wellsTable->makeNewRow(wellID,true,true); // row,doInsertRow,loadRow
   _auxTable.resize(0);
   _auxValidTable.resize(0);
   _data->constructAuxWellDataTable(_auxTable,_auxValidTable,wellID);
   addDataTable(wellID,true,noOfDataRows);
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::preparingToLoadData() END\n";
#endif
}

void WellsTab::addDataTable(const int wellNo, const bool modelExists, const int rows)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::addDataTable START | wellNo = "<<wellNo<<" | modelExists = "<<modelExists<<" | rows = "<<rows<<" <= represents rows with data if : "<<modelExists<<"\n";
#endif
   WellsDataTable * newDataTable;
   if (modelExists) 
   {
      newDataTable = new WellsDataTable(this,_data,_tableSize,wellNo,rows,modelExists);
      newDataTable->loadTable(_auxTable,_auxValidTable);
   }
   else newDataTable = new WellsDataTable(this,_data,_tableSize,wellNo);
   _dataTablesLayout->addWidget(newDataTable,Qt::AlignTop);
   _dataTables.push_back(newDataTable);
   newDataTable->hide();
   _wellsDataLabel->hide();
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::addDataTable END | noOfDataTables = "<<_dataTables.size()<<"\n";
#endif 
}

void WellsTab::removeWell(const int tableNo)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::removeWell START | removing wellID = "<<tableNo<<" from Data and deleting its DataTable\n";
#endif
   _data->removeWell(tableNo);
   _dataTables.erase(_dataTables.begin() + tableNo);
   if (_parameterTab->boundaryWidget()->boundary() != nullptr)
      _parameterTab->boundaryWidget()->boundary()->removeRefWellItem(tableNo+1);
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::removeWell END\n";
#endif
}

void WellsTab::showCorrectDataTable(const int wellNo, const int)
{
   const int noOfWells = _wellsTable->rowCount()-1;
// #ifdef QDEBUG_ALL
//    qDebug()<<"WellsTab::showCorrectDataTable() START | noOfWells = "<<noOfWells<<"\n";
// #endif
   if (_dataTables.size() == 0) return;
   for (int n=0;n<noOfWells;n++)
   {
      if (n == wellNo) 
      {
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTab::showCorrectDataTable() | show well with ID = "<<wellNo<<"\n";
#endif
         _dataTables[n]->show();
         const QString wellsName = static_cast<LineEdit*>(_wellsTable->cellWidget(n,0))->text();
         const QString labelText = QString("ENTER DATA FOR WELL ").append(wellsName);
         _wellsDataLabel->setText(labelText);
         _wellsDataLabel->show();
      }
      else 
      {
         _dataTables[n]->hide();
      }
   }
   if (wellNo>=noOfWells) _wellsDataLabel->hide();
// #ifdef QDEBUG_ALL
//    qDebug()<<"WellsTab::showCorrectDataTable() END\n";
// #endif
}

void WellsTab::setMaxRowsShown(const int)
{
   _wellsTable->adjustTableSize();
   const int noOfWells = _dataTables.size();
   for (int i=0;i<noOfWells;i++) _dataTables[i]->adjustTableSize();
}

// change font color for A & B in data table when A,B sliders are moved
void WellsTab::setBigAModified(const int wellNo)
{
   _dataTables[wellNo]->setBigAModified();
}

void WellsTab::setBigBModified(const int wellNo)
{
   _dataTables[wellNo]->setBigBModified();
}

// set A & B in data table at row=0 (called from Data when no A,B is set)
void WellsTab::setInitialBigA(const int wellNo)
{
   _dataTables[wellNo]->setInitialBigA();
}

void WellsTab::setInitialBigB(const int wellNo)
{
   _dataTables[wellNo]->setInitialBigB();
}
