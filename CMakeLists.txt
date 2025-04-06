# ---------------------------------------------------- #
#                 Required envs assert                 #
# ---------------------------------------------------- #

#if(NOT DEFINED ENV{COMMONLIB_SSE_FOLDER})
#  message(FATAL_ERROR "Missing COMMONLIB_SSE_FOLDER environment variable")
#endif()

# ---------------------------------------------------- #
#               Imports and definitions                #
# ---------------------------------------------------- #

include(cmake/headerlist.cmake)
include(cmake/sourcelist.cmake)

cmake_minimum_required(VERSION 3.21)
set(PRODUCT_NAME "Diorama")
set(AUTHOR_NAME "QuanTiger")
set(BEAUTIFUL_NAME "Diorama - Plugin Creation Engine")
project(Diorama VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# ---------------------------------------------------- #
#                      Resources                       #
# ---------------------------------------------------- #

#configure_file(
#    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
#    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
#    @ONLY)
#
#set(RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
#set(RESOURCE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.res)
#
#add_custom_command(
#    OUTPUT ${RESOURCE_OUTPUT}
#    COMMAND rc /fo ${RESOURCE_OUTPUT} ${RESOURCE_FILE}
#    DEPENDS ${RESOURCE_FILE}
#)
#
#add_custom_target(Resource ALL DEPENDS ${RESOURCE_OUTPUT})

# ---------------------------------------------------- #
#                Plugin Configuration                  #
# ---------------------------------------------------- #

include(FetchContent)

set(ENABLE_SKYRIM_SE ON CACHE BOOL " " FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL " " FORCE)
set(ENABLE_SKYRIM_VR OFF CACHE BOOL " " FORCE)
set(BUILD_TESTS OFF CACHE BOOL " " FORCE)

add_subdirectory("$ENV{COMMONLIB_SSE_FOLDER}" ${CMAKE_CURRENT_BINARY_DIR}/_deps/clib-build)
get_target_property(COMMONLIB_SRC_DIR CommonLibSSE SOURCE_DIR)

find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/utils.hpp")

#file(
#  GLOB_RECURSE
#  sources
#  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
#  ${sources}
#)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${headers} ${sources}
)

include(${COMMONLIB_SRC_DIR}/cmake/CommonLibSSE.cmake)

#find_package(CommonLibSSE CONFIG REQUIRED)
find_package(directxtk CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(efsw CONFIG REQUIRED)

add_commonlibsse_plugin(
  ${PROJECT_NAME}
  AUTHOR AUTHOR_NAME
  SOURCES ${headers} ${sources}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23) # <--- use C++23 standard
target_precompile_headers(${PROJECT_NAME} PRIVATE include/PCH.h) # <--- PCH.h is required!

target_include_directories(
	"${PROJECT_NAME}"
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
    efsw::efsw
	imgui::imgui
	DXGI.lib
)

if(DEFINED ENV{SKYRIM_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_FOLDER}/Data")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_FOLDER}/Data")
endif()

# If you're using Mod Organizer 2 or Vortex, you might want this to go inside
# of your "mods" folder, inside of a subfolder named "<your mod>".
#
# To do this automatically, set the `SKYRIM_MODS_FOLDER` environment variable
# to the path of your "mods" folder
if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/${PROJECT_NAME} ${CMAKE_BUILD_TYPE}")
endif()


# When your SKSE .dll is compiled, this will automatically copy the .dll into your mods folder.
# Only works if you configure DEPLOY_ROOT above (or set the SKYRIM_MODS_FOLDER environment variable)
if(DEFINED OUTPUT_FOLDER)
    # If you specify an <OUTPUT_FOLDER> (including via environment variables)
    # then we'll copy your mod files into Skyrim or a mod manager for you!

    # Copy the SKSE plugin .dll files into the SKSE/Plugins/ folder
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    set (CONFIG_FOLDER "Template_Plugin")
    set(SRC_CONFIG_FOLDER "${CMAKE_SOURCE_DIR}/config")
    
    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM
    )
    if (EXISTS "${SRC_CONFIG_FOLDER}")
        add_custom_command(
            TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/config" "${DLL_FOLDER}/${CONFIG_FOLDER}"
            VERBATIM
        )
    endif()
    # If you perform a "Debug" build, also copy .pdb file (for debug symbols)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(
            TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM
        )
    endif()
endif()