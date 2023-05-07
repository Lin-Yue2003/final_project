#ifndef PRE_ORDER_SYSTEM_H
    #define PRE_ORDER_SYSTEM_H

    #define item_num_in_page 10
    #define MAX_PAGE_NUM 50
    void pre_order_system(struct item *category[]);
    void initialize_page_list(struct item *page_list[],struct item *item_head, int total_page);
    void initialize_page(struct item *cur_page[]);
    void read_page(struct item *cur_page[], struct item *current);
    void page_system(struct item *item_head, int *order_num, int *condition, int *option, struct item **selected_item);
    int print_page(struct item *cur_page[],int total_item, int current_page_num,int total_page, struct item *selected_item);
#endif