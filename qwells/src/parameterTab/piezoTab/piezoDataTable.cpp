#include "piezoDataTable.h"
#include "piezoTab.h"
#include "qWells.h"
#include "engine/data.h"
#include "engine/well.h"
#include "widgets/tables/lineEdit.h"
#include "helperFunctions.h"

#include <QApplication>
#include <QClipboard>
#include <QLabel>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif
   
// if withData => rows = no of rows that contain data, otherwise row = total number of rows (i.e. includes the last, empty, row)
PiezoDataTable::PiezoDataTable(PiezoTab * piezoTab, Data * data, TableSize * tableSize, const int piezometerNo, const int rows, const bool withData) : WellPiezoDataTable(rows,2,tableSize), _piezoTab(piezoTab), _data(data), _piezometerNo(piezometerNo)
{
   setHorizontalHeaderLabels(QString("Time;Sm").split(";"));

   QTableWidgetItem * measuredDrawdawn=horizontalHeaderItem(1);
   measuredDrawdawn->setToolTip("Measured drawdown [m]");

   int lastRow = 0;
   if (withData)
   {
      for (int i=0;i<rows;i++) makeNewRow(i,false);
      insertRow(rows);
      lastRow = rows;
   }
   makeNewRow(lastRow,false);
   
   adjustTableSize();

   connect(_data, SIGNAL(timeUnitSet(size_t)), this,
         SLOT(setTimeToolTip(size_t)));
   
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable START | data table for the piezometerID = "<<piezometerNo<<" constructed\n";
#endif
}

int PiezoDataTable::checkAllData(const int row)
{
   if (!entryIsValid(row,0)) return 0;
   if (!entryIsValid(row,1)) return 1;
   else return -1;
}

void PiezoDataTable::submitCell(const int col)
{
   const int row = currentRow();
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::submitCell() START | Submitting cell["<<row<<","<<col<<"] of piezometerNo = "<<_piezometerNo<<"\n";
#endif
   
   const double dMax = std::numeric_limits<double>::max();
   const bool ok = conversion::convertQStringToDouble(cell(row,0)->text(),_dat[0]);
   if (!ok) 
   {
      setCurrentCell(row,0);
      return;
   }
   
   if (col == 1) 
   {
      if (_dat[1] != dMax) 
      {
         _data->changeVectorData(_data->piezometers()[_piezometerNo].Sm(), Point2(_dat[0],_dat[1]));
      }
#ifdef QDEBUG_ALL
      qDebug()<<" **** PiezoDataTable | (time,Sm) = ("<<_dat[0]<<","<<_dat[1]<<") submitted\n";
#endif
   }
   
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::submitCell() END\n";
#endif                                 
}

void PiezoDataTable::submitTime(const double tVal)
{  
   if (_dat[0] == tVal) 
   {    
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::submitTime START | oldTime = newTime = "<<tVal<<"\n";
#endif
      return;                 
   }
   if (_dat[1] != dMax) 
   {
      if(_data->piezometers()[_piezometerNo].Sm().size()==0)
      {
         _data->changeVectorData(_data->piezometers()[_piezometerNo].Sm(), Point2(_dat[0],_dat[1]));
      }
      else
      {
         _data->changeTimeData(_data->piezometers()[_piezometerNo].Sm(), _dat[0],tVal);
      }
#ifdef QDEBUG_ALL
      qDebug()<<"PiezoDataTable::submitTime() | (time,Sm) = ("<<tVal<<","<<_dat[1]<<") submitted\n";
#endif
   }
}

void PiezoDataTable::deleteRowOfData(const double time)
{
   _data->deleteRowOfPiezoData(_piezometerNo,time);
}

