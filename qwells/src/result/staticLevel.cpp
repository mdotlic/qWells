#include "staticLevel.h"
#include "qWells.h"
#include "engine/data.h"
#include "centralWindow.h"
#include "interactionTab/interactionTab.h"
#include "widgets/qcustomplot/qwellsPlot.h"
#include "visualizationTab/visualizationTab.h"
#include "visualizationTab/visData.h"
#include "visualizationTab/linesTabs/tabs.h"
#include "visualizationTab/linesTabs/isoDataTab.h"
#include "printTab/printTab.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QDebug>

StaticLevel::StaticLevel(QWells * qWells): QDialog(qWells)
{//window for setting static level
   setWindowTitle("Set static level");
   QGridLayout * layout = new QGridLayout;
   QCheckBox * check = new QCheckBox("Use static level");
   check->setCheckState(qWells->centralWindow()->interactionTab()->splot()->useStaticBoolean() ? Qt::Checked : Qt::Unchecked);
   layout->addWidget(check, 0, 0, 1, 1);
   QLabel * label= new QLabel("Static level");
   layout->addWidget(label,1,0,1,1);
   _lineEdit = new QLineEdit;
   _lineEdit->setText(QString::number(*qWells->data()->staticLevel()));
   //if(!check->isChecked())
   _lineEdit->setDisabled(!check->isChecked());

   layout->addWidget(_lineEdit,1,1,1,1);
   setLayout(layout);
   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel, Qt::Horizontal,this);
   connect(buttonBox,SIGNAL(accepted()), this, SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()), this, SLOT(reject()));
   
   layout->addWidget(buttonBox,2,0,1,2,Qt::AlignCenter);
   setLayout(layout);
   connect(check, SIGNAL(stateChanged(int)), this, SLOT(staticSet(int)));
   if(exec() == QDialog::Accepted)
   {
      qWells->centralWindow()->interactionTab()->splot()->setStaticLevel(check->isChecked());
      qWells->centralWindow()->printTab()->splot()->setStaticLevel(check->isChecked());
      qWells->data()->setStatic(_lineEdit->text().toDouble());
      //call of the setRangeStaticLevel slot for splot on interaction tab
      qWells->centralWindow()->interactionTab()->splot()->setRangeStaticLevel(qWells->centralWindow()->interactionTab()->splot()->yAxis->range());
      //call of the setRangeStaticLevel slot for splot on print tab
      qWells->centralWindow()->printTab()->splot()->setRangeStaticLevel(qWells->centralWindow()->printTab()->splot()->yAxis->range());

      qWells->centralWindow()->visualizationTab()->visData()->tabs()->isoDataTab()->enableUseStatic(check->isChecked());
   }
}

void StaticLevel::staticSet(int state)
{
   if(state)
      _lineEdit->setDisabled(false);
   else
      _lineEdit->setDisabled(true);
}
