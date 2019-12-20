QT -= gui

TEMPLATE = lib
DEFINES += PARSER_LIBRARY

CONFIG += staticlib

SOURCES += \
    clauses.cpp \
    datatypes.cpp \
    dllmain.cpp \
    file.cpp \
    functions.cpp \
    guess.cpp \
    helpers.cpp \
    language.cpp \
    oracle.cpp \
    patterns.cpp \
    post.cpp \
    report.cpp \
    select.cpp \
    sqlparser.cpp \
    statements.cpp \
    stats.cpp \
    storage.cpp \
    str.cpp \
    token.cpp

HEADERS += \
    doc.h \
    file.h \
    listt.h \
    listw.h \
    listwm.h \
    report.h \
    sqlparser.h \
    stats.h \
    str.h \
    token.h
