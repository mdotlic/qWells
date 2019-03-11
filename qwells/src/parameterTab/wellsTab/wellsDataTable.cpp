#include "wellsDataTable.h"
#include "wellsTab.h"
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
WellsDataTable::WellsDataTable(WellsTab * wellsTab, Data * data, TableSize * tableSize, const int wellNo, const int rows, const bool withData) : WellPiezoDataTable(rows,5,tableSize), _wellsTab(wellsTab), _data(data), _wellNo(wellNo)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable START | wellID = "<<wellNo<<" | rows = "<<rows<<"\n";
#endif
   setHorizontalHeaderLabels(QString("Time;Q;Sm;A;B").split(";"));

   QTableWidgetItem * discharge=horizontalHeaderItem(1);
   discharge->setToolTip("Well discharge rate [l/s]");
   QTableWidgetItem * measuredDrawdown=horizontalHeaderItem(2);
   measuredDrawdown->setToolTip("Measured drawdown [m]");
   QTableWidgetItem * linLoss=horizontalHeaderItem(3);
   linLoss->setToolTip("Linear loss [s/m"+QString(QChar(0x00B2))+"]");
   QTableWidgetItem * quadLoss=horizontalHeaderItem(4);
   quadLoss->setToolTip("Quadratic loss [s"+QString(QChar(0x00B2))+"/m"
         +QString(QChar(0x2075))+"]");

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
   qDebug()<<"WellsDataTable END | data table for the wellID = "<<wellNo<<" constructed\n";
#endif
}

int WellsDataTable::checkAllData(const int row)
{
   if (!entryIsValid(row,0)) return 0;
   bool ok = false;
   for (int i=1;i<5;i++) ok = ok || entryIsValid(row,i);
   if (ok) return -1;
   else return 1;
}

void WellsDataTable::submitCell(const int col)
{
   const int row = currentRow();
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::submitCell() START | Submitting cell["<<row<<","<<col<<"] of wellNo = "<<_wellNo<<"\n";
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
      if (_dat[1] != dMax) _data->changeVectorData(_data->wells()[_wellNo].q(), Point2(_dat[0],_dat[1]));
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::submitCell() | (time,Q) = ("<<_dat[0]<<","<<_dat[1]<<") submitted\n";
#endif
   }
   
   else if (col == 2) 
   {
      if (_dat[2] != dMax) _data->changeVectorData(_data->wells()[_wellNo].Sm(), Point2(_dat[0],_dat[2]));
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::submitCell() | (time,Sm) = ("<<_dat[0]<<","<<_dat[2]<<") submitted\n";
#endif
   }
   
   else if (col == 3) 
   {
      if (_dat[3] != dMax) _data->changeABVectorData(_data->wells()[_wellNo].bigA(), Point2(_dat[0],_dat[3]),_wellNo,0);
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::submitCell() | (time,A) = ("<<_dat[0]<<","<<_dat[3]<<") submitted\n";
#endif
   }
   
   else if (col == 4) 
   {
      if (_dat[4] != dMax) _data->changeABVectorData(_data->wells()[_wellNo].bigB(), Point2(_dat[0],_dat[4]),_wellNo,1);
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::submitCell() | (time,B) = ("<<_dat[0]<<","<<_dat[4]<<") submitted\n";
#endif
   }
                
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::submitCell() END\n";
#endif
}

