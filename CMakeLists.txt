project(MaterialX)
set(MATERIALX_MAJOR_VERSION 1)
set(MATERIALX_MINOR_VERSION 37)
set(MATERIALX_BUILD_VERSION 0)
set(MATERIALX_LIBRARY_VERSION ${MATERIALX_MAJOR_VERSION}.${MATERIALX_MINOR_VERSION}.${MATERIALX_BUILD_VERSION})

cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_MACOSX_RPATH ON)
enable_testing()

option(MATERIALX_BUILD_PYTHON "Build the MaterialX Python package from C++ bindings. Requires Python 2.7 or greater." OFF)
option(MATERIALX_BUILD_VIEWER "Build the MaterialX Viewer." ON)
option(MATERIALX_BUILD_RUNTIME "Build the MaterialX Runtime data model library." OFF)
option(MATERIALX_BUILD_DOCS "Create HTML documentation using Doxygen. Requires that Doxygen be installed." OFF)
option(MATERIALX_PYTHON_LTO "Enable link-time optimizations for MaterialX Python." ON)
option(MATERIALX_INSTALL_PYTHON "Install the MaterialX Python package as a third-party library when the install target is built." ON)
option(MATERIALX_WARNINGS_AS_ERRORS "Interpret all compiler warnings as errors." OFF)

set (MATERIALX_DEBUG_POSTFIX "" CACHE STRING 
    "Postfix to append to the target file name for debug configuration (e.g. 'd').")
set(MATERIALX_PYTHON_VERSION "" CACHE STRING
    "Python version to be used in building the MaterialX Python package (e.g. '2.7').")
set(MATERIALX_PYTHON_EXECUTABLE "" CACHE FILEPATH
    "Python executable to be used in building the MaterialX Python package (e.g. 'C:/Python27/python.exe').")
set(MATERIALX_PYTHON_OCIO_DIR "" CACHE PATH
    "Path to a folder containing the default OCIO configuration to be packaged with MaterialX Python (e.g. 'D:/Projects/OpenColorIO-Configs/aces_1.0.3').")
set(MATERIALX_PYTHON_PYBIND11_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source/PyMaterialX/PyBind11" CACHE PATH
    "Path to a folder containing the PyBind11 source. Defaults to the included PyBind11 source, which has been extended to support Python 2.6.")

# Settings to define installation layout    
set (MATERIALX_INSTALL_INCLUDE_PATH "include" CACHE STRING 
    "Install header include path (e.g. 'inc', 'include').")
set (MATERIALX_INSTALL_LIB_PATH "lib" CACHE STRING 
    "Install lib path (e.g. 'libs', 'lib').")
set (MATERIALX_INSTALL_STDLIB_PATH "libraries" CACHE STRING 
    "Install path for mtlx std libs (e.g. 'libraries').")    

mark_as_advanced(MATERIALX_BUILD_DOCS)
mark_as_advanced(MATERIALX_PYTHON_LTO)
mark_as_advanced(MATERIALX_INSTALL_PYTHON)
mark_as_advanced(MATERIALX_WARNINGS_AS_ERRORS)
mark_as_advanced(MATERIALX_DEBUG_POSTFIX)
mark_as_advanced(MATERIALX_PYTHON_VERSION)
mark_as_advanced(MATERIALX_PYTHON_EXECUTABLE)
mark_as_advanced(MATERIALX_PYTHON_OCIO_DIR)
mark_as_advanced(MATERIALX_PYTHON_PYBIND11_DIR)

# Shading language generation options
option(MATERIALX_BUILD_GEN_OGSXML "Build the OGSXML shader generator back-end." ON)
mark_as_advanced(MATERIALX_BUILD_GEN_OGSXML)
option(MATERIALX_BUILD_GEN_OGSFX "Build the OGSFX shader generator back-end." ON)
mark_as_advanced(MATERIALX_BUILD_GEN_OGSFX)
option(MATERIALX_BUILD_GEN_ARNOLD "Build the Arnold OSL shader generator back-end." ON)
mark_as_advanced(MATERIALX_BUILD_GEN_ARNOLD)

# Disable render module building option
option(MATERIALX_DISABLE_BUILD_RENDER "Disable building render modules." OFF)
mark_as_advanced(MATERIALX_DISABLE_BUILD_RENDER)

# Validation options for compiling and rendering generated shaders
option(MATERIALX_TEST_RENDER "Run rendering tests for MaterialX Render module. GPU required for graphics validation." OFF)
mark_as_advanced(MATERIALX_TEST_RENDER)
if (MATERIALX_DISABLE_BUILD_RENDER)
    set(MATERIALX_TEST_RENDER OFF)
    set(MATERIALX_BUILD_VIEWER OFF)
    set(MATERIALX_BUILD_CONTRIB OFF)
endif()
option(MATERIALX_BUILD_OIIO "Build OpenImageIO support for MaterialXRender." OFF)
mark_as_advanced(MATERIALX_BUILD_OIIO)
set(MATERIALX_OIIO_DIR "" CACHE PATH "Path to the root folder of the OpenImageIO installation.")
mark_as_advanced(MATERIALX_OIIO_DIR)
option(MATERIALX_BUILD_CONTRIB "Build contribution folder." ON)
mark_as_advanced(MATERIALX_BUILD_CONTRIB)
option(MATERIALX_BUILD_CROSS "Build MaterialXCross folder." OFF)
mark_as_advanced(MATERIALX_BUILD_CROSS)

