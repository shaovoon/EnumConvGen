/***** EnumConvGen 1.0                  *****
 ***** Copyright 2016 by Wong Shao Voon *****/

 #include "EnumStrConv.h"
#include <iostream>
#include <sstream>

std::string EnumStrConv::result;

bool EnumStrConv::is_whitespace(char c)
{
	return c==' ' || c=='\r' || c=='\n' || c=='\t';
}

bool EnumStrConv::is_alphanum(char c)
{
	return (c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='_';
}

bool EnumStrConv::is_keyword(std::string str, std::string keyword, size_t pos)
{
	if(pos==std::string::npos)
		return false;

	bool ret = true;
	if(pos>0) // check prefix char
	{
		ret &= !is_alphanum(str[pos-1]);
	}
	if(pos+keyword.size()<str.size()) // check suffix char
	{
		ret &= !is_alphanum(str[pos+keyword.size()]);
	}
	return ret;
}

bool EnumStrConv::is_terminator(char c)
{
	return is_whitespace(c) || c==',' || c=='=' || c=='\'' || c=='}' || c==';' || (c>='0'&&c<='9');
}

void EnumStrConv::generate(const std::string& str, std::ostream& os, std::ostream& err_os)
{
	size_t pos=0;
	while(pos!=std::string::npos)
	{
		std::string enum_name="";
		std::string inner_enum_name="";
		std::string int_type="";
		std::vector<std::string> enums;
		bool enum_class = false;
		bool cpp98_enum = false;
		bool enclosing = false;

		parse(str, pos, enum_name, inner_enum_name, enums, enum_class, cpp98_enum, enclosing, int_type, err_os);
		os << output(str, enum_name, inner_enum_name, enums, enum_class, cpp98_enum, enclosing, int_type);
	}
}

const std::string& EnumStrConv::output(const std::string& str, const std::string& enum_name, const std::string& inner_enum_name, 
				   const std::vector<std::string>& enums, bool enum_class, bool cpp98_enum, bool enclosing, std::string int_type)
{
	if(int_type.empty())
		int_type = "int";

	std::ostringstream declarations_oss;
	std::ostringstream oss;
	std::string enum_type_str = enum_name;
	if(!inner_enum_name.empty())
	{
		enum_type_str += "::";
		enum_type_str += inner_enum_name;
	}
	{
		// std::string to_string(enum_type const & val);
		std::ostringstream temp_oss;
		temp_oss << "std::string to_string(" << enum_type_str << " const & val)";
		declarations_oss << temp_oss.str() << ";\n";

		temp_oss << "\n{";
		temp_oss << "\n\tswitch(val)";
		temp_oss << "\n\t{";
		for(size_t i=0; i<enums.size(); ++i)
		{
			if(enum_class)
			{
				temp_oss << "\n\t\tcase " << enum_name << "::" << enums[i] << ": \n\t\t\treturn \"" << enums[i] << "\";";
			}
			else if(enclosing)
			{
				temp_oss << "\n\t\tcase (" << int_type << ")" << enum_name << "::" << enums[i] << ": \n\t\t\treturn \"" << enums[i] << "\";";
			}
			else
			{
				temp_oss << "\n\t\tcase (" << int_type << ")" << enums[i] << ": \n\t\t\treturn \"" << enums[i] << "\";";
			}
		}

		temp_oss << "\n\t}";
		temp_oss << "\n\treturn \"\";";
		temp_oss << "\n}\n";

		oss << temp_oss.str() << std::endl;
	}
	{
		// void from_string(std::string const & val, enum_type & out_val);
		std::ostringstream temp_oss;
		temp_oss << "void from_string(std::string const & val, /* OUT */ " << enum_type_str << " & out_val)";
		declarations_oss << temp_oss.str() << ";\n";

		temp_oss << "\n{";
		for(size_t i=0; i<enums.size(); ++i)
		{
			if(i==0)
			{
				temp_oss << "\n\tif( ";
			}
			else
			{
				temp_oss << "\n\telse if( ";
			}
			
			temp_oss << "val == \"" << enums[i] << "\" )";
			if(enclosing||enum_class)
			{
				temp_oss << "\n\t{ out_val = " << enum_name << "::" << enums[i] << "; }";
			}
			else
			{
				temp_oss << "\n\t{ out_val = " << enums[i] << "; }";
			}
		}

		temp_oss << "\n\telse";
		temp_oss << "\n\t{";
		temp_oss << "\n\t\tstd::ostringstream oss;";
		temp_oss << "\n\t\toss << \"" << enum_type_str << " unknown match in string:\" << val;";
		temp_oss << "\n\t\tthrow std::runtime_error(oss.str());";
		temp_oss << "\n\t}";

		temp_oss << "\n}\n";

		oss << temp_oss.str() << std::endl;
	}
	{
		// int to_int(enum_type const& val);
		std::ostringstream temp_oss;
		temp_oss << int_type << " to_" << int_type << "(" << enum_type_str << " const & val)";
		declarations_oss << temp_oss.str() << ";\n";

		temp_oss << "\n{";
		temp_oss << "\n\treturn (" << int_type << ")val;";
		temp_oss << "\n}\n";
		oss << temp_oss.str() << std::endl;
	}
	{
		// void from_int(int val, enum_type & out_val);
		std::ostringstream temp_oss;
		temp_oss << "void from_" << int_type << "(" << int_type << " const & val, /* OUT */ " << enum_type_str << " & out_val)";
		declarations_oss << temp_oss.str() << ";\n";

		temp_oss << "\n{";
		temp_oss << "\n\tswitch(val)";
		temp_oss << "\n\t{";
		for(size_t i=0; i<enums.size(); ++i)
		{
			if(enclosing||enum_class)
			{
				temp_oss << "\n\t\tcase (" << int_type << ")" << enum_name << "::" << enums[i] << ": \n\t\t\tout_val = " << enum_name << "::" << enums[i] << "; break;";
			}
			else
			{
				temp_oss << "\n\t\tcase (" << int_type << ")" << enums[i] << ": \n\t\t\tout_val = " << enums[i] << "; break;";
			}
		}

		temp_oss << "\n\t\tdefault:";
		temp_oss << "\n\t\t{";
		temp_oss << "\n\t\t\tstd::ostringstream oss;";
		temp_oss << "\n\t\t\toss << \"" << enum_type_str << " unknown match in integral:\" << val;";
		temp_oss << "\n\t\t\tthrow std::runtime_error(oss.str());";
		temp_oss << "\n\t\t}";

		temp_oss << "\n\t}";
		temp_oss << "\n}\n";

		oss << temp_oss.str() << std::endl;
	}
	{
		// std::ostream & operator<< (std::ostream & os, enum_type const & val);
		std::ostringstream temp_oss;
		temp_oss << "std::ostream & operator<<(std::ostream & os, " << enum_type_str << " const & val)";
		declarations_oss << temp_oss.str() << ";\n";

		temp_oss << "\n{";
		temp_oss << "\n\treturn os << to_string(val);";
		temp_oss << "\n}\n";
		oss << temp_oss.str() << std::endl;
	}
	{
		// std::istream & operator>> (std::istream & is, enum_type & val);
		std::ostringstream temp_oss;
		temp_oss << "std::istream & operator>>(std::istream & is, " << enum_type_str << " & val)";
		declarations_oss << temp_oss.str() << ";\n";

		temp_oss << "\n{";
		temp_oss << "\n\tstd::string str;";
		temp_oss << "\n\tis >> str;";
		temp_oss << "\n\tfrom_string(str, val);";
		temp_oss << "\n\treturn is;";
		temp_oss << "\n}\n";

		oss << temp_oss.str() << std::endl;
	}

	std::ostringstream main_oss;
	main_oss << declarations_oss.str() << "\n";
	main_oss << oss.str();

	result = main_oss.str();

	return result;
}

bool EnumStrConv::parse(const std::string& str, size_t& pos, std::string& enum_name, std::string& inner_enum_name, 
	std::vector<std::string>& enums, bool& enum_class, bool& cpp98_enum, bool& enclosing, std::string& int_type, std::ostream& err_os)
{
	size_t start_pos = pos;
	size_t namespace_pos = str.find("namespace", pos);
	size_t struct_pos = str.find("struct", pos);
	size_t enum_pos = str.find("enum", pos);

	if(namespace_pos<struct_pos && namespace_pos<enum_pos)
	{
		if(is_keyword(str, "namespace", namespace_pos))
		{
			enclosing = true;
			namespace_pos += sizeof("namespace");
			pos = namespace_pos;
		}
	}
	else if(struct_pos<namespace_pos && struct_pos<enum_pos)
	{
		if(is_keyword(str, "struct", struct_pos))
		{
			enclosing = true;
			struct_pos += sizeof("struct");
			pos = struct_pos;
		}
	}
	else if(enum_pos<namespace_pos && enum_pos<struct_pos)
	{
		size_t class_pos = str.find("class", start_pos);
		size_t struct_pos = str.find("struct", start_pos);
		size_t brace_pos = str.find("{", start_pos);
		if(class_pos > enum_pos && class_pos < brace_pos)
		{
			if(is_keyword(str, "class", class_pos))
			{
				enum_class = true;
				pos = class_pos + sizeof("class");
			}
			else
			{
				cpp98_enum = true;
			}
		}
		else if(struct_pos > enum_pos && struct_pos < brace_pos)
		{
			if(is_keyword(str, "struct", struct_pos))
			{
				enum_class = true;
				pos = struct_pos + sizeof("struct");
			}
			else
			{
				cpp98_enum = true;
			}
		}
		else
		{
			cpp98_enum = true;
		}
		if(cpp98_enum&&is_keyword(str, "enum", enum_pos))
		{
			pos += sizeof("enum");
		}
	}
	else
	{
		pos=std::string::npos;
		err_os << "Error: No keyword found for struct, namespace and enum" << std::endl;
		return false;
	}

	while(is_whitespace(str[pos++])&&pos<str.size());

	if(pos>=str.size())
	{
		err_os << "Error: Premature end while searching struct or namespace name" << std::endl;
		return false;
	}

	--pos;
	while(!is_whitespace(str[pos])&&str[pos]!=':'&&pos<str.size())
	{
		enum_name += str[pos++];
	}

	if(pos>=str.size())
	{
		err_os << "Error: Premature end while populating struct or namespace name" << std::endl;
		return false;
	}

	bool within_enum = false;
	if(enclosing)
	{
		size_t enum_pos = str.find("enum", pos);
		if(!is_keyword(str, "enum", enum_pos))
		{
			err_os << "Error: Cannot find enum keyword in enclosing struct or namespace name" << std::endl;
			return false;
		}
		if(enum_pos!=std::string::npos)
		{
			size_t brace_begin = str.find("{", enum_pos);
			size_t colon_begin = str.find(":", enum_pos);
			if(brace_begin!=std::string::npos&&colon_begin!=std::string::npos&&colon_begin<brace_begin)
			{
				size_t enum_type_pos = colon_begin+1;
				while(is_whitespace(str[enum_type_pos++])&&enum_type_pos<brace_begin);
				--enum_type_pos;

				while(!is_whitespace(str[enum_type_pos])&&enum_type_pos<brace_begin)
				{
					int_type += str[enum_type_pos++];
				}
			}
			if(brace_begin!=std::string::npos)
			{
				while(is_terminator(str[++brace_begin])&&brace_begin<str.size());

				within_enum = (brace_begin<str.size());

				pos = brace_begin;
				
				enum_pos += 5;
				while(is_whitespace(str[enum_pos++])&&enum_pos<str.size());

				--enum_pos;

				while(!is_whitespace(str[enum_pos])&&str[enum_pos]!=':'&&enum_pos<str.size())
				{
					inner_enum_name += str[enum_pos++];
				}
			}
		}
	}
	else
	{
		size_t brace_begin = str.find("{", pos);
		size_t colon_begin = str.find(":", enum_pos);
		if(brace_begin!=std::string::npos&&colon_begin!=std::string::npos&&colon_begin<brace_begin)
		{
			size_t enum_type_pos = colon_begin+1;
			while(is_whitespace(str[enum_type_pos++])&&enum_type_pos<brace_begin);
			--enum_type_pos;

			while(!is_whitespace(str[enum_type_pos])&&enum_type_pos<brace_begin)
			{
				int_type += str[enum_type_pos++];
			}
		}
		if(brace_begin!=std::string::npos)
		{
			while(is_terminator(str[++brace_begin])&&brace_begin<str.size());
			
			within_enum = (brace_begin<str.size());
			
			pos = brace_begin;
		}
	}
	
	if(within_enum)
	{
		std::string name="";
		do
		{
			name="";
			while(!is_terminator(str[pos])&&pos<str.size())
			{
				name += str[pos++];
			}

			if(!name.empty())
			{
				enums.push_back(name);
			}
			
			--pos;
			size_t comma_pos = str.find(",", pos);
			size_t semi_colon_pos = str.find(";", pos);

			if(comma_pos!=std::string::npos&&semi_colon_pos!=std::string::npos)
			{
				if(comma_pos>semi_colon_pos)
				{
					if(enclosing&&semi_colon_pos!=std::string::npos)
					{
						pos = str.find(";", semi_colon_pos);
					}
					else if(semi_colon_pos!=std::string::npos)
					{
						pos = semi_colon_pos;
					}
					break;
				}
			}
			else if(comma_pos==std::string::npos)
			{
				pos = std::string::npos;
				break;
			}

			pos = comma_pos;

			while(is_terminator(str[pos++])&&pos<str.size())
			
			if(pos>=str.size())
			{
				pos = std::string::npos;
				break;
			}

			--pos;
		}
		while(name!="");
	}

	/*
	std::cout << "Name: "<< enum_name << std::endl;
	std::cout << "Inner Name: "<< inner_enum_name << std::endl;
	for(size_t i=0; i<enums.size(); ++i)
	{
		std::cout << "  " << enums[i] << std::endl;
	}
	*/

	return true;
}
