#include "test_framework.h"
#include <string>

void AssertImpl(bool value, const std::string& expr_str, const std::string& func_name, const std::string& file_name, int line_number, const std::string& hint) {
    if (!value) {
        std::cerr << file_name << "(" << line_number << "): ";
        std::cerr << func_name << ":";
        std::cerr << " ASSERT("s << expr_str << ") failed. ";
        if (!hint.empty()) {
            std::cout << "Hint: "s << hint;
        }
        std::cerr << std::endl;
        abort();
    }
}
void TestGroupByTime() {
	std::vector<Object> objects = {
		{"Today", "type1", 1.0f, 1.0f, 1724499340},
		{"Today", "type2", 2.0f, 2.0f, 1724499350},
		{"Today", "type3", 3.0f, 3.0f, 1724499360},
		{"Yesterday", "type4", 4.0f, 4.0f, 1724412940},
		{"Yesterday", "type5", 5.0f, 5.0f, 1724412930},
		{"Yesterday", "type6", 6.0f, 6.0f, 1724412950},
		{"ThisWeek", "type7", 7.0f, 7.0f, 1724240140},
		{"ThisWeek", "type8", 8.0f, 8.0f, 1724240150},
		{"ThisWeek", "type9", 9.0f, 9.0f, 1724240160},
		{"ThisMonth", "type10", 10.0f, 10.0f, 1723376140},
		{"ThisMonth", "type11", 11.0f, 11.0f, 1723376150},
		{"ThisMonth", "type12", 12.0f, 12.0f, 1723376160},
		{"ThisYear", "type13", 13.0f, 13.0f, 1710156940},
		{"ThisYear", "type14", 14.0f, 14.0f, 1710156950},
		{"ThisYear", "type15", 15.0f, 15.0f, 1710156960},
		{"Old", "type16", 16.0f, 16.0f, 1678534540},
		{"Old", "type17", 17.0f, 17.0f, 1678534530},
		{"Old", "type18", 18.0f, 18.0f, 1678534550},
		{"Old", "type19", 19.0f, 19.0f, 1678534570},
		{"Old", "type20", 20.0f, 20.0f, 1678534580}
	};
	ObjectManager objectManager(objects);
	std::map<std::string, std::vector<Object>> byTime = objectManager.GroupByTime();
	ASSERT_EQUAL(byTime.size(), 6);
	ASSERT_EQUAL(byTime["Сегодня"].size(), 3);
	ASSERT_EQUAL(byTime["Вчера"].size(), 3);
	ASSERT_EQUAL(byTime["На этой неделе"].size(), 9);
	ASSERT_EQUAL(byTime["В этом месяце"].size(), 12);
	ASSERT_EQUAL(byTime["В этом году"].size(), 15);
	ASSERT_EQUAL(byTime["Ранее"].size(), 5);
}
void TestGroupByType() {
	std::vector<Object> objects = {
		{"Today", "type1", 1.0, 1.0, 1724499340},
		{"Today", "type1", 2.0, 2.0, 1724499350},
		{"Today", "type1", 3.0, 3.0, 1724499360},
		{"Yesterday", "type2", 4.0, 4.0, 1724412940},
		{"Yesterday", "type2", 5.0, 5.0, 1724412930},
		{"Yesterday", "type2", 6.0, 6.0, 1724412950},
		{"ThisWeek", "type3", 7.0, 7.0, 1724240140},
		{"ThisWeek", "type3", 8.0, 8.0, 1724240150},
		{"ThisWeek", "type3", 9.0, 9.0, 1724240160},
		{"ThisMonth", "type3", 10.0, 10.0, 1723376140},
		{"ThisMonth", "type3", 11.0, 11.0, 1723376150},
		{"ThisMonth", "type4", 12.0, 12.0, 1723376160},
		{"ThisYear", "type1", 13.0, 13.0, 1710156940},
		{"ThisYear", "type1", 14.0, 14.0, 1710156950},
		{"ThisYear", "type1", 15.0, 15.0, 1710156960},
		{"Old", "type10", 16.0, 16.0, 1678534540},
		{"Old", "type10", 17.0, 17.0, 1678534530},
		{"Old", "type10", 18.0, 18.0, 1678534550},
		{"Old", "type10", 19.0, 19.0, 1678534570},
		{"Old", "type10", 20.0, 20.0, 1678534580}
	};
	ObjectManager objectManager(objects);
	{
		std::map<std::string, std::vector<Object>> byType = objectManager.GroupByType(1); //группировка по типу, минимальное количество объектов в группе 1
		ASSERT_EQUAL(byType.size(), 5);
		ASSERT_EQUAL(byType["type1"].size(), 6);
		ASSERT_EQUAL(byType["type2"].size(), 3);
		ASSERT_EQUAL(byType["type3"].size(), 5);
		ASSERT_EQUAL(byType["type4"].size(), 1);
		ASSERT_EQUAL(byType["type10"].size(), 5);
	}
	{
		std::map<std::string, std::vector<Object>> byType = objectManager.GroupByType(2); //группировка по типу, минимальное количество объектов в группе 2
		ASSERT_EQUAL(byType.size(), 5);
		ASSERT_EQUAL(byType["type1"].size(), 6);
		ASSERT_EQUAL(byType["type1"][0].type_, "type1");
		ASSERT_EQUAL(byType["type2"].size(), 3);
		ASSERT_EQUAL(byType["type2"][1].type_, "type2");
		ASSERT_EQUAL(byType["type3"].size(), 5);
		ASSERT_EQUAL(byType["type3"][2].type_, "type3");
		ASSERT_EQUAL(byType["Разное"].size(), 1);
		ASSERT_EQUAL(byType["Разное"][0].type_, "type4");
		ASSERT_EQUAL(byType["type10"].size(), 5);
	}
	{
		std::map<std::string, std::vector<Object>> byType = objectManager.GroupByType(5); //группировка по типу, минимальное количество объектов в группе 5
		ASSERT_EQUAL(byType.size(), 4);
		ASSERT_EQUAL(byType["type1"].size(), 6);
		ASSERT_EQUAL(byType.at("type1")[0].type_, "type1");
		ASSERT_EQUAL(byType["type3"].size(), 5);
		ASSERT_EQUAL(byType.at("type3")[2].type_, "type3");
		ASSERT_EQUAL(byType["type10"].size(), 5);
		ASSERT_EQUAL(byType.at("type10")[4].type_, "type10");
		ASSERT_EQUAL(byType["Разное"].size(), 4);
	}
	{
		std::map<std::string, std::vector<Object>> byType = objectManager.GroupByType(10); //группировка по типу, минимальное количество объектов в группе 10
		ASSERT_EQUAL(byType.size(), 1);
		ASSERT_EQUAL(byType["Разное"].size(), 20);
	}
}
void TestGroupByName() {
	std::vector<Object> objects = {
		{"Абв", "type1", 1.0, 1.0, 1724499340},
		{"Аб", "type1", 2.0, 2.0, 1724499350},
		{"Абвг", "type1", 3.0, 3.0, 1724499360},
		{"Абв", "type2", 4.0, 4.0, 1724412940},
		{"Бд", "type2", 5.0, 5.0, 1724412930},
		{"Ббб", "type2", 6.0, 6.0, 1724412950},
		{"Бббб", "type3", 7.0, 7.0, 1724240140},
		{"А", "type3", 8.0, 8.0, 1724240150},
		{"Б", "type3", 9.0, 9.0, 1724240160},
		{"В", "type3", 10.0, 10.0, 1723376140},
		{"2А", "type3", 11.0, 11.0, 1723376150},
		{"5А", "type4", 12.0, 12.0, 1723376160},
		{"#Б", "type1", 13.0, 13.0, 1710156940},
		{"Abc", "type1", 14.0, 14.0, 1710156950},
		{"Abd", "type1", 15.0, 15.0, 1710156960},
		{"#H", "type10", 16.0, 16.0, 1678534540},
	};
	ObjectManager objectManager(objects);
	{
		std::map<std::string, std::vector<Object>> byName = objectManager.GroupByName();
		ASSERT_EQUAL(byName.size(), 4);
		ASSERT_EQUAL(byName["А"].size(), 5);
		ASSERT_EQUAL(byName["Б"].size(), 4);
		ASSERT_EQUAL(byName["В"].size(), 1);
		ASSERT_EQUAL(byName["#"].size(), 6);
		ASSERT_EQUAL(byName["А"][0].name_, "А");
		ASSERT_EQUAL(byName["А"][1].name_, "Аб");
		ASSERT_EQUAL(byName["#"][0].name_, "#H");
	}
}
void TestGroupByDistance() {
	std::vector<Object> objects = {
	{"14", "type1", 14.0, 14.0, 1710156950},
	{"15", "type1", 155.0, -150.0, 1710156960},
	{"16", "type10", 165.0, 165.0, 1678534540},
	{"17", "type10", 500, 3500.0, 1678534530},
	{"18", "type10", -1000.0, 2000.0, 1678534550},
	{"1", "type1", 1.0, 1.0, 1724499340},
	{"2", "type1", 2.0, 2.0, 1724499350},
	{"5", "type2", 5.0, 5.0, 1724412930},
	{"6", "type2", 6.0, 6.0, 1724412950},
	{"7", "type3", 7.0, 7.0, 1724240140},
	{"8", "type3", 8.0, 8.0, 1724240150},
	{"9", "type3", 9.0, 9.0, 1724240160},
	{"10", "type3", 10.0, 10.0, 1723376140},
	{"11", "type3", 11.0, 11.0, 1723376150},
	{"12", "type4", 12.0, 12.0, 1723376160},
	{"13", "type1", 13.0, 13.0, 1710156940},
	{"3", "type1", 3.0, 3.0, 1724499360},
	{"4", "type2", 4.0, 4.0, 1724412940},
	{"19", "type10", 1000, 2000.0, 1678534570},
	{"20", "type10", 10000.0, 9500.0, 1678534580}
	};
	ObjectManager objectManager(objects);
	{
		std::map<std::string, std::vector<Object>> byDistance = objectManager.GroupByDistance(0.0, 0.0); // Сортировка по расстоянию до точки 0.0 0.0
		ASSERT_EQUAL(byDistance.size(), 4);
		ASSERT_EQUAL(byDistance[TILL100].size(), 14);
		ASSERT_EQUAL(byDistance[TILL1000].size(), 16);
		ASSERT_EQUAL(byDistance[TILL10000].size(), 19);
		ASSERT_EQUAL(byDistance[TOOFAR].size(), 1);
		ASSERT_EQUAL(byDistance.at(TILL100)[0].name_, "1");
		ASSERT_EQUAL(byDistance.at(TILL100)[13].name_, "14");
		ASSERT_EQUAL(byDistance.at(TILL1000)[0].name_, "1");
		ASSERT_EQUAL(byDistance.at(TILL1000)[15].name_, "16");
		ASSERT_EQUAL(byDistance.at(TILL10000)[0].name_, "1");
		ASSERT_EQUAL(byDistance.at(TILL10000)[18].name_, "17");
		ASSERT_EQUAL(byDistance.at(TOOFAR)[0].name_, "20");
	}
	{
		std::map<std::string, std::vector<Object>> byDistance = objectManager.GroupByDistance(10001.0, 9501.0); // Сортировка по расстоянию до точки 10001.0 9501.0
		ASSERT_EQUAL(byDistance.size(), 4);
		ASSERT_EQUAL(byDistance[TILL100].size(), 1);
		ASSERT_EQUAL(byDistance[TILL1000].size(), 1);
		ASSERT_EQUAL(byDistance[TILL10000].size(), 1);
		ASSERT_EQUAL(byDistance[TOOFAR].size(), 19);
		ASSERT_EQUAL(byDistance.at(TILL100)[0].name_, "20");
		ASSERT_EQUAL(byDistance.at(TILL1000)[0].name_, "20");
		ASSERT_EQUAL(byDistance.at(TILL10000)[0].name_, "20");
		ASSERT_EQUAL(byDistance.at(TOOFAR)[0].name_, "17");
		ASSERT_EQUAL(byDistance.at(TOOFAR)[1].name_, "19");
		ASSERT_EQUAL(byDistance.at(TOOFAR)[2].name_, "18");
		ASSERT_EQUAL(byDistance.at(TOOFAR)[18].name_, "1");
	}
}
void TestObjectManager() {
	RUN_TEST(TestGroupByTime);
	RUN_TEST(TestGroupByType);
	RUN_TEST(TestGroupByName);
	RUN_TEST(TestGroupByDistance);
}