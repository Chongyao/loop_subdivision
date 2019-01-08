file(GLOB header *.h)
file(GLOB source *.cc *.cpp)
add_library(mylib SHARED ${source} )

