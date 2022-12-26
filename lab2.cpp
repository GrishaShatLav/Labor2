#include <iostream>
#include <string>
#include "Array.h"

int main() {
	Array<std::string> arr;

	try {
		arr.insert("10");
		arr.insert("20");
		arr.insert("30");
		arr.insert("40");
		arr.insert(0, "50");

		for (auto it = arr.iterator(); it.hasNext(); it.next()) {
			std::cout << it.get() << ' ';
		}

		std::cout << std::endl;
	}
	catch (const char* exception) {
		std::cout << exception << std::endl;
	}
}
