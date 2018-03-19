#include "finalParameters.h"
#include "qWells.h"
#include "engine/data.h"
#include "engine/well.h"
#include "engine/boundary.h"
#include <QVBoxLayout>
#include <QPlainTextEdit>

FinalParameters::FinalParameters(QWells * qWells): QDialog(qWells)
{
   resize(500,500);
   setWindowTitle("Final parameters");
   QVBoxLayout * layout = new QVBoxLayout;
   QPlainTextEdit * plain = new QPlainTextEdit;
   plain->insertPlainText(" FINAL PARAMETERS \n");
   plain->insertPlainText("__________________________\n\n");

   QString string1(QString(QChar(0x03BC))+": ");
   string1.append(QString::number(qWells->data()->getModel().mu()));
   
   string1.append(", T: "+QString::number(qWells->data()->getModel().bigT()));
   string1.append("\n\n");
   plain->insertPlainText(string1);

   for(int i=0;i<qWells->data()->noOfWells();i++)
   {
      QString string("Well: ");
      string.append(qWells->data()->getWell(i).name() + " in (" + 
            QString::number(qWells->data()->getWell(i).pos()._x)+";"+
            QString::number(qWells->data()->getWell(i).pos()._y)+") with radius:"+ QString::number(qWells->data()->getWell(i).radius()) +"\n");
      std::vector<double> times;
      double divide=qWells->data()->convertTimeUnit();

      for(size_t j=0;j<qWells->data()->getWell(i).nA();j++)
         times.push_back(qWells->data()->getWell(i).bigA(j)._x);
      for(size_t j=0;j<qWells->data()->getWell(i).nB();j++)
         times.push_back(qWells->data()->getWell(i).bigB(j)._x);

      std::sort (times.begin(), times.end());

      if(times.size()!=0)
      {
         for(size_t i=0;i<times.size()-1;i++)
            if(times[i]==times[i+1])
            {
               times.erase(times.begin()+i);
               i--;
            }
      }

      string.append("time                    |   A                           |   B\n");
      for(size_t j=0;j<times.size();j++)
      {
         string.append(QString::number(times[j]/divide));
         int blanks=15-(QString::number(times[j]/divide)).size();
         for(int k=0;k<2*blanks+blanks/8;k++)
            string.append(" ");
         int posA=qWells->data()->findElementForGivenTime(qWells->data()->getWell(i).bigA(), times[j]);
         int posB=qWells->data()->findElementForGivenTime(qWells->data()->getWell(i).bigB(), times[j]);
         if(posA!=-1)
         {
            string.append(QString::number(qWells->data()->getWell(i).bigA(posA)._y));
            blanks=15-(QString::number(qWells->data()->getWell(i).bigA(posA)._y)).size();
            for(int k=0;k<2*blanks+blanks/4;k++)
               string.append(" ");
         }
         else
            string.append("                                 ");
         if(posB!=-1)
            string.append(QString::number(qWells->data()->getWell(i).bigB(posB)._y));
         string.append("\n");
      }

      string.append("\n");

      plain->insertPlainText(string);
   }

   const double pi = 3.14159265358979;

   plain->insertPlainText("Boundary type: ");
   string1=" ";
   size_t type=qWells->data()->getBoundary().type();
   if(type==0)
      string1="no boundary\n";
   else if(type==1){
      string1="one line\n";
      string1.append("Alpha: "+QString::number(qWells->data()->getBoundary().alpha()*180.0/pi)+"\n");
      string1.append("Referent well: "+qWells->data()->getWell(qWells->data()->getBoundary().refWell()).name()+"\n");
      string1.append("Distance to line "+QString::number(qWells->data()->getBoundary().rho(0))+"\n");
   }else if(type==2){
      string1="paralell lines\n";
      string1.append("Alpha: "+QString::number(qWells->data()->getBoundary().alpha()*180.0/pi)+"\n");
      string1.append("Referent well: "+qWells->data()->getWell(qWells->data()->getBoundary().refWell()).name()+"\n");
      string1.append("Distance to line 1: "+QString::number(qWells->data()->getBoundary().rho(0))+"\n");
      string1.append("Distance to line 2: "+QString::number(qWells->data()->getBoundary().rho(1))+"\n");
   }else if(type==3){
      string1="angle\n";
      string1.append("Alpha: "+QString::number(qWells->data()->getBoundary().alpha()*180.0/pi)+"\n");
      string1.append("Beta: "+QString::number(qWells->data()->getBoundary().alpha())+"\n");
      string1.append("Referent well: "+qWells->data()->getWell(qWells->data()->getBoundary().refWell()).name()+"\n");
      string1.append("Distance to line 1: "+QString::number(qWells->data()->getBoundary().rho(0))+"\n");
      string1.append("Distance to line 2: "+QString::number(qWells->data()->getBoundary().rho(1))+"\n");
   }else if(type==4){
      string1="three lines\n";
      string1.append("Alpha: "+QString::number(qWells->data()->getBoundary().alpha()*180.0/pi)+"\n");
      string1.append("Referent well: "+qWells->data()->getWell(qWells->data()->getBoundary().refWell()).name()+"\n");
      string1.append("Distance to line 1: "+QString::number(qWells->data()->getBoundary().rho(0))+"\n");
      string1.append("Distance to line 2: "+QString::number(qWells->data()->getBoundary().rho(1))+"\n");
      string1.append("Distance to line 3: "+QString::number(qWells->data()->getBoundary().rho(1))+"\n");
   }else if(type==5){
      string1="four lines\n";
      string1.append("Alpha: "+QString::number(qWells->data()->getBoundary().alpha()*180.0/pi)+"\n");
      string1.append("Referent well: "+qWells->data()->getWell(qWells->data()->getBoundary().refWell()).name()+"\n");
      string1.append("Distance to line 1: "+QString::number(qWells->data()->getBoundary().rho(0))+"\n");
      string1.append("Distance to line 2: "+QString::number(qWells->data()->getBoundary().rho(1))+"\n");
      string1.append("Distance to line 3: "+QString::number(qWells->data()->getBoundary().rho(2))+"\n");
      string1.append("Distance to line 4: "+QString::number(qWells->data()->getBoundary().rho(3))+"\n");
   }

   plain->insertPlainText(string1);

   plain->setReadOnly(true);   
   
   layout->addWidget(plain);
   setLayout(layout);

   exec();

}
