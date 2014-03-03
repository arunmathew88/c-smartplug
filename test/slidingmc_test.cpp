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
		sm = new SlidingMc();
	}

	void tearDown()
	{
		delete sm;
	}

	void test1()
	{
		for(float i=1; i<4000; i++)
			sm->insert(i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(2016.5, sm->getMedian(), 0.00001);
	}

	void test2()
	{
		for(float i=1; i<80000; i++)
			sm->insert(i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(40228, sm->getMedian(), 0.00001);
	}

	void test3()
	{
		for(float i=1; i<90000; i++)
			sm->insert(i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(44950.5, sm->getMedian(), 0.00001);
	}

	void test4()
	{
		for(float i=1; i<9; i++)
			sm->insert(i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5, sm->getMedian(), 0.00001);
	}


	void test6()
	{
		for(float i=1; i<2; i++)
			sm->insert(i);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1, sm->getMedian(), 0.00001);
	}

	void test_overlap1()
	{
		for(float i=1; i<24*3600+1; i++)
			sm->insert(i);
		sm->insert(100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(42789, sm->getMedian(), 0.00001);
	}

	void test_overlap2()
	{
		for(float i=1; i<24*3600+1; i++)
			sm->insert(24*3600-i);
		sm->insert(100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(43077, sm->getMedian(), 0.00001);
	}

	void test_random1()
	{
		for(float i=1; i<65584; i++)
			sm->insert(rand()%100+50);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(100.5, sm->getMedian(), 0.00001);
	}

	void test_random2()
	{
		for(float i=1; i<65584; i++)
			sm->insert((float)(rand()%10000)/100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(50.655, sm->getMedian(), 0.00001);
	}

	void test_random3()
	{
		for(float i=1; i<65584; i++)
			sm->insert((float)(rand()%50000)/100);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(253.455, sm->getMedian(), 0.00001);
	}

	void test_del1()
	{
		for(float i=1; i<4000; i++)
			sm->insert(i);
		for(float i=1; i<4000; i++)
			sm->del(i);

		CPPUNIT_ASSERT_DOUBLES_EQUAL(0, sm->getMedian(), 0.00001);
	}

	void test_del2()
	{
		for(float i=1; i<80000; i++)
			sm->insert(i);
		for(float i=1; i<80000; i++)
			sm->del(i);

		CPPUNIT_ASSERT_DOUBLES_EQUAL(0, sm->getMedian(), 0.00001);
	}

	CPPUNIT_TEST_SUITE(SlidingMcTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST(test2);
	CPPUNIT_TEST(test3);
	CPPUNIT_TEST(test4);
	CPPUNIT_TEST(test6);
	CPPUNIT_TEST(test_overlap1);
	CPPUNIT_TEST(test_overlap2);
	CPPUNIT_TEST(test_random1);
	CPPUNIT_TEST(test_random2);
	CPPUNIT_TEST(test_random3);
	CPPUNIT_TEST(test_del1);
	CPPUNIT_TEST(test_del2);
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
