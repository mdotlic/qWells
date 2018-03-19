#ifndef PAINTERWIDGET
#define PAINTERWIDGET
#include <QWidget>
#include <QPixmap>
#include "lineEditSetBoundary.h"

class QDialog;
class BoundaryWidget;
class WellsTable;
class PiezoTable;
class Data;
class QImage;
class QLineEdit;
class QFrame;
//class LineEdit;

class PainterWidget : public QWidget
{
   Q_OBJECT
   public:
       PainterWidget(BoundaryWidget*,WellsTable*,PiezoTable*);
       QSize minimumSizeHint() const Q_DECL_OVERRIDE;
       void mouseDoubleClickEvent(QMouseEvent *) Q_DECL_OVERRIDE;
       void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
       void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
       void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
       void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      // QSize sizeHint() const Q_DECL_OVERRIDE;
//getters
       bool showWellNames()const {return _showWellNames;}
       bool showPiezNames()const {return _showPiezNames;}
       Data * data()const {return _data;}
       QPixmap pixmap()const {return _pixmap;}
       bool pixmapExist()const {return !_pixmap.isNull();}
       int wellRadius()const {return _wellRadius;}
       int piezRadius()const {return _piezRadius;}
       int wellXOffset()const {return _wellXOffset;}
       int wellYOffset()const {return _wellYOffset;}
       int piezXOffset()const {return _piezXOffset;}
       int piezYOffset()const {return _piezYOffset;}



       double transformX(const double x) const;
       double transformY(const double y) const;
       double inverseTransformX(const double x) const;
       double inverseTransformY(const double y) const;

       void setTransform(const double g, const double h);
       void loadImage(const QString & imageName);       
       double distance(const double x1, const double y1, const double x2, const double y2, const double refX, const double refY) const;
       double distancePoint(const double a, const double b, const double x, const double y, const double refX, const double refY) const;
       void paint(QPainter *painter, QPaintEvent * event);

       
public slots:
       void repaint();
       
   private slots:
      void setShowWellName();
      void setHideWellName();
      void setShowPiezName();
      void setHidePiezName();
      void imageProp();
      void setObjectProp();
      void savePic();
      void addWell();
      void addPiez();
      void setBound();
      void contextMenuRequest(QPoint point);
      void browse();
      void setRange();
      void submitWell();
      void submitPiez();
      void submitBound();
      void closeAddDialog();
      void closeAddPiezDialog();
      void closeAddBDialog();
      void setFocusBoundary(int);
      void distanceTool();
      void closeImagePropDialog();
      void setImageProp();

 //  signals:
   //   void clickedPaint(QPoint);
       
   protected:
       void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE; 

   private:
       BoundaryWidget * _boundaryWidget;
       WellsTable * _wellsTable;
       PiezoTable * _piezoTable;
       Data * _data;
       QPixmap _pixmap;
       double _transform_a = 1.0;//every point is transform to 
       double _transform_b = 0.0;//f(x)=ax+b
       double _transform_c = 1.0;//f(y)=cx+d
       double _transform_d = 0.0;
      /* double _picXmin = 0.0;
       double _picYmin = 0.0;
       double _picXmax = 150.0;
       double _picYmax = 150.0;*/
       //QString _fileName;
       bool _showWellNames = true;
       bool _showPiezNames = true;
       int _wellXOffset = -5; 
       int _wellYOffset = -4;
       int _piezXOffset = -5; 
       int _piezYOffset = -4;
       int _wellColor = 1;
       int _piezColor = 2;
       int _constColor = 2;
       int _impermColor = 1;
       int _wellRadius = 2;
       int _piezRadius = 2;
       std::vector<int> _color;
       //QGraphicsScene *scene;
       QImage * _image;
       //QDialog * _dialog;
       QLineEdit * _lineEdit;
       QPoint _lastDragPos;
       bool _drag = false;
       QDialog * _addDialog;
       QLineEdit * _nameLineEdit;
       QLineEdit * _xLineEdit;
       QLineEdit * _yLineEdit;
       std::vector<LineEditSetBoundary *> _xLineEditB;
       std::vector<LineEditSetBoundary *> _yLineEditB;
       QLineEdit * _rLineEdit;
       QLineEdit * _lineEditXmin;
       QLineEdit * _lineEditXmax;
       QLineEdit * _lineEditYmin;
       QLineEdit * _lineEditYmax;
       bool _addDialogIsActive = false;
       bool _boundaryDialogIsActive = false;
       bool _imagePropActive = false;
       size_t _clicked = 0;
       bool _distanceT=false;
       bool _paintDistance=false;
       QPoint _distanceOrigin;
       QPoint _distanceEnd;
};
#endif

