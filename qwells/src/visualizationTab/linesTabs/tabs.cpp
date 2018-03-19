#include "tabs.h"
#include "isoDataTab.h"
#include "streamDataTab.h"

Tabs::Tabs(Data * data, VisualizationTab * visualizationTab)
{
   _isoDataTab = new IsoDataTab(data, visualizationTab);
   _isoDataTab->setFocusPolicy(Qt::ClickFocus);
   addTab(_isoDataTab, "Isolines");
   _streamDataTab = new StreamDataTab(data, visualizationTab);//(data, visualizationTab);
   _streamDataTab->setFocusPolicy(Qt::ClickFocus);
   addTab(_streamDataTab, "Streamlines");
}
