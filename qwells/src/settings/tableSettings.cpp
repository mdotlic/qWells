#include "tableSettings.h"

#include "qWells.h"

#include <QDialog>
#include <QStackedWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFrame>

#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

TableSettings::TableSettings(QWells * qWells) : QObject(), _qWells(qWells) 
{}

void TableSettings::startDialog()
{
#ifdef QDEBUG_ALL
   qDebug()<<"TableSettings::startDialog START\n";
#endif
   _dialog = new QDialog(_qWells);
   
   _dialog->setWindowTitle("Table settings");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * maxRowsMainLabel = new QLabel("Max number of rows shown in tables:");
   maxRowsMainLabel->setAlignment(Qt::AlignLeft);
   maxRowsMainLabel->setStyleSheet("padding:0px;");
      
   _maxRowsLabel = new QLabel;
   _maxRowsLabel->setFrameShape(QFrame::StyledPanel);
   _maxRowsLabel->setStyleSheet("padding:14px;font:12px bold;");
   _maxRowsLabel->setAlignment(Qt::AlignCenter);
   
   QLabel * changeLabel = new QLabel("Change");
   changeLabel->setFixedSize(changeLabel->sizeHint());
   QComboBox * labelChooser = new QComboBox;
   labelChooser->addItem("Maximal, screen dependent (default)");
   labelChooser->addItem("User defined");
   labelChooser->setEditable(true);
   labelChooser->lineEdit()->setReadOnly(true);
   labelChooser->lineEdit()->setAlignment(Qt::AlignCenter);
   labelChooser->setItemData(0, Qt::AlignCenter, Qt::TextAlignmentRole); 
   labelChooser->setItemData(1, Qt::AlignCenter, Qt::TextAlignmentRole); 
   connect(labelChooser,SIGNAL(currentIndexChanged(int)),
           this,SLOT(setMaxRowsLabel(int)));

   _setLabel = new QLabel("Set");
   _setLabel->setFixedSize(changeLabel->sizeHint());
   _setNoOfRows = new QComboBox;
   _setNoOfRows->setEditable(true);
   for (int i=1;i<=_maxRowsShown;i++)
   {
      _setNoOfRows->addItem(QString(QString::number(i)));
      _setNoOfRows->setItemData(i,Qt::AlignCenter,Qt::TextAlignmentRole);
   }
   _setNoOfRows->lineEdit()->setReadOnly(true);
   _setNoOfRows->lineEdit()->setAlignment(Qt::AlignCenter);
   connect(_setNoOfRows,SIGNAL(currentIndexChanged(int)),
           this,SLOT(setCurrentRowsShown(int)));
   
   QWidget * spacer = new QWidget;
   spacer->setFixedSize(_setNoOfRows->sizeHint());
   
   
   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,_dialog);
   connect(buttonBox,SIGNAL(accepted()),_dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),_dialog,SLOT(reject()));
         
   layout->addWidget(maxRowsMainLabel,1,0,1,3);
   layout->addWidget(_maxRowsLabel,2,0,1,3);
   layout->addWidget(changeLabel,4,0);
   layout->addWidget(labelChooser,4,1,1,2);
   layout->addWidget(_setLabel,5,0,1,2);
   layout->addWidget(_setNoOfRows,5,1);
   layout->addWidget(spacer,5,2);
   layout->addWidget(buttonBox,7,0,1,3,Qt::AlignCenter);
   layout->setRowStretch(3,10);

   if ((!_maxRowsIsLoaded)  || (_currentRowsShown == _maxRowsShown))
   {
      labelChooser->setCurrentIndex(0);
      setMaxRowsLabel(0);
   }
   else labelChooser->setCurrentIndex(1);
   
   _dialog->setLayout(layout);
   if (_dialog->exec() == QDialog::Accepted) 
      emit setMaxRows(_currentRowsShown+1);
#ifdef QDEBUG_ALL
   qDebug()<<"TableSettings::startDialog END\n";
#endif
}

void TableSettings::setMaxRowsLabel(const int id)
{
#ifdef QDEBUG_ALL
   qDebug()<<"TableSettings::setMaxRowsLabel | setting label to "<<id<<" value\n";
#endif
   if (id == 0)
   {
      QString str = QString("MAXIMAL, SCREEN DEPENDENT (currently = %1)").arg(_maxRowsShown);
      _maxRowsLabel->setText(str);
      _setLabel->hide();
      _setNoOfRows->hide();
      _currentRowsShown = _maxRowsShown;
   }
   else 
   {
      QString str = QString("USER DEFINED (currently = %1)").arg(_currentRowsShown);
      _maxRowsLabel->setText(str);
      _setLabel->show();
      _setNoOfRows->show();
   }
}

void TableSettings::setMaxRowsShown(const int hh)
{
   _maxRowsShown = int((_maxTableHeight - hh - 2) / _cellHeight);
   _currentRowsShown = _maxRowsShown;
}

void TableSettings::setCurrentRowsShown(const int index)
{
   _currentRowsShown = index + 1;
   _maxRowsIsLoaded = true; // TODO change the name of this var!
}

void TableSettings::setSizes(const int ch, const int hHeaderHeight)
{
   // NOTE: later, function should check if cellHeight is set by user
   _cellHeight = ch;
   if (!_maxRowsIsLoaded) setMaxRowsShown(hHeaderHeight);
   else setCurrentRowsShown(hHeaderHeight);
}

void TableSettings::setScreenGeometry(const QRect & rect) 
{ 
   _screenGeometry = rect;
//    _maxTableHeight = _screenGeometry.height() - 580; // 580 comes from observed height of the 4-line boundary widget, with menu and window borders added
   _maxTableHeight = _screenGeometry.height() - 650; // 580 comes from observed height of the 4-line boundary widget, with menu and window borders added
}
