#ifndef BOUNDARY_H
#define BOUNDARY_H

#include <limits>
#include "twoDgeom.h"

static const double pi = 3.14159265358979;
class Boundary
{
   public:     
      Boundary()
      {
          _maxDistance=std::numeric_limits<double>::max();
          _alpha=std::numeric_limits<double>::max();
          _beta=std::numeric_limits<double>::max();
         // _nmaps1=std::numeric_limits<size_t>::max();
         // _nmaps2=std::numeric_limits<size_t>::max();
          _type=std::numeric_limits<size_t>::max();
          _referentWell=std::numeric_limits<size_t>::max();          
      }
      size_t type() const {return _type;}
      double rho(size_t i) const {return _rho[i];}
      std::vector<double> rho() const { return _rho; }
      std::vector<bool> lineType() const { return _lineType; }
      bool lineType(size_t i) const {return _lineType[i];}
      double alpha() const {return _alpha;}
      double alphaDeg() const {return _alpha*180.0/pi;}
      double betaDeg() const {return _beta*180.0/pi;}
      double beta() const {return _beta;}
      //Line & line(size_t i){ return _lines[i];}
      size_t nlines()const  {return _lineType.size();}
      size_t refWell() const {return _referentWell;}
     // size_t nMaps1() const {return _nmaps1;}
     // size_t nMaps2() const {return _nmaps2;}
      double maxDistance() const {return _maxDistance;}

      void setType(size_t type) 
      {
         _type=type;
         if(type<3){
            _rho.resize(type, std::numeric_limits<double>::max());
            _lineType.resize(type, false);
            //_lines.resize(type);
         }else if(type<6){
            _rho.resize(type-1, std::numeric_limits<double>::max());
            _lineType.resize(type-1,false);      
            //_lines.resize(type-1);
         }else{//should not happen
            std::cerr<<" ERROR!!! Bad boundary type.";
            assert(false);
         }
      }
      void setRho(size_t i, double value)  { _rho[i]=value;}
      void setLineType(size_t i, bool sign) {_lineType[i]=sign;}
      void setAlpha(double value)  
      {
         _alpha=pi*value/180.0;
      }
      void setBeta(double value)  
      {
         if(fmod(360.0, value)!=0.0)
         {//should not happen
            std::cout<<" wtf : fmod(360.0, value) = "<<fmod(360.0, value)<<" value = "<<value<<"\n";
            std::cerr<<" ERROR!!! 360 is not divisible with prescribed angle.\n";
            exit(-1);
         }
         _beta=pi*value/180.0;
      }
      void setRefWell(const size_t i) {_referentWell=i;}
     // void setnMaps1(const size_t i) {_nmaps1=i;}
     // void setnMaps2(const size_t i) {_nmaps2=i;}
      void setMaxDist(const double value) {_maxDistance=value;}
      void clear()
      {
         _rho.clear();
         _lineType.clear();
         //_lines.clear();
      }
   private:
      double _alpha, _beta;
      std::vector<double> _rho;
      std::vector<bool> _lineType; 
      //true for same sign -- water impermeable boundary, 
      //false for negative sign -- constant potnetial on the boundary

      size_t _referentWell;
      double _maxDistance;
      //size_t _nmaps1, _nmaps2;
      size_t _type;
      //std::vector<Line> _lines;
      //boundType 0 - no boundary --- no rho no alpha
      //1 - single boundary --- rho0, alpha
      //2 - parallel boundary --- rho0, rho1, alpha
      //3 - angle boundary --- rho0, rho1, alpha, beta
      //4 - three lines boundary --- rho0, rho1, rho2, alpha
      //5 - box boundary --- rho0, rho1, rho2, rho3, alpha
};



#endif
