set(searchpath ${FCCEDMEXT_DIR} ${PODIO_DIR}  )

# first find the FCC EDM
find_library(FCCEDMEXT_LIBRARY
              NAMES datamodelExt
              HINTS ${searchpath}
              PATH_SUFFIXES lib)

find_path(FCCEDMEXT_INCLUDE_DIR
           NAMES datamodelExt/Point.h
           HINTS ${searchpath} $ENV{FCCEDM_DIR}/include
           PATH_SUFFIXES include)

unset(searchpath)

set(FCCEDMEXT_INCLUDE_DIRS ${FCC_INCLUDE_DIR})
set(FCCEDMEXT_LIBRARIES ${FCCEDM_LIBRARY})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FCCEDM_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FCCEDMEXT DEFAULT_MSG FCCEDMEXT_INCLUDE_DIR FCCEDMEXT_LIBRARY)

mark_as_advanced(FCCEDMEXT_FOUND)
