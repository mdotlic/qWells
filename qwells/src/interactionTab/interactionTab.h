#ifndef INTERACTIONTAB_H
#define INTERACTIONTAB_H

#include <QSplitter>

class Data;
class Result;
class TMuSliderFrame;
class TabbedTools;
class QWellsPlot;

class InteractionTab : public QSplitter
{
   Q_OBJECT
public:
   InteractionTab(Data*,Result*);

   QWellsPlot * splot()const {return _splot;}
   TabbedTools * tabbedTools()const {return _tabbedTools;}
   
private:
   Data * _data;
   TMuSliderFrame * _sliderT;
   TMuSliderFrame * _sliderMu; 
   TabbedTools * _tabbedTools;
   QWellsPlot * _splot;
};
#endif
