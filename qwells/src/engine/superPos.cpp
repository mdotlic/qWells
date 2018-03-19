
#include <QVector>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <cmath>
#include <limits>
#include <fstream>

// #include "position.h"
// #include "twoDgeom.h"
#include "well.h"
#include "modelInformation.h"
#include "boundary.h"
#include "superPos.h"

//static const double pi = 3.14159265358979;

void SuperPos::makeSMHDtimes(QVector<double> & times,
      QVector<double> & timeInSMHD, const size_t timeUnit)
{
   if(timeUnit==0)
   {
      timeInSMHD=times;      
   }else{
      timeInSMHD.resize(times.size());
      double divide=(timeUnit==1 ? 60.0: (timeUnit == 2 ? 3600.0 : 86400.0));
      //#pragma omp parallel for
      for(int i=0;i<times.size();i++)
      {
         timeInSMHD[i]=times[i]/divide;
      }
   }
}


void SuperPos::makeTimes(const std::vector<Well> & wells, 
      const std::vector<Piezometer>  & piez, QVector<double> & times, 
      QVector<double> & timeInSMHD, const size_t timeUnit)
{
   size_t nwells=wells.size();
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      for(size_t iq=0;iq<wells[iwell].nq();iq++)      
        times.push_back(wells[iwell].qi(iq)._x);
      for(size_t is=0;is<wells[iwell].nS();is++)
         times.push_back(wells[iwell].Smi(is)._x);
      for(size_t ia=0;ia<wells[iwell].nA();ia++)
         times.push_back(wells[iwell].bigA(ia)._x);
      for(size_t ib=0;ib<wells[iwell].nB();ib++)
         times.push_back(wells[iwell].bigB(ib)._x);
   } 
   size_t npiez=piez.size();
   for(size_t ipiez=0;ipiez<npiez;ipiez++)
   {
      for(size_t is=0;is<piez[ipiez].nS();is++)
         times.push_back(piez[ipiez].Smi(is)._x);
   }
   std::sort (times.begin(), times.end());
//    if(times.size()==0)
//    {
//       std::cerr<<" ERROR!!! Times are not initialized. \n";
//       exit (-1);
//    }
  
   if(times.size()!=0)
   {
      for(int i=0;i<times.size()-1;i++)
         if(times[i]==times[i+1])
         {
            times.erase(times.begin()+i);
            i--;
         }
   }else
   { 
      QString qstr("Nothing to calculate - no time data!!!");
      throw qstr;
   }
   makeSMHDtimes(times, timeInSMHD, timeUnit);
}

/*inline void makeFictiveParallelNMaps(const std::vector<Well> & wells, 
      std::vector<FictWell> & fictiveWells, const std::vector<Line> & lines,
      const std::vector<bool> & lineType, const size_t nmaps, 
      const double wholeDist)
{//parallel lines if number of mappings are prescribed
   size_t nwells=wells.size();
   fictiveWells.resize(2*nwells*nmaps);
   //distance between parallel lines
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      std::vector<double> proj(2);
      proj[0]=projDistance(lines[0], wells[iwell].pos());
      proj[1]=wholeDist-proj[0];

      for(size_t i=0;i<nmaps;i++)
      {
         for(size_t j=0;j<2;j++)
         {
            auto q= (i==0 ? wells[iwell].q() : 
                  fictiveWells[2*nmaps*iwell+2*(i-1)+j].q());
            if(i%2==0)
            {
               fictiveWells[2*nmaps*iwell+2*i+j].setPos(
                     rateReflection((2*proj[j]+i*(wholeDist))/proj[j],
                        lines[j], wells[iwell].pos()));
            }else{
               fictiveWells[2*nmaps*iwell+2*i+j].setPos(
                     rateReflection((i+1)*wholeDist/proj[j], 
                        lines[j], wells[iwell].pos()));
            }
            fictiveWells[2*nmaps*iwell+2*i+j].setRadius(
                  wells[iwell].radius());
            if((lineType[0] && lineType[1]) ||
              (lineType[j] && i%2==0) || (!lineType[j] && i%2==1))
            {
               fictiveWells[2*nmaps*iwell+2*i+j].copyQ(q);
            }else{
               fictiveWells[2*nmaps*iwell+2*i+j].minusCopyQ(q);
            }
         }
      }
   }
}*/

