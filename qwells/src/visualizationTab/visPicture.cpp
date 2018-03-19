#include "visPicture.h"
#include "engine/data.h"
#include "qWells.h"
#include "visualizationTab.h"
#include "visData.h"
#include "visResult.h"
#include "linesTabs/isoDataTab.h"
#include "linesTabs/tabs.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QStatusBar>
#include <QLabel>
#include <QMenu>
#include <QFileDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QFrame>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>


VisPicture::VisPicture(Data * data, VisualizationTab * visualizationTab):_data(data), _visualizationTab(visualizationTab) 
{  
   setMouseTracking(true);
   _color = {Qt::black, Qt::red, Qt::blue, Qt::green, 
      Qt::yellow, Qt::cyan, Qt::magenta, Qt::gray};
   setContextMenuPolicy(Qt::CustomContextMenu);
   QSize newSize(width(),height());
   _image=new QImage(newSize, QImage::Format_RGB32);

   _data->xmin()=_data->getModel().visXmin();
   _data->xmax()=_data->getModel().visXmax();
   _data->ymin()=_data->getModel().visYmin();
   _data->ymax()=_data->getModel().visYmax();
   _data->setRatio();

   _pixmap.load(_data->getModel().pictureName());

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
   _streamColor = _data->getModel().streamLineColor();
   _isoLabelNumber = _data->getModel().isoLabelIncidence();
   _playDuration = _data->getModel().isoPlayDuration();
   _isoColor = _data->getModel().isoColor();

   connect(this, SIGNAL(customContextMenuRequested(QPoint)), 
            this, SLOT(contextMenuRequest(QPoint)));
   connect(_data, SIGNAL(pictureLoaded()), this, SLOT(loadPicture()));
}

void VisPicture::mouseMoveEvent(QMouseEvent * event)
{
   double x1=inverseTransformX(event->pos().x());
   double y1=inverseTransformY(event->pos().y());
   if(_drag)
   {
      double moveX=x1-inverseTransformX(_lastDragPos.x());
      double moveY=y1-inverseTransformY(_lastDragPos.y());
      _data->xmin()-=moveX;
      _data->xmax()-=moveX;
      _data->ymin()-=moveY;
      _data->ymax()-=moveY;
      repaint();
      _lastDragPos=event->pos();
   }
   QString str;
   if(_paintDistance)
   {
      double x0=inverseTransformX(_distanceOrigin.x());
      double y0=inverseTransformY(_distanceOrigin.y());
      double dist=sqrt((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1));
      _distanceEnd=event->pos();
      repaint();
      str="Distance from ("+QString::number(x0, 'f', 2)+";"+QString::number(y0, 'f', 2)+")"+" to ("+QString::number(x1, 'f', 2)+";"+QString::number(y1, 'f', 2)+")   is   "+QString::number(dist, 'f', 2);
   }else
      str="("+QString::number(x1, 'f', 2)+";"+QString::number(y1, 'f', 2)+")";
   if(_visualizationTab->visResult()->resultsExist())
   {
      str+="  Result ";
      if(_visualizationTab->visResult()->inDomainOfCalc(x1, y1))
      {
         double number = _visualizationTab->visResult()->interpolate(x1, y1,
               _visualizationTab->visData()->tabs()->isoDataTab()->timeStep());
         str+=QString::number(number,'f',2);
         if(_visualizationTab->visData()->tabs()->isoDataTab()->useStatic())
         {
            str+=" relative to static level ";
            str+=QString::number((*_data->staticLevel())-number,'f',2);
         }
      }
   }
   _data->qWells()->statusBar()->showMessage(str);

  // qDebug()<<" IIIimamo da je "<<_transform_a<<" "<<_transform_b<<" "<<_transform_c<<" "<<_transform_d<<" stoga "<<event->pos().x()<<" "<<event->pos().y()<<" ide u "<<inverseTransformX(transformX(event->pos().x()))<<" "<<inverseTransformY(transformY(event->pos().y()))<<"\n";
   
}

void VisPicture::mousePressEvent(QMouseEvent * event)
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

void VisPicture::mouseDoubleClickEvent(QMouseEvent *)
{
   _visualizationTab->visData()->xmin()->setText(QString::number(_data->xmin(),'f',2));
   _visualizationTab->visData()->xmax()->setText(QString::number(_data->xmax(),'f',2));
   _visualizationTab->visData()->ymin()->setText(QString::number(_data->ymin(),'f',2));
   _visualizationTab->visData()->ymax()->setText(QString::number(_data->ymax(),'f',2));
}

