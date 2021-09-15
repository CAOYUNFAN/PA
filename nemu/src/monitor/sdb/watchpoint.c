#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  word_t data;
  char* exp;
  bool success;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
   for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *args){
	if(free_==NULL) assert(0);
	WP *ans=free_;
	free_=free_->next;
	ans->next=head;
	head=ans;
	
	strcpy(ans->exp,args);
	ans->success=1;
	ans->data=expr(args,&ans->success);

	Log("Watch point %d has been built.\n",ans->NO);
	return ans;
}

void free_wp(WP *wp){
	wp->next=free_;
	free_=wp;
	Log("Watch point %d has been freed.\n",wp->NO);
	return;
}
