#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "list.h"
#include "bitmap.h"


char list_name[10][50];
char hash_name[10][50];
char bitmap_name[10][50];

struct list lists[10];
struct hash hashes[10];
struct bitmap* bitmaps[10];

void init_data_structure();

// Find idx for the structure name
int find_name_idx(char input[50], int select);

/* list cmp func */
bool list_func(const struct list_elem *a, const struct list_elem *b, void* aux);

/* hash hash, cmp func */
unsigned my_hash_hash(const struct hash_elem *e, void *aux);
bool my_hash_less(const struct hash_elem *a, const struct hash_elem *b, void *aux);

/* hash_action_func */
void my_hash_delete_func(struct hash_elem *e, void *aux);
void my_hash_square_func(struct hash_elem *e, void *aux);
void my_hash_triple_func(struct hash_elem *e, void *aux);

int main()
{
    char buf[1000];
    char arg[6][100];
    char command[100];
    int quit = 0;
    int arg_num;
    /////
    init_data_structure();
    /////
    while(!quit)
    {
        int i = 0;
        for(i=0; i<6; i++)
            arg[i][0] = '\0';
        fgets(buf, sizeof(buf), stdin);
        arg_num = sscanf(buf, "%s%s%s%s%s%s%s", command, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
        if( strcmp(command, "quit") == 0)
        {
            quit = 1;
        }
        else if( strcmp(command, "create") == 0)
        {
            int suc = 0;
            if(strcmp(arg[0], "list") == 0)
            {
                for(i=0; i<10; i++)
                {
                    if( strcmp(list_name[i], "") == 0)
                    {
                        suc = 1;
                        strcpy(list_name[i], arg[1]);
                        list_init(&lists[i]);
                        break;
                    }
                }
            }
            else if(strcmp(arg[0], "hashtable") == 0)
            {
                for(i=0; i<10; i++)
                {
                    if( strcmp(hash_name[i], "") == 0)
                    {
                        suc = 1;
                        strcpy(hash_name[i], arg[1]);
                        hash_init(&hashes[i], my_hash_hash, my_hash_less, NULL);
                        break;
                    }
                }
            }
            else if(strcmp(arg[0], "bitmap") == 0)
            {
                for(i=0; i<10; i++)
                {
                    if( strcmp(bitmap_name[i], "") == 0)
                    {
                        suc = 1;
                        strcpy(bitmap_name[i], arg[1]);
                        bitmaps[i] = bitmap_create(atoi(arg[2]));
                        break;
                    }
                }
            }
            if(!suc)
            {
                puts("Creation failed..");
            }
        }
        else if( strcmp(command, "delete") == 0)
        {
            int idx = -1;
            idx = find_name_idx(arg[0], 0);
            if( idx != -1)
            {   // delete list
                struct list_elem* it;
                for(it = list_begin(&lists[idx]); it != list_end(&lists[idx]);)
                {
                    struct list_elem *tmp = it;
                    it = list_remove(it);
                    free(list_entry(tmp, struct list_item, elem));
                }
                list_init(&lists[idx]);
                strcpy(list_name[idx], "");
                continue;
            }
            idx = find_name_idx(arg[0], 1);
            if( idx != -1)
            {   // delete hash
                hash_destroy(&hashes[idx], my_hash_delete_func);
                strcpy(hash_name[idx], "");
                continue;
            }
            idx = find_name_idx(arg[0], 2);
            if( idx != -1)
            {   // delete bitmap
                strcpy(bitmap_name[idx], "");
                continue;
            }
        }
        else if( strcmp(command, "dumpdata") == 0)
        {
            // list check plz. There can be bitmap, hash too
            struct list_elem* it;
            int idx = find_name_idx(arg[0], 0);
            if(idx != -1)
            {   // Dumpdata list
                int list_idx = idx;
                for(it = list_begin(&lists[list_idx]); it != list_end(&lists[list_idx]); it = list_next(it))
                {
                    printf("%d ", list_entry(it, struct list_item, elem)->data );
                }
                puts("");
                continue;
            }
            idx = find_name_idx(arg[0], 1);
            if(idx != -1)
            {   // Dumpdata hash
                int hash_idx = idx;
                struct hash_iterator it;
                hash_first(&it, &hashes[hash_idx]);
                while(hash_next(&it))
                {
                    struct hash_item *t = hash_entry(it.elem, struct hash_item, elem);
                    printf("%d ", t->data);
                }
                puts("");
                continue;
            }
            idx = find_name_idx(arg[0], 2);
            if( idx != -1) 
            {   // Dumpdata bitmap
                unsigned int ui;
                int bitmap_idx = idx;
                for(ui=0; ui<bitmap_size(bitmaps[bitmap_idx]); ui++)
                    printf("%d", bitmap_test(bitmaps[bitmap_idx], ui));
                puts("");
                continue;
            }
            //else Err: Doesn't exist
        }
        else if( strcmp(command, "list_insert") == 0)
        {
            struct list_elem* it;
            int list_idx = find_name_idx(arg[0], 0);
            int pos = 0;
            int before;
            struct list_item* item = (struct list_item*)malloc(sizeof(struct list_item));
            
            before = atoi(arg[1]);
            item->data = atoi(arg[2]);
            it = list_begin(&lists[list_idx]);
            while(pos != before) pos++, it = list_next(it);
            list_insert(it, &(item->elem));
        }
        else if( strcmp(command, "list_splice") == 0)
        {
            struct list_elem *it1, *it2, *it3;
            int list_idx1 = find_name_idx(arg[0], 0);
            int list_idx2 = find_name_idx(arg[2], 0);
            int pos = 0;
            int target, start, end;
            target = atoi(arg[1]);
            start = atoi(arg[3]);
            end = atoi(arg[4]);
            it1 = list_begin(&lists[list_idx1]);

            while(pos != target) pos++, it1 = list_next(it1);
            pos = 0;
            it2 = list_begin(&lists[list_idx2]);
            while(pos != start) pos++, it2 = list_next(it2);
            pos = 0;
            it3 = list_begin(&lists[list_idx2]);
            while(pos != end) pos++, it3 = list_next(it3);

            list_splice(it1, it2, it3);
        }
        else if( strcmp(command, "list_push_front") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            struct list_item* item = (struct list_item*)malloc(sizeof(struct list_item));
            item->data = atoi(arg[1]);
            list_push_front(&lists[list_idx], &(item->elem));
        }
        else if( strcmp(command, "list_push_back") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            struct list_item* item = (struct list_item*)malloc(sizeof(struct list_item));
            item->data = atoi(arg[1]);
            list_push_back(&lists[list_idx], &(item->elem));
        }
        else if( strcmp(command, "list_remove") == 0)
        {
            struct list_elem *it;
            int list_idx = find_name_idx(arg[0], 0);
            int pos = 0;
            int before;
            
            before = atoi(arg[1]);
            it = list_begin(&lists[list_idx]);
            while(pos != before) pos++, it = list_next(it);
            list_remove(it);
            free(list_entry(it, struct list_item, elem));
        }
        else if( strcmp(command, "list_pop_front") == 0)
        {
            struct list_elem *tmp;
            int list_idx = find_name_idx(arg[0], 0);
            tmp = list_pop_front(&lists[list_idx]);
            free(list_entry(tmp, struct list_item, elem));
        }
        else if( strcmp(command, "list_pop_back") == 0)
        {
            struct list_elem *tmp;            
            int list_idx = find_name_idx(arg[0], 0);
            tmp = list_pop_back(&lists[list_idx]);
            free(list_entry(tmp, struct list_item, elem));

        }
        else if( strcmp(command, "list_front") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            printf("%d\n", list_entry(list_front(&lists[list_idx]), struct list_item, elem)->data);
        }
        else if( strcmp(command, "list_back") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            printf("%d\n", list_entry(list_back(&lists[list_idx]), struct list_item, elem)->data);
        }
        else if( strcmp(command, "list_size") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            printf("%d\n", (int)(list_size(&lists[list_idx])));
        }
        else if( strcmp(command, "list_empty") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            if( list_empty(&lists[list_idx])) puts("true");
            else puts("false");
        }
        else if( strcmp(command, "list_reverse") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            list_reverse(&lists[list_idx]);
        }
        else if( strcmp(command, "list_sort") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            list_sort(&lists[list_idx], list_func, NULL);
        }
        else if( strcmp(command, "list_insert_ordered") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);
            int data = atoi(arg[1]);
            struct list_item* item = (struct list_item*)malloc(sizeof(struct list_item));
            item->data = data;
            list_insert_ordered(&lists[list_idx], &(item->elem), list_func, NULL);
        }
        else if( strcmp(command, "list_unique") == 0)
        {
            int list_idx1 = find_name_idx(arg[0], 0);
            int list_idx2 = find_name_idx(arg[1], 0);
            if(arg_num == 3)
                list_unique(&lists[list_idx1], &lists[list_idx2], list_func, NULL);
            else if(arg_num == 2)
                list_unique(&lists[list_idx1], NULL, list_func, NULL);
            //If there is One list argument, then there can be Memory leakage.
        }
        else if( strcmp(command, "list_max") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);            
            printf("%d\n", list_entry(list_max(&lists[list_idx], list_func, NULL), struct list_item, elem)->data);
        }
        else if( strcmp(command, "list_min") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);            
            printf("%d\n", list_entry(list_min(&lists[list_idx], list_func, NULL), struct list_item, elem)->data);
        }
        else if( strcmp(command, "list_swap") == 0)
        {
            struct list_elem *it1, *it2;
            int list_idx = find_name_idx(arg[0], 0);      
            int t1 = atoi(arg[1]);
            int t2 = atoi(arg[2]);     
            int pos = 0;
            it1 = it2 = list_begin(&lists[list_idx]);
            while(pos != t1) pos++, it1 = list_next(it1);
            pos = 0;
            while(pos != t2) pos++, it2 = list_next(it2);
            list_swap(it1, it2);
        }
        else if( strcmp(command, "list_shuffle") == 0)
        {
            int list_idx = find_name_idx(arg[0], 0);            
            list_shuffle(&lists[list_idx]);
        }
        /* End of list commands */

        /* Hash commands */
        else if( strcmp(command, "hash_insert") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            struct hash_item *item = (struct hash_item*)malloc(sizeof(struct hash_item));
            item->data = atoi(arg[1]);
            hash_insert(&hashes[hash_idx], &(item->elem));
        }
        else if( strcmp(command, "hash_replace") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            struct hash_item *item = (struct hash_item*)malloc(sizeof(struct hash_item));
            item->data = atoi(arg[1]);
            hash_replace(&hashes[hash_idx], &(item->elem));
        }
        else if( strcmp(command, "hash_find") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            struct hash_item *item = (struct hash_item*)malloc(sizeof(struct hash_item));
            struct hash_elem *t;
            item->data = atoi(arg[1]);
            t = hash_find(&hashes[hash_idx], &(item->elem));
            if( t != NULL) printf("%d\n", hash_entry(t, struct hash_item, elem)->data);
        }
        else if( strcmp(command, "hash_delete") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            struct hash_item *item = (struct hash_item*)malloc(sizeof(struct hash_item));
            item->data = atoi(arg[1]);
            hash_delete(&hashes[hash_idx], &(item->elem));
        }
        else if( strcmp(command, "hash_clear") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            hash_clear(&hashes[hash_idx], my_hash_delete_func);
        }
        else if( strcmp(command, "hash_size") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            printf("%u\n", hash_size(&hashes[hash_idx]));
        }
        else if( strcmp(command, "hash_empty") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            if( hash_empty(&hashes[hash_idx]) ) puts("true");
            else puts("false");
        }
        else if( strcmp(command, "hash_apply") == 0)
        {
            int hash_idx = find_name_idx(arg[0], 1);
            if( strcmp(arg[1], "square") == 0)
                hash_apply(&hashes[hash_idx], my_hash_square_func);
            else if( strcmp(arg[1], "triple") == 0)
                hash_apply(&hashes[hash_idx], my_hash_triple_func);
        }
        /* End of Hash commands */

        /* bitmap commands */
        else if( strcmp(command, "bitmap_size") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            printf("%u\n", bitmap_size(bitmaps[bitmap_idx]));
        }
        else if( strcmp(command, "bitmap_set") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( strcmp(arg[2], "true") == 0)
                bitmap_set(bitmaps[bitmap_idx], atoi(arg[1]), true);
            else if( strcmp(arg[2], "false") == 0)
                bitmap_set(bitmaps[bitmap_idx], atoi(arg[1]), false);
        }
        else if( strcmp(command, "bitmap_mark") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            bitmap_mark(bitmaps[bitmap_idx], atoi(arg[1]));
        }
        else if( strcmp(command, "bitmap_reset") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            bitmap_reset(bitmaps[bitmap_idx], atoi(arg[1]));
        }
        else if( strcmp(command, "bitmap_flip") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            bitmap_flip(bitmaps[bitmap_idx], atoi(arg[1]));
        }
        else if( strcmp(command, "bitmap_test") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( bitmap_test(bitmaps[bitmap_idx], atoi(arg[1]))) puts("true");
            else puts("false");
        }
        else if( strcmp(command, "bitmap_set_all") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( strcmp(arg[1], "true") == 0)
                bitmap_set_all(bitmaps[bitmap_idx], true);
            else if( strcmp(arg[1], "false") == 0)
                bitmap_set_all(bitmaps[bitmap_idx], false);
        }
        else if( strcmp(command, "bitmap_set_multiple") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( strcmp(arg[3], "true") == 0)
                bitmap_set_multiple(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), true);
            else if( strcmp(arg[3], "false") == 0)
                bitmap_set_multiple(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), false);
        }
        else if( strcmp(command, "bitmap_count") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( strcmp(arg[3], "true") == 0)
                printf("%u\n", bitmap_count(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), true));
            else if( strcmp(arg[3], "false") == 0)
                printf("%u\n", bitmap_count(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), false));
        }
        else if( strcmp(command, "bitmap_contains") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( strcmp(arg[3], "true") == 0)
            {
                if( bitmap_contains(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), true)) puts("true");
                else puts("false");
            }
            else if( strcmp(arg[3], "false") == 0)
            {
                if( bitmap_contains(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), false)) puts("true");
                else puts("false");
            }
        }
        else if( strcmp(command, "bitmap_any") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if (bitmap_any(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]))) puts("true");
            else puts("false");
        }
        else if( strcmp(command, "bitmap_none") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if (bitmap_none(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]))) puts("true");
            else puts("false");
        }
        else if( strcmp(command, "bitmap_all") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if (bitmap_all(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]))) puts("true");
            else puts("false");
        }
        else if( strcmp(command, "bitmap_scan") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( strcmp(arg[3], "true") == 0)
                printf("%u\n", bitmap_scan(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), true));
            else if( strcmp(arg[3], "false") == 0)
                printf("%u\n", bitmap_scan(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), false));
        }
        else if( strcmp(command, "bitmap_scan_and_flip") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            if( strcmp(arg[3], "true") == 0)
                printf("%u\n", bitmap_scan_and_flip(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), true));
            else if( strcmp(arg[3], "false") == 0)
                printf("%u\n", bitmap_scan_and_flip(bitmaps[bitmap_idx], atoi(arg[1]), atoi(arg[2]), false));
        }
        else if( strcmp(command, "bitmap_dump") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            bitmap_dump(bitmaps[bitmap_idx]);
        }
        else if( strcmp(command, "bitmap_expand") == 0)
        {
            int bitmap_idx = find_name_idx(arg[0], 2);
            bitmaps[bitmap_idx] = bitmap_expand(bitmaps[bitmap_idx], atoi(arg[1]));
        }
        /* End of bitmap commands */
        else
        {
            puts("UnPredicted command");
            quit = 1;
        }

    }



    return 0;
}


