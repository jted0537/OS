#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>

#include "debug.h"
#include "round.h"
#include "list.h"
#include "limits.h"
#include "bitmap.h"
#include "hash.h"
#include "hex_dump.h"

struct list list_arr[10];
struct hash hash_arr[10];
struct bitmap *bm_arr[10];

bool list_less_func_new(const struct list_elem *e1, const struct list_elem *e2, void *aux){
	struct list_item *temp1 = list_entry(e1, struct list_item, elem);
	struct list_item *temp2 = list_entry(e2, struct list_item, elem);
	if(temp1->data < temp2->data) return true;
	else return false;
}

unsigned hash_hash_func_new(const struct hash_elem *e, void *aux){
	struct hash_item *h = hash_entry(e, struct hash_item, elem);
	return hash_int(h->data);
}

bool hash_less_func_new(const struct hash_elem *a, const struct hash_elem *b, void *aux){
	struct hash_item *temp1 = hash_entry(a, struct hash_item, elem);
	struct hash_item *temp2 = hash_entry(b, struct hash_item, elem);
	if(temp1->data < temp2->data) return true;
	else return false;
}

void hash_action_func_des(struct hash_elem *e, void *aux){
	struct hash_item *temp = hash_entry(e, struct hash_item, elem);
	free(temp);
}

void hash_action_func_square(struct hash_elem *e, void *aux){
	struct hash_item *temp = hash_entry(e, struct hash_item, elem);
	temp->data *= temp->data;
}

void hash_action_func_triple(struct hash_elem *e, void *aux){
	struct hash_item *temp = hash_entry(e, struct hash_item, elem);
	temp->data = temp->data * temp->data * temp->data;
}

