#include "wellPiezoTable.h"

#include "lineEdit.h"
#include "helperFunctions.h"
#include "parameterTab/tableSize.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QString>
#include <QKeyEvent>
#include <vector>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

WellPiezoTable::WellPiezoTable(const int rows, const int cols, TableSize * tableSize) : QTableWidget(rows,cols), _tableSize(tableSize)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable START | rows = "<<rows<<" cols = "<<cols<<"\n";
#endif
   _dat.resize(cols-1);
   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      
   connect(verticalHeader(),SIGNAL(sectionClicked(int)),
           this,SLOT(selectThisRow(int)));
   connect(this,&WellPiezoTable::cellActivated,
           this,&WellPiezoTable::unselectRow);
   
   const QColor c = palette().highlight().color();
   //    const QColor c = palette().color(QPalette::Active,QPalette::AlternateBase);
   _colorString = "background-color:rgb(" + QString::number(c.red()) + "," + QString::number(c.green()) + "," + QString::number(c.blue()) + ");border:0px;";

#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable END\n";
#endif
}

void WellPiezoTable::makeNewRow(const int row, const bool doInsertRow, const bool loadRow)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::makeNewRow START | row = "<<row<<" | doInsertRow = "<<doInsertRow<<" | loadRow = "<<loadRow<<"\n";
#endif
   std::vector<bool> valid(columnCount(),loadRow);
   _entryIsValid.push_back(valid);
   
   if (doInsertRow) 
   {
      insertRow(row);
      if (loadRow)
      {
         for (int j=0;j<columnCount();j++) 
         {
            cell(row+1,j)->increaseRowByOne(row+1);
            _entryIsValid[row+1][j] = false;
            _entryIsValid[row][j] = true;
         }
      }
      else emit rowMade(row-1); // rowMade(int,false = no loaded model)
   }
   
   for (int j=0;j<columnCount();j++)
   {
      if (j>0) horizontalHeader()->resizeSection(j,80);
      LineEdit * newItem = new LineEdit(row,j);
      setCellWidget(row,j,newItem);
      connect(newItem,&QLineEdit::returnPressed,
              this,&WellPiezoTable::returnPressHandle);
      connect(newItem,&LineEdit::getData,
              this,&WellPiezoTable::getDataFromModel);
      connect(newItem,&LineEdit::inFocus,
              this,&WellPiezoTable::setFocusToCell);
      connect(this,&WellPiezoTable::rowRemoved,
              newItem,&LineEdit::reduceRowByOne);
   }
   if (loadRow) { /*qDebug()<<"WellsPiezoTable::makeNewRow | loadingRow now - JUST DON'T SAY 'FIX ME PLEASE'"<<"\n";*/ loadData(row);}
      
   adjustTableSize();
   if ((loadRow) && (row < rowCount())) return;
   setCurrentCell(row,0);
}

void WellPiezoTable::setFocusToCell(int i, int j)
{
   emit QTableWidget::cellActivated(i,j);
   setCurrentCell(i,j);
   if (i != currentRow()) loadDatData(i);
}

void WellPiezoTable::loadDatData(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::loadDatData() | row = "<<row<<"\n";
#endif
   _wellPiezoName = cell(row,0)->text();
   bool ok;
   for (int i=1;i<columnCount();i++) 
   {
      _dat[i-1] = dMax;
      if (!cell(row,i)->text().isEmpty()) 
      {
         ok = conversion::convertQStringToDouble(cell(row,i)->text(),_dat[i-1]);
         if (!ok)
         { 
#ifdef QDEBUG_ALL
            qDebug()<<"WellPiezoTable::loadDatData() | ERROR : conversion at cell ("<<row<<","<<i<<")\n";
#endif
            cell(row,i)->setStyleSheet("color:rgb(204,0,0);border:0px;");
         }
         else cell(row,i)->setStyleSheet("color:black;border:0px;");
      }
      else ok = false;
#ifdef QDEBUG_ALL
      qDebug()<<"WellPiezoTable::loadDatData() | _dat["<<i-1<<"] = "<<_dat[i-1]<<" (data good = "<<ok<<")\n";
#endif
   }
}

int WellPiezoTable::checkAllData(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::checkAllData() START\n";
#endif
   int i = 0;
   bool ok = true;
   while ((ok) && (i<columnCount()))
   {
      ok = ok && _entryIsValid[row][i];
      i = i + 1;
   }
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::checkAllData() END | data ok = "<<ok<<"\n";
#endif
   if (ok) return -1;
   else return i-1;
}

