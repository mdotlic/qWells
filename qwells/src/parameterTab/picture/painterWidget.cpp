#include "painterWidget.h"
#include "engine/data.h"
#include "qWells.h"
#include "helperFunctions.h"
#include "parameterTab/boundaryWidget.h"
#include "parameterTab/boundaryWidget/boundaryDataFrame.h"
#include "parameterTab/wellsTab/wellsTable.h"
#include "parameterTab/piezoTab/piezoTable.h"
#include "widgets/doubleValue.h"
#include "lineEditSetBoundary.h"
#include <QPainter>
#include <QPointF>
#include <QMouseEvent>
#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QFileDialog>
#include <QComboBox>
#include <QStatusBar>
#include <QCheckBox>
#include <QCursor>
#include <QDebug>


PainterWidget::PainterWidget(BoundaryWidget * boundaryWidget, WellsTable * wellsTable, PiezoTable * piezoTable)
    : QWidget(), _boundaryWidget(boundaryWidget), _wellsTable(wellsTable), _piezoTable(piezoTable), _data(_wellsTable->data())
{
   _color = {Qt::black, Qt::red, Qt::blue, Qt::green, 
      Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
/*    shape = Polygon;
    antialiased = false;
    transformed = false;
    pixmap.load(":/images/qt-logo.png");*/
   QSize newSize(width(),height());
   _image=new QImage(newSize, QImage::Format_RGB32);
   loadImage(_data->getModel().pictureName());
   _xLineEditB.resize(1);
   _yLineEditB.resize(1);
   _data->xmin()=_data->getModel().visXmin();
   _data->xmax()=_data->getModel().visXmax();
   _data->ymin()=_data->getModel().visYmin();
   _data->ymax()=_data->getModel().visYmax();
   _data->setRatio();

    setMouseTracking(true);

    _wellXOffset=_data->getModel().wellXOffset();
    _wellYOffset=_data->getModel().wellYOffset();
    _wellRadius=_data->getModel().wellRadius();
    _wellColor=_data->getModel().wellColor();
    _showWellNames=_data->getModel().showWellNames();
    _piezXOffset=_data->getModel().piezXOffset();
    _piezYOffset=_data->getModel().piezYOffset();
    _piezRadius=_data->getModel().piezRadius();
    _piezColor=_data->getModel().piezColor();
    _showPiezNames=_data->getModel().showPiezNames();
    _constColor = _data->getModel().constColor();
    _impermColor = _data->getModel().impermColor();

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    //_fileName=QString(" ");
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));
    connect(_wellsTable,&WellsTable::repaintPicture,
            this,&PainterWidget::repaint);
    connect(_piezoTable,&PiezoTable::repaintPicture,
            this,&PainterWidget::repaint);
}

double PainterWidget::transformX(double x) const
{
   return _transform_a*x+_transform_b;
}

double PainterWidget::transformY(double y) const
{
   return _transform_c*y+_transform_d;
}

double PainterWidget::inverseTransformX(double x) const
{
   return (1/_transform_a)*x-_transform_b/_transform_a;
}

double PainterWidget::inverseTransformY(double y) const
{
   return (1/_transform_c)*y-_transform_d/_transform_c;
}

QSize PainterWidget::minimumSizeHint() const
{
    return QSize(150,150);
}

void PainterWidget::mouseDoubleClickEvent(QMouseEvent * event) 
{
   if(!_addDialogIsActive)
      return;
   if(_imagePropActive)
   {
      _lineEditXmin->setText(QString::number(_data->xmin()));
      _lineEditXmax->setText(QString::number(_data->xmax()));
      _lineEditYmin->setText(QString::number(_data->ymin()));
      _lineEditYmax->setText(QString::number(_data->ymax()));
   }
   else if (!_boundaryDialogIsActive)
   {
      _xLineEdit->setText(QString::number(inverseTransformX(event->pos().x())));
      _yLineEdit->setText(QString::number(inverseTransformY(event->pos().y())));
   }else if(_boundaryDialogIsActive)
   {
      _xLineEditB[_clicked]->setText(QString::number(inverseTransformX(event->pos().x())));
      _yLineEditB[_clicked]->setText(QString::number(inverseTransformY(event->pos().y())));
      repaint();
      _clicked++;
      if(_clicked==_xLineEditB.size())
         _clicked=0;
   }
}

void PainterWidget::mouseMoveEvent(QMouseEvent * event)
{
   //qDebug()<<" imamo move event "<<event->pos().x()<<" "<<event->pos().y()<<"\n";
   //setMouseTracking(false);
   if(_drag)
   {
      double moveX=inverseTransformX(event->pos().x())-inverseTransformX(_lastDragPos.x());
      double moveY=inverseTransformY(event->pos().y())-inverseTransformY(_lastDragPos.y());
      _data->xmin()-=moveX;
      _data->xmax()-=moveX;
      _data->ymin()-=moveY;
      _data->ymax()-=moveY;
      repaint();
      _lastDragPos=event->pos();
   }
   //setMouseTracking(true);
   QString str;
   if(_paintDistance)
   {
      double x0=inverseTransformX(_distanceOrigin.x());
      double y0=inverseTransformY(_distanceOrigin.y());
      double x1=inverseTransformX(event->pos().x());
      double y1=inverseTransformY(event->pos().y());
      double dist=sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
      _distanceEnd=event->pos();
      repaint();
      str="Distance from ("+QString::number(x0, 'f', 2)+";"+QString::number(y0, 'f', 2)+")"+" to ("+QString::number(x1, 'f', 2)+";"+QString::number(y1, 'f', 2)+")   is   "+QString::number(dist, 'f', 2);
   }else
      str="("+QString::number(inverseTransformX(event->pos().x()), 'f', 2)+";"+QString::number(inverseTransformY(event->pos().y()), 'f', 2)+")";
   _data->qWells()->statusBar()->showMessage(str);
  // qDebug()<<" IIIimamo da je "<<_transform_a<<" "<<_transform_b<<" "<<_transform_c<<" "<<_transform_d<<" stoga "<<event->pos().x()<<" "<<event->pos().y()<<" ide u "<<inverseTransformX(transformX(event->pos().x()))<<" "<<inverseTransformY(transformY(event->pos().y()))<<"\n";
   
}

void PainterWidget::mousePressEvent(QMouseEvent * event)
{
   if (event->button() == Qt::LeftButton)
   {
      if(_paintDistance)
      {
         _distanceT=false;
         _paintDistance=false;
         this->setCursor(Qt::ArrowCursor);
         repaint();
      }else if(_distanceT)
      {
         _paintDistance=true;
         _distanceOrigin = event->pos();
      }else{
         this->setCursor(Qt::ClosedHandCursor);
        _lastDragPos = event->pos();
        _drag=true;
      }
   }
}

