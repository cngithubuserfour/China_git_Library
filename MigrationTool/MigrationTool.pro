TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        MigrationTool.cpp \
        applog.cpp \
        file.cpp \
        filelist.cpp \
        main.cpp \
        os.cpp \
        parameters.cpp

HEADERS += \
    MigrationTool.h \
    applog.h \
    file.h \
    filelist.h \
    os.h \
    parameters.h \
    sqlparserexp.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Parser/release/ -lParser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Parser/debug/ -lParser
else:unix:!macx: LIBS += -L$$OUT_PWD/../Parser/ -lParser

INCLUDEPATH += $$PWD/../Parser
DEPENDPATH += $$PWD/../Parser

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Parser/release/libParser.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Parser/debug/libParser.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Parser/release/Parser.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Parser/debug/Parser.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../Parser/libParser.a
