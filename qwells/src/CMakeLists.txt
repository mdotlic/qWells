project(qWells)

cmake_minimum_required(VERSION 2.8)

# FIND_PACKAGE (Threads REQUIRED)

include_directories (
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set your files and resources here
set( Srcs
      main.cpp
      license.cpp
      qWells.cpp
      centralWindow.cpp
      mainMenu.cpp
      dialogs/message.cpp
      dialogs/about.cpp
      engine/superPos.cpp
      engine/data.cpp
      parameterTab/parameterTab.cpp
      parameterTab/wellPiezoTab.cpp
      parameterTab/boundaryWidget.cpp
      parameterTab/wellsTab/wellsTab.cpp
      parameterTab/wellsTab/wellsTable.cpp
      parameterTab/wellsTab/wellsDataTable.cpp
      parameterTab/piezoTab/piezoTab.cpp
      parameterTab/piezoTab/piezoTable.cpp
      parameterTab/piezoTab/piezoDataTable.cpp
      parameterTab/boundaryWidget/boundaryDataFrame.cpp
      parameterTab/boundaryWidget/potentialComboBox.cpp
      parameterTab/picture/painterWidget.cpp
      parameterTab/picture/lineEditSetBoundary.cpp
      interactionTab/interactionTab.cpp
      interactionTab/muT/tMuSliderFrame.cpp
      interactionTab/muT/valueForTMuSlider.cpp
      interactionTab/tabbedTools/tabbedTools.cpp
      interactionTab/tabbedTools/chooserTab/plotChooser.cpp
      interactionTab/tabbedTools/chooserTab/wellChooserFrame.cpp
      interactionTab/tabbedTools/chooserTab/piezoChooserFrame.cpp
      interactionTab/tabbedTools/adjusterTab/adjuster.cpp
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster.cpp
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceSlider.cpp
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceValue.cpp
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceSlider.cpp
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceValue.cpp
      interactionTab/tabbedTools/adjusterTab/wellWearAdjuster.cpp
      interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearSlider.cpp
      interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearValue.cpp
      printTab/printTab.cpp
      visualizationTab/visualizationTab.cpp
      visualizationTab/visData.cpp
      visualizationTab/visPicture.cpp
      visualizationTab/visResult.cpp
      visualizationTab/isoList.cpp
      visualizationTab/linesTabs/isoDataTab.cpp
      visualizationTab/linesTabs/streamDataTab.cpp
      visualizationTab/linesTabs/tabs.cpp
      settings/settings.cpp
      settings/tableSettings.cpp
      settings/timeUnitSettings.cpp
      settings/plotSettings.cpp
      widgets/doubleValue.cpp
      widgets/minMaxDoubleValue.cpp
      widgets/tables/imExFrame.cpp
      widgets/tables/lineEdit.cpp
      widgets/tables/wellPiezoTable.cpp
      widgets/tables/wellPiezoDataTable.cpp
      widgets/tabTools/tabToolBase.cpp
      widgets/tabTools/collapsingBar.cpp
      widgets/qcustomplot/qcustomplot.cpp
      widgets/qcustomplot/qwellsPlot.cpp
      result/result.cpp
      result/finalParameters.cpp
      result/staticLevel.cpp
   )

set( Hdrs
      qWells.h
      license.h
      centralWindow.h
      mainMenu.h
      io.h
      helperFunctions.h
      dialogs/message.h
      dialogs/about.h
      engine/boundary.h
      engine/data.h
      engine/modelInformation.h
      engine/position.h
      engine/superPos.h
      engine/twoDgeom.h
      engine/well.h
      parameterTab/parameterTab.h
      parameterTab/wellPiezoTab.h
      parameterTab/tableSize.h
      parameterTab/boundaryWidget.h
      parameterTab/wellsTab/wellsTab.h
      parameterTab/wellsTab/wellsTable.h
      parameterTab/wellsTab/wellsDataTable.h
      parameterTab/piezoTab/piezoTab.h
      parameterTab/piezoTab/piezoTable.h
      parameterTab/piezoTab/piezoDataTable.h
      parameterTab/boundaryWidget/boundaryDataFrame.h
      parameterTab/boundaryWidget/potentialComboBox.h
      parameterTab/picture/painterWidget.h
      parameterTab/picture/lineEditSetBoundary.h
      interactionTab/interactionTab.h
      interactionTab/muT/tMuSliderFrame.h
      interactionTab/muT/valueForTMuSlider.h
      interactionTab/tabbedTools/tabbedTools.h
      interactionTab/tabbedTools/chooserTab/plotChooser.h
      interactionTab/tabbedTools/chooserTab/wellChooserFrame.h
      interactionTab/tabbedTools/chooserTab/piezoChooserFrame.h
      interactionTab/tabbedTools/adjusterTab/adjuster.h
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster.h
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceSlider.h
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/distanceValue.h
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceSlider.h
      interactionTab/tabbedTools/adjusterTab/boundaryAdjuster/maxDistanceValue.h
      interactionTab/tabbedTools/adjusterTab/wellWearAdjuster.h
      interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearSlider.h
      interactionTab/tabbedTools/adjusterTab/wellWearAdjuster/wearValue.h
      printTab/printTab.h
      visualizationTab/visualizationTab.h
      visualizationTab/visData.h
      visualizationTab/visPicture.h
      visualizationTab/visResult.h
      visualizationTab/isoList.h
      visualizationTab/linesTabs/isoDataTab.h
      visualizationTab/linesTabs/streamDataTab.h
      visualizationTab/linesTabs/tabs.h
      settings/settings.h
      settings/tableSettings.h
      settings/timeUnitSettings.h
      settings/plotSettings.h
      widgets/doubleValue.h
      widgets/minMaxDoubleValue.h
      widgets/tables/imExFrame.h
      widgets/tables/lineEdit.h
      widgets/tables/wellPiezoTable.h
      widgets/tables/wellPiezoDataTable.h
      widgets/tabTools/tabToolBase.h
      widgets/tabTools/collapsingBar.h
      widgets/qcustomplot/qcustomplot.h
      widgets/qcustomplot/qwellsPlot.h
      result/result.h
      result/finalParameters.h
      result/staticLevel.h
   )

set( MOC_Hdrs qWells.h )

set( UIs )

set( QRCs 
      qWells.qrc
)


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "-Wall -W -pipe -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -Wp,-D_FORTIFY_SOURCE=2 -fno-omit-frame-pointer")

