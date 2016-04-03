/***** EnumConvGen 1.0                  *****
 ***** Copyright 2016 by Wong Shao Voon *****/

#include <string>
#include <vector>

struct EnumStrConv
{
	static bool is_whitespace(char c);
	static bool is_alphanum(char c);
	static bool is_keyword(std::string str, std::string keyword, size_t pos);
	static bool is_terminator(char c);
	
	static void generate(const std::string& str, std::ostream& os, std::ostream& err_os);

	static bool parse(const std::string& str, size_t& pos, std::string& enum_name, std::string& inner_enum_name, 
		std::vector<std::string>& enums, bool& enum_class, bool& cpp98_enum, bool& enclosing, std::string& int_type, std::ostream& err_os);

	static const std::string& output(const std::string& str, const std::string& enum_name, const std::string& inner_enum_name,
	   const std::vector<std::string>& enums, bool enum_class, bool cpp98_enum, bool enclosing, std::string int_type);

	static std::string result;
};
	