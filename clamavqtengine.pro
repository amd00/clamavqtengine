OBJECTS_DIR = .obj
MOC_DIR = .moc

QT -= gui
TEMPLATE = lib
VERSION = 1.0.0
TARGET = clamavqtengine
INCLUDEPATH += .
LIBS += -lclamav

# Input
HEADERS += clamavengine.h dirscanner.h filescanner.h
SOURCES += clamavengine.cpp dirscanner.cpp filescanner.cpp