void VisPicture::mouseReleaseEvent(QMouseEvent * event)
{
   if (event->button() == Qt::LeftButton)
   {
      if(!_distanceT)
         this->setCursor(Qt::ArrowCursor);
      _drag=false;
   }
}

void VisPicture::wheelEvent(QWheelEvent * event)
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

void VisPicture::updateMinMax(const double xmin, const double xmax, 
      const double ymin, const double ymax)
{
   _data->xmin()=xmin;
   _data->xmax()=xmax;
   _data->ymin()=ymin;
   _data->ymax()=ymax;
   _data->setRatio();
   repaint();
}

void VisPicture::loadPicture()
{
   _data->xmin()=_data->getModel().picXmin();
   _data->xmax()=_data->getModel().picXmax();
   _data->ymin()=_data->getModel().picYmin();
   _data->ymax()=_data->getModel().picYmax();
   _data->setRatio();

   _pixmap.load(_data->getModel().pictureName()); 
}

/*void VisPicture::refresh()
{
   _data->xmin()=_data->getModel().picXmin();
   _data->xmax()=_data->getModel().picXmax();
   _data->ymin()=_data->getModel().picYmin();
   _data->ymax()=_data->getModel().picYmax();
   _ratio=(_data->getModel().picXmax()-_data->getModel().picXmin())/
      (_data->getModel().picYmax()-_data->getModel().picYmin());

   _pixmap.load(_data->getModel().pictureName());

}*/

QComboBox * addComboBoxIso(int defColor)
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

void VisPicture::repaint()
{
   update(0,0,width(),height());
}

void VisPicture::savePic()
{
   QString fileName = QFileDialog::getSaveFileName(this,
         tr("Save image"),".",tr("JPG Files (*.jpg)"));

   if(!fileName.endsWith(".jpg"))
      fileName.append(".jpg");

   _image->save(fileName);
}

void VisPicture::exportToDxf()
{
   QString fileN = QFileDialog::getSaveFileName(this,
         tr("Save dxf"),".",tr("DXF Files (*.dxf)"));
   
   if(!fileN.endsWith(".dxf"))
      fileN.append(".dxf");
   
   QFile file(fileN);
   file.open(QIODevice::WriteOnly | QIODevice::Text);
   QTextStream in(&file);

   in<<" 0\nSECTION\n  2\nENTITIES\n  0\n";

   int timeStep=_visualizationTab->visData()->tabs()->isoDataTab()->timeStep();
   int nisol=_visualizationTab->visResult()->nIsolines();
   for(int isol=0;isol<nisol;isol++)
   {
      int nlines=_visualizationTab->visResult()->nlines(timeStep, isol);
      for(int iline=0;iline<nlines;iline++)
      {
         Point2 p1 = _visualizationTab->visResult()->line1P(timeStep, isol, iline);
         Point2 p2 = _visualizationTab->visResult()->line2P(timeStep, isol, iline);
         writeLineDxf(p1._x, p1._y, p2._x, p2._y, in);
      }
   }

   int nswells=_visualizationTab->visResult()->nStreamLines();
   
   for(int i=0; i<nswells; i++)
   {
      int nlines=_visualizationTab->visResult()->nWStreamLines(i);
      for(int j=0; j<nlines;j++)
      {
         int nl=_visualizationTab->visResult()->streamLinesNLines(i,j);
         for(int k=0;k<nl;k++)
         {
            Point2 p1 = _visualizationTab->visResult()->line1PStream(i,j,k);
            Point2 p2 = _visualizationTab->visResult()->line2PStream(i,j,k);
            writeLineDxf(p1._x, p1._y, p2._x, p2._y, in);
         }
         int nd = _visualizationTab->visResult()->streamDotsNDots(i,j);
         for(int k=0; k<nd; k++)
         {
            Point2 p1 = _visualizationTab->visResult()->dotStream(i,j,k);
            writePointDxf(p1._x, p1._y, in);
         }
      }
   }  

   int nwells = _data->noOfWells();

   for(int iwell=0;iwell<nwells;iwell++)
      writePointDxf(_data->wells()[iwell].pos()._x, 
            _data->wells()[iwell].pos()._y, in);
   
   int npiez = _data->noOfPiezometers();

   for(int ipiez=0;ipiez<npiez;ipiez++)
      writePointDxf(_data->piezometers()[ipiez].pos()._x, 
            _data->piezometers()[ipiez].pos()._y, in);

   in<<"ENDSEC\n  0\nEOF";
   file.close();
}

