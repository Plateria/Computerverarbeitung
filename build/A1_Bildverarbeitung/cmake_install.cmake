# Install script for directory: C:/CGBVRahmenprogramm/A1_Bildverarbeitung

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/CGBVRahmenprogramm/Abgabe")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/A1_Bildverarbeitung" TYPE EXECUTABLE FILES "C:/CGBVRahmenprogramm/build/A1_Bildverarbeitung/Debug/A1_Bildverarbeitung.exe")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/A1_Bildverarbeitung" TYPE EXECUTABLE FILES "C:/CGBVRahmenprogramm/build/A1_Bildverarbeitung/Release/A1_Bildverarbeitung.exe")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xA1_Bildverarbeitungx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/A1_Bildverarbeitung" TYPE FILE FILES
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/ImageProcessing.cpp"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderBewMit.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/VertexShader.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderBrightness_Contrast.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderDilatation.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderErosion.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderGauss3x3.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderGauss5x5.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderGauss7x7.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderLaplace.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderPassthrough.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderSharpen.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/FragmentShaderSobel.glsl"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/Timer.h"
    "C:/CGBVRahmenprogramm/A1_Bildverarbeitung/Timer.cpp"
    )
endif()

