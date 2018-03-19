#include "data.h"
#include "well.h"
#include "modelInformation.h"
#include "boundary.h"
#include "superPos.h"

#include "qWells.h"
#include "io.h"

#include <QObject>
#include <QString>
#include <vector>
#include <QDebug>
#include <assert.h>

using namespace io;
   
Data::Data(QWells * qWells) : QObject(), _qWells(qWells) {}

// ADDING/REMOVING DATA USING TABLES:

void Data::addWell(const size_t wellID, const QString & name, const Point2 & pos, const double r)
{
//    qDebug()<<"Data::addWell | WellID = "<<wellID<<" | name = "<<name<<" | position = ("<<pos._x<<","<<pos._y<<") | r = "<<r<<"\n";
   if (wellID == _wells.size()) 
   {
      _wells.push_back(Well(name,pos,r));
      emit wellAdded(wellID);
      _qWells->dataHasBeenModified();
   }
   else assert(false);
}

void Data::addPiezometer(const size_t piezoID, const QString & name, const Point2 & pos)
{
   if (piezoID == _piez.size()) 
   {
      _piez.push_back(Piezometer(name,pos));
      emit piezometerAdded(piezoID);
      _qWells->dataHasBeenModified();
   }
   else assert(false);
}

void Data::removeWell(const size_t wellID)
{
   if (wellID >= _wells.size()) 
   {
      qDebug()<<"Data::removeWell | can't remove well that doesn't exist!\n";
      assert(false);
   }
   auto it = _wells.begin() + wellID;
   _wells.erase(it);
   emit wellRemoved(wellID);
   _qWells->dataHasBeenModified();
}

void Data::removePiezometer(const size_t piezoID)
{
   if (piezoID >= _piez.size())
   {
      qDebug()<<"Data::removePiezometer | can't remove piezometer that doesn't exist!\n";
      assert(false);
   }
   auto it = _piez.begin() + piezoID;
   _piez.erase(it);
   emit piezometerRemoved(piezoID);
   _qWells->dataHasBeenModified();
}

// IMPORTING DATA USING IMPORT BUTTONS:

void Data::notifyOfWellImport()
{
   const int wellID = _wells.size()-1;
   std::vector<std::vector<double>> auxTable;
   std::vector<std::vector<bool>> auxBoolTable;
   constructAuxWellDataTable(auxTable,auxBoolTable,wellID);
   const int noOfData = auxTable.size();
   emit wellImported(wellID,noOfData);
   _qWells->dataHasBeenModified();
}

void Data::notifyOfPiezometerImport()
{
   const int piezoID = _piez.size()-1;
   std::vector<std::vector<double>> auxTable;
   std::vector<std::vector<bool>> auxBoolTable;
   constructAuxPiezoDataTable(auxTable,auxBoolTable,piezoID);
   const int noOfData = auxTable.size();
   emit piezometerImported(piezoID,noOfData);   
   _qWells->dataHasBeenModified();
}

// auxiliary functions for import:

void Data::loadVectorIntoTable(std::vector<std::vector<double>> & table, std::vector<std::vector<bool>> & validTable, const std::vector<Point2> & vec, const int vecID)
{
   const double dMax = std::numeric_limits<double>::max();
   std::vector<double> row(5,dMax);
   
   std::vector<bool> validRow(5,false);
   
   const int vecSize = vec.size();
   size_t k;
   int l;
   bool doInsert;
   double ti,vi;
   for (int i=0;i<vecSize;i++)
   {
      ti = vec[i]._x / convertTimeUnit();
      vi = vec[i]._y;
      k = 0; // rows in table
      l = -1; // id for row position where [ti,vi] will be placed in table
      doInsert=false; // insert or pushback
      while (k<table.size())
      {
         if (fabs(ti - table[k][0])<1e-6 )
         {
            l = k;
            break;
         }
         else if (ti < table[k][0])
         {
            l = k;
            doInsert = true;
            break;
         }
         k = k + 1;
      }
      if (l >= 0)
      {
         if (doInsert)
         {
            row[0] = ti;
            row[vecID] = vi;
            auto it = table.begin() + l;
            table.insert(it,row);
            validRow[0] = true;
            validRow[vecID] = true;
            auto it1 = validTable.begin() + l;
            validTable.insert(it1,validRow);
         }
         else
         {
            table[l][vecID] = vi;
            validTable[l][vecID] = true;
         }
      }
      else
      {
         row[0] = ti;
         row[vecID] = vi;
         table.push_back(row);
         validRow[0] = true;
         validRow[vecID] = true;
         validTable.push_back(validRow);
      }
   }
}

