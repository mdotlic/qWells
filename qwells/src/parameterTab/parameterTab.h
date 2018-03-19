#ifndef PARAMETERTAB_H
#define PARAMETERTAB_H

#include <QSplitter>

class QWells;
class WellPiezoTab;
class BoundaryWidget;

class ParameterTab : public QSplitter
{
   Q_OBJECT
public:
   ParameterTab(QWells *);
   
   WellPiezoTab * wellPiezoTab() { return _wellPiezoTab; }
   BoundaryWidget * boundaryWidget() { return _boundaryWidget; }
   
public slots:
   void loadBoundaryWidget();
   
private:
   WellPiezoTab * _wellPiezoTab;
   BoundaryWidget * _boundaryWidget;
};
#endif