void PainterWidget::mouseReleaseEvent(QMouseEvent * event)
{
   if (event->button() == Qt::LeftButton)
   {
      if(!_distanceT)
         this->setCursor(Qt::ArrowCursor);
      _drag=false;
   }
}

void PainterWidget::wheelEvent(QWheelEvent * event)
{

   double zoomInFactor=0.95;
   int numDegrees = event->delta() / 8;
    double numSteps = numDegrees / 15.0f;
    double zoom=pow(zoomInFactor, numSteps);
    double invX=inverseTransformX(event->pos().x());
    double xdiff=invX-_data->xmin();
    _data->xmin()=invX-xdiff*zoom;
    double xMaxDiff=invX-_data->xmax();
    _data->xmax()=invX-xMaxDiff*zoom;
    
    double invY=inverseTransformY(event->pos().y());
    double ydiff=invY-_data->ymin();
    _data->ymin()=invY-ydiff*zoom;
    double yMaxDiff=invY-_data->ymax();
    _data->ymax()=invY-yMaxDiff*zoom;

   repaint();
}

void PainterWidget::setTransform(const double g, const double h)
{
   _transform_a=g/(_data->xmax()-_data->xmin());
   _transform_b=-_transform_a*_data->xmin();
   _transform_c=-h/(_data->ymax()-_data->ymin());
   _transform_d=h-_transform_c*_data->ymin();
}

void PainterWidget::repaint()
{
   update(0,0,width(),height());
}

void PainterWidget::loadImage(const QString & imageName)
{
   _pixmap.load(imageName);
}

void PainterWidget::setShowWellName() 
{
   _showWellNames = true; 
   repaint();
}

void PainterWidget::setHideWellName() 
{ 
   _showWellNames = false; 
   repaint();
}

void PainterWidget::setShowPiezName() 
{ 
   _showPiezNames = true; 
   repaint();
}

void PainterWidget::setHidePiezName() 
{ 
   _showPiezNames = false; 
   repaint();
}

void PainterWidget::browse(/*QString & fileName*/)
{
   QString fileName = QFileDialog::getOpenFileName();
   if(!fileName.isEmpty())
   {
      _data->getModel().setPictureName(fileName);
      _lineEdit->setText(_data->getModel().pictureName());
   } 
   //            tr("Import image"),".",tr("Model Files (*.wjci)"));
     /* if (!fileName.isEmpty()) 
      {
         return fileName;
      }*/
}

void PainterWidget::imageProp()
{
   if(_addDialogIsActive)
   {
      QString err("Close the other dialog first.");
      _data->qWells()->modalWarning(this, err);
      return;
   }

   _addDialogIsActive = true;
   _addDialog = new QDialog(this);
   _addDialog->setWindowTitle("Image properties");
   Qt::WindowFlags flags = windowFlags();
   _addDialog->setWindowFlags(flags | Qt::Tool);
   _addDialog->setWindowTitle("Image properties");
   QVBoxLayout * dialogLayout = new QVBoxLayout(_addDialog);

   QHBoxLayout * inputLayout = new QHBoxLayout;

   QLabel * label = new QLabel("Image file ");
   _lineEdit = new QLineEdit;
   _lineEdit->setText(_data->getModel().pictureName());
   inputLayout->addWidget(label);
   inputLayout->addWidget(_lineEdit);
   QPushButton * browseButton = new QPushButton(tr("Choose"));
   inputLayout->addWidget(browseButton);
   connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
   
   dialogLayout->addLayout(inputLayout);


   QFrame * limitFrame = new QFrame;
   QVBoxLayout * allLimitLayout = new QVBoxLayout;
   QLabel * labelLimits = new QLabel("Limits");
   allLimitLayout->addWidget(labelLimits);

   QGridLayout * limitLayout = new QGridLayout;

   QLabel * labelXmin = new QLabel("x min");
   _lineEditXmin = new QLineEdit;
   _lineEditXmin->setText(QString::number(_data->getModel().picXmin(), 'g',10));
   QLabel * labelXmax = new QLabel("x max");
   _lineEditXmax = new QLineEdit;
   _lineEditXmax->setText(QString::number(_data->getModel().picXmax(), 'g',10));
   QLabel * labelYmin = new QLabel("y min");
   _lineEditYmin = new QLineEdit;
   _lineEditYmin->setText(QString::number(_data->getModel().picYmin(), 'g',10));
   QLabel * labelYmax = new QLabel("y max");
   _lineEditYmax = new QLineEdit;
   _lineEditYmax->setText(QString::number(_data->getModel().picYmax(), 'g',10));
   limitLayout->addWidget(labelXmin, 0, 0);
   limitLayout->addWidget(_lineEditXmin, 0, 1);
   limitLayout->addWidget(labelXmax, 0, 2);
   limitLayout->addWidget(_lineEditXmax, 0, 3);
   limitLayout->addWidget(labelYmin, 1, 0);
   limitLayout->addWidget(_lineEditYmin, 1, 1);
   limitLayout->addWidget(labelYmax, 1, 2);
   limitLayout->addWidget(_lineEditYmax, 1, 3);
   //limitLayout->addWidget(limitFrame);
   allLimitLayout->addLayout(limitLayout);

   limitFrame->setLayout(allLimitLayout);
   limitFrame->setFrameShape(QFrame::StyledPanel);
   
   dialogLayout->addWidget(limitFrame);
   
   _imagePropActive = true;
   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,_addDialog);

   connect(buttonBox,SIGNAL(accepted()),this,SLOT(setImageProp()));
   connect(buttonBox,SIGNAL(rejected()),this,SLOT(closeImagePropDialog()));

   dialogLayout->addWidget(buttonBox);
   _addDialog->setLayout(dialogLayout);

   _addDialog->show();
}

void PainterWidget::closeImagePropDialog()
{
   _addDialogIsActive = false;
   _imagePropActive = false;
   _clicked=0;
   _addDialog->reject();
   delete _lineEdit;
   delete _lineEditXmin;
   delete _lineEditXmax;
   delete _lineEditYmin;
   delete _lineEditYmax;
   delete _addDialog;
   repaint();

}

void PainterWidget::setImageProp()
{
   _addDialogIsActive = false;
   _data->getModel().setPicXmin(_lineEditXmin->text().toDouble());
   _data->getModel().setPicYmin(_lineEditYmin->text().toDouble()); 
   _data->getModel().setPicXmax(_lineEditXmax->text().toDouble());
   _data->getModel().setPicYmax(_lineEditYmax->text().toDouble());
   _data->xmin()=_data->getModel().picXmin();
   _data->xmax()=_data->getModel().picXmax();
   _data->ymin()=_data->getModel().picYmin();
   _data->ymax()=_data->getModel().picYmax();
   _data->setRatio();

   //setRatio(); 
   _data->getModel().setPictureName(_lineEdit->text());
   loadImage(_data->getModel().pictureName());
   emit _data->pictureLoaded();
   closeImagePropDialog();
}

