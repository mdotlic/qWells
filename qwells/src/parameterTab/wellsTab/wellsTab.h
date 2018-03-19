#ifndef WELLSTAB_H
#define WELLSTAB_H

#include <QFrame>
#include <vector>

class QWells;
class ParameterTab;
class Data;
class TableSize;
class WellsTable;
class WellsDataTable;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QFrame;

typedef std::vector<std::vector<double>> TableOfDoubles;
typedef std::vector<std::vector<bool>> TableOfBooleans;

class WellsTab : public QFrame
{
   Q_OBJECT
public:
   WellsTab(QWells*,ParameterTab*,TableSize*);
   
   QWells * qWells() { return _qWells; }
   
   void constructWellsTable(const bool modelExists=false);
   
   WellsTable * wellsTable() { return _wellsTable; }
   QLabel * warningLabel() { return _timeWarningLabel; }
   QLabel * rowMoveLabel() { return _rowMoveLabel; }
   
   TableOfDoubles auxTable() const { return _auxTable; }
   TableOfBooleans auxValidTable() const { return _auxValidTable; }
   
signals:
   void loadInteractionTab(int); // id = 0 emitted from wells, id = 1 from piezo

public slots:
   void loadModel();
   void prepareToExportWell();
   void prepareToLoadData(const int,const int);
   void addDataTable(const int,const bool modelExists=false,const int rows=1);
   void removeWell(const int);
   void showCorrectDataTable(const int,const int);
   void setBigAModified(const int);
   void setBigBModified(const int);
   void setInitialBigA(const int);
   void setInitialBigB(const int);
   void setMaxRowsShown(const int);
   
private:
   QWells * _qWells;
   ParameterTab * _parameterTab;
   Data * _data;
   TableSize * _tableSize;
   WellsTable * _wellsTable;
   std::vector<WellsDataTable*> _dataTables;
   QGridLayout * _wellsTabLayout;
   QHBoxLayout * _dataTablesLayout;
   QFrame * _dataTablesFrame;
   QLabel * _wellsDataLabel;
   QLabel * _timeWarningLabel;
   QLabel * _rowMoveLabel;
   TableOfDoubles _auxTable;
   TableOfBooleans _auxValidTable;
   int _maxTableHeight; // HACK
};
#endif // WELLSTAB_H