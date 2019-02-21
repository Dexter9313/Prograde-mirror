find_package(Boost REQUIRED)

set(PROJECT_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(PROJECT_LIBRARIES ${Boost_LIBRARIES} -lquadmath)