QComboBox * addComboBox(int defColor)
{
   QComboBox * comboBox = new QComboBox;
   comboBox->addItem(QIcon(":resources/images/colors/black.png"), QString("black"));
   comboBox->addItem(QIcon(":resources/images/colors/red.png"), QString("red"));
   comboBox->addItem(QIcon(":resources/images/colors/blue.png"), QString("blue"));
   comboBox->addItem(QIcon(":resources/images/colors/green.png"), QString("green"));
   comboBox->addItem(QIcon(":resources/images/colors/yellow.png"), QString("yellow"));
   comboBox->addItem(QIcon(":resources/images/colors/cyan.png"), QString("cyan"));
   comboBox->addItem(QIcon(":resources/images/colors/magenta.png"), QString("magenta"));
   comboBox->addItem(QIcon(":resources/images/colors/gray.png"), QString("gray"));
   comboBox->setCurrentIndex(defColor);//because default is blue
   for(int i = 0; i < comboBox->count(); i++)
    comboBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
   return comboBox;
}

void PainterWidget::setObjectProp()
{
   QDialog dialog(this);
   dialog.setWindowTitle("Object properties");
   QVBoxLayout * dialogLayout = new QVBoxLayout(&dialog);

   QFrame * wellFrame = new QFrame;
   QLabel * labelWells = new QLabel("Wells");
   QGridLayout * wellLayout = new QGridLayout;
   wellLayout->addWidget(labelWells,0,0);
   QLabel * labelColor = new QLabel("Color");
   wellLayout->addWidget(labelColor,1,0);
   QComboBox * wellColorCombo=addComboBox(_wellColor);
   wellLayout->addWidget(wellColorCombo,1,1);
   QLabel * labelWellXOffset = new QLabel("Name x offset");
   wellLayout->addWidget(labelWellXOffset,2,0);
   QLineEdit * lineEditWellXOffset = new QLineEdit(&dialog);
   wellLayout->addWidget(lineEditWellXOffset,2,1);
   lineEditWellXOffset->setText(QString::number(_wellXOffset));
   QLabel * labelWellYOffset = new QLabel("Name y offset");
   wellLayout->addWidget(labelWellYOffset,3,0);
   QLineEdit * lineEditWellYOffset = new QLineEdit(&dialog);
   wellLayout->addWidget(lineEditWellYOffset,3,1);
   lineEditWellYOffset->setText(QString::number(-_wellYOffset));
   QLabel * labelWellRadius = new QLabel("Radius");
   wellLayout->addWidget(labelWellRadius,4,0);
   QLineEdit * lineEditWellRadius = new QLineEdit(&dialog);
   wellLayout->addWidget(lineEditWellRadius,4,1);
   lineEditWellRadius->setText(QString::number(_wellRadius));
   QLabel * labelWellNames = new QLabel("Show well names");
   wellLayout->addWidget(labelWellNames,5,0);
   QCheckBox * wellNamesLineEdit = new QCheckBox;
   wellLayout->addWidget(wellNamesLineEdit,5,1);
   wellNamesLineEdit->setChecked(_showWellNames);
   wellFrame->setLayout(wellLayout);
   wellFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(wellFrame);


   QFrame * piezFrame = new QFrame;
   QLabel * labelPiez = new QLabel("Piezometers");
   QGridLayout * piezLayout = new QGridLayout;
   piezLayout->addWidget(labelPiez,0,0);
   QLabel * labelColorP = new QLabel("Color");
   piezLayout->addWidget(labelColorP,1,0);
   QComboBox * piezColorCombo=addComboBox(_piezColor);
   piezLayout->addWidget(piezColorCombo,1,1);
   QLabel * labelPiezXOffset = new QLabel("Name x offset");
   piezLayout->addWidget(labelPiezXOffset,2,0);
   QLineEdit * lineEditPiezXOffset = new QLineEdit(&dialog);
   piezLayout->addWidget(lineEditPiezXOffset,2,1);
   lineEditPiezXOffset->setText(QString::number(_piezXOffset));
   QLabel * labelPiezYOffset = new QLabel("Name y offset");
   piezLayout->addWidget(labelPiezYOffset,3,0);
   QLineEdit * lineEditPiezYOffset = new QLineEdit(&dialog);
   piezLayout->addWidget(lineEditPiezYOffset,3,1);
   lineEditPiezYOffset->setText(QString::number(-_piezYOffset));
   QLabel * labelPiezRadius = new QLabel("Radius");
   piezLayout->addWidget(labelPiezRadius,4,0);
   QLineEdit * lineEditPiezRadius = new QLineEdit(&dialog);
   piezLayout->addWidget(lineEditPiezRadius,4,1);
   lineEditPiezRadius->setText(QString::number(_piezRadius));
   QLabel * labelPiezNames = new QLabel("Show piezometer names");
   piezLayout->addWidget(labelPiezNames,5,0);
   QCheckBox * piezNamesLineEdit = new QCheckBox;
   piezLayout->addWidget(piezNamesLineEdit,5,1);
   piezNamesLineEdit->setChecked(_showPiezNames);
   piezFrame->setLayout(piezLayout);
   piezFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(piezFrame);
   
   QFrame * boundFrame = new QFrame;
   QLabel * labelBound = new QLabel("Boundary lines");
   QGridLayout * boundLayout = new QGridLayout;
   boundLayout->addWidget(labelBound,0,0);
   QLabel * labelColorConst = new QLabel("Constant potential");
   boundLayout->addWidget(labelColorConst,1,0);
   QComboBox * constColorCombo = addComboBox(_constColor);
   boundLayout->addWidget(constColorCombo,1,1);
   QLabel * labelColorImper = new QLabel("Impermeable");
   boundLayout->addWidget(labelColorImper,2,0);
   QComboBox * impColorCombo = addComboBox(_impermColor);
   boundLayout->addWidget(impColorCombo,2,1);
   boundFrame->setLayout(boundLayout);
   boundFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(boundFrame);

   QHBoxLayout * buttonBoxLayout = new QHBoxLayout;

   QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
         Qt::Horizontal, &dialog);
   buttonBoxLayout->addWidget(&buttonBox);
   dialogLayout->addLayout(buttonBoxLayout);
   //dialog.setLayout(dialogLayout);