void SuperPos::makeFictiveParallelMaxDist(const std::vector<Well> & wells, 
      std::vector<FictWell> & fictiveWells, const std::vector<Line> & lines,
      const std::vector<bool> & lineType, const double maxDist, 
      const double wholeDist, const size_t refWell)
{//parallel lines if number of mappings are prescribed
   size_t nwells=wells.size();
   //fictiveWells.resize(2*nwells*nfw1);
   //distance between parallel lines
   Point2 refWellPos=wells[refWell].pos();
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      std::vector<double> proj(2);
      proj[0]=projDistance(lines[0], wells[iwell].pos());
      proj[1]=wholeDist-proj[0];
      if(proj[1]<=0)
         continue;
      for(size_t j=0;j<2;j++)
      {
         for(size_t i=0;i<1000;i++)//i shows the number of mapping
         {
            auto q= (i==0 ? wells[iwell].q() : 
                  fictiveWells.back().q());
            FictWell fictW;
            Point2 pos;
            if(i%2==0)
            {
               pos=rateReflection((2*proj[j]+i*(wholeDist))/proj[j],
                     lines[j], wells[iwell].pos());
            }else{
               pos=rateReflection((i+1)*wholeDist/proj[j], 
                     lines[j], wells[iwell].pos());
            }
            if((pos-refWellPos).two_norm()>maxDist)
               break;
            fictW.setPos(pos);
            fictW.setRadius(
                  wells[iwell].radius());

            if((lineType[0] && lineType[1]) ||
                  (lineType[j] && i%2==0) || (!lineType[j] && i%2==1))
            {
               fictW.copyQ(q);
            }else{
               fictW.minusCopyQ(q);
            }
            fictiveWells.push_back(fictW);
         }
      }
   }
}

void SuperPos::makeFictiveWellsOneLine(const std::vector<Well> & wells, 
      std::vector<FictWell> & fictiveWells, const Line & line, 
      const bool lineType)
{
   size_t nwells=wells.size();
   fictiveWells.resize(nwells);
   for(size_t i=0;i<nwells;i++)
   {
      fictiveWells[i].setPos(reflection(line, wells[i].pos()));
      fictiveWells[i].setRadius(wells[i].radius());
      if(lineType)
         fictiveWells[i].copyQ(wells[i].q());
      else
         fictiveWells[i].minusCopyQ(wells[i].q());
   }
}

