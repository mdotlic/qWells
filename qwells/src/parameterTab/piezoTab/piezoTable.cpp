#include "piezoTable.h"

#include "widgets/tables/wellPiezoTable.h"
#include "widgets/tables/lineEdit.h"
#include "parameterTab/tableSize.h"
#include "engine/data.h"
#include "engine/position.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QString>
#include <vector>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

PiezoTable::PiezoTable(Data * data, TableSize * tableSize, const int rows, const bool modelExists) : WellPiezoTable(rows,3,tableSize), _data(data)
{   
   setHorizontalHeaderLabels(QString("Name;X;Y").split(";"));
   QTableWidgetItem * xCoor=horizontalHeaderItem(1);
   xCoor->setToolTip("X coordinate [m]");
   QTableWidgetItem * yCoor=horizontalHeaderItem(2);
   yCoor->setToolTip("Y coordinate [m]");
   
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
   }
   adjustTableSize();
}

void PiezoTable::submitData(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::submitData() START | piezometer No = "<<row<<"\n";
#endif
   //loadDatData(row);FIXME 20.02.2017 Milan in accordance to wellTable.cpp
   
   _data->addPiezometer(row,_wellPiezoName,Point2(_dat[0],_dat[1]));
   emit repaintPicture();
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::submitData() END\n";
#endif
}

void PiezoTable::submitCell(const int row, const int col)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::submitCell() START | cell["<<row<<","<<col<<"]\n";
#endif
   loadDatData(row);//FIXME 20.02.2017 Milan in accordance to wellTable.cpp
   if (col == 0) 
   {
      _data->changePiezoName(row,_wellPiezoName);
#ifdef QDEBUG_ALL
      qDebug()<<"PiezoTable::submitCell() | submitted piezo name = "<<_wellPiezoName<<" for piezometerID = "<<row<<"\n";
#endif
   }
   
   if ((col == 1) || (col == 2)) 
   {
      _data->changePiezoPosition(row,Point2(_dat[0],_dat[1]));
#ifdef QDEBUG_ALL
      qDebug()<<"PiezoTable::submitCell() | submitted position = ("<<_dat[0]<<","<<_dat[1]<<") for piezometerID = "<<row<<"\n";
#endif
   }
   
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::submitCell() END\n";
#endif
}

void PiezoTable::loadData(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::loadData() START | piezoID = "<<row<<"]\n";
#endif 
   
   QString cellText;
   cellText = _data->piezometers()[row].name();
   cell(row,0)->enterData(cellText);
   cell(row,0)->setStyleSheet("color:black;border:0px;");
   cellText = QString::number(_data->piezometers()[row].pos()._x, 'f', 2);
   cell(row,1)->enterData(cellText);
   cell(row,1)->setStyleSheet("color:black;border:0px;");
   cellText = QString::number(_data->piezometers()[row].pos()._y, 'f', 2);
   cell(row,2)->enterData(cellText);
   cell(row,2)->setStyleSheet("color:black;border:0px;");
   
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::loadData() END\n";
#endif
}

void PiezoTable::getDataFromModel(const int row, const int col)
{
   if (int(_data->piezometers().size()) < row+1) 
   {
      cell(row,col)->enterData("");
      return;
   }
   
   QString str;
   switch (col)
   {
      case 0:
         str = _data->piezometers()[row].name();
         break;
      case 1:
         str = QString::number(_data->piezometers()[row].pos()._x, 'f', 2);
         break;
      case 2:
         str = QString::number(_data->piezometers()[row].pos()._y, 'f', 2);
         break;
      default:
      {qDebug()<<"PiezoTable::getDataFromModel | ERROR0 in defining the cell!\n"; assert(false);}
   }
   cell(row,col)->enterData(str);
}

void PiezoTable::addFromPicture(const QString & name, const double x, const double y)
{
   const int row = rowCount() - 1;
   
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::addFromPicture() START | name = "<<name<<" | position = ("<<x<<","<<y<<")\n";
#endif
   
   insertRow(row);
   makeNewRow(row,false,false);
   emit rowMade(row);
   
   _wellPiezoName = name;
   cell(row,0)->enterData(name);
   _dat[0] = x;
   cell(row,1)->enterData(QString::number(x));
   _dat[1] = y;
   cell(row,2)->enterData(QString::number(y));
   for (int j=0;j<columnCount();j++) _entryIsValid[row][j] = true;
   _data->addPiezometer(row, name, Point2(_dat[0],_dat[1]));
   emit repaintPicture();
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::addFromPicture() END\n";
#endif
}

bool PiezoTable::isInModel(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoTable::isInModel() START | checking if piezometer No = "<<row<<" is in Data or not.\n";
#endif
   if (_data->noOfPiezometers()-1 >= row) return true;
   else return false;
   
   if (_data->noOfPiezometers()-1 >= row) 
#ifdef QDEBUG_ALL
   {
      qDebug()<<"PiezoTable::isInModel() | IS IN MODEL\n";
#endif
      return true;
#ifdef QDEBUG_ALL
   }
#endif
   else 
#ifdef QDEBUG_ALL
   {
      qDebug()<<"PiezoTable::isInModel() | IS NOT IN MODEL\n";
#endif
      return false;
#ifdef QDEBUG_ALL
   }
#endif
}
