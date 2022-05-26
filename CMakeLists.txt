cmake_policy(SET CMP0091 NEW)
cmake_minimum_required(VERSION 3.21)
project(seven)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# VST SDK debug binaries.
find_library(BASE_LIB_Debug base PATHS "${VST_SDK_374_BUILD_DIR}/lib/Debug")
find_library(SDK_LIB_Debug sdk PATHS "${VST_SDK_374_BUILD_DIR}/lib/Debug")
find_library(SDK_COMMON_LIB_Debug sdk_common PATHS "${VST_SDK_374_BUILD_DIR}/lib/Debug")
find_library(PLUGINTERFACES_LIB_Debug pluginterfaces PATHS "${VST_SDK_374_BUILD_DIR}/lib/Debug")
find_library(VSTGUI_LIB_Debug vstgui PATHS "${VST_SDK_374_BUILD_DIR}/lib/Debug")
find_library(VSTGUI_SUPPORT_LIB_Debug vstgui_support PATHS "${VST_SDK_374_BUILD_DIR}/lib/Debug")
find_library(VSTGUI_UIDESCRIPTION_LIB_Debug vstgui_uidescription PATHS "${VST_SDK_374_BUILD_DIR}/lib/Debug")

# VST SDK release binaries.
find_library(BASE_LIB_Release base PATHS "${VST_SDK_374_BUILD_DIR}/lib/Release")
find_library(SDK_LIB_Release sdk PATHS "${VST_SDK_374_BUILD_DIR}/lib/Release")
find_library(SDK_COMMON_LIB_Release sdk_common PATHS "${VST_SDK_374_BUILD_DIR}/lib/Release")
find_library(PLUGINTERFACES_LIB_Release pluginterfaces PATHS "${VST_SDK_374_BUILD_DIR}/lib/Release")
find_library(VSTGUI_LIB_Release vstgui PATHS "${VST_SDK_374_BUILD_DIR}/lib/Release")
find_library(VSTGUI_SUPPORT_LIB_Release vstgui_support PATHS "${VST_SDK_374_BUILD_DIR}/lib/Release")
find_library(VSTGUI_UIDESCRIPTION_LIB_Release vstgui_uidescription PATHS "${VST_SDK_374_BUILD_DIR}/lib/Release")

# Svn base library.
set(SVN_BASE_SRC_DIR "src/svn.base")
file(GLOB_RECURSE SVN_BASE_SRC "${SVN_BASE_SRC_DIR}/*.*")
add_library(seven.base STATIC ${SVN_BASE_SRC})
source_group(TREE "../${SVN_BASE_SRC_DIR}" FILES ${SVN_BASE_SRC})
target_include_directories(seven.base PRIVATE ${SVN_BASE_SRC_DIR})

# Svn synth library.
set(SVN_SYNTH_SRC_DIR "src/svn.synth")
file(GLOB_RECURSE SVN_SYNTH_SRC "${SVN_SYNTH_SRC_DIR}/*.*")
add_library(seven.synth STATIC ${SVN_SYNTH_SRC})
source_group(TREE "../${SVN_SYNTH_SRC_DIR}" FILES ${SVN_SYNTH_SRC})
target_include_directories(seven.synth PRIVATE ${SVN_BASE_SRC_DIR})
target_include_directories(seven.synth PRIVATE ${SVN_SYNTH_SRC_DIR})
target_link_libraries(seven.synth seven.base)

# Svn VST bindings.
set(SVN_VST_SRC_DIR "src/vst")
file(GLOB_RECURSE SVN_VST_SRC "${SVN_VST_SRC_DIR}/*.*")
add_library(seven.vst SHARED ${SVN_VST_SRC})
source_group(TREE "../${SVN_VST_SRC_DIR}" FILES ${SVN_VST_SRC})

target_include_directories(seven.vst PRIVATE ${SVN_SRC_DIR})
target_include_directories(seven.vst PRIVATE ${SVN_VST_SRC_DIR})
target_include_directories(seven.vst PRIVATE ${VST_SDK_374_SRC_DIR})
target_include_directories(seven.vst PRIVATE ${VST_SDK_374_SRC_DIR}/vstgui4)

target_link_libraries(seven.vst seven)
target_link_libraries(seven.vst debug "${BASE_LIB_Debug}" optimized "${BASE_LIB_Release}")
target_link_libraries(seven.vst debug "${SDK_LIB_Debug}" optimized "${SDK_LIB_Release}")
target_link_libraries(seven.vst debug "${SDK_COMMON_LIB_Debug}" optimized "${SDK_COMMON_LIB_Release}")
target_link_libraries(seven.vst debug "${PLUGINTERFACES_LIB_Debug}" optimized "${PLUGINTERFACES_LIB_Release}")
target_link_libraries(seven.vst debug "${VSTGUI_LIB_Debug}" optimized "${VSTGUI_LIB_Release}")
target_link_libraries(seven.vst debug "${VSTGUI_SUPPORT_LIB_Debug}" optimized "${VSTGUI_SUPPORT_LIB_Release}")
target_link_libraries(seven.vst debug "${VSTGUI_UIDESCRIPTION_LIB_Debug}" optimized "${VSTGUI_UIDESCRIPTION_LIB_Release}")

set_target_properties(seven.vst PROPERTIES SUFFIX ".vst3")
set_target_properties(seven.vst PROPERTIES DEBUG_POSTFIX ".debug")
set_target_properties(seven.vst PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release/seven.vst3/Contents/x86_64-win")
set_target_properties(seven.vst PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug/seven.debug.vst3/Contents/x86_64-win")

add_custom_target(copy-resources-release ALL DEPENDS seven.vst COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/vst/resources/output ${CMAKE_BINARY_DIR}/Release/seven.vst3/Contents/Resources)
add_custom_target(copy-resources-debug ALL DEPENDS seven.vst COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/vst/resources/output ${CMAKE_BINARY_DIR}/Debug/seven.debug.vst3/Contents/Resources)