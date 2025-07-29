#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#define JSON_FILE "students.json"

void display_students(cJSON *root);
void add_student(cJSON *root);
void update_student(cJSON *root);
void remove_student(cJSON *root);
void save_to_file(cJSON *root);
cJSON* load_json();

int main() {
	cJSON *root = load_json();
	if (!root) return 1;

	int choice;
	do {
		printf("\n--- Student Manager ---\n");
		printf("1. Display Students\n");
		printf("2. Add Student\n");
		printf("3. Update Student Age\n");
		printf("4. Remove Student\n");
		printf("5. Save and Exit\n");
		printf("Enter your choice: ");
		scanf("%d", &choice);
		getchar(); // consume newline

		switch (choice) {
			case 1: display_students(root); break;
			case 2: add_student(root); break;
			case 3: update_student(root); break;
			case 4: remove_student(root); break;
			case 5: save_to_file(root); printf("Changes saved.\n"); break;
			default: printf("Invalid choice.\n");
		}
	} while (choice != 5);

	cJSON_Delete(root);
	return 0;
}

cJSON* load_json() {
	FILE *fp = fopen(JSON_FILE, "r");
	if (!fp) {
		// If file doesn't exist, create a new root with empty students array
		printf("JSON file not found. Creating new one.\n");
		cJSON *new_root = cJSON_CreateObject();
		cJSON_AddItemToObject(new_root, "students", cJSON_CreateArray());
		return new_root;
	}

	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	rewind(fp);

	char *data = (char *)malloc(len + 1);
	fread(data, 1, len, fp);
	data[len] = '\0';
	fclose(fp);

	cJSON *root = cJSON_Parse(data);
	free(data);
	if (!root) {
		printf("Error parsing JSON.\n");
		return NULL;
	}
	return root;
}

void display_students(cJSON *root) {
	cJSON *students = cJSON_GetObjectItem(root, "students");
	if (!cJSON_IsArray(students)) return;

	printf("\n-- Students List --\n");
	int size = cJSON_GetArraySize(students);
	for (int i = 0; i < size; i++) {
		cJSON *stu = cJSON_GetArrayItem(students, i);
		cJSON *name = cJSON_GetObjectItem(stu, "name");
		cJSON *age = cJSON_GetObjectItem(stu, "age");
		printf("%d. %s (Age %d)\n", i + 1, name->valuestring, age->valueint);
	}
}

void add_student(cJSON *root) {
	char name[100];
	int age;

	printf("Enter student name: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = 0; // remove newline

	printf("Enter age: ");
	scanf("%d", &age);
	getchar(); // consume newline

	cJSON *students = cJSON_GetObjectItem(root, "students");
	cJSON *stu = cJSON_CreateObject();
	cJSON_AddStringToObject(stu, "name", name);
	cJSON_AddNumberToObject(stu, "age", age);
	cJSON_AddItemToArray(students, stu);

	printf("Student added.\n");
}

void update_student(cJSON *root) {
	char name[100];
	int new_age;

	printf("Enter student name to update: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = 0;

	cJSON *students = cJSON_GetObjectItem(root, "students");
	int size = cJSON_GetArraySize(students);
	for (int i = 0; i < size; i++) {
		cJSON *stu = cJSON_GetArrayItem(students, i);
		cJSON *stu_name = cJSON_GetObjectItem(stu, "name");
		if (strcmp(stu_name->valuestring, name) == 0) {
			printf("Enter new age: ");
			scanf("%d", &new_age);
			getchar();
			cJSON_ReplaceItemInObject(stu, "age", cJSON_CreateNumber(new_age));
			printf("Student age updated.\n");
			return;
		}
	}
	printf("Student not found.\n");
}

void remove_student(cJSON *root) {
	char name[100];
	printf("Enter student name to remove: ");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = 0;

	cJSON *students = cJSON_GetObjectItem(root, "students");
	int size = cJSON_GetArraySize(students);
	for (int i = 0; i < size; i++) {
		cJSON *stu = cJSON_GetArrayItem(students, i);
		cJSON *stu_name = cJSON_GetObjectItem(stu, "name");
		if (strcmp(stu_name->valuestring, name) == 0) {
			cJSON_DeleteItemFromArray(students, i);
			printf("Student removed.\n");
			return;
		}
	}
	printf("Student not found.\n");
}

void save_to_file(cJSON *root) {
	char *json_str = cJSON_Print(root);
	FILE *fp = fopen(JSON_FILE, "w");
	if (fp) {
		fprintf(fp, "%s", json_str);
		fclose(fp);
	}
	free(json_str);
}

