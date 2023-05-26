#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "database.h"
#define MAX_LINE_LEN 300
#define MAX_NAME_LEN 100
#define MAX_TARGET_NUM 150
#define CATEGORY_NUM 13
const char *category_table[CATEGORY_NUM] = {"便當", "飯糰", "飲料", "麵包", "麵", "沙拉", "粥",
                                            "湯", "水餃、鍋貼", "甜品", "冰品", "糖果", "餅乾" };

//print current category table
void print_category_table(void){
    for(int i = 0; i < CATEGORY_NUM; i++)
        printf("%2d. %s\n", i+1,category_table[i]);
}
//add a new item to the last of the list
void add_to_item_list(struct item **head, struct item *new_item){
    if (new_item == NULL){
        printf("Fail to %s\n", __func__);
        return;
    }
    if (*head == NULL){
        *head = new_item;
    } else{
        struct item *last_item = *head;
        while(last_item->next != NULL){
            last_item = last_item->next;
        }
        last_item->next = new_item;
    }
}
//read data into linked list
void data_initialize(struct item **head,const char *data_name){
    FILE *fp;
    char line[MAX_LINE_LEN];
    
    fp = fopen(data_name, "r");
    if(fp == NULL){
        printf("Fail to open %s", data_name);
        exit(1);
    }
    
    //read line
    *head = NULL;
    while (fgets(line, MAX_LINE_LEN, fp)){
        struct item *new_item = malloc(sizeof(struct item));
        if (new_item == NULL) printf("ERROR in LINE %d\n", __LINE__);
        char *token = strtok(line, ",");
        strcpy(new_item->name, token);
        token = strtok(NULL,",");
        new_item->price = atoi(token);
        token = strtok(NULL,",");
        new_item->calories = atoi(token);
        new_item->next = NULL;
        new_item->goods_num = 5;    // what is the initial value??
        new_item->oldest_goods = NULL;
        new_item->preserve_days = rand(); // what is the initial value??
        new_item->vec[0] = 0; //price
        new_item->vec[1] = 0; //calories
        token = strtok(NULL,",");
        if (token == NULL) new_item->vec[2] = 0.0;
        else new_item->vec[2] = (atoi(token))/100.0; //preference
        //if (new_item->vec[2] != 0) printf("%s %f\n", new_item->name, new_item->vec[2]);
        for (int i = 3; i < MAX_VECTOR_NUM;i++){
            token = strtok(NULL,",");
            if (token == NULL){
                new_item->vec[i] = 0;
            }else {
                new_item->vec[i] = atoi(token);  
            }
        }
        new_item->vec_norm = 0;
        //printf("%s,價格:NT$%d,熱量:%d大卡\n", new_item->name, new_item->price, new_item->calories);

        // Add to list
        add_to_item_list(head,new_item);
    }

    fclose(fp);
}
//print all details of the list
void print_item_list(struct item *head){
    struct item *current = head;
    int i = 1;
    while(current != NULL){
        printf("%2d. 名稱:%-s\t,價格:%d  熱量:%d  保存天數:%d  尚餘存貨:%d  喜愛程度:%.2f%%\n", 
                    i++, current->name, current->price, current->calories,
                    current->preserve_days, current->goods_num, current->vec[2]*100);
        current = current->next;
    }
}
//count the number of the list
int count_item_num(struct item *current){
    int num = 0;
    while(current != NULL){
        num++;
        current = current->next;
    }
    return num;
}
//free all the item in the list
void free_item_list(struct item *head){
    struct item *next;
    while(head != NULL){
        next = head->next;
        free(head);
        head = next;
    }
}
//reverse the string
void reverse(char *first, char *last){
    for (;first < last; first++, last--){
        char temp = *first;
        *first = *last;
        *last= temp;
    }
}
//change int to string
void itoa(int n, char *s){
    int sign = 1;
    if (n == 0) {
        s[0] = '0';
        s[1] = '\0';
        return;
    } 
    if (n < 0){
        sign = -1;
        n = -n;
    }
    int i = 0;
    while(n != 0){
        s[i++] = n % 10 + '0';
        n = n / 10;
    }

    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s,s+i-1);
}
//update data
void data_update(struct item *head ,const char *data_name){
    FILE *fp = fopen(data_name, "w");
    if (fp == NULL){
        printf("Fail to open file %s\n", data_name);
        exit(1);
    }
    struct item *p = head;
    while (p != NULL){
        fputs(p->name, fp);
        fputc(',', fp);
        char temp[50];
        itoa(p->price, temp);
        fputs(temp, fp);
        fputc(',', fp);
        itoa(p->calories,temp);
        fputs(temp, fp);
        fputc(',', fp);
        itoa((int)(p->vec[2] * 100),temp);
        fputs(temp, fp);
        for(int j = 3; j < MAX_VECTOR_NUM; j++){
            fputc(',', fp);
            itoa(((int)(p->vec[j])),temp);
            fputs(temp, fp);
        }
        fputc('\n', fp);
        p = p->next;
    }
    fclose(fp);
}
//download all the data in the catagory list
void data_download(struct item **catagory ,const char *data_name){
    FILE *fp = fopen(data_name, "w");
    if (fp == NULL){
        printf("Fail to open file %s\n", data_name);
        exit(1);
    }
    for (int i = 0; i < CATEGORY_NUM; i++){
        struct item *p = catagory[i];
        while (p != NULL){
            fputs(p->name, fp);
            fputc(',', fp);
            char temp[50];
            itoa(p->price, temp);
            fputs(temp, fp);
            fputc(',', fp);
            itoa(p->calories,temp);
            fputs(temp, fp);
            fputc(',', fp);
            itoa((int)(p->vec[2] * 100),temp);
            fputs(temp, fp);
            for(int j = 3; j < MAX_VECTOR_NUM; j++){
                fputc(',', fp);
                itoa(((int)(p->vec[j])),temp);
                fputs(temp, fp);
            }
            fputc('\n', fp);
            p = p->next;
        }
    }
    fclose(fp);
}
//delete item in all list by item address
void delete_item(struct item **head, struct item *target){
    if (target == NULL) return;
    struct item *current = *head;
    struct item *prior = NULL;
    while (current != NULL){
        if (current == target){
            if(prior == NULL){
                *head = current->next;
            }else {
                prior->next = current->next; 
            }
            free(current);
            break;
        }
        prior = current;
        current = current->next;
    }
}

