#include "visualizationTab.h"
#include "visData.h"
#include "visPicture.h"
#include "visResult.h"
#include <QDebug>
#include <QList>
#include <QHBoxLayout>
#include <QVBoxLayout>

VisualizationTab::VisualizationTab(Data * data)
{
   QWidget * pictWidget = new QWidget;
   QVBoxLayout * visPictLayout = new QVBoxLayout;

   _visResult = new VisResult(data);
   
   _visData = new VisData(data, this);
   addWidget(_visData);

   _visPicture = new VisPicture(data, this);
   visPictLayout->addWidget(_visPicture);
   pictWidget->setLayout(visPictLayout);
   addWidget(pictWidget);
   QList<int> list= this->sizes();
   list.replace(0,this->height()/0.8);
   list.replace(1,this->height()/0.2);
   this->setSizes(list);

}

void VisualizationTab::refresh()
{
   _visData->refresh();  
}
