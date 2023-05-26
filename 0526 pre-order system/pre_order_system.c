#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include "database.h"
#include "recommender.h"
#include "pre_order_system.h"
#define EXIT_SYSTEM 0
#define EXIT_CURRENT_LIST 1
#define CONTINUE 2
#define FINISHED_ORDER 3
#define SEPERATE() printf("  ----------------------------------------- \n")
//the main system of pre_order
void pre_order_system(struct item *category[CATEGORY_NUM], struct item **history_selected_item){
    int option, type = 0, order_num = 0;
    struct item *selected_item = NULL;
    struct recommender_item *recommender_item = NULL;
    int condition = CONTINUE;
    SEPERATE();
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
        SEPERATE();
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
            selected_item_system(&selected_item,&condition,&option,&order_num);
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
                printf("請輸入選項： ");
                scanf("%d", &option);
                SEPERATE();
                if (option == 1) condition = CONTINUE;
                if (option == 2) condition = EXIT_SYSTEM;
                continue;
            }
        }
        if (condition == FINISHED_ORDER){

            while(1){
                printf("即將前往結帳\n");
                printf("1. 確認 2. 取消\n");
                printf("請輸入選項： ");
                scanf("%d", &option);
                if (option >= 1 && option <= 2){ 
                    break;
                } else{
                    printf("沒有這個選項，請重新輸入\n");
                }   
            }
            if (option == 2){
                condition = CONTINUE;
                continue;    
            }       
            if (selected_item == NULL){
                printf("您尚未選擇任何商品\n");
                SEPERATE();
            } else{
                //finish select item
                //suggest item
                //recommender
                condition = CONTINUE;
                recommender_system(category,history_selected_item,&selected_item,&condition,&option,&order_num);
                //finished pre-order    
                //add to history pre-order list
                if (condition != FINISHED_ORDER) continue; 
                for (int i = 0;i < MAX_HISTORY_SELECTED_ITEM;i++){
                    if (history_selected_item[i] == NULL){
                        history_selected_item[i] = selected_item;
                        break;
                    }
                }
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
//the main system of recommender
void recommender_system(struct item **category,struct item **history_selected_item, struct item **selected_item, int *condition, int *option, int *order_num){
    //initialize
    struct item *cur_page[item_num_in_page];
    struct recommended_item *recommended_item = NULL;
    struct item target;
    initialize_target(&target,*selected_item,history_selected_item);
    initialize_recommended_list(&recommended_item,category,&target);
    struct recommended_item *temp = recommended_item;
    while(temp != NULL){
        printf("%s 相似度 %f\n", temp->item->name,temp->similarity);
        temp = temp->next;
    }  
    struct recommended_item *cur_list = recommended_item;
    int total_item_num = count_recommended_item_num(recommended_item);
    int total_num_page = total_item_num / item_num_in_page + ((total_item_num % item_num_in_page == 0)? 0:1);
    struct recommended_item *page_list[MAX_PAGE_NUM];
    initialize_recommended_page_list(page_list, recommended_item, total_num_page);
    int current_page_num = 1;
    int current_item_num;    
    while (1){
        //determine the condition for exit system 
        if ((*condition) == EXIT_SYSTEM || 
            (*condition) == FINISHED_ORDER || 
            (*condition) == EXIT_CURRENT_LIST) {
            free_recommended_item_list(recommended_item);
            break;
        }
        //setup the current page
        initialize_page(cur_page);
        read_recommender_page(cur_page, cur_list);
        while(1){
            //print the current page and store the numbers of items in this page
            printf("選擇以加購以下項目\n");
            current_item_num = print_page(cur_page,total_item_num, 
                                            current_page_num,total_num_page,*selected_item);
            //scan the option from user
            scanf("%d", option);
            SEPERATE();
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
            SEPERATE();
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
                SEPERATE();
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
                SEPERATE();
            }else
                printf("ERROR in %s line %d\n", __func__, __LINE__);                        
        }
    }
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
            SEPERATE();
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
            SEPERATE();
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
                SEPERATE();
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
                SEPERATE();
            }else
                printf("ERROR in %s line %d\n", __func__, __LINE__);                        
        }
    }    
}
//the main system to check current selected item list
void selected_item_system(struct item **selected_item, int *condition, int *option, int *order_num){
    //initialize
    struct item *cur_page[item_num_in_page];
    struct item *cur_list = *selected_item;  
    int total_num_page = (*order_num) / item_num_in_page + (((*order_num) % item_num_in_page == 0)? 0:1);
    struct item *page_list[MAX_PAGE_NUM];
    initialize_page_list(page_list, (*selected_item), total_num_page);
    int current_page_num = 1;
    int current_item_num;    
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
            current_item_num = print_selected_items_page(cur_page,(*order_num), 
                                            current_page_num,total_num_page,*selected_item);
            //scan the option from user
            scanf("%d", option);
            SEPERATE();
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
        struct item *item = cur_page[(*option)-1];
        if (item == NULL){
            printf("ERROR in %s line %d\n", __func__, __LINE__); 
            continue;
        }
        while(1){
            printf("確定要刪除『%s』嗎?\n", item->name);
            printf("1. 是\n2. 否\n");
            printf("請輸入選項： ");
            scanf("%d", option);
            if ((*option) >= 1 && (*option) <= 2){ 
                break;
            } else{
                printf("沒有這個選項，請重新輸入\n");
            }   
        }
        if ((*option) == 2)
            continue;
        if ((*option) == 1){
            delete_item(selected_item,item);
            (*order_num)--;
            initialize_page_list(page_list, (*selected_item), total_num_page);
            cur_list = *selected_item;  
            total_num_page = (*order_num) / item_num_in_page + (((*order_num) % item_num_in_page == 0)? 0:1);
            initialize_page_list(page_list, (*selected_item), total_num_page);
            current_page_num = 1;
            continue;
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
//search and store the first recommended item's address of each page
void initialize_recommended_page_list(struct recommended_item *page_list[],struct recommended_item *item_head, int total_page){
    struct recommended_item *current = item_head;
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
//make every elements in current page point to NULL
void initialize_page(struct item *cur_page[]){
    for (int i = 0; i < item_num_in_page; i++)
        cur_page[i] = NULL;
}
//make every elements in current page point to corresponding items
// by using the first item of this page   
void read_page(struct item *cur_page[], struct item *current){
    int i = 0;
    while (current != NULL && i < item_num_in_page){
        cur_page[i] = current;
        current = current->next;
        i++;
    }
}
//make every elements in current page point to corresponding items
// by using the first recommended item of this page  
void read_recommender_page(struct item *cur_page[], struct recommended_item *current){
    int i = 0;
    while (current != NULL && i < item_num_in_page){
        cur_page[i] = current->item;
        current = current->next;
        i++;
    }
}
//print all items in current list and options including
//  select item and finish order if order is not empty  
int print_page(struct item *cur_page[],int total_item, 
                int current_page_num,int total_page, struct item *selected_item){
    int i;
    printf(" 0. 回到項目列表 \n");
    for (i = 0; i < item_num_in_page; i++){
        if (cur_page[i] != NULL)
            printf("%2d. 名稱:%-21s\t,價格:%d 元 熱量:%d 大卡 存貨數量:%d\n", 
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
//print items which have been selected and options including
//  delete item, finish order, change page
int print_selected_items_page(struct item *cur_page[],int total_item, 
                int current_page_num,int total_page, struct item *selected_item){
    int i;
    printf(" 0. 回到項目列表 \n");
    for (i = 0; i < item_num_in_page; i++){
        if (cur_page[i] != NULL)
            printf("%2d. 名稱:%-21s\t,價格:%d 元 熱量:%d 大卡 存貨數量:%d\n", 
                    (i+1), cur_page[i]->name, cur_page[i]->price,
                    cur_page[i]->calories, cur_page[i]->goods_num);
        else 
            break;
    }
    printf("            (編號). 刪除編號項目\n ");
    printf("    %2d. 上一頁     %2d. 下一頁", (i+1), (i+2));
    if(selected_item != NULL) printf("    %2d. 完成訂單", (i+3));
    printf("\n\t總共有 %d 個項目   第%d頁 / 共%d頁\n", total_item,current_page_num,total_page);
    
    printf("請輸入編號: ");
    return (i);
}

