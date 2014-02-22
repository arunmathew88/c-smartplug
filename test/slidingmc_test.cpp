#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>

#include "../src/slidingmc.cpp"
#include "../include/slidingmc.h"

class SlidingMcTest : public CppUnit::TestFixture
{
  protected:
  	SlidingMc *sm;

  public:
	void setUp()
	{
		sm = new SlidingMc(1);
	}

	void tearDown()
	{
		delete sm;
	}

	void test1()
	{
		for(int i=1; i<4000; i++)
			sm->insert(i, i);
		cout<<sm->getMedian(4000, WINDOW_1HR)<<endl;
		CPPUNIT_ASSERT(sm->getMedian(4000, WINDOW_1HR) == 2000);
	}

	CPPUNIT_TEST_SUITE(SlidingMcTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();
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