void VisPicture::writeLineDxf(const double p1x, const double p1y, 
      const double p2x, const double p2y, QTextStream & in)
{
   in<<"LINE\n 8\n0\n";
   in<<" 10\n"<<p1x<<"\n";
   in<<" 20\n"<<p1y<<"\n";
   in<<" 30\n0.0\n";
   in<<" 11\n"<<p2x<<"\n";
   in<<" 21\n"<<p2y<<"\n";
   in<<" 31\n0.0\n";
   in<<"0\n";
}

void VisPicture::writePointDxf(const double p1x, const double p1y,
      QTextStream & in)
{
   in<<"POINT\n 8\n0\n";
   in<<" 10\n"<<p1x<<"\n";
   in<<" 20\n"<<p1y<<"\n";
   in<<" 30\n0.0\n";
   in<<"0\n";
}

void VisPicture::setObjectProp()
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
   QComboBox * wellColorCombo=addComboBoxIso(_wellColor);
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
   QComboBox * piezColorCombo=addComboBoxIso(_piezColor);
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
   QComboBox * constColorCombo = addComboBoxIso(_constColor);
   boundLayout->addWidget(constColorCombo,1,1);
   QLabel * labelColorImper = new QLabel("Impermeable");
   boundLayout->addWidget(labelColorImper,2,0);
   QComboBox * impColorCombo = addComboBoxIso(_impermColor);
   boundLayout->addWidget(impColorCombo,2,1);
   boundFrame->setLayout(boundLayout);
   boundFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(boundFrame);

   QFrame * isoNumbFrame = new QFrame;
   QLabel * isoLabel = new QLabel("Isolines");
   QLabel * isoNumbLabel = new QLabel("Label incidence");
   QGridLayout * isoNumbLayout = new QGridLayout;
   isoNumbLayout->addWidget(isoLabel,0,0);
   isoNumbLayout->addWidget(isoNumbLabel,1,0);
   QLineEdit * isoNumbLineEdit = new QLineEdit(&dialog);
   isoNumbLayout->addWidget(isoNumbLineEdit,1,1);
   isoNumbLineEdit->setText(QString::number(_isoLabelNumber));
   QLabel * play = new QLabel("Play duration in seconds");
   isoNumbLayout->addWidget(play,2,0);
   QLineEdit * playLineEdit = new QLineEdit(&dialog);
   isoNumbLayout->addWidget(playLineEdit, 2,1);
   playLineEdit->setText(QString::number(_playDuration));
   QLabel * labelIsoColor = new QLabel("Color");
   isoNumbLayout->addWidget(labelIsoColor,3,0);
   QComboBox * isoColorCombo=addComboBoxIso(_isoColor);
   isoNumbLayout->addWidget(isoColorCombo,3,1);
   isoNumbFrame->setLayout(isoNumbLayout);
   isoNumbFrame->setFrameShape(QFrame::StyledPanel);
   isoNumbFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(isoNumbFrame);

   QFrame * streamFrame = new QFrame;
   QLabel * streamLabel = new QLabel("Streamlines");
   QGridLayout * streamLayout = new QGridLayout;
   streamLayout->addWidget(streamLabel, 0, 0);
   QLabel * streamColorLabel = new QLabel("Color");
   streamLayout->addWidget(streamColorLabel, 1, 0);
   QComboBox * streamColorCombo=addComboBoxIso(_streamColor);
   streamLayout->addWidget(streamColorCombo,1,1);
   streamFrame->setLayout(streamLayout);
   streamFrame->setFrameShape(QFrame::StyledPanel);
   streamFrame->setFrameShape(QFrame::StyledPanel);
   dialogLayout->addWidget(streamFrame);

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
      _isoLabelNumber=isoNumbLineEdit->text().toInt();
      _playDuration=playLineEdit->text().toDouble();
      _isoColor=isoColorCombo->currentIndex();
      _streamColor=streamColorCombo->currentIndex();
      _data->getModel().setParamObjProp(_wellXOffset, _wellYOffset,
            _wellRadius, _wellColor, _showWellNames, _piezXOffset, _piezYOffset,
            _piezRadius, _piezColor, _showPiezNames, _constColor, _impermColor);
      _data->getModel().setIsolineProp(_isoLabelNumber, _playDuration, 
            _isoColor);
      _data->getModel().setStreamColor(_streamColor);
      repaint();
   }
}

void VisPicture::distanceTool()
{
   _distanceT=true;
   this->setCursor(Qt::CrossCursor);
}

void VisPicture::contextMenuRequest(QPoint pos)
{
   QMenu *menu = new QMenu(this);
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Save image", this, SLOT(savePic()));
  
   menu->addAction("Objects properties", this, SLOT(setObjectProp()));

   menu->addAction("Export to dxf", this, SLOT(exportToDxf()));

   menu->addAction("Distance tool", this, SLOT(distanceTool()));

   menu->popup(mapToGlobal(pos));
}

