#include "boundaryWidget.h"
#include "boundaryWidget/boundaryDataFrame.h"
#include "boundaryWidget/potentialComboBox.h"
#include "picture/painterWidget.h"
#include "wellsTab/wellsTable.h"
#include "piezoTab/piezoTable.h"
#include "engine/data.h"
#include "engine/boundary.h"
#include "widgets/doubleValue.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItemModel>


#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

BoundaryWidget::BoundaryWidget(WellsTable * wellsTable, PiezoTable * piezoTable, const bool modelExists) : QSplitter(Qt::Horizontal), _wellsTable(wellsTable), _modelExists(modelExists)
{   
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget START | modelExists = "<<modelExists<<"\n";
#endif
   
   QFrame * boundaryFrame = new QFrame;
   
   QVBoxLayout * boundaryLayout = new QVBoxLayout;
   boundaryLayout->setAlignment(Qt::AlignTop);
   boundaryLayout->setContentsMargins(10,10,0,0);
   
   QLabel * boundaryLabel = new QLabel(tr("BOUNDARY"));
   boundaryLayout->addWidget(boundaryLabel);
   
   QFrame * boundaryData = new QFrame;
   _boundaryDataLayout = new QVBoxLayout;
   _boundaryDataLayout->setAlignment(Qt::AlignLeft);
   constructBoundaryComboBox();
   QHBoxLayout * stretchLayout = new QHBoxLayout;
   stretchLayout->addWidget(_boundaryTypeBox);
   stretchLayout->addStretch();
   _boundaryDataLayout->addLayout(stretchLayout);
   connect(_boundaryTypeBox,SIGNAL(activated(int)),
           this,SLOT(activateCorrectWidget(int)));
   boundaryData->setLayout(_boundaryDataLayout);
   
   boundaryLayout->addWidget(boundaryData);
   boundaryFrame->setLayout(boundaryLayout);
   
   boundaryFrame->setFrameShape(QFrame::StyledPanel);
   boundaryFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   
   addWidget(boundaryFrame);
   
   // interactive map :
   
   QWidget * pictureWidget = new QWidget;
   QVBoxLayout * pictureLayout = new QVBoxLayout;

   _painterWidget = new PainterWidget(this,_wellsTable,piezoTable);

   pictureLayout->addWidget(_painterWidget);
   pictureWidget->setLayout(pictureLayout);

   addWidget(pictureWidget);
   
   if (modelExists)
   {
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget | loading model\n";
#endif
      Boundary & boundary = _wellsTable->data()->getBoundary();
      const size_t iMax = std::numeric_limits<size_t>::max();
      const double dMax = std::numeric_limits<double>::max();
      // check if boundaryType is defined in model, construct _boundary
      const size_t boundaryType = boundary.type();
      if (boundaryType != iMax) 
      {
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget | loading model - boundaryType = "<<boundaryType<<"\n";
#endif
         _boundaryTypeBox->setCurrentIndex(boundaryType+1);
         activateCorrectWidget(boundaryType+1);// -1 difference with data
         const int noOfWells = _wellsTable->rowCount()-1;
         if (noOfWells == 0) return;
         if (boundary.refWell() != iMax)
         {
            const int refWell = int(boundary.refWell())+1;
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget | loading model - refWell = "<<boundary.refWell()<<"\n";
#endif
            emit _boundary->refWell()->activated(refWell);
            _boundary->refWell()->setCurrentIndex(refWell);
            QString dString;
            if (boundary.alpha() != dMax)
               _boundary->alpha()->enterData(boundary.alphaDeg());
            if ((boundaryType==3) && (boundary.beta() != dMax))
               _boundary->beta()->enterData(boundary.betaDeg());
            if (boundary.rho().size() > 0)
            {
               const int noOfDist = boundary.rho().size();
               for (int i=0;i<noOfDist;i++)
               {
                  if (boundary.rho(i) != dMax)
                     _boundary->distance(i)->enterData(boundary.rho(i));
               }
            }
            if (boundary.lineType().size() > 0)
            {
               const int noOfLineTypes = boundary.lineType().size();
               bool isImpermeable;
               for (int i=0;i<noOfLineTypes;i++)
               {
                  isImpermeable = boundary.lineType(i);
                  // constantpot = 1, imperm = 2
                  if (isImpermeable) 
                  {
                     _boundary->potential(i)->setCurrentIndex(1);
                     emit _boundary->potential(i)->activated(1);
                  }
                  else 
                  {
                     _boundary->potential(i)->setCurrentIndex(0);
                     emit _boundary->potential(i)->activated(0);
                  }
               }
            }
            if ((boundaryType>=1) && (boundaryType!=3) && 
                (boundary.maxDistance() != dMax)) 
               _boundary->maxDistance()->enterData(boundary.maxDistance());
         }
      }
   }
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget END | sizeOfThisWidget = "<<size()<<" | sizeHint = "<<sizeHint()<<"\n";
#endif
}

