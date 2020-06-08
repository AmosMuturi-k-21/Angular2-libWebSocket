#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct json {
		uint8_t* buffer;
		uint32_t siz;
		uint32_t maxsiz;
		uint32_t kvsize;
		char isarray;
		char freeme;
	};

	enum {
		error_quote_key = -1,
		error_key_end = -2,
		error_key_missing = -3,
		error_value_missing = -4,
		error_missing_point = -5,

	} json_error_type;

	enum  {
		data_end,
		data_null,
		data_number,
		data_string,
		data_object,
		data_array,
		data_boolean
	} json_data_type;

	union json_value_data {
		uint8_t* ptr;
		struct json_token* next;
		float number;
	};

	struct json_value {
		uint8_t json_data_type;
		union json_value_data value;
	};

	struct json_token {
		uint8_t* key;
		struct json_value value;
	};

	void json_init_buffer(struct json* data, char *buffer, uint32_t size,char isarray);
	void json_free(struct json* data);
	void json_clear(struct json* data);
	void json_add(struct json* data, char* key, const char* type, void* value);

	int json_parse(struct json_token* list, int size, char * data, int datasize);
	struct json_value json_get_value(struct json_token* list, int size, char* key);

//#define check_val(json_value,val) ()

#define JISNUM(x) (x.json_data_type == data_number)
#define JISSTR(x) (x.json_data_type == data_string)

#define JADDSTR(k,x,y) json_add(&k, x, "%s", y);
#define JADDJSON(k,x,y) json_add(&k, x, "%j", y);
#define JADDNUM(k,x,y) {\
float temp = (y);\
if (isnan(temp)) temp = 0;\
json_add(&k, x, "%f", &temp);\
}

#ifdef __cplusplus
} // closing brace for extern "C"
#endif