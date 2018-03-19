#ifndef PIEZOCHOOSERFRAME_H
#define PIEZOCHOOSERFRAME_H

#include <QFrame>
#include <QString>
#include <QLabel>
class PlotChooser;
class QHBoxLayout;
class QCheckBox;
class QComboBox;
class Data;

class PiezoChooserFrame : public QFrame
{
   Q_OBJECT
public:
   PiezoChooserFrame(PlotChooser*,const int,const QString&, Data *);
   
   constexpr static int itemFlag = 1;
   
   QComboBox * constructLineStyle();
   QComboBox * constructLineColor();
   void constructSmTickType();
   void setConnections();
   void initialize();
   
   QComboBox * smTickType() { return _smTickType; }
   
   void checkNameLabelWidth(const int);
   void setNameLabelWidth(const int);
   void setName(const QString&);
   void reduceIDbyOne();
   
   QCheckBox * checkBox() { return _checkBox; }
   QCheckBox * smCheckBox() { return _smCheckBox; }

   QComboBox * lineStyle() {return _lineStyle;}
   QComboBox * lineColor() {return _lineColor;}
   
private slots:
   void addRemovePlot(const int);
   void setLineStyle(const int);
   void setLineColor(const int);
   void showSm(const int);
   void setSmTickType(const int);
   
private:
   Data * _data;
   PlotChooser * _chooser;
   int _piezoID;
   QString _name;
   QLabel * _nameLabel;
   int _minWidth = 15;
   int _maxWidth = 100;
   QHBoxLayout * _frameLayout;
   QCheckBox * _checkBox;
   QComboBox * _lineStyle;
   QComboBox * _lineColor;
   QCheckBox * _smCheckBox;
   QComboBox * _smTickType;
};
#endif
