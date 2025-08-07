#include <stdlib.h>
#include <stdio.h>
#include <cjson/cJSON.h>

void *internal_calloc(size_t size){
	void *p = calloc(1,size);
	return p;
}
void internal_free(void *p){
	free(p);
}


int main(){
	cJSON_Hooks hooks;
	hooks.malloc_fn = internal_calloc;
	hooks.free_fn = internal_free;

	cJSON_InitHooks(&hooks);

	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root,"hello","name");

	char *json_string = cJSON_Print(root);
    	printf("JSON Output:\n%s\n", json_string);

    	free(json_string);  
	cJSON_Delete(root);
}
