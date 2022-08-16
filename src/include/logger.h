#include <iostream>
#include <string>

class Logger {
	public:
		inline void error(std::string desc, int code) {
			std::cout << "\e[1;31m[ERROR]: " << desc << " (" << code << ")\e[0m" << std::endl;
		}
		inline void warn(std::string warning) {
			std::cout << "\e[1;33m[WARNING]: " << warning << "\e[0m" << std::endl;
		}
		
		inline void info(std::string info) {
			std::cout << "\e[0;97m[INFO]: " << info << "\e[0m" << std::endl;
		}
};
