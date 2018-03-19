#ifndef WELLPIEZOTAB_H
#define WELLPIEZOTAB_H

#include <QTabWidget>
class QWells;
class WellsTab;
class ParameterTab;
class PiezoTab;
class TableSize;

class WellPiezoTab : public QTabWidget
{
public:
   WellPiezoTab(QWells*,ParameterTab*);
   
   WellsTab * wellsTab() { return _wellsTab; }
   PiezoTab * piezoTab() { return _piezoTab; }
   TableSize * tableSize() { return _tableSize; }
   
private:
   WellsTab * _wellsTab;
   PiezoTab * _piezoTab;
   TableSize * _tableSize;
};
#endif // WELLPIEZOTAB_H