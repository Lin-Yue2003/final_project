#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "database.h"
#include "pre_order_system.h"

int main(void){
    struct item *catagory[CATEGORY_NUM];
    data_initialize(&catagory[0], "food.txt");
    data_initialize(&catagory[1], "riceball.txt");
    //print_item_list(catagory[0]);
    //struct item **target = search_item_list(catagory[1],2,"70");
    //printf("TARGET PRICE UNDER 70\n");
    //print_item_list(*target);
    pre_order_system(catagory);
    //data_download(catagory,"update_data.txt");
    for (int i = 0; i < CATEGORY_NUM; i++){
        free_item_list(catagory[i]);
    }
    return 0;
}