int main(){
	char command[100];
	char first_str[20];
	char second_str[20];
	char third_str[20];
	char fourth_str[20];
	char fifth_str[20];
	char sixth_str[20];
	int input_num = 0;
	int aux = 0;
	int listIdx, hashIdx, bmIdx;
	while(1){
		aux = 0;
		memset(command,0,sizeof(command));
		fgets(command, sizeof(command), stdin);
		command[strlen(command)-1] = '\0';
		input_num = sscanf(command, "%s%s%s%s%s%s", first_str, second_str, third_str, fourth_str, fifth_str, sixth_str);
		/*---------- List function ----------*/
		listIdx = second_str[strlen(second_str)-1] - 48;
		hashIdx = second_str[strlen(second_str)-1] - 48;
		bmIdx = second_str[strlen(second_str)-1] - 48;
		if(!strcmp(first_str, "create")) {
				if(!strcmp(second_str, "list")){
					listIdx = third_str[strlen(third_str)-1] - 48;
					list_init(&list_arr[listIdx]);
				}//List create
				else if(!strcmp(second_str, "hashtable")){
					hashIdx = third_str[strlen(third_str)-1] - 48;
					hash_init(&hash_arr[hashIdx], hash_hash_func_new, hash_less_func_new, &aux);
				}//Hash create
				else if(!strcmp(second_str, "bitmap")){
					bmIdx = third_str[strlen(third_str)-1] - 48;
					int num = atoi(fourth_str);
					bm_arr[bmIdx] = bitmap_create(num);
				}//Bitmap create
		}
		else if(!strcmp(first_str, "dumpdata")){
			if(!strncmp(second_str, "list", 4)){
				for(struct list_elem *it = list_begin(&list_arr[listIdx]); it!=list_end(&list_arr[listIdx]); it = list_next(it)){
					struct list_item *temp = list_entry(it, struct list_item, elem);
					printf("%d ", temp->data);
				}
				if(list_size(&list_arr[listIdx])) printf("\n");
			}//List dump
			else if(!strncmp(second_str, "hash", 4)){
				struct hash_iterator it;
				hash_first(&it, &hash_arr[hashIdx]);
				while(hash_next(&it)){
					struct hash_elem *e = hash_cur(&it);
					struct hash_item *temp = hash_entry(e, struct hash_item, elem);
					printf("%d ", temp->data);
				}
				if(hash_size(&hash_arr[hashIdx])) printf("\n");
			}//Hash dump
			else if(!strncmp(second_str, "bm", 2)){
				int size = bitmap_size(bm_arr[bmIdx]);
				for(int i = 0; i<size; i++) printf("%d", bitmap_test(bm_arr[bmIdx], i));
				if(size) printf("\n");
			}//Bitmap dump
		}
		else if(!strcmp(first_str, "list_push_back")){
			int num = atoi(third_str);
			struct list_elem *e = (struct list_elem *)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(e,struct list_item,elem);
			temp->data = num;
			list_push_back(&list_arr[listIdx], e);
		}
		else if(!strcmp(first_str, "list_front")){
			struct list_elem *e = list_front(&list_arr[listIdx]);
			struct list_item *temp = list_entry(e,struct list_item, elem);
			printf("%d\n",temp->data);
		}
		else if(!strcmp(first_str, "list_back")){
            struct list_elem *e = list_back(&list_arr[listIdx]);
            struct list_item *temp = list_entry(e,struct list_item, elem);
            printf("%d\n",temp->data);
		}
		else if(!strcmp(first_str, "list_pop_front")){
			struct list_elem *e = list_pop_front(&list_arr[listIdx]);
			struct list_item *temp = list_entry(e,struct list_item, elem);
			free(temp);
		}
		else if(!strcmp(first_str, "list_pop_back")){
			struct list_elem *e = list_pop_back(&list_arr[listIdx]);
			struct list_item *temp = list_entry(e,struct list_item, elem);
			free(temp);
		}
		else if(!strcmp(first_str, "delete")){
			if(!strncmp(second_str, "list", 4)){
				while(!list_empty(&list_arr[listIdx])){
					struct list_elem *e = list_pop_front(&list_arr[listIdx]);
					struct list_item *temp = list_entry(e,struct list_item, elem);
					free(temp);
				}
			}
			else if(!strncmp(second_str, "hash", 4)){
				hash_destroy(&hash_arr[hashIdx], hash_action_func_des);
			}
			else if(!strncmp(second_str, "bm", 2)){
				bitmap_destroy(bm_arr[bmIdx]);
			}
		}//list_frontback.in
		
		else if(!strcmp(first_str, "list_insert")){
			int i = 0;
			int loc = atoi(third_str);
			int val = atoi(fourth_str);
			struct list_elem *buf_e = (struct list_elem *)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(buf_e,struct list_item, elem);
			temp->data = val;
			struct list_elem *e = list_begin(&list_arr[listIdx]);
			for(; i<loc && e!=list_end(&list_arr[listIdx]); e = list_next(e), i++);
			list_insert(e, buf_e);
		}//list_insert.in
		
		else if(!strcmp(first_str, "list_insert_ordered")){
			int val = atoi(third_str);
			struct list_elem *e = (struct list_elem *)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(e,struct list_item, elem);
			temp->data = val;
			list_insert_ordered(&list_arr[listIdx], e, list_less_func_new, &aux);
		}//list_insert_ordered.in
		
		else if(!strcmp(first_str, "list_empty")){
			if(list_empty(&list_arr[listIdx])) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(first_str, "list_size")){
			printf("%zu\n", list_size(&list_arr[listIdx]));
		}
		else if(!strcmp(first_str, "list_max")){
			struct list_elem *e = list_max(&list_arr[listIdx], list_less_func_new, &aux);
			struct list_item *temp = list_entry(e,struct list_item, elem);
			printf("%d\n",temp->data);
		}
		else if(!strcmp(first_str, "list_min")){
			struct list_elem *e = list_min(&list_arr[listIdx], list_less_func_new, &aux);
			struct list_item *temp = list_entry(e,struct list_item, elem);
			printf("%d\n",temp->data);
		}//list_maxminsizeempty.in

		else if(!strcmp(first_str, "list_push_front")){
			int num = atoi(third_str);
			struct list_elem *e = (struct list_elem*)malloc(sizeof(struct list_elem));
			struct list_item *temp = list_entry(e,struct list_item,elem);
			temp->data = num;
			list_push_front(&list_arr[listIdx], e);
		}//list_pushes.in

		else if(!strcmp(first_str, "list_remove")){
			int loc = atoi(third_str);
			struct list_elem *e = list_begin(&list_arr[listIdx]);
			for(int i=0; i<loc && e!=list_end(&list_arr[listIdx]); e = list_next(e), i++);
			struct list_item *temp = list_entry(e, struct list_item, elem);
			list_remove(e);
			free(temp);
		}//list_remove.in

		else if(!strcmp(first_str, "list_reverse")){
			list_reverse(&list_arr[listIdx]);
		}//list_reverse.in

		else if(!strcmp(first_str, "list_shuffle")){
			list_shuffle(&list_arr[listIdx]);	
		}//list_shuffle.in
		
		else if(!strcmp(first_str, "list_sort")){
			list_sort(&list_arr[listIdx], list_less_func_new, &aux);
		}//list_sort.in

		else if(!strcmp(first_str, "list_splice")){
			int first_loc = atoi(third_str);
			int listIdx2 = fourth_str[strlen(fourth_str)-1] - 48;
			int start_loc = atoi(fifth_str);
			int end_loc = atoi(sixth_str);
			struct list_elem *e1 = list_begin(&list_arr[listIdx]);
			struct list_elem *e2 = list_begin(&list_arr[listIdx2]);
			struct list_elem *e3 = list_begin(&list_arr[listIdx2]);
			for(int i=0; i<first_loc && e1!=list_end(&list_arr[listIdx]); e1 = list_next(e1), i++);
			for(int i=0; i<start_loc && e2!=list_end(&list_arr[listIdx2]); e2 = list_next(e2), i++);
			for(int i=0; i<end_loc && e3!=list_end(&list_arr[listIdx2]); e3 = list_next(e3), i++);
			list_splice(e1,e2,e3);
		}//list_splice.in

		else if(!strcmp(first_str, "list_swap")){
			int first_idx = atoi(third_str);
			int second_idx = atoi(fourth_str);
			struct list_elem *e1 = list_begin(&list_arr[listIdx]);
			for(int i = 0; i<first_idx && e1!=list_end(&list_arr[listIdx]); e1 = list_next(e1), i++);
			struct list_elem *e2 = list_begin(&list_arr[listIdx]);
			for(int i = 0; i<second_idx && e2!=list_end(&list_arr[listIdx]); e2 = list_next(e2), i++);
			list_swap(e1,e2);
		}//list_swap.in

		else if(!strcmp(first_str, "list_unique")){
			if(input_num == 3){
				int listIdx2 = third_str[strlen(third_str)-1] - 48;
				list_unique(&list_arr[listIdx], &list_arr[listIdx2], list_less_func_new, &aux);
			}
			else if(input_num == 2){
				struct list temp_list;
				list_init(&temp_list);
				list_unique(&list_arr[listIdx], &temp_list, list_less_func_new, &aux);
				while(!list_empty(&temp_list)){
					struct list_elem *e = list_pop_front(&temp_list);
                    struct list_item *temp = list_entry(e,struct list_item, elem);
                    free(temp);
				}	
			}
		}//list_unique.in
		/*---------- List function ----------*/

		
		/*---------- Hash function ----------*/
		else if(!strcmp(first_str, "hash_insert")){
			int val = atoi(third_str);
			struct hash_elem *e = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(e, struct hash_item, elem);
			temp->data = val;
			hash_insert(&hash_arr[hashIdx], e);
		}
		else if(!strcmp(first_str, "hash_apply")){
			if(!strcmp(third_str, "square")){
				hash_apply(&hash_arr[hashIdx], hash_action_func_square);
			}
			else if(!strcmp(third_str, "triple")){
				hash_apply(&hash_arr[hashIdx], hash_action_func_triple);
			}
		}//hash_apply.in
		
		else if(!strcmp(first_str, "hash_delete")){
			int val = atoi(third_str);
			struct hash_elem *e = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(e, struct hash_item, elem);
			temp->data = val;
			struct hash_elem *temp_e = hash_delete(&hash_arr[hashIdx], e);
			if(temp_e!=NULL) {
				struct hash_item *temp_temp = hash_entry(temp_e, struct hash_item, elem);
				free(temp_temp);
			}
		}//hash_delete.in

		else if(!strcmp(first_str, "hash_empty")){
			if(hash_empty(&hash_arr[hashIdx])) printf("true\n");
			else printf("false\n");
		}
		else if(!strcmp(first_str, "hash_size")){
			printf("%zu\n", hash_size(&hash_arr[hashIdx]));
		}
		else if(!strcmp(first_str, "hash_clear")){
			hash_clear(&hash_arr[hashIdx], hash_action_func_des);
		}//hash_etc.in
		
		else if(!strcmp(first_str, "hash_find")){
			int val = atoi(third_str);
			struct hash_elem *e = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(e, struct hash_item, elem);
			temp->data = val;
			struct hash_elem *e2 = hash_find(&hash_arr[hashIdx], e);
			if(e2!=NULL) printf("%d\n", temp->data);
		}//hash_find.in

		else if(!strcmp(first_str, "hash_replace")){
			int val = atoi(third_str);
			struct hash_elem *e = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item *temp = hash_entry(e, struct hash_item, elem);
			temp->data = val;
			struct hash_elem *temp_e = hash_replace(&hash_arr[hashIdx], e);
			if(temp_e != NULL) free(temp_e);
		}

		/*---------- Hash function ----------*/
		
		
		/*---------- Bitmap function ----------*/
		else if(!strcmp(first_str, "bitmap_mark")){
			size_t loc = atoi(third_str);
			bitmap_mark(bm_arr[bmIdx], loc);
		}
		else if(!strcmp(first_str, "bitmap_all")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			if(bitmap_all(bm_arr[bmIdx], start, cnt)) printf("true\n");
			else printf("false\n");
		}//bitmap_all.in

		else if(!strcmp(first_str, "bitmap_any")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			if(bitmap_any(bm_arr[bmIdx], start, cnt)) printf("true\n");
			else printf("false\n");
		}//bitmap_any.in

		else if(!strcmp(first_str, "bitmap_contains")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			bool value;
			if(!strcmp(fifth_str, "true")) value = true;
			else if(!strcmp(fifth_str, "false")) value = false;
			if(bitmap_contains(bm_arr[bmIdx], start, cnt, value)) printf("true\n");
			else printf("false\n");
		}//bitmap_contains.in

		else if(!strcmp(first_str, "bitmap_count")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			bool value;
			if(!strcmp(fifth_str, "true")) value = true;
			else if(!strcmp(fifth_str, "false")) value = false;
			printf("%zu\n", bitmap_count(bm_arr[bmIdx], start, cnt, value));
		}//bitmap_count.in

		else if(!strcmp(first_str, "bitmap_dump")){
			bitmap_dump(bm_arr[bmIdx]);
		}//bitmap_dump.in

		else if(!strcmp(first_str, "bitmap_expand")){
			size_t size = atoi(third_str);
			bm_arr[bmIdx] = bitmap_expand(bm_arr[bmIdx], size); 
		}
		else if(!strcmp(first_str, "bitmap_set_all")){
			bool value;
			if(!strcmp(third_str, "true")) value = true;			
			else if(!strcmp(third_str, "false")) value = false;
			bitmap_set_all(bm_arr[bmIdx], value);
		}//bitmap_expand.in

		else if(!strcmp(first_str, "bitmap_flip")){
			size_t loc = atoi(third_str);
			bitmap_flip(bm_arr[bmIdx], loc);
		}//bitmap_flip.in

		else if(!strcmp(first_str, "bitmap_none")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			if(bitmap_none(bm_arr[bmIdx], start, cnt)) printf("true\n");
			else printf("false\n");
		}//bitmap_none.in

		else if(!strcmp(first_str, "bitmap_reset")){
			size_t loc = atoi(third_str);
			bitmap_reset(bm_arr[bmIdx], loc);
		}//bitmap_reset.in

		else if(!strcmp(first_str, "bitmap_scan_and_flip")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			bool value;
			if(!strcmp(fifth_str, "true")) value = true; 
			else if(!strcmp(fifth_str, "false")) value = false;
			printf("%zu\n", bitmap_scan_and_flip(bm_arr[bmIdx], start, cnt, value));
		}//bitmap_scan_and_flip.in
		
		else if(!strcmp(first_str, "bitmap_scan")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			bool value;
			if(!strcmp(fifth_str, "true")) value = true;
			else if(!strcmp(fifth_str, "false")) value = false;
			printf("%zu\n", bitmap_scan(bm_arr[bmIdx], start, cnt, value));
		}//bitmap_scan.in

		else if(!strcmp(first_str, "bitmap_set")){
			size_t loc = atoi(third_str);
			bool value;
			if(!strcmp(fourth_str, "true")) value = true;
			else if(!strcmp(fourth_str, "false")) value = false;
			bitmap_set(bm_arr[bmIdx], loc, value);
		}//bitmap_set.in

		else if(!strcmp(first_str, "bitmap_set_multiple")){
			size_t start = atoi(third_str);
			size_t cnt = atoi(fourth_str);
			bool value;
			if(!strcmp(fifth_str, "true")) value = true;
			else if(!strcmp(fifth_str, "false")) value = false;
			bitmap_set_multiple(bm_arr[bmIdx], start, cnt, value);
		}//bitmap_set_multiple.in

		else if(!strcmp(first_str, "bitmap_size")){
			printf("%zu\n", bitmap_size(bm_arr[bmIdx]));
		}//bitmap_size.in

		else if(!strcmp(first_str, "bitmap_test")){
			int loc = atoi(third_str);
			if(bitmap_test(bm_arr[bmIdx], loc)) printf("true\n");
			else printf("false\n");
		}//bitmap_test.in

		/*---------- Bitmap function ----------*/
		else if(!strcmp(command,"quit")) break;
		else continue;
	}
	return 0;
}