void PiezoDataTable::moveRow(const int row, const int direction)
{
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::moveRow() START | Moving row = "<<row<<" up ("<<bool(direction-1)<<") or down ("<<bool(direction+1)<<") | dat[0] = "<<_dat[0]<<"\n";
#endif
   
   bool movingNewEntry;
   if (row == rowCount()-1) movingNewEntry = true;
   else movingNewEntry = false;
   
   int fromRow;
   int toRow = row + direction;
   if (direction == 1) // move row down
   {
      while ((toRow < rowCount()-1) && (_dat[0] > _time[toRow])) toRow = toRow + 1;
      fromRow = row;
   }
   else if (direction == -1) // move row up
   {
      while ((toRow >= 0) && (_dat[0] < _time[toRow])) toRow = toRow - 1;
      toRow = toRow + 1;
      fromRow = row - direction;
   }
   else {qDebug()<<"PiezoDataTable::moveRow() | ERROR: direction = 0. Exiting.\n";assert(false);}
   
   const int cols = columnCount();
   
   makeNewRow(toRow,true);
   
   QString cellText;
//    qDebug()<<"MOVE 1 COPY | fromRow = "<<fromRow<<" | toRow = "<<toRow<<"\n";
//    qDebug()<<"              time.size() = "<<_time.size()<<" | _entryIsValid.size() = "<<_entryIsValid.size()<<"\n";
   for (int i=0;i<cols;i++)
   {
      LineEdit * cellFrom = static_cast<LineEdit*>(cellWidget(fromRow,i));
      cellText = cellFrom->text();
      LineEdit * cellTo = static_cast<LineEdit*>(cellWidget(toRow,i));
      cellTo->enterData(cellText);
      cellFrom->clear();
      _entryIsValid[toRow][i] = _entryIsValid[fromRow][i];
      _entryIsValid[fromRow][i] = false;
      //       _time[toRow] = _time[fromRow];
   }
   
   if (!movingNewEntry) 
   {
//       qDebug()<<"MOVE ERASE | fromRow = "<<fromRow<<" | toRow = "<<toRow<<"\n";
      _entryIsValid.erase(_entryIsValid.begin()+fromRow);
      _time.erase(_time.begin()+fromRow);
      for (int i=0;i<cols;i++) removeCellWidget(fromRow,i);
      removeRow(fromRow);
//       qDebug()<<"MOVE PostERASE| entryIsValid.size() = "<<_entryIsValid.size()<<"\n";
//       for (size_t i=0;i<_entryIsValid.size();i++)
//       {
//          qDebug()<<"row = "<<i<<" | t = "<<_entryIsValid[i][0]<<" | Q = "<<_entryIsValid[i][1]<<" | Sm = "<<_entryIsValid[i][2]<<" | A = "<<_entryIsValid[i][3]<<" | B = "<<_entryIsValid[i][4]<<"\n";
//       }
//       qDebug()<<"\nMOVE PostERASE | time.size() = "<<_time.size()<<"\n";
//       for (size_t i=0;i<_time.size();i++) qDebug()<<"row = "<<i<<" | time = "<<_time[i]<<"\n";
   }
   for (int i=0;i<rowCount();i++)
   {
      for (int j=0;j<cols;j++) static_cast<LineEdit*>(cellWidget(i,j))->setRow(i);
   }
   _moveThisRow = false;
   _piezoTab->rowMoveLabel()->hide();
   adjustTableSize();
   
   setCurrentCell(fromRow,0);
   
   #ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::moveRow END rowCount = "<<rowCount()<<"\n";
   #endif
}

