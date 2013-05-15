OBJECTS_DIR = .obj
MOC_DIR = .moc

QT -= gui
TEMPLATE = lib
VERSION = 1.1.0
TARGET = clamavqtengine
INCLUDEPATH += .
LIBS += -lclamav

target.path = /usr/local/lib
includes.files = *.h
includes.path = /usr/local/include

INSTALLS += target includes

# Input
HEADERS += clamavengine.h   dirscanner.h   filescanner.h   memscanner.h
SOURCES += clamavengine.cpp dirscanner.cpp filescanner.cpp memscanner.cpp
