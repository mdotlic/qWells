#ifndef VISUALIZATIONTAB_H
#define VISUALIZATIONTAB_H

#include <QSplitter>

class VisData;
class VisPicture;
class VisResult;
class Data;

class VisualizationTab : public QSplitter
{
   Q_OBJECT
public:
      VisualizationTab(Data *);
      void refresh();
      VisPicture * visPicture() {return _visPicture;}
      VisResult * visResult() {return _visResult;}
      VisData * visData(){return _visData;}
private:
      VisPicture * _visPicture;
      VisData * _visData;
      VisResult * _visResult;
      Data * _data;
};

#endif
