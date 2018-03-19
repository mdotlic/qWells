#include "timeUnitSettings.h"

#include "qWells.h"
#include "engine/data.h"
#include "engine/modelInformation.h"

#include <QObject>
#include <QMessageBox>
#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QString>

#include <QDebug>

TimeUnitSettings::TimeUnitSettings(QWells * qWells) : QObject(), _qWells(qWells)
{
   _timeUnitsSet = false;
}

void TimeUnitSettings::checkIfSet()
{
   if (_timeUnitsSet)
   {
      const size_t timeID = _qWells->data()->getModel().timeUnit();
      QString text;
      switch (timeID)
      {
         case 0:
            text = "Time unit : second";
            break;
         case 1:
            text = "Time unit : minute";
            break;
         case 2:
            text = "Time unit : hour";
            break;
         case 3:
            text = "Time unit : day";
            break;
         default:
            text = "ERROR";
      }
      QMessageBox * showTimeUnits = 
      new QMessageBox(QMessageBox::Icon::Information,QString("INFO"),text,QMessageBox::Ok,_qWells);
      showTimeUnits->exec();
      return;
   }
   else startDialog();   
}

void TimeUnitSettings::startDialog()
{
   QDialog dialog(_qWells);
   QFormLayout form(&dialog);
   dialog.setWindowTitle("Time unit settings");
   QComboBox * comboBox = new QComboBox();
   comboBox->addItem("seconds");
   comboBox->addItem("minutes");
   comboBox->addItem("hours");
   comboBox->addItem("days");
   comboBox->setCurrentIndex(_qWells->data()->getModel().timeUnit());

   QString label = QString("Times in ");
   form.addRow(label, comboBox);
   
   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
   
   if (dialog.exec() == QDialog::Accepted) {
      _qWells->data()->setTimeUnit(comboBox->currentIndex());
  }
}

void TimeUnitSettings::timeUnitIsSet(const bool yN) 
{ 
   _timeUnitsSet = yN;
   if (yN)
   {
      const size_t timeUnit = _qWells->data()->getModel().timeUnit();
      emit _qWells->data()->timeUnitSet(timeUnit);
   }
}