#ifndef WELLPIEZODATATABLE_H
#define WELLPIEZODATATABLE_H

#include <QTableWidget>
#include <QString>
#include <vector>
class LineEdit;
class TableSize;
class QKeyEvent;

typedef std::vector<std::vector<double>> TableOfDoubles;
typedef std::vector<std::vector<bool>> TableOfBooleans;

class WellPiezoDataTable : public QTableWidget
{
   Q_OBJECT
public:
   WellPiezoDataTable(const int,const int,TableSize*);
   
   void makeNewRow(const int,const bool);
   virtual int checkAllData(){ return -1; }
   void loadDatData(const int);
   virtual void submitCell(const int){}
   virtual void deleteRowOfData(const double){}
   virtual void verifyEntryAndMoveFocus(const int,const int,const QString){}
//    virtual void pasteFromClipboard(){}
   void loadTable(TableOfDoubles&,TableOfBooleans&);
   void adjustTableSize();
   void setMaxHeight(const int);
   LineEdit * cell(const int,const int);
   
   void keyPressEvent(QKeyEvent*); // TODO delete stuff
   void toggleRowBg(const bool); // TODO delete stuff
   
   //bool setEntryIsValid(std::vector<std::vector<bool>> & vec);
   bool entryIsValid(const int row,const int col) {return _entryIsValid[row][col];}
   
signals:
   void rowRemoved(int); // TODO delete stuff
   
public slots:
   void returnPressHandle();
   void setFocusToCell(int,int);
   virtual void getDataFromModel(const int,const int){}
   void removeThisRow(const int); // TODO delete stuff
   void selectThisRow(const int); // TODO delete stuff
   virtual void pasteFromClipboard(){}
   
private slots:
   void unselectRow(); // TODO delete stuff
   
protected:
   QString _wellPiezoName;
   std::vector<double> _dat; // row of data
   std::vector<double> _time; // column of time values
   TableOfBooleans _entryIsValid;
   const double dMax = std::numeric_limits<double>::max(); // cell default value
   TableSize * _tableSize;
private:
   int _selectedRow = -1; // TODO delete stuff
   QString _colorString;
};
#endif // WELLPIEZODATATABLE_H
