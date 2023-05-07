#ifndef DATABASE_H
    #define DATABASE_H
    #include <stdbool.h>
    #define MAX_LINE_LEN 110
    #define MAX_NAME_LEN 90
    #define MAX_TARGET_NUM 150
    #define CATEGORY_NUM 2
    #define MAX_LABEL_NUM 5
    struct goods{
        struct goods *next;
    };
    struct item{
        char name[MAX_NAME_LEN+1];
        unsigned int price;
        unsigned int calories;
        float preferences; //how much the consumer likes this item(value [LOW]0 ~ 1[HIGH])
        bool label[MAX_LABEL_NUM];
        unsigned int preserve_days;
        unsigned int goods_num;
        struct goods *oldest_goods;
        struct item *next;
    };
    void print_category_table(void);
    void add_to_item_list(struct item **head, struct item *new_item);
    void data_initialize(struct item **head,char *data_name);
    void print_item_list(struct item *head);
    void free_item_list(struct item *head);
    struct item **search_item_list(struct item *head, int option, char target[]);
    int count_item_num(struct item *current);
    void reverse(char *first, char *last);
    void itoa(int n, char *s);
    void data_download(struct item **catagory , char *data_name);
#endif
