#ifndef MODEL_INFORMATION_H
#define MODEL_INFORMATION_H
#include <limits>
#include <QVector>
#include <QDebug>

class PlotChooserLoadWell
{
   public:
      bool wellChecked;
      bool withLossChecked;
      int withLossLine;
      int withLossColor;
      bool noLossChecked;
      int noLossLine;
      int noLossColor;
      bool smChecked;
      int smType;
};

class PlotChooserLoadPiez
{
   public:
      bool piezChecked;
      int piezLine;
      int piezColor;
      bool smChecked;
      int smType;
};

class ModelInformation
{
   public:
      ModelInformation()
      {
         _plotChWell.resize(0);
//          _bigT=std::numeric_limits<double>::max();
//          _mu=std::numeric_limits<double>::max();
      }
      
      double bigT() const {return _bigT;}
      double mu() const {return _mu;}
      size_t timeUnit() const {return _timeUnit;}
      QString pictureName() const {return _pictureName;}
      double picXmin () const {return _picXmin;}
      double picXmax () const {return _picXmax;}
      double picYmin () const {return _picYmin;}
      double picYmax () const {return _picYmax;}
      double visXmin () const {return _visXmin;}
      double visXmax () const {return _visXmax;}
      double visYmin () const {return _visYmin;}
      double visYmax () const {return _visYmax;}
      double visXdiv () const {return _visXdiv;}
      double visYdiv () const {return _visYdiv;}
      double porosity() const {return _porosity;}
      double thickness() const {return _thickness;}
      double timeLength() const {return _timeLength;}
      int timeStream() const {return _timeStream;}
      int noStreamLines() const {return _noStreamLines;}
      int wellXOffset() const {return _wellXOffset;}
      int wellYOffset() const {return _wellYOffset;}
      int wellRadius() const {return _wellRadius;}
      int wellColor() const {return _wellColor;}
      bool showWellNames() const {return _showWellNames;}
      int piezXOffset() const {return _piezXOffset;}
      int piezYOffset() const {return _piezYOffset;}
      int piezRadius() const {return _piezRadius;}
      int piezColor() const {return _piezColor;}
      bool showPiezNames() const {return _showPiezNames;}
      int constColor() const {return _constColor;}
      int impermColor() const {return _impermColor;}

      const QVector<double> & isoValues()const { return _isoValues;}
      int lineWidth()const {return _lineWidth;}
      int isoLabelIncidence() const {return _isoLabelIncidence;}
      double isoPlayDuration() const {return _isoPlayDuration;}
      int isoColor() const {return _isoColor;}
      int streamLineColor() const {return _streamLineColor;}
      
      void setBigT(const double value) {_bigT=value;}
      void setMu(const double value) {_mu=value;}
      void setTimeUnit (const size_t i){_timeUnit=i;}
      void setPictureName(const QString & str) {_pictureName=str;}
      void setPicXmin (const double value) {_picXmin=value;}
      void setPicXmax (const double value) {_picXmax=value;}
      void setPicYmin (const double value) {_picYmin=value;}
      void setPicYmax (const double value) {_picYmax=value;}
      
      void setVisXmin (const double value) {_visXmin=value;}
      void setVisXmax (const double value) {_visXmax=value;}
      void setVisYmin (const double value) {_visYmin=value;}
      void setVisYmax (const double value) {_visYmax=value;}
      void setVisXdiv (const double value) {_visXdiv=value;}
      void setVisYdiv (const double value) {_visYdiv=value;}
      void pushVisData(const double xmin, const double xmax, 
            const double ymin, const double ymax, const int xdiv, 
            const int ydiv)
      {
         _visXmin = xmin;
         _visXmax = xmax;
         _visYmin = ymin;
         _visYmax = ymax;
         _visXdiv = xdiv;
         _visYdiv = ydiv;
      }
      void setPorosity(const double value) {_porosity=value;}
      void setThickness(const double value) {_thickness=value;}
      void setTimeLength(const double value) {_timeLength=value;}
      void setTimeStream(const int value) {_timeStream=value;}
      void setNoOfStreamLines(const int value) {_noStreamLines=value;}
      void pushStreamData(const double porosity, const double thickness, 
            const double timeLength, const int timeStream, const int noSL)
      {
         _porosity=porosity;
         _thickness=thickness;
         _timeLength=timeLength;
         _timeStream=timeStream;
         _noStreamLines=noSL;
      }

