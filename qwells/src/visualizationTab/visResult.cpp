#include "visResult.h"
#include "engine/data.h"
#include "qWells.h"
#include <QDebug>
#include <QProgressDialog>
#include <QMessageBox>
VisResult::VisResult(Data * data):_data(data)
{}

void VisResult::calculateResult(const double xmin, const double xmax, 
           const double ymin, const double ymax, const int xdiv, const int ydiv)
{
   _xmin=xmin;
   _xmax=xmax;
   _ymin=ymin;
   _ymax=ymax;
   _xdiv=xdiv;
   _ydiv=ydiv;
   _dx=(xmax-xmin)/xdiv;
   _dy=(ymax-ymin)/ydiv;
   _result.resize(xdiv+1);
   _minVal = std::numeric_limits<double>::max();
   _maxVal = -std::numeric_limits<double>::max();


    QProgressDialog progress("Calculating...", "Cancel", 0, (xdiv+1));
    progress.setWindowModality(Qt::ApplicationModal);
    progress.setMinimumDuration(0);


   auto boundary=_data->getBoundary();
   std::vector<Line> lines(boundary.nlines());
   Point2 wpos(_data->getWell(boundary.refWell()).pos());
   std::vector<double> wl(boundary.nlines());
   for(size_t i=0;i<boundary.nlines();i++)
   {
      if(i>=2)
         lines[i].constructLine(boundary.alpha()+pi/2, boundary.rho(i),
            _data->getWell(boundary.refWell()).pos());
      else if(boundary.type()==3 && i==1)//angle
         lines[i].constructLine(boundary.alpha()+boundary.beta(), boundary.rho(i),
            _data->getWell(boundary.refWell()).pos());
      else
         lines[i].constructLine(boundary.alpha(), boundary.rho(i),
            _data->getWell(boundary.refWell()).pos()); 
      wl[i] = lines[i].a()*wpos._x+lines[i].b()*wpos._y+lines[i].c();
   }
  // size_t nthreads=omp_get_max_threads();
  // #pragma omp parallel for
   for(int i=0;i<=xdiv;i++)
   {
    //  if(omp_get_thread_num()==0)
    //  {       
         progress.setValue(i/**nthreads*/);
    //  }
      _result[i].resize(ydiv+1);
//      #pragma omp parallel for
      for(int j=0;j<=ydiv;j++)
      {
         _result[i][j]=SuperPos::calculateInPoint(_data->wells(), 
         _data->fictiveWells(), _data->getModel(), _data->t(), 
         Point2 (xmin+_dx*i,ymin+_dy*j));
         if(!inDomain(Point2 (xmin+_dx*i,ymin+_dy*j), lines, wl))
            continue;
  //       #pragma omp parallel for
         for(int k=0;k<_result[i][j].size();k++)
         {
            if(_result[i][j][k]>_maxVal)
               _maxVal =_result[i][j][k];
            if(_result[i][j][k]<_minVal)
               _minVal =_result[i][j][k]; 
         }
      }
   }
/*   QString text = "FINISHED !!!";
   QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("ERROR"),text,
                                            QMessageBox::Ok);
   newError->exec();*/

   progress.setValue((xdiv+1));

}

bool VisResult::inDomain(const Point2 & p0, const std::vector<Line> & lines, 
      const std::vector<double> & wl)
{
   for(size_t i=0;i<lines.size();i++)
   {
      double p0l = lines[i].a()*p0._x+lines[i].b()*p0._y+lines[i].c();
      if((p0l>0 && wl[i]<0) || (p0l<0 && wl[i]>0))
         return false;
   }
   return true;
}

