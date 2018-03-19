#ifndef TABLESETTINGS_H
#define TABLESETTINGS_H

#include <QObject>
#include <QRect>
class QWells;
class QDialog;
class QLabel;
class QComboBox;
class QFrame;

class TableSettings : public QObject
{
   Q_OBJECT
public:
   TableSettings(QWells*);
   
   void setScreenGeometry(const QRect&);
   void setMaxRowsShown(const int);
   void setSizes(const int,const int);
   int getMaxRowsShown() { return _maxRowsShown; }
   int getCurrentRowsShown() { return _currentRowsShown; }
   int getMaxTableHeight() { return _maxTableHeight; }
   
signals:
   void setMaxRows(int);
   
public slots:
   void startDialog();
   void setMaxRowsLabel(const int);
   void setCurrentRowsShown(const int);
   
private:
   QWells *  _qWells;
   QDialog * _dialog;
   QLabel * _setLabel;
   QComboBox * _setNoOfRows;
   QRect _screenGeometry;
   QLabel * _maxRowsLabel;
   QFrame * _frame;
   int _maxTableHeight;
   int _maxRowsShown;
   int _maxRowsID = -1;
   int _currentRowsShown = 1;
   int _cellHeight = 25;
   bool _maxRowsIsLoaded = false;
};
#endif // TABLESETTINGS_H