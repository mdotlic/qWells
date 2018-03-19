#include "message.h"
#include "qWells.h"

#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QIcon>
#include <QString>
#include <QWidget>

Message::Message(QWells * qWells) : _qWells(qWells) {}

void Message::newError(const QString text)
{
   QMessageBox * newError = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("ERROR"),text,
                                            QMessageBox::Ok,_qWells);
   newError->exec();
}

void Message::newWarning(const QString text)
{
   QMessageBox * newWarning = new QMessageBox(QMessageBox::Icon::Information,
                                            QString("Information"),text,
                                            QMessageBox::Ok,_qWells);
   newWarning->exec();
}

int Message::newModalWarning(QWidget * parent, const QString text)
{
   _modalParent = parent;
   QMessageBox * newWarning = new QMessageBox(QMessageBox::Icon::Warning,
                                            QString("Warning"),text,
                                              QMessageBox::Ok | QMessageBox::Cancel, _modalParent);
   newWarning->setDefaultButton(QMessageBox::Cancel);
   if (newWarning->exec() == QMessageBox::Cancel) return false;
   else return true;
   _modalParent = nullptr;
}
