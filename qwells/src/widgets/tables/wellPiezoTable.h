#ifndef WELLPIEZOTABLE_H
#define WELLPIEZOTABLE_H

#include <QTableWidget>
#include <QString>
#include <vector>
class QKeyEvent;
class LineEdit;
class TableSize;

#include <QDebug>
class WellPiezoTable : public QTableWidget
{
   Q_OBJECT
public:
   WellPiezoTable(const int,const int,TableSize*);
   
   void makeNewRow(const int,const bool,const bool loadRow=false);
   int checkAllData(const int);
   void loadDatData(const int);
   virtual void submitData(const int){}
   virtual void submitCell(const int, const int){}
   virtual void loadData(const int){}
   virtual bool isInModel(const int){ qDebug()<<"ERROR - this function should not be called!\n"; return false; }
   void verifyEntryAndMoveFocus(const int, const QString);
   void adjustTableSize();
   void setMaxHeight(const int);
   LineEdit * cell(const int,const int);
   
   void keyPressEvent(QKeyEvent*); // TODO delete stuff
   void toggleRowBg(const bool); // TODO delete stuff
   
signals:
   void rowMade(int,bool a=false,int i=1);
   void rowRemoved(int); // TODO delete stuff
   
public slots:
   void returnPressHandle();
   void setFocusToCell(int,int);
   virtual void getDataFromModel(const int,const int){}
   void removeThisRow(const int); // TODO delete stuff
   void selectThisRow(const int); // TODO delete stuff
   
private slots:
   void unselectRow(); // TODO delete stuff
   
protected:
   QString _wellPiezoName;
   std::vector<double> _dat;
   std::vector<std::vector<bool>> _entryIsValid;
   const double dMax = std::numeric_limits<double>::max(); // cell default value
   TableSize * _tableSize;
   
private:
   int _selectedRow = -1; // TODO delete stuff
   QString _colorString;
};
#endif // WELLPIEZOTABLE_H