bool VisResult::inRealDomain(Point2 & p0, Point2 & p1,
      const std::vector<Line> & lines, const std::vector<double> & wl) const
{
   if(_data->getBoundary().type()==0)
      return true;
   Point2 ptemp;
   for(size_t i=0;i<lines.size();i++)
   {
      bool p0in=true;
      bool p1in=true;
      double p0l = lines[i].a()*p0._x+lines[i].b()*p0._y+lines[i].c();
      double p1l = lines[i].a()*p1._x+lines[i].b()*p1._y+lines[i].c();

      p0l=(fabs(p0l)<1e-8 ? 0.0 : p0l);
      p1l=(fabs(p1l)<1e-8 ? 0.0 : p1l);

      if((p0l>0 && wl[i]<0) || (p0l<0 && wl[i]>0))
         p0in=false;//p0 ne pripada
      if((p1l>0 && wl[i]<0) || (p1l<0 && wl[i]>0))
         p1in=false;//p1 ne pripada
      //p and w are on the opposite sides of the line
      if(!p0in && !p1in)
         return false;
      else if(p0in && !p1in){
         Line line;
         line.constructLinePoints(p0,p1);
         ptemp=intersection(lines[i], line);
         //value1=valueInterp(p0,p1,ptemp, value0, value1);
         p1=ptemp;         
      }else if(!p0in && p1in){
         Line line;
         line.constructLinePoints(p0,p1);
         ptemp=intersection(lines[i], line);
         //value0=valueInterp(p0,p1,ptemp, value0, value1);
         p0=ptemp;
      }//else if(p0in && p1in){to je ok
      //}
   }
   return true;
}

