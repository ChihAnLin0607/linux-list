#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "common.h"

#define DICT_FILE "cities.txt"

static char *values[100000] = {0};

static void list_mergesort(struct list_head *head, unsigned int n);

int main(void)
{
    FILE *dict = fopen(DICT_FILE, "r");
    char city[64] = {0};

    if (!dict) {
        fprintf(stderr, "error: file open failed in '%s'.\n", DICT_FILE);
        fclose(dict);
    }

    struct list_head testlist;
    struct listitem *item = NULL, *is = NULL;
    unsigned int len, data_n, i;

    INIT_LIST_HEAD(&testlist);

    assert(list_empty(&testlist));

    data_n = 0;
    while (fscanf(dict, "%[^\n]\n", city) != EOF) {
        len = strlen(city);
        item = (struct listitem *) malloc(sizeof(*item));
        assert(item);
        item->city = (char *) malloc(len + 1);
        memcpy(item->city, city, len);
        item->city[len] = 0;
        list_add_tail(&item->list, &testlist);
        values[data_n] = item->city;
        data_n++;
    };

    assert(!list_empty(&testlist));
    list_mergesort(&testlist, data_n);

    qsort(values, data_n, sizeof(values[0]), cmpstr);

    i = 0;
    list_for_each_entry_safe (item, is, &testlist, list) {
        assert(item->city == values[i]);
        list_del(&item->list);
        free(item->city);
        free(item);
        i++;
    }

    assert(i == data_n);
    assert(list_empty(&testlist));
}

static void list_mergesort(struct list_head *head, unsigned int n)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    struct list_head llist, tmp_head;
    struct list_head *cut_node = head, *rlist;

    INIT_LIST_HEAD(&llist);

    int i;
    for (i = 0; i < n / 2; i++)
        cut_node = cut_node->next;

    list_cut_position(&llist, head, cut_node);
    rlist = head;  // rename the head

    list_mergesort(&llist, n / 2);
    list_mergesort(rlist, n - n / 2);

    INIT_LIST_HEAD(&tmp_head);

    struct listitem *item;

    i = 0;
    struct listitem *l_item1, *r_item1;
    while (i++ < n) {
        l_item1 = container_of(llist.next, struct listitem, list);
        r_item1 = container_of(rlist->next, struct listitem, list);

        if (cmpstr(&l_item1->city, &r_item1->city) < 0) {  // r_item1 bigger

            list_move_tail(llist.next, &tmp_head);
            if (list_empty(&llist)) {
                list_splice_tail(rlist, &tmp_head);
                break;
            }

        } else {
            list_move_tail(rlist->next, &tmp_head);
            if (list_empty(rlist)) {
                list_splice_tail(&llist, &tmp_head);
                break;
            }
        }
    }

    INIT_LIST_HEAD(head);
    list_splice_tail(&tmp_head, head);
    /*
    #ifdef DEBUG
        printf("head list:\n");
        list_for_each_entry (item, head, list) {
            printf("\t%s\n", item->city);
        }
        printf("\n\n");
    #endif
    */
    return;
}
