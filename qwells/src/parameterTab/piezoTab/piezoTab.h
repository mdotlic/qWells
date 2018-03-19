#ifndef PIEZOTAB_H
#define PIEZOTAB_H

#include <QFrame>
#include <vector>

class QWells;
class Data;
class TableSize;
class PiezoTable;
class PiezoDataTable;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QFrame;

typedef std::vector<std::vector<double>> TableOfDoubles;
typedef std::vector<std::vector<bool>> TableOfBooleans;

class PiezoTab : public QFrame
{
   Q_OBJECT
public:
   PiezoTab(QWells*,TableSize*);
   
   QWells * qWells() { return _qWells; }
   
   void constructPiezoTable(const bool modelExists=false);
   
   PiezoTable * piezoTable() { return _piezoTable; }
   QLabel * warningLabel() { return _timeWarningLabel; }
   QLabel * rowMoveLabel() { return _rowMoveLabel; }
   
   TableOfDoubles auxTable() const { return _auxTable; }
   TableOfBooleans auxValidTable() const { return _auxValidTable; }
   
signals:
   void loadBoundaryWidget();
   void loadInteractionTab(int);
   
public slots:
   void loadModel();
   void prepareToExportPiezometer();
   void prepareToLoadData(const int,const int);
   void addDataTable(const int,const bool modelExists=false,const int rows=1);
   void removePiezometer(const int);
   void showCorrectDataTable(const int,const int);
   void setMaxRowsShown(const int);
   
private:
   QWells * _qWells;
   Data * _data;
   TableSize * _tableSize;
   PiezoTable * _piezoTable;
   std::vector<PiezoDataTable*> _dataTables;
   QGridLayout * _piezoTabLayout;
   QHBoxLayout * _dataTablesLayout;
   QFrame * _dataTablesFrame;
   QLabel * _piezoDataLabel;
   QLabel * _timeWarningLabel;
   QLabel * _rowMoveLabel;
   TableOfDoubles _auxTable;
   TableOfBooleans _auxValidTable;
};
#endif // PIEZOTAB_H