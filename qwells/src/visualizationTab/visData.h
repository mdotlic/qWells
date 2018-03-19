#ifndef VISDATA_H
#define VISDATA_H

#include <QWidget>

class QToolBar;
class Data;
class QLineEdit;
class VisualizationTab;
class Tabs;
class QPushButton;


class VisData : public QWidget
{
   Q_OBJECT
public:
      VisData(Data *, VisualizationTab *);
      void refresh();
      Tabs * tabs() {return _tabs;}
      QLineEdit * xmin() {return _xmin;}
      QLineEdit * xmax() {return _xmax;}
      QLineEdit * ymin() {return _ymin;}
      QLineEdit * ymax() {return _ymax;}
private slots:
      //void addToList();
      void calculate();
      void enableCalc(const QString);
     /* void showResult();
      void first();
      void previous();
      void play();
      void next();
      void last();
      void setTimeValue(int);
      void automaticIsoValues();
      void listCh();*/
//signals:

private:
      Data * _data;
      QLineEdit * _xdiv;
      QLineEdit * _ydiv;
      QLineEdit * _xmin;
      QLineEdit * _ymin;
      QLineEdit * _xmax;
      QLineEdit * _ymax;
      //QLineEdit * _isoVal;
      //IsoList * _list;
      QPushButton * _calculateButton;

      VisualizationTab * _visualizationTab;
      Tabs * _tabs;

      //QAction * _firstAct;
      //QAction * _prevAct;
      //QAction * _playAct;
      //QAction * _nextAct;
      //QAction * _lastAct;
};

#endif
