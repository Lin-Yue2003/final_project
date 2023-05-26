#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include "database.h"
#include "pre_order_system.h"
#include "recommender.h"
int main(void){
    const char *category_file[CATEGORY_NUM] = {"boxed_meal.txt", "riceball.txt",
                                                "drink.txt", "bread.txt","noodles.txt",
                                                "salad.txt", "congee.txt","soup.txt",
                                                "dumpling.txt", "dessert.txt",
                                                "ice.txt", "candy.txt", "cookie.txt"};
    struct item *category[CATEGORY_NUM];
    //data_initialize(&category[0],category_file[2]);
    //free_item_list(category[0]);
    for (int i = 0 ; i < CATEGORY_NUM; i++){
        data_initialize(&category[i],category_file[i]);
    }
    initialize_label(category);
    for (int i = 0 ; i < CATEGORY_NUM; i++){
        initialize_item_vector(category[i]);
    }
    struct item *history_selected_item[MAX_HISTORY_SELECTED_ITEM];
    for (int i = 0; i < MAX_HISTORY_SELECTED_ITEM;i++){
        history_selected_item[i] = NULL;
    }
    pre_order_system(category,history_selected_item);
    for (int i = 1; i < CATEGORY_NUM-1; i++){
        for (int j = i+1; j < CATEGORY_NUM; j++);
            //printf("%-20s %-20s\t相似度 %f\n", category[i]->name,category[j]->name,cosine_similarity(category[i],category[j]));
    }
    for (int i = 0 ; i < CATEGORY_NUM; i++){
        //data_update(category[i],category_file[i]);
    }
    //data_download(category,"update_data.txt");
    for (int i = 0; i < CATEGORY_NUM; i++){
        free_item_list(category[i]);
    }
    for (int i = 0; i < MAX_HISTORY_SELECTED_ITEM; i++){
        if (history_selected_item[i] != NULL)
            free_item_list(history_selected_item[i]);
    }
    return 0;
}