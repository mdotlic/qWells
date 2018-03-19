#include "wellPiezoDataTable.h"

#include "lineEdit.h"
#include "helperFunctions.h"
#include "parameterTab/tableSize.h"

#include <QHeaderView>
#include <QLineEdit>
#include <QString>
#include <QKeyEvent>
#include <vector>
#include <QApplication>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

WellPiezoDataTable::WellPiezoDataTable(const int rows, const int cols, TableSize * tableSize) : QTableWidget(rows,cols), _tableSize(tableSize)
{
   _dat.resize(cols);
   for (int j=0;j<cols;j++) _dat[j]=dMax;
   setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   
   connect(verticalHeader(),SIGNAL(sectionClicked(int)), // TODO delete stuff
           this,SLOT(selectThisRow(int)));
   connect(this,&WellPiezoDataTable::cellActivated,
           this,&WellPiezoDataTable::unselectRow);
   
   const QColor c = palette().highlight().color();
//    const QColor c = palette().color(QPalette::Active,QPalette::AlternateBase);
   _colorString = "background-color:rgb(" + QString::number(c.red()) + "," + QString::number(c.green()) + "," + QString::number(c.blue()) + ");border:0px;";
}

// doInsertRow - for initial construction of the table only; insert at the end of the table only (basically adding rows at the end)
void WellPiezoDataTable::makeNewRow(const int row, const bool doInsertRow)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::makeNewRow() START | adding new row = "<<row<<"\n";
   #endif
   
//    qDebug()<<"makeNewRow | entryIsValid.size() = "<<_entryIsValid.size()<<"\n";
//    for (size_t i=0;i<_entryIsValid.size();i++)
//    {
//       qDebug()<<"row = "<<i<<" | t = "<<_entryIsValid[i][0]<<" | Q = "<<_entryIsValid[i][1]<<" | Sm = "<<_entryIsValid[i][2]<<" | A = "<<_entryIsValid[i][3]<<" | B = "<<_entryIsValid[i][4]<<"\n";
//    }
//    qDebug()<<"\nmakeNewRow | time.size() = "<<_time.size()<<"\n";
//    for (size_t i=0;i<_time.size();i++) qDebug()<<"row = "<<i<<" | time = "<<_time[i]<<"\n";
   
   std::vector<bool> valid(columnCount(),false);
//    _entryIsValid.push_back(valid);
   _entryIsValid.insert(_entryIsValid.begin()+row,valid);
   
   if (doInsertRow) 
   {
      if ((row==(int)_time.size()+1)||((int)_time.size()==0)) _time.push_back(_dat[0]);
      else _time.insert(_time.begin()+row,_dat[0]);
      insertRow(row);
   }
   for (int j=0;j<columnCount();j++)
   {
      horizontalHeader()->resizeSection(j,80);
      LineEdit * newItem = new LineEdit(row,j);
      setCellWidget(row,j,newItem);
      connect(newItem,&LineEdit::returnPressed,
              this,&WellPiezoDataTable::returnPressHandle);
      connect(newItem,&LineEdit::getData,
              this,&WellPiezoDataTable::getDataFromModel);
      connect(newItem,&LineEdit::inFocus,
              this,&WellPiezoDataTable::setFocusToCell);
      connect(this,&WellPiezoDataTable::rowRemoved,
              newItem,&LineEdit::reduceRowByOne);
      connect(newItem,&LineEdit::pasteData,
              this,&WellPiezoDataTable::pasteFromClipboard);
      _dat[j]=dMax;
   }
   adjustTableSize();
   //    setCurrentCell(row,0);
   
//    qDebug()<<"makeNewRow | entryIsValid.size() = "<<_entryIsValid.size()<<"\n";
//    for (size_t i=0;i<_entryIsValid.size();i++)
//    {
//       qDebug()<<"row = "<<i<<" | t = "<<_entryIsValid[i][0]<<" | Q = "<<_entryIsValid[i][1]<<" | Sm = "<<_entryIsValid[i][2]<<" | A = "<<_entryIsValid[i][3]<<" | B = "<<_entryIsValid[i][4]<<"\n";
//    }
//    qDebug()<<"\nmakeNewRow |       time.size() = "<<_time.size()<<"\n";
//    for (size_t i=0;i<_time.size();i++) qDebug()<<"i = "<<i<<" | time = "<<_time[i]<<"\n";
   
#ifdef QDEBUG_ALL
//    qDebug()<<"WellPiezoDataTable::makeNewRow() END\n";
   qDebug()<<"WellPiezoDataTable::makeNewRow() END\n";
#endif
}

void WellPiezoDataTable::setFocusToCell(int i, int j)
{
   loadDatData(i);
   setCurrentCell(i,j); //6.9.2016 Boris added line 
   emit QTableWidget::cellActivated(i,j); // FIXME need this signal!!
//    if (i != currentRow()) loadDatData(i);
//    setCurrentCell(i,j);
}

void WellPiezoDataTable::loadDatData(const int row)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::loadDatData() | row = "<<row<<" of total "<<rowCount()<<" rows (0-"<<rowCount()-1<<")\n";
#endif
//    if (row == rowCount()) return;
   
   bool ok;
   for (int i=0;i<columnCount();i++) 
   {
      _dat[i] = dMax;
      ok = true;
      if (!cell(row,i)->text().isEmpty())
      {
         ok = conversion::convertQStringToDouble(cell(row,i)->text(),_dat[i]);
         if (!ok) 
         { 
#ifdef QDEBUG_ALL
            qDebug()<<"WellPiezoDataTable::loadDatData() | ERROR : conversion at cell ("<<row<<","<<i<<")\n"; 
#endif
            cell(row,i)->setStyleSheet("color:rgb(204,0,0);");
         }
//          else cell(row,i)->setStyleSheet("color:black;");
      }
      else ok = false;
#ifdef QDEBUG_ALL
      qDebug()<<"WellPiezoDataTable::loadDatData() | _dat["<<i<<"] = "<<_dat[i]<<" (data good = "<<ok<<")\n";
#endif
   }
}