void VisResult::calcMarchingSquares(const QVector<double> & listNumbers)
{
   auto boundary=_data->getBoundary();
   std::vector<Line> lines(boundary.nlines());
   Point2 wpos(_data->getWell(boundary.refWell()).pos());
   std::vector<double> wl(boundary.nlines());
   for(size_t i=0;i<boundary.nlines();i++)
   {
      if(i>=2)
         lines[i].constructLine(boundary.alpha()+pi/2, boundary.rho(i),
            _data->getWell(boundary.refWell()).pos());
      else if(boundary.type()==3 && i==1)//angle
         lines[i].constructLine(boundary.alpha()+boundary.beta(), boundary.rho(i),
            _data->getWell(boundary.refWell()).pos());
      else
         lines[i].constructLine(boundary.alpha(), boundary.rho(i),
            _data->getWell(boundary.refWell()).pos()); 
      wl[i] = lines[i].a()*wpos._x+lines[i].b()*wpos._y+lines[i].c();
   }
   const int nisol=listNumbers.size();
   const int ntimes=_data->t().size();
   const double dmax=std::numeric_limits<double>::max();
  
   _lines.resize(ntimes);
   QVector<QVector<QVector<Point2> > > verEdgesPoint(ntimes);
   QVector<QVector<QVector<Point2> > > horEdgesPoint(ntimes);

  // size_t nthreads=omp_get_max_threads();
//#pragma omp parallel for
   for(int it=0;it<ntimes;it++)
   {
      _lines[it].resize(nisol);
      verEdgesPoint[it].resize(_ydiv*(_xdiv+1));
      horEdgesPoint[it].resize(_xdiv*(_ydiv+1));
      for(int iver=0;iver<(_ydiv*(_xdiv+1));iver++)
      {
         int xcount=iver%(_xdiv+1);
         int ycount=iver/(_xdiv+1);
         Point2 p0(_xmin+xcount*_dx, _ymin+ycount*_dy);
         Point2 p1(_xmin+xcount*_dx, _ymin+(ycount+1)*_dy);
         double value0=_result[xcount][ycount][it];
         double value1=_result[xcount][ycount+1][it];
         verEdgesPoint[it][iver]= QVector<Point2> (nisol, Point2(dmax,dmax));
         bool lineInRealDomain=inRealDomain(p0, p1, lines, wl);
         if(!lineInRealDomain)
            continue;
         for(int iisol=0;iisol<nisol;iisol++)
         {
            if((listNumbers[iisol]>=value0 && listNumbers[iisol]<=value1) ||
              (listNumbers[iisol]<=value0 && listNumbers[iisol]>=value1))
               verEdgesPoint[it][iver][iisol] = linearInterp(p0, p1, value0,
                     value1, listNumbers[iisol]);
         }
      }
      //#pragma omp parallel for
      for(int ihor=0;ihor<(_xdiv*(_ydiv+1));ihor++)
      {
         int xcount=ihor%_xdiv;
         int ycount=ihor/_xdiv;
         Point2 p0(_xmin+xcount*_dx, _ymin+ycount*_dy);
         Point2 p1(_xmin+(xcount+1)*_dx, _ymin+ycount*_dy);
         double value0=_result[xcount][ycount][it];
         double value1=_result[xcount+1][ycount][it];
         horEdgesPoint[it][ihor]=QVector<Point2>(nisol, Point2(dmax, dmax));
         bool lineInRealDomain=inRealDomain(p0, p1, lines, wl);
         if(!lineInRealDomain)
            continue;

         for(int iisol=0;iisol<nisol;iisol++)
         {
            if((listNumbers[iisol]>=value0 && listNumbers[iisol]<=value1) ||
              (listNumbers[iisol]<=value0 && listNumbers[iisol]>=value1))
               horEdgesPoint[it][ihor][iisol] = linearInterp(p0, p1, value0,
                     value1, listNumbers[iisol]);
         }
      }
      //#pragma omp parallel for
      for(int iisol=0;iisol<nisol;iisol++)
      {
         for(int iy=0;iy<_ydiv;iy++)
         {
            for(int ix=0;ix<_xdiv;ix++)
            {//za svaku isoliniju prolazimo kroz sve kvadrate
               QVector<Point2> points;
               if(horEdgesPoint[it][ix+iy*_xdiv][iisol]._x!=dmax)//gornja hor
                  points.push_back(horEdgesPoint[it][ix+iy*_xdiv][iisol]);
               if(horEdgesPoint[it][ix+(iy+1)*_xdiv][iisol]._x!=dmax)//donja h
                  points.push_back(horEdgesPoint[it][ix+(iy+1)*_xdiv][iisol]);
               if(verEdgesPoint[it][ix+(iy*(_xdiv+1))][iisol]._x!=dmax)//leva v
                  points.push_back(verEdgesPoint[it][ix+(iy*(_xdiv+1))][iisol]);
               if(verEdgesPoint[it][ix+(iy*(_xdiv+1))+1][iisol]._x!=dmax)      
                points.push_back(verEdgesPoint[it][ix+(iy*(_xdiv+1))+1][iisol]);
               if(points.size()==2)
                  _lines[it][iisol].push_back(Line2D(points[0], points[1]));
               else if(points.size()==4)
               {
                  _lines[it][iisol].push_back(Line2D(points[0], points[2]));
                  _lines[it][iisol].push_back(Line2D(points[1], points[3]));
               }
            }
         }
      }
   }

}

