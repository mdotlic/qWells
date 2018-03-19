#include "wellsTable.h"

#include "widgets/tables/wellPiezoTable.h"
#include "widgets/tables/lineEdit.h"
#include "parameterTab/tableSize.h"
#include "engine/data.h"
#include "engine/position.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QString>
#include <QScrollBar>
#include <vector>

#include <QDebug>
//#ifndef QDEBUG_ALL
//#define QDEBUG_ALL
//#endif

WellsTable::WellsTable(Data * data, TableSize * tableSize, const int rows, const bool modelExists) : WellPiezoTable(rows,4,tableSize), _data(data)
{   
   setHorizontalHeaderLabels(QString("Name;X;Y;r").split(";"));
   QTableWidgetItem * xCoor=horizontalHeaderItem(1);
   xCoor->setToolTip("X coordinate [m]");
   QTableWidgetItem * yCoor=horizontalHeaderItem(2);
   yCoor->setToolTip("Y coordinate [m]");
   QTableWidgetItem * radiusInM=horizontalHeaderItem(3);
   radiusInM->setToolTip("Radius [m]");
   
   int lastRow = 0;
   if (modelExists)
   {
      for (int i=0;i<rows;i++) makeNewRow(i,false,true);
      insertRow(rows);
      lastRow = rows;
   }
   makeNewRow(lastRow,false,false);
   
   if (!_tableSize->allSet(rows))
   {
      const size_t vHeaderWidth = verticalHeader()->size().width();
      _tableSize->setVHeaderWidth(vHeaderWidth,rows);
      const size_t hHeaderHeight = horizontalHeader()->size().height();
      _tableSize->setHHeaderHeight(hHeaderHeight);
      _tableSize->setCellWidth(horizontalHeader()->sectionSize(columnCount()-1));
      _tableSize->setCellHeight(verticalHeader()->sectionSize(rows-1));
      _tableSize->setScrollBarWidth(verticalScrollBar()->sizeHint().width());
   }
   adjustTableSize();
}

void WellsTable::submitData(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::submitData() START | well No = "<<row<<"\n";
#endif
   _data->addWell(row, _wellPiezoName, Point2(_dat[0],_dat[1]),_dat[2]);
   emit repaintPicture();
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::submitData() END\n";
#endif
}

void WellsTable::submitCell(const int row, const int col)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::submitCell() START | cell["<<row<<","<<col<<"]\n";
#endif
   loadDatData(row);
   
   if (col == 0) 
   {
      _data->changeWellName(row,_wellPiezoName);
#ifdef QDEBUG_ALL
      qDebug()<<"WellsTable::submitCell() | submitted well name = "<<_wellPiezoName<<" for wellID = "<<row<<"\n";
#endif
   }
   
   if ((col == 1) || (col == 2)) 
   {
//       qDebug()<<"WellsTable::submitCell() | changing position to ("<<_dat[0]<<","<<_dat[1]<<")\n";
      _data->changeWellPosition(row,Point2(_dat[0],_dat[1]));
#ifdef QDEBUG_ALL
      qDebug()<<"WellsTable::submitCell() | submitted position = ("<<_dat[0]<<","<<_dat[1]<<") for wellID = "<<row<<"\n";
#endif
   }
   
   if (col == 3) 
   {
      _data->changeWellRadius(row,_dat[2]);  
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::submitCell() | submitted radius = "<<_dat[2]<<" for wellID = "<<row<<"\n";
#endif
   }
   
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::submitCell() END\n";
#endif
}

void WellsTable::loadData(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::loadData() START | wellID = "<<row<<"\n";
#endif 

   QString cellText;
   cellText = _data->wells()[row].name();
   cell(row,0)->enterData(cellText);
   cell(row,0)->setStyleSheet("color:black;border:0px;");
   //qDebug()<<"WellsTable::loadData() | name = "<<cellText<<"\n";
   cellText = QString::number(_data->wells()[row].pos()._x, 'f', 2);
   cell(row,1)->enterData(cellText);
   cell(row,1)->setStyleSheet("color:black;border:0px;");
   //qDebug()<<"WellsTable::loadData() | x = "<<cellText<<"\n";
   cellText = QString::number(_data->wells()[row].pos()._y, 'f', 2);
   cell(row,2)->enterData(cellText);
   cell(row,2)->setStyleSheet("color:black;border:0px;");
   //qDebug()<<"WellsTable::loadData() | y = "<<cellText<<"\n";
   cellText = QString::number(_data->wells()[row].radius());
   cell(row,3)->enterData(cellText);
   cell(row,3)->setStyleSheet("color:black;border:0px;");
   //qDebug()<<"WellsTable::loadData() | r = "<<cellText<<"\n";
   
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::loadData() END\n";
#endif
}

void WellsTable::getDataFromModel(const int row, const int col)
{
   if (int(_data->wells().size()) < row+1) 
   {
      cell(row,col)->enterData("");
      return;
   }
      
   QString str;
   switch (col)
   {
      case 0:
         str = _data->wells()[row].name();
         break;
      case 1:
         str = QString::number(_data->wells()[row].pos()._x, 'f', 2);
         break;
      case 2:
         str = QString::number(_data->wells()[row].pos()._y, 'f', 2);
         break;
      case 3:
         str = QString::number(_data->wells()[row].radius());
         break;
      default:
      {qDebug()<<"WellsTable::getDataFromModel | ERROR0 in defining the cell!\n"; assert(false);}
   }
   cell(row,col)->enterData(str);
}

void WellsTable::addFromPicture(const QString & name, const double x, const double y, const double r)
{
   const int row = rowCount() - 1;
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::addFromPicture() START | WellID = "<<row<<" | name = "<<name<<" | position = ("<<x<<","<<y<<") | radius = "<<r<<"\n";
#endif
   
   insertRow(row);
   makeNewRow(row,false,false);
   emit rowMade(row);
   
   _wellPiezoName = name;
   cell(row,0)->enterData(name);
   _dat[0] = x;
   cell(row,1)->enterData(QString::number(x, 'f', 2));
   _dat[1] = y;
   cell(row,2)->enterData(QString::number(y, 'f', 2));
   _dat[2] = r;
   cell(row,3)->enterData(QString::number(r));
   for (int j=0;j<columnCount();j++) _entryIsValid[row][j] = true;
   _data->addWell(row, name, Point2(_dat[0],_dat[1]),_dat[2]);
   emit repaintPicture();
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::addFromPicture() END\n";
#endif
}

bool WellsTable::isInModel(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsTable::isInModel() START | checking if well No = "<<row<<" is in Data or not.\n";
#endif
   if (_data->noOfWells()-1 >= row) 
#ifdef QDEBUG_ALL
   {
      qDebug()<<"WellsTable::isInModel() | IS IN MODEL\n";
#endif
      return true;
#ifdef QDEBUG_ALL
   }
#endif
   else 
#ifdef QDEBUG_ALL
   {
      qDebug()<<"WellsTable::isInModel() | IS NOT IN MODEL\n";
#endif
      return false;
#ifdef QDEBUG_ALL
   }
#endif
}
