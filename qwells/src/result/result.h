#ifndef RESULT_H
#include <QVector>
#include <cstddef>
class Data;

class Result
{
   public:
      Result(const size_t nwells, const size_t npiez, const Data * data);
      void resizeResult();
      //~Result();
      const QVector<double> & piez(const size_t ipiez) const; 
      QVector<double> & piez(const size_t ipiez);
      
      const QVector<double> & well(const size_t iwell) const; 
      QVector<double> & well(const size_t iwell);

      const QVector<double> & wellLosses(const size_t iwell) const;
      QVector<double> & wellLosses(const size_t iwell);

      void clearIwell(const size_t iwell);
      void clearIwellLosses(const size_t iwell);
      void clearIpiez(const size_t ipiez);
      void calculateWell(const size_t iwell);
      void calculateWellLosses(const size_t iwell);
      void calculatePiez(const size_t ipiez);
      void recalculateAll();
      const Data * data() const{return _data;}
   private:
      QVector<QVector<double> > _piezResult;
      QVector<QVector<double> > _wellResult;
      QVector<QVector<double> > _wellLosses;
      const Data * _data;
};
#endif
