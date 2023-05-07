#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "database.h"
#define MAX_LINE_LEN 110
#define MAX_NAME_LEN 90
#define MAX_TARGET_NUM 150
#define MAX_LABEL_NUM 5
#define CATEGORY_NUM 2

const char *category_table[CATEGORY_NUM] = {"便當","飯糰"};
//const char *category_table[CATEGORY_NUM] = {"便當", "飯糰", "飲料", "麵包", "麵", "沙拉", "粥",
                                            //"湯", "水餃、鍋貼", "甜品", "冰品", "糖果", "餅乾" };
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
void data_initialize(struct item **head,char *data_name){
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
        char *token = strtok(line, ",");
        strcpy(new_item->name, token);
        token = strtok(NULL,",");
        new_item->price = atoi(token);
        token = strtok(NULL,",");
        new_item->calories = atoi(token);
        new_item->next = NULL;
        new_item->goods_num = 5;    // what is the initial value??
        new_item->oldest_goods = NULL;
        new_item->preferences = 0.5;
        new_item->preserve_days = 20; // what is the initial value??
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
        printf("%2d. 名稱:%-s\t,價格:%d  熱量:%d  保存天數:%d  尚餘存貨:%d  喜愛程度:%.1f%%\n", 
                    i++, current->name, current->price, current->calories,
                    current->preserve_days, current->goods_num, current->preferences*100);
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
        char tamp = *first;
        *first = *last;
        *last= tamp;
    }
}
//change int to string
void itoa(int n, char *s){
    int sign = 1;
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
//download all the data in the catagory list
void data_download(struct item **catagory , char *data_name){
    FILE *fp = fopen(data_name, "a");
    if (fp == NULL){
        printf("Fail to open file %s\n", data_name);
        exit(1);
    }
    for (int i = 0; i < CATEGORY_NUM; i++){
        struct item *p = catagory[i];
        while (p != NULL){
            fputs(p->name, fp);
            fputc(',', fp);
            char tamp[50];
            itoa(p->price, tamp);
            fputs(tamp, fp);
            fputc(',', fp);
            itoa(p->calories,tamp);
            fputs(tamp, fp);
            fputc(',', fp);
            itoa(p->preserve_days,tamp);
            fputs(tamp, fp);
            fputc(',', fp);
            itoa((int)(p->preferences * 100),tamp);
            fputs(tamp, fp);
            fputc('\n', fp);
            p = p->next;
        }
        fputc('@', fp);
    }
    fclose(fp);
}

/*
    searchin the list for the same value as target(need to change into string type) 
    option for the type you want to search
        1: name
        2: price
        3: calories
        4: preferences([LOW] 0 ~ 100 [HIGH])
        5: preserve_days
        6: goods_num
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
                if (target_num >= current->price) target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        case 3:
            while(current != NULL){
                if (target_num >= current->calories) target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        case 4:
            while(current != NULL){
                if (target_preferences <= current->preferences) target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        case 5:
            while(current != NULL){
                if (target_num <= current->preserve_days) target_item[num++] = current; 
                current = current->next;
            }
            return target_item;
            break;
        case 6:
            while(current != NULL){
                if (target_num == current->goods_num) target_item[num++] = current; 
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
