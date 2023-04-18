cmake_minimum_required(VERSION 3.8)
project("VulkanRenderer" VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(./env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

include(CMakeDependentOption)

# set VULKAN_SDK_PATH to target specifical version vulkan
if(DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib")
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if not find vulkan
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Counld not find Vulkan Library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

# 2. Set GLFW_PATH in .env.cmake to target specific glfw
if (DEFINED GLFW_PATH)
  message(STATUS "Using GLFW path specified in .env")
  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}")
  if (MSVC)
    set(GLFW_LIB "${GLFW_PATH}/GLFW/lib-vc2019") # 2.1 Update lib-vc2019 to use same version as your visual studio
  elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")
    set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64") # 2.1 make sure matches glfw mingw subdirectory
  else() # g++ compiler on linux
    set(GLFW_LIB "${GLFW_PATH}/lib-linux")
    message(STATUS ${GLFW_LIB})
  endif()
else()
  find_package(glfw3 3.3 REQUIRED)
  set(GLFW_LIB glfw)
  message(STATUS "Found GLFW")
endif()
if (NOT GLFW_LIB)
    message(FATAL_ERROR "Could not find glfw library!")
else()
    message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCES_CPP ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(APPEND SOURCES ${SOURCES_CPP})
 
add_executable(${PROJECT_NAME} ${SOURCES})
#add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/engine.cpp ${PROJECT_SOURCE_DIR}/src/main.cpp)

message(STATUS "Sources: ${SOURCES}")
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# set cmake configuration types

set(CMAKE_CONFIGURATION_TYPES "DebugEditor;ReleaseEditor;DebugGame;ReleaseGame" CACHE STRING "possible configurations" FORCE)
# In case the user does not setup CMAKE_BUILD_TYPE, assume it's RelWithDebInfo
# In c++ code you can write: #ifdef EDITOR
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE DebugEditor CACHE STRING "build type default to DebugEditor" FORCE)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "DebugEditor" OR CMAKE_BUILD_TYPE STREQUAL "ReleaseEditor")
    add_definitions("-DEDITOR")
    add_definitions("-g")
    if (WIN32)
    #add_compile_options()
    target_compile_options(${PROJECT_NAME} PUBLIC "/ZI")
    target_link_options(${PROJECT_NAME} PUBLIC "/DEBUG")
    endif()
    
    #target_link_options(${PROJECT_NAME} PUBLIC "/ZI")
    #target_compile_definitions(${PROJECT_NAME} PUBLIC "/ZI")
	list(APPEND CMAKE_CXX_FLAGS "-g")
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")  # no exception
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")  # have exception

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")  # no Runtime Type Information

#set(CMAKE_CXX_FLAGS_DEBUGEDITOR "${CMAKE_CXX_FLAGS_DEBUG} -g ")  # 调试信息
list(APPEND CMAKE_CXX_FLAGS_DEBUGEDITOR "-g")

message(${CMAKE_CXX_FLAGS})

# end set cmake configuration types

if (WIN32)
  message(STATUS "CREATING BUILD FOR WINDOWS")
 
  if (USE_MINGW)
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/include
    )
    target_link_directories(${PROJECT_NAME} PUBLIC
      ${MINGW_PATH}/lib
    )
  endif()

  target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${Vulkan_INCLUDE_DIRS}
    ${TINYOBJ_PATH}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_PATH}
    ${OBJ_LOADER_PATH}
    )
 
  target_link_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
  )
 
  target_link_libraries(${PROJECT_NAME} glfw3 vulkan-1)
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    message(STATUS "Using glm at: ${GLM_PATH}")
    
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${PROJECT_SOURCE_DIR}/src
      ${TINYOBJ_PATH}
      ${GLM_PATH}
      ${OBJ_LOADER_PATH}
    )
    target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES}
    pthread
    )
endif()

set(VulkanRenderer_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_INSTALL_PREFIX "${VulkanRenderer_ROOT_DIR}/bin")
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


# [生成目录]
##禁止使用EXECUTABLE_OUTPUT_PATH、LIBRARY_OUTPUT_PATH的值，请使用Project_OUTPUT_DIR（因为VS环境与非VS环境不一样）
message(STATUS "[Project output directory] ${BINARY_ROOT_DIR}")

if(${CMAKE_GENERATOR} MATCHES "Visual Studio*")
    #在Visual Studio生成器（即VS工程）下，会在EXECUTABLE_OUTPUT_PATH、EXECUTABLE_OUTPUT_PATH后面自动加一个${CMAKE_BUILD_TYPE}
    SET(EXECUTABLE_OUTPUT_PATH ${BINARY_ROOT_DIR}) #可执行文件输出目录
    set(ARCHIVE_OUTPUT_DIRECTORY ${BINARY_ROOT_DIR})     #静态库文件输出目录
    set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})    #库文件输出目录
else()
    #在其他环境下，不会动，原来是什么就是什么
    SET(EXECUTABLE_OUTPUT_PATH ${BINARY_ROOT_DIR}${CMAKE_BUILD_TYPE})
    set(ARCHIVE_OUTPUT_DIRECTORY ${BINARY_ROOT_DIR}${CMAKE_BUILD_TYPE})
    set(LIBRARY_OUTPUT_PATH ${BINARY_ROOT_DIR}${CMAKE_BUILD_TYPE})
endif()

# [lib目录]
#set(Project_Lib_DIR ${Project_OUTPUT_DIR})
#link_directories(${LIBRARY_OUTPUT_PATH}) #全局，子模块中不需要添加了
#message(STATUS "[Project Library directory] ${Project_Lib_DIR}")


include_directories (${INCLUDE_DIRS})
# include_directories(${VulkanRenderer_ROOT_DIR}/EngineLib/)
# include_directories(${VulkanRenderer_ROOT_DIR}/EngineDLL/)


# add_subdirectory(EngineLib)
# add_subdirectory(EngineDLL)
# add_subdirectory(Editor)
# add_subdirectory(EngineTest)
#add_subdirectory(ContentTools)