void WellsDataTable::submitTime(const int col, const double tVal)
{                      
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::submitTime START | column = "<<col<<" newTime = "<<tVal<<" oldTime = "<<_dat[0]<<"\n";
#endif
   if (_dat[0] == tVal) 
   {    
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::submitTime START | oldTime = newTime = "<<tVal<<"\n";
#endif
      return;                 
   }
   
   if (col == 1) 
   {
      if (_dat[1] != dMax) 
      {
         _data->changeTimeData(_data->wells()[_wellNo].q(), _dat[0], tVal);                   
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::submitTime | (time,Q) = ("<<tVal<<","<<_dat[1]<<") submitted\n";
#endif
      }
   }
   
   else if (col == 2) 
   {
      if (_dat[2] != dMax) 
      {
         _data->changeTimeData(_data->wells()[_wellNo].Sm(),  _dat[0], tVal);                   
#ifdef QDEBUG_ALL
         qDebug()<<"WellsDataTable::submitTime | (time,Sm) = ("<<tVal<<","<<_dat[2]<<") submitted\n";
#endif
      }
   }
   
   else if (col == 3) 
   {
      if (_dat[3] != dMax) 
      {
         _data->changeTimeData(_data->wells()[_wellNo].bigA(),  _dat[0], tVal);                   
#ifdef QDEBUG_ALL
         qDebug()<<"WellsDataTable::submitTime | (time,A) = ("<<tVal<<","<<_dat[3]<<") submitted\n";
#endif
      }
   }
   
   else if (col == 4) 
   {
      if (_dat[4] != dMax) 
      {
         _data->changeTimeData(_data->wells()[_wellNo].bigB(),  _dat[0], tVal);                   
#ifdef QDEBUG_ALL
         qDebug()<<"WellsDataTable::submitTime | (time,B) = ("<<tVal<<","<<_dat[4]<<") submitted\n";
#endif
      }
   }
                                  
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::submitTime() END\n";
#endif
}

void WellsDataTable::deleteRowOfData(const double time)
{
   _data->deleteRowOfWellData(_wellNo,time);
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::deleteRowOfData | for wellID = "<<_wellNo<<", deleted row with time = "<<time<<"\n";
#endif
}
 
void WellsDataTable::deleteCell(const int row, const int col)
{                             
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::deleteCell() START | cell("<<row<<","<<col<<")\n";
#endif
   bool anotherDataInRow = false;
   for (int j=1;j<columnCount();j++)
   {
      if (j == col) continue;
      else anotherDataInRow = (anotherDataInRow || _entryIsValid[row][j]);
   }
   if (anotherDataInRow) 
   {
      switch (col)
      {
         case 1 :
         {
            std::vector<Point2> & vector = _data->wells()[_wellNo].q();
            _data->deleteSingleTableData(vector,_time[row]);
            break;
         }
         case 2 :
         {
            std::vector<Point2> & vector = _data->wells()[_wellNo].Sm();
            _data->deleteSingleTableData(vector,_time[row]);
            break;
         }
         case 3 :
         {
            std::vector<Point2> & vector = _data->wells()[_wellNo].bigA();
            _data->deleteSingleTableData(vector,_time[row]);
            break;
         }
         case 4 :
         {
            std::vector<Point2> & vector = _data->wells()[_wellNo].bigB();
            _data->deleteSingleTableData(vector,_time[row]);
            break;
         }
         default:
         {
            qDebug()<<"WellsDataTable::deleteCell | ERROR - bad cell ("<<row<<","<<col<<"), column "<<col<<" undefined!\n";
            assert(false);
         }
      }           
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::deleteCell | deleted cell ("<<row<<","<<col<<")\n";
#endif
   }
   else 
   {
      bool doDeleteRow = (bool)_data->qWells()->modalWarning(static_cast<QWidget*>(this),QString("This will delete the whole row. Continue?"));                 
#ifdef QDEBUG_ALL
      qDebug()<<"WellsDataTable::deleteCell() | Deleting whole row = "<<row<<"? Answer = "<<doDeleteRow<<"\n";
#endif
      if (doDeleteRow) 
      {
         deleteRowOfData(_time[row]);
         removeRow(row);
         _entryIsValid.erase(_entryIsValid.begin()+row);
         _time.erase(_time.begin()+row);
         _rowHasBeenDeleted = true;
         for (int i=row;i<rowCount();i++)
         {
            for (int j=0;j<columnCount();j++)
               static_cast<LineEdit*>(cellWidget(i,j))->setRow(i);
         }
         adjustTableSize();
         setCurrentCell(row,0);
      }
      else getDataFromModel(row,col);
   }
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::deleteCell() END\n";
#endif
}

