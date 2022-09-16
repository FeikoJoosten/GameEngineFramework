# Install script for directory: D:/Projects/GameEngineFramework/ThirdParty/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
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

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.4-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/lib/Debug/assimp-vc143-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/lib/Release/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/lib/MinSizeRel/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/lib/RelWithDebInfo/assimp-vc143-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.4" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/bin/Debug/assimp-vc143-mtd.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/bin/Release/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/bin/MinSizeRel/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/bin/RelWithDebInfo/assimp-vc143-mt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/anim.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/aabb.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/ai_assert.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/camera.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/color4.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/color4.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/config.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/ColladaMetaData.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/commonMetaData.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/defs.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/cfileio.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/light.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/material.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/material.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/matrix3x3.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/matrix3x3.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/matrix4x4.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/matrix4x4.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/mesh.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/ObjMaterial.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/pbrmaterial.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/GltfMaterial.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/postprocess.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/quaternion.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/quaternion.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/scene.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/metadata.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/texture.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/types.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/vector2.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/vector2.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/vector3.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/vector3.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/version.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/cimport.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/importerdesc.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Importer.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/DefaultLogger.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/ProgressHandler.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/IOStream.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/IOSystem.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Logger.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/LogStream.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/NullLogger.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/cexport.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Exporter.hpp"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/DefaultIOStream.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/DefaultIOSystem.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/SceneCombiner.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/fast_atof.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/qnan.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/BaseImporter.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Hash.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/ParsingUtils.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/StreamReader.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/StreamWriter.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/StringComparison.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/StringUtils.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/SGSpatialSort.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/GenericProperty.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/SpatialSort.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/SmallVector.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/SmoothingGroups.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/SmoothingGroups.inl"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/StandardShapes.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/RemoveComments.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Subdivision.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Vertex.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/LineSplitter.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/TinyFormatter.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Profiler.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/LogAux.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Bitmap.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/XMLTools.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/IOStreamBuffer.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/CreateAnimMesh.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/XmlParser.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/BlobIOSystem.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/MathFunctions.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Exceptional.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/ByteSwapper.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Compiler/poppack1.h"
    "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/Debug/assimp-vc143-mtd.pdb")
  endif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Projects/GameEngineFramework/ThirdParty/assimp/code/RelWithDebInfo/assimp-vc143-mt.pdb")
  endif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
endif()

