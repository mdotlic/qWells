#ifndef WEARSLIDER_H
#define WEARSLIDER_H

#include <QFrame>
class Data;
class QWellsPlot;
class Result;
class MinMaxDoubleValue;
class WearValue;
class QString;
class QLineEdit;
class QSlider;
class QLabel;

class WearSlider : public QFrame
{
   Q_OBJECT
public:
   WearSlider(Data*,QWellsPlot*,Result*,const int);
   
   void setConnections();
   void setInitialization();
   void initialize(const double,const double,const int,const double,
                   const double,const double,const int,const double);
   void setName(const QString&);
   void setAFromData(const int);
   void setBFromData(const int);
   
public slots:
   void fixMin(const int);
   void fixMax(const int);
   
private slots:
   void setValueAMin(const double);
   void setValueAMax(const double);
   void setNoOfStepsA();
   void showRangeWarningLabelA(const bool);
   void showNanWarningLabelA(const bool);
   
   void setValueBMin(const double);
   void setValueBMax(const double);
   void setNoOfStepsB();
   void showRangeWarningLabelB(const bool);
   void showNanWarningLabelB(const bool);
   
private:
   Data * _data;
   int _wellID;
   double _dMinA,_dMaxA,_dMinB,_dMaxB;
   MinMaxDoubleValue * _minValueA, *_minValueB;
   MinMaxDoubleValue * _maxValueA, *_maxValueB;
   QLineEdit * _noOfStepsA, *_noOfStepsB;
   QSlider * _sliderA, *_sliderB;
   WearValue * _valueA, *_valueB;
   QLabel * _nameLabel;
   QLabel * _rangeWarningLabelA, *_rangeWarningLabelB;
   QLabel * _nanWarningLabelA, *_nanWarningLabelB;
};
#endif // WEARSLIDER_H