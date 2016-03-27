/***** EnumConvGen 1.0                  *****
 ***** Copyright 2016 by Wong Shao Voon *****/

#include "EnumStrConv.h"

#ifdef __EMSCRIPTEN__
#include <iostream>
#include <sstream>

extern "C" const char* gen_enum_conv(const char* cs);

extern "C" const char* gen_enum_conv(const char* cs)
{
	std::string str = cs;

	std::ostringstream oss;
	std::ostringstream err_oss;
	EnumStrConv::generate(str, oss, err_oss);

	EnumStrConv::error = err_oss.str();

	if(!EnumStrConv::error.empty())
		EnumStrConv::result = EnumStrConv::error;

	return EnumStrConv::result.c_str();
}

int main(int argc, char* argv [])
{
	return 0;
}
#else
#include <iostream>
#include <fstream>

int main(int argc, char* argv [])
{
	std::ifstream ifs("c:\\temp\\enum_example.txt");

	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	std::ofstream ofs("c:\\temp\\src.cpp");
	if (ofs.is_open())
	{
		EnumStrConv::generate(str, ofs, std::cerr);
		ofs.flush();
		ofs.close();
	}

	return 0;
}
#endif

