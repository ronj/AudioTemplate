INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${LIBRARIES_INCLUDE_DIR})

SET(NODES_SOURCES utils/indexer.cpp
                  models/node.cpp
                  models/field.cpp
                  models/connection.cpp
                  collections/node_collection.cpp
                  collections/field_collection.cpp)

ADD_LIBRARY(nodes ${LIBRARY_TYPE} ${NODES_SOURCES})

SET(NODES_LIBRARY nodes CACHE INTERNAL "Nodes library" FORCE)

_SUBDIRS(tests)
