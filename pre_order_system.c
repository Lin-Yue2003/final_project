#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "database.h"
#include "pre_order_system.h"
#define item_num_in_page 10
#define MAX_PAGE_NUM 50
#define EXIT_SYSTEM 0
#define EXIT_CURRENT_LIST 1
#define CONTINUE 2
#define FINISHED_ORDER 3
//the main system of pr_order
void pre_order_system(struct item *category[CATEGORY_NUM]){
    int option, type = 0, order_num = 0;
    struct item *selected_item = NULL;
    int condition = CONTINUE;
    printf("歡迎來到預購系統\n");
    while(1){
        if (condition == EXIT_SYSTEM) break;
        printf(" 0. 離開預購系統\n");
        print_category_table();
        if (order_num > 0) {
            printf("%2d. 查看已預購商品\n", CATEGORY_NUM+1);
            printf("%2d. 完成預購\n", CATEGORY_NUM+2);
        }
        printf("請輸入選項： ");
        scanf("%d", &option);
        if (option >= 0 && option <= CATEGORY_NUM+2){ 
            condition = CONTINUE;
            type = option-1;
        } else{
            printf("沒有這個選項，請重新輸入\n");
            continue;
        }

        if (option == 0 ||option == (CATEGORY_NUM+2)){
            // EXIT_SYSTEM
            type = 0;
            condition = EXIT_SYSTEM;
        } 
        if (option == (CATEGORY_NUM+2)){
            // FINISHED_ORDER
            type = 0;
            condition = FINISHED_ORDER;
        } 
        if (option == CATEGORY_NUM+1){
            //unfinished part
            //function of current selected items(print, delete)
            struct item *temp = selected_item;
            while(temp != NULL){
                printf("名稱:%-s\t,價格:%d  熱量:%d\n", temp->name,temp->price,temp->calories);
                temp = temp->next;
            }
            continue;
        }
        if (condition == CONTINUE)
            page_system(category[type],&order_num, &condition,&option,&selected_item);

        if(condition == EXIT_CURRENT_LIST){
            condition = CONTINUE;
            continue;
        }
        if(condition == EXIT_SYSTEM){
            if (selected_item != NULL){
                printf("您尚未完成預購，離開後本次已選購項目將會被消除\n");
                printf("是否離開預購系統？\n");
                printf("1.返回購物系統\n2.離開\n");
                scanf("%d", &option);
                if (option == 1) condition = CONTINUE;
                if (option == 2) condition = EXIT_SYSTEM;
                continue;
            }
        }
        if (condition == FINISHED_ORDER){
            if (selected_item == NULL){
                printf("您尚未選擇任何商品\n");
            } else{
                //  unfinished part

                //finish select item
                //suggest item
                printf("是否加購以下項目？\n");
                //struct item *recommed_item_list[RECOMMED_ITEM_NUM] = NULL;
                //recommender

                //finished pre-order    
                //add to history pre-order list
                free_item_list(selected_item);
                selected_item = NULL;
                order_num = 0;
                printf("  ------------------------------------ \n");
                printf("| 已完成預購，請在預購時間準時拿取商品 |\n"); 
                printf("  ------------------------------------ \n");           
            }
        }
    }
    if (selected_item != NULL){
        free_item_list(selected_item);
        selected_item = NULL;
    }
    //end pre_order_system
}
//the main system of the pages for pre-order system
void page_system(struct item *item_head, int *order_num,
                 int *condition, int *option, struct item **selected_item){
    //initialize
    struct item *cur_page[item_num_in_page];
    struct item *cur_list = item_head;  
    int item_num = count_item_num(item_head);
    int total_num_page = item_num / item_num_in_page + ((item_num % item_num_in_page == 0)? 0:1);
    struct item *page_list[MAX_PAGE_NUM];
    initialize_page_list(page_list, item_head, total_num_page);
    int current_page_num = 1;
    int current_item_num;
    
    //print page
    while (1){
        //determine the condition for exit system 
        if ((*condition) == EXIT_SYSTEM || 
            (*condition) == FINISHED_ORDER || 
            (*condition) == EXIT_CURRENT_LIST) break;
        //setup the current page
        initialize_page(cur_page);
        read_page(cur_page, cur_list);
        while(1){
            //print the current page and store the numbers of items in this page
            current_item_num = print_page(cur_page,item_num, 
                                            current_page_num,total_num_page,*selected_item);
            //scan the option from user
            scanf("%d", option);
            //determine whether option is in the range
            int min = 0, max = current_item_num + (((*selected_item) == NULL)? 2:3);
            if ((*option) >= min && (*option) <= max){ 
                break;
            } else{
                printf("沒有這個選項，請重新輸入\n");
            }               
        }

        // Back to catagory list
        if((*option) == 0){ 
            (*condition) = EXIT_CURRENT_LIST;
            break;
        }
        //prior page
        if((*option) == current_item_num+1){ 
            if(current_page_num == 1){ 
                // first page
            }else {
                cur_list = page_list[current_page_num-1];
                current_page_num--;
            }
            continue; 
        }
        //next page
        if((*option) == current_item_num+2){ 
            if(current_page_num == total_num_page){ 
                // last page
            }else {
                cur_list = page_list[current_page_num+1];
                current_page_num++;
            }
            continue;
        }
        if((*option) == current_item_num+3){ 
            (*condition) = FINISHED_ORDER;
            continue;
        }
        //select item
        struct item *item = NULL;
        item = cur_page[(*option)-1];
        if(item == NULL){
            printf("ERROR in %s line %d\n", __func__, __LINE__); 
            continue;
        }
        // successfully selected item
        if(item->goods_num == 0){
            //no goods 
            printf("沒貨了，需要到貨時提醒嗎？\n");
            continue;
        }

        //Add new selected item into selected item list
        struct item *new_item = malloc(sizeof(struct item));
        if (new_item == NULL){
            //Fail to malloc
            printf("Fail to malloc in %s at %d\n", __func__, __LINE__);
            continue;
        }
        //copy item
        *new_item = *item; 
        new_item->next = NULL;
        if ((*selected_item) == NULL){
            // first item in the selected item list
            (*selected_item) = new_item;
            if ((*selected_item) != NULL){
                // successfully add to list
                (*order_num)++;
                printf("『%s』已成功加入選購清單\n", (*selected_item)->name);
            } else printf("ERROR in %s line %d\n", __func__, __LINE__);

        } else{
            // add new item to be the last one of the list
            struct item *last_selected_item = (*selected_item);
            while (last_selected_item->next != NULL){
                last_selected_item = last_selected_item->next;
            }
            last_selected_item->next = new_item;
            if (last_selected_item->next != NULL){
                // successfully add to list
                (*order_num)++;
                printf("『%s』已成功加入選購清單\n", last_selected_item->next->name);
            }else
                printf("ERROR in %s line %d\n", __func__, __LINE__);                        
        }
    }    
}

