cmake_minimum_required(VERSION 3.0)

project(lslpub_OTB)

#get the sources and headers
file(GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "src/*.c")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "include/*.hpp")

#add the include libraries directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

#create our exec file
add_executable(lslpub_OTB ${SRCS} ${HEADERS})

#link LSL libraries
if(UNIX)
  target_link_libraries (lslpub_OTB lsl64)
  ADD_CUSTOM_COMMAND (	TARGET lslpub_OTB POST_BUILD
						#copy the dll files 
						COMMENT "Saving exec in bin/ directory..." VERBATIM
						COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/UNIX/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}
						COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lslpub_OTB ${CMAKE_CURRENT_SOURCE_DIR}/bin/UNIX/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}
						)
endif(UNIX)

if(WIN32)
  target_link_libraries (lslpub_OTB liblsl32)
  target_link_libraries (lslpub_OTB wsock32 ws2_32)

  ADD_CUSTOM_COMMAND (	TARGET lslpub_OTB POST_BUILD
						#copy the dll files 
						COMMENT "Copying LSL binaries..." VERBATIM
						COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/liblsl32.dll ${CMAKE_BINARY_DIR}/Release
						#copy the dll files 
						COMMENT "Saving exec in bin/ directory..." VERBATIM
						COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/bin/WIN32/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}
						COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/Release ${CMAKE_CURRENT_SOURCE_DIR}/bin/WIN32/${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_VERSION}
						)

endif(WIN32)