void init_data_structure()
{
    int i;
    for(i=0; i<10; i++)
    {
        list_name[i][0] = '\0';
        bitmap_name[i][0] = '\0';
        hash_name[i][0] = '\0';
    }
}
// Find idx for the structure name
int find_name_idx(char input[50], int select)
{
    int i, ret = -1;
    if(select == 0)
    {
        for(i=0; i<10; i++)
            if( strcmp(list_name[i], input) == 0) return i;
    }
    else if( select == 1)
    {
        for(i=0; i<10; i++)
            if( strcmp(hash_name[i], input) == 0) return i;
    }
    else if( select ==2 )
    {
        for(i=0; i<10; i++)
            if( strcmp(bitmap_name[i], input) == 0) return i;
    }
    return ret;
}
/* list cmp func */
bool list_func(const struct list_elem *a, const struct list_elem *b, void* aux)
{
    struct list_item* t1 = list_entry(a, struct list_item, elem);
    struct list_item* t2 = list_entry(b, struct list_item, elem);
    if( t1->data < t2->data) return true;
    return false;
}
/* hash hash, cmp func */
unsigned my_hash_hash(const struct hash_elem *e, void *aux)
{
    return hash_int(hash_entry(e, struct hash_item, elem)->data);
}
bool my_hash_less(const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
    struct hash_item* t1 = hash_entry(a, struct hash_item, elem);
    struct hash_item* t2 = hash_entry(b, struct hash_item, elem);
    if( t1->data < t2->data) return true;
    return false;
}
/* hash_action_func */
void my_hash_delete_func(struct hash_elem *e, void *aux)
{
    if(e != NULL) free(hash_entry(e, struct hash_item, elem));
}
void my_hash_square_func(struct hash_elem *e, void *aux)
{
    if( e == NULL) return;
    int data = hash_entry(e, struct hash_item, elem)->data;
    hash_entry(e, struct hash_item, elem)->data = data * data;
}
void my_hash_triple_func(struct hash_elem *e, void *aux)
{
    if( e == NULL) return;
    int data = hash_entry(e, struct hash_item, elem)->data;
    hash_entry(e, struct hash_item, elem)->data = data * data * data;
}
