# Install script for directory: I:/Projects/Year 2/Y2017A-Y2-Team04/Projects/RTS Engine/ThirdParty/GainInput/samples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/Project")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("I:/Projects/Year 2/Y2017A-Y2-Team04/Projects/RTS Engine/ThirdParty/GainInput/build64/samples/basic/cmake_install.cmake")
  include("I:/Projects/Year 2/Y2017A-Y2-Team04/Projects/RTS Engine/ThirdParty/GainInput/build64/samples/dynamic/cmake_install.cmake")
  include("I:/Projects/Year 2/Y2017A-Y2-Team04/Projects/RTS Engine/ThirdParty/GainInput/build64/samples/gesture/cmake_install.cmake")
  include("I:/Projects/Year 2/Y2017A-Y2-Team04/Projects/RTS Engine/ThirdParty/GainInput/build64/samples/listener/cmake_install.cmake")
  include("I:/Projects/Year 2/Y2017A-Y2-Team04/Projects/RTS Engine/ThirdParty/GainInput/build64/samples/recording/cmake_install.cmake")
  include("I:/Projects/Year 2/Y2017A-Y2-Team04/Projects/RTS Engine/ThirdParty/GainInput/build64/samples/sync/cmake_install.cmake")

endif()

