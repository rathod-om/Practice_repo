#include <stdio.h>
#include<stdlib.h>
#include <cjson/cJSON.h>

int main() {
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "Om");
    cJSON_AddNumberToObject(root, "age", 19);

    char *json_string = cJSON_Print(root);
    printf("JSON Output:\n%s\n", json_string);

    cJSON_Delete(root);
    free(json_string);
    return 0;
}

