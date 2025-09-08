#include <stdio.h>
#include <stdlib.h>
#include <string.h>                                         
#include <cjson/cJSON.h>
#include <time.h>

//#include "log_func.h"

#define JSON_FILE "data.json"
#define MAX_LEN 100

/* Function Declaration */
cJSON *load_json();
void save_to_file(cJSON *);
void add_string(cJSON *);
void add_array_string(cJSON *);
void add_number(cJSON *);
void add_boolean(cJSON *);
void add_null(cJSON *);
void display_json_data(cJSON *root);
void remove_from_object(cJSON *root);
void nested_object(cJSON *root);
void update_item(cJSON *root);

//FILE *log_file = NULL;

int main(){
	cJSON *root = load_json();
	int choice;
	do{
		printf("Options to add to Object\n\t1. Display\n\t2. Add string\n\t3. Add number\n\t4. Add boolean\n\t5. Add null\n\t6. add Array\n\t7. Nested Object\n\t8. Update item\n\t9. Remove\n\t10. Exit\n");
		printf("Enter your choice:");
		scanf("%d",&choice);
		getchar();

		switch(choice){
			case 1:
				display_json_data(root);
				break;
			case 2:
				add_string(root);
				break;
			case 3:
				add_number(root);	
				break;
			case 4: 
				add_boolean(root);
				break;
			case 5:
				add_null(root);
				break;
			case 6:
				add_array_string(root);
				break;
			case 7:
				nested_object(root);
				break;
			case 8:
				update_item(root);
				break;
			case 9:
				remove_from_object(root);
				break;
			case 10:
				printf("Exiting Program\n");
				break;
			default:
				printf("Invalid input\n");

		}
		save_to_file(root);
	}while(choice!=10);

	return 0;
}

void nested_object(cJSON *root){
	char key[MAX_LEN];

	printf("Enter key: ");
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = 0;

	cJSON *nested = cJSON_CreateObject();

	int choice;
	do{
		printf("Options to add to Object\n\t1. add string\n\t2. add number\n\t3. add boolean\n\t4. add null\n\t5. add string array\n\t6. Keep empty\n\t7.Back menu\n");
		printf("Enter your choice:");
		scanf("%d",&choice);
		getchar();

		switch(choice){
			case 1:
				add_string(nested);
				break;
			case 2:
				add_number(nested);
				break;
			case 3:
				add_boolean(nested);
				break;
			case 4:
				add_null(nested);
				break;
			case 5: 
				add_array_string(nested);
				break;
			case 6:
				printf("No element is added in nested object\n");
				break;
			case 7:
				printf("entered in main menu\n");
				break;
			default: 
				printf("Invalid input\n");

		}
	}while(choice != 7);



	cJSON_AddItemToObject(root,key,nested);	
}

void add_array_string(cJSON *root){
	char key[MAX_LEN];
	char value[MAX_LEN];
	int n;

	printf("Enter key: ");
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = 0;

	printf("Enter number of elements: ");
	scanf("%d",&n);;
	while(getchar()!= '\n');

	cJSON *array = cJSON_CreateArray();
	for (int i=0;i<n;i++){
		printf("Enter element %d: ",i+1);
		fgets(value, sizeof(value), stdin);
		value[strcspn(value, "\n")] = 0;

		cJSON_AddItemToArray(array,cJSON_CreateString(value));
	}
	cJSON_AddItemToObject(root,key,array);
	//save_to_file(root);

}

void add_string(cJSON *root){
	char str_key[MAX_LEN];
	char str_value[MAX_LEN];
	printf("Enter key: ");
	fgets(str_key,sizeof(str_key),stdin);
	str_key[strcspn(str_key,"\n")] = 0;

	printf("Enter value: ");
	fgets(str_value,sizeof(str_value),stdin);
	str_value[strcspn(str_value,"\n")] = 0;
	cJSON_AddStringToObject(root,str_key,str_value);
	//save_to_file(root);
}

cJSON *load_json(){
	FILE *fp = fopen(JSON_FILE,"r");
	if (!fp){
		printf("Creating json file\n");
		cJSON *new_root = cJSON_CreateObject();
		return new_root;
	}

	fseek(fp,0,SEEK_END);

	long len = ftell(fp);
	rewind(fp);

	char *data = (char *)malloc(len+1);
	fread(data,1,len,fp);
	data[len] = '\0';
	fclose(fp);

	cJSON *root = cJSON_Parse(data);
	free(data);
	if(!root){
		printf("Error parsing JSON.\n");
		return NULL;
	}
	return root;
}

void add_number(cJSON *root) {
	char key[MAX_LEN];
	double number;

	printf("Enter key: ");
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = 0;

	printf("Enter number value: ");
	scanf("%lf", &number);
	while(getchar() != '\n'); 

	cJSON_AddNumberToObject(root, key, number);
	//save_to_file(root);
}

