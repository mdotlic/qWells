#include "boundaryDataFrame.h"

#include "parameterTab/boundaryWidget.h"
#include "parameterTab/picture/painterWidget.h"
#include "parameterTab/wellsTab/wellsTable.h"
#include "engine/data.h"
#include "widgets/doubleValue.h"
#include "widgets/tables/lineEdit.h"
#include "potentialComboBox.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayoutItem>
#include <QComboBox>
#include <QLabel>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QChar>

#include <assert.h>
#include <QDebug>
// #ifndef QDEBUG_ALL
// #define QDEBUG_ALL
// #endif

BoundaryDataFrame::BoundaryDataFrame(BoundaryWidget * boundaryWidget, WellsTable * wellsTable, const int noOfLines, const bool isBoundaryAngles) : QWidget(), _data(wellsTable->data()), _boundaryWidget(boundaryWidget), _wellsTable(wellsTable), _noOfLines(noOfLines), _isBoundaryAngles(isBoundaryAngles)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame START | No of lines = "<<_noOfLines<<"\n";
#endif
      
   for (int i=0;i<_noOfLines;i++) 
   {
      _lineFrame.push_back(nullptr);
      _distanceModified.push_back(false);
   }
   
   _outerLayout = new QHBoxLayout;
   _outerLayout->setAlignment(Qt::AlignLeft);
   
   QFrame * refWellAngleFrame = new QFrame;
//    refWellAngleFrame->setFrameShape(QFrame::StyledPanel);
//    refWellAngleFrame->setFrameShadow(QFrame::Sunken);
   QVBoxLayout * refWellAngleLayout = new QVBoxLayout;
   refWellAngleLayout->setContentsMargins(20,10,20,20);
   
   QLabel * refWellLabel = new QLabel(QString(tr("Referent well")));
   refWellLabel->setAlignment(Qt::AlignCenter);
   refWellAngleLayout->addWidget(refWellLabel);
   constructReferentWellComboBox();
   refWellAngleLayout->addWidget(_refWell);
      
   //QLabel * alphaLabel = new QLabel(QString(tr("alpha")));
   QLabel * alphaLabel = new QLabel(QString(QChar(0x03B1)));
   alphaLabel->setToolTip("in degrees");
   alphaLabel->setAlignment(Qt::AlignCenter);
   refWellAngleLayout->addWidget(alphaLabel);
   _alpha = new DoubleValue(_data,"alpha");
   refWellAngleLayout->addWidget(_alpha);
   connect(_alpha,&DoubleValue::focusNextItem,
           this,&BoundaryDataFrame::moveFocusToNextItem);
   connect(_alpha,SIGNAL(returnPressed()), this, SLOT(repaint()));
   
   if (_isBoundaryAngles)
   {
      //QLabel * betaLabel = new QLabel(QString(tr("beta")));
      QLabel * betaLabel = new QLabel(QString(QChar(0x03B2)));
      betaLabel->setToolTip("in degrees");
      betaLabel->setAlignment(Qt::AlignCenter);
      refWellAngleLayout->addWidget(betaLabel);
      _beta = new DoubleValue(_data,"beta");
      refWellAngleLayout->addWidget(_beta);
      connect(_beta,&DoubleValue::focusNextItem,
              this,&BoundaryDataFrame::moveFocusToNextItem);
      connect(_beta,SIGNAL(returnPressed()), this, SLOT(repaint()));
   }
   
   refWellAngleLayout->addStretch();
   refWellAngleFrame->setLayout(refWellAngleLayout);
   refWellAngleFrame->setMaximumWidth(200); // TODO bring back if not good
   
   _outerLayout->addWidget(refWellAngleFrame,Qt::AlignLeft);
   
   connect(_refWell,SIGNAL(activated(int)),
           this,SLOT(referentWellChosen(int)));
   connect(_wellsTable,SIGNAL(rowMade(int,bool)),
           this,SLOT(addRefWell(int,bool)));
   connect(_data,&Data::distanceModified,
           this,&BoundaryDataFrame::distanceModified);
   connect(_data,&Data::itemNameChanged,
           this,&BoundaryDataFrame::changeWellName);

   _outerLayout->setSpacing(60);
   QFrame * lineOuterFrame = new QFrame;
   lineOuterFrame->setFixedWidth(450); // TODO bring back if not good
   _lineOuterLayout = new QVBoxLayout;
   lineOuterFrame->setLayout(_lineOuterLayout);
   _outerLayout->addWidget(lineOuterFrame,Qt::AlignLeft);
   QWidget * modelDrawing = new QWidget; // this is where Milan puts drawing
   _outerLayout->addWidget(modelDrawing);
   _outerLayout->addStretch();
   
   setLayout(_outerLayout);
   
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame END\n";
#endif
}