void WellPiezoTable::verifyEntryAndMoveFocus(const int col, const QString text)
{
   const int row = currentRow();
#ifdef QDEBUG_ALL
   qDebug()<<"WellsPiezoTable::verifyEntryAndMoveFocus() START | cellText["<<row<<","<<col<<"] = "<<text<<"\n";
#endif
   if (text.isEmpty()) 
   {
      _entryIsValid[row][col] = false;
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::verifyEntryAndMoveFocus()| ERROR : cell ("<<row<<","<<col<<") is empty!\n";
#endif
      return;
   }
   if (col == 0) 
   {
      _wellPiezoName = text;
      _entryIsValid[row][0] = true;
      const int invalidCell = checkAllData(row);
      const bool existingData = isInModel(row);
      if ((existingData) && (invalidCell<0)) submitCell(row,col);
      setCurrentCell(row,1);
      return;
   }
   double datTry;
   bool ok = false;
   ok = conversion::convertQStringToDouble(text,datTry);
   if (!ok)
   {
      _entryIsValid[row][col] = false;
      cell(row,col)->setStyleSheet("color:rgb(204,0,0);border:0px;");
#ifdef QDEBUG_ALL
      qDebug()<<"WellPiezoTable::verifyEntryAndMoveFocus() | ERROR : entry in cell ("<<row<<","<<col<<") is not a number! (entry = "<<text<<")\n";
#endif
      return;
   }
   // if correct data is entered in the cell, then:
   cell(row,col)->setStyleSheet("color:black;border:0px;");
   _dat[col-1] = datTry;
   _entryIsValid[row][col] = true;
   const int invalidCell = checkAllData(row);
   const bool existingData = isInModel(row);
   if (existingData)
   {
      if (invalidCell<0) // no cell is invalid, all contain data
      {
         submitCell(row,col);
         if (col < columnCount()-1) setCurrentCell(row,col+1);
         else setCurrentCell(row+1,0);
      }
      else setCurrentCell(row,invalidCell);
   }
   else // not existing data => row = rowCount()-1
   {
      if (invalidCell<0)  // no cell is invalid, all contain data
      {
         makeNewRow(row+1,true);
         submitData(row);
         setCurrentCell(row+1,0);
      }
      else setCurrentCell(row,invalidCell);
   }
   
#ifdef QDEBUG_ALL
   qDebug()<<"WellsPiezoTable::returnPressHandle() END\n";
#endif
}

void WellPiezoTable::adjustTableSize()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::adjustTableSize() START\n";
#endif
   const int noOfRows = rowCount();
   const int noOfCols = columnCount();
   
   const size_t vHeaderWidth = verticalHeader()->size().width();
   _tableSize->setVHeaderWidth(vHeaderWidth,noOfRows);
   
   setFixedHeight(_tableSize->getTableHeight(noOfRows));
   int w = _tableSize->getVHeader(noOfRows) + 
           horizontalHeader()->sectionSize(0) + 
          (noOfCols-1) * _tableSize->getCellWidth();
   if (noOfRows > _tableSize->settings()->getCurrentRowsShown()) 
       w = w + _tableSize->getScrollWidth();
//    setFixedWidth(w + 2);
   setFixedWidth(w + 4);
   
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::adjustTableSize() END | noOfRows = "<<noOfRows<<" | noOfCols = "<<noOfCols<<" | tableSize = "<<size()<<"\n";
#endif
}

LineEdit * WellPiezoTable::cell(const int row, const int col)
{
   return static_cast<LineEdit*>(cellWidget(row,col));
}

void WellPiezoTable::returnPressHandle()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::returnPressHandle() START\n";
#endif
   LineEdit * cell = static_cast<LineEdit*>(sender());
   const int col = cell->getColumn();
   //horizontalHeaderItem(col)->setSelected(true);
   verifyEntryAndMoveFocus(col,cell->text());   
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::returnPressHandle() END\n";
#endif
}

void WellPiezoTable::unselectRow()
{
   toggleRowBg(false);
   _selectedRow = -1;
   clearSelection();
}

void WellPiezoTable::selectThisRow(const int row)
{
   setFocusToCell(row,0); // activate DataTable
   unselectRow(); // clear previous selection
   _selectedRow = row;
   toggleRowBg(true);
   //selectRow(row);
}

void WellPiezoTable::removeThisRow(const int row)
{ 
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::removeThisRow END | row = "<<row<<"\n";
#endif
   if ((row != _selectedRow) || (row == rowCount()-1)) return;
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::removeThisRow | About to delete row = "<<row<<"\n";
#endif
   unselectRow();
//    if (row < rowCount()-1) setCurrentCell(row+1,0);
//    else return;
   for (int j=0;j<columnCount();j++) delete cell(row,j);
   removeRow(row);
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::removeThisRow |  row = "<<row<<" removed from the table, removing from Data and rest of application now\n";
#endif
   emit rowRemoved(row);
   adjustTableSize();
   auto it = _entryIsValid.begin() + row;
   _entryIsValid.erase(it);
   setCurrentCell(row,0);
   _selectedRow = -1;  
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoTable::removeThisRow END | row = "<<row<<" removed, new rowCount() = "<<rowCount()<<", currentRow() = "<<currentRow()<<"\n";
#endif
}

void WellPiezoTable::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Delete) 
   {
      if (_selectedRow >= 0) removeThisRow(_selectedRow);
      else { qDebug()<<" here goes a call to deleteCell()\n"; }
   }
   QTableWidget::keyPressEvent(event); // TODO comment out to remove all other functionalities of Delete
}

void WellPiezoTable::toggleRowBg(const bool selected)
{
   if (_selectedRow < 0) return;
   if (selected)
   {
//       for (int j=0;j<columnCount();j++) cellWidget(_selectedRow,j)->setStyleSheet("background-color:rgb(240,240,255);border:0px;");
      for (int j=0;j<columnCount();j++) cellWidget(_selectedRow,j)->setStyleSheet(_colorString);
   }
   else
   {
      for (int j=0;j<columnCount();j++) cellWidget(_selectedRow,j)->setStyleSheet("background-color:white;border:0px;");
   }
}
