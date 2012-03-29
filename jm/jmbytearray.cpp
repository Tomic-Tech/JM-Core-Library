#include "jmbytearray.h"
#include <boost/smart_ptr.hpp>

struct _JMByteArray
{
	uint8_t *arr;
	size_t length;
	size_t allocated;
};

JMByteArray* jm_byte_array_new(void)
{
	JMByteArray *obj = new JMByteArray();
	obj->arr = NULL;
	obj->length = 0;
	obj->allocated = 0;
	return obj;
}

JMByteArray* jm_byte_array_sized_new(size_t count)
{
	JMByteArray *obj = new JMByteArray();
	obj->arr = new uint8_t[count];
	obj->length = 0;
	obj->allocated = count;
	return obj;
}

void jm_byte_array_free(JMByteArray *self)
{
	if (self == NULL)
		return;
	if (self->arr != NULL)
		delete[] self->arr;
	delete self;
}

size_t jm_byte_array_size(JMByteArray *self)
{
	if (self == NULL)
		return 0;
	return self->length;
}

uint8_t* jm_byte_array_data(JMByteArray *self)
{
	if (self == NULL)
		return NULL;

	return self->arr;
}

void jm_byte_array_append(JMByteArray *self, const uint8_t *data, size_t count)
{
	if (self == NULL)
		return;

	if (self->allocated < (self->length + count))
	{
		self->allocated = (self->allocated + count) * 2;
		uint8_t *new_buff = new uint8_t[self->allocated];
		if (self->arr != NULL)
		{
			memcpy(new_buff, self->arr, self->length);
			delete[] self->arr;
			self->arr = new_buff;
		}
	}

	memcpy(self->arr + self->length, data, count);
	self->length += count;
}
