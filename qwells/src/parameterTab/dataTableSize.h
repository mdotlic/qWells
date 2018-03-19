#ifndef DATATABLESIZE_H
#define DATATABLESIZE_H
namespace dataTableSize 
{
   int wellTableWidth;
   int piezoTableWidth;
   int hHeaderHeight;
   bool wellTableWidthSet = false;
   bool piezoTableWidthSet = false;
   bool heightSet = false;
   
   inline void setWidth(const int w, const int columnNo)
   {
      if (columnNo == 5)
      {
         if (!wellTableWidthSet)
         {
            wellTableWidth = w;
            wellTableWidthSet = true;
         }
      }
      else if (columnNo == 2)
      {
         if (!piezoTableWidthSet)
         {
            piezoTableWidth = w;
            piezoTableWidthSet = true;
         }
      }
   }
   
   inline void setHeaderHeight(const int h)
   {
      if (!heightSet) 
      {
         hHeaderHeight = h;
         heightSet = true;
      }
   }
   
   inline int getWidth(const int columnNo)
   {
      if (columnNo == 5) return wellTableWidth;
      else if (columnNo == 2) return piezoTableWidth;
      else return -1;
   }
   
   inline void resetAll()
   {
      wellTableWidthSet = false;
      piezoTableWidthSet = false;
      heightSet = false;
   }
}
#endif // DATATABLESIZE_H