//search and store the first item's address of each page
void initialize_page_list(struct item *page_list[],struct item *item_head, int total_page){
    struct item *current = item_head;
    page_list[0] = item_head;
    for (int i = 1; i < MAX_PAGE_NUM; i++){
        page_list[i] = NULL;
    }
    if (total_page >= MAX_PAGE_NUM) total_page%=MAX_PAGE_NUM;
    for (int i = 1; i <= total_page; i++){
        page_list[i] = current;
        for (int j = 0; j < 10; j++){
            if (current == NULL) return;
            current = current->next;
        }
    }
}

void initialize_page(struct item *cur_page[]){
    for (int i = 0; i < item_num_in_page; i++)
        cur_page[i] = NULL;
}

void read_page(struct item *cur_page[], struct item *current){
    int i = 0;
    while (current != NULL && i < item_num_in_page){
        cur_page[i] = current;
        current = current->next;
        i++;
    }
}

int print_page(struct item *cur_page[],int total_item, 
                int current_page_num,int total_page, struct item *selected_item){
    int i;
    printf(" 0. 回到項目列表 \n");
    for (i = 0; i < item_num_in_page; i++){
        if (cur_page[i] != NULL)
            printf("%2d. 名稱:%-s\t,價格:%d  熱量:%d 存貨數量:%d\n", 
                    (i+1), cur_page[i]->name, cur_page[i]->price,
                    cur_page[i]->calories, cur_page[i]->goods_num);
        else 
            break;
    }
    printf("    %2d. 上一頁     %2d. 下一頁", (i+1), (i+2));
    if(selected_item != NULL) printf("    %2d. 完成訂單", (i+3));
    printf("\n\t總共有 %d 個項目   第%d頁 / 共%d頁\n", total_item,current_page_num,total_page);
    
    printf("請輸入編號: ");
    return (i);
}

