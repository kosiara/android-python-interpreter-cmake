cmake_minimum_required(VERSION 3.4.1)

############################################################################################################################################
####### SET CRYSTAX PATH ############ Download from:  https://www.crystax.net/download/crystax-ndk-10.3.2-darwin-x86_64.tar.xz #############
############################################################################################################################################
set(CRYSTAX_PATH /Users/bko/crystax-ndk-10.3.2)


set(PYTHON_HEADERS ${CRYSTAX_PATH}/sources/python/3.5/include/python/)
set(PYTHON_LIBS ${CRYSTAX_PATH}/sources/python/3.5/libs/)
include_directories (${PYTHON_HEADERS})
include_directories (${PYTHON_LIBS})

############ export libnative-lib.so #######################################################################################################
add_library(native-lib SHARED src/main/cpp/native-lib.cpp src/main/cpp/native-util.cpp )

######## ADD PYTHON LIBRARY ################################################################################################################
set(distribution_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jniLibs)
add_library(python3.5m SHARED IMPORTED)
set_target_properties(python3.5m PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libpython3.5m.so)

######## ADD CRYSTAX LIBRARY ###############################################################################################################
add_library(crystax SHARED IMPORTED)
set_target_properties(crystax PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libcrystax.so)

############################################################################################################################################

find_library( log-lib log )

target_link_libraries( native-lib
                       python3.5m
                       crystax
                       ${log-lib} )

