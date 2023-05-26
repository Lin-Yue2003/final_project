#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "database.h"
#include "recommender.h"
#include <stdio.h>
#include <math.h>
//caculate the average calories of selected items and history orders
int average_calories(struct item *selected_item, struct item **history_selected_item){
    int num = 0, sum = 0, ans = 0;
    struct item *current = selected_item;
    for (int i = 0; i < MAX_HISTORY_SELECTED_ITEM && history_selected_item[i] != NULL; i++){
        current = history_selected_item[i]; 
        while(current != NULL){
            num++;
            sum += current->calories;
            current = current->next;
        }
    }
    if (num != 0) ans += (sum/num)*HISTORY_RATE;
    num = 0; sum = 0;
    while(current != NULL){
        num++;
        sum += current->calories;
        current = current->next;
    }
    if (num != 0) ans += (sum/num)*(1-HISTORY_RATE);
    
    return ans;
}
//caculate the average price of selected items and history orders
int average_price(struct item *selected_item, struct item **history_selected_item){
    int num = 0, sum = 0, ans = 0;
    struct item *current = selected_item;
    for (int i = 0; i < MAX_HISTORY_SELECTED_ITEM && history_selected_item[i] != NULL; i++){
        current = history_selected_item[i]; 
        while(current != NULL){
            num++;
            sum += current->price;
            current = current->next;
        }
    }
    if (num != 0) ans += (sum/num)*HISTORY_RATE;
    num = 0; sum = 0;
    while(current != NULL){
        num++;
        sum += current->price;
        current = current->next;
    }
    if (num != 0) ans += (sum/num)*(1-HISTORY_RATE);
    
    return ans;
}
//caculate the average value of each vector of selected items and history orders
float average_vector(struct item *selected_item, struct item **history_selected_item,int index){
    int num = 0;
    float sum = 0.0, ans = 0.0;
    struct item *current = selected_item;
    if (index < 0 || index >= MAX_VECTOR_NUM) return 0;
    for (int i = 0; i < MAX_HISTORY_SELECTED_ITEM && history_selected_item[i] != NULL; i++){
        current = history_selected_item[i]; 
        while(current != NULL){
            num++;
            sum += current->vec[index];
            current = current->next;
        }
    }
    if (num != 0) ans += (sum/num)*HISTORY_RATE;
    num = 0; sum = 0;
    while(current != NULL){
        num++;
        sum += current->vec[index];
        current = current->next;
    }
    if (num != 0) ans += (sum/num)*(1-HISTORY_RATE);
    
    return ans;
}
//initialize the each value of target which is used to evalute similar items 
void initialize_target(struct item *target,struct item *selected_item, struct item **history_selected_item){
    target->calories = average_calories(selected_item,history_selected_item);
    target->goods_num = 0;
    target->name[0] = '\0';
    target->price = average_price(selected_item,history_selected_item);
    for (int i = 0 ; i < MAX_VECTOR_NUM;i++){
        target->vec[i] = average_vector(selected_item,history_selected_item,i);
    }
    target->vec_norm = caculate_vec_norm(target->vec);
    target->goods_num = 0;
    target->next = NULL;
    target->oldest_goods = NULL;
    target->preserve_days = 0;
}
//establish the linked list of recommended item list and 
//use qsort to sort it in descending order by similarity 
void initialize_recommended_list(struct recommended_item **head, struct item **category, struct item *target){
    if ((*head) != NULL) free_recommended_item_list((*head));
    (*head) = NULL;
    for(int i = 0; i < CATEGORY_NUM; i++){
        struct item *current = category[i];
        while(current != NULL){
            struct recommended_item *new_item = malloc(sizeof(struct recommended_item));
            if(new_item == NULL){
                printf("Fail to malloc in %s\n", __func__);
                return;
            }
            new_item->item = current;
            new_item->similarity = cosine_similarity(target,new_item->item);
            new_item->next = NULL;
            add_to_recommended_item_list(head,new_item);
            current = current->next;
        }
    }
    quick_sort((*head), last_item((*head)));
}
// We are Setting the given last node position to its proper
// position
struct recommended_item *partition(struct recommended_item *first, struct recommended_item *last){
    // Get first node of given linked list
    struct recommended_item *pivot = first;
    struct recommended_item *front = first;
    struct item *temp_item = NULL;
    float temp_similarity = 0.0;
    while (front != NULL && front != last) {
        if (front->similarity > last->similarity) {
            pivot = first;
  
            // Swapping  node values
            temp_item = first->item;
            first->item = front->item;
            front->item = temp_item;

            temp_similarity = first->similarity;
            first->similarity = front->similarity;
            front->similarity = temp_similarity;
            
            // Visiting the next node
            first = first->next;
        }
  
        // Visiting the next node
        front = front->next;
    }
  
