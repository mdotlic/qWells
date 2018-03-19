#ifndef PLOTCHOOSER_H
#define PLOTCHOOSER_H

#include <QFrame>
// #include <QScrollArea>
#include <QPair>
// #include <QString>
#include <QIcon>
#include <QColor>
#include <vector>
class Data;
class QWellsPlot;
class Result;
class WellChooserFrame;
class PiezoChooserFrame;
class QVBoxLayout;

typedef QPair<QString,QPair<QIcon,QColor>> Color;

class PlotChooser : public QFrame
{
   Q_OBJECT
public:
   PlotChooser(Data*,QWellsPlot*,Result*);
   
   std::vector<Color> & colors() { return _colors; }
   QString & colorName(const int i) { return _colors[i].first; }
   void makeColors();
   void fixNameLabelWidths(const int);
   int maxNameWidth() { return _maxNameWidth; }
   
   void calculateWell(const int);
   void clearWell(const int);
   void addPlot(const int,const int,const int,const int,
                const QString&);
   void addNoLossPlot(const int,const int,const int,
                      const QString&);
   void addSmPlot(const int,const int,
                  const int,const int,const QString &);
   void removePlot(const int,const int,
                   const bool isSmPlot=false,
                   const bool isNoLossPlot=false);
   
   void setLineProperties(const int,const int,const int,const int);
   void setNoLossLineProperties(const int,const int,const int);
   void setSmTickType(const int,const int,const int,const int);
   const std::vector<WellChooserFrame*> & wellFrames ()
   {return _wellFrames;}
   const std::vector<PiezoChooserFrame*> & piezFrames ()
   {return _piezoFrames;}
   
signals:
   void wellAdded(int);
   void wellRemoved(int);
   
public slots:
   void addWell(const int);
   void addPiezometer(const int);
   void removeWell(const int);
   void removePiezometer(const int);
   void changeItemName(const int,const int,const QString&);
   
private:
   Data * _data;
   QWellsPlot * _plot;
   Result * _result;
   std::vector<WellChooserFrame*> _wellFrames;
   std::vector<PiezoChooserFrame*> _piezoFrames;
   int _noOfWells;
   int _noOfPiezometers;
   int _maxNameWidth = 0;
   QVBoxLayout * _wellChooserLayout;
   QVBoxLayout * _piezoChooserLayout;
   std::vector<Color> _colors;
};
#endif // PLOTCHOOSER_H