/*
    searchin the list for the same value as target(need to change into string type) 
    option for the type you want to search
        1: name
        2: price
        3: calories
        4: preserve_days
        5: goods_num
*/
struct item **search_item_list(struct item *head, int option, char target[]){
    //initialize
    struct item *current = head;
    int target_num = atoi(target);
    float target_preferences = target_num/100.0;
    static struct item *target_item[MAX_TARGET_NUM];
    for (int i = 0; i < MAX_TARGET_NUM; i++) target_item[i] = NULL; 
    int num = 0;

    //search
    switch (option){
        case 1: 
            while(current != NULL){
                if (strcmp(target, current->name) == 0){ 
                    target_item[num++] = current;
                }
                current = current->next;
            }
            return target_item;
            break;
        case 2:
            while(current != NULL){
                if (target_num >= current->price) 
                    target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        case 3:
            while(current != NULL){
                if (target_num >= current->calories) 
                    target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        case 4:
            while(current != NULL){
                if (target_num <= current->preserve_days) 
                    target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        case 5:
            while(current != NULL){
                if (target_num == current->goods_num) 
                    target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        default:
            printf("There is no option%d for %s\n", option, __func__);
            break;
    }
        return NULL;
}
//caculate the vector norm(vector dimension = MAX_VECTOR_NUM)
double caculate_vec_norm(float vector[]){
    double ans = 0.0;
    // 計算向量的norm
    for (int i = 0; i < MAX_VECTOR_NUM; i++) {
        ans += vector[i] * vector[i];
    }
    ans = sqrt(ans);
    return ans;
}
//initialize all items' vector in a list 
void initialize_item_vector(struct item *head){
    //use max price and calories of the list 
    // to make the range between 0 and 1
    int max_price = search_max_price(head);
    int min_price = search_min_price(head);
    int min_calories = search_min_calories(head);
    int max_calories = search_max_calories(head);
    float price_range = max_price - min_price;
    float calories_range = max_calories - min_calories;
    struct item *current = head;
    if (price_range != 0 && calories_range != 0){
        while (current != NULL){
            current->vec[0] = (current->price - min_price) / price_range;
            current->vec[1] = (current->calories - min_calories) / calories_range;
            current->vec_norm = caculate_vec_norm(current->vec);
            current = current->next;
        }
    }
}
int search_max_price(struct item *head){
    int max_price = 0;
    while (head != NULL){
        if (head->price > max_price) 
            max_price = head->price;
        head = head->next;
    }
    return max_price;
}
int search_min_price(struct item *head){
    if (head == NULL) return 0;
    int min_price = head->price;
    while (head != NULL){
        if (head->price < min_price) 
            min_price = head->price;
        head = head->next;
    }
    return min_price;
}
int search_min_calories(struct item *head){
    if (head == NULL) return 0;
    int min_calories = head->calories;
    while (head != NULL){
        if (head->calories < min_calories) 
            min_calories = head->calories;
        head = head->next;
    }
    return min_calories;
}
int search_max_calories(struct item *head){
    int max_calories = 0;
    while (head != NULL){
        if (head->calories > max_calories) 
            max_calories = head->calories;
        head = head->next;
    }
    return max_calories;
}