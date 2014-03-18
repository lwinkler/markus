
find_library(CPPUNIT_LIBS cppunit)
if("${CPPUNIT_LIBS}" STREQUAL "CPPUNIT_LIBS-NOTFOUND")
	message(WARNING "CppUnit library not found on your system. Unit tests will not be added to the project.")
	set(CPPUNIT_LIBS "")
endif()
# add_library(testing
#unitTesting.cpp
#)


if(NOT "${CPPUNIT_LIBS}" STREQUAL "")
	add_executable(unitTesting unitTesting.cpp)
	target_link_libraries(unitTesting ${DEPS} ${CPPUNIT_LIBS})
endif()

