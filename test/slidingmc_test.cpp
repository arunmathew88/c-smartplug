#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>

#include "../src/slidingmc.cpp"
#include "../include/slidingmc.h"

class SlidingMcTest : public CppUnit::TestFixture
{
  public:
	void setUp()
	{
	}

	void tearDown()
	{
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(SlidingMcTest);

int main()
{
	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());
	bool wasSucessful = runner.run("", false);
	return wasSucessful;
}
