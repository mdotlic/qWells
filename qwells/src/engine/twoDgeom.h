#ifndef TWODGEOM_H
#define TWODGEOM_H

#include <iostream>
#include <assert.h>
#include "position.h"

template<class Point2>
inline double dot (const Point2 & v1, const Point2 & v2)
{
   return v1._x*v2._x+v1._y*v2._y;
}

template<class Point2>
inline double twoDcross (const Point2 & v1, const Point2 & v2)
{//we take two dimensional vectors and take that the third coordinate is zero
   //also we return only the intensity of cross product
   return v1._x*v2._y-v1._y*v2._x;
}

class Line
{
   public:
      double a()const {return _line_a;}
      double b()const {return _line_b;}
      double c()const {return _line_c;}
      void constructLineWithPoint(const double angle, const Point2 & point)
      {//line passes through point with angle angle with x-axis
          _line_a=-sin(angle);
          _line_b=cos(angle);
          _line_c=-(_line_a*point._x+_line_b*point._y);
      }

      void constructLine(const double angle, const double distance,
            const Point2 & wellPos)
      {
         _line_a=-sin(angle);
         _line_b=cos(angle);      
         _line_c=distance+twoDcross(wellPos, Point2 (cos(angle), sin(angle)));
         //(cos,sin) is normal to our line
      }
      
      void constructLinePoints(const Point2 & p0, const Point2 & p1)
      {//constructs lines with two points
         _line_a=p0._y-p1._y;
         _line_b=p1._x-p0._x;
         _line_c=p0._x*p1._y-p1._x*p0._y;

      }
   private:
      double _line_a;//line is writen in hessian normal form
      double _line_b;//ax+by+c=0
      double _line_c;//- sin(alpha) x + cos(alpha) y + distance from (0;0) 
};

//template<class Point2>


inline Point2 projection(const Line & line, const Point2 & point)
{//line can be given with a+tn where t is real number and
 //if line.b!=0 then n=(1, -a/b) a=(0, -c/b)
 //else if line.b==0 then n=(-b/a) a=(-c/a, 0)
   Point2 a,n;
   if(fabs(line.a())<1e-13 && fabs(line.b())<1e-13)//should not happen
      assert(false);
   if(fabs(line.b())>1e-13){
      n._x=1.0;
      n._y=-line.a()/line.b();
      a._x=0.0;
      a._y=-line.c()/line.b();
   }else{
      n._x=-line.b()/line.a();
      n._y=1.0;
      a._x=-line.c()/line.a();
      a._y=0.0;
   }
   n.normalize();
   return a-(dot((a-point),n)*n);
}

inline Point2 reflection (const Line & line, const Point2 & point)
{
   return 2*projection(line, point)-point;
}

inline double projDistance (const Line & line, const Point2 & point)
{
   return (projection(line, point)-point).two_norm();
}

inline Point2 rateReflection (const double rate, const Line & line, 
      const Point2 & point)
{//rate=dist_to_/point_dist_to_projection
   return rate*(projection(line, point)-point)+point;
}

inline Point2 intersection (const Line & line0, const Line & line1)
{
   double d=line0.a()*line1.b()-line1.a()*line0.b();   
   if(fabs(d)<1e-10)
   {//should not happen
      std::cerr<<" lines are parallel "<<line0.a()<<"*"<<line1.b()<<" - "<<line1.a()<<"*"<<line0.b()<<"\n";
      exit(-1);
   }
   double dx=-line0.c()*line1.b()+line1.c()*line0.b();//because minus c
   double dy=-line0.a()*line1.c()+line1.a()*line0.c();
   return Point2 (dx/d,dy/d);
}/*
Point2 rotation (const Point2 point, const Point2 center, const double theta)
{//rotation of point for angle theta around center
   return Point2 (
         cos(theta)*(point._x-center._x)-sin(theta)*(point._y-center._y),
         sin(theta)*(point._x-center._x)-cos(theta)*(point._y-center._y));
}*/

#endif
