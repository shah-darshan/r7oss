INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR} )

ADD_LIBRARY(also MODULE initng_also.c)
TARGET_LINK_LIBRARIES(also initng)
INSTALL_TARGETS(${LIB_INSTALL_DIR}/initng also)

