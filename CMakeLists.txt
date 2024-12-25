cmake_minimum_required(VERSION 3.16)

project(flightmanager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql)

set(TS_FILES flightmanager_ru_RU.ts)

set(PROJECT_SOURCES
        main.cpp

        LoginPage.ui



        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(flightmanager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
         DataBase.h Flight.h  Queue.h
         DataBase.cpp Flight.cpp
        sqlite3.h LoginPage.cpp LoginPage.h






        AdminPanell.h AdminPanell.cpp AdminPanell.ui
        FilterTable.h FilterTable.cpp
        UserPanell.h UserPanell.cpp UserPanell.ui
        BookedFlightsWindow.h BookedFlightsWindow.cpp BookedFlightsWindow.ui
        BasePanel.h BasePanel.cpp


        ValidatedSqlTableModel.h ValidatedSqlTableModel.cpp
        ManagerException.h
        VectorIt.h
        DataSaver.h
        AirCraft.h AirCraft.cpp
        BookedFlight.h
        FlightBooking.h FlightBooking.cpp FlightBooking.ui

        PanelInterface.h
        Passenger.h Passenger.cpp
        BookedFlight.cpp





    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET flightmanager APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(flightmanager SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(flightmanager
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(flightmanager PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.flightmanager)
endif()
set_target_properties(flightmanager PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS flightmanager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(flightmanager)
endif()