    // Change last node value to current node
    temp_item = first->item;
    first->item = last->item;
    last->item = temp_item;

    temp_similarity = first->similarity;
    first->similarity = last->similarity;
    last->similarity = temp_similarity;

    return pivot;
}
// Performing quick sort in  the given linked list
void quick_sort(struct recommended_item *first, struct recommended_item *last){
    if (first == last) {
        return;
    }
    struct recommended_item *pivot = partition(first, last);
  
    if (pivot != NULL && pivot->next != NULL) {
        quick_sort(pivot->next, last);
    }
  
    if (pivot != NULL && first != pivot) {
        quick_sort(first, pivot);
    }
}
//find the last recommended item in the list
struct recommended_item *last_item(struct recommended_item *head){
    struct recommended_item *temp = head;
    while (temp != NULL && temp->next != NULL) {
        temp = temp->next;
    }
    return temp;
}
//delete a recommended item in the list by its address
void delete_recommended_item(struct recommended_item **head, struct recommended_item *target){
    if (target == NULL) return;
    struct recommended_item *current = *head;
    struct recommended_item *prior = NULL;
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
//free all the recommended items in the list 
void free_recommended_item_list(struct recommended_item *head){
    struct recommended_item *next;
    while(head != NULL){
        next = head->next;
        free(head);
        head = next;
    }
}
//add a new item to the last of the list
void add_to_recommended_item_list(struct recommended_item **head, struct recommended_item *new_item){
    if (new_item == NULL){
        printf("Fail to %s\n", __func__);
        return;
    }
    if (*head == NULL){
        *head = new_item;
    } else{
        struct recommended_item *last_item = *head;
        while(last_item->next != NULL){
            last_item = last_item->next;
        }
        last_item->next = new_item;
    }
}
//delete a label of the list
void delete_label(struct item **catagory,int vec_index){
    for (int i = 0; i < CATEGORY_NUM; i++){
        struct item *current = catagory[i];
        while (current != NULL){
            current->vec[vec_index] = 0;
            current = current->next;
        }
    }
}
//use keywords to search and add label 
void add_label_with_keywords(struct item **catagory,char *label, int vec_index){
    for (int i = 0; i < CATEGORY_NUM; i++){
        struct item *current = catagory[i];
        while (current != NULL){
            if (strstr(current->name,label) != NULL) current->vec[vec_index] = 1;
            current = current->next;
        }
    }
}
//delete a label of all lists
void delete_label_to_all_items(struct item *head, int vec_index){
    struct item *current = head;
    while (current != NULL){
        current->vec[vec_index] = 0;
        current = current->next;
    }
}
//add label to all items in current list
void add_label_to_all_items(struct item *head, int vec_index){
    struct item *current = head;
    while (current != NULL){
        current->vec[vec_index] = 1;
        current = current->next;
    }
}
//caculate the cosine similarity of two items
double cosine_similarity(struct item *item1, struct item *item2) {
    double dot_product = 0.0;
    int i;
    if (item1 == NULL || item2 == NULL) return 0.0;
    //printf("%f %f\n", item1->vec_norm, item2->vec_norm);
    // 計算兩個向量的內積
    for (i = 0; i < MAX_VECTOR_NUM; i++) {
        //printf("label %d: %f %f\n", i,item1->vec[i], item2->vec[i]);
        dot_product += item1->vec[i] * item2->vec[i];
    }

    // 計算餘弦相似度
    if (item1->vec_norm != 0 && item2->vec_norm != 0) {
        return dot_product / (item1->vec_norm * item2->vec_norm);
    } else {
        return 0.0;
    }
}
//count how many items in the recommended list
int count_recommended_item_num(struct recommended_item *current){
    int num = 0;
    while(current != NULL){
        num++;
        current = current->next;
    }
    return num;
}
//add corresponding labels to all items
void initialize_label(struct item **category){
    for (int i = 3; i < MAX_VECTOR_NUM;i++) 
        delete_label(category,i);
    int n = 3;//早餐
    add_label_to_all_items(category[RICEBALL],n);
    add_label_to_all_items(category[BREAD],n);
    add_label_to_all_items(category[SALAD],n);
    add_label_to_all_items(category[DRINK],n);
    n = 4;//午餐
    add_label_to_all_items(category[BOXED_MEAL],n);
    add_label_to_all_items(category[RICEBALL],n);
    add_label_to_all_items(category[NOODLES],n);
    add_label_to_all_items(category[CONGEE],n);
    add_label_to_all_items(category[SALAD],n);
    add_label_to_all_items(category[DUMPLING],n);
    add_label_to_all_items(category[SOUP],n);
    add_label_to_all_items(category[DRINK],n);
    n = 5;//點心
    add_label_to_all_items(category[RICEBALL],n);
    add_label_to_all_items(category[BREAD],n);
    add_label_to_all_items(category[SALAD],n);
    add_label_to_all_items(category[DESSERT],n);
    add_label_to_all_items(category[ICE],n);
    add_label_to_all_items(category[CANDY],n);
    add_label_to_all_items(category[COOKIE],n);
    add_label_to_all_items(category[DRINK],n);
    n = 6;//晚餐
    add_label_to_all_items(category[BOXED_MEAL],n);
    add_label_to_all_items(category[RICEBALL],n);
    add_label_to_all_items(category[NOODLES],n);
    add_label_to_all_items(category[CONGEE],n);
    add_label_to_all_items(category[SALAD],n);
    add_label_to_all_items(category[DUMPLING],n);
    add_label_to_all_items(category[SOUP],n);
    add_label_to_all_items(category[DRINK],n);
    n = 7;//甜
    add_label_to_all_items(category[BREAD],n);
    add_label_to_all_items(category[DESSERT],n);
    add_label_to_all_items(category[ICE],n);
    add_label_to_all_items(category[CANDY],n);
    add_label_to_all_items(category[DRINK],n);
    add_label_with_keywords(category,"甜",n);
    n = 8;//鹹
    add_label_to_all_items(category[BOXED_MEAL],n);
    add_label_to_all_items(category[RICEBALL],n);
    add_label_to_all_items(category[NOODLES],n);
    add_label_to_all_items(category[CONGEE],n);
    add_label_to_all_items(category[DUMPLING],n);
    add_label_to_all_items(category[SOUP],n);
    add_label_with_keywords(category,"鹹",n);
    add_label_with_keywords(category,"肉",n);
    add_label_with_keywords(category,"咖哩",n);
    n = 9;//辣
    add_label_with_keywords(category,"辣",n);
    add_label_with_keywords(category,"泡菜",n);
    add_label_with_keywords(category,"麻",n);
    add_label_with_keywords(category,"紅油",n);
    add_label_with_keywords(category,"紅燒",n);
    n = 10;//熟食
    add_label_to_all_items(category[BOXED_MEAL],n);
    add_label_to_all_items(category[RICEBALL],n);
    add_label_to_all_items(category[NOODLES],n);
    add_label_to_all_items(category[CONGEE],n);
    add_label_to_all_items(category[DUMPLING],n);
    add_label_to_all_items(category[SOUP],n);
    n = 11;//冰、涼
    add_label_to_all_items(category[SALAD],n);
    add_label_to_all_items(category[ICE],n);
    add_label_to_all_items(category[DRINK],n);
    add_label_with_keywords(category,"涼",n);
    add_label_with_keywords(category,"冰",n);
    n = 12;//炸
    add_label_with_keywords(category, "炸",n);
    n = 13;//雞
    add_label_with_keywords(category, "雞",n);
    n = 14;//豬
    add_label_with_keywords(category, "豬",n);
    n = 15;//牛
    add_label_with_keywords(category, "牛",n);
    n = 16;//海鮮
    add_label_with_keywords(category, "海鮮",n);
    add_label_with_keywords(category, "明太子",n); 
    add_label_with_keywords(category, "蝦",n);  
    add_label_with_keywords(category, "魚",n); 
    add_label_with_keywords(category, "蟹",n);    
    n = 17;//台式
    add_label_with_keywords(category, "台式",n);
    add_label_with_keywords(category, "古早味",n);
    add_label_with_keywords(category, "肉燥",n);       
    n = 18;//法式
    add_label_with_keywords(category, "法式",n);
    add_label_with_keywords(category, "法國",n);       
    n = 19;//韓式
    add_label_with_keywords(category, "韓式",n);  
    add_label_with_keywords(category, "韓國",n);  
    add_label_with_keywords(category, "泡菜",n);   
    n = 20;//日式
    add_label_with_keywords(category, "日式",n);   
    add_label_with_keywords(category, "日本",n);
    add_label_with_keywords(category, "抹茶",n);
    n = 21;//美式
    add_label_with_keywords(category, "美式",n);   
    add_label_with_keywords(category, "美國",n);        
    n = 22;//汽水
    add_label_with_keywords(category, "汽水",n);
    add_label_with_keywords(category, "可樂",n);  
    add_label_with_keywords(category, "雪碧",n); 
    add_label_with_keywords(category, "芬達",n);   
    add_label_with_keywords(category, "氣泡",n);     
    n = 23;//茶
    add_label_with_keywords(category, "茶",n);   
    n = 24;//豆漿
    add_label_with_keywords(category, "豆漿",n);   
    n = 25;//蔬果（水果）
    add_label_with_keywords(category, "蔬果",n);  
    add_label_with_keywords(category, "蔬菜",n); 
    add_label_with_keywords(category, "水果",n); 
    add_label_with_keywords(category, "果菜",n); 
    add_label_with_keywords(category, "果汁",n);      
    n = 26;//乳製品
    add_label_with_keywords(category, "乳",n);
    add_label_with_keywords(category, "奶",n); 
    add_label_with_keywords(category, "拿鐵",n);      
    n = 27;//咖啡
    add_label_with_keywords(category, "咖啡",n);
    add_label_with_keywords(category, "拿鐵",n);
    add_label_with_keywords(category,"卡布奇諾",n);
    add_label_with_keywords(category,"焦糖瑪琪朵",n);  
    n = 28;//布丁
    add_label_with_keywords(category, "布丁",n);    
    n = 29;//蛋糕
    add_label_with_keywords(category, "蛋糕",n);   
    n = 30;//奶酪
    add_label_with_keywords(category, "奶酪",n);   
    n = 31;//冰淇淋
    add_label_with_keywords(category, "冰淇淋",n);   
    n = 32;//雪糕
    add_label_with_keywords(category, "雪糕",n);   
    n = 33;//巧克力
    add_label_with_keywords(category, "巧克力",n);
    add_label_with_keywords(category, "可可",n);   
    n = 34;//洋芋片
    add_label_with_keywords(category, "洋芋片",n);
    n = 35;//糖
    add_label_with_keywords(category, "糖",n);    
}
