cmake_minimum_required(VERSION 3.6)

if( WIN32 )
  
else()
  set(CMAKE_CXX_FLAGS "-std=c++14 -frtti -Wno-switch")

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-braces")

endif()



find_package(Boost REQUIRED COMPONENTS system timer)

include_directories(${Boost_INCLUDE_DIRS})

include_directories(include)

add_executable(ConnectFour ConnectFour.cpp)
target_link_libraries(ConnectFour ${Boost_LIBRARIES})
