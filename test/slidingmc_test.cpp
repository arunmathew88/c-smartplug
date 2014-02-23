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
		for(float i=1; i<4000; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(2204, sm->getMedian(4000, WINDOW_1HR), 0.00001);
	}

	void test2()
	{
		for(float i=1; i<80000; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(78212.5, sm->getMedian(80001, WINDOW_1HR), 0.00001);
	}

	void test3()
	{
		for(float i=1; i<90000; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(46921.5, sm->getMedian(90001, WINDOW_24HR), 0.00001);
	}

	void test4()
	{
		for(float i=1; i<9; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(10, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(10, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(1000, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(1000, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(10000, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-1, sm->getMedian(10000, WINDOW_1HR), 0.00001);
	}

	void test5()
	{
		for(float i=1; i<9; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(10, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(10, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(1000, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(1000, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-1, sm->getMedian(10000, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(10000, WINDOW_24HR), 0.00001);
	}


	void test6()
	{
		for(float i=1; i<2; i++)
			sm->insert(i, i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1, sm->getMedian(10, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1, sm->getMedian(1000, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1, sm->getMedian(10000, WINDOW_24HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-1, sm->getMedian(100000, WINDOW_24HR), 0.00001);
	}

	void test_overlap1()
	{
		for(float i=1; i<24*3600+1; i++)
			sm->insert(i, i);
		sm->insert(MAX_WINDOW_SIZE+100, 100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(48281, sm->getMedian(MAX_WINDOW_SIZE+100, WINDOW_24HR), 0.00001);
	}

	void test_overlap2()
	{
		for(float i=1; i<24*3600+1; i++)
			sm->insert(i, 24*3600-i);
		sm->insert(MAX_WINDOW_SIZE+100, 100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(38590, sm->getMedian(MAX_WINDOW_SIZE+100, WINDOW_24HR), 0.00001);
	}

	void test_random1()
	{
		for(float i=1; i<65584; i++)
			sm->insert(i, rand()%100+50);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(101.5, sm->getMedian(65583, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(100.5, sm->getMedian(65583, WINDOW_24HR), 0.00001);
	}

	void test_random2()
	{
		for(float i=1; i<65584; i++)
			sm->insert(i, (float)(rand()%10000)/100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(50.685, sm->getMedian(65583, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(50.655, sm->getMedian(65583, WINDOW_24HR), 0.00001);
	}

	void test_random3()
	{
		for(float i=1; i<65584; i++)
			sm->insert(i, (float)(rand()%50000)/100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(242.44, sm->getMedian(65583, WINDOW_1HR), 0.00001);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(253.455, sm->getMedian(65583, WINDOW_24HR), 0.00001);
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
	CPPUNIT_TEST(test_random1);
	CPPUNIT_TEST(test_random2);
	CPPUNIT_TEST(test_random3);
	CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(SlidingMcTest);

int main()
{
	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());
	bool wasSucessful = runner.run("", true);
	return wasSucessful;
}
