#include "Singleton.h"
#include <iostream>
#include <string>

class Test : public Singleton<Test>
{
	public:
		void write(const std::string& text)
		{
			std::cout << text << std::endl;
		}
};

int main(int argc, char** argv)
{
	Singleton<Test>::instance().write("Hello");
	Singleton<Test, RealLock>::instance().write("World");
	return 0;
}
