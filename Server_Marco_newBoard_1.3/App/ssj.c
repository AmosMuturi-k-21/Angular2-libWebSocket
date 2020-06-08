#include "ssj.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void json_putchar(struct json* data, char value) {
	data->buffer[data->siz++] = value;
	data->buffer[data->siz] = 0;
}
static void json_putstr(struct json* data, char* value) {
	while (*value) json_putchar(data, *value++);
}
static void json_removelast(struct json* data){
	data->siz--;
	data->buffer[data->siz] = 0;
}
static void json_add_braket(struct json* data) {
	if (data->isarray) {
		json_putchar(data, '[');
		json_putchar(data, ']');
		return;
	}
	json_putchar(data,'{');
	json_putchar(data,'}');
}
static void json_add_endbraket(struct json* data) {
	if (data->isarray) {
		json_putchar(data, ']');
		return;
	}
	json_putchar(data, '}');
}
static void json_remove_endbraket(struct json* data) {
	json_removelast(data);
}

void json_init_buffer(struct json* data, char *buffer,uint32_t size, char isarray) {
	data->maxsiz = size;
	data->buffer = buffer;
	data->siz = 0;
	data->kvsize = 0;
	data->isarray = isarray;
	if (buffer == 0) {
		data->buffer = (char*)malloc(size);
		data->freeme = 1;
	}
	json_add_braket(data);
}

void json_free(struct json* data) {
	data->buffer = 0;
	if (data->freeme) {
		free(data->buffer);
	}
}
void json_clear(struct json* data) {
	data->siz = 0;
	data->kvsize = 0;
	json_add_braket(data);
}

void json_add(struct json* data,char* key,const char* type,void* value) {
	json_remove_endbraket(data);

	if (data->kvsize > 0)
		json_putchar(data, ',');

	//printf("%d\n", data->siz);
	//printf("%s\n", data->buffer);
	//printf("ptr = %d\n",value);
	//printf("ptr = %d\n", *((char*)value));
	if (!data->isarray) {
		json_putchar(data, '"');
		json_putstr(data, key);
		json_putchar(data, '"');
		json_putchar(data, ':');
	}

	if (strcmp(type, "%s") == 0) {
		json_putchar(data, '"');
		data->siz += sprintf(&data->buffer[data->siz], type, value);
		json_putchar(data, '"');
	}
	else
		if (strcmp(type, "%j") == 0) { //json or array
			data->siz += sprintf(&data->buffer[data->siz], "%s", value);
		}
	else {
		float tempdata = *((float*)value);
		data->siz += sprintf(&data->buffer[data->siz], type, tempdata);
	}

	json_add_endbraket(data);

	data->kvsize++;

	//printf("%s\n", data->buffer);
	//crash
	//memset(0, 0, 1024);
}


struct json_value json_get_value(struct json_token* list,int size,char* key) {
	int index = 0;
	while (index < size) {
		if (strcmp(list[index].key, key) == 0) {
			return list[index].value;
		}
		index++;
	}

	//return null
	struct json_value val;
	val.json_data_type = data_null;
	val.value.number = 0;
	return val;
}

#define CK(x) (data[index] == x)
#define CKNUM() (data[index] >= '0' && data[index] <= '9' || data[index] == '-')
#define CMP(x) (memcmp(&data[index],x,strlen(x)) == 0)
#define END() (CK(0) || index >= datasize)

int json_parse(struct json_token* list,int size,char * data,int datasize) {
	int list_index = 0;
	int list_size = 0;
	int index = 0;
	int temp = 0;

	//key search
key_search:
	if (END()){ //endlist
		list[list_size].value.json_data_type = data_end;
		return list_size; 
	}
	if (CK('{') || CK('[') || CK(' ') || CK(',') || CK('\n') || CK('\r')) {
		index++;
		goto key_search;
	}
	if (CK('}')) return list_size; //endlist
	if (!CK('"')) return error_quote_key; //parse error
	index++;
	temp = index;
	list[list_size].key = &data[index];//get pointer

key_get:
	if (END()) return error_key_end; //parse error
	if (!CK('"')) {
		index++;
		goto key_get;
	}
	temp -= 1;
	if (temp == 0) return error_key_missing; //parse error

	data[index] = 0; //tokenize
	index++;

search_value:
	if (END()) return error_value_missing; //endlist
	if (CK(' ') || CK('\n') || CK('\r')) {
		index++;
		goto search_value;
	}
	if (!CK(':')) return error_missing_point; //parse error
	index++;

scan_value:
	if (END()) return error_value_missing; //endlist
	if (CK(' ') || CK('\n') || CK('\r')) {
		index++;
		goto scan_value;
	}
	if (CK('"')) {
		index++;
		list[list_size].value.value.ptr = &data[index];//get pointer
		list[list_size].value.json_data_type = data_string;//get pointer
		goto get_string;
	}else
	if (CMP("null") || CMP("NULL")) {
		index += 4;
		data[index] = 0; //tokenize
		index++;
		list[list_size].value.value.number = 0;
		list[list_size].value.json_data_type = data_null;//get pointer
		goto end;
	}
	else
	if (CMP("true") || CMP("TRUE")) {
		index += 4;
		data[index] = 0; //tokenize
		index++;
		list[list_size].value.value.number = 1;
		list[list_size].value.json_data_type = data_boolean;//get pointer
		goto end;
	}else
	if (CMP("false") || CMP("FALSE")) {
		index += 5;
		data[index] = 0; //tokenize
		index++;
		list[list_size].value.value.number = 0;
		list[list_size].value.json_data_type = data_boolean;//get pointer
		goto end;
	}else
	if (CKNUM()) {
		temp = index;
		list[list_size].value.value.number = 0;
		list[list_size].value.json_data_type = data_number;//get pointer
		goto get_number;
	}

	return -1; //parse error

get_string:
	if (END()) return error_value_missing; //endlist
	if (!CK('"')) {
		index++;
		goto get_string;
	}
	else {
		//is the real end line? check for \"
		if (data[index - 1] == '\\') {
			index++;
			goto get_string;
		}
	}
	data[index] = 0; //tokenize
	index++;
	goto end;

get_number:
	if (END()) return error_value_missing; //endlist
	if (CKNUM() || CK('.')) {
		index++;
		goto get_number;
	}
	data[index] = 0; //tokenize
	index++;

	list[list_size].value.value.number = atof(&data[temp]);
	goto end;

end:
	list_size++;
	goto key_search;

}