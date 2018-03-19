#include "engine/well.h"
#include "engine/modelInformation.h"
#include "engine/boundary.h"
#include "engine/superPos.h"
#include "helperFunctions.h"
#include "interactionTab/tabbedTools/chooserTab/wellChooserFrame.h"
#include "interactionTab/tabbedTools/chooserTab/piezoChooserFrame.h"

#include <QString>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>

namespace io
{

inline void checkLoadError(const QString & errMes, const std::vector<double> t, 
         const double value1, const double divide)
{
   for(size_t it=0;it<t.size();it++)
   {
      if(t[it]==value1)
      {//errMes is the error message that should be thrown  
         QString err=errMes;
         err.append(QString(" at time %1").arg(value1/divide));
         throw err;
      }
   }
}

inline void exportWell (const QString & fileN, const Well & well, 
      const size_t & timeUnit)
{
   std::string fileName;
   conversion::convertQStringToString(fileN, fileName);
   std::fstream in;    // Create an output file stream.
   const char* fname=fileName.c_str();

   in.open(fname, std::fstream::out | std::ios::trunc);  // Use it to write to a file.
   if ( !in.is_open() ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for writing!");
      throw errMes;
   }
   in.precision(11);

   double divide=(timeUnit==0 ? 1.0 : 
         (timeUnit==1 ? 60.0 : (timeUnit==2 ? 3600.0 : 86400)));
   
   in<<"Well name: \n";
   std::string nameStr;
   conversion::convertQStringToString(well.name(),nameStr);
   in<<nameStr<<"\n";
   in<<"Position: \n";
   in<<well.pos()._x<<", "<<well.pos()._y<<"\n";
   in<<"Radius: \n";
   in<<well.radius()<<"\n";
   
   in<<"Time, Discharge rate: \n";
   size_t nq=well.nq();      
   for(size_t iq=0;iq<nq;iq++)
   {
      in<<well.qi(iq)._x/divide<<", "<<well.qi(iq)._y<<"\n";
   }
   in<<"Time, Measured drawdown: \n";
   size_t ns=well.nS();
   for(size_t is=0;is<ns;is++)
   {
      in<<well.Smi(is)._x/divide<<", "<<well.Smi(is)._y<<"\n";
   }
   in<<"Time, Linear loss parameter: \n";
   size_t nA=well.nA();
   for(size_t ia=0;ia<nA;ia++)
   {
      in<<well.bigA(ia)._x/divide<<", "<<well.bigA(ia)._y<<"\n";
   }
   in<<"Time, Quadratic loss parameter: \n";
   size_t nB=well.nB();
   for(size_t ib=0;ib<nB;ib++)
   {
      in<<well.bigB(ib)._x/divide<<", "<<well.bigB(ib)._y<<"\n";
   }
   in.close();   
}

inline Well importWell (const QString & fileN, const size_t timeUnit)
{
   std::string fileName;
   char s;
   conversion::convertQStringToString(fileN, fileName);

   Well well;
   std::ifstream in;    // Create an input file stream.
   in.open(fileName.c_str(),std::ifstream::in );  // Use it to read from a file.
   if ( ! in ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for reading!");
      throw errMes;
   }
   std::string str;
   double value1, value2;
   double multiply=(timeUnit==0 ? 1.0 : 
         (timeUnit==1 ? 60.0 : (timeUnit==2 ? 3600.0 : 86400)));
   getline(in, str);//"Well name: \n";
   std::string name;
   getline(in, name);
   if(!(name.find("Position: ")!=std::string::npos))
   {//there name for this well
      if(name[name.length() - 1]=='\n')
         name.resize(name.length()-1);
      if(name[name.length() - 1]==',')//because of writing 
         name.resize(name.length()-1);
      QString nameQStr;
      conversion::convertStringToQString(name,nameQStr);
      well.setName(nameQStr);
      getline(in,str);//"Position: \n";
   }//else go to Position
   in>>value1; in>>s; in>>value2;
   well.setPos(Point2 (value1, value2));
   getline(in, str);//"\n";
   getline(in, str);//"Radius: \n";
   in>>value1;
   well.setRadius(value1);
   getline(in,str);//"\n";
   getline(in,str);//"Discharge rate: \n";
   std::vector<double> t;
   while(in.peek()!='T')
   {
      in>>value1; in>>s; in>>value2;

      checkLoadError("Error!!! Trying to import well with two same time values for discharge rate: ", t, value1*multiply, multiply);
      t.push_back(value1*multiply);

      well.addQ(Point2 (value1*multiply, value2));
      getline(in, str);//"\n";
   }
   getline(in, str);//"Measured drawdown: \n";
  /* size_t ns;
   in>>ns;
   getline(in, str);*/
   t.clear();
   while(in.peek()!='T')
   {
      in>>value1; in>>s; in>>value2;

      checkLoadError("Error!!! Trying to import well with two same time values for measured drawdown: ", t, value1*multiply, multiply);
      t.push_back(value1*multiply);

      well.addMeasuredS(Point2 (value1*multiply,value2));
      getline(in,str);//"\n";
   }
   getline(in,str);//"Linear loss parameter: \n";
   /*size_t nA;
   in>>nA;
   getline(in,str);//"\n";*/
   t.clear();
   while(in.peek()!='T')
   {
      in>>value1; in>>s; in>>value2;

      checkLoadError("Error!!! Trying to import well with two same time values for linear loss parameter: ", t, value1*multiply, multiply);      
      t.push_back(value1*multiply);

      well.addBigA(Point2 (value1*multiply,value2));
      getline(in,str);//"\n";
   }
   getline(in,str);//"Quadratic loss parameter: \n";
   /*size_t nB;
   in>>nB;
   getline(in,str);//"\n";*/
   t.clear();
   int c=in.peek();
   while(c!=EOF)
   {
      in>>value1; in>>s; in>>value2;

      checkLoadError("Error!!! Trying to import well with two same time values for quadratic loss parameter: ", t, value1*multiply, multiply);
      t.push_back(value1*multiply);

      well.addBigB(Point2 (value1*multiply,value2));
      getline(in,str);//"\n";
      c=in.peek();
   }
   in.close();
   return well;
}

inline void exportPiez (const QString & fileN, const Piezometer & piez,
      const size_t timeUnit)
{
   std::string fileName;
   conversion::convertQStringToString(fileN, fileName);
   std::fstream in;    // Create an output file stream.
   const char* fname=fileName.c_str();

   in.open(fname, std::fstream::out | std::ios::trunc);  // Use it to write to a file.
   if ( !in.is_open() ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for writing!");
      throw errMes;
   }

   in.precision(11);

   double divide=(timeUnit==0 ? 1.0 : 
         (timeUnit==1 ? 60.0 : (timeUnit==2 ? 3600.0 : 86400)));
   
   in<<"Piezometer name: \n";
   std::string nameStr;
   conversion::convertQStringToString(piez.name(),nameStr);
   in<<nameStr<<"\n";
   in<<"Position: \n";
   in<<piez.pos()._x<<", "<<piez.pos()._y<<"\n";
   
   in<<"Time, Measured drawdown: \n";
   size_t ns=piez.nS();
   for(size_t is=0;is<ns;is++)
   {
      in<<piez.Smi(is)._x/divide<<", "<<piez.Smi(is)._y<<"\n";
   }
   in.close();   
}

inline Piezometer importPiez (const QString & fileN, const size_t timeUnit)
{
   std::string fileName;
   conversion::convertQStringToString(fileN, fileName);
   Piezometer piez;
   std::ifstream in;    // Create an input file stream.
   in.open(fileName.c_str(),std::ifstream::in );  // Use it to read from a file.
   if ( ! in ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for reading!");
      throw errMes;
   }
   double multiply=(timeUnit==0 ? 1.0 : 
         (timeUnit==1 ? 60.0 : (timeUnit==2 ? 3600.0 : 86400)));
   std::string str;
   double value1, value2;
   getline(in, str);//"Piezometer name: \n";
   std::string name;
   getline(in, name);
   if(!(name.find("Position: ")!=std::string::npos))
   {//there name for this piez
      if(name[name.length() - 1]=='\n')
         name.resize(name.length()-1);
      QString nameQStr;
      conversion::convertStringToQString(name,nameQStr);
      piez.setName(nameQStr);
      getline(in,str);//"Position: \n";
   }//else go to Position
   char s;
   in>>value1; in>>s;  in>>value2;
   piez.setPos(Point2 (value1, value2));
   getline(in, str);//"\n";
  
   getline(in, str);//"Measured drawdown: \n";
   int c=in.peek();
   std::vector<double> t;
   while(c!=EOF)
   {
      in>>value1; in>>s; in>>value2;
      checkLoadError("Error!!! Trying to import piezometer with two same time values for Sm: ", t, value1*multiply, multiply);
      t.push_back(value1*multiply);
      piez.addMeasuredS(Point2 (value1*multiply,value2));
      getline(in,str);
      c=in.peek();
   }
   in.close();

   return piez;
}

inline void readDat (const QString & fileN, std::vector<Well> & wells,
      std::vector<Piezometer> & piez, ModelInformation & model, 
      Boundary & boundary, QVector<double> & times)
{
   std::string fileName;
   conversion::convertQStringToString(fileN, fileName);

   std::ifstream in;    // Create an input file stream.
   in.open(fileName.c_str(),std::ifstream::in);  // Use it to read from a file.
   if ( ! in ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for reading!");
      throw errMes;
   }
   double value;
   std::string str;
   bool day;//true for days, false for hours
   getline(in,str);// name
   in>>day;
   if(day)
      model.setTimeUnit(3);
   else
      model.setTimeUnit(2);
   getline(in, str); //one more parameter, something about output
   //finished 3. and 4.
   //
   size_t ntime;
   in>>ntime;
   getline(in,str);//finished 5

   size_t nrows=ceil(ntime/10.0);
   for(size_t irow=0;irow<nrows;irow++)
   {
      for(size_t i=0;i<10;i++)//because there is 10 in line
      {
         in>>value;
         if(!day)
            value*=3600;
         else
            value*=86400;
         times.push_back(value);
         if(irow*10+i+1==ntime)
         {
            break;
         }
      }
      getline(in,str);
   }
   //finished 5.1
   //
   in>>value;
   model.setBigT(value);
   in>>value;
   model.setMu(value);
   getline(in,str);//finished 6

   size_t nwells;
   in>>nwells;
   wells.resize(nwells);
   size_t npiez;
   in>>npiez;
   piez.resize(npiez);
   
   //finished 7 and 8

   for(size_t iwell=0;iwell<nwells;iwell++)
   {//for every well
      Point2 p;
      in>>p._x; in>>p._y;
      wells[iwell].setPos(p);
      in>>value;
      wells[iwell].setRadius(value);
      in>>value;
      wells[iwell].addBigA(Point2(0.0, value));
      in>>value;
      wells[iwell].addBigB(Point2(0.0, value));
      getline(in,str);
      size_t nq;
      in>>nq;
      getline(in,str);
      for(size_t iq=0;iq<nq;iq++)
      {
         Point2 q;
         in>>q._x; in>>q._y;
         q._x*=(!day ? 3600 :86400);
         //q._y/=1000;
         wells[iwell].addQ(q);
         getline(in,str);
      }
   }//finished 9.

   for(size_t ipiez=0;ipiez<npiez;ipiez++)
   {
      Point2 p; 
      in>>p._x;  in>>p._y;
      piez[ipiez].setPos(p);
   }
   getline(in,str);
   //finished 10.1
   size_t np;
   in>>np;
   getline(in,str);
   for(size_t ip=0;ip<np;ip++)
   {
      Point2 p;
      in>>p._x; in>>p._y;
      QString nameQStr;
      std::string nameStr;
      in>>nameStr;
      conversion::convertStringToQString(nameStr,nameQStr);
      size_t jpiez=std::numeric_limits<size_t>::max();
      size_t jwell=std::numeric_limits<size_t>::max();
      for(size_t ipiez=0;ipiez<npiez;ipiez++)
         if(p==piez[ipiez].pos())
            jpiez=ipiez;
      for(size_t iwell=0;iwell<nwells;iwell++)
         if(p==wells[iwell].pos())
            jwell=iwell;

      if(jpiez!=std::numeric_limits<size_t>::max() &&
            jwell == std::numeric_limits<size_t>::max())
      {//this is piezometer     
         piez[jpiez].setName(nameQStr);    
         for(size_t irow=0;irow<nrows;irow++)
         {
            for(size_t i=0;i<10;i++)//because there is 10 in line
            {              
               in>>value;
               if(value!=999.00)
                  piez[jpiez].addMeasuredS(Point2 (times[irow*10+i], value));
               if(irow*10+i+1==ntime)
                  break;
            }
            getline(in,str);
         }
      }else if(jpiez==std::numeric_limits<size_t>::max() &&
            jwell != std::numeric_limits<size_t>::max())
      {//this is well
         wells[jwell].setName(nameQStr);
         for(size_t irow=0;irow<nrows;irow++)
         {
            for(size_t i=0;i<10;i++)//because there is 10 in line
            {              
               in>>value;
               if(value!=999.00)
                  wells[jwell].addMeasuredS(Point2 (times[irow*10+i], value));
               if(irow*10+i+1==ntime)
                  break;
            }
            getline(in,str);
         }
      }else{
         QString err("There is no well nor piezometer in ");
         err.append(QString(" %1").arg(p._x));
         err.append(QString(" %1").arg(p._y));
         throw err;
      }
   }//finished 10.2
   
   size_t index;
   in>>index;
   if(index<4)
      boundary.setType(index);
   if(index==3)
   {
      boundary.setBeta(90.0);
   }
   else if(index==4)//because this code coudn't work with 3 lines boundary
      boundary.setType(index+1);
   getline(in,str);
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      if(fabs(wells[iwell].pos()._x)<1e-12 && 
            fabs(wells[iwell].pos()._y)<1e-12)
      {
         if(boundary.type()!=0)
            boundary.setRefWell(iwell);
         break;
      }else if(iwell==nwells-1)
      {
         std::cerr<<" ERROR!!! Did not find referent well. There is no well in "<<0.0<<";"<<0.0<<"\n";
      }
   }
   in>>index;
   if(index==0)
   {//apscisa
      boundary.setAlpha(90.0);                  
   }else
      boundary.setAlpha(180.0);
   for(size_t iline=0;iline<boundary.nlines();iline++)
   {//there is two numbers for each line x or y-axis and distance      
      bool t;
      in>>t;
      boundary.setLineType(iline,t);
      in>>value;
      boundary.setRho(iline,value);
      if(iline%2==1)
      {
         in>>index;//koliko preslikavanja ima 
         boundary.setMaxDist(1000.0);
         /*if(iline==1)
            boundary.setnMaps1(index);
         else
            boundary.setnMaps2(index);*/
      }
   }
   in.close();
      
}

