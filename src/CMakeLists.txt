# src/CMakeLists.txt

include_directories(/usr/include/taglib)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(mpb
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
        add_library(mpb SHARED ${PROJECT_SOURCES})
    else()
        add_executable(mpb ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(mpb
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    tag
)

set_target_properties(mpb PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tocaE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

install(TARGETS mpb
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(mpb)
endif()

