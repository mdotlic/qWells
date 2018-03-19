#ifndef WELLSDATATABLE_H
#define WELLSDATATABLE_H

#include "widgets/tables/wellPiezoDataTable.h"
class WellsTab;
class Data;

class WellsDataTable : public WellPiezoDataTable
{
   Q_OBJECT
public:
   WellsDataTable(WellsTab*,Data*,TableSize*,const int,const int rows=1,const bool withData=false);
   
   int checkAllData(int);
   void submitCell(const int);
   void submitTime(const int,const double);
   void deleteRowOfData(const double);
   void deleteCell(const int,const int);
   void moveRow(const int,const int);
   void verifyEntryAndMoveFocus(const int, const int, const QString);
   
   void setBigAModified();
   void setBigBModified();
   void setInitialBigA();
   void setInitialBigB();
   
public slots:
   void getDataFromModel(const int,const int);
   void setTimeToolTip(const size_t);
   void pasteFromClipboard();
   
private:
   WellsTab * _wellsTab;
   Data * _data;
   int _wellNo;
   int _moveDirection;
   bool _moveThisRow = false;
   bool _bigAModified = false;
   bool _bigBModified = false;
   bool _rowHasBeenDeleted = false;
   bool _nanFlagUp = false;
};
#endif