void PiezoDataTable::verifyEntryAndMoveFocus(const int row, const int col, const QString text)
{
//    const int row = currentRow();
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::verifyEntryAndMoveFocus() START | cellText["<<row<<","<<col<<"] = "<<text<<"\n";
#endif
   if (!text.isEmpty())
   {
      double tryD;
      const bool ok = conversion::convertQStringToDouble(text,tryD);
      if (ok)
      {
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::verifyEntryAndMoveFocus() | QString to double conversion ok, tryD = "<<tryD<<"\n";
#endif
         if (col == 0) 
         {
            for (int i=0;i<rowCount()-1;i++)
            {
               if ((tryD == _time[i]) && (i != row))
               {
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::verifyEntryAndMoveFocus() | TIME ERROR! Column = "<<i<<" has the same time ("<<tryD<<")\n";
#endif
                  _entryIsValid[row][0] = false;
                  _piezoTab->warningLabel()->show();
                  cell(row,0)->setStyleSheet("color:red;border:0px;");
                  return;
               }
               else if (tryD == _time[i])
               {
                  setCurrentCell(row,1);
                  _entryIsValid[row][0] = true; // added on Sept.30 2016 (clash?)
                  return;
               }
               _piezoTab->warningLabel()->hide();
            }
            if (_entryIsValid[row][0] && _entryIsValid[row][1])
            {
               submitTime(tryD);
            }
            _entryIsValid[row][0] = true;
            _dat[0] = tryD;
            if (_time.size()>0) _time[row] = tryD;
            if (row >= 0)
            {
               if (row>0 && tryD < _time[row-1])
               {
                  _piezoTab->rowMoveLabel()->show();
                  _moveThisRow = true;
                  _moveDirection = -1;
               }
               else if ((row<rowCount()-2) && (tryD > _time[row+1]))
               {
                  _piezoTab->rowMoveLabel()->show();
                  _moveThisRow = true;
                  _moveDirection = 1;
               }
               else
               {
                  _piezoTab->rowMoveLabel()->hide();
                  _moveThisRow = false;
                  _moveDirection = 0;
               }
            }
            setCurrentCell(row,1);
         }
         else
         {
            _entryIsValid[row][col] = true;
            _dat[col] = tryD;
            submitCell(col);
            if (!_entryIsValid[row][0])
            {
               for (int i=0;i<columnCount();i++) cell(row,i)->setStyleSheet("color:rgb(204,0,0);border:0px;");
               setCurrentCell(row,0);
               return;
            }
            else
            {
               for (int i=0;i<columnCount();i++) cell(row,i)->setStyleSheet("color:black;border:0px;");
            }
            const int nextColumn = checkAllData(row);
            if (nextColumn == -1) 
            {
               if ((row == rowCount()-1) && (!_moveThisRow)) 
               {
                  makeNewRow(row+1,true);
                  setCurrentCell(row+1,0);
                  qDebug()<<" 444 "<<_data->piezometers()[0].Sm().size()<<"\n";
                  return;
               }
               else if (_moveThisRow) 
               {
                  moveRow(row,_moveDirection);
                  return;
               }            
               else setCurrentCell(row+1,0);
            }
            else setCurrentCell(row,nextColumn);
         }
         return;
      }
      else _entryIsValid[row][col] = false;
      cell(row,col)->setStyleSheet("color:rgb(204,0,0);border:0px;");
   }
   else _entryIsValid[row][col] = false;
#ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::returnPressHandle() END\n";
#endif 
   qDebug()<<" na kraju "<<_data->piezometers()[0].Sm().size()<<"\n";
}