void SuperPos::makeFictiveWells(const std::vector<Well> & wells, 
      Boundary & boundary, std::vector<FictWell> & fictiveWells)
{
   size_t nwells=wells.size();
   //if(boundary.type()==0) there is no fictive wells
   if(boundary.type()==1){//there is only one boundary line
      Line line;
      QString qstr1(" ");
      if(boundary.rho(0) == std::numeric_limits<double>::max())
      {
         boundary.setRho(0, 100.0);
         qstr1.append("Distance to line1 set to default value 100.\n");
      }

      line.constructLine(boundary.alpha(), boundary.rho(0),
            wells[boundary.refWell()].pos());
      makeFictiveWellsOneLine(wells, fictiveWells, line, boundary.lineType(0));
      if(qstr1!=" ")
         throw qstr1;

   }else if(boundary.type()==2){
      std::vector<bool> lineType={boundary.lineType(0), boundary.lineType(1)};
      std::vector<Line> lines(2);
      QString qstr1(" ");
      if(boundary.rho(0) == std::numeric_limits<double>::max())
      {
         boundary.setRho(0, 100.0);
         qstr1.append("Distance to line1 set to default value 100.\n");
      }
      if(boundary.rho(1) == std::numeric_limits<double>::max())
      {
         boundary.setRho(1, -100.0);
         qstr1.append("Distance to line2 set to default value -100.\n");
      }
      if(boundary.maxDistance() == std::numeric_limits<double>::max())
      {
         boundary.setMaxDist(400.0);
         qstr1.append("maxDist is set to default value 400.\n");
      }

      lines[0].constructLine(boundary.alpha(), boundary.rho(0),
         wells[boundary.refWell()].pos());
      lines[1].constructLine(boundary.alpha(), boundary.rho(1),
         wells[boundary.refWell()].pos());
      double wholeDist=fabs(boundary.rho(0))+fabs(boundary.rho(1));
      /*if(boundary.nMaps1()!=std::numeric_limits<size_t>::max())
         makeFictiveParallelNMaps(wells, fictiveWells, lines, lineType,
               boundary.nMaps1(), wholeDist);
      else*/
         makeFictiveParallelMaxDist(wells, fictiveWells, lines, lineType,
               boundary.maxDistance(), wholeDist, boundary.refWell());
       if(qstr1!=" ")
         throw qstr1;

   }else if(boundary.type()==3){//angle
      if(boundary.beta() == std::numeric_limits<double>::max())
      {
         throw(QString("Beta is not set. Calculation is not performed!\n"));
      }
      QString qstr1(" ");
      if(boundary.rho(0) == std::numeric_limits<double>::max())
      {
         boundary.setRho(0, 100.0);
         qstr1.append("Distance to line1 set to default value 100.\n");
      }
      if(boundary.rho(1) == std::numeric_limits<double>::max())
      {
         boundary.setRho(1, -100.0);
         qstr1.append("Distance to line2 set to default value -100.\n");
      }
      /*if(boundary.maxDistance() == std::numeric_limits<double>::max())
      {
         boundary.setMaxDist(400.0);
         qstr1.append("maxDist is set to default value 400.\n");
      }*/


      if(fmod(2*pi/boundary.beta(), 2.0)==1 && 
            (boundary.lineType(0)==false || boundary.lineType(1)==false))
      {
         QString qstr(" ERROR!!! Method is not applicable, mapping is not possible with this angle and boundary types.");
         throw qstr;
      }
      size_t nrot=2*pi/boundary.beta()-1;//number of fictive regions
      fictiveWells.resize(nrot*nwells);
      std::vector<Line> lines(2);
      lines[0].constructLine(boundary.alpha(), boundary.rho(0),
         wells[boundary.refWell()].pos());
      lines[1].constructLine(boundary.alpha()+boundary.beta(), boundary.rho(1),
         wells[boundary.refWell()].pos());
      Point2 intersect=intersection(lines[0], lines[1]);      
      
      for(size_t i=0;i<nrot;i++)
      {
         for(size_t iwell=0;iwell<nwells;iwell++)
         {
            Line line;
            line.constructLineWithPoint(
                  boundary.alpha()+(i+1)*boundary.beta(), intersect);
            Point2 point= (i==0 ? wells[iwell].pos() : 
                  fictiveWells[(i-1)*nwells+iwell].pos());
            fictiveWells[i*nwells+iwell].setPos(reflection(line, point));
            fictiveWells[i*nwells+iwell].setRadius(
                     wells[iwell].radius());
            if((boundary.lineType(0) && boundary.lineType(1)) ||
                (boundary.lineType(0)!=boundary.lineType(1) && 
                 ((i%2==0 && boundary.lineType(1)) || 
                  (i%2==1 && !boundary.lineType(1)))))
            {
               fictiveWells[i*nwells+iwell].copyQ(i==0 ? wells[iwell].q() : 
                     fictiveWells[(i-1)*nwells+iwell].q());
            }else //if(!boundary.lineType(0) && !boundary.lineType(1)
            {//change sign
               fictiveWells[i*nwells+iwell].minusCopyQ(i==0? wells[iwell].q() :
                  fictiveWells[(i-1)*nwells+iwell].q());
            }

         }
      }
      if(qstr1!=" ")
         throw qstr1;
   
   }else if(boundary.type()==4){//Pi boundary
      QString qstr1(" ");
      if(boundary.rho(0) == std::numeric_limits<double>::max())
      {
         boundary.setRho(0, 100.0);
         qstr1.append("Distance to line1 set to default value 100.\n");
      }
      if(boundary.rho(1) == std::numeric_limits<double>::max())
      {
         boundary.setRho(1, 100.0);
         qstr1.append("Distance to line2 set to default value 100.\n");
      } 
      if(boundary.rho(2) == std::numeric_limits<double>::max())
      {
         boundary.setRho(2, -100.0);
         qstr1.append("Distance to line3 set to default value -100.\n");
      }
      if(boundary.maxDistance() == std::numeric_limits<double>::max())
      {
         boundary.setMaxDist(400.0);
         qstr1.append("maxDist is set to default value 400.\n");
      }

      Line line;
      line.constructLine(boundary.alpha()+pi/2, boundary.rho(2),
            wells[boundary.refWell()].pos());
      std::vector<FictWell> fictW1;//fictive wells on the upper boundary
      makeFictiveWellsOneLine(wells, fictW1, line, boundary.lineType(1));
      std::vector<Well> wells1;//wells1 are wells + fictW1
      for(size_t iwell=0;iwell<nwells;iwell++)
         wells1.push_back(wells[iwell]);
      for(size_t ifw=0;ifw<nwells;ifw++)
      {
         Well well;
         well.setPos(fictW1[ifw].pos());
         well.setRadius(fictW1[ifw].radius());
         well.copyQ(fictW1[ifw].q());
         wells1.push_back(well);
      }
      std::vector<FictWell> fictW2;//fictive wells maked by parallel lines
      std::vector<bool> lineType={boundary.lineType(0), boundary.lineType(2)};
      std::vector<Line> lines(2);
      lines[0].constructLine(boundary.alpha(), boundary.rho(0),
         wells[boundary.refWell()].pos());
      lines[1].constructLine(boundary.alpha(), boundary.rho(1),
         wells[boundary.refWell()].pos());
      double wholeDist=fabs(boundary.rho(0))+fabs(boundary.rho(1));

      //wells1 are mapped with parallel line

     /* if(boundary.nMaps1()!=std::numeric_limits<size_t>::max())
         makeFictiveParallelNMaps(wells1, fictW2, lines, lineType,
               boundary.nMaps1(), wholeDist);
      else*/
         makeFictiveParallelMaxDist(wells1, fictW2, lines, lineType,
               boundary.maxDistance(), wholeDist, boundary.refWell());
      for(size_t i=0;i<fictW1.size();i++)
         fictiveWells.push_back(fictW1[i]);
      for(size_t i=0;i<fictW2.size();i++)
         fictiveWells.push_back(fictW2[i]);   

      if(qstr1!=" ")
         throw qstr1;
   }else if(boundary.type()==5){//four boundaries
      QString qstr1(" ");
      if(boundary.rho(0) == std::numeric_limits<double>::max())
      {
         boundary.setRho(0, 100.0);
         qstr1.append("Distance to line1 set to default value 100.\n");
      }
      if(boundary.rho(1) == std::numeric_limits<double>::max())
      {
         boundary.setRho(1, 100.0);
         qstr1.append("Distance to line2 set to default value 100.\n");
      } 
      if(boundary.rho(2) == std::numeric_limits<double>::max())
      {
         boundary.setRho(2, -100.0);
         qstr1.append("Distance to line3 set to default value -100.\n");
      }
      if(boundary.rho(3) == std::numeric_limits<double>::max())
      {
         boundary.setRho(3, -100.0);
         qstr1.append("Distance to line4 set to default value -100.\n");
      }
      if(boundary.maxDistance() == std::numeric_limits<double>::max())
      {
         boundary.setMaxDist(400.0);
         qstr1.append("maxDist is set to default value 400.\n");
      }

      std::vector<FictWell> fictW1;
      //fictive wells maked by first pair of parallel lines
      std::vector<bool> lineType1={boundary.lineType(0), boundary.lineType(1)};
      std::vector<Line> lines1(2);
      lines1[0].constructLine(boundary.alpha(), boundary.rho(0),
         wells[boundary.refWell()].pos());
      lines1[1].constructLine(boundary.alpha(), boundary.rho(1),
         wells[boundary.refWell()].pos());
      double wholeDist1=fabs(boundary.rho(0))+fabs(boundary.rho(1));

     /* if(boundary.nMaps1()!=std::numeric_limits<size_t>::max())
         makeFictiveParallelNMaps(wells, fictW1, lines1, lineType1,
               boundary.nMaps1(), wholeDist1);
      else*/
         makeFictiveParallelMaxDist(wells, fictW1, lines1, lineType1,
               boundary.maxDistance(), wholeDist1, boundary.refWell());

      std::vector<Well> wells1;//wells1 are wells + fictW1
      for(size_t iwell=0;iwell<nwells;iwell++)
         wells1.push_back(wells[iwell]);
      for(size_t ifw=0;ifw<fictW1.size();ifw++)
      {
         Well well;
         well.setPos(fictW1[ifw].pos());
         well.setRadius(fictW1[ifw].radius());
         well.copyQ(fictW1[ifw].q());
         wells1.push_back(well);
      }
      std::vector<FictWell> fictW2;
      //fictive wells maked by second pair of parallel lines
      std::vector<bool> lineType2={boundary.lineType(2), boundary.lineType(3)};
      std::vector<Line> lines2(2);
      lines2[0].constructLine(boundary.alpha()+pi/2, boundary.rho(2),
         wells[boundary.refWell()].pos());
      lines2[1].constructLine(boundary.alpha()+pi/2, boundary.rho(3),
         wells[boundary.refWell()].pos());

      double wholeDist2=fabs(boundary.rho(2))+fabs(boundary.rho(3));

      //wells1 are mapped with parallel line

     /* if(boundary.nMaps2()!=std::numeric_limits<size_t>::max())
         makeFictiveParallelNMaps(wells1, fictW2, lines2, lineType2,
               boundary.nMaps2(), wholeDist2);
      else*/
         makeFictiveParallelMaxDist(wells1, fictW2, lines2, lineType2,
               boundary.maxDistance(), wholeDist2, boundary.refWell());
      for(size_t i=0;i<fictW1.size();i++)
         fictiveWells.push_back(fictW1[i]);
      for(size_t i=0;i<fictW2.size();i++)
         fictiveWells.push_back(fictW2[i]); 
      
      if(qstr1!=" ")
         throw qstr1;
   }
}

