#ifndef FORMAT_H
#define FORMAT_H

#include "storm.h"

extern nil_t result_fmt(str_t *buffer, result_t error);
extern result_t value_fmt(str_t *buffer, value_t value);

#endif
