file(GLOB header *.h)
file(GLOB source *.cc *.cpp)
# add_library(mylib SHARED ${source} )

add_executable(loop_subdivision  ${source} ${header})
target_link_libraries(loop_subdivision
  ${Boost_LIBRARIES})

