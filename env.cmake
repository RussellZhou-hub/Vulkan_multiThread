if (WIN32)
    set(GLFW_PATH ./3rdParty)
    set(GLM_PATH ./3rdParty/glm)
    set(OBJ_LOADER_PATH ./3rdParty/objLoader)
    set(VULKAN_SDK_PATH  D:/Data/VulkanSDK/1.2.198.1)
elseif (UNIX)
    
    find_package(glfw3 3.3 REQUIRED)
    set(GLFW_LIB glfw)
    if (NOT GLFW_LIB)
        set(GLFW_PATH ./3rdParty/GLFW)
        message(FATAL_ERROR "Could not find glfw library!")
    else()
        message(STATUS "Using glfw lib at: ${GLFW_LIB}")
    endif()
    set(GLM_PATH ./3rdParty/glm)
    set(OBJ_LOADER_PATH ./3rdParty/objLoader)

    find_package(Vulkan REQUIRED FATAL_ERROR)
endif()
 

# Set MINGW_PATH if using mingwBuild.bat and not VisualStudio20XX
# set(MINGW_PATH "C:/Program Files/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64")
 
# Optional set TINYOBJ_PATH to target specific version, otherwise defaults to external/tinyobjloader
# set(TINYOBJ_PATH X:/dev/Libraries/tinyobjloader)