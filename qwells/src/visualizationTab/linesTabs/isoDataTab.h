#ifndef ISODATATAB_H
#define ISODATATAB_H

#include <QFrame>
#include <QCheckBox>
class Data;
class VisualizationTab;
class QLineEdit;
class IsoList;
class QPushButton;
class QAction;
class QToolBar;
class QLabel;
class QSpinBox;
class IsoDataTab : public QFrame
{
   Q_OBJECT
   public:
      IsoDataTab(Data *, VisualizationTab *);
      QLineEdit * isoVal() {return _isoVal;}
      QPushButton * showButton() {return _showButton;}
      IsoList * list() {return _list;}
      void refresh();
      int timeStep();
      QString isolineVal(const int i);
      void createActions();
      void createToolBars();
      void disableTimeChange();
      void enableTimeChange();
      void enableUseStatic(bool);
      bool useStatic(){return _useStatic->isChecked();}
   private slots:
      void addToList();
      void showResult();
      void first();
      void previous();
      void play();
      void next();
      void last();
      void setTimeValue(int);
      void automaticIsoValues();
      void listCh();
      void staticChanged(int);
   public slots:
      void setTime(double);
      void setTimeToolTip(size_t);
   private:
      QLineEdit * _isoVal;
      IsoList * _list;
      QVector<double> _listNumbers;
      QPushButton * _showButton;
      QCheckBox * _useStatic;

      VisualizationTab * _visualizationTab;

      QAction * _firstAct;
      QAction * _prevAct;
      QAction * _playAct;
      QAction * _nextAct;
      QAction * _lastAct;

      QToolBar * _timeToolBar;
      Data * _data;
      QLabel * _time;
      QSpinBox * _spinBox;


};

#endif
