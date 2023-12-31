//
// XMLConfigurationTest.cpp
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "XMLConfigurationTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/AutoPtr.h"
#include "Poco/Exception.h"
#include <sstream>
#include <algorithm>


using Poco::Util::XMLConfiguration;
using Poco::Util::AbstractConfiguration;
using Poco::AutoPtr;
using Poco::NotImplementedException;
using Poco::NotFoundException;


XMLConfigurationTest::XMLConfigurationTest(const std::string& name): AbstractConfigurationTest(name)
{
}


XMLConfigurationTest::~XMLConfigurationTest()
{
}


void XMLConfigurationTest::testLoad()
{
	static const std::string xmlFile =
		"<config>"
		"	<prop1>value1</prop1>"
		"	<prop2>value2</prop2>"
		"	<prop3>"
		"		<prop4 attr='value3'/>"
		"		<prop4 attr='value4'/>"
		"	</prop3>"
		"	<prop5 id='1'>value5</prop5>"
		"	<prop5 id='2'>value6</prop5>"
		"   <prop6 id='foo'>"
		"       <prop7>value7</prop7>"
		"   </prop6>"
		"   <prop6 id='bar'>"
		"       <prop7>value8</prop7>"
		"   </prop6>"
		"</config>";

	std::istringstream istr(xmlFile);
	AutoPtr<XMLConfiguration> pConf = new XMLConfiguration(istr);

	assertTrue (pConf->getString("prop1") == "value1");
	assertTrue (pConf->getString("prop2") == "value2");
	assertTrue (pConf->getString("prop3.prop4[@attr]") == "value3");
	assertTrue (pConf->getString("prop3.prop4[1][@attr]") == "value4");
	assertTrue (pConf->getString("prop5") == "value5");
	assertTrue (pConf->getString("prop5[0]") == "value5");
	assertTrue (pConf->getString("prop5[1]") == "value6");
	assertTrue (pConf->getString("prop5[@id=1]") == "value5");
	assertTrue (pConf->getString("prop5[@id='2']") == "value6");
	assertTrue (pConf->getString("prop6[@id=foo].prop7") == "value7");
	assertTrue (pConf->getString("prop6[@id='bar'].prop7") == "value8");

	AbstractConfiguration::Keys keys;
	pConf->keys(keys);
	assertTrue (keys.size() == 7);
	assertTrue (std::find(keys.begin(), keys.end(), "prop1") != keys.end());
	assertTrue (std::find(keys.begin(), keys.end(), "prop2") != keys.end());
	assertTrue (std::find(keys.begin(), keys.end(), "prop3") != keys.end());
	assertTrue (std::find(keys.begin(), keys.end(), "prop5") != keys.end());
	assertTrue (std::find(keys.begin(), keys.end(), "prop5[1]") != keys.end());
	assertTrue (std::find(keys.begin(), keys.end(), "prop6") != keys.end());
	assertTrue (std::find(keys.begin(), keys.end(), "prop6[1]") != keys.end());

	pConf->keys("prop3", keys);
	assertTrue (keys.size() == 2);
	assertTrue (std::find(keys.begin(), keys.end(), "prop4") != keys.end());
	assertTrue (std::find(keys.begin(), keys.end(), "prop4[1]") != keys.end());

	assertTrue (pConf->hasProperty("prop3.prop4[@attr]"));
	pConf->remove("prop3.prop4[@attr]");
	assertTrue (!pConf->hasProperty("prop3.prop4[@attr]"));

	assertTrue (pConf->hasProperty("prop3"));
	pConf->remove("prop3");
	assertTrue (!pConf->hasProperty("prop3"));

	try
	{
		std::string s = pConf->getString("foo");
		fail("No property - must throw");
	}
	catch (NotFoundException&)
	{
	}

	try
	{
		std::string s = pConf->getString("prop5[@id='3']");
		fail("No property - must throw");
	}
	catch (NotFoundException&)
	{
	}
}


