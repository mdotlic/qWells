#ifndef TABBEDTOOLS_H
#define TABBEDTOOLS_H

#include <QTabWidget>
class Data;
class QWellsPlot;
class Result;
class PlotChooser;
class Adjuster;

class TabbedTools : public QTabWidget
{
   Q_OBJECT
public:
   TabbedTools(Data*,QWellsPlot*,Result*);
   PlotChooser * chooserTab() {return _chooserTab;}
   
private:
   PlotChooser * _chooserTab;
   Adjuster * _adjusterTab;
};
#endif // TABBEDTOOLS_H