double SuperPos::calculateU(const ModelInformation & model, const double time, 
      const double r)
{
   if (time==0.0)
      return 0.0;
   
   return r*r*model.mu()/(4.0*model.bigT()*time);
}



double SuperPos::calculateW(const ModelInformation & model, const double time, 
      const double r)
{
   double u = calculateU(model, time, r);
   if(u==0.0)
      return 0.0;
/*   if(u<0.05)
   {
      return log(2.25*model.bigT()*time/(r*r*model.mu()));
   }*/
   else if(u>5.0 && u<=7.0)
      return (1-0.8/u)*(exp(-u)/u);
   else if(u>7.0)
      return 0.0;
   double w=-0.5772-log(u);
   int i=1;
   while(true){
      double add=std::pow(u,i)/(i*factorial(i))-std::pow(u,i+1)/((i+1)*factorial(i+1));
      w+=add;

      if(fabs(add)<0.00000001 && add!=0.0 && !(fabs(u-4)<1e-8 && i==1))
         break;
      i+=2;      
   }
   return w;
}

double SuperPos::calculateS(const double Q0y, const Point2 & Q1, 
      const ModelInformation & model, const double time, const double r)
{
   double s;
   if(time<Q1._x)
      return 0.0;
   else
   {
      s=0.001*(Q1._y-Q0y)/(4*pi*model.bigT())*calculateW(model,time-Q1._x,r);
   }
   return s;


 /*  double s=Q0.y()/(4*pi*model.bigT())*
      (calculateW(model, time-Q0.x(), r)-calculateW(model, Q1.x()-time, r));
  // std::cout<<" jer je "<<4*pi*model.bigT()<<" "<<calculateW(model, time-Q0.x(), r)<<"-"<<calculateW(model, Q1.x()-time, r)<<"\n";
   return s;*/
}

