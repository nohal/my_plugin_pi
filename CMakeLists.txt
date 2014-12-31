##---------------------------------------------------------------------------
## Author:      nohal aka. Pavel Kalian
## Copyright:
## License:     wxWidgets License
##---------------------------------------------------------------------------

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(aisradar_pi)

SET(PACKAGE_NAME aisradar_pi)
SET(VERBOSE_NAME AISRadar)
SET(TITLE_NAME AISRadar)
SET(CPACK_PACKAGE_CONTACT "Johan Sman")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "98")

#SET(CMAKE_BUILD_TYPE Debug)

INCLUDE("cmake/PluginConfigure.cmake")

SET(SRC_AISRADAR
      src/my_plugin_pi.h
      src/my_plugin_pi.cpp
      src/icons.h
      src/icons.cpp
      
    )
ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_AISRADAR})

INCLUDE("cmake/PluginInstall.cmake")

