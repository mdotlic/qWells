#ifndef PIEZODATATABLE_H
#define PIEZODATATABLE_H

#include "widgets/tables/wellPiezoDataTable.h"
class PiezoTab;
class Data;
class TableSize;

class PiezoDataTable : public WellPiezoDataTable
{
   Q_OBJECT
public:
   PiezoDataTable(PiezoTab*,Data*,TableSize*,const int,const int rows=1,const bool withData=false);
   
   int checkAllData(const int);
   void submitCell(const int);
   void submitTime(const double);
   void deleteRowOfData(const double);
   void deleteCell(const int,const int){} // do nothing
   void moveRow(const int,const int);
   void verifyEntryAndMoveFocus(const int, const int, const QString);
   
public slots:
   void getDataFromModel(const int,const int);
   void setTimeToolTip(const size_t);
   void pasteFromClipboard();
   
private:
   PiezoTab * _piezoTab;
   Data * _data;
   int _piezometerNo;
   int _moveDirection;
   bool _moveThisRow = false;
};
#endif // PIEZODATATABLE_H