double SuperPos::timeValue(const double timestep, const std::vector<Point2> & q)
{//returns q(t)
   for(size_t iq=0;iq<q.size();iq++)
   {
      if(timestep<=q[iq]._x) 
         return q[iq-1]._y;
   }
   return q.back()._y;
}

double SuperPos::linearLoss(const double timestep, 
      const std::vector<Point2> & q, const double A)
{
   double Q=timeValue(timestep, q);
   return 0.001*Q*A;
}

double SuperPos::quadraticLoss(const double timestep, 
      const std::vector<Point2> & q, const double B)
{
   double Q=timeValue(timestep, q);
   return 0.000001*Q*Q*B;
}

double SuperPos::calculateInTimePoint(const std::vector<Point2> & q, 
      const ModelInformation & model, const double timeStep, const double rho)
{
   double s=0;
   s+=calculateS(0.0, q[0], model,
            timeStep, rho);

//   #pragma omp parallel for
   for(size_t iq=1;iq<q.size();iq++)
   {
      s+=calculateS(q[iq-1]._y, q[iq], model,
            timeStep, rho);     
   }
   return s;
}

bool SuperPos::hasValue (const std::vector<Point2> vec, const double value,
      size_t & i)
{
   for(size_t j=0;j<vec.size();j++)
   {
      if(vec[j]._x==value)
      {
         i=j;
         return true;
      }else if(vec[j]._x>value)
         break;
   }
   return false;
}

