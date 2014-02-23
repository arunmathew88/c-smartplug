#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TestRunner.h>
#include <ctime>

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
		CPPUNIT_ASSERT(sm->getMedian(4000, WINDOW_1HR) == 2200.5);
	}

	void test2()
	{
		for(int i=1; i<80000; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT(sm->getMedian(80001, WINDOW_1HR) == 78201.5);
	}

	void test3()
	{
		for(int i=1; i<90000; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT(sm->getMedian(90001, WINDOW_24HR) == 46800);
	}

	void test4()
	{
		for(int i=1; i<9; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT(sm->getMedian(10, WINDOW_24HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(10, WINDOW_1HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(1000, WINDOW_24HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(1000, WINDOW_1HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(10000, WINDOW_24HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(10000, WINDOW_1HR) == -1);
	}

	void test5()
	{
		for(int i=1; i<9; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT(sm->getMedian(10, WINDOW_1HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(10, WINDOW_24HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(1000, WINDOW_1HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(1000, WINDOW_24HR) == 4.5);
		CPPUNIT_ASSERT(sm->getMedian(10000, WINDOW_1HR) == -1);
		CPPUNIT_ASSERT(sm->getMedian(10000, WINDOW_24HR) == 4.5);
	}


	void test6()
	{
		for(int i=1; i<2; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT(sm->getMedian(10, WINDOW_24HR) == 1);
		CPPUNIT_ASSERT(sm->getMedian(1000, WINDOW_24HR) == 1);
		CPPUNIT_ASSERT(sm->getMedian(10000, WINDOW_24HR) == 1);
		CPPUNIT_ASSERT(sm->getMedian(100000, WINDOW_24HR) == -1);
	}

	void test_overlap1()
	{
		for(int i=1; i<24*3600+1; i++)
			sm->insert(i, i);
		sm->insert(MAX_WINDOW_SIZE+100, 100);
		CPPUNIT_ASSERT(sm->getMedian(MAX_WINDOW_SIZE+100, WINDOW_24HR) == 48051.5);
	}

	void test_overlap2()
	{
		for(int i=1; i<24*3600+1; i++)
			sm->insert(i, 24*3600-i);
		sm->insert(MAX_WINDOW_SIZE+100, 100);
		CPPUNIT_ASSERT(sm->getMedian(MAX_WINDOW_SIZE+100, WINDOW_24HR) == 38352);
	}

	CPPUNIT_TEST_SUITE(SlidingMcTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	CPPUNIT_TEST(test3);
	CPPUNIT_TEST(test4);
	CPPUNIT_TEST(test5);
	CPPUNIT_TEST(test6);
	CPPUNIT_TEST(test_overlap1);
	CPPUNIT_TEST(test_overlap2);
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