void add_boolean(cJSON *root) {
	char key[MAX_LEN];
	int bool_val;

	printf("Enter key: ");
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = 0;

	printf("Enter boolean value (0 for false, 1 for true): ");
	scanf("%d", &bool_val);
	while(getchar() != '\n');

	cJSON_AddBoolToObject(root, key, bool_val ? cJSON_True : cJSON_False);
	//save_to_file(root);
}


void add_null(cJSON *root) {
	char key[MAX_LEN];

	printf("Enter key: ");
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = 0;

	cJSON_AddNullToObject(root, key);
	//save_to_file(root);
}

void remove_from_object(cJSON *root){
	char key[MAX_LEN];
	printf("Enter key to remove item: ");
	fgets(key, sizeof(key), stdin);
	key[strcspn(key, "\n")] = 0;
	
	cJSON *item = cJSON_GetObjectItemCaseSensitive(root,key);

	if (item == NULL) {
        	printf("Key not found.\n");
        	return;
    	}

	if (cJSON_IsObject(item)){
		if (cJSON_GetArraySize(item) == 0){
			cJSON_DeleteItemFromObject(root,key);
			return;
		}
		remove_from_object(item);
	}
	else{	
		cJSON_DeleteItemFromObject(root,key);
	}
	
	//save_to_file(root);	
}

void update_item(cJSON *root){
	char str_key[MAX_LEN];
	char str_value[MAX_LEN];
	printf("Enter key: ");
	fgets(str_key,sizeof(str_key),stdin);
	str_key[strcspn(str_key,"\n")] = 0;

	cJSON *item = cJSON_GetObjectItemCaseSensitive(root,str_key);

	if(cJSON_IsString(item)){
		printf("Enter value to update: ");
		fgets(str_value,sizeof(str_value),stdin);
		str_value[strcspn(str_value,"\n")] = 0;
		cJSON_ReplaceItemInObject(root,str_key,cJSON_CreateString(str_value));
	}
	else if (cJSON_IsNumber(item)) {
		printf("Current type: Number\nEnter new value: ");
		fgets(str_value, sizeof(str_value), stdin);
		cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateNumber(atof(str_value)));
	}

	else if (cJSON_IsObject(item)){
		cJSON *nested = cJSON_GetObjectItem(root,str_key);
		update_item(nested);
	}

	else if (cJSON_IsArray(item)){
		cJSON *array = cJSON_GetObjectItem(root,str_key);
		int index;
		printf("Enter index to update value: ");
		scanf("%d",&index);
		while(getchar()!= '\n');

		if (index > cJSON_GetArraySize(array)){
			printf("Array out of bound\n");
			return;
		} 

		char new_value[MAX_LEN];
		printf("Enter new value: ");
		fgets(new_value,sizeof(new_value),stdin);
		new_value[strcspn(new_value,"\n")] = 0;

		cJSON_ReplaceItemInArray(array,index,cJSON_CreateString(new_value));
		printf("Item has been updated\n");

	}

	else if (cJSON_IsTrue(item) || cJSON_IsFalse(item)) {
		printf("Current type: Boolean\nEnter new value (true/false): ");
		fgets(str_value, sizeof(str_value), stdin);
		str_value[strcspn(str_value, "\n")] = 0;
		if (strcmp(str_value, "true") == 0)
			cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateTrue());
		else if (strcmp(str_value, "false") == 0)
			cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateFalse());
		else
			printf("Invalid boolean value.\n");
	} 
	else if (cJSON_IsNull(item)) {
		printf("Current type: Null\nEnter new value (null/string/number/true/false): ");
		fgets(str_value, sizeof(str_value), stdin);
		str_value[strcspn(str_value, "\n")] = 0;

		if (strcmp(str_value, "null") == 0)
			cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateNull());
		else if (strcmp(str_value, "true") == 0)
			cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateTrue());
		else if (strcmp(str_value, "false") == 0)
			cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateFalse());
		else if (strspn(str_value, "0123456789.") == strlen(str_value))
			cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateNumber(atof(str_value)));
		else
			cJSON_ReplaceItemInObject(root, str_key, cJSON_CreateString(str_value));
	} 

	else
	{
		printf("Can't update\n");
	}
}

void display_json_data(cJSON *root){
	char *json_str = cJSON_Print(root);
	printf("%s\n",json_str);
	free(json_str);
}

void save_to_file(cJSON *root){
	char *json_str = cJSON_Print(root);
	FILE *fp = fopen(JSON_FILE,"w");
	if(fp){
		fprintf(fp, "%s\n",json_str);
		fclose(fp);
	}
	free(json_str);
}




