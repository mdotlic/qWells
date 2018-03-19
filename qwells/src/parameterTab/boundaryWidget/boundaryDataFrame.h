#ifndef BOUNDARYDATAFRAME_H
#define BOUNDARYDATAFRAME_H

#include <QWidget>
#include <QColor>
class Data;
class BoundaryWidget;
class WellsTable;
class DoubleValue;
class PotentialComboBox;
class QComboBox;
class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QFrame;

class BoundaryDataFrame : public QWidget
{
   Q_OBJECT
public:
   BoundaryDataFrame(BoundaryWidget*,WellsTable*,const int,bool isBoundaryAngles);
   
   void constructReferentWellComboBox();
   void constructLineFrame(const int,const QString&);
   void constructFictiveDistance();
   void removeRefWellItem(const int);
   void setDistanceLabel(const int,const QString&);
   
   void clearLineOuterLayout();
   
   QComboBox * refWell() { return _refWell; }
   DoubleValue * alpha() { return _alpha; }
   DoubleValue * beta() { return _beta; }
   DoubleValue * distance(const int i) { return _distance[i]; }
   PotentialComboBox * potential(const int i) { return _potential[i]; }
   DoubleValue * maxDistance() { return _maxDistance; }
   
public slots:
   void addRefWell(const int,const bool); // 2nd argument dummy because this slot connects to a signal that has two arguments; the second is not used here
   void distanceModified(int);
   void unmodifyDistance(int);
   void maxDistanceModified();
   void unmodifyMaxDistance();
   
protected slots:
   void referentWellChosen(const int);
   void changeWellName(const int,const int,const QString&);
   void potentialChosen(const int,const int);
   void moveFocusToNextItem();
   void repaint();
   
protected:
   Data * _data;
   DoubleValue * _alpha;
   DoubleValue * _beta;
   DoubleValue * _maxDistance = nullptr;
   QComboBox * _refWell = nullptr;
   QHBoxLayout * _outerLayout;
   QVBoxLayout * _lineOuterLayout;
   std::vector<DoubleValue*> _distance;
   std::vector<PotentialComboBox*> _potential;
   std::vector<bool> _distanceModified;
   bool _maxDistanceModified;
      
private:
   BoundaryWidget * _boundaryWidget;
   WellsTable * _wellsTable;
   int _noOfLines;
   bool _isBoundaryAngles;
   std::vector<QFrame*> _lineFrame;
   std::vector<QLabel*> _distanceLabel;
};
#endif // BOUNDARYDATAFRAME_H