//   form.addRow(&buttonBox);
   QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
   QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

   // Show the dialog as modal
   if (dialog.exec() == QDialog::Accepted) {
      _wellXOffset=lineEditWellXOffset->text().toInt(); 
      _wellYOffset=-lineEditWellYOffset->text().toInt();
      _piezXOffset=lineEditPiezXOffset->text().toInt(); 
      _piezYOffset=-lineEditPiezYOffset->text().toInt();
      _wellRadius=lineEditWellRadius->text().toInt();
      _piezRadius=lineEditPiezRadius->text().toInt();
      _wellColor=wellColorCombo->currentIndex();
      _piezColor=piezColorCombo->currentIndex();
      _constColor=constColorCombo->currentIndex();
      _impermColor=impColorCombo->currentIndex();
      _showWellNames=wellNamesLineEdit->isChecked();
      _showPiezNames=piezNamesLineEdit->isChecked();
      _data->getModel().setParamObjProp(_wellXOffset, _wellYOffset,
            _wellRadius, _wellColor, _showWellNames, _piezXOffset, _piezYOffset,
            _piezRadius, _piezColor, _showPiezNames, _constColor, _impermColor);
      repaint();
   }
}

void PainterWidget::savePic()
{
   QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save image"),".",tr("JPG Files (*.jpg)"));

   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");

   _image->save(fileName);
}

void PainterWidget::closeAddDialog()
{
   _addDialogIsActive = false;
   _boundaryDialogIsActive = false;
   _clicked=0;
   _addDialog->reject();
   delete _nameLineEdit;
   delete _xLineEdit;
   delete _yLineEdit;
   delete _rLineEdit;
   delete _addDialog;
}

void PainterWidget::closeAddPiezDialog()
{
   _addDialogIsActive = false;
   _boundaryDialogIsActive = false;
   _clicked=0;
   _addDialog->reject();
   delete _nameLineEdit;
   delete _xLineEdit;
   delete _yLineEdit;
   delete _addDialog;
}

void PainterWidget::closeAddBDialog()
{
   _addDialogIsActive = false;
   _boundaryDialogIsActive = false;
   _clicked=0;
   _addDialog->reject();
   for (size_t i=0; i< _xLineEditB.size();i++)
   {
     delete (_xLineEditB[i]);
   } 
   _xLineEditB.clear();
   for (size_t i=0; i< _yLineEditB.size();i++)
   {
     delete (_yLineEditB[i]);
   } 
   _yLineEditB.clear();
   delete _addDialog;
}

void PainterWidget::addWell()
{
   if(_addDialogIsActive)
   {
      QString err("Close the other dialog first.");
      _data->qWells()->modalWarning(_addDialog, err);
      return;
   }

   _addDialogIsActive = true;
   _addDialog = new QDialog;
   _addDialog->setWindowTitle("Add well");
   Qt::WindowFlags flags = windowFlags();
   _addDialog->setWindowFlags(flags | Qt::WindowStaysOnTopHint);

   QGridLayout * layout = new QGridLayout;

   QLabel * nameLabel = new QLabel("Name:");
   layout->addWidget(nameLabel,0,0,1,1);
   _nameLineEdit = new QLineEdit(_addDialog);
   layout->addWidget(_nameLineEdit,1,0,1,1);
   
   QLabel * xLabel = new QLabel("X:");
   layout->addWidget(xLabel,0,1,1,1);
   _xLineEdit = new QLineEdit(_addDialog);
   layout->addWidget(_xLineEdit,1,1,1,1);
   
   QLabel * yLabel = new QLabel("Y:");
   layout->addWidget(yLabel,0,2,1,1);
   _yLineEdit = new QLineEdit(_addDialog);
   layout->addWidget(_yLineEdit,1,2,1,1);

   QLabel * rLabel = new QLabel("r:");
   layout->addWidget(rLabel,0,3,1,1);
   _rLineEdit = new QLineEdit(_addDialog);
   layout->addWidget(_rLineEdit,1,3,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,_addDialog);
   connect(buttonBox,SIGNAL(accepted()),this,SLOT(submitWell()));
   connect(buttonBox,SIGNAL(rejected()),this,SLOT(closeAddDialog()));

   layout->addWidget(buttonBox,2,1,1,3,Qt::AlignCenter);
   _addDialog->setLayout(layout);

   _addDialog->show();
}

void PainterWidget::submitWell()
{
   if ((_rLineEdit->text().isEmpty()) || (_xLineEdit->text().isEmpty()) 
         || (_yLineEdit->text().isEmpty())) return;
   double r,x,y;
   const bool ok = conversion::convertQStringToDouble(_rLineEdit->text(),r)
                && conversion::convertQStringToDouble(_xLineEdit->text(),x)
                && conversion::convertQStringToDouble(_yLineEdit->text(),y);
   if ((ok) && (r > 0)) 
   {
      _wellsTable->addFromPicture(_nameLineEdit->text(),x,y,r);
      _rLineEdit->setStyleSheet("color:black;");
      //_addDialog->accept();
      closeAddDialog();
   }
   else if (r == 0) _rLineEdit->setStyleSheet("color:red;");
   else assert(false);
}

void PainterWidget::addPiez()
{
   if(_addDialogIsActive)
   {
      QString err("Close the other dialog first.");
      _data->qWells()->modalWarning(_addDialog, err);
      return;
   }
   _addDialogIsActive = true;
   _addDialog = new QDialog;
   _addDialog->setWindowTitle("Add piezometer");
   Qt::WindowFlags flags = windowFlags();
   _addDialog->setWindowFlags(flags | Qt::WindowStaysOnTopHint);

   QGridLayout * layout = new QGridLayout;

   QLabel * nameLabel = new QLabel("Name:");
   layout->addWidget(nameLabel,0,0,1,1);
   _nameLineEdit = new QLineEdit(_addDialog);
   //yminLineEdit->setText(QString::number(_ymin));
   layout->addWidget(_nameLineEdit,1,0,1,1);
   
   QLabel * xLabel = new QLabel("X:");
   layout->addWidget(xLabel,0,1,1,1);
   _xLineEdit = new QLineEdit(_addDialog);
   layout->addWidget(_xLineEdit,1,1,1,1);
   
   QLabel * yLabel = new QLabel("Y:");
   layout->addWidget(yLabel,0,2,1,1);
   _yLineEdit = new QLineEdit(_addDialog);
   layout->addWidget(_yLineEdit,1,2,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,_addDialog);
   connect(buttonBox,SIGNAL(accepted()),this,SLOT(submitPiez()));
   connect(buttonBox,SIGNAL(rejected()),this,SLOT(closeAddPiezDialog()));

   layout->addWidget(buttonBox,2,1,1,3,Qt::AlignCenter);
   _addDialog->setLayout(layout);

   _addDialog->show();
}

