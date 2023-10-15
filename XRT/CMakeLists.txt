set(EXE_NAME XRT)
set(ROOT_DIR "${CMAKE_SOURCE_DIR}/${EXE_NAME}")

# set(CMAKE_UNITY_BUILD true)
# set(CMAKE_UNITY_BUILD_BATCH_SIZE 16)
include("_Sources.cmake")

add_compile_definitions("_CRT_SECURE_NO_WARNINGS")

add_compile_definitions("CFXS_VERSION_STRING=\"v0.1-dev\"")
add_compile_definitions("CFXS_PROGRAM_NAME=\"CFXS XRT\"")

add_executable(${EXE_NAME} ${sources})

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include")

target_precompile_headers(
  ${EXE_NAME}
  PRIVATE
  <memory>
  <cstdint>
  <cstddef>
  <cstdlib>
)

if(WIN32)
  target_compile_definitions(${EXE_NAME} PRIVATE "CFXS_PLATFORM_WINDOWS")
  target_compile_definitions(${EXE_NAME} PRIVATE "PATH_SEPARATOR='\\\\'")
elseif(UNIX)
  target_compile_definitions(${EXE_NAME} PRIVATE "CFXS_PLATFORM_LINUX")
  target_compile_definitions(${EXE_NAME} PRIVATE "PATH_SEPARATOR='/'")
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/vendor")

if(MSVC)
  target_compile_options(${EXE_NAME} PRIVATE "/MP")
  target_compile_options(${EXE_NAME} PRIVATE "/GL")

  # target_link_options(${EXE_NAME} PRIVATE "/LTCG")
endif()

target_link_libraries(
  ${EXE_NAME}
  PRIVATE project_options
  project_warnings
)

if(BUILD_PROFILER)
  target_link_options(${EXE_NAME} PRIVATE "/PROFILE")
endif()

if(WIN32)
string(REGEX REPLACE "/W[3|4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
