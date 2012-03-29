#ifndef __JM_BYTE_ARRAY_H__
#define __JM_BYTE_ARRAY_H__

#include <jm/jmtype.h>

JM_BEGIN_DECLS

typedef struct _JMByteArray JMByteArray;

JMByteArray* jm_byte_array_new(void);
JMByteArray* jm_byte_array_sized_new(size_t count);
void jm_byte_array_free(JMByteArray *self);
size_t jm_byte_array_size(JMByteArray *self);
uint8_t* jm_byte_array_data(JMByteArray *self);
void jm_byte_array_append(JMByteArray *self, const uint8_t *data, size_t count);

JM_END_DECLS

#endif