# Helpers for OSL validation
set(MATERIALX_OSLC_EXECUTABLE "" CACHE FILEPATH "Full path to the oslc binary.")
mark_as_advanced(MATERIALX_OSLC_EXECUTABLE)
add_definitions(-DMATERIALX_OSLC_EXECUTABLE=\"${MATERIALX_OSLC_EXECUTABLE}\")
set(MATERIALX_TESTRENDER_EXECUTABLE "" CACHE FILEPATH "Full path to the testrender binary.")
mark_as_advanced(MATERIALX_TESTRENDER_EXECUTABLE)
add_definitions(-DMATERIALX_TESTRENDER_EXECUTABLE=\"${MATERIALX_TESTRENDER_EXECUTABLE}\")
set(MATERIALX_OSL_INCLUDE_PATH "" CACHE PATH "Full path to osl include paths. e.g. location of stdosl.h")
mark_as_advanced(MATERIALX_OSL_INCLUDE_PATH)
add_definitions(-DMATERIALX_OSL_INCLUDE_PATH=\"${MATERIALX_OSL_INCLUDE_PATH}\")

# Adjust the default installation path
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/installed" CACHE PATH "Default install path" FORCE)
endif()

# Adjust compiler settings
if(MSVC)
    add_compile_options(/MP)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        add_compile_options(/W4)
    endif()
    if(MATERIALX_WARNINGS_AS_ERRORS)
        add_compile_options(/WX)
    endif()

    # Use /Z7 to avoid having pdbs
    if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/Zi")
        string(REGEX REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7")
    endif()
    if(CMAKE_CXX_FLAGS_RELEASE MATCHES "/Zi")
        string(REGEX REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Z7")
    endif()
    if(CMAKE_CXX_FLAGS_RELWITHDEBINFO MATCHES "/Zi")
        string(REGEX REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    else()
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Z7")
    endif()
else()
    add_compile_options(-Wall -Wno-missing-braces)
    if(MATERIALX_WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()
endif()

# Shared functions
function(assign_source_group prefix)
    foreach(_source IN ITEMS ${ARGN})
        if(IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${prefix}\\${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

# Add core subdirectories
add_subdirectory(source/MaterialXCore)
add_subdirectory(source/MaterialXFormat)

# Add shader generation subdirectories
add_subdirectory(source/MaterialXGenShader)
add_subdirectory(source/MaterialXGenOsl)
add_subdirectory(source/MaterialXGenGlsl)

if (MATERIALX_BUILD_CROSS)
    add_definitions(-DMATERIALX_BUILD_CROSS)
    add_subdirectory(source/MaterialXCross)
endif()

if (MATERIALX_BUILD_GEN_OGSXML)
    add_definitions(-DMATERIALX_BUILD_GEN_OGSXML)
    add_subdirectory(source/MaterialXGenOgsXml)
endif()
if (MATERIALX_BUILD_GEN_OGSFX)
    add_definitions(-DMATERIALX_BUILD_GEN_OGSFX)
    add_subdirectory(source/MaterialXGenOgsFx)
endif()
if (MATERIALX_BUILD_GEN_ARNOLD)
    add_definitions(-DMATERIALX_BUILD_GEN_ARNOLD)
    add_subdirectory(source/MaterialXGenArnold)
endif()

# Add render module subdirectories
if(MATERIALX_BUILD_OIIO)
    add_definitions(-DMATERIALX_BUILD_OIIO)
    add_definitions(-DOPENIMAGEIO_ROOT_DIR=\"${OPENIMAGEIO_ROOT_DIR}\")
endif()
if (MATERIALX_DISABLE_BUILD_RENDER)
    add_definitions(-DMATERIALX_DISABLE_BUILD_RENDER)    
else()
    add_subdirectory(source/MaterialXRender)
    add_subdirectory(source/MaterialXRenderOsl)
    add_subdirectory(source/MaterialXRenderHw)
    add_subdirectory(source/MaterialXRenderGlsl)
endif()

if (MATERIALX_BUILD_CONTRIB)
    add_definitions(-DMATERIALX_BUILD_CONTRIB)
    add_subdirectory(source/MaterialXContrib)
endif()

if(MATERIALX_TEST_RENDER)
    add_definitions(-DMATERIALX_TEST_RENDER)
endif()

# Add viewer subdirectory
if(MATERIALX_BUILD_VIEWER)
    add_subdirectory(source/MaterialXView)
endif()

# Add runtime subdirectory
if(MATERIALX_BUILD_RUNTIME)
    add_definitions(-DMATERIALX_BUILD_RUNTIME)
    add_subdirectory(source/MaterialXRuntime)
endif()

# Add test subdirectory
if (NOT MATERIALX_DISABLE_BUILD_RENDER)
    add_subdirectory(source/MaterialXTest)
endif()

# Add resource and library subdirectories
add_subdirectory(libraries)
add_subdirectory(resources)

if(MATERIALX_BUILD_PYTHON)
    add_subdirectory(source/PyMaterialX)
    add_subdirectory(python)
endif()
if(MATERIALX_BUILD_DOCS)
    add_subdirectory(documents)
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.6.2")
    if(MATERIALX_BUILD_VIEWER)
        set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT MaterialXView)
    else()
        if (NOT MATERIALX_DISABLE_BUILD_RENDER)
            set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT MaterialXTest)
        endif()
    endif()
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/MaterialXConfigVersion.cmake
    VERSION ${CMAKE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Package for using the developer build
export(
    EXPORT MaterialX
    FILE ${CMAKE_BINARY_DIR}/cmake/MaterialXConfig.cmake
    )

# Package for using the installed build
install(
    EXPORT      MaterialX
    DESTINATION cmake
    FILE        MaterialXConfig.cmake)
install(
    FILES "${CMAKE_BINARY_DIR}/cmake/MaterialXConfigVersion.cmake"
    DESTINATION cmake)
