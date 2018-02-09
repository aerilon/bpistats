#include <exception>
#include <iostream>

#include <main.hpp>

int
main(int argc, const char** argv)
{
	try
	{
		bpi::main(argc, argv)();
	}
	catch (std::exception &e)
	{
		std::cout << "uncaught exception: " << e.what() << std::endl;
	}

	return 0;
}
