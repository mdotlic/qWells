#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <QString>
#include <string>
#include <assert.h>

namespace conversion
{
   inline bool convertQStringToDouble(const QString & qStr, double & d)
   {
      bool ok;
      d = qStr.toDouble(&ok);
      return ok;
   }

   inline bool convertQStringToInt(const QString & qStr, int & i)
   {
      bool ok;
      i = qStr.toInt(&ok);
      return ok;
   }

   inline void convertStringToQString(const std::string str, QString & qStr)
   {
      qStr = QString(QString::fromUtf8(str.c_str()));
   }

   inline void convertQStringToString(const QString qStr, std::string & str)
   {
      str = qStr.toUtf8().constData();
   }
  
   template<class Point2>
   inline QVector<double> point2FirstToQvec (const std::vector<Point2> & vec,
         const size_t timeUnit)
   {
      size_t size=vec.size();
      QVector<double> retVec(size);
      double divide=(timeUnit==0 ? 1.0 : (timeUnit==1 ? 60.0: (timeUnit == 2 ? 3600.0 : 86400.0)));
      for(size_t i=0;i<size;i++)
         retVec[i]=vec[i]._x/divide;
      return retVec;
   }
   template<class Point2>
   inline QVector<double> point2SecondToQvec (const std::vector<Point2> & vec)
   {
      size_t size=vec.size();
      QVector<double> retVec(size);
      for(size_t i=0;i<size;i++)
         retVec[i]=vec[i]._y;
      return retVec;
   }  
}

/*inline void operator+= (QVector<double> & v1, 
      const QVector<double> & v2)
{
   int n=v2.size();
   QVector<double> ret(n);
   for(int i=0;i<n;i++)
      v1[i] += v2[i];
}*/

namespace qvecOperation
{
   inline QVector<double> qvecPlus (const QVector<double> & v1, 
         const QVector<double> & v2)
   {
      int n=v1.size();
      if(n!=v2.size())//should never happen
         assert(false);
      QVector<double> ret(n);
      for(int i=0;i<n;i++)
         ret[i]=v1[i]+v2[i];
      return ret;
   }
}

/*inline void operator-= (QVector<double> & v1, 
      const QVector<double> & v2)
{
   int n=v2.size();
   QVector<double> ret(n);
   for(int i=0;i<n;i++)
      v1[i] -= v2[i];
}

inline QVector<double> operator- (const QVector<double> & v1, 
      const QVector<double> & v2)
{
   QVector<double> ret(v1);
   ret-=v2;
   return ret;
}*/
#endif