void PainterWidget::submitPiez()
{
   if ((_xLineEdit->text().isEmpty()) || (_yLineEdit->text().isEmpty()))
      return;
   double x,y;
   const bool xok = conversion::convertQStringToDouble(_xLineEdit->text(),x);
   const bool yok = conversion::convertQStringToDouble(_yLineEdit->text(),y);
   const bool ok = xok && yok;
   if (ok) 
   {
      _piezoTable->addFromPicture(_nameLineEdit->text(),x,y);
      closeAddPiezDialog();
   }
   else assert(false);
}

void PainterWidget::setFocusBoundary(int row)
{
   _clicked=row;
}

void PainterWidget::setBound()
{
   size_t type = _data->getBoundary().type();
   if(_addDialogIsActive)
   {
      QString err("Close the other dialog first.");
      _data->qWells()->modalWarning(this, err);
      return;
   }
   if(type==std::numeric_limits<size_t>::max())
   {
      QString err("Please set boundary type first.");
      _data->qWells()->modalWarning(this, err);
      return;
   }
   if(type==0)
   {
      QString err("For boundary type - no boundary, there is no lines.");
      _data->qWells()->modalWarning(this, err);
      return;
   }
   if(_data->getBoundary().refWell() == std::numeric_limits<size_t>::max())
   {
      QString err("Please set refferent well first.");
      _data->qWells()->modalWarning(this, err);
      return;
   }

   _addDialogIsActive = true;
   _boundaryDialogIsActive = true;
   
   size_t limit=type>2 ? type : type+1;
   //because 1 line has 2 points, parallel lines - 3 points, angle - 3 points, 
   //three lines 4 points, four lines 5 points
   _xLineEditB.resize(limit);
   _yLineEditB.resize(limit);

   _addDialog = new QDialog;
   _addDialog->setWindowTitle("Set boundary");
   Qt::WindowFlags flags = windowFlags();
   _addDialog->setWindowFlags(flags | Qt::WindowStaysOnTopHint);

   QVBoxLayout * wholeLayout = new QVBoxLayout;

   QFrame * frame = new QFrame;  
   QGridLayout * layout = new QGridLayout;
   QLabel * LineLabel = new QLabel("Line "+QString::number(1));
   layout->addWidget(LineLabel,0,0,1,1);
   QLabel * XLabel = new QLabel("X");
   layout->addWidget(XLabel,1,1,1,1);
   QLabel * YLabel = new QLabel("Y");
   layout->addWidget(YLabel,1,2,1,1);

   QLabel * PointLabel = new QLabel("Point 1");
   layout->addWidget(PointLabel,2,0,1,1);
   _xLineEditB[0] = new LineEditSetBoundary(0,_addDialog);
   layout->addWidget(_xLineEditB[0],2,1,1,1);
   _yLineEditB[0] = new LineEditSetBoundary(0,_addDialog);
   layout->addWidget(_yLineEditB[0],2,2,1,1);
   connect(_xLineEditB[0], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);
   connect(_yLineEditB[0], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);

   QLabel * PointLabel2 = new QLabel("Point 2");
   layout->addWidget(PointLabel2,3,0,1,1);
   _xLineEditB[1] = new LineEditSetBoundary(1, _addDialog);
   layout->addWidget(_xLineEditB[1],3,1,1,1);
   _yLineEditB[1] = new LineEditSetBoundary(1, _addDialog);
   layout->addWidget(_yLineEditB[1],3,2,1,1);
   connect(_xLineEditB[1], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);
   connect(_yLineEditB[1], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);
   //layout->addWidget(buttonBox,3,1,1,2,Qt::AlignCenter);
   frame->setLayout(layout);
   frame->setFrameShape(QFrame::StyledPanel);
   frame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
   wholeLayout->addWidget(frame);
   //delete frame;


   if(type==3)
   {
      /* QString err("Type 3 boundary can not be set through picture.");
      _data->qWells()->modalWarning(_addDialog, err);
      _addDialogIsActive = false;
      _boundaryDialogIsActive = false;
      return;*/
      QFrame * frameA = new QFrame;
      layout = new QGridLayout;
      LineLabel = new QLabel("Line "+QString::number(2));
      layout->addWidget(LineLabel,0,0,1,1);
      XLabel = new QLabel("X");
      layout->addWidget(XLabel,1,1,1,1);
      YLabel = new QLabel("Y");
      layout->addWidget(YLabel,1,2,1,1);

      PointLabel = new QLabel("Point 1");
      layout->addWidget(PointLabel,2,0,1,1);
      _xLineEditB[2] = new LineEditSetBoundary(2, _addDialog);
      layout->addWidget(_xLineEditB[2],2,1,1,1);
      _yLineEditB[2] = new LineEditSetBoundary(2, _addDialog);
      layout->addWidget(_yLineEditB[2],2,2,1,1);
      connect(_xLineEditB[2], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);
      connect(_yLineEditB[2], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);

      frameA->setLayout(layout);
      frameA->setFrameShape(QFrame::StyledPanel);
      frameA->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
      wholeLayout->addWidget(frameA);
      //delete frameA;

   }else if(type!=1){
      for(size_t i=2;i<limit;i++)
      {
         QFrame * frameI = new QFrame;
         layout = new QGridLayout;
         LineLabel = new QLabel("Line "+QString::number(i));
         layout->addWidget(LineLabel,0,0,1,1);
         XLabel = new QLabel("X");
         layout->addWidget(XLabel,1,1,1,1);
         YLabel = new QLabel("Y");
         layout->addWidget(YLabel,1,2,1,1);

         PointLabel = new QLabel("Point");
         layout->addWidget(PointLabel,2,0,1,1);
         _xLineEditB[i] = new LineEditSetBoundary(i, _addDialog);
         layout->addWidget(_xLineEditB[i],2,1,1,1);
         _yLineEditB[i] = new LineEditSetBoundary(i, _addDialog);
         layout->addWidget(_yLineEditB[i],2,2,1,1);
         connect(_xLineEditB[i], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);
   connect(_yLineEditB[i], &LineEditSetBoundary::boundInFocus, this, 
         &PainterWidget::setFocusBoundary);

         frameI->setLayout(layout);
         frameI->setFrameShape(QFrame::StyledPanel);
         frameI->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
         wholeLayout->addWidget(frameI);
         //delete frameI;
      }
   }

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,_addDialog);
   connect(buttonBox,SIGNAL(accepted()),this,SLOT(submitBound()));
   connect(buttonBox,SIGNAL(rejected()),this,SLOT(closeAddBDialog()));

   wholeLayout->addWidget(buttonBox,Qt::AlignCenter);
   _addDialog->setLayout(wholeLayout);

   _addDialog->show();
}

