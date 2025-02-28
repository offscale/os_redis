set(LIBRARY_NAME "redis_version")
set(libacquire_git "https://raw.githubusercontent.com/offscale/libacquire/master")

foreach (cm "libacquire_get_header.cmake" "FindCurlCustom.cmake"
        "FindLibreSSL.cmake" "libacquire_helpers.cmake"
        "FindLibAcquire.cmake")
    set(local_cmake_file "${CMAKE_BINARY_DIR}/cmake/${cm}")
    if (NOT EXISTS "${local_cmake_file}")
        file(DOWNLOAD "${libacquire_git}/cmake/${cm}"
                "${local_cmake_file}")
    endif (NOT EXISTS "${local_cmake_file}")
    file(SIZE "${local_cmake_file}" local_cmake_file_size)
    if (local_cmake_file_size EQUAL 0)
        message(FATAL_ERROR "Download failed of \"${cm}\" from \"${libacquire_git}/cmake/${cm}\"")
    elseif (NOT cm STREQUAL "FindLibreSSL.cmake")
        include("${CMAKE_BINARY_DIR}/cmake/${cm}")
        if (cm STREQUAL "libacquire_helpers.cmake")
            set_crypto_lib()

            set_ssl_lib()

            set_http_https_lib()

            set_extract_lib()

            set_checksum_lib()
        endif (cm STREQUAL "libacquire_helpers.cmake")
    endif ()
endforeach (cm "libacquire_get_header.cmake" "FindCurlCustom.cmake"
            "FindLibreSSL.cmake" "libacquire_helpers.cmake"
            "FindLibAcquire.cmake")

get_libacquire_header("${PROJECT_BINARY_DIR}/src/acquire.h")

include("${CMAKE_SOURCE_DIR}/cmake/modules/GenerateRedisVersions.cmake")

set(Header_Files "version_redis.h")

#if (CMAKE_SYSTEM_NAME STREQUAL "Windows")

generate_windows_redis_versions("windows_redis_versions.h")
list(APPEND Header_Files "windows_redis_versions.h")

#else ()

generate_redis_versions("redis_versions.h")
list(APPEND Header_Files "redis_versions.h")

#endif ()


source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "version_redis.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${PROJECT_BINARY_DIR}/src/acquire.h" "${Source_Files}")

target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:include>"
)

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
    string(SUBSTRING "${_variableName}" 0 4 maybe_use)
    if (maybe_use STREQUAL "USE_")
        target_compile_definitions("${LIBRARY_NAME}" PUBLIC "${_variableName}=${${_variableName}}")
    endif (maybe_use STREQUAL "USE_")
endforeach(_variableName ${_variableNames})

message(STATUS "${PROJECT_NAME} LIBACQUIRE_LIBRARIES = ${LIBACQUIRE_LIBRARIES}")
target_link_libraries("${LIBRARY_NAME}" PUBLIC "${LIBACQUIRE_LIBRARIES}")

if (NOT DEFINED TARGET_ARCH)
    set(TARGET_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif (NOT DEFINED TARGET_ARCH)

configure_file(config.h.in "${PROJECT_BINARY_DIR}/src/config.h" @ONLY)

#######################################
# Export / installation configuration #
#######################################

# install rules
include(GNUInstallDirs)

include(GenerateExportHeader)
set(_export_file "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

set(installable_libs "${LIBRARY_NAME}" "${PROJECT_NAME}_compiler_flags")

if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif (TARGET "${DEPENDANT_LIBRARY}")
install(FILES
        "${PROJECT_BINARY_DIR}/src/acquire.h"
        "${PROJECT_BINARY_DIR}/src/config.h"
        "${_export_file}"
        ${Header_Files}
        TYPE "INCLUDE")
install(TARGETS ${installable_libs}
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIBRARY_NAME}"
        EXPORT "${LIBRARY_NAME}Targets")
install(TARGETS ${installable_libs}
        EXPORT "${LIBRARY_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
