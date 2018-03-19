#ifndef POSITION_H
#define POSITION_H
#include <math.h>

class Point2
{ //two double numbers can be position in xy or time value 
   public:
      Point2(){}
      Point2(const Point2 & p):_x(p._x), _y(p._y) {}
      Point2 (const double x, const double y):_x(x), _y(y)  {}
      //const double x() const { return _x;}
      //const double y() const { return _y;}     

      //void set (const double xvalue, const double yvalue)
      //{
      //   _x=xvalue;_y=yvalue;
      //}
      //void setX(const double value){_x=value;}
      //void setY(const double value){_y=value;}
      double two_norm() 
      {
         return sqrt(this->_x*this->_x+this->_y*this->_y);   
      }
      void normalize()
      {
         double norm=this->two_norm();
         this->_x/=norm;
         this->_y/=norm;
      }
      double _x,_y;
};

inline Point2 operator+(const Point2 & p1, const Point2 & p2)
{
   return Point2(p1._x+p2._x,p1._y+p2._y);
}

inline Point2 operator-(const Point2 & p1, const Point2 & p2)
{
   return Point2(p1._x-p2._x,p1._y-p2._y);
}

inline Point2 operator*(const double c, const Point2 & p)
{
   return Point2(c*p._x,c*p._y);
}

inline bool operator==(const Point2 & p1, const Point2 & p2)
{
   return (fabs (p1._x-p2._x)<1e-8 && fabs(p1._y-p2._y)<1e-8);
}

class Line2D
{
   public:
      Line2D(){}
      Line2D (const Point2 & p1, const Point2 & p2):_p1(p1), _p2(p2)  {}
      Point2 p1(){return _p1;}
      Point2 p2(){return _p2;}
   private:
      Point2 _p1;
      Point2 _p2;
};

#endif