inline void writeOutput(const QString & fileN, const std::vector<Well> & wells, 
      const std::vector<FictWell> fictiveWells,
      const std::vector<Piezometer> & piez, const ModelInformation & model, 
      const QVector<double> & t, const QVector<double> & timeInSMHD)
{
   std::string fileName;
   conversion::convertQStringToString(fileN, fileName);
   const char* fname=fileName.c_str();
   FILE * pFile;
   pFile=fopen(fname,"w");
   std::fstream in (fname, std::ios::out|std::ios::trunc);    // Create an output file stream.

 //  in.open(fname, std::fstream::out | std::ios::trunc);  // Use it to write to a file.
   if ( !in.is_open() ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for writing!");
      throw errMes;
   }

   QVector<double> scalc(t.size());
   QVector<double> losses(t.size());
   for(size_t iwell=0;iwell<wells.size();iwell++)
   {
      std::string wellName; conversion::convertQStringToString(wells[iwell].name(),wellName);
      fprintf(pFile, " \n\n **************************************** \n");
      fprintf(pFile, " Well ---- %s in (%f;%f)\n", wellName.c_str(), wells[iwell].pos()._x, wells[iwell].pos()._y);
      fprintf(pFile, " **************************************** \n\n");
      fprintf(pFile, "    time       S        Wloss       STOT        Sm      STOT-Sm\n");  
      fprintf(pFile,   "   ------    ------    -------     ------     ------    -------\n");

      
      scalc=SuperPos::calculateInPoint(wells, fictiveWells, model, t,
            wells[iwell].pos(), iwell);
      losses=SuperPos::lossesInWell(t, wells[iwell]);
      for(int it=0;it<t.size();it++)
      {
         size_t j=0;
         if(SuperPos::hasValue(wells[iwell].Sm(), t[it], j)){
            fprintf(pFile, "%8.2f   %8.4f   %8.4f   %8.4f   %8.4f  %8.4f\n",
                  timeInSMHD[it], scalc[it], losses[it], scalc[it]+losses[it],
                  wells[iwell].Smi(j)._y, 
                  scalc[it]+losses[it]-wells[iwell].Smi(j)._y);
         }else
            fprintf(pFile, "%8.2f   %8.4f   %8.4f   %8.4f\n", timeInSMHD[it], scalc[it], losses[it], scalc[it]+losses[it]);
      }
        // std::cout<<"  "<<t[it]<<"   "<<scalc[it]<<"   "<<losses[it]<<"   "<<scalc[it]+losses[it]<<"\n";   
   }
   for(size_t ipiez=0;ipiez<piez.size();ipiez++)
   {
      std::string piezoName;
      conversion::convertQStringToString(piez[ipiez].name(),piezoName);
      fprintf(pFile, " \n\n **************************************** \n");
      fprintf(pFile, " Piezometer ---- %s in (%f;%f)\n", piezoName.c_str(), piez[ipiez].pos()._x, piez[ipiez].pos()._y);
      fprintf(pFile, " **************************************** \n\n");
      fprintf(pFile, "    time       S          Sm        S-Sm\n");  
      fprintf(pFile, "   ------    ------     ------     ------\n");
      scalc=SuperPos::calculateInPoint(wells, fictiveWells, model, t,
            piez[ipiez].pos());
      for(int it=0;it<t.size();it++)
      {
         size_t j=0;
         if(SuperPos::hasValue(piez[ipiez].Sm(), t[it], j))
            fprintf(pFile, "%8.2f   %8.4f   %8.4f   %8.4f\n", timeInSMHD[it],
                  scalc[it], piez[ipiez].Smi(j)._y, 
                  scalc[it]-piez[ipiez].Smi(j)._y);
         else
            fprintf(pFile, "%8.2f   %8.4f\n", timeInSMHD[it], scalc[it]);
      }
      fprintf(pFile,"\n");
   }
   fflush(pFile);
}

