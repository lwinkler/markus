#include <cppunit/ui/text/TestRunner.h>
#include "ConfigReaderTest.h"




bool run_tests()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(ConfigReaderTest::suite());
	runner.run();
	return 0;
}