void BoundaryWidget::constructBoundaryComboBox()
{
   _boundaryTypeBox = new QComboBox();   
   _boundaryTypeBox->addItem(" --- Choose Boundary Type --- ");
   qobject_cast<QStandardItemModel *>(_boundaryTypeBox->model())->item(0)->setSelectable( false );
   _boundaryTypeBox->addItem(" - No Boundary - ");
   _boundaryTypeBox->addItem(" - One Line - ");
   _boundaryTypeBox->addItem(" - Parallel Lines - ");
   _boundaryTypeBox->addItem(" - Angle - ");
   _boundaryTypeBox->addItem(" - Three Lines - ");
   _boundaryTypeBox->addItem(" - Four Lines - ");
   // First : Set the combobox the editable (this allows us to use the lineEdit)
   _boundaryTypeBox->setEditable(true);
   // Second : Put the lineEdit in read-only mode
   _boundaryTypeBox->lineEdit()->setReadOnly(true);
   // Third  : Align the lineEdit to right
   _boundaryTypeBox->lineEdit()->setAlignment(Qt::AlignCenter);  
   // Fourth : Align each item in the combo to the right
   for(int i = 0; i < _boundaryTypeBox->count(); i++)
    _boundaryTypeBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
}

void BoundaryWidget::activateCorrectWidget(const int boundaryType)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget::activateCorrectWidget() START | Required boundary ID = "<<boundaryType<<" | boundary name = "<<_boundaryTypeBox->itemText(boundaryType)<<" | previously active boundary = "<<_boundaryTypeBox->itemText(_boundaryActive)<<"\n";
#endif
   if (boundaryType > _noOfBoundaryTypes + 1) assert(false);
   if (boundaryType != _boundaryActive)
   {
      if ((_boundaryActive>=0) && (_boundary != nullptr))
      {
         delete _boundary;
         _boundary = nullptr;
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget::activateCorrectWidget() | Deleted boundary " <<_boundaryTypeBox->itemText(_boundaryActive)<<" | activating boundary "<<_boundaryTypeBox->itemText(boundaryType)<<"\n";
#endif
      }
      if (boundaryType <= 0) return;
      bool isBoundaryAngles = false;
      int noOfLines = -1;
      const int typeInData = _wellsTable->data()->getBoundary().type() + 1;
//       qDebug()<<"BOUNDARY TEST | typeInData = "<<typeInData<<" = "<<boundaryType<<"  boundaryType\n";
      switch (boundaryType)
      {
         case 1: // no boundary
         {
            if (typeInData != boundaryType) _wellsTable->data()->setBoundary(0);
            noOfLines = 0;
            break;
         }
         case 2: // one line
         {
            if (typeInData != boundaryType) _wellsTable->data()->setBoundary(1);
            noOfLines = 1;
            break;
         }
         case 3: // parallel lines
         {
            if (typeInData != boundaryType) _wellsTable->data()->setBoundary(2);
            noOfLines = 2;
            break;
         }
         case 4: // angles
         {
            if (typeInData != boundaryType) _wellsTable->data()->setBoundary(3);
            noOfLines = 2;
            isBoundaryAngles = true;
            break;
         }
         case 5: // three lines
         {
            if (typeInData != boundaryType) _wellsTable->data()->setBoundary(4);
            noOfLines = 3;
            break;
         }
         case 6: // four lines
         {
            if (typeInData != boundaryType) _wellsTable->data()->setBoundary(5);
            noOfLines = 4;
            break;
         }
         default: break;
      }
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryWidget::activateCorrectWidget() | activating boundary "<<_boundaryTypeBox->itemText(boundaryType)<<"\n";
#endif
      
      if (boundaryType >= 2)
      {
         _boundary = new BoundaryDataFrame(this,_wellsTable,noOfLines,isBoundaryAngles);
         _boundaryDataLayout->addWidget(_boundary,Qt::AlignLeft);
      }
      _boundaryActive = boundaryType;
      _painterWidget->repaint();
      //_boundaryTypeBox->setStyleSheet("font:bold;");
   }
}
