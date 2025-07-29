// json_create.c
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

int main() {
    // Create JSON object
    cJSON *person = cJSON_CreateObject();

    // Add key-value pairs
    cJSON_AddStringToObject(person, "name", "Om Rathod");
    cJSON_AddNumberToObject(person, "age", 19);
    cJSON_AddStringToObject(person, "city", "Mumbai");

    // Convert to JSON string
    char *json_str = cJSON_Print(person);

    // Write to file
    FILE *fp = fopen("output.json", "w");
    if (fp == NULL) {
        perror("Failed to create file");
        return 1;
    }

    fprintf(fp, "%s", json_str);
    fclose(fp);

    // Cleanup
    cJSON_Delete(person);
    free(json_str);

    printf("JSON file created: output.json\n");
    return 0;
}