void PiezoDataTable::pasteFromClipboard()
{
   #ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::pasteFromClipboard() START\n";
   #endif
   QClipboard * clipboardPtr = QApplication::clipboard();
   const QString clipboardString = clipboardPtr->text();
   
   const QString endRowMark = QString("\n");
   const QString endColMark = QString("\t");
   
   QString rowString, dataString;
   
   TableOfDoubles  auxDataTable;
   TableOfBooleans auxValidTable;
   bool ok;
   bool dataComplete=false;
   double data;
   
   // parse the clipboard string and check contents for errors
   const int noOfRows = clipboardString.count(endRowMark);
   int noOfCols = 0;
   const double dMax = std::numeric_limits<double>::max();
   std::vector<double> dataRow(columnCount(),dMax);
   std::vector<bool> validRow(columnCount(),false);
   for (int i=0;i<noOfRows;i++)
   {
      rowString = clipboardString.section(endRowMark,i,i);
      noOfCols = rowString.count(endColMark)+1;
      auxDataTable.push_back(dataRow);
      auxValidTable.push_back(validRow);
      if (noOfCols != columnCount())
      {
         #ifdef QDEBUG_ALL
         qDebug()<<"PiezoDataTable::pasteFromClipboard() | ERROR - number of columns in row = "<<i<<" does not match the table! (columnCount = "<<columnCount()<<" != "<<noOfCols<<" = noOfCols in clipboard)\n";
         #endif
         _piezoTab->qWells()->error("You are trying to paste "+QString::number(noOfCols+1)+" columns instead of "+QString::number(columnCount())+"!");
         return;
      }
      for (int j=0;j<noOfCols;j++)
      {
         dataString = rowString.section(endColMark,j,j);
         if (dataString=="") 
         {
            if (j==0) 
            {
               #ifdef QDEBUG_ALL
               qDebug()<<"PiezoDataTable::pasteFromClipboard() | ERROR - time data missing in row = "<<i<<"!\n";
               #endif
               _piezoTab->qWells()->error("Time data in row "+QString::number(i)+" is missing!");
               return;
            }
            auxValidTable[i][j] = false;
            continue;
         }
         else
         {
            ok = conversion::convertQStringToDouble(dataString,data);
            if (ok)
            {
               auxDataTable[i][j] = data;
               auxValidTable[i][j] = true;
               if (j!=0) dataComplete = true;
            }
            else
            {
               qDebug()<<"PiezoDataTable::pasteFromClipboard() | ERROR - conversion QString->double failed at (row,column) = ("<<i<<","<<j<<")!\n";
               assert(false);
               return;
            }
         }
      }
   }
   // change Table & Data:
   if (!dataComplete)
   {
      _piezoTab->qWells()->error("Not enough data in one of the rows!");
      return;
   }
   const int rowDiff = rowCount() - 1 - noOfRows; // 1 is for last row
   for (int i=0;i<abs(rowDiff);i++)
   {
      if (rowDiff < 0) 
      {
         const size_t newRow = rowCount();
         #ifdef QDEBUG_ALL
         qDebug()<<"PiezoDataTable::pasteFromClipboard() | ADDING ROW | row = "<<newRow<<" (add a total of "<<rowDiff<<" rows)\n";
         #endif
         insertRow(newRow);
         makeNewRow(newRow,false);
      }
      else 
      {
         #ifdef QDEBUG_ALL
         qDebug()<<"PiezoDataTable::pasteFromClipboard() | REMOVING ROW "<<rowCount()-1<<" | rowCount = "<<rowCount()<<" (remove a total of "<<rowDiff<<" rows)\n";
         #endif
         for (int i=0;i<columnCount();i++) removeCellWidget(rowCount()-1,i);
         removeRow(rowCount()-1);
      }
   }
   _entryIsValid.resize(0);
   _time.resize(0);
   _dat.clear();
   _dat.resize(noOfCols);
   for (int i=0;i<noOfRows;i++)
   {
      for (int j=0;j<noOfCols;j++)
      {
         if (auxDataTable[i][j]!=dMax) static_cast<LineEdit*>(cellWidget(i,j))->enterData(QString::number(auxDataTable[i][j])); // set Table cell
         else static_cast<LineEdit*>(cellWidget(i,j))->clear();
         if (auxValidTable[i][j]) 
         {
            _dat[j] = auxDataTable[i][j];

            if (j==0) 
            {
               submitTime(_dat[0]);
               _time.push_back(_dat[0]);
            }
            else 
            {
               setCurrentCell(i,j);
               submitCell(j);               
            }
         }

         validRow[j] = auxValidTable[i][j];
      }
      _entryIsValid.push_back(validRow);
   }               
   for (int j=0;j<noOfCols;j++)
   {
      LineEdit * tcell = static_cast<LineEdit*>(cellWidget(rowCount()-1,j));
      tcell->clear();
      tcell->setRow(rowCount()-1);
      _entryIsValid.push_back(validRow);
   }
   
   adjustTableSize();
   setCurrentCell(rowCount()-1,0);
   #ifdef QDEBUG_ALL
   qDebug()<<"PiezoDataTable::pasteFromClipboard() END\n";
   #endif
}


void PiezoDataTable::getDataFromModel(const int row, const int col)
{
   if (col == 0) 
   {
      if (_time.size()-1 >= row) cell(row,col)->enterData(QString::number(_time[row]));
      else cell(row,col)->enterData("");
      return;
   }
   int k = 0;
   for (int i=0;i<=row;i++)
   {
      if (_entryIsValid[i][col]) k = k + 1;
   }
   if (k > 0) 
   {
      QString str;
      if (_data->piezometers()[_piezometerNo].nS() >= row+1)
         str = QString(QString::number(_data->piezometers()[_piezometerNo].Smi(k-1)._y));
      else
         str = "";
      cell(row,col)->enterData(str);
   }
   else {qDebug()<<"PiezoDataTable::getDataFromModel | ERROR1 in defining the cell!\n"; assert(false);}
}

void PiezoDataTable::setTimeToolTip(const size_t timeUnit)
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
   QTableWidgetItem * time=horizontalHeaderItem(0);
   time->setToolTip(text);


}
