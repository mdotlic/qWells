
TEMPLATE = app
TARGET = qWells_1.3.29

CONFIG += qt \
c++11

RESOURCES = qWells.qrc

QT += widgets printsupport 
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3
#INCLUDEPATH += /home/mdotlic/Qt/5.5/gcc_64/lib/cmake /home/mdotlic/Qt/5.5/gcc_64/include

SOURCES += main.cpp \
           qWells.cpp \
           centralWindow.cpp \
           mainMenu.cpp \
           dialogs/message.cpp \
           dialogs/about.cpp \
           engine/superPos.cpp \
           engine/data.cpp \
           parameterTab/parameterTab.cpp \
           parameterTab/wellPiezoTab.cpp \
           parameterTab/boundaryWidget.cpp \
           parameterTab/wellsTab/wellsTab.cpp \
           parameterTab/wellsTab/wellsTable.cpp \
           parameterTab/wellsTab/wellsDataTable.cpp \
           parameterTab/piezoTab/piezoTab.cpp \
           parameterTab/piezoTab/piezoTable.cpp \
           parameterTab/piezoTab/piezoDataTable.cpp \
           parameterTab/boundaryWidget/boundaryDataFrame.cpp \
           parameterTab/boundaryWidget/potentialComboBox.cpp \
           parameterTab/picture/painterWidget.cpp \
           parameterTab/picture/lineEditSetBoundary.cpp \
           interactionTab/interactionTab.cpp \
           interactionTab/muT/tMuSliderFrame.cpp \
           interactionTab/muT/valueForTMuSlider.cpp \
           interactionTab/tabbedTools/tabbedTools.cpp \
           interactionTab/tabbedTools/chooserTab/plotChooser.cpp \
           interactionTab/tabbedTools/chooserTab/wellChooserFrame.cpp \
           interactionTab/tabbedTools/chooserTab/piezoChooserFrame.cpp \
           interactionTab/tabbedTools/adjusterTab/adjuster.cpp \
           interactionTab/tabbedTools/adjusterTab/boundaryAdjuster.cpp \
           interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceSlider.cpp \
           interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceValue.cpp \
           interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceSlider.cpp \
           interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceValue.cpp \
           interactionTab/tabbedTools/adjusterTab/wellWearAdjuster.cpp \
           interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearSlider.cpp \
           interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearValue.cpp \
           visualizationTab/visualizationTab.cpp \
           visualizationTab/visData.cpp \
           visualizationTab/visPicture.cpp \
           visualizationTab/visResult.cpp \
           visualizationTab/isoList.cpp \
           visualizationTab/linesTabs/isoDataTab.cpp \
           visualizationTab/linesTabs/streamDataTab.cpp \
           visualizationTab/linesTabs/tabs.cpp \
           settings/settings.cpp \
           settings/tableSettings.cpp \
           settings/timeUnitSettings.cpp \
           settings/plotSettings.cpp \
           printTab/printTab.cpp \
           widgets/doubleValue.cpp \
           widgets/minMaxDoubleValue.cpp \
           widgets/tables/imExFrame.cpp \
           widgets/tables/lineEdit.cpp \
           widgets/tables/wellPiezoTable.cpp \
           widgets/tables/wellPiezoDataTable.cpp \
           widgets/tabTools/tabToolBase.cpp \
           widgets/tabTools/collapsingBar.cpp \
           widgets/qcustomplot/qcustomplot.cpp \
           widgets/qcustomplot/qwellsPlot.cpp \
           result/result.cpp \
           result/finalParameters.cpp \
           result/staticLevel.cpp \

HEADERS += qWells.h \
   centralWindow.h \
   mainMenu.h \
   io.h \
   helperFunctions.h \
   dialogs/message.h \
   dialogs/about.h \
   engine/boundary.h \
   engine/data.h \ 
   engine/modelInformation.h \
   engine/position.h \
   engine/superPos.h \
   engine/twoDgeom.h \
   engine/well.h \
   parameterTab/parameterTab.h \
   parameterTab/wellPiezoTab.h \
   parameterTab/tableSize.h \
   parameterTab/boundaryWidget.h \
   parameterTab/wellsTab/wellsTab.h \
   parameterTab/wellsTab/wellsTable.h \
   parameterTab/wellsTab/wellsDataTable.h \
   parameterTab/piezoTab/piezoTab.h \
   parameterTab/piezoTab/piezoTable.h \
   parameterTab/piezoTab/piezoDataTable.h \
   parameterTab/boundaryWidget/boundaryDataFrame.h \
   parameterTab/boundaryWidget/potentialComboBox.h \
   parameterTab/picture/painterWidget.h \
   parameterTab/picture/lineEditSetBoundary.h \
   interactionTab/interactionTab.h \
   interactionTab/muT/tMuSliderFrame.h \
   interactionTab/muT/valueForTMuSlider.h \
   interactionTab/tabbedTools/tabbedTools.h \
   interactionTab/tabbedTools/chooserTab/plotChooser.h \
   interactionTab/tabbedTools/chooserTab/wellChooserFrame.h \
   interactionTab/tabbedTools/chooserTab/piezoChooserFrame.h \
   interactionTab/tabbedTools/adjusterTab/adjuster.h \
   interactionTab/tabbedTools/adjusterTab/boundaryAdjuster.h \
   interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceSlider.h \
   interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceValue.h \
   interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceSlider.h \
   interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceValue.h \
   interactionTab/tabbedTools/adjusterTab/wellWearAdjuster.h \
   interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearSlider.h \
   interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearValue.h \
   visualizationTab/visualizationTab.h \
   visualizationTab/visData.h \
   visualizationTab/visPicture.h \
   visualizationTab/visResult.h \
   visualizationTab/isoList.h \
   visualizationTab/linesTabs/isoDataTab.h \
   visualizationTab/linesTabs/streamDataTab.h \
   visualizationTab/linesTabs/tabs.h \
   settings/settings.h \
   settings/tableSettings.h \
   settings/timeUnitSettings.h \
   settings/plotSettings.h \
   printTab/printTab.h \
   widgets/doubleValue.h \
   widgets/minMaxDoubleValue.h \
   widgets/tables/imExFrame.h \
   widgets/tables/lineEdit.h \
   widgets/tables/wellPiezoTable.h \
   widgets/tables/wellPiezoDataTable.h \
   widgets/tabTools/tabToolBase.h \
   widgets/tabTools/collapsingBar.h \
   widgets/qcustomplot/qcustomplot.h \
   widgets/qcustomplot/qwellsPlot.h \
   result/result.h \
   result/finalParameters.h \
   result/staticLevel.h
      

 
