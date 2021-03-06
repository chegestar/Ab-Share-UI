CONFIG += qt debug
QT -= gui
TEMPLATE = lib

# profiling
CONFIG(profiling) {
    message(Building with profiling)
    QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs
    LIBS += -lgcov
}

DEFINES += MDURI_EXPORTS
QMAKE_CXXFLAGS += -O2 -Werror -Wall

VER_MAJ=0
VER_MIN=2
VER_PAT=0

TARGET = mdatauri

DEPENDPATH  += ./src
# ./src is where the private files will be kept
INCLUDEPATH += ./inc \
               ./src

VPATH += $$DEPENDPATH $$INCLUDEPATH

OBJECTS_DIR = ./obj
MOC_DIR = ./moc
DESTDIR = ./out
QMAKE_CLEAN += obj/* \
               out/* \
               moc/*

HEADERS += mdatauri.h
             
SOURCES += mdatauri.cpp
                
# Install binary application
target.path = /usr/lib

# Install public headers
pubheaders.path = /usr/include/MDataUri
pubheaders.files = inc/*
INSTALLS += pubheaders

# Install prf file for others to use
prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
prf.files = mdatauri.prf
INSTALLS += prf
            
# Install pkgconfig file for other to use
pkgconfigpc.path = /usr/lib/pkgconfig
pkgconfigpc.files = mdatauri.pc
INSTALLS += pkgconfigpc

INSTALLS += target

# API Documentation with Doxygen
dox.path        = /usr/share/doc/mdatauri
dox.commands    = doxygen doxygen.cfg
dox.depends     = inc
dox.files       = doc/html/*
dox.CONFIG     += no_check_exist
INSTALLS       += dox
QMAKE_EXTRA_TARGETS += dox