Point2 VisResult::calcOtherPoint(const Point2 & startPoint, 
      double & singleTime, int & xp, int & yp, 
      const double K, const double porosity, const int timeStep, 
      const std::vector<Line> & lines, const std::vector<double> & wl)
{
   if(xp==-1 && yp==-1)
   {
      xp=std::round((startPoint._x-_xmin)/_dx);//because this mesh is dual
      yp=std::round((startPoint._y-_ymin)/_dy);
   }

   if(xp<=0 || yp<=0 || xp>=_xdiv || yp>=_ydiv)
   {
      throw 1;
   }

   //for speed one minus exist, one is because we use depresions and one because we use reverse particle tracking. Therefore, we have one minus sign
   double vx1=-K*(_result[xp][yp][timeStep]-_result[xp-1][yp][timeStep]) /
      (porosity*_dx);
   double vx2=-K*(_result[xp+1][yp][timeStep]-_result[xp][yp][timeStep]) /
      (porosity*_dx);
   double vy1=-K*(_result[xp][yp][timeStep]-_result[xp][yp-1][timeStep]) /
      (porosity*_dy);
   double vy2=-K*(_result[xp][yp+1][timeStep]-_result[xp][yp][timeStep]) /
      (porosity*_dy);


   double x1=_xmin+xp*_dx-_dx/2;
   double y1=_ymin+yp*_dy-_dy/2;
   double Ax=0, Ay=0, vxp=0, vyp=0;

   int addxp=0;
   int addyp=0;

   if(fabs(vx1)<1e-14 && fabs(vx2)<1e-14 && fabs(vy1)<1e-14 && fabs(vy2)<1e-14)
   {//all velocities are zero
      throw 2;
   }

   if(fabs((vx1-vx2)/vx1)<1e-7)
   {//vx1 is equal to vx2
      double t=((vx1>0.0 ? x1+_dx : x1) -startPoint._x)/vx1;
      if(singleTime>t && t>0.0)
      {
         singleTime=t;
         addxp=(vx1>0.0) ? 1 : -1;
         addyp=0;
      }
   }
   else{
      Ax=(vx2-vx1)/_dx;
      vxp=Ax*(startPoint._x - x1)+vx1;
      if(vx1/vxp>0.0 && vx1<0.0)
      {
         double t=(1.0/Ax)*log(vx1/vxp);
         if(singleTime>t && t>0.0)
         {
            singleTime=t;
            addxp=-1;
            addyp=0;
         }
      }
      if(vx2/vxp>0.0 && vx2>0.0)
      {
         double t=(1.0/Ax)*log(vx2/vxp);
         if(singleTime>t && t>0.0)
         {
            singleTime=t;
            addxp=1;
            addyp=0;
         }
      }
   }
   if(fabs((vy1-vy2)/vy1)<1e-7)
   {
      double t=((vy1>0.0 ? y1+_dy : y1)-startPoint._y)/vy1;
      if(singleTime>t && t>0.0)
      {
         singleTime=t;
         addyp=(vy1>0.0) ? 1 : -1;
         addxp=0;
      }
   }
   else{
      Ay=(vy2-vy1)/_dy;
      vyp=Ay*(startPoint._y - y1)+vy1;
      if(vy1/vyp>0.0 && vy1<0.0)
      {
         double t=(1.0/Ay)*log(vy1/vyp);
         if(singleTime>t && t>0.0)
         {
            singleTime=t;
            addxp=0;
            addyp=-1;
         }
      }
      if(vy2/vyp>0.0 && vy2>0.0)
      {
         double t=(1.0/Ay)*log(vy2/vyp);
         if(singleTime>t && t>0.0)
         {
            singleTime=t;
            addxp=0;
            addyp=1;
         }
      }
   }
   if(vx1>0.0 && vx2<0.0 && vy1>0.0 && vy2<0.0)
      throw 3;
   if(singleTime==std::numeric_limits<double>::max())
   {
      throw 5;
   }

   
   double xe, ye;
   if(fabs((vx1-vx2)/vx1)<1e-7)
      xe=startPoint._x+vx1*singleTime;
   else
      xe=x1+(1/Ax)*(vxp*exp(Ax*singleTime)-vx1);
   if(fabs((vy1-vy2)/vy1)<1e-7)
      ye=startPoint._y+vy1*singleTime;
   else
      ye=y1+(1/Ay)*(vyp*exp(Ay*singleTime)-vy1);


   xp+=addxp;
   yp+=addyp;

   Point2 other=Point2(xe,ye);
   if(!inDomain(other, lines, wl))
   {
      throw 4;
   }


   return Point2(xe, ye);

}

