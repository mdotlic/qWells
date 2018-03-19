#ifndef DATA_H
#define DATA_H

#include "well.h"
#include "modelInformation.h"
#include "boundary.h"
#include "superPos.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <vector>

class QWells;

class Data : public QObject
{
   Q_OBJECT
public:
   Data(QWells*);
   
   // data return functions:
   int noOfWells() const { return int(_wells.size()); }
   int noOfPiezometers() const { return int(_piez.size()); }
   const std::vector<Well> & wells() const { return _wells; }
   std::vector<Well> & wells() { return _wells; }
   const std::vector<Piezometer> & piezometers() const { return _piez; }
   std::vector<Piezometer> & piezometers() { return _piez; }
   
   QVector<double> & t() { return _t; }
   QVector<double> & timeInSMHD() { return _timeInSMHD; }
   std::vector<FictWell> & fictiveWells() { return _fictiveWells;}
   ModelInformation & getModel() { return _model; }
   Boundary & getBoundary() { return _boundary; }
   Boundary * getBoundaryPtr() { return &_boundary; }
   
   const Well & getWell(int i) const { return _wells[i]; }
   const FictWell & getFictWell(int i) const { return _fictiveWells[i]; }
   const Piezometer & getPiezometer(int i) const { return _piez[i]; }
   const ModelInformation & getModel() const { return _model; }
   const Boundary & getBoundary() const { return _boundary; }
   const QVector<double> & t() const { return _t; }
   const QVector<double> & timeInSMHD() const { return _timeInSMHD; }

   const std::vector<FictWell> & fictiveWells() const { return _fictiveWells; } 
   
   QWells * qWells() { return _qWells; }
   
   // adding/removing data using tables:
   void addWell(const size_t, const QString&, const Point2&, const double);
   void addPiezometer(const size_t, const QString&, const Point2&);
   void removeWell(const size_t);
   void removePiezometer(const size_t);
   
   // adding/removing data using import buttons:
   void notifyOfWellImport();
   void notifyOfPiezometerImport();
   //  auxiliary functions for import/export
   void loadVectorIntoTable(std::vector<std::vector<double>>&,
                            std::vector<std::vector<bool>>&,
                            const std::vector<Point2> & vec,const int);
   void constructAuxWellDataTable(std::vector<std::vector<double>> &,
                            std::vector<std::vector<bool>>&,const int);
   void constructAuxPiezoDataTable(std::vector<std::vector<double>> &,
                            std::vector<std::vector<bool>>&,const int);
   
   // changing data using tables:
   void changeWellName(const int,const QString&);
   void changeWellPosition(const int,const Point2&);
   void changeWellRadius(const int,const double);
   void changePiezoName(const int,const QString&);
   void changePiezoPosition(const int,const Point2&);
   void changePiezoData(std::vector<Point2>&,const Point2&);
   void changeTimeData(std::vector<Point2>&,const double,const double);
   void changeVectorData(std::vector<Point2>&,const Point2&);
   void changeABVectorData(std::vector<Point2>&,const Point2&,const int,const int);
   int findElementForGivenTime(const std::vector<Point2>&,const double&);
   void deleteRowOfWellData(const int,const double&);
   void deleteRowOfPiezoData(const int,const double&);
   void deleteSingleTableData(std::vector<Point2>&,const double&);
   
   // changing boundary data:
   void setBoundary(const int);
   void setRefWell(const int);
   void setAngle(const int, const double);
   void setDistance(const int, const double);
   void setDistanceFromSlider(const int, const double); // from slider
   void setBoundaryType(const int, const bool);
   void setMaxDistance(const double);
   void setMaxDistanceFromSlider(const double);

   void setAFromSlider(const int,const double); // from slider
   void setBFromSlider(const int,const double); // from slider
   
   // changing model information:
   void setBigT(double);
   void setMu(double);
   void setTimeUnit(const size_t);
   bool isEverythingSet();

   //conversion
   double convertTimeUnit();
   double & xmin(){return _xmin;}
   double & xmax(){return _xmax;}
   double & ymin(){return _ymin;}
   double & ymax(){return _ymax;}
   double & ratio(){return _ratio;}
   void setRatio()
   {
      _ratio=(_xmax-_xmin)/(_ymax-_ymin);
   }

   void setStatic(const double s) {_staticLevel=s;}
   double * staticLevel() {return &_staticLevel;}
      
signals:
   void wellAdded(int);
   void wellImported(int,int); // wellID, number of rows in its data table
   void piezometerAdded(int);
   void piezometerImported(int,int); // piezoID, number of rows in its data table
   void wellRemoved(int);
   void piezometerRemoved(int);
   void itemNameChanged(int,int,const QString&); // table to chooser/adjuster
   void boundarySet();
   void refWellSet(int);
   void distanceSet(int,double); // set by boundary widget
   void distanceModified(int); // modified by slider
   void maxDistanceSet(double); // set in boundary widget
   void maxDistanceModified(double); // modified by slider
   void bigAModified(int); // modified by slider, signal to table TODO unify
   void bigBModified(int); // modified by slider, signal to table TODO unify
   void bigASet(int); // set by table, signal to slider TODO unify
   void bigBSet(int); // set by table, signal to slider TODO unify
   void initialBigASet(int); // A set by Data, signal to table TODO unify
   void initialBigBSet(int); // B set by Data, signal to table TODO unify
   void timeUnitSet(size_t);
   // no change in data, signal sent from BoundaryWidget to BoundaryAdjuster:
   void noBoundarySet();
   void pictureLoaded();
   
private:
   QWells * _qWells;
   std::vector<Well> _wells;
   std::vector<FictWell> _fictiveWells;
   std::vector<Piezometer> _piez;
   ModelInformation _model;
   Boundary _boundary;
   QVector<double> _t;
   QVector<double> _timeInSMHD;
   std::string _fileName;
   double _xmin=0.0;
   double _xmax=150.0;
   double _ymin=0.0;
   double _ymax=150.0;
   double _ratio=1.0;
   double _staticLevel=0.0;
};
#endif
