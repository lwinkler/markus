
find_package(CxxTest)
if(CXXTEST_FOUND)
	file(GLOB tests_src1 "../*.test.h")
	file(GLOB tests_src2 "../*/*.test.h")
	file(GLOB tests_src3 "../*/*/*.test.h")

	add_executable(unitTests unitTests.cpp ../modules/allModules.cpp)
	target_link_libraries(unitTests ${MK_DEPS} ${MK_DEPS} ${MK_EXT_DEPS}) # double the dependencies to avoid linking error

	add_custom_command(
		OUTPUT unitTests.cpp
		DEPENDS ${tests_src1} ${tests_src2} ${tests_src3}
		COMMAND cxxtestgen -o unitTests.cpp ${tests_src1} ${tests_src2} ${tests_src3} --xunit-printer
	)
else()
	message(WARNING "CxxTest library not found on your system. Unit tests will not be added to the project.")
endif()

