#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QString>
class QWells;

class About : public QDialog
{
public:
   About(QWells*);
   void constructText();
private:
   QString _textQWells, _textCredits, _copyright;
};
#endif // ABOUT_H