void Data::constructAuxWellDataTable(std::vector<std::vector<double>> & table, std::vector<std::vector<bool>> & validTable, const int wellID)
{
   const double dMax = std::numeric_limits<double>::max();
   std::vector<double> row(5,dMax);
   
   std::vector<bool> validRow(5,false);
   
   const Well & well = _wells[wellID];
   
   const int qSize = well.nq();
   for (int i=0;i<qSize;i++)
   {
      row[0] = well.qi(i)._x/convertTimeUnit();
      row[1] = well.qi(i)._y;
      table.push_back(row);
      validRow[0] = true;
      validRow[1] = true;
      validTable.push_back(validRow);
   }
   loadVectorIntoTable(table,validTable,well.Sm(),2);
   loadVectorIntoTable(table,validTable,well.bigA(),3);
   loadVectorIntoTable(table,validTable,well.bigB(),4);
}

void Data::constructAuxPiezoDataTable(std::vector<std::vector<double>> & table, std::vector<std::vector<bool>> & validTable, const int piezoID)
{
   const double dMax = std::numeric_limits<double>::max();
   std::vector<double> row(2,dMax);
   
   std::vector<bool> validRow(2,false);
   
   const Piezometer & piezo = _piez[piezoID];
   const int smSize = piezo.nS();
   for (int i=0;i<smSize;i++)
   {
      row[0] = piezo.Smi(i)._x / convertTimeUnit();
      row[1] = piezo.Smi(i)._y;
      table.push_back(row);
      validRow[0] = true;
      validRow[1] = true;
      validTable.push_back(validRow);
   }
}

// CHANGING DATA USING TABLES:

void Data::changeWellName(const int wellID, const QString & name)
{
   _wells[wellID].setName(name);
   _qWells->dataHasBeenModified();
   emit itemNameChanged(wellID,0,name); // 0 = well, 1 = piezometer
}

void Data::changeWellPosition(const int wellID, const Point2 & pos)
{
   _wells[wellID].setPos(pos);
   _qWells->dataHasBeenModified();
}

void Data::changeWellRadius(const int wellID, const double r)
{
   _wells[wellID].setRadius(r);
   _qWells->dataHasBeenModified();
}

void Data::changePiezoName(const int piezoID, const QString & name)
{
   _piez[piezoID].setName(name);
   _qWells->dataHasBeenModified();
   emit itemNameChanged(piezoID,1,name); // 0 = well, 1 = piezometer
}

void Data::changePiezoPosition(const int piezoID, const Point2 & pos)
{
   _piez[piezoID].setPos(pos);
   _qWells->dataHasBeenModified();
}

void Data::changeTimeData(std::vector<Point2> & vec, const double oldTime, const double newTime)
{
   const int vecSize = int(vec.size());
//   qDebug()<<"Data::changeTimeData | vecSize = "<<vecSize<<" | oldTime = "<<oldTime<<" | newTime = "<<newTime<<"\n";
  if (vecSize==0) {qDebug()<<"Data::changeTimeData | ERROR - attempting to change the time in a vector of size zero! Exiting.\n";assert(false);}
  
   int i = 0;
   int saveI = vecSize;
   while (i < vecSize)
   {
      if (fabs(vec[i]._x - oldTime * convertTimeUnit())<1e-6 )
      {
//         qDebug()<<"Data::changeTimeData | MAKE CHANGE to "<<newTime * convertTimeUnit()<<" i.e. "<<newTime<<"\n";
         vec[i]._x = newTime * convertTimeUnit();
         saveI = i;
         i = vecSize+1; // exit while loop
      }
      else if (vec[i]._x < oldTime * convertTimeUnit()) i = i + 1;
      else assert(false);
   }
   // TODO sort vec by time, in ascending order:
   i = 0;
//    qDebug()<<"ENTER THE LOOP IF i = "<<i<<" < "<<saveI<<" = saveI\n";
   Point2 e = vec[saveI];
   while (i < saveI)
   {
//       qDebug()<<" lower loop i = "<<i<<"\n";
      if (vec[i]._x < e._x) i = i + 1;
      else break;
   }
   if (i<saveI)
   {
      vec.erase(vec.begin()+saveI);
      vec.insert(vec.begin()+i,e);
      return;
   }
   i = saveI + 1;
   while ((i > saveI) && (i<vecSize))
   {
//       qDebug()<<" higher loop i = "<<i<<"\n";
      if (vec[i]._x < vec[saveI]._x) i = i + 1;
      else break;
   }
   if (i>saveI+1)
   {
      vec.insert(vec.begin()+i,e);
      vec.erase(vec.begin()+saveI);
      return;
   }
}

