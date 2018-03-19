#ifndef TABTOOLBASE_H
#define TABTOOLBASE_H

#include <QObject>
#include <QFrame>
#include <QString>

class QVBoxLayout;
class CollapsingBar;

class TabToolBase : public QFrame
{
   Q_OBJECT
public:
   TabToolBase(QString);
   
   void appendToTitle(QString);
   
   CollapsingBar * titleBar() { return _titleBar; }
   QVBoxLayout * layout()     { return _layout; }
   
private:
   CollapsingBar * _titleBar;
   QVBoxLayout * _layout;
};
#endif
