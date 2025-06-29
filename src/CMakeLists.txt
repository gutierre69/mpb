# src/CMakeLists.txt

include_directories(/usr/include/taglib)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MPB
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
        waveform.h waveform.cpp
        audioplayer.h audioplayer.cpp
        aboutdialog.ui
        aboutdialog.h aboutdialog.cpp
    )
else()
    if(ANDROID)
        add_library(MPB SHARED ${PROJECT_SOURCES})
    else()
        add_executable(MPB ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(MPB
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    tag
)

set_target_properties(MPB PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tocaE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS MPB
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MPB)
endif()

