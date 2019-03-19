cmake_minimum_required(VERSION 3.0)

project(lslpub_OTB)

#get the sources and headers
file(GLOB SRCS "src/*.cpp" "src/*.c")
file(GLOB HEADERS "include/*.h" "include/*.hpp")

#add the include libraries directories
include_directories(include)
link_directories(lib)

#create our exec file
add_executable(lslpub_OTB ${SRCS} ${HEADERS})

#link LSL libraries
if(UNIX)
  target_link_libraries (lslpub_OTB lsl64)
endif(UNIX)

if(WIN32)
  target_link_libraries (lslpub_OTB lsl32)
endif(WIN32)