void Data::changeVectorData(std::vector<Point2> & vec, const Point2 & e) 
{
   Point2 entry = Point2(e._x * convertTimeUnit(),e._y);
   const int vecSize = int(vec.size());
   if (vecSize==0) 
   {
      vec.push_back(entry);
      _qWells->dataHasBeenModified();
      return;
   }
   int i = 0;
   while (i < vecSize)
   {
      if (entry._x > vec[i]._x) i = i + 1;
      else if (entry._x == vec[i]._x) 
      {
         vec[i] = entry;
         _qWells->dataHasBeenModified();
         return;
      }
      else break;
   }
   if (i == vecSize) 
   {
      vec.push_back(entry);
      _qWells->dataHasBeenModified();
   }
   else 
   {
      auto it = vec.begin() + i;
      vec.insert(it,entry);
      _qWells->dataHasBeenModified();
   }
}

void Data::changeABVectorData(std::vector<Point2> & vec, const Point2 & entry, const int wellID, const int signalFlag)
{
   changeVectorData(vec,entry);
   if (signalFlag == 0) emit bigASet(wellID);
   else emit bigBSet(wellID);
}

int Data::findElementForGivenTime(const std::vector<Point2> & vec, const double & t)
{
   const size_t vecSize = vec.size();
   size_t i=0;
   while (i < vecSize)
   {
      if (vec[i]._x == t) return i;
      else i = i + 1;
   }
   return -1;
}

void Data::deleteRowOfWellData(const int wellID, const double & time)
{
   std::vector<Point2> & q = _wells[wellID].q();
   const int qi = findElementForGivenTime(q,time*convertTimeUnit());
   if (qi >= 0) q.erase(q.begin()+qi);
//    else 
//    {
//       std::cout<<"Data::deleteRowOfWellData | ERROR - corresponding Qi not found!\n";
//       assert(false);
//    }
   
   std::vector<Point2> & sm = _wells[wellID].Sm();
   const int smi = findElementForGivenTime(sm,time*convertTimeUnit());
   if (smi >= 0) sm.erase(sm.begin()+smi);
//    {
//       std::cout<<"Data::deleteRowOfWellData | ERROR - corresponding Smi not found!\n";
//       assert(false);
//    }
   
   std::vector<Point2> & a = _wells[wellID].bigA();
   const int ai = findElementForGivenTime(a,time*convertTimeUnit());
   if (ai >= 0) a.erase(a.begin()+ai);
//    {
//       std::cout<<"Data::deleteRowOfWellData | ERROR - corresponding Ai not found!\n";
//       assert(false);
//    }
   
   std::vector<Point2> & b = _wells[wellID].Sm();
   const int bi = findElementForGivenTime(b,time*convertTimeUnit());
   if (bi >= 0) b.erase(b.begin()+bi);
//    {
//       std::cout<<"Data::deleteRowOfWellData | ERROR - corresponding Bi not found!\n";
//       assert(false);
//    }
   _qWells->dataHasBeenModified();
}

void Data::deleteRowOfPiezoData(const int piezoID, const double & time)
{
   deleteSingleTableData(_piez[piezoID].Sm(),time);
}

void Data::deleteSingleTableData(std::vector<Point2> & vec, const double & time)
{
   const int vi = findElementForGivenTime(vec,time*convertTimeUnit());
   if (vi >= 0) vec.erase(vec.begin()+vi);
   else
   {
      std::cout<<"Data::deleteSingleTableData | ERROR - bad data location!\n";
      assert(false);
   }
}

// CHANGING BOUNDARY DATA:

void Data::setBoundary(const int typeID)
{
   _boundary = Boundary();
   _boundary.setType(typeID);
   _qWells->dataHasBeenModified();
   emit boundarySet();
   if(typeID==0)
      emit noBoundarySet();
}

void Data::setRefWell(const int wellID)
{
   _boundary.setRefWell(wellID);
   _qWells->dataHasBeenModified();
   emit refWellSet(wellID);
}

void Data::setAngle(const int angleID, const double value)
{
   if (angleID == 0)
      _boundary.setAlpha(value);
   else
      _boundary.setBeta(value);
   _qWells->dataHasBeenModified();
}
   
