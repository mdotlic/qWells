#include "result.h"
#include <QDebug>
#include "engine/data.h"
#include "engine/well.h"
#include "engine/modelInformation.h"

Result::Result(const size_t nwells, const size_t npiez, const Data * data): 
   _data(data)
{
   _piezResult.resize(npiez);
   _wellResult.resize(nwells);
   _wellLosses.resize(nwells);
}

void Result::resizeResult()//(/*Data * data*/)
{
   _piezResult.resize(_data->noOfPiezometers());
   _wellResult.resize(_data->noOfWells());
   _wellLosses.resize(_data->noOfWells());
}
/*Result::~Result()
{
  // delete _data;
   _wellResult.resize(0);
   _wellLosses.resize(0);
   _piezResult.resize(0);
}*/

const QVector<double> & Result::piez(const size_t ipiez) const
{
   return _piezResult[ipiez];
}
QVector<double> & Result::piez(const size_t ipiez)
{
   return _piezResult[ipiez];
}
const QVector<double> & Result::well(const size_t iwell) const 
{
   return _wellResult[iwell];
}
QVector<double> & Result::well(const size_t iwell)
{
   return _wellResult[iwell];
}
const QVector<double> & Result::wellLosses(const size_t iwell) const
{
   return _wellLosses[iwell];
}
QVector<double> & Result::wellLosses(const size_t iwell) 
{
   return _wellLosses[iwell];
}
void Result::clearIwell(const size_t iwell)
{
   _wellResult[iwell].clear();
}

void Result::clearIwellLosses(const size_t iwell)
{
   _wellLosses[iwell].clear();
}

void Result::clearIpiez(const size_t ipiez)
{
   _piezResult[ipiez].clear();
}

void Result::calculateWell(const size_t iwell)
{
   _wellResult[iwell]=SuperPos::calculateInPoint(_data->wells(), 
         _data->fictiveWells(), _data->getModel(), _data->t(), 
         _data->wells()[iwell].pos(), iwell);
}

void Result::calculateWellLosses(const size_t iwell)
{
   _wellLosses[iwell]=SuperPos::lossesInWell(_data->t(), _data->wells()[iwell]);
}

void Result::calculatePiez(const size_t ipiez)
{
   _piezResult[ipiez]=SuperPos::calculateInPoint(_data->wells(), 
         _data->fictiveWells(), _data->getModel(), _data->t(), 
         _data->getPiezometer(ipiez).pos());
}

void Result::recalculateAll()
{
   int nwells=_wellResult.size();
   int npiez=_piezResult.size();
   for(int iwell=0;iwell<nwells;iwell++)
   {
      if(_wellResult[iwell].size()==0)
         continue;
      else
         calculateWell(iwell);
   }
   for(int iwell=0;iwell<nwells;iwell++)
   {
      if(_wellLosses[iwell].size()==0)
         continue;
      else
         calculateWellLosses(iwell);
   }
   for(int ipiez=0;ipiez<npiez;ipiez++)
   {
      if(_piezResult[ipiez].size()==0)
         continue;
      else
         calculatePiez(ipiez);
   }
}