void BoundaryDataFrame::constructReferentWellComboBox()
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::constructReferentWellComboBox() START\n";
#endif
   _refWell = new QComboBox();
   _refWell->setEditable(true);
   _refWell->lineEdit()->setReadOnly(true);
   _refWell->lineEdit()->setAlignment(Qt::AlignCenter);  
   _refWell->addItem(" -- Choose --");
   _refWell->setItemData(0, Qt::AlignCenter, Qt::TextAlignmentRole);
   qobject_cast<QStandardItemModel *>(_refWell->model())->item(0)->setSelectable( false );
   const int noOfWells = _wellsTable->rowCount()-1;
   QString wellsName;
   for (int k=0;k<noOfWells;k++)
   {
      wellsName = qobject_cast<LineEdit*>(_wellsTable->cellWidget(k,0))->text();
      _refWell->addItem(wellsName);
   }
   for (int k=0;k<_refWell->count();k++) _refWell->setItemData(k, Qt::AlignCenter, Qt::TextAlignmentRole);
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::constructReferentWellComboBox() END\n";
#endif
}

void BoundaryDataFrame::addRefWell(const int wellNo, const bool)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::addRefWell() START | wellNo = "<<wellNo<<"\n";
#endif
   if (_refWell != nullptr) 
   {
      const QString & wellName = static_cast<LineEdit*>(_wellsTable->cellWidget(wellNo,0))->text();
      _refWell->addItem(wellName);
      _refWell->setItemData(wellNo+1, Qt::AlignCenter, Qt::TextAlignmentRole);
   }
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::addRefWell() END\n";
#endif
}

// should be called by hitting Delete key on a row in a well table
void BoundaryDataFrame::removeRefWellItem(const int wellNo)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::removeRefWellItem() START | wellNo = "<<wellNo<<"\n";
#endif
   const int currentRefWellID = _refWell->currentIndex();
   if (wellNo == currentRefWellID) clearLineOuterLayout();
   _refWell->removeItem(wellNo);   
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::removeRefWellItem() END\n";
#endif
}

void BoundaryDataFrame::changeWellName(const int wellID, const int itemFlag, const QString & name)
{
   if (itemFlag == 1) return; // it's piezometer
   _refWell->setItemText(wellID+1,name);
   const int refWellID = _refWell->currentIndex()-1;
   if (refWellID >= 0)
   {
      if (refWellID == wellID) 
      {
         for (int i=0;i<_noOfLines;i++) setDistanceLabel(i,name);
      }
   }
}

void BoundaryDataFrame::referentWellChosen(const int itemNo)
{
   // submit referent well:
   const int wellNo = itemNo - 1; // minus the "choose" item

   if ((int) _data->getBoundaryPtr()->refWell() != wellNo) 
      _data->setRefWell(wellNo);
   if (_distance.size() == 0) 
   {
      for (int i=0;i<_noOfLines;i++)
      {
         _distance.push_back(nullptr);
         _potential.push_back(nullptr);
      }
   }
//    if (_distance.size() == 0) _distance.resize(_noOfLines);
//    if (_potential.size() == 0) _potential.resize(_noOfLines);
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::refWellChosen() | wellNo = "<<wellNo<<" noOfLines = "<<_noOfLines<<"\n";
#endif
   // clear previous widgets in _lineFrameLayout 
   _distanceLabel.clear();
   clearLineOuterLayout();
   // construct new widgets
   for (int i=0;i<_noOfLines;i++) 
   { 
      _distanceLabel.push_back(new QLabel("Distance"));
      _distanceLabel[i]->setAlignment(Qt::AlignCenter);
      constructLineFrame(i,_refWell->itemText(itemNo));
   }
   if ((!_isBoundaryAngles) && ((_noOfLines >= 2) && (_noOfLines <= 4))) 
   {
      _lineOuterLayout->addSpacing(20);
      constructFictiveDistance();
   }
   _lineOuterLayout->addStretch();
}

void BoundaryDataFrame::clearLineOuterLayout()
{
   if (_lineFrame[0] != nullptr) 
   {
      for (int i=0;i<_noOfLines;i++) 
      {
         delete _lineFrame[i];
         _lineFrame[i] = nullptr;
      }
      QLayoutItem * item = _lineOuterLayout->takeAt(0);
      while (item) 
      {
         delete item;
         item = _lineOuterLayout->takeAt(0);
      }
   }
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::clearLineOuterLayout() | referent well changed, layout cleared\n";
#endif
}

void BoundaryDataFrame::potentialChosen(const int potentialTypeID, const int potentialID)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::potentialChosen() | potentialType = "<<potentialTypeID-1<<" | potentialID (lineNo.) = "<<potentialID<<"\n";
#endif
   _data->setBoundaryType(potentialID,bool(potentialTypeID));
   _boundaryWidget->painterWidget()->repaint();
}

void BoundaryDataFrame::setDistanceLabel(const int lineNo, const QString & wellName)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::setDistanceLabel | lineNo = "<<lineNo<<" | wellName = "<<wellName<<"\n";
#endif
   _distanceLabel[lineNo]->setText(QString("Distance").append((QString(" to ")).append(wellName)));
   _distanceLabel[lineNo]->setToolTip("in metres");
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::setDistanceLabel | label text set to = "<<_distanceLabel[lineNo]->text()<<"\n";
#endif
}