void XMLConfigurationTest::testSave()
{
	AutoPtr<XMLConfiguration> pConf = new XMLConfiguration;
	pConf->loadEmpty("config");

	std::ostringstream ostr;
	pConf->save(ostr);
	std::string s(ostr.str());
	assertTrue (s == "<config/>\n");

	pConf->setString("prop1", "value1");
	assertTrue (pConf->getString("prop1") == "value1");

	pConf->setString("prop2", "value2");
	assertTrue (pConf->getString("prop2") == "value2");

	pConf->setString("prop3.prop4[@attr]", "value3");
	assertTrue (pConf->getString("prop3.prop4[@attr]") == "value3");

	pConf->setString("prop3.prop4[1][@attr]", "value4");
	assertTrue (pConf->getString("prop3.prop4[1][@attr]") == "value4");

	pConf->setString("prop5", "value5a");
	assertTrue (pConf->getString("prop5") == "value5a");

	pConf->setString("prop5[0]", "value5");
	assertTrue (pConf->getString("prop5[0]") == "value5");
	assertTrue (pConf->getString("prop5") == "value5");

	pConf->setString("prop5[1]", "value6");
	assertTrue (pConf->getString("prop5[1]") == "value6");

	try
	{
		pConf->setString("prop5[3]", "value7");
		fail("bad index - must throw");
	}
	catch (Poco::InvalidArgumentException&)
	{
	}

	std::ostringstream ostr2;
	pConf->save(ostr2);
	s = ostr2.str();

	assertTrue (s ==
		"<config>\n"
		"\t<prop1>value1</prop1>\n"
		"\t<prop2>value2</prop2>\n"
		"\t<prop3>\n"
		"\t\t<prop4 attr=\"value3\"/>\n"
		"\t\t<prop4 attr=\"value4\"/>\n"
		"\t</prop3>\n"
		"\t<prop5>value5</prop5>\n"
		"\t<prop5>value6</prop5>\n"
		"</config>\n");

	pConf->setString("prop1", "value11");
	assertTrue (pConf->getString("prop1") == "value11");

	pConf->setString("prop2", "value21");
	assertTrue (pConf->getString("prop2") == "value21");

	pConf->setString("prop3.prop4[1][@attr]", "value41");
	assertTrue (pConf->getString("prop3.prop4[1][@attr]") == "value41");

	pConf->setString("prop3.prop4[2][@attr]", "value42");
	assertTrue (pConf->getString("prop3.prop4[2][@attr]") == "value42");

	std::ostringstream ostr3;
	pConf->save(ostr3);
	s = ostr3.str();
	assertTrue (s ==
		"<config>\n"
		"\t<prop1>value11</prop1>\n"
		"\t<prop2>value21</prop2>\n"
		"\t<prop3>\n"
		"\t\t<prop4 attr=\"value3\"/>\n"
		"\t\t<prop4 attr=\"value41\"/>\n"
		"\t\t<prop4 attr=\"value42\"/>\n"
		"\t</prop3>\n"
		"\t<prop5>value5</prop5>\n"
		"\t<prop5>value6</prop5>\n"
		"</config>\n");
}


void XMLConfigurationTest::testLoadAppendSave()
{
	AutoPtr<XMLConfiguration> pConf = new XMLConfiguration;
	std::istringstream istr("<config>\n"
		"\t<prop1>value1</prop1>\n"
		"</config>\n");
	pConf->load(istr);

	pConf->setString("prop2", "value2");
	assertTrue (pConf->getString("prop2") == "value2");

	std::ostringstream ostr;
	pConf->save(ostr);
	std::string s(ostr.str());

	assertTrue (s ==
		"<config>\n"
		"\t<prop1>value1</prop1>\n"
		"\t<prop2>value2</prop2>\n"
		"</config>\n");
}


