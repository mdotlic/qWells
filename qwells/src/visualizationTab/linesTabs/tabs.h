#ifndef TABS_H
#define TABS_H

#include <QTabWidget>
class Data;
class VisualizationTab;
class IsoDataTab;
class StreamDataTab;
class Tabs : public QTabWidget
{
   Q_OBJECT
   public:
      Tabs(Data *, VisualizationTab *);
      IsoDataTab * isoDataTab() {return _isoDataTab;}
      StreamDataTab * streamDataTab() {return _streamDataTab;}
   private:
      IsoDataTab * _isoDataTab;
      StreamDataTab * _streamDataTab;
};
#endif