void VisResult::calcStreamlines(const int i, const int npoints, const double K, 
      const double porosity, const int timeStep, const double timeLength, 
      const double timeSample)
{
   auto boundary=_data->getBoundary();
   std::vector<Line> lines(boundary.nlines());
   Point2 wpos(_data->getWell(boundary.refWell()).pos());
   std::vector<double> wl(boundary.nlines());
   for(size_t i=0;i<boundary.nlines();i++)
   {
      if(i>=2)
         lines[i].constructLine(boundary.alpha()+pi/2, boundary.rho(i),
            _data->getWell(boundary.refWell()).pos());
      else if(boundary.type()==3 && i==1)//angle
         lines[i].constructLine(boundary.alpha()+boundary.beta(), boundary.rho(i),
            _data->getWell(boundary.refWell()).pos());
      else
         lines[i].constructLine(boundary.alpha(), boundary.rho(i),
            _data->getWell(boundary.refWell()).pos()); 
      wl[i] = lines[i].a()*wpos._x+lines[i].b()*wpos._y+lines[i].c();
   }


   QVector<QVector<Line2D>> wellStreamLines;
   QVector<QVector<Point2>> wellStreamDots;
   for(int j=0;j<npoints;j++)
   {
      QVector<Line2D> streamLine;
      QVector<Point2> streamDot;
      double alpha=2*pi*j/npoints;
      Point2 startPoint;
      startPoint._x=_data->getWell(i).pos()._x+_data->getWell(i).radius() * 
         cos(alpha);
      startPoint._y=_data->getWell(i).pos()._y+_data->getWell(i).radius() *
         sin(alpha);

      double time=0.0;
      int xp=-1;
      int yp=-1;
      double oldTime=0.0;
      while(true)
      {
         double singleTime=std::numeric_limits<double>::max();
         Point2 oldPoint(startPoint);
         try {
            startPoint = calcOtherPoint(oldPoint, singleTime, xp, yp, 
               K, porosity, timeStep, lines, wl);
         }catch(int e)
         {
            QString str;
            if(e==1)
               str="Expand the area for calculation. Streamline "+QString::number(j)+" of the well "+_data->getWell(i).name()+" is not shown.";
            else if(e==2)
               str="Pore velocity is zero after time "+QString::number(time/_data->convertTimeUnit())+" for line "+QString::number(j)+" of the well "+_data->getWell(i).name()+", therefore this streamline is only until "+QString::number(time/_data->convertTimeUnit())+".";
            else if(e==3)
               str="Streamline "+QString::number(j)+" of the well "+_data->getWell(i).name()+" comes to a source after time "+QString::number(time/_data->convertTimeUnit())+".";
            else if(e==4)
               str="Boundary is found for line "+QString::number(j)+" of the well "+_data->getWell(i).name()+", therefore this streamline is only until "+QString::number(time/_data->convertTimeUnit())+".";
            else if(e==5)
               str="Streamline "+QString::number(j)+" of the well "+_data->getWell(i).name()+" comes to an end after "+QString::number(time/_data->convertTimeUnit())+".";
            else 
               str="Something is wrong !!! Please contact support. \n";
            _data->qWells()->error(str);
            break;
         }
        
         time+=singleTime;
         if(time>timeLength)
            break;
         
         for(int it=1;;it++)
         {//treba da nadjes tacke it*streamLineStep - oldTime , 
            if(it*timeSample-oldTime>singleTime)
            {
               oldTime=singleTime-((it-1)*timeSample-oldTime);
               break;
            }
            streamDot.push_back(linearInterp(oldPoint, startPoint, 0, singleTime, it*timeSample-oldTime));
         }


         streamLine.push_back(Line2D(oldPoint,startPoint));

         if(xp<=0 || yp<=0 || xp>=_xdiv || yp>=_ydiv)
         {
            _data->qWells()->error("Expand the area for calculation. Streamline "+QString::number(j)+" of the well "+_data->getWell(i).name()+" is shown until time "+QString::number(time/_data->convertTimeUnit()));
            break;
         }
      }
      wellStreamLines.push_back(streamLine);
      wellStreamDots.push_back(streamDot);
   }
   _streamLines.push_back(wellStreamLines);
   _streamDots.push_back(wellStreamDots);
}

