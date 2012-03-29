#include "jmstringarray.h"
#include <string>
#include <vector>
#include <boost/array.hpp>

struct _JMStringArray
{
	std::vector<std::string> str_array;
};

JMStringArray* jm_string_array_new(void)
{
	JMStringArray *obj = new JMStringArray();
    return obj;
}

void jm_string_array_free(JMStringArray *array)
{
	if (array == NULL)
		return;
	delete array;
}

void jm_string_array_append(JMStringArray *array, const char *str)
{
	if (array == NULL)
		return;

	array->str_array.push_back(str);
}

size_t jm_string_array_size(JMStringArray *array)
{
	if (array == NULL)
		return 0;
	return array->str_array.size();
}

const char* jm_string_array_get(JMStringArray *array, size_t index)
{
	if (array == NULL)
		return NULL;
	if (index > array->str_array.size())
		return NULL;
	static boost::array<char, 255> temp;
	strcpy(temp.data(), array->str_array[index].c_str());
	return temp.data();
}
