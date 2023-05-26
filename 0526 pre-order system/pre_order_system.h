#ifndef PRE_ORDER_SYSTEM_H
    #define PRE_ORDER_SYSTEM_H

    #define item_num_in_page 10
    #define MAX_PAGE_NUM 100
    void pre_order_system(struct item *category[], struct item **history_selected_item);
    void initialize_page_list(struct item *page_list[],struct item *item_head, int total_page);
    void initialize_recommended_page_list(struct recommended_item *page_list[],struct recommended_item *item_head, int total_page);
    void initialize_page(struct item *cur_page[]);
    void read_page(struct item *cur_page[], struct item *current);
    void read_recommender_page(struct item *cur_page[], struct recommended_item *current);
    void selected_item_system(struct item **selected_item, int *condition, int *option, int *order_num);
    void page_system(struct item *item_head, int *order_num, int *condition, int *option, struct item **selected_item);
    void recommender_system(struct item **category,struct item **history_selected_item, struct item **selected_item, int *condition, int *option, int *order_num);
    int print_page(struct item *cur_page[],int total_item, int current_page_num,int total_page, struct item *selected_item);
    int print_selected_items_page(struct item *cur_page[],int total_item, int current_page_num,int total_page, struct item *selected_item);
#endif