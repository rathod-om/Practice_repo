#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>

#define JSON_FILE "data.json"
#define MAX_LEN 100

void display_employee(cJSON *root);
void add_employee(cJSON *root);
void update_employee(cJSON *root);
void remove_employee(cJSON *root);
void save_to_file(cJSON *root);
cJSON * load_json();


int main(){
	cJSON *root = load_json();
	if (!root) return 1;
	int choice;

	do {
		printf("1.Display\n2.add\n3.update\n4.remove\n5.save and exit\n");
		scanf("%d",&choice);
		getchar();
		switch(choice){
			case 1:
				display_employee(root);
				break;
			case 2:
				add_employee(root);	
				break;
			case 3:
				update_employee(root);
				break;
			case 4:
				remove_employee(root);
				break;
			case 5:
				save_to_file(root);
				break;
			default:
				printf("Invalid choice\n");
				break;
		}
	}while (choice != 5);
	cJSON_Delete(root);
	return 0;
}


cJSON *load_json(){
	FILE *fp = fopen(JSON_FILE,"r");
	if (!fp){
		printf("Creating json file\n");
		cJSON *new_root = cJSON_CreateObject();
		cJSON_AddItemToObject(new_root,"Employees",cJSON_CreateArray());
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

void add_employee(cJSON *root){
	char name[MAX_LEN];
	int GID;
	char domain[MAX_LEN];

	printf("Enter employee name: ");
	fgets(name,sizeof(name),stdin);
	name[strcspn(name,"\n")] = 0;

	printf("Enter GID: ");
	scanf("%d",&GID);
	getchar();

	printf("Enter employee domain: ");
	fgets(domain,sizeof(domain),stdin);
	domain[strcspn(domain,"\n")] = 0;


	cJSON *employees = cJSON_GetObjectItem(root,"employees");
	cJSON *add_new = cJSON_CreateObject();
	cJSON_AddStringToObject(add_new,"name",name);
	cJSON_AddNumberToObject(add_new,"GID",GID);
	cJSON_AddStringToObject(add_new,"Domain",domain);
	cJSON_AddItemToArray(employees,add_new);

	printf("Student added\n");
}

void update_employee(cJSON * root){
	int GID;
	printf("Enter GID to update employee information: ");
	scanf("%d",&GID);
	getchar();

	cJSON *employees = cJSON_GetObjectItem(root,"employees");
	int size = cJSON_GetArraySize(employees);
	for (int i =0;i<size;i++){
		cJSON *element = cJSON_GetArrayItem(employees,i);
		cJSON *element_GID = cJSON_GetObjectItem(element,"GID");
		if ( element_GID->valuedouble == GID ){
			printf("Enter name to change: ");
			char name[MAX_LEN];
			fgets(name,sizeof(name),stdin);
			name[strcspn(name,"\n")] = 0;

			cJSON_ReplaceItemInObject(element,"name",cJSON_CreateString(name));
			printf("employee name updated\n");
			return;
		}
	}
	printf("employee with GID:%d not found\n",GID);
}

void remove_employee(cJSON *root){
	int GID;
	printf("Enter GID of employee to remove data: ");
	scanf("%d",&GID);
	getchar();

	cJSON *employees = cJSON_GetObjectItem(root,"employees");
	int size = cJSON_GetArraySize(employees);
	for (int i = 0; i < size ; i++ ){
		cJSON *element = cJSON_GetArrayItem(employees,i);
		cJSON *element_GID = cJSON_GetObjectItem(element,"GID");
		if (element_GID->valuedouble == GID){
			cJSON_DeleteItemFromArray(employees,i);
			printf("Employee data removed\n");
			return;
		}
	}
	printf("employee with GID:%d not found\n",GID);


}

void display_employee(cJSON *root){

	char *json_str = cJSON_Print(root);
	printf("%s\n",json_str);
	free(json_str);

}

void save_to_file(cJSON *root){
	char *json_str = cJSON_Print(root);
	FILE *fp = fopen(JSON_FILE,"w");
	if(fp){
		fprintf(fp, "%s",json_str);
		fclose(fp);
	}
	free(json_str);
}

