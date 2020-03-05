file(GLOB pymaterialxgenshader_source "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB pymaterialxgenshader_headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

pybind11_add_module(PyMaterialXGenShader ${PYBIND11_MODULE_FLAGS} ${pymaterialxgenshader_source} ${pymaterialxgenshader_headers})

if(APPLE)
    set_target_properties(PyMaterialXGenShader PROPERTIES CXX_VISIBILITY_PRESET "default")
endif()

set_target_properties(
    PyMaterialXGenShader
    PROPERTIES
    OUTPUT_NAME PyMaterialXGenShader
    COMPILE_FLAGS "${EXTERNAL_COMPILE_FLAGS}"
    LINK_FLAGS "${EXTERNAL_LINK_FLAGS}"
    DEBUG_POSTFIX "${MATERIALX_PYTHON_DEBUG_POSTFIX}")

target_link_libraries(
    PyMaterialXGenShader
    PRIVATE MaterialXCore
            MaterialXFormat
            MaterialXGenShader
            ${CMAKE_DL_LIBS})

install(TARGETS PyMaterialXGenShader
        LIBRARY DESTINATION "python/MaterialX")