double PainterWidget::distance(const double x1, const double y1, 
      const double x2, const double y2, const double refX, const double refY) const
{
   //p = -b*x + a*y + c1 line x1x2
   //q = a*x + b*y + c2 line perpendicular to x1x2
   double a = x2-x1;
   double b = y2-y1;
   double c1 = x1*y2 - y1*x2;
   double c2 = -a*refX -b*refY;
   double x = (-c2*a+c1*b)/(a*a+b*b);
   double y = (-c1*a-c2*b)/(a*a+b*b);
   double divisor = sqrt(a*a+b*b);
   if(fabs(divisor)<1e-12)
   {
      QString err("Point 1 and Point two are same.");
      _data->qWells()->modalWarning(_addDialog, err);
      return std::numeric_limits<double>::max();
   }
   double distance = fabs((x2-x1)*(y1-refY) - (x1-refX)*(y2-y1))/divisor;
   if(fabs(refX-x)<1e-12)
      return (refY>y) ? distance : -distance;
      
   return (refX>x)? -distance : distance;
}

double PainterWidget::distancePoint(const double a, const double b, 
      const double x, const double y, const double refX, const double refY) const
{//line p: ax+by+c
   double c = -a*x -b*y; 
   //line normal to a line p is q=-bx+ay+c1
   double c1 = b*refX - a*refY;
   double x_res=(-a*c+b*c1)/(a*a+b*b);
   double y_res=(-a*c1-b*c)/(a*a+b*b);
   double distance = fabs(a*refX + b*refY +c)/sqrt(a*a+b*b);
   if(fabs(refX-x_res)<1e-12)
      return (refY>y_res) ? distance : -distance;
      
   return (refX>x_res)? -distance : distance;
}

void PainterWidget::submitBound()
{
   size_t type = _data->getBoundary().type();

   double refX = _data->wells()[_data->getBoundary().refWell()].pos()._x;
   double refY = _data->wells()[_data->getBoundary().refWell()].pos()._y;
   double x1 = _xLineEditB[0]->text().toDouble();
   double y1 = _yLineEditB[0]->text().toDouble();
   double x2 = _xLineEditB[1]->text().toDouble();
   double y2 = _yLineEditB[1]->text().toDouble();
   double d0=distance(x1,y1,x2,y2,refX, refY);
   if(d0==std::numeric_limits<double>::max())
      return;//this means that the line is not calculated properly point1=point2
   _boundaryWidget->boundary()->distance(0)->enterData(d0);
   _boundaryWidget->boundary()->distance(0)->verifyAndSubmitData();
   double d1=0.0;
   double d2;
   double d3;
   double beta;
   double angle;
   if(fabs(x1-x2)<1e-12)
      angle=pi/2;
   else
      angle=atan((y2-y1)/(x2-x1));
   if(angle<0)
   {
      angle= pi+angle;
   }
   angle*=180/pi;
   _boundaryWidget->boundary()->alpha()->enterData(angle);
   _boundaryWidget->boundary()->alpha()->verifyAndSubmitData();

   if(type==3)
   {
      double x3 = _xLineEditB[2]->text().toDouble();
      double y3 = _yLineEditB[2]->text().toDouble();
      d1 = distance(x2,y2,x3,y3,refX,refY);
      beta=acos(((x1-x2)*(x3-x2)+(y1-y2)*(y3-y2))/(sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))*sqrt((x3-x2)*(x3-x2)+(y3-y2)*(y3-y2))));
      beta*=180/pi;
      if(fmod(360.0,beta)!=0.0)
      {
         QString err("360 is not divisible with determined beta."+ QString(QChar(0x03B2)).append(" = ")+QString::number(beta));
         _data->qWells()->modalWarning(_addDialog, err);
         return;
      }
      _boundaryWidget->boundary()->beta()->enterData(beta);
      _boundaryWidget->boundary()->beta()->verifyAndSubmitData();
   }
   if(type>=2 && type!=3)
   {
      double x3 = _xLineEditB[2]->text().toDouble();
      double y3 = _yLineEditB[2]->text().toDouble();

      d1 = distancePoint(y1-y2, x2-x1, x3, y3, refX, refY);
   }
   if(type!=1)
   {
      _boundaryWidget->boundary()->distance(1)->enterData(d1);
      _boundaryWidget->boundary()->distance(1)->verifyAndSubmitData();
   }
   if(type>=4)
   {
      double x4 = _xLineEditB[3]->text().toDouble();
      double y4 = _yLineEditB[3]->text().toDouble();
      d2 = distancePoint(x2-x1, y2-y1, x4, y4, refX, refY);
      _boundaryWidget->boundary()->distance(2)->enterData(d2);
      _boundaryWidget->boundary()->distance(2)->verifyAndSubmitData();
   }
   if(type==5)
   {
      double x5 = _xLineEditB[4]->text().toDouble();
      double y5 = _yLineEditB[4]->text().toDouble();
      d3 = distancePoint(x2-x1, y2-y1, x5, y5, refX, refY);
      _boundaryWidget->boundary()->distance(3)->enterData(d3);
      _boundaryWidget->boundary()->distance(3)->verifyAndSubmitData();
   }   
   closeAddBDialog();
}

void PainterWidget::setRange()
{
   QDialog * dialog = new QDialog;
   dialog->setWindowTitle("Set range");
   
   QGridLayout * layout = new QGridLayout;
   
   QLabel * xminLabel = new QLabel("Xmin:");
   layout->addWidget(xminLabel,0,0,1,1);
   QLineEdit * xminLineEdit = new QLineEdit(dialog);
   xminLineEdit->setText(QString::number(_data->xmin(), 'g', 10));
   layout->addWidget(xminLineEdit,0,1,1,1);
   
   QLabel * xmaxLabel = new QLabel("Xmax:");
   layout->addWidget(xmaxLabel,0,2,1,1);
   QLineEdit * xmaxLineEdit = new QLineEdit(dialog);
   xmaxLineEdit->setText(QString::number(_data->xmax(), 'g', 10));
   layout->addWidget(xmaxLineEdit,0,3,1,1);

   QLabel * yminLabel = new QLabel("Ymin:");
   layout->addWidget(yminLabel,1,0,1,1);
   QLineEdit * yminLineEdit = new QLineEdit(dialog);
   yminLineEdit->setText(QString::number(_data->ymin(), 'g', 10));
   layout->addWidget(yminLineEdit,1,1,1,1);

   QLabel * ymaxLabel = new QLabel("Ymax:");
   layout->addWidget(ymaxLabel,1,2,1,1);
   QLineEdit * ymaxLineEdit = new QLineEdit(dialog);
   ymaxLineEdit->setText(QString::number(_data->ymax(), 'g', 10));
   layout->addWidget(ymaxLineEdit,1,3,1,1);

   QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |QDialogButtonBox::Cancel,Qt::Horizontal,dialog);
   connect(buttonBox,SIGNAL(accepted()),dialog,SLOT(accept()));
   connect(buttonBox,SIGNAL(rejected()),dialog,SLOT(reject()));

   layout->addWidget(buttonBox,2,0,1,3,Qt::AlignCenter);
   dialog->setLayout(layout);
   if(dialog->exec() == QDialog::Accepted)
   {
      _data->xmin()=xminLineEdit->text().toDouble();
      _data->xmax()=xmaxLineEdit->text().toDouble();
      _data->ymin()=yminLineEdit->text().toDouble();
      _data->ymax()=ymaxLineEdit->text().toDouble();
      _data->setRatio();
      repaint();
   }
}

