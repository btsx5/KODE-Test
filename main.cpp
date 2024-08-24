#include "object_manager.h"
#include "test_framework.h"
#include <string>
#include <iostream>


int main() {
	setlocale(LC_ALL, "Rus");
	RUN_TEST(TestObjectManager);
	ObjectManager objectManager;
	//objectManager.LoadFromJSON();
	objectManager.LoadObjects();
	std::string command;
	while (true) {
		std::cout << "Для сортировки по типу объекта введите - 1, по времени - 2, по имени - 3, по расстоянию - 4, для выхода - 5" << std::endl;
		std::cin >> command;

		if (command == "1") {
			objectManager.MakeOutputFile(objectManager.GroupByType());
		}
		else if (command == "2") {
			objectManager.MakeOutputFile(objectManager.GroupByTime());
		}
		else if (command == "3") {
			objectManager.MakeOutputFile(objectManager.GroupByName());
		}
		else if (command == "4") {
			objectManager.MakeOutputFile(objectManager.GroupByDistance());
		}
		else if (command == "5") {
			break;
		}
		else {
			std::cout << "Неизвестная команда" << std::endl;
		}
		std::cin.clear();
		std::flush(std::cout);
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	return 0;
}