# The rest should just work (sure...)
# We have ui files, this will bring in the macro: qt5_wrap_ui
find_package(Qt5Widgets REQUIRED QUIET)
find_package(Qt5LinguistTools REQUIRED)
qt5_wrap_ui(UI_Srcs ${UIs})
qt5_add_resources(QRC_Srcs ${QRCs} )
SET(Lang_files
  qWells_sr.ts
)
qt5_create_translation(LangSrcs ${Srcs} ${Hdrs} ${QRCs} ${Lang_files})
#qt5_add_translation(LangSrcs ${Lang_files}) # FIXME This does not seem to do anything

source_group("Resources" FILES
      ${UIs}
      ${QRCs}
      ${EXE_ICON} # Not present
      )

source_group("Generated" FILES
      ${UI_Srcs}
      ${MOC_Srcs}
      ${QRC_Srcs}
      )
if(MINGW)
add_executable(qWells_0.1.4 WIN32
      ${Srcs} ${Hdrs} ${UI_Srcs} ${MOC_Hdrs} ${QRC_Srcs} ${LangSrc})
      qt5_use_modules(qWells_0.1.4 Core Gui Widgets PrintSupport)
else()
add_executable(qWells 
      ${Srcs} ${Hdrs} ${UI_Srcs} ${MOC_Hdrs} ${QRC_Srcs} ${LangSrc})
      qt5_use_modules(qWells Core Gui Widgets PrintSupport)
endif()
# target_link_libraries(qWells ${CMAKE_THREAD_LIBS_INIT})
