#ifndef DATABASE_H
    #define DATABASE_H
    #include <stdbool.h>
    #define MAX_LINE_LEN 300
    #define MAX_NAME_LEN 100
    #define MAX_TARGET_NUM 150
    #define CATEGORY_NUM 13
    #define MAX_VECTOR_NUM 38
    #define MAX_HISTORY_SELECTED_ITEM 100
    enum CATAGORY_NAME{BOXED_MEAL,RICEBALL,DRINK,
                        BREAD,NOODLES,SALAD,CONGEE,SOUP,
                        DUMPLING,DESSERT,ICE,CANDY,COOKIE};
    struct goods{
        struct goods *next;
    };
    struct item{
        char name[MAX_NAME_LEN+1];
        unsigned int price;
        unsigned int calories;
        unsigned int preserve_days;
        unsigned int goods_num;
        float vec[MAX_VECTOR_NUM]; //0 price 1 calories 2 preference 3~ label(0 or 1)
        double vec_norm;
        struct goods *oldest_goods;
        struct item *next;
    };
    void print_category_table(void);
    void add_to_item_list(struct item **head, struct item *new_item);
    void data_initialize(struct item **head,const char *data_name);
    double caculate_vec_norm(float vector[]);
    void initialize_item_vector(struct item *head);
    int search_max_price(struct item *head);
    int search_min_price(struct item *head);
    int search_min_calories(struct item *head);
    int search_max_calories(struct item *head);
    void print_item_list(struct item *head);
    void free_item_list(struct item *head);
    struct item **search_item_list(struct item *head, int option, char target[]);
    int count_item_num(struct item *current);
    void reverse(char *first, char *last);
    void itoa(int n, char *s);
    void data_download(struct item **catagory ,const char *data_name);
    void data_update(struct item *head ,const char *data_name);
    void delete_item(struct item **head, struct item *target);
#endif
