
cmake_minimum_required(VERSION 2.8)

project(clamavqtengine)
find_package(Qt4 REQUIRED)

set(SOURCE_LIB clamavengine.cpp  dirscanner.cpp  filescanner.cpp  memscanner.cpp)
set(HEADER_LIB clamavengine.h  dirscanner.h  filescanner.h  memscanner.h  scanner.h)

QT4_WRAP_CPP(HEADERS_MOC ${HEADER_LIB})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_library(clamavqtengineS STATIC ${SOURCE_LIB} ${HEADERS_MOC})
add_library(clamavqtengine SHARED ${SOURCE_LIB} ${HEADERS_MOC})

target_link_libraries(clamavqtengine clamav)
target_link_libraries(clamavqtengineS clamav)

set_target_properties(clamavqtengine PROPERTIES
	  VERSION 1.0.0
	  SOVERSION 1.0
	)

INSTALL(TARGETS clamavqtengineS clamavqtengine
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

INSTALL(FILES ${HEADER_LIB} 
	DESTINATION include)