QSize VisPicture::minimumSizeHint() const
{
    return QSize(150,150);
}

void VisPicture::setTransform(const double g, const double h)
{
   _transform_a=g/(_data->xmax()-_data->xmin());
   _transform_b=-_transform_a*_data->xmin();
   _transform_c=-h/(_data->ymax()-_data->ymin());
   _transform_d=h-_transform_c*_data->ymin();
}

double VisPicture::transformX(double x) const
{
   return _transform_a*x+_transform_b;
}

double VisPicture::transformY(double y) const
{
   return _transform_c*y+_transform_d;
}

double VisPicture::inverseTransformX(double x) const
{
   return (1/_transform_a)*x-_transform_b/_transform_a;
}

double VisPicture::inverseTransformY(double y) const
{
   return (1/_transform_c)*y-_transform_d/_transform_c;
}

void VisPicture::paint(QPainter *painter, QPaintEvent * event)
{//this function is used to paint everything
   painter->setRenderHint(QPainter::Antialiasing, true);
   QPen pen;
   

    //QPen pen(QPen(Qt::red));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    painter->setPen(pen);
          
    double sirina;
    double visina;
    int wid=_visualizationTab->width()-_visualizationTab->visData()->width()-24;
    //24 because of qsplitter
    int hei = _visualizationTab->height()-10;
    if(ceil(wid/_data->ratio())<hei)
    {
       sirina=wid;
       visina=ceil(sirina/_data->ratio());
    }else{
       visina=hei;
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
          painter->drawText(xc+_piezXOffset, yc+_piezYOffset, 
                _data->piezometers()[i].name());
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

   int timeStep=_visualizationTab->visData()->tabs()->isoDataTab()->timeStep();
   int nisol=_visualizationTab->visResult()->nIsolines();
   pen.setColor(Qt::GlobalColor(_color[_isoColor]));
   painter->setPen(pen);
   for(int isol=0;isol<nisol;isol++)
   {
      int nlines=_visualizationTab->visResult()->nlines(timeStep, isol);
      for(int iline=0;iline<nlines;iline++)
      {
         Point2 p1 = _visualizationTab->visResult()->line1P(timeStep, isol, iline);
         Point2 p2 = _visualizationTab->visResult()->line2P(timeStep, isol, iline);

         painter->drawLine(transformX(p1._x), transformY(p1._y),
               transformX(p2._x), transformY(p2._y));
         if(iline%_isoLabelNumber==0)
            painter->drawText(transformX(p1._x)+2, transformY(p1._y)+2,
         _visualizationTab->visData()->tabs()->isoDataTab()->isolineVal(isol));

      }
   }
  
   pen.setWidth(2);
   pen.setColor(Qt::GlobalColor(_color[_streamColor]));
   painter->setPen(pen);
   int nswells=_visualizationTab->visResult()->nStreamLines();
   QPainterPath path;
   
   for(int i=0; i<nswells; i++)
   {
      int nlines=_visualizationTab->visResult()->nWStreamLines(i);
      for(int j=0; j<nlines;j++)
      {
         int nl=_visualizationTab->visResult()->streamLinesNLines(i,j);
         for(int k=0;k<nl;k++)
         {
            Point2 p1 = _visualizationTab->visResult()->line1PStream(i,j,k);
            Point2 p2 = _visualizationTab->visResult()->line2PStream(i,j,k);
            if(k==0)
               path.moveTo(transformX(p1._x), transformY(p1._y));
            path.lineTo(transformX(p2._x), transformY(p2._y));
         //painter->drawLine(transformX(p1._x), transformY(p1._y),
         //       transformX(p2._x), transformY(p2._y));
         }
         int nd = _visualizationTab->visResult()->streamDotsNDots(i,j);
         for(int k=0; k<nd; k++)
         {
            Point2 p1 = _visualizationTab->visResult()->dotStream(i,j,k);
            painter->drawEllipse(transformX(p1._x), transformY(p1._y), 3, 3);
         }
      }

   }
   painter->strokePath(path, pen);

  
    painter->setPen(palette().dark().color());
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(QRect(0, 0, width() - 1, height() - 1));
    
    QPainter painterW(this);
    QRect dirtyRect = event->rect();
    painterW.drawImage(dirtyRect, *_image, dirtyRect);      
}

void VisPicture::paintEvent(QPaintEvent * event)
{
   QSize newSize(width(),height());
   delete _image;
   _image=new QImage(newSize, QImage::Format_RGB32);
    _image->fill(qRgb(255, 255, 255));
    QPainter painter(_image);    
    paint(&painter, event);
}
