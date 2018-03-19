#ifndef WELLCHOOSERFRAME_H
#define WELLCHOOSERFRAME_H

#include <QFrame>
#include <QString>
#include <QLabel>
class PlotChooser;
class QGridLayout;
class QCheckBox;
class QComboBox;
class Data;

class WellChooserFrame : public QFrame
{
   Q_OBJECT
public:
   WellChooserFrame(PlotChooser*,const int,const QString&, Data *);
   
   constexpr static int itemFlag = 0;
   
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
   QCheckBox * lossCheckBox() { return _lossCheckBox; }
   QCheckBox * noLossCheckBox() { return _noLossCheckBox; }
   QCheckBox * smCheckBox() { return _smCheckBox; }
   QComboBox * lossLineStyle() {return _lossLineStyle;}
   QComboBox * lossLineColor() {return _lossLineColor;}
   QComboBox * noLossLineStyle() {return _noLossLineStyle;}
   QComboBox * noLossLineColor() {return _noLossLineColor;}

private slots:
   void switchboard(const int);
   void setLossLineStyle(const int);
   void setLossLineColor(const int);
   void setNoLossLineStyle(const int);
   void setNoLossLineColor(const int);
   void setSmTickType(const int);
   
private:
   Data * _data;
   PlotChooser * _chooser;
   int _wellID;
   QString _name;
   QLabel * _nameLabel;
   int _minWidth = 15;
   int _maxWidth = 100;
   QGridLayout * _frameLayout;
   QCheckBox * _checkBox;
   QCheckBox * _lossCheckBox;
   QCheckBox * _noLossCheckBox;
   QComboBox * _lossLineStyle;
   QComboBox * _lossLineColor;
   QComboBox * _noLossLineStyle;
   QComboBox * _noLossLineColor;
   QCheckBox * _smCheckBox;
   QComboBox * _smTickType;
};
#endif