void XMLConfigurationTest::testOtherDelimiter()
{
	static const std::string xmlFile =
		"<config>"
		"	<prop1>value1</prop1>"
		"	<prop2>value2</prop2>"
		"	<prop3>"
		"		<prop4 attr='value3'/>"
		"		<prop4 attr='value4'/>"
		"	</prop3>"
		"	<prop5 id='1'>value5</prop5>"
		"	<prop5 id='2'>value6</prop5>"
		"   <prop6 id='foo'>"
		"       <prop7>value7</prop7>"
		"   </prop6>"
		"   <prop6 id='bar'>"
		"       <prop7>value8</prop7>"
		"   </prop6>"
		"</config>";

	std::istringstream istr(xmlFile);
	AutoPtr<XMLConfiguration> pConf = new XMLConfiguration(istr, '/');

	assertTrue (pConf->getString("prop1") == "value1");
	assertTrue (pConf->getString("prop2") == "value2");
	assertTrue (pConf->getString("prop3/prop4[@attr]") == "value3");
	assertTrue (pConf->getString("prop3/prop4[1][@attr]") == "value4");
	assertTrue (pConf->getString("prop5") == "value5");
	assertTrue (pConf->getString("prop5[0]") == "value5");
	assertTrue (pConf->getString("prop5[1]") == "value6");
	assertTrue (pConf->getString("prop5[@id=1]") == "value5");
	assertTrue (pConf->getString("prop5[@id='2']") == "value6");
	assertTrue (pConf->getString("prop6[@id=foo]/prop7") == "value7");
	assertTrue (pConf->getString("prop6[@id='bar']/prop7") == "value8");
}


AbstractConfiguration::Ptr XMLConfigurationTest::allocConfiguration() const
{
	XMLConfiguration* pConfig = new XMLConfiguration();
	pConfig->loadEmpty("TestConfiguration");

	return pConfig;
}


void XMLConfigurationTest::testSaveEmpty()
{
	Poco::AutoPtr<XMLConfiguration> pConfig = new XMLConfiguration;
	std::ostringstream ostr;
	pConfig->save(ostr);
	assertTrue (ostr.str() == "<config/>\n");
}


void XMLConfigurationTest::testFromScratch()
{
	Poco::AutoPtr<XMLConfiguration> pConfig = new XMLConfiguration;
	pConfig->setString("foo", "bar");
	std::ostringstream ostr;
	pConfig->save(ostr);
	assertTrue (ostr.str() == "<config>\n\t<foo>bar</foo>\n</config>\n");
}


void XMLConfigurationTest::testLoadEmpty()
{
	Poco::AutoPtr<XMLConfiguration> pConfig = new XMLConfiguration;
	pConfig->loadEmpty("AppConfig");
	pConfig->setString("foo", "bar");
	std::ostringstream ostr;
	pConfig->save(ostr);
	assertTrue (ostr.str() == "<AppConfig>\n\t<foo>bar</foo>\n</AppConfig>\n");
}


void XMLConfigurationTest::testManyKeys()
{
	std::ostringstream ostr;
	ostr << "<config>\n";
	const size_t count = 200000;
	for (size_t i = 0; i < count; ++i)
	{
		ostr << "<element>" << i << "</element>\n";
	}
	ostr << "</config>\n";

	std::istringstream istr(ostr.str());
	AutoPtr<XMLConfiguration> pConf = new XMLConfiguration(istr);

	AbstractConfiguration::Keys all_elements;
	pConf->keys("", all_elements);

	assertTrue(all_elements.size() == count);
}


void XMLConfigurationTest::setUp()
{
}


void XMLConfigurationTest::tearDown()
{
}


CppUnit::Test* XMLConfigurationTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("XMLConfigurationTest");

	AbstractConfigurationTest_addTests(pSuite, XMLConfigurationTest);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testLoad);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testSave);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testLoadAppendSave);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testOtherDelimiter);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testSaveEmpty);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testFromScratch);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testLoadEmpty);
	CppUnit_addTest(pSuite, XMLConfigurationTest, testManyKeys);

	return pSuite;
}