      void pushIsolineValue(QVector<double> listNumbers)
      {
         _isoValues=listNumbers;
      }

      void clearWellPiez()
      {
         _plotChWell.clear();
         _plotChPiez.clear();
      }

      void resizePlotChWell(const int i)
      {
         _plotChWell.resize(i);
      }
      
      void resizePlotChPiez(const int i)
      {
         _plotChPiez.resize(i);
      }
      
      int sizePlotChWell()const
      {
         return _plotChWell.size();
      }
      
      int sizePlotChPiez()const
      {
         return _plotChPiez.size();
      }

      PlotChooserLoadWell & plotChWell(const int i)
      {
         return _plotChWell[i];
      }
      
      PlotChooserLoadPiez & plotChPiez(const int i)
      {
         return _plotChPiez[i];
      }

      int * setLineWidth()
      {
         return & _lineWidth;
      }

      void setParamObjProp (int wellXOffset, int wellYOffset, int wellRadius, 
            int wellColor, bool showWellNames, int piezXOffset, int piezYOffset,
            int piezRadius, int piezColor, bool showPiezNames, int constColor,
            int impermColor)
      {
         _wellXOffset = wellXOffset;
         _wellYOffset = wellYOffset;
         _wellRadius = wellRadius;
         _wellColor = wellColor;
         _showWellNames = showWellNames;
         _piezXOffset = piezXOffset;
         _piezYOffset = piezYOffset;
         _piezRadius = piezRadius;
         _piezColor = piezColor;
         _showPiezNames = showPiezNames;
         _constColor = constColor;
         _impermColor = impermColor;
      }

      void setIsolineProp(const int labelInc, const double playDuration, 
            const int isoColor)
      {
         _isoLabelIncidence=labelInc;
         _isoPlayDuration=playDuration;
         _isoColor=isoColor;
      }

      void setStreamColor(const int streamColor)
      {
         _streamLineColor=streamColor;
      }

   private:
      double _bigT=0.001;
      double _mu=0.001;
      size_t _timeUnit=0;//=0 in seconds, 1 in minutes, 2 in hours, 3 in days
      QString _pictureName=QString(" ");
      double _picXmin = 0.0;
      double _picYmin = 0.0;
      double _picXmax = 150.0;
      double _picYmax = 150.0;
      double _visXmin = 0.0;
      double _visXmax = 150.0;
      double _visYmin = 0.0;
      double _visYmax = 150.0;
      int _visXdiv = 10;
      int _visYdiv = 10;     
      double _porosity=0.1;
      double _thickness=1;
      double _timeLength=100;
      int _timeStream=0;
      int _noStreamLines=2;
      QVector<double> _isoValues;
      int _wellXOffset = -5;
      int _wellYOffset = -4;
      int _wellRadius = 2;
      int _wellColor = 1;
      bool _showWellNames = true;
      int _piezXOffset = -5;
      int _piezYOffset = -4;
      int _piezRadius = 2;
      int _piezColor = 2;
      bool _showPiezNames = true;
      int _constColor = 2;
      int _impermColor = 1;
      QVector<PlotChooserLoadWell> _plotChWell;
      QVector<PlotChooserLoadPiez> _plotChPiez;
      int _lineWidth = 2;
      int _isoLabelIncidence=2;
      double _isoPlayDuration=10.0;
      int _isoColor=0;
      int _streamLineColor = 0;
    //  QVector<bool> _wellChecked;
};
#endif
