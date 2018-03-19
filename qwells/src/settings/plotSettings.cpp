#include "plotSettings.h"
#include "qWells.h"
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
//#include <QCheckBox>

PlotSettings::PlotSettings(QWells * qWells) : QObject(), _qWells(qWells) 
{}

void PlotSettings::startDialog()
{
   _dialog = new QDialog(_qWells);
   
   _dialog->setWindowTitle("Plot settings");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * zoomFactorLabel = new QLabel("Zoom factor:");
   zoomFactorLabel->setAlignment(Qt::AlignVCenter);
   zoomFactorLabel->setStyleSheet("padding:0px;");
   layout->addWidget(zoomFactorLabel,0,0,1,1);
   QLineEdit * lineEditZoomFactor = new QLineEdit(_dialog);
   lineEditZoomFactor->setText(QString::number(_zoomFactor));
   layout->addWidget(lineEditZoomFactor,0,1,1,1);
   
   QLabel * lineWidthLabel = new QLabel("Line width:");
   lineWidthLabel->setAlignment(Qt::AlignVCenter);
   lineWidthLabel->setStyleSheet("padding:0px;");
   layout->addWidget(lineWidthLabel,1,0,1,1);
   QLineEdit * lineEditLineWidth = new QLineEdit(_dialog);
   lineEditLineWidth->setText(QString::number(_lineWidth));
   layout->addWidget(lineEditLineWidth,1,1,1,1);


   /*QLabel * checkLegendLabel = new QLabel("Show legend on the interaction tab:");
   QCheckBox * checkLegendBox = new QCheckBox;
   checkLegendBox->setChecked(_showLegend);

   layout->addWidget(checkLegendLabel, 1,0,1,2);
   layout->addWidget(checkLegendBox, 1, 2, 1,1);*/

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,_dialog);
   connect(buttonBox,SIGNAL(accepted()),_dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),_dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   _dialog->setLayout(layout);
   if(_dialog->exec() == QDialog::Accepted)
   {
      //_showLegend=checkLegendBox->isChecked();
      _zoomFactor=lineEditZoomFactor->text().toDouble();
      _lineWidth = lineEditLineWidth->text().toDouble();
      emit setZoomFactor(_zoomFactor, _lineWidth);
      //emit showLegend(_showLegend);
   }
}

QWells * PlotSettings::qWells()
{
   return _qWells;
}

