#ifndef SUPERPOS_H
#define SUPERPOS_H

class Well;
class Piezometer;
class FictWell;
class Line;
class Point2;
class ModelInformation;
class Boundary;

namespace Enums
{
   enum BoundaryType {NO_BOUNDARY, ONE_LINE, PARALLEL_LINES, ANGLE, THREE_LINES,
      FOUR_LINES};
};

class SuperPos
{
   public:
      static void makeSMHDtimes(QVector<double> & times,
            QVector<double> & timeInSMHD, const size_t timeUnit);
      
      static void makeTimes(const std::vector<Well> & wells, 
            const std::vector<Piezometer>  & piez, QVector<double> & times, 
            QVector<double> & timeInSMHD, const size_t timeUnit);

      static void makeFictiveParallelMaxDist(const std::vector<Well> & wells, 
          std::vector<FictWell> & fictiveWells, const std::vector<Line> & lines,
          const std::vector<bool> & lineType, const double maxDist, 
          const double wholeDist, const size_t refWell);

      static void makeFictiveWellsOneLine(const std::vector<Well> & wells,
            std::vector<FictWell> & fictiveWells, const Line & line, 
            const bool lineType);

      static void makeFictiveWells(const std::vector<Well> & wells, 
            Boundary & boundary, std::vector<FictWell> & fictiveWells);

      static double calculateU(const ModelInformation & model, 
            const double time, const double r);

      static double calculateW(const ModelInformation & model, 
            const double time, const double r);

      static double calculateS(const double Q0y, const Point2 & Q1,
            const ModelInformation & model, const double time, const double r);

      static double timeValue(const double timestep, 
            const std::vector<Point2> & q);
      
      static double linearLoss(const double timestep, 
            const std::vector<Point2> & q, const double A);

      static double quadraticLoss(const double timestep, 
            const std::vector<Point2> & q, const double B);

      static double calculateInTimePoint(const std::vector<Point2> & q, 
       const ModelInformation & model, const double timeStep, const double rho);

      static bool hasValue (const std::vector<Point2> vec, const double value, 
            size_t & i);

      static QVector<double> lossesInWell(const QVector<double> times, 
            const Well & well);

      static QVector<double> calculateInPoint(const std::vector<Well> & wells, 
            const std::vector<FictWell> & fictiveWells,
            const ModelInformation & model, const QVector<double> times, 
            const Point2 & point, 
            const size_t jwell=std::numeric_limits<size_t>::max());

      static double factorial(double n)
      {
         return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
      } 
};

#endif
