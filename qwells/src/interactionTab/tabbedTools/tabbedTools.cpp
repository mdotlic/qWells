#include "tabbedTools.h"
#include "chooserTab/plotChooser.h"
#include "adjusterTab/adjuster.h"

#include <QTabWidget>

TabbedTools::TabbedTools(Data * data,QWellsPlot * plot,Result * result) : QTabWidget()
{
   setTabPosition(QTabWidget::West);
   _chooserTab = new PlotChooser(data,plot,result);
   _chooserTab->setFocusPolicy(Qt::ClickFocus);
   QString chooserString = QString("CHOOSER");
   _adjusterTab = new Adjuster(data,plot,result,_chooserTab);
   _adjusterTab->setFocusPolicy(Qt::ClickFocus);
   QString adjusterString = QString("ADJUSTER");
   addTab(_chooserTab,chooserString);
   addTab(_adjusterTab,adjusterString);
}