#ifndef VISPICTURE_H
#define VISPICTURE_H
#include <QWidget>
#include <QPixmap>

class Data;
class VisualizationTab;
class QTextStream;

class VisPicture : public QWidget
{
   Q_OBJECT
   public:
      VisPicture(Data *, VisualizationTab *);
      QSize minimumSizeHint() const Q_DECL_OVERRIDE;
      void paint(QPainter *painter, QPaintEvent * event);
      void setTransform(const double g, const double h);
      double transformX(const double x) const;
      double transformY(const double y) const;
      double inverseTransformX(const double x) const;
      double inverseTransformY(const double y) const;
      void mouseMoveEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseDoubleClickEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void mouseReleaseEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
      void wheelEvent(QWheelEvent * event) Q_DECL_OVERRIDE;
      void updateMinMax(const double xmin, const double xmax, 
      const double ymin, const double ymax);
      double playDuration(){return _playDuration;}
      void writeLineDxf(const double p1x, const double p1y, const double p2x,
            const double p2y, QTextStream & in);
      void writePointDxf(const double p1x, const double p1y, QTextStream & in);
      QImage * image() {return _image;}

   protected:
      void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
   public slots:
      void repaint();
   private slots:
      void contextMenuRequest(QPoint point);
      void savePic();
      void setObjectProp();
      void loadPicture();
      void distanceTool();
      void exportToDxf();
   private:
      Data * _data;
      VisualizationTab * _visualizationTab;
      double _transform_a = 1.0;//every point is transform to 
      double _transform_b = 0.0;//f(x)=ax+b
      double _transform_c = 1.0;//f(y)=cx+d
      double _transform_d = 0.0;
      double _xmin;
      double _ymin;
      double _xmax;
      double _ymax;
      double _ratio;
      QImage * _image;
      int _wellColor = 1;
      int _piezColor = 2;
      int _constColor = 2;
      int _impermColor = 1;
      int _isoColor = 0;
      int _streamColor = 0;
      bool _showWellNames = true;
      bool _showPiezNames = true;
      int _wellXOffset = -5; 
      int _wellYOffset = -4;
      int _piezXOffset = -5; 
      int _piezYOffset = -4;
      int _wellRadius = 2;
      int _piezRadius = 2;
      bool _drag=false;
      QPoint _lastDragPos;
      std::vector<int> _color;
      QPixmap _pixmap;
      int _isoLabelNumber=2;
      double _playDuration=10.0;
      bool _distanceT=false;
      bool _paintDistance=false;
      QPoint _distanceOrigin;
      QPoint _distanceEnd;
};

#endif