void WellsDataTable::moveRow(const int row, const int direction)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::moveRow() START | Moving row = "<<row<<" up ("<<bool(direction-1)<<") or down ("<<bool(direction+1)<<") | dat[0] = "<<_dat[0]<<"\n";
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
   else {qDebug()<<"WellsDataTable::moveRow() | ERROR: direction = 0. Exiting.\n";assert(false);}
   
   const int cols = columnCount();
   
   makeNewRow(toRow,true);
   
   QString cellText;
   for (int i=0;i<cols;i++)
   {
      LineEdit * cellFrom = static_cast<LineEdit*>(cellWidget(fromRow,i));
      cellText = cellFrom->text();
      LineEdit * cellTo = static_cast<LineEdit*>(cellWidget(toRow,i));
      cellTo->enterData(cellText);
      cellFrom->clear();
      _entryIsValid[toRow][i] = _entryIsValid[fromRow][i];
      _entryIsValid[fromRow][i] = false;
   }
   
   if (!movingNewEntry) 
   {
      _entryIsValid.erase(_entryIsValid.begin()+fromRow);
      _time.erase(_time.begin()+fromRow);
      for (int i=0;i<cols;i++) removeCellWidget(fromRow,i);
      removeRow(fromRow);
   }
   for (int i=0;i<rowCount();i++)
   {
      for (int j=0;j<cols;j++) static_cast<LineEdit*>(cellWidget(i,j))->setRow(i);
   }
   _moveThisRow = false;
   _wellsTab->rowMoveLabel()->hide();
   adjustTableSize();
   
   setCurrentCell(fromRow,0);
   
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::moveRow END rowCount = "<<rowCount()<<"\n";
#endif
}

void WellsDataTable::verifyEntryAndMoveFocus(const int row, const int col, const QString text)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::verifyEntryAndMoveFocus() START | cellText["<<row<<","<<col<<"] = "<<text<<"\n";
#endif
   
   double tryD;
   if (!text.isEmpty())
   {
      const bool ok = conversion::convertQStringToDouble(text,tryD);
      if (ok)
      {
#ifdef QDEBUG_ALL
         qDebug()<<"WellsDataTable::verifyEntryAndMoveFocus() | QString to double conversion ok, tryD = "<<tryD<<"\n";
#endif
         if (col == 0) 
         {
            for (int i=0;i<rowCount()-1;i++) // check if time is repeated
            {
               if ((tryD == _time[i]) && (i != row))
               {
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::verifyEntryAndMoveFocus() | TIME ERROR! Column = "<<i<<" has the same time ("<<tryD<<")\n";
#endif
                  _entryIsValid[row][0] = false;
                  _wellsTab->warningLabel()->show();
                  cell(row,0)->setStyleSheet("color:red;border:0px;");
                  return;
               }
               else if (tryD == _time[i])
               {
                  setCurrentCell(row,1);
                  _entryIsValid[row][0] = true; // added on Sept.30 2016 (clash?)
                  return;
               }
               _wellsTab->warningLabel()->hide();
            }
            if (_entryIsValid[row][0])
            {
               for (int j=1;j<=4;j++)
               {
                  if (entryIsValid(row,j)) submitTime(j,tryD);
               }
            }
            _entryIsValid[row][0] = true;
            _dat[0] = tryD;
            if (_time.size()>0) _time[row] = tryD;
            if (row >= 0)
            {
               if ((row>0) && (tryD < _time[row-1]))
               {
                  _wellsTab->rowMoveLabel()->show();
                  _moveThisRow = true;
                  _moveDirection = -1;
               }
               else if ((row<rowCount()-2) && (tryD > _time[row+1]))
               {
                  _wellsTab->rowMoveLabel()->show();
                  _moveThisRow = true;
                  _moveDirection = 1;
               }
               else
               {
                  _wellsTab->rowMoveLabel()->hide();
                  _moveThisRow = false;
                  _moveDirection = 0;
               }
            }
//             qDebug()<<"\nVERIFY _entryIsValid.size = "<<_entryIsValid.size()<<"\n";
//             for (size_t i=0;i<_entryIsValid.size();i++)
//             {
//                qDebug()<<"VERIFY  time["<<i<<"] = "<<_entryIsValid[i][0]<<" Q = "<<_entryIsValid[i][1]<<" Sm = "<<_entryIsValid[i][2]<<" A = "<<_entryIsValid[i][3]<<" B = "<<_entryIsValid[i][4]<<"\n";
//             }
//             qDebug()<<"\nVERIFY _time.size = "<<_time.size()<<"\n";
//             for (size_t i=0;i<_time.size();i++)
//             {
//                qDebug()<<"VERIFY  time["<<i<<"] = "<<_time[i]<<"\n";
//             }
//             qDebug()<<"VERIFY setCurrentCell to ("<<row<<","<<1<<")\n";
            setCurrentCell(row,1);
         }
         else
         {
            _entryIsValid[row][col] = true;
            _dat[col] = tryD;
            submitCell(col);
            if (col == columnCount()-1)
            {
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
               if ((row == rowCount()-1) && (!_moveThisRow)) 
               {
                  makeNewRow(row+1,true);
                  setCurrentCell(row+1,0);
                  return;
               }
               else if (_moveThisRow) 
               {
                  moveRow(row,_moveDirection);
                  return;
               }
               else setCurrentCell(row+1,0);
            }
            else setCurrentCell(row,col+1);
         }
         return;
      }
      _entryIsValid[row][col] = false;
      cell(row,col)->setStyleSheet("color:rgb(204,0,0);border:0px;");
      return;
   }
   if (col == 0) return;
   if (_entryIsValid[row][col]) deleteCell(row,col);
   if (_rowHasBeenDeleted) 
   {
      _rowHasBeenDeleted = false;
      return;
   }
   if (col == columnCount()-1)
   {
      const int nextColumn = checkAllData(row);
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::verifyEntryAndMoveFocus() | nextColumn = "<<nextColumn<<"\n";
#endif
      if (nextColumn == -1)
      {
         if ((row == rowCount()-1) && (!_moveThisRow)) 
         {
            makeNewRow(row+1,true);
            //qDebug()<<"VERIFY|afterMakeNewRow setCurrentCell to ("<<row+1<<","<<1<<")\n";
            setCurrentCell(row+1,0);
            return;
         }
         else if (_moveThisRow) 
         {
            moveRow(row,_moveDirection);
            return;
         }
         else setCurrentCell(row+1,0);
      }
      else 
      { 
#ifdef QDEBUG_ALL
         qDebug()<<" focusing cell = ("<<row<<","<<nextColumn<<")\n";
#endif
         setCurrentCell(row,nextColumn);
      }
   }
   else setCurrentCell(row,col+1);
   _entryIsValid[row][col] = false;
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::verifyEntryAndMoveFocus() END\n";
#endif  
}

