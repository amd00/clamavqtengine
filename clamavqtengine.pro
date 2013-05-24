
/*
 *  Copyright (C) 2013 Andrey Dudakov
 *
 *  Authors: Andrey "amd00" Dudakov
 *
 *  This file is part of clamavqtengine.
 *
 *  clamavqtengine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  clamavqtengine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with clamavqtengine.  If not, see <http://www.gnu.org/licenses/>.
 */

OBJECTS_DIR = .obj
MOC_DIR = .moc

QT -= gui
TEMPLATE = lib
VERSION = 1.2.1
TARGET = clamavqtengine
INCLUDEPATH += .
LIBS += -lclamav

target.path = /usr/local/lib
includes.files = *.h
includes.path = /usr/local/include

INSTALLS += target includes

# Input
HEADERS += clamavengine.h   dirscanner.h   filescanner.h   memscanner.h   scanner.h
SOURCES += clamavengine.cpp dirscanner.cpp filescanner.cpp memscanner.cpp