void PainterWidget::distanceTool()
{
   _distanceT=true;
   this->setCursor(Qt::CrossCursor);
}

void PainterWidget::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Image properties", this, SLOT(imageProp()));
   menu->addAction("Save image", this, SLOT(savePic()));
   menu->addAction("Add well", this, SLOT(addWell()));
   menu->addAction("Add piezometer", this, SLOT(addPiez()));
   menu->addAction("Set boundary",this, SLOT(setBound()));
  /* if(showWellNames())
      menu->addAction("Hide well names", this, SLOT(setHideWellName()));
   else
      menu->addAction("Show well names", this, SLOT(setShowWellName()));
   if(showPiezNames())
      menu->addAction("Hide piez names", this, SLOT(setHidePiezName()));
   else
      menu->addAction("Show piez names", this, SLOT(setShowPiezName()));*/
   menu->addAction("Objects properties", this, SLOT(setObjectProp()));
   menu->addAction("Set range",this, SLOT(setRange()));
   menu->addAction("Distance tool", this, SLOT(distanceTool()));

/*   menu->addSeparator();
   menu->addAction("Plot settings", _data->qWells()->plotSettings(),
         SLOT(startDialog()));*/
   menu->popup(mapToGlobal(pos));
}

void PainterWidget::paint(QPainter *painter, QPaintEvent * event)
{//this function is used to paint everything
    painter->setRenderHint(QPainter::Antialiasing, true);

   
    QPen pen;
   
    //QPen pen(QPen(Qt::red));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    painter->setPen(pen);


    double sirina;
    double visina;
    if(ceil(width()/_data->ratio())<height())
    {
       sirina=width();
       visina=ceil(sirina/_data->ratio());
    }else{
       visina=height();
       sirina=floor(visina*_data->ratio());
    }

    setTransform(sirina, visina);
    painter->drawPixmap(transformX(_data->getModel().picXmin()),
          transformY(_data->getModel().picYmax()), 
transformX(_data->getModel().picXmax())-transformX(_data->getModel().picXmin()),
transformY(_data->getModel().picYmin())-transformY(_data->getModel().picYmax()),
_pixmap);
    
    setGeometry(5,5,sirina,visina);

    if(_paintDistance)
    {
       painter->setPen(QPen(Qt::black,1,Qt::DashLine));
       painter->drawLine(_distanceOrigin, _distanceEnd);
       double x0=inverseTransformX(_distanceOrigin.x());
       double y0=inverseTransformY(_distanceOrigin.y());
       double x1=inverseTransformX(_distanceEnd.x());
       double y1=inverseTransformY(_distanceEnd.y());
       double dist=sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
       painter->drawText((_distanceOrigin.x()+_distanceEnd.x())/2,
             (_distanceOrigin.y()+_distanceEnd.y())/2,
             QString::number(dist, 'f', 2));
    }
    
    double razmera = powf(10.0f, floorf(log10f(inverseTransformX(90)-inverseTransformX(0))));
    QString unitString;
    if(razmera>1000)
    {
       unitString=QString::number(razmera/1000)+"km";
    }else if(razmera<0.01)
    {
       unitString=QString::number(razmera*100)+"cm";
    }else
       unitString=QString::number(razmera)+"m";

    double trans = transformX(razmera)-transformX(0);
    painter->drawLine(sirina-65-trans, visina-10, sirina-65, visina-10);
    painter->drawLine(sirina-65-trans, visina-8, sirina-65-trans, visina-12);
    painter->drawLine(sirina-65, visina-8, sirina-65, visina-12);
    painter->drawText(sirina-62, visina-10, unitString);

    pen.setColor(Qt::GlobalColor(_color[_wellColor]));
    painter->setPen(pen);
    for(int i=0;i<_data->noOfWells();i++)
    {
       double xc=transformX(_data->wells()[i].pos()._x);
       double yc=transformY(_data->wells()[i].pos()._y);
       QPointF center(xc, yc);
       painter->drawEllipse(center, _wellRadius, _wellRadius);
       if(_showWellNames)       
          painter->drawText(xc+_wellXOffset, yc+_wellYOffset, 
                _data->wells()[i].name());
    }

    pen.setColor(Qt::GlobalColor(_color[_piezColor]));
    painter->setPen(pen);

    for(int i=0;i<_data->noOfPiezometers();i++)
    {
       double xc=transformX(_data->piezometers()[i].pos()._x);
       double yc=transformY(_data->piezometers()[i].pos()._y);
       QPointF center(xc, yc);
       painter->drawEllipse(center, _piezRadius, _piezRadius);
       if(_showPiezNames)
          painter->drawText(xc+_piezXOffset, yc+_piezYOffset, _data->piezometers()[i].name());
    } 

    if(_data->getBoundary().alpha()!=std::numeric_limits<double>::max() &&
          _data->getBoundary().refWell()!=std::numeric_limits<size_t>::max())
    {
       for(size_t i=0;i<_data->getBoundary().nlines();i++)
       {
          if(!_data->getBoundary().lineType(i))
          {
              pen.setColor(Qt::GlobalColor(_color[_constColor]));
          }else
              pen.setColor(Qt::GlobalColor(_color[_impermColor]));
          painter->setPen(pen);
          double angle = _data->getBoundary().alpha();
          if ((_data->getBoundary().type()==3) && (i != 0))
          {
             if(_data->getBoundary().beta()==std::numeric_limits<double>::max())
                break;
             angle+=_data->getBoundary().beta();
          }
          if(_data->getBoundary().type()==4 && i==2)
             angle+=pi/2;
          if(_data->getBoundary().type()==5 && (i==2 || i==3) )
             angle+=pi/2;
          Point2 refWellPos=_data->wells()[_data->getBoundary().refWell()].pos();

          Line line;   
          line.constructLine(angle, _data->getBoundary().rho(i), refWellPos);
          if(fabs(line.b())<1e-12)
          {
             painter->drawLine(transformX((-line.b()*_data->ymin()-line.c())/
                      line.a()), visina, 
                   transformX((-line.b()*_data->ymax()-line.c())/line.a()), 0);
          }else if(fabs(line.a())<1e-12)
          {
             painter->drawLine(0, 
               transformY((-line.a()*_data->xmin()-line.c())/line.b()), sirina,
               transformY((-line.a()*_data->xmax()-line.c())/line.b()));
          }else
          {
             double x0=transformX((-line.b()*_data->ymin()-line.c())/line.a());
             double y0=visina;
             double y1=0;
             double x1=transformX((-line.b()*_data->ymax()-line.c())/line.a());
             if(x0<=0.0)
             {
                x0=0.0;
                y0=transformY((-line.a()*_data->xmin()-line.c())/line.b());
             }
             else if(x1>=sirina)
             {
                x1=sirina;
                y1=transformY((-line.a()*_data->xmax()-line.c())/line.b());
             }
             painter->drawLine(x0,y0,x1,y1);
          }
       }
    }

    pen.setColor(Qt::magenta);
    painter->setPen(pen);

    if(_boundaryDialogIsActive && _xLineEditB.size()!=0 && 
          _data->getBoundary().refWell()!=std::numeric_limits<size_t>::max() &&
          _data->getBoundary().type()!=0)
    {
       if(_data->getBoundary().type()==3)
       {
          if(!_xLineEditB[0]->text().isEmpty() && 
                !_yLineEditB[0]->text().isEmpty())
          {
             painter->drawEllipse(
                   transformX(_xLineEditB[0]->text().toDouble()),
                   transformY(_yLineEditB[0]->text().toDouble()),1,1);
          }
          if(!_xLineEditB[0]->text().isEmpty() && 
                !_yLineEditB[0]->text().isEmpty() && 
                !_xLineEditB[1]->text().isEmpty() && 
                !_yLineEditB[1]->text().isEmpty())
          {

             painter->drawLine(
                   transformX(_xLineEditB[0]->text().toDouble()),
                   transformY(_yLineEditB[0]->text().toDouble()), 
                   transformX(_xLineEditB[1]->text().toDouble()),
                   transformY(_yLineEditB[1]->text().toDouble()));
          }
          if(!_xLineEditB[1]->text().isEmpty() && 
                !_yLineEditB[1]->text().isEmpty() && 
                !_xLineEditB[2]->text().isEmpty() && 
                !_yLineEditB[2]->text().isEmpty())
          {
             painter->drawLine(
                   transformX(_xLineEditB[1]->text().toDouble()),
                   transformY(_yLineEditB[1]->text().toDouble()), 
                   transformX(_xLineEditB[2]->text().toDouble()),
                   transformY(_yLineEditB[2]->text().toDouble()));
          }
       }else{
          if(!_xLineEditB[0]->text().isEmpty() && 
                !_yLineEditB[0]->text().isEmpty())
          {
             painter->drawEllipse(
                   transformX(_xLineEditB[0]->text().toDouble()),
                   transformY(_yLineEditB[0]->text().toDouble()),1,1);
          }
          if(!_xLineEditB[0]->text().isEmpty() && 
                !_yLineEditB[0]->text().isEmpty() && 
                !_xLineEditB[1]->text().isEmpty() && 
                !_yLineEditB[1]->text().isEmpty())
          {
             double vecx=transformX(_xLineEditB[1]->text().toDouble())-
                   transformX(_xLineEditB[0]->text().toDouble());
             double vecy=transformY(_yLineEditB[1]->text().toDouble())-
                   transformY(_yLineEditB[0]->text().toDouble());
             painter->drawLine(
                   transformX(_xLineEditB[0]->text().toDouble())-100*vecx,
                   transformY(_yLineEditB[0]->text().toDouble())-100*vecy, 
                   transformX(_xLineEditB[0]->text().toDouble())+100*vecx,
                   transformY(_yLineEditB[0]->text().toDouble())+100*vecy);
             if(_data->getBoundary().type()>1 && 
                   !_xLineEditB[2]->text().isEmpty() && 
                   !_yLineEditB[2]->text().isEmpty())
             {
                
                painter->drawLine(
                      transformX(_xLineEditB[2]->text().toDouble())-100*vecx,
                      transformY(_yLineEditB[2]->text().toDouble())-100*vecy, 
                      transformX(_xLineEditB[2]->text().toDouble())+100*vecx,
                      transformY(_yLineEditB[2]->text().toDouble())+100*vecy);
             }if(_data->getBoundary().type()>3 && 
                   !_xLineEditB[3]->text().isEmpty() && 
                   !_yLineEditB[3]->text().isEmpty())
             {//
                painter->drawLine(
                      transformX(_xLineEditB[3]->text().toDouble())+100*vecy,
                      transformY(_yLineEditB[3]->text().toDouble())-100*vecx, 
                      transformX(_xLineEditB[3]->text().toDouble())-100*vecy,
                      transformY(_yLineEditB[3]->text().toDouble())+100*vecx);
             }if(_data->getBoundary().type()>4 && 
                   !_xLineEditB[4]->text().isEmpty() && 
                   !_yLineEditB[4]->text().isEmpty())
             {//
                painter->drawLine(
                      transformX(_xLineEditB[4]->text().toDouble())+100*vecy,
                      transformY(_yLineEditB[4]->text().toDouble())-100*vecx, 
                      transformX(_xLineEditB[4]->text().toDouble())-100*vecy,
                      transformY(_yLineEditB[4]->text().toDouble())+100*vecx);
             }
          }
         /* if(_data->getBoundary().type()>1)
          {
             double vecx=transformX(_xLineEditB[1]->text().toDouble())-
                transformX(_xLineEditB[0]->text().toDouble());
             double vecy=transformY(_yLineEditB[1]->text().toDouble())-
                transformY(_yLineEditB[0]->text().toDouble());
             painter.drawLine(
                   transformX(_xLineEditB[0]->text().toDouble()),
                   transformY(_yLineEditB[0]->text().toDouble()), 
                   transformX(_xLineEditB[0]->text().toDouble())+vecx,
                   transformY(_yLineEditB[0]->text().toDouble())+vecy);
          }*/
       }
       
    }

   // for(size_t )
    
    painter->setPen(palette().dark().color());
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRect(0, 0, width() - 1, height() - 1));
    
    QPainter painterW(this);
    QRect dirtyRect = event->rect();
    painterW.drawImage(dirtyRect, *_image, dirtyRect);      
}

void PainterWidget::paintEvent(QPaintEvent * event)
{
    QSize newSize(width(),height());
    delete _image;
    _image=new QImage(newSize, QImage::Format_RGB32);
    _image->fill(qRgb(255, 255, 255));
    QPainter painter(_image);    
    paint(&painter, event);
}


