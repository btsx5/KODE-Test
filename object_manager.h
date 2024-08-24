#pragma once
#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <iomanip>
#include "constants.h"
#include "json.h"

struct Object {
	std::string name_; // имя объекта
	std::string type_; // тип объекта
	double x_ = 0.f; // координата x
	double y_ = 0.f; // координата y
	double timeStamp_ = 0.0;  // unix временная метка
	std::optional<double> distance_ = std::nullopt; // расстояние до указанной точки
};

class ObjectManager {
public:
	ObjectManager() = default;
	ObjectManager(std::vector<Object> objects);
	ObjectManager(const ObjectManager&) = delete;
	ObjectManager(ObjectManager&&) = delete;
	ObjectManager& operator=(const ObjectManager&) = delete;
	ObjectManager& operator=(ObjectManager&&) = delete;
	~ObjectManager() = default;
	const std::map<std::string, std::vector<Object>> GroupByTime();
	const std::map<std::string, std::vector<Object>> GroupByType(int minGroupSize);
	const std::map<std::string, std::vector<Object>> GroupByType();
	const std::map<std::string, std::vector<Object>> GroupByName();
	const std::map<std::string, std::vector<Object>> GroupByDistance(double x, double y);
	const std::map<std::string, std::vector<Object>> GroupByDistance();
	void LoadObjects();
	void LoadFromJSON();
	void MakeOutputFile(const std::map<std::string, std::vector<Object>>& objects);
	
private:
	std::vector<Object> objects_;
	double CalculateDistance(double x1, double y1, double x2, double y2);
	void ProcessObjects(std::ifstream& file);
	Object ParseObject(const std::string& line);
	std::tm unixTimestampToTm(unsigned long long timestamp);
	bool isToday(const std::tm& tm);
	bool isYesterday(const std::tm& tm);
	bool isThisWeek(const std::tm& tm);
	bool isThisMonth(const std::tm& tm);
	bool isThisYear(const std::tm& tm);
};