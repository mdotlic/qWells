#ifndef STREAMDATATAB_H
#define STREAMDATATAB_H

#include <QFrame>

class Data;
class QListWidget;
class QListWidgetItem;
class QLabel;
class QSpinBox;
class QPushButton;
class QLineEdit;
class VisualizationTab;
class StreamDataTab : public QFrame
{
   Q_OBJECT
   public:
      StreamDataTab(Data *, VisualizationTab *);
      void refresh();
      QPushButton * showButton(){return _streamShowButton;}
      QListWidget * list() {return _list;}
   public slots:
      void checkAll();
      void setTime(double);
   private slots:
      void checkAllInt(const int);
      void checkAllString(const QString);
      void setTimeValue (const int value);
      void streamShowResult();
      void listClick(QListWidgetItem*);
      void setTimeToolTip(const size_t);
   private:
      Data * _data;
      QListWidget * _list;
      QLabel * _time;
      QSpinBox * _spinBox;
      QSpinBox * _nspinBox;
      QPushButton * _streamShowButton;
      QLineEdit * _porosity;
      QLineEdit * _thickness;
      QLineEdit * _timeLength;
      QLineEdit * _timeSample;
      VisualizationTab * _visualizationTab;
      std::vector<bool> _checked;
};
#endif
