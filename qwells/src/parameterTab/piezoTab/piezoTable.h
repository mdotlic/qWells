#ifndef PIEZOTABLE_H
#define PIEZOTABLE_H

#include "widgets/tables/wellPiezoTable.h"
#include <QString>
class Data;
class TableSize;

class PiezoTable : public WellPiezoTable
{
   Q_OBJECT
public:
   PiezoTable(Data*,TableSize*,const int rows=1,const bool modelExists=false);
   
   void submitData(const int);
   void submitCell(const int,const int);
   void loadData(const int);
   void addFromPicture(const QString&,const double,const double);
   bool isInModel(const int);
   
signals:
   void repaintPicture();
   
public slots:
   void getDataFromModel(const int,const int);
   
private:
   Data * _data;
};
#endif // PIEZOTABLE_H