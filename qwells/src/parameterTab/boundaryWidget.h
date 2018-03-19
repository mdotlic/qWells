#ifndef BOUNDARYWIDGET_H
#define BOUNDARYWIDGET_H

#include <QFrame>
#include <QSplitter>

class WellsTable;
class PiezoTable;
class BoundaryDataFrame;
class QComboBox;
class QVBoxLayout;
class QWidget;
class PainterWidget;

class BoundaryWidget : public QSplitter
{
   Q_OBJECT
public:
   BoundaryWidget(WellsTable *,PiezoTable*,const bool modelExists=false);
   
   void constructBoundaryComboBox();
   
   BoundaryDataFrame * boundary() { return _boundary; }
   PainterWidget * painterWidget() { return _painterWidget; }
      
public slots:
   void activateCorrectWidget(const int);
   
private:
   WellsTable * _wellsTable;
   const int _noOfBoundaryTypes = 5;
   BoundaryDataFrame * _boundary = nullptr;
   QComboBox * _boundaryTypeBox;
   QVBoxLayout * _boundaryDataLayout;
   int _boundaryActive = -1;
   PainterWidget * _painterWidget;
   bool _modelExists = false;
};
#endif // BOUNDARYWIDGET_H