void Data::setDistance(const int distID, const double value)
{
   _boundary.setRho(distID,value);
   _qWells->dataHasBeenModified();
   emit distanceSet(distID,value);
}
   
void Data::setDistanceFromSlider(const int distID, const double value)
{
   _boundary.setRho(distID,value);
   _qWells->dataHasBeenModified();
   emit distanceModified(distID);
}

void Data::setBoundaryType(const int lineID, const bool typeID)
{
   _boundary.setLineType(lineID,typeID);
   _qWells->dataHasBeenModified();
}

void Data::setMaxDistance(const double value)
{
   _boundary.setMaxDist(value);
   _qWells->dataHasBeenModified();
   emit maxDistanceSet(value);
}

void Data::setMaxDistanceFromSlider(const double value)
{
   _boundary.setMaxDist(value);
   _qWells->dataHasBeenModified();
   emit maxDistanceModified(value);
}

// WELL WEAR (A,B) CHANGES FROM SLIDER:

void Data::setAFromSlider(const int wellID, double a)
{
   _wells[wellID].bigA(0)._y = a;
   _qWells->dataHasBeenModified();
   emit bigAModified(wellID);
}

void Data::setBFromSlider(const int wellID, double b)
{
   _wells[wellID].bigB(0)._y = b;
   _qWells->dataHasBeenModified();
   emit bigBModified(wellID);
}

// CHANGING MODEL INFORMATION:

void Data::setBigT(const double valueT)
{
   _model.setBigT(valueT);
   _qWells->dataHasBeenModified();
}

void Data::setMu(const double valueMu)
{
   _model.setMu(valueMu);
   _qWells->dataHasBeenModified();
}

// OTHER FUNCTIONS:

bool Data::isEverythingSet()
{
   QString q1("Well flux is not set for wells: ");
   QString a1("Linear loss parameter is set to 0 for wells: ");
   QString b1("Quadratic loss parameter is set to 0 for wells: ");
   QString q;
   QString a;
   QString b;
   bool set=true;
   size_t nwells=_wells.size();
   for(size_t iwell=0;iwell<nwells;iwell++)
   {
      double firstTime=std::numeric_limits<double>::max();
      if(_wells[iwell].nq()!=0)
         firstTime=_wells[iwell].qi(0)._x;
      if(_wells[iwell].nS()!=0)// && _wells[iwell].Smi(0)._x<firstTime)
         firstTime=std::min(_wells[iwell].Smi(0)._x, firstTime);
      if(_wells[iwell].nA()!=0)// && _wells[iwell].bigA(0)._x<firstTime)
         firstTime=std::min(_wells[iwell].bigA(0)._x, firstTime);
      if(_wells[iwell].nB()!=0) //&& _wells[iwell].bigB(0)._x<firstTime)
         firstTime=std::min(_wells[iwell].bigB(0)._x, firstTime);
      if(firstTime==std::numeric_limits<double>::max())
         firstTime=0.0;

      if(_wells[iwell].nq()==0){
         q.append(_wells[iwell].name());
         q.append(" ");
         set=false;
      }
      if(_wells[iwell].nA()==0){
         a.append(_wells[iwell].name());
         a.append(" ");
         _wells[iwell].addBigA(Point2 (firstTime, 0.0));
         set=false;
         emit initialBigASet(iwell); 
      }
      if(_wells[iwell].nB()==0){
         b.append(_wells[iwell].name());
         b.append(" ");
         _wells[iwell].addBigB(Point2 (firstTime, 0.0));
         set=false;
         emit initialBigBSet(iwell); 
      }
   }
   if(!set)
   {
      if(!a.isEmpty())
         _qWells->warning(a1.append(a));
      if(!b.isEmpty())
         _qWells->warning(b1.append(b));

      if(!q.isEmpty())
      {
         _qWells->error(q1.append(q));
         return false;
      }
   }
   return true;
}

void Data::setTimeUnit(const size_t timeUnit)
{
   _model.setTimeUnit(timeUnit);
   emit timeUnitSet(timeUnit);
}

double Data::convertTimeUnit()
{

   if(_model.timeUnit()==0)
      return 1.0;//seconds
   else if(_model.timeUnit()==1)
      return 60.0;//minutes
   else if(_model.timeUnit()==2)
      return 3600.0;//hours
   else if(_model.timeUnit()==3)
      return 86400.0;//days
   return 1.0;
}
