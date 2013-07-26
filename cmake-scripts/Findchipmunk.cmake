
SET(CHIPMUNK_SEARCH_PATHS
    /usr/local/
    /usr
    /opt
)

FIND_PATH(CHIPMUNK_INCLUDE_DIR chipmunk/chipmunk.h 
    HINTS ${CHIPMUNK_ROOT}
    PATH_SUFFIXES include include/chipmunk
    PATHS ${CHIPMUNK_SEARCH_PATHS}
)
FIND_LIBRARY(CHIPMUNK_LIBRARY chipmunk 
    HINTS ${CHIPMUNK_ROOT}
    PATH_SUFFIXES lib64 lib bin
    PATHS ${CHIPMUNK_SEARCH_PATHS}
)

IF(CHIPMUNK_INCLUDE_DIR AND CHIPMUNK_LIBRARY)
   SET(CHIPMUNK_FOUND TRUE)
ENDIF (CHIPMUNK_INCLUDE_DIR AND CHIPMUNK_LIBRARY)


IF(CHIPMUNK_FOUND)
    MESSAGE(STATUS "Found Chipmunk: ${CHIPMUNK_LIBRARY}")
ELSE(CHIPMUNK_FOUND)
    MESSAGE(WARNING "Could not find Chipmunk")
ENDIF(CHIPMUNK_FOUND)