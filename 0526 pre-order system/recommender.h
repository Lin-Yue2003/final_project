#ifndef RECOMMENDER_H
    #define RECOMMENDER_H
    #define HISTORY_RATE 0.3
    struct recommended_item{
        struct item *item;
        float similarity;
        struct recommended_item *next;
    };
    int average_calories(struct item *selected_item, struct item **history_selected_item);
    int average_price(struct item *selected_item, struct item **history_selected_item);
    float average_vector(struct item *selected_item, struct item **history_selected_item,int index);
    void initialize_target(struct item *target,struct item *selected_item, struct item **history_selected_item);
    void initialize_recommended_list(struct recommended_item **head, struct item **category, struct item *target);
    struct recommended_item *partition(struct recommended_item *first, struct recommended_item *last);
    void quick_sort(struct recommended_item *first, struct recommended_item *last);
    struct recommended_item *last_item(struct recommended_item *head);
    void delete_recommended_item(struct recommended_item **head, struct recommended_item *target);
    void add_to_recommended_item_list(struct recommended_item **head, struct recommended_item *new_item);
    void free_recommended_item_list(struct recommended_item *head);
    void delete_label(struct item **catagory,int vec_index);
    void delete_label_to_all_items(struct item *head, int vec_index);
    void add_label_with_keywords(struct item **catagory,char *label, int vec_index);
    void add_label_to_all_items(struct item *head, int vec_index);
    double cosine_similarity(struct item *item1, struct item *item2);
    int count_recommended_item_num(struct recommended_item *current);
    void initialize_label(struct item **category);
#endif