inline void saveFile (const QString & fileN, const std::vector<Well> & wells, 
      const std::vector<Piezometer> & piez, const ModelInformation & model, 
      const Boundary & boundary, 
      const std::vector<WellChooserFrame*> & wellFrames,
      const std::vector<PiezoChooserFrame*> & piezFrames, const int lineWidth)
{
   std::string fileName;
   conversion::convertQStringToString(fileN, fileName);
   std::fstream in;    // Create an output file stream.
   const char* fname=fileName.c_str();

   in.open(fname, std::fstream::out | std::ios::trunc);  // Use it to write to a file.
   if ( !in.is_open() ) {      
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for writing!");
      throw errMes;

   }
   in.precision(11);

   in<<"Picture path\n";
   in<<model.pictureName().toUtf8().constData()<<"\n";
   in<<"Picture Xmin, Ymin, Xmax, Ymax\n";
   in<<model.picXmin()<<" "<<model.picYmin()<<" "<<model.picXmax()<<" "<<model.picYmax()<<"\n";
   in<<"Picture: well offsets, radius, color and show\n";
   in<<model.wellXOffset()<<" "<<model.wellYOffset()<<" "<<model.wellRadius()
      <<" "<<model.wellColor()<<" "<<model.showWellNames()<<"\n";
   in<<"Picture: piez offsets, radius, color and show\n";
   in<<model.piezXOffset()<<" "<<model.piezYOffset()<<" "<<model.piezRadius()
      <<" "<<model.piezColor()<<" "<<model.showPiezNames()<<"\n";
   in<<"Picture: const color and imperm color\n";
   in<<model.constColor()<<" "<<model.impermColor()<<"\n";
   in<<"Number of well frames:\n";
   in<<wellFrames.size()<<"\n";
   for(size_t i=0;i<wellFrames.size();i++)
   {
      in<<"Frame:\n";
      in<<i<<"\n";
      in<<"Checked:\n";
      in<<wellFrames[i]->checkBox()->isChecked()<<"\n";
      in<<"With loss:\n";
      in<<wellFrames[i]->lossCheckBox()->isChecked()<<" ";
      in<<wellFrames[i]->lossLineStyle()->currentIndex()<<" ";
      in<<wellFrames[i]->lossLineColor()->currentIndex()<<"\n";
      in<<"Without:\n";
      in<<wellFrames[i]->noLossCheckBox()->isChecked()<<" ";
      in<<wellFrames[i]->noLossLineStyle()->currentIndex()<<" ";
      in<<wellFrames[i]->noLossLineColor()->currentIndex()<<"\n";
      in<<"Sm check:\n";
      in<<wellFrames[i]->smCheckBox()->isChecked()<<" ";
      in<<wellFrames[i]->smTickType()->currentIndex()<<"\n";
   }
   in<<"Number of piezo frames:\n";
   in<<piezFrames.size()<<"\n";
   for(size_t i=0;i<piezFrames.size();i++)
   {
      in<<"Frame:\n";
      in<<i<<"\n";
      in<<"Checked:\n";
      in<<piezFrames[i]->checkBox()->isChecked()<<"\n";
      in<<"Piezometer:\n";
      in<<piezFrames[i]->lineStyle()->currentIndex()<<" ";
      in<<piezFrames[i]->lineColor()->currentIndex()<<"\n";
      in<<"Sm check:\n";
      in<<piezFrames[i]->smCheckBox()->isChecked()<<" ";
      in<<piezFrames[i]->smTickType()->currentIndex()<<"\n";
   }
   in<<"Line width:\n";
   in<<lineWidth<<"\n";

   in<<"Time shown in: 0-seconds, 1-minutes, 2-hours, 3-days\n";
   in<<model.timeUnit()<<"\n";
   double divide=(model.timeUnit()==0 ? 1.0 : 
         (model.timeUnit()==1 ? 60.0 : (model.timeUnit()==2 ? 3600.0 : 86400)));
   
   in<<"Number of wells: \n";
   size_t nwells=wells.size();
   in<<nwells<<"\n";
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      std::string nameStr;
      conversion::convertQStringToString(wells[iwell].name(),nameStr);
      in<<"Well: \n";
      in<<nameStr<<"\n";
      in<<"Position: \n";
      in<<wells[iwell].pos()._x<<" "<<wells[iwell].pos()._y;in<<"\n";
      in<<"Radius: \n";
      in<<wells[iwell].radius()<<"\n";
      in<<"Discharge rate: \n";
      size_t nq=wells[iwell].nq();      
      in<<nq;
      in<<"\n";
      for(size_t iq=0;iq<nq;iq++)
      {
         in<<wells[iwell].qi(iq)._x/divide<<" "<<wells[iwell].qi(iq)._y<<"\n";
      }
      in<<"Measured drawdown: \n";
      size_t ns=wells[iwell].nS();
      in<<ns<<"\n";
      for(size_t is=0;is<ns;is++)
      {
         in<<wells[iwell].Smi(is)._x/divide<<" "<<wells[iwell].Smi(is)._y<<"\n";
      }
      in<<"Linear loss parameter: \n";
      size_t nA=wells[iwell].nA();
      in<<nA<<"\n";
      for(size_t ia=0;ia<nA;ia++)
      {
         in<<wells[iwell].bigA(ia)._x/divide<<" "<<wells[iwell].bigA(ia)._y<<"\n";
      }
      in<<"Quadratic loss parameter: \n";
      size_t nB=wells[iwell].nB();
      in<<nB<<"\n";
      for(size_t ib=0;ib<nB;ib++)
      {
         in<<wells[iwell].bigB(ib)._x/divide<<" "<<wells[iwell].bigB(ib)._y<<"\n";
      }
   }
   in<<"Number of piezometers: \n";
   size_t npiez=piez.size();
   in<<npiez<<"\n";
   for(size_t ipiez=0;ipiez<npiez;ipiez++)
   {
      std::string nameStr;
      conversion::convertQStringToString(piez[ipiez].name(),nameStr);
      in<<"Piezometer: \n";
      in<<nameStr<<"\n";
      in<<"Position: \n";
      in<<piez[ipiez].pos()._x<<" "<<piez[ipiez].pos()._y;in<<"\n";
      in<<"Measured drawdown: \n";
      size_t ns=piez[ipiez].nS();
      in<<ns<<"\n";
      for(size_t is=0;is<ns;is++)
      {
         in<<piez[ipiez].Smi(is)._x/divide<<" "<<piez[ipiez].Smi(is)._y<<"\n";
      }
   }
   size_t type=boundary.type();
   if(type!=std::numeric_limits<size_t>::max())
   {
      in<<"Boundary type:\n";
      in<<type<<"\n";
      size_t nlines=boundary.nlines();
      for(size_t iline=0;iline<nlines;iline++)   
      {
         if(iline==0)
         {
            if(boundary.refWell()!=std::numeric_limits<size_t>::max())
               in<<"Referent well:\n"<<boundary.refWell()<<"\n";
            if(boundary.alpha()!=std::numeric_limits<double>::max())
               in<<"Alpha:\n"<<boundary.alpha()*180/pi<<"\n";
            if(type==Enums::ANGLE && 
                  boundary.beta()!=std::numeric_limits<double>::max())
               in<<"Beta:\n"<<boundary.beta()*180/pi<<"\n";
            else if((type==Enums::PARALLEL_LINES || 
                  type==Enums::THREE_LINES || type==Enums::FOUR_LINES) &&
                  boundary.maxDistance()!=std::numeric_limits<double>::max())
            {
               in<<"Maximal distance:\n"<<boundary.maxDistance()<<"\n";
            }
         }
         if(boundary.rho(iline)!=std::numeric_limits<double>::max())
            in<<"Distance"<<iline<<":\n"<<boundary.rho(iline)<<"\n";
         
         in<<"Line type: 0 - constant potential, 1 - water impermeable:\n";
         if(boundary.lineType(iline))
            in<<"1\n";
         else
            in<<"0\n";
         
      }
   }
   if(model.bigT()!=std::numeric_limits<double>::max())
   {
      in<<"Transmissivity:\n";
      in<<model.bigT()<<"\n";
   }
   if(model.mu()!=std::numeric_limits<double>::max())
   {
      in<<"Specific storage:\n";
      in<<model.mu()<<"\n";
   }
   in<<"Domain of calculation:\n";
   in<<model.visXmin()<<" "<<model.visXmax()<<" "<<model.visYmin()<<" "<<model.visYmax()<<"\n";
   in<<"Division:\n";
   in<<model.visXdiv()<<" "<<model.visYdiv()<<"\n";
   in<<"Isoline values:\n";
   in<<model.isoValues().size()<<"\n";
   if(model.isoValues().size()!=0)
   {
      for(int i=0;i<model.isoValues().size();i++)
         in<<model.isoValues()[i]<<" ";
      in<<"\n";
   }
   in<<"Isoline: label incidence, play duration and color:\n";
   in<<model.isoLabelIncidence()<<" "<<model.isoPlayDuration()<<" "<<model.isoColor()<<"\n";
   in<<"Porosity:\n";
   in<<model.porosity()<<"\n";
   in<<"Layer thickness:\n";
   in<<model.thickness()<<"\n";
   in<<"Time length of streamlines:\n";
   in<<model.timeLength()<<"\n";
   in<<"Choosen time:\n";
   in<<model.timeStream()<<"\n";
   in<<"Number of streamlines per well:\n";
   in<<model.noStreamLines()<<"\n";
   in<<"Stream line color:\n";
   in<<model.streamLineColor()<<"\n";

   in.close();
}