void WellPiezoDataTable::returnPressHandle()
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::returnPressHandle() START\n";
#endif
   LineEdit * cell = static_cast<LineEdit*>(sender());
   const int col = cell->getColumn();
   horizontalHeaderItem(col)->setSelected(true);
   verifyEntryAndMoveFocus(cell->getRow(),col,cell->text());
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::returnPressHandle() END\n";
#endif
}

void WellPiezoDataTable::setMaxHeight(const int maxHeight)
{
   setMaximumHeight(maxHeight);
}

void WellPiezoDataTable::adjustTableSize()
{
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
   setFixedWidth(w + 4);
// #ifdef QDEBUG_ALL
//    qDebug()<<"WellPiezoDataTable::adjustTableSize() | table size (width, height) = ("<<w+2<<","<<_tableSize->getTableHeight(noOfRows)<<")\n";
// #endif
}

LineEdit * WellPiezoDataTable::cell(const int row, const int col)
{
   return static_cast<LineEdit*>(cellWidget(row,col));
}

void WellPiezoDataTable::loadTable(std::vector<std::vector<double>> & table,
                               std::vector<std::vector<bool>> & validTable)
{
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::loadTable() START | table.size() = "<<table.size()<<" | validTable.size() = "<<validTable.size()<<"\n";
#endif
   const int rows = table.size();
   const int cols = columnCount();
   _dat.clear();
   _entryIsValid.clear();
   _dat.resize(cols);
   _entryIsValid.resize(0);
   _time.resize(0);
   std::vector<bool> valid(cols,false);
   for (int i=0;i<rows;i++)
   { 
      if (table[i][0] != dMax) 
      {
         cell(i,0)->enterData(QString::number(table[i][0]));
         cell(i,0)->setStyleSheet("border:0px;");
         valid[0] = validTable[i][0];
      }
      for (int j=1;j<cols;j++)
      {
         if (table[i][j] != dMax) cell(i,j)->enterData(QString::number(table[i][j]));
         valid[j] = validTable[i][j];
         cell(i,j)->setStyleSheet("border:0px;");
      }
      _time.push_back(table[i][0]);
      _entryIsValid.push_back(valid);
   }
   for (int j=0;j<cols;j++) 
   {
      _dat[j]=dMax;
      valid[j] = false;
   }
   _entryIsValid.push_back(valid);
   
   
//    qDebug()<<"loadTable | entryIsValid.size() = "<<_entryIsValid.size()<<"\n";
//    for (size_t i=0;i<_entryIsValid.size();i++)
//    {
//       qDebug()<<"row = "<<i<<" | t = "<<_entryIsValid[i][0]<<" | Q = "<<_entryIsValid[i][1]<<" | Sm = "<<_entryIsValid[i][2]<<" | A = "<<_entryIsValid[i][3]<<" | B = "<<_entryIsValid[i][4]<<"\n";
//    }
//    qDebug()<<"\nloadTable | time.size() = "<<_time.size()<<"\n";
//    for (size_t i=0;i<_time.size();i++) qDebug()<<"i = "<<i<<" | time = "<<_time[i]<<"\n";
#ifdef QDEBUG_ALL
   qDebug()<<"WellsDataTable::loadTable() END\n";
#endif
}

void WellPiezoDataTable::unselectRow()
{
   toggleRowBg(false);
   _selectedRow = -1;
   clearSelection();
}

void WellPiezoDataTable::selectThisRow(const int row)
{
   setFocusToCell(row,0); // activate DataTable
   unselectRow(); // clear previous selection
   _selectedRow = row;
   toggleRowBg(true);
   //selectRow(row);
}

void WellPiezoDataTable::removeThisRow(const int row)
{ 
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::removeThisRow END | row = "<<row<<"\n";
#endif
   if ((row != _selectedRow) || (row == rowCount()-1)) return;
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::removeThisRow | About to delete row = "<<row<<"\n";
#endif
   unselectRow();
//    if (row < rowCount()-1) setCurrentCell(row+1,0);
//    else return;
   for (int j=0;j<columnCount();j++) delete cell(row,j);
   removeRow(row);
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::removeThisRow |  row = "<<row<<" removed from the table, removing from Data and rest of application now\n";
#endif
   emit rowRemoved(row);
   auto it = _entryIsValid.begin() + row;
   _entryIsValid.erase(it);
   deleteRowOfData(_time[row]);
   _time.erase(_time.begin()+row);
   adjustTableSize();
   //setCurrentCell(row,0);
   _selectedRow = -1;  
#ifdef QDEBUG_ALL
   qDebug()<<"WellPiezoDataTable::removeThisRow END | row = "<<row<<" removed, new rowCount() = "<<rowCount()<<", currentRow() = "<<currentRow()<<"\n";
#endif
}

void WellPiezoDataTable::keyPressEvent(QKeyEvent * event)
{
   if (event->key() == Qt::Key_Delete) 
   {
      qDebug()<<"WellPiezoDataTable::keyPressEvent | DELETE PRESSED | _selectedRow = "<<_selectedRow<<"\n";
      if (_selectedRow >= 0) removeThisRow(_selectedRow);
      return; // removed all other functionalities of Delete
   }
   QTableWidget::keyPressEvent(event);
}

void WellPiezoDataTable::toggleRowBg(const bool selected)
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
