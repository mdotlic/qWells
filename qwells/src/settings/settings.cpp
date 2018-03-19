#include "settings.h"
#include "timeUnitSettings.h"
#include "tableSettings.h"
#include "plotSettings.h"


#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

// NOTE: Settings should, upon starting, read the settings file stored locally. If there is no such fild, default values should be used.
Settings::Settings(QWells * qWells) : QObject()
{
   _timeUnitSettings = new TimeUnitSettings(qWells);
   _tableSettings = new TableSettings(qWells);
   _plotSettings = new PlotSettings(qWells);
}

Settings::~Settings()
{
   delete _timeUnitSettings;
   delete _tableSettings;
   delete _plotSettings;
}

void Settings::startDialog() // FIXME complete rewrite
{
   _dialog = new QDialog();
   
   _dialog->setWindowTitle (tr ("Options"));

   QGridLayout * layout = new QGridLayout;

   QTabWidget * tabWidget = new QTabWidget;

   QWidget * tab0 = new QWidget;
   QGridLayout * layout0 = new QGridLayout;

   QLabel * languageLabel = new QLabel (tab0);
   languageLabel->setText(tr("Language\n(requires restart)"));
   layout0->addWidget (languageLabel,2,1);
   QStringList languageList;
   languageList << "English" << "Srpski";
   QComboBox * language = new QComboBox (tab0);
   language->insertItems (0, languageList);
   language->setObjectName ("language");
   language->setEditable (false);
   layout0->addWidget (language,2,2);

// file location to open

   QLabel * fileLocationLabel = new QLabel (tab0);
   fileLocationLabel->setText(tr("Open file location:"));
   layout0->addWidget (fileLocationLabel,3,1);
   QStringList fileLocationList;
   fileLocationList << "Last used" << "Choose location";
   QComboBox * fileLocationBox = new QComboBox (tab0);
   fileLocationBox->insertItems (0, fileLocationList);
   fileLocationBox->setObjectName ("fileLocation");
   fileLocationBox->setEditable (false);
   connect(fileLocationBox,SIGNAL(currentIndexChanged(int)),
           this,SLOT(updateFileLocation(int)));
   layout0->addWidget (fileLocationBox,3,2);
   _fileLocation = QString(""); // FIXME

   tab0->setLayout (layout0);

   tabWidget->addTab (tab0, "General");
   layout->addWidget (tabWidget,1,1);
   _dialog->setLayout (layout);
}

void Settings::updateFileLocation(int index)
{
   if (index==0) {/* TODO return last used location */}
   else if (index==1) {/* TODO open file dialog & return the chosen location */}
}