inline void openFile (const QString & fileN, std::vector<Well> & wells, 
      std::vector<Piezometer> & piez, ModelInformation & model, 
      Boundary & boundary, int * lineWidth)
{
   std::string fileName;
   conversion::convertQStringToString(fileN, fileName);

   //deleting everything 
   wells.clear();
   piez.clear();
   //model.clear(); does not need to be cleared
   boundary.clear();
   //
   std::ifstream in;    // Create an input file stream.
   in.open(fileName.c_str(),std::ifstream::in );  // Use it to read from a file.
   if ( ! in ) {
      QString errMes("Error opening file ");
      errMes.append(fileN);
      errMes.append(" for reading!");
      throw errMes;
   }
   std::string str;
   double value1, value2;
   getline(in,str);//"Picture path\n";
   getline(in,str);
   model.setPictureName(QString::fromUtf8(str.c_str()));
   getline(in,str);//"Picture Xmin, Ymin, Xmax, Ymax\n";
   in>>value1;
   model.setPicXmin(value1);
   in>>value1;
   model.setPicYmin(value1);
   in>>value1;
   model.setPicXmax(value1);
   in>>value1;
   model.setPicYmax(value1);
   getline(in,str); 
   getline(in,str);//"Picture: well offsets, radius, color and show\n";

   if(str.find("Picture: well offsets, radius, color and show")!=std::string::npos)//else str is Time shown in ...
   {
      int wxo, wyo, wr, wc, pxo, pyo, pr, pc, cc, ic;
      bool swn, spn;
      in>>wxo>>wyo>>wr>>wc>>swn;
      getline(in,str); 
      getline(in,str);//"Picture: piez offsets, radius, color and show\n";getline(in,str); 
      in>>pxo>>pyo>>pr>>pc>>spn;

      getline(in,str); 
      getline(in,str); //in<<"Picture: const color and imperm color\n";
      in>>cc>>ic;
      
      model.setParamObjProp(wxo,wyo,wr,wc,swn,pxo,pyo,pr,pc,spn,cc,ic);
      getline(in,str);
      getline(in,str);//"Number of well frames" or "Time shown in: 0-seconds, 1-minutes, 2-hours, 3-days\n";
   }

   if(str.find("Number of well frames:")!=std::string::npos)
   {
      int nwellFrames;
      in>>nwellFrames;
      getline(in,str);
      model.resizePlotChWell(nwellFrames);
      for(int i=0;i<nwellFrames;i++)
      {
         getline(in,str);//Frame:\n
         getline(in,str);//actual number identical to i
         getline(in,str);//Checked:\n
         in>>model.plotChWell(i).wellChecked;
         getline(in,str);//\n
         getline(in,str);//With loss\n
         in>>model.plotChWell(i).withLossChecked>>model.plotChWell(i).withLossLine>>model.plotChWell(i).withLossColor;
         getline(in,str);//\n
         getline(in,str);//Without:\n
         in>>model.plotChWell(i).noLossChecked>>model.plotChWell(i).noLossLine>>model.plotChWell(i).noLossColor;
         getline(in,str);//\n
         getline(in,str);//Sm check:\n
         in>>model.plotChWell(i).smChecked>>model.plotChWell(i).smType;
         getline(in,str);//\n
      }
      getline(in,str);//"Number of piezo frames:\n"
      int npiezFrames;
      in>>npiezFrames;
      getline(in,str);
      model.resizePlotChPiez(npiezFrames);
      for(int i=0;i<npiezFrames;i++)
      {
         getline(in,str);//Frame:\n
         getline(in,str);//actual number identical to i
         getline(in,str);//Checked:\n
         in>>model.plotChPiez(i).piezChecked;
         getline(in,str);//\n
         getline(in,str);//Piezometer \n 
         in>>model.plotChPiez(i).piezLine>>model.plotChPiez(i).piezColor;
         getline(in,str);//\n
         getline(in,str);//Sm check \n
         in>>model.plotChPiez(i).smChecked>>model.plotChPiez(i).smType;
         getline(in,str);//\n
      }
      getline(in,str);//"Line width\n";
   }
   
   if(str.find("Line width:")!=std::string::npos)
   {
      int lw;
      in>>lw;
      (*lineWidth)=lw;
      getline(in,str);
      getline(in,str);//"Time shown in: 0-seconds, 1-minutes, 2-hours, 3-days\n";
   }

   size_t timeUnit;
   in>>timeUnit;
   model.setTimeUnit(timeUnit);
   double multiply=(model.timeUnit()==0 ? 1.0 : 
         (model.timeUnit()==1 ? 60.0 : (model.timeUnit()==2 ? 3600.0 : 86400)));
   getline(in,str);//"\n";
   getline(in,str);//"Number of wells: \n";
   size_t nwells;
   in>>nwells;
   getline(in,str);//"\n";
   wells.resize(nwells);
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      getline(in, str);//"Well: \n";
      std::string name;
      getline(in, name);
      if(!(name.find("Position: ")!=std::string::npos))
      {//there name for this well
         if(name[name.length() - 1]=='\n')
            name.resize(name.length()-1);
         QString nameQStr;
         conversion::convertStringToQString(name,nameQStr);
         wells[iwell].setName(nameQStr);
         getline(in,str);//"Position: \n";
      }//else go to Position
      in>>value1; in>>value2;
      wells[iwell].setPos(Point2 (value1, value2));
      getline(in, str);//"\n";
      getline(in, str);//"Radius: \n";
      in>>value1;
      wells[iwell].setRadius(value1);
      getline(in,str);//"\n";
      getline(in,str);//"Discharge rate: \n";
      size_t nq;
      in>>nq;
      getline(in,str);//"\n";
      std::vector<double> t;
      for(size_t iq=0;iq<nq;iq++)
      {
         in>>value1; in>>value2;

         checkLoadError("Error!!! Trying to import well with two same time values for discharge rate: ", t, value1*multiply, multiply);
         t.push_back(value1);

         wells[iwell].addQ(Point2 (value1*multiply, value2));
         getline(in, str);//"\n";
      }
      getline(in, str);//"Measured drawdown: \n";
      size_t ns;
      in>>ns;
      getline(in, str);
      t.clear();
      for(size_t is=0;is<ns;is++)
      {
         in>>value1; in>>value2;
         value1*=multiply;

         checkLoadError("Error!!! Trying to import well with two same time values for measured drawdown: ", t, value1*multiply, multiply);
         t.push_back(value1);

         wells[iwell].addMeasuredS(Point2 (value1,value2));
         getline(in,str);//"\n";
      }
      getline(in,str);//"Linear loss parameter: \n";
      size_t nA;
      in>>nA;
      getline(in,str);//"\n";
      t.clear();
      for(size_t ia=0;ia<nA;ia++)
      {
         in>>value1; in>>value2;
         value1*=multiply;
         checkLoadError("Error!!! Trying to import well with two same time values for linear loss parameter: ", t, value1*multiply, multiply);
         t.push_back(value1);

         wells[iwell].addBigA(Point2 (value1,value2));
         getline(in,str);//"\n";
      }
      getline(in,str);//"Quadratic loss parameter: \n";
      size_t nB;
      in>>nB;
      getline(in,str);//"\n";
      t.clear();
      for(size_t ib=0;ib<nB;ib++)
      {
         in>>value1; in>>value2;
         value1*=multiply;
         checkLoadError("Error!!! Trying to import well with two same time values for quadratic loss parameter: ", t, value1*multiply, multiply);
         t.push_back(value1);

         wells[iwell].addBigB(Point2 (value1,value2));
         getline(in,str);//"\n";
      }
   }
   getline(in,str);//"Number of piezometers: \n";
   size_t npiez;
   in>>npiez;
   piez.resize(npiez);
   getline(in,str);//"\n";
   for(size_t ipiez=0;ipiez<npiez;ipiez++)
   {
      getline(in,str);
      //in<<"Piezometer: \n";
      in>>str;
      if(str[str.length() - 1]=='\n')
         str.resize(str.length()-1);
      QString qStr;
      conversion::convertStringToQString(str,qStr);
      piez[ipiez].setName(qStr);
      getline(in,str);//"\n"
      getline(in,str);//"Position: \n";
      in>>value1; in>>value2;
      piez[ipiez].setPos(Point2 (value1, value2));
      getline(in,str);//"\n";
      getline(in,str);//"Measured drawdown: \n";
      size_t ns;
      in>>ns;
      getline(in,str);//"\n";
      std::vector<double> t;
      for(size_t is=0;is<ns;is++)
      {
         in>>value1; in>>value2;
         value1*=multiply;

         checkLoadError("Error!!! Trying to import piezometer with two same time values for mesurad drawdown: ", t, value1*multiply, multiply);
         t.push_back(value1);

         piez[ipiez].addMeasuredS(Point2 (value1, value2));
         getline(in,str);//"\n";
      }
   }
   size_t type;
   
   getline(in,str);//"Boundary type:\n" or next string 
         

   if((str.find("Boundary type:")!=std::string::npos))
   {
      in>>type;
      boundary.setType(type);
      getline(in,str);//"\n"
      size_t nlines=(type<3 ? type : type-1);
      for(size_t iline=0;iline<nlines;iline++)   
      {
         if(iline==0)
         {
            getline(in,str);//"Referent well:\n";
            if(str.find("Referent well:")!=std::string::npos)
            {
               size_t index;
               in>>index;
               getline(in,str);//"\n";
               boundary.setRefWell(index);
               getline(in,str);//"Alpha:\n"; or next string
            }
            if(str.find("Alpha:")!=std::string::npos)
            {
               in>>value1;
               boundary.setAlpha(value1);
               getline(in, str);//"\n";
               getline(in,str);//"Beta:" or next string 
            }
            if(type==Enums::ANGLE && str.find("Beta:")!=std::string::npos){
               //getline(in,str);//"Beta:"
               in>>value1;
               std::cout<<"IO::openFile | beta/value1 = "<<value1<<"\n";
               boundary.setBeta(value1);
               getline(in, str);//"\n";
               getline(in,str);//"Maximal distance:" or next string
            }

            if((type==Enums::PARALLEL_LINES || 
                  type==Enums::THREE_LINES || type==Enums::FOUR_LINES) && 
                  str.find("Maximal distance:")!=std::string::npos)
            {
               in>>value1;
               boundary.setMaxDist(value1);
               getline(in,str);//"\n";
               getline(in,str);//"Distance" or next string
            }
         }
         QString s;
         conversion::convertStringToQString(str,s);
         if(str.find("Distance")!=std::string::npos)
         {
            in>>value1;
            boundary.setRho(iline,value1);
            getline(in,str);//"\n";
         }
         getline(in,str);//"Line type: 0 - constant potential, 1 - water impermeable:\n";

         
         size_t index;
         in>>index;
         getline(in,str);//"\n"
         if(index==1)
            boundary.setLineType(iline,true);
         else
            boundary.setLineType(iline,false);
         getline(in,str);//"Transmissivity:\n" or next distance
      }
      if(nlines==0)
         getline(in,str);//"Transmissivity:\n";
   }
   if(str.find("Transmissivity")!=std::string::npos)
   {
      in>>value1;
      getline(in,str);//"\n"
      model.setBigT(value1);
      getline(in,str);//"Specific storage:\n";
   }
   if(str.find("Specific storage")!=std::string::npos)
   {
      in>>value1;
      getline(in,str);//"\n"
      model.setMu(value1);
      getline(in,str);//" Domain of calculation "
   }
   if(str.find("Domain of calculation")!=std::string::npos)
   {
      in>>value1;
      model.setVisXmin(value1);
      in>>value1;
      model.setVisXmax(value1);
      in>>value1;
      model.setVisYmin(value1);
      in>>value1;
      model.setVisYmax(value1);
      getline(in,str);//"\n"
      getline(in,str);//
   }
   if(str.find("Division")!=std::string::npos)
   {
      int intValue;
      in>>intValue;
      model.setVisXdiv(intValue);
      in>>intValue;
      model.setVisYdiv(intValue);
      getline(in,str);//"\n"
      getline(in,str);//
   }
   if(str.find("Isoline values") != std::string::npos)
   {
      int niso;
      in>>niso;
      getline(in,str);//"\n"
      if(niso!=0)
      {
         QVector<double> listNumbers(niso);
         for(int i=0;i<niso;i++)
            in>>listNumbers[i];
         model.pushIsolineValue(listNumbers);
         getline(in,str);//"\n"
      }
      getline(in,str);//
   }
   if(str.find("Isoline: label incidence, play duration and color:") != std::string::npos)
   {
      int i1, i2;     
      in>>i1>>value1>>i2;
      getline(in,str);//"\n"
      getline(in,str);//
      model.setIsolineProp(i1, value1, i2);
   }
   if(str.find("Porosity") != std::string::npos)
   {
       in>>value1;
       model.setPorosity(value1);
       getline(in,str);//"\n"
       getline(in,str);//
   }
   if(str.find("Layer thickness") != std::string::npos)
   {
      in>>value1;
      model.setThickness(value1);
      getline(in,str);//"\n"
      getline(in,str);//
   }
   if(str.find("Time length of streamlines") != std::string::npos)
   {
      in>>value1;
      model.setTimeLength(value1);
      getline(in,str);//"\n"
      getline(in,str);//
   }
   if(str.find("Choosen time") != std::string::npos)
   {
      int intValue;
      in>>intValue;
      model.setTimeStream(intValue);
      getline(in,str);
      getline(in,str);//
   }
   if(str.find("Number of streamlines per well") != std::string::npos)
   {
      int intValue;
      in>>intValue;
      model.setNoOfStreamLines(intValue);
      getline(in,str);//"\n"
      getline(in,str);
   }
   if(str.find("Stream line color:") != std::string::npos)
   {
      int intValue;
      in>>intValue;
      model.setStreamColor(intValue);
   }

   in.close();
}

}