void BoundaryDataFrame::constructLineFrame(const int lineNo, const QString & wellName)
{
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::constructLineFrame() START | lineNo = "<<lineNo<<"\n";
#endif
   _lineFrame[lineNo] = new QFrame;
   QGridLayout * frameLayout = new QGridLayout;
   frameLayout->setContentsMargins(20,0,10,0);
   
   QLabel * lineLabel = new QLabel(QString("LINE %1").arg(lineNo+1));
   setDistanceLabel(lineNo,wellName);
   _distance[lineNo] = new DoubleValue(_data,"distance",lineNo);
   connect(_distance[lineNo],&DoubleValue::focusNextItem,
           this,&BoundaryDataFrame::moveFocusToNextItem);
   connect(_distance[lineNo],&DoubleValue::newDistanceSet,
           this,&BoundaryDataFrame::unmodifyDistance);
   QLabel * potentialLabel = new QLabel(tr("Boundary type"));
   potentialLabel->setAlignment(Qt::AlignCenter);
   _potential[lineNo] = new PotentialComboBox(lineNo);
   connect(_potential[lineNo],&PotentialComboBox::activatedWithID,
           this,&BoundaryDataFrame::potentialChosen);
   frameLayout->addWidget(_distanceLabel[lineNo],0,2,Qt::AlignBottom);
   frameLayout->addWidget(potentialLabel,0,4,Qt::AlignBottom);
   frameLayout->addWidget(lineLabel,1,0,Qt::AlignVCenter);
   frameLayout->addWidget(_distance[lineNo],1,2,Qt::AlignTop);
   frameLayout->addWidget(_potential[lineNo],1,4,Qt::AlignTop);
   frameLayout->setColumnMinimumWidth(1,15);
   frameLayout->setColumnMinimumWidth(3,10);
   _distance[lineNo]->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Fixed);
   _potential[lineNo]->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
   
   _lineFrame[lineNo]->setLayout(frameLayout);

   _lineOuterLayout->addWidget(_lineFrame[lineNo]);
   _lineOuterLayout->setSpacing(15);
#ifdef QDEBUG_ALL
   qDebug()<<"BoundaryDataFrame::constructLineFrame() END\n";
#endif
}

void BoundaryDataFrame::constructFictiveDistance()
{
   QHBoxLayout * maxDistLayout = new QHBoxLayout;
   maxDistLayout->setContentsMargins(20,0,10,0);
   QLabel * maxDistLabel = new QLabel(tr("Maximal distance of fictive wells"));
   maxDistLabel->setToolTip("in metres");
   maxDistLabel->setAlignment(Qt::AlignCenter);
   _maxDistance = new DoubleValue(_data,"maxDistance");
   connect(_maxDistance,&DoubleValue::newMaxDistanceSet,
           this,&BoundaryDataFrame::unmodifyMaxDistance);
   maxDistLayout->addWidget(maxDistLabel);
   maxDistLayout->addWidget(_maxDistance);
   _lineOuterLayout->addLayout(maxDistLayout);
}

void BoundaryDataFrame::moveFocusToNextItem()
{
   QWidget * currentItem = static_cast<QWidget*>(sender());
   if (currentItem == _refWell) _alpha->setFocus();
   else if (currentItem == _alpha)
   {
      if (_isBoundaryAngles) _beta->setFocus();
      else if (_lineFrame[0] != nullptr) _distance[0]->setFocus();
   }
   else if ((currentItem == _beta) && (_lineFrame[0] != nullptr)) _distance[0]->setFocus();
   else if ((_lineFrame[0] == nullptr) || (currentItem == _maxDistance)) return;
   else
   {
      int k = _noOfLines;
      for (int i=0;i<_noOfLines;i++)
      {
         if (currentItem == _distance[i]) 
         {
            k = i;
            break;
         }
      }
      if (k < _noOfLines-1) _distance[k+1]->setFocus();
      else if (_maxDistance != nullptr) _maxDistance->setFocus();
   }
}

void BoundaryDataFrame::repaint()
{
   _boundaryWidget->painterWidget()->repaint();
}

void BoundaryDataFrame::distanceModified(const int distID)
{
   if (!_distanceModified[distID]) 
   {
      _distance[distID]->setModified("");
      _distanceModified[distID] = true;
   }
}

void BoundaryDataFrame::unmodifyDistance(const int distID)
{
   _distanceModified[distID] = false;
   _boundaryWidget->painterWidget()->repaint();
}

void BoundaryDataFrame::maxDistanceModified()
{
   _maxDistance->setModified("");
   _maxDistanceModified = true;
}

void BoundaryDataFrame::unmodifyMaxDistance()
{
   _maxDistanceModified = false;
   _boundaryWidget->painterWidget()->repaint();
}