Point2 VisResult::linearInterp(const Point2 & p0, const Point2 & p1, 
      const double value0, const double value1, const double value) const
{
   double alpha=(value-value0)/(value1-value0);
   return p0+alpha*(p1-p0);
}

double VisResult::valueInterp(const Point2 & p0, const Point2 & p1,
      const Point2 & pm, const double value0, const double value1) const
{
   double alpha=(pm-p0).two_norm()/(p1-p0).two_norm();
   return alpha*(value1-value0)+value0;
}

int VisResult::nIsolines()const
{
   return _lines.size()>0 ? _lines[0].size() : 0;
}

int VisResult::nlines(const int timestep, const int isol)const 
{
   return _lines[timestep][isol].size();
}

int VisResult::streamLinesNLines(const int i, const int j) const
{
   return _streamLines[i][j].size();
}

int VisResult::streamDotsNDots(const int i, const int j) const
{
   return _streamDots[i][j].size();
}

Point2 VisResult::line1P(const int timestep, const int isol, const int iline)
{
   return _lines[timestep][isol][iline].p1();
}

Point2 VisResult::line2P(const int timestep, const int isol, const int iline)
{
   return _lines[timestep][isol][iline].p2();
}

Point2 VisResult::line1PStream(const int i, const int j, const int k) 
{
   return _streamLines[i][j][k].p1();
}

Point2 VisResult::line2PStream(const int i, const int j, const int k) 
{
   return _streamLines[i][j][k].p2();
}

Point2 VisResult::dotStream(const int i, const int j, const int k) 
{
   return _streamDots[i][j][k];
}

void VisResult::clear()
{
   _lines.clear();
}

void VisResult::clearStreamLines()
{
   _streamLines.clear();
   _streamDots.clear();
}

bool VisResult::resultsExist() const
{
   return _result.size()>0;
}

double VisResult::interpolate(const double x, const double y, const int k) const
{

   size_t i = (int) ((x-_xmin)/_dx);
   size_t j = (int) ((y-_ymin)/_dy);

   if(fabs(_xmax-x)<1e-6)
      i--;
   if(fabs(_ymax-y)<1e-6)
      j--;

   double w1 = sqrt(pow(_xmin+_dx*i-x, 2) + pow(_ymin+_dy*j-y, 2));
   double w2 = sqrt(pow(_xmin+_dx*(i+1)-x, 2) + pow(_ymin+_dy*j-y, 2));
   double w3 = sqrt(pow(_xmin+_dx*i-x, 2) + pow(_ymin+_dy*(j+1)-y, 2));
   double w4 = sqrt(pow(_xmin+_dx*(i+1)-x, 2) + pow(_ymin+_dy*(j+1)-y, 2));
   if(w1<1e-4)
      return _result[i][j][k];
   else if(w2<1e-4)
      return _result[i+1][j][k];
   else if(w3<1e-4)
      return _result[i][j+1][k];
   else if(w4<1e-4)
      return _result[i+1][j+1][k];

   double sumw = 1/w1 + 1/w2 + 1/w3 + 1/w4;

   return 1/(w1*sumw)*_result[i][j][k] + 1/(w2*sumw)*_result[i+1][j][k]
      + 1/(w3*sumw)*_result[i][j+1][k] + 1/(w4*sumw)*_result[i+1][j+1][k];

   /*if(fabs((x-_xmin)/_dx-round((x-_xmin)/_dx))<0.05)
      qDebug()<<" to je to po x \n";
   if(fabs((y-_ymin)/_dy-round((y-_ymin)/_dy))<0.05)
      qDebug()<<" to je to po y \n";*/

   
   //return 0.0;
}

bool VisResult::inDomainOfCalc(const double x, const double y) const
{
   if(resultsExist() && x>_xmin && x<_xmax && y>_ymin && y<_ymax)
      return true;
   return false;
}
