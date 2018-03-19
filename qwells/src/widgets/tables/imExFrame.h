#ifndef IMEXFRAME_H
#define IMEXFRAME_H

#include <QFrame>
class QPushButton;

class ImportExportFrame : public QFrame
{
public:
   ImportExportFrame();
   
   QPushButton * importButton() { return _importButton; }
   QPushButton * exportButton() { return _exportButton; }
   
private:
   QPushButton * _importButton;
   QPushButton * _exportButton;
};
#endif