#ifndef COLLAPSINGBAR_H
#define COLLAPSINGBAR_H

#include <QObject>
#include <QFrame>
#include <QString>
#include <QPalette>
class QLabel;
class QMouseEvent;

class CollapsingBar : public QFrame
{
   Q_OBJECT
public:
   CollapsingBar(QString);
   
   void switchArrow(bool);
   void mousePressEvent(QMouseEvent *);
   
   QLabel * barLabel() { return _barLabel; }
   QString title() { return _title; }
   bool isVisible() { return _isVisible; }
   void collapse();
   
signals:
   void switchVisibility(bool);
   
private:
   QLabel * _barArrow;
   QLabel * _barLabel;
   QString _title;
   bool _isVisible;
   QPalette _savePalette;
};
#endif