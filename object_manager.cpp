#include "object_manager.h"

ObjectManager::ObjectManager(std::vector<Object> objects) : objects_(objects) {}
const std::map<std::string, std::vector<Object>> ObjectManager::GroupByTime() {
	std::sort(objects_.begin(), objects_.end(), [](const Object& a, const Object& b) {
		return a.timeStamp_ < b.timeStamp_;
		});

	std::map<std::string, std::vector<Object>> objectsByTime;
	for (const Object& obj : objects_) {
		std::tm objTime = unixTimestampToTm(obj.timeStamp_);
		bool old = true;
		if (isToday(objTime)) {
			objectsByTime[TODAY].push_back(obj);
			old = false;
		}
		if (isYesterday(objTime)) {
			objectsByTime[YESTERDAY].push_back(obj);
			old = false;
		}
		if (isThisWeek(objTime)) {
			objectsByTime[THIS_WEEK].push_back(obj);
			old = false;
		}
		if (isThisMonth(objTime)) {
			objectsByTime[THIS_MONTH].push_back(obj);
			old = false;
		}
		if (isThisYear(objTime)) {
			objectsByTime[THIS_YEAR].push_back(obj);
			old = false;
		}
		if (old) {
			objectsByTime[EARLIER].push_back(obj);
		}
	}
	return objectsByTime;
}
const std::map<std::string, std::vector<Object>> ObjectManager::GroupByType(int minGroupSize) {
	std::sort(objects_.begin(), objects_.end(), [](const Object& a, const Object& b) {
		if (a.type_ == b.type_) {
			return a.name_ < b.name_;
		}
		return a.type_ < b.type_;
		});

	std::map<std::string, std::vector<Object>> objectsByTypes;
	auto itStart = objects_.begin();
	auto itCurrent = objects_.begin() + 1;
	for (; itCurrent != objects_.end(); ++itCurrent) {
		if (itCurrent->type_ != itStart->type_) {
			if (std::distance(itStart, itCurrent) >= minGroupSize) {
				objectsByTypes[itStart->type_].insert(objectsByTypes[itStart->type_].end(), itStart, itCurrent);
			}
			else {
				objectsByTypes[OTHERTYPE].insert(objectsByTypes[OTHERTYPE].end(), itStart, itCurrent);
			}
			itStart = itCurrent;
		}
	}
	//обработка последней группы
	if (std::distance(itStart, itCurrent) >= minGroupSize) {
		objectsByTypes[objects_.back().type_].insert(objectsByTypes[objects_.back().type_].end(), itStart, itCurrent);
	}
	else {
		objectsByTypes[OTHERTYPE].insert(objectsByTypes[OTHERTYPE].end(), itStart, itCurrent);
	}
	return objectsByTypes;
}
const std::map<std::string, std::vector<Object>> ObjectManager::GroupByType() {
	std::cout << "Введите минимальный размер группы (>1):" << std::endl;
	int minGroupSize = 0;
	while (!(std::cin >> minGroupSize) || minGroupSize < 0) {
		std::cout << "Неправильный ввод. Введите не отрицательное число:" << std::endl;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return GroupByType(minGroupSize);
}
const std::map<std::string, std::vector<Object>> ObjectManager::GroupByName() {
	std::sort(objects_.begin(), objects_.end(), [](const Object& a, const Object& b) {
		return a.name_ < b.name_;
		});
	std::map<std::string, std::vector<Object>> objectsByNames;
	std::regex pattern(R"(^[А-ЯЁ])");
	for (const Object& obj : objects_) {
		std::string a(1, obj.name_[0]);
		if (std::regex_match(a, pattern)) {
			objectsByNames[a].push_back(obj);
		}
		else {
			objectsByNames[OTHERNAME].push_back(obj);
		}
	}
	return objectsByNames;
}
const std::map<std::string, std::vector<Object>> ObjectManager::GroupByDistance(double x, double y) {
	std::sort(objects_.begin(), objects_.end(), [x, y, this](const Object& a, const Object& b) {
		return CalculateDistance(a.x_, a.y_, x, y) < CalculateDistance(b.x_, b.y_, x, y);
		});

	std::map<std::string, std::vector<Object>> objectsByDistances;
	for (Object& obj : objects_) {
		obj.distance_ = CalculateDistance(obj.x_, obj.y_, x, y);
		if (obj.distance_ < 100) {
			objectsByDistances[TILL100].push_back(obj);
			objectsByDistances[TILL1000].push_back(obj);
			objectsByDistances[TILL10000].push_back(obj);
		}
		else if (obj.distance_ < 1000 && obj.distance_ >= 100) {
			objectsByDistances[TILL1000].push_back(obj);
			objectsByDistances[TILL10000].push_back(obj);
		}
		else if (obj.distance_ < 10000 && obj.distance_ >= 1000) {
			objectsByDistances[TILL10000].push_back(obj);
		}
		else {
			objectsByDistances[TOOFAR].push_back(obj);
		}
	}
	return objectsByDistances;
}
const std::map<std::string, std::vector<Object>> ObjectManager::GroupByDistance() {
    std::cout << "Enter the coordinates of the point x and y (example 0.0 1.4):" << std::endl;
    double x, y;
    while (!(std::cin >> x >> y)) {
    std::cout << "Invalid input. Please enter two numbers separated by a space:" << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return GroupByDistance(x, y);
}
void ObjectManager::LoadObjects() {
	std::ifstream file("..\\..\\..\\input.txt");
	if (!file) { // Проверка, успешно ли открыт файл
		std::cerr << "Can't open " << "input.txt" << std::endl;
		return; // Возвращаем ненулевой код ошибки
	}
	ProcessObjects(file);
	file.close();
}
void ObjectManager::MakeOutputFile(const std::map<std::string, std::vector<Object>>& objects) {
	std::cout << "Введите имя файла вывода:" << std::endl;
	std::string filename;
	std::cin >> filename;
	std::string fullPath = "..\\..\\..\\" + filename + ".txt";
	std::ofstream fileOut(fullPath);
	if (!fileOut) { // Проверка, успешно ли открыт файл
		std::cerr << "Не получилось создать/открыть " << filename << ".txt" << std::endl;
		return; 
	}

	for (const auto& [type, object] : objects) {
		fileOut << type << ":" << std::endl;
		for (const auto& obj : object) {
			fileOut << obj.name_ << " " << obj.x_ << " " << obj.y_ << " " << obj.type_ << " " << obj.timeStamp_;
			if (obj.distance_.has_value()) {

				fileOut << " " << std::setprecision(8) << obj.distance_.value();
			}
			fileOut << std::endl;
		}
	}
	std::cout << "Создание " << filename << ".txt" << " успешно." << std::endl;

	std::vector<Object> rawObjects_;
}
double ObjectManager::CalculateDistance(double x1, double y1, double x2, double y2) {
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}
void ObjectManager::ProcessObjects(std::ifstream& file) {
	std::string line;
	while (std::getline(file, line)) {
		objects_.push_back(ParseObject(line));
	}
}
Object ObjectManager::ParseObject(const std::string& line) {
	Object object;
	std::istringstream stream(line);
	stream >> object.name_ >> object.x_ >> object.y_ >> object.type_ >> object.timeStamp_;
	return object;
}
std::tm ObjectManager::unixTimestampToTm(unsigned long long timestamp) {
	std::time_t time = static_cast<std::time_t>(timestamp);
	std::tm tm = *std::gmtime(&time); // Преобразование в структуру tm в формате UTC
	return tm;
}
bool ObjectManager::isToday(const std::tm& tm) {
	std::time_t now = std::time(nullptr);
	std::tm today = *std::gmtime(&now);
	return (tm.tm_year == today.tm_year && tm.tm_yday == today.tm_yday);
}
bool ObjectManager::isYesterday(const std::tm& tm) {
	std::time_t now = std::time(nullptr);
	std::tm today = *std::gmtime(&now);
	return (tm.tm_year == today.tm_year && tm.tm_yday == today.tm_yday - 1);

}
bool ObjectManager::isThisWeek(const std::tm& tm) {
	std::time_t now = std::time(nullptr);
	std::tm today = *std::gmtime(&now);
	std::tm startOfWeek = today;
	std::tm startOfWeekRef = tm;
	startOfWeek.tm_mday -= today.tm_wday;
	startOfWeekRef.tm_mday -= tm.tm_wday;
	std::mktime(&startOfWeek);
	std::mktime(&startOfWeekRef);
	return 	(startOfWeek.tm_year == startOfWeekRef.tm_year && startOfWeek.tm_yday == startOfWeekRef.tm_yday);
}
bool ObjectManager::isThisMonth(const std::tm& tm) {
	std::time_t now = std::time(nullptr);
	std::tm today = *std::gmtime(&now);
	return (tm.tm_year == today.tm_year && tm.tm_mon == today.tm_mon);
}
bool ObjectManager::isThisYear(const std::tm& tm) {
	std::time_t now = std::time(nullptr);
	std::tm today = *std::gmtime(&now);
	return (tm.tm_year == today.tm_year);
}