void WellsDataTable::pasteFromClipboard()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::pasteFromClipboard() START\n";
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
         qDebug()<<"WellsDataTable::pasteFromClipboard() | ERROR - number of columns in row = "<<i<<" does not match the table! (columnCount = "<<columnCount()<<" != "<<noOfCols<<" = noOfCols in clipboard)\n";
#endif
         _wellsTab->qWells()->error("You are trying to paste "+QString::number(noOfCols+1)+" columns instead of "+QString::number(columnCount())+"!");
         return;
      }
      for (int j=0;j<noOfCols;j++)
      {
         dataString = rowString.section(endColMark,j,j).trimmed();
#ifdef QDEBUG_ALL
         qDebug()<<"WellsDataTable::pasteFromClipboard() | i = "<<i<<" j = "<<j<<" | dataString = "<<dataString<<"\n";
#endif
         if (dataString=="") 
         {
            if (j==0) 
            {
#ifdef QDEBUG_ALL
               qDebug()<<"WellsDataTable::pasteFromClipboard() | ERROR - time data missing in row = "<<i<<"!\n";
#endif
               _wellsTab->qWells()->error("Time data in row "+QString::number(i)+" is missing!");
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
               QString errorString = QString("ERROR - conversion QString->double failed at (row,column) = (").append(QString::number(i+1)).append(QString(",")).append(QString::number(j+1)).append(QString(")! ")).append(QString("| Bad entry = ")).append(dataString);
               _wellsTab->qWells()->error(errorString);
#ifdef QDEBUG_ALL
               qDebug()<<errorString;
#endif
               return;
            }
         }
      }
   }
   
   // change Table & Data:
   if (!dataComplete)
   {
      _wellsTab->qWells()->error("Not enough data in one of the rows!");
      return;
   }
   const int rowDiff = rowCount() - 1 - noOfRows; // 1 is for last row
   for (int i=0;i<abs(rowDiff);i++)
   {
      if (rowDiff < 0) 
      {
         const size_t newRow = rowCount();
#ifdef QDEBUG_ALL
         qDebug()<<"WellsDataTable::pasteFromClipboard() | ADDING ROW | row = "<<newRow<<" (add a total of "<<rowDiff<<" rows)\n";
#endif
         insertRow(newRow);
         makeNewRow(newRow,false);
      }
      else 
      {
#ifdef QDEBUG_ALL
         qDebug()<<"WellsDataTable::pasteFromClipboard() | REMOVING ROW "<<rowCount()-1<<" | rowCount = "<<rowCount()<<" (remove a total of "<<rowDiff<<" rows)\n";
#endif
         for (int j=0;j<columnCount();j++) removeCellWidget(rowCount()-1,j);
         removeRow(rowCount()-1);
      }
   }
   for (int i=0;i<_time.size();i++)
      deleteRowOfData(_time[i]);
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
               submitTime(j,_dat[0]);
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
   qDebug()<<"WellsDataTable::pasteFromClipboard() END\n";
