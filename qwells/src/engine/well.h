#ifndef WELL_H
#define WELL_H

#include <QString>
#include <cstddef>
#include <vector>
#include <string>
#include "position.h"
//bool compare (const Point2 & p,const Point2 & q) { return (p.x()<q.x()); }
class Base
{
   public:
      ~Base()
      {}
      Point2 pos() const  { return _pos;}
      
      void setPos(const Point2 & position) 
      {
         _pos=Point2 (position);
      }
   protected:
      Point2 _pos;
};

class FictWell: public Base
{//this is class for the fictive wells
   public:     
      ~FictWell()
      {
         _q.clear();
      }
      size_t nq() const {return _q.size();}
      Point2 qi(const size_t i) const {return _q[i];}
      const std::vector<Point2> & q()const {return _q;}      
      double radius() const {return _r;}
      
      // non-const for changing q from tables:
      Point2 qi(const size_t i) {return _q[i];}
      std::vector<Point2> & q() {return _q;}   
      
      void addQ(const Point2 & timeValue) 
      {
         _q.push_back(timeValue);
      }  
      void setRadius (const double r){ _r=r;}
      void copyQ (const std::vector<Point2> & q){ _q=q;}
      void minusCopyQ (const std::vector<Point2> & q) 
      {
         _q.resize(q.size());
         for(size_t i=0;i<q.size();i++)
         {
            _q[i]._x=q[i]._x;
            _q[i]._y=-q[i]._y;
         }
      }
   protected:
      double _r;
      std::vector<Point2> _q; //first number is time, second is q-value
};

class Piezometer: public Base
{
   public:
      Piezometer(const QString & name, const Point2 & pos)
      {
         setName(name);
         setPos(pos);
      }
      Piezometer(){}
      ~Piezometer()
      {
         _Sm.clear();
         //delete _name;
      }
      QString name ()const { return _name;}
      size_t nS () const {return _Sm.size();}
      Point2 Smi(const size_t i) const {return _Sm[i];}
      const std::vector<Point2> & Sm() const {return _Sm;}   

      // non-const for changing the vectors from tables:
      Point2 Smi(const size_t i) {return _Sm[i];}
      std::vector<Point2> & Sm() {return _Sm;} 

      void setName (const QString & name) { _name=name;}
      void addMeasuredS(const Point2 & timeValue) 
      {
         _Sm.push_back(timeValue);
      }
   protected:
      QString _name;
      std::vector<Point2> _Sm; //first number is time, second is S-mesured
};

class Well:public FictWell
{
   public:
      Well(const QString & name, const Point2 & pos, const double r)
      {
         setName(name);
         setPos(pos);
         setRadius(r);
      }
      Well() {}
      ~Well()
      {
         _Sm.clear();
         _bigA.clear();
         _bigB.clear();
      }

      QString name () const { return _name;} 
      size_t nS () const {return _Sm.size();}
      Point2 Smi(const size_t i) const {return _Sm[i];}
      const std::vector<Point2> & Sm() const {return _Sm;}
      size_t nA () const {return _bigA.size();}
      size_t nB () const {return _bigB.size();}
      Point2 bigA(const size_t i) const {return _bigA[i];}
      Point2 bigB(const size_t i) const {return _bigB[i];}
      const std::vector<Point2> & bigA()const {return _bigA;}
      const std::vector<Point2> & bigB()const {return _bigB;}

      // non-const for changing the vectors from tables:
      Point2 Smi(const size_t i) {return _Sm[i];}
      std::vector<Point2> & Sm() {return _Sm;}  
      Point2 & bigA(const size_t i) {return _bigA[i];}
      Point2 & bigB(const size_t i) {return _bigB[i];} 
      std::vector<Point2> & bigA() {return _bigA;}
      std::vector<Point2> & bigB() {return _bigB;}

      void addMeasuredS(const Point2 & timeValue) 
      {
         _Sm.push_back(timeValue);
      }
      void setName (const QString & name) { _name=name;}  
      void addBigA(const Point2 & timeValue) 
      {
         _bigA.push_back(timeValue);
      } 
      void addBigB(const Point2 & timeValue) 
      {
         _bigB.push_back(timeValue);
      }
   protected:
      QString _name;  
      std::vector<Point2> _Sm; //first number is time, second is S-mesured
      std::vector<Point2> _bigA;
      std::vector<Point2> _bigB;
};
#endif
