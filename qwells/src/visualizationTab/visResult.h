#ifndef VISRESULT_H
#define VISRESULT_H
//#include <QLine>
#include <QVector>

class Data;
class Point2;
class Line2D;
class Line;

class VisResult
{
   public:
      VisResult(Data *);
      void calculateResult(const double xmin, const double xmax, 
         const double ymin, const double ymax, const int xdiv, const int ydiv);
      void calcMarchingSquares(const QVector<double> & listNumbers);
      Point2 calcOtherPoint(const Point2 & startPoint, double & singleTime, 
            int & xp, int & yp, const double K, const double porosity, 
            const int timeStep, const std::vector<Line> & lines, const std::vector<double> & wl);
      void calcStreamlines(const int i, const int npoints, const double K,
            const double porosity, const int timeStep, const double timeLength,
            const double timeSample);
      Point2 linearInterp(const Point2 & p0, const Point2 & p1, 
            const double value0, const double value1, const double value) const;
      int nlines(const int timestep, const int isol)const;
      Point2 line1P(const int timestep, const int isol, const int iline);
      Point2 line2P(const int timestep, const int isol, const int iline);
      void clear();
      int nIsolines()const;
      bool inRealDomain(Point2 & p0, Point2 & p1,
        const std::vector<Line> & lines, const std::vector<double> & wl) const;
      double valueInterp(const Point2 & p0, const Point2 & p1,
            const Point2 & pm, const double value0, const double value1)const;
      bool inDomain(const Point2 & p, const std::vector<Line> & lines, 
      const std::vector<double> & wl);
      double minVal() {return _minVal;}
      double maxVal() {return _maxVal;}
      int nStreamLines() const {return _streamLines.size();} 
      int nWStreamLines(const int i) const  {return _streamLines[i].size();}
      int streamLinesNLines(const int i, const int j) const;
      int streamDotsNDots(const int i, const int j) const;
      Point2 line1PStream(const int i, const int j, const int k); 
      Point2 line2PStream(const int i, const int j, const int k);
      Point2 dotStream(const int i, const int j, const int k);
      void clearStreamLines();
      bool resultsExist() const;
      double interpolate(const double x, const double y, const int k) const;
      bool inDomainOfCalc(const double x, const double y) const;
   private:
      QVector<QVector<QVector<double> > > _result;
      QVector<QVector<QVector<Line2D> > > _lines;
      QVector<QVector<QVector<Line2D> > > _streamLines;
      QVector<QVector<QVector<Point2> > > _streamDots;
      Data * _data;
      double _xmin;
      double _xmax;
      double _ymin;
      double _ymax;
      double _dx;
      double _dy;
      int _xdiv;
      int _ydiv;
      double _minVal;
      double _maxVal;
};
#endif