#endif
}

void WellsDataTable::setBigAModified()
{
   if (_bigAModified) return;
   if (_entryIsValid[0][3]) cell(0,3)->setModified("");
   _bigAModified = true;
}

void WellsDataTable::setBigBModified()
{   
   if (_bigBModified) return;
   if (_entryIsValid[0][4]) cell(0,4)->setModified("");
   _bigBModified = true;
}

void WellsDataTable::setInitialBigA()
{
   cell(0,3)->enterData(QString::number(0.0));
   _entryIsValid[0][3] = true;
   const int rows = rowCount();
   if (rows == 1)
   {
      if (cell(0,0)->text().isEmpty()) cell(0,0)->enterData(QString::number(0.0));
      _entryIsValid[0][0] = true;
      makeNewRow(1,true);
   }
}

void WellsDataTable::setInitialBigB()
{   
   cell(0,4)->enterData(QString::number(0.0));
   _entryIsValid[0][4] = true;
   const int rows = rowCount();
   if (rows == 1)
   {
      if (cell(0,0)->text().isEmpty()) cell(0,0)->enterData(QString::number(0.0));
      _entryIsValid[0][0] = true;
      makeNewRow(1,true);
   }
}

void WellsDataTable::getDataFromModel(const int row, const int col)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::getDataFromModel() START | cell =  ("<<row<<","<<col<<")\n";
#endif
   if (col == 0) 
   {
      if (_time.size()-1 >= row) cell(row,col)->enterData(QString::number(_time[row]));
      else cell(row,col)->enterData("");
      return;
   }
   int k = 0; // k = position in vector in data
   for (int i=0;i<=row;i++)
   {
      if (_entryIsValid[i][col]) k = k + 1;
   }
   QString str;
   if (k > 0) 
   { 
      switch (col)
      {
         case 1:
         {
            if (_data->wells()[_wellNo].nq() >= row+1)
               str = QString(QString::number(_data->wells()[_wellNo].qi(k-1)._y));
            else 
               str = "";
            break;
         }
         case 2:
         {
            if (int(_data->wells()[_wellNo].nS()) >= row+1)
               str = QString(QString::number(_data->wells()[_wellNo].Smi(k-1)._y));
            else 
               str = "";
            break;
         }
         case 3:
         {
            if (_data->wells()[_wellNo].nA() >= row+1)
               str = QString(QString::number(_data->wells()[_wellNo].bigA(k-1)._y));
            else
               str = "";
            break;
         }
         case 4:
         {
            if (_data->wells()[_wellNo].nB() >= row+1)
               str = QString(QString::number(_data->wells()[_wellNo].bigB(k-1)._y));
            else
               str = "";
            break;
         }
         default:
         {qDebug()<<"WellsDataTable::getDataFromModel | ERROR0 in defining the cell!\n"; assert(false);}
      }
      cell(row,col)->enterData(str);
   }
   else {qDebug()<<"WellsDataTable::getDataFromModel | ERROR1 in defining the cell!\n"; assert(false);}
   if (col == 3) _bigAModified = false;
   else if (col == 4) _bigBModified = false;
}

void WellsDataTable::setTimeToolTip(const size_t timeUnit)
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