QVector<double> SuperPos::lossesInWell(const QVector<double> times, 
      const Well & well)
{
   QVector<double> losses(times.size(), 0.0);
  // #pragma omp parallel for
   for(int it=0;it<times.size();it++)
   {
      losses[it] = 
         linearLoss(times[it], well.q(), timeValue(times[it], well.bigA()))
         +quadraticLoss(times[it], well.q(), timeValue(times[it], well.bigB()));
   }
   return losses;
}

QVector<double> SuperPos::calculateInPoint(const std::vector<Well> & wells, 
      const std::vector<FictWell> & fictiveWells,
      const ModelInformation & model, const QVector<double> times, 
      const Point2 & point, 
      const size_t jwell/*=std::numeric_limits<size_t>::max()*/)
{//returns s without losses 
   QVector<double> ss(times.size());
 /*  if(jwell!=std::numeric_limits<size_t>::max())
      losses.resize(times.size());*/
   size_t nwells(wells.size());
   size_t nFicWells(fictiveWells.size());
   std::vector<double> dist(nwells);
   std::vector<double> ficDist(nFicWells);
   for(size_t iwell=0;iwell<nwells;iwell++)
      dist[iwell]=(point-wells[iwell].pos()).two_norm();
   for(size_t ifwell=0;ifwell<nFicWells;ifwell++)
   {
      ficDist[ifwell]=(point-fictiveWells[ifwell].pos()).two_norm();
   }
   
  // #pragma omp parallel for
   for(int it=0;it<times.size();it++)
   {
      double timeStep=times[it];
      double s=0.0;
    //  #pragma omp parallel for
      for(size_t iwell=0;iwell<nwells;iwell++)
      {     
         if(iwell==jwell || dist[iwell]<wells[iwell].radius())
         {
            s+=calculateInTimePoint(wells[iwell].q(), model, timeStep,
                  wells[iwell].radius());
      /*      loss+=linearLoss(timeStep, wells[jwell].q(), 
                  timeValue(timeStep, wells[jwell].bigA()));
            loss+=quadraticLoss(timeStep, wells[jwell].q(), 
                  timeValue(timeStep, wells[jwell].bigB()));      */
         }
         else
         {
            s+=calculateInTimePoint(wells[iwell].q(), model, timeStep,
                  dist[iwell]);
         }
      }
      //#pragma omp parallel for
      for(size_t ifwell=0;ifwell<nFicWells;ifwell++)
      {  
         s+=calculateInTimePoint(fictiveWells[ifwell].q(), model, timeStep, ficDist[ifwell]);
      }
      ss[it]=s;
      //losses[it]=loss;
   }
   return ss;

}
