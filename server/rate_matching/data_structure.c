#include "matching.h"

/* list */
node_t* Nth_data(data_t* data, node_t* head) {
	node_t* p, * tmp = head;
	for (p = head;; p = p->next) {
		if (p->data.rating < data->rating) {
			if (p->next == NULL)
				return p;
		}
		else {
			return tmp;
		}
		tmp = p;
	}

	return p;
}

node_t* insert_node(node_t* n, data_t *data) {
	node_t* new_node;
	new_node = (node_t*)malloc(sizeof(node_t));
	if (new_node == NULL)
		return NULL;
	new_node->data = *data;
	new_node->next = n->next;
	n->next = new_node;

	return new_node;
}

node_t* insert_data(data_t* data, node_t* head) {
	return insert_node(Nth_data(data, head), data);
}

void remove_node(node_t* n) {
	node_t* old_node;
	if (n->next == NULL) {
		return;
	}
	old_node = n->next;
	n->next = old_node->next;
	free(old_node);
	return;
}

node_t* Nth_node(node_t* head, int n) {
	node_t* p;
	int i;
	for (p = head->next, i = 1; p != NULL; p = p->next, i++) {
		if (i == n)
			break;
	}
	return p;
}

/* queue */
void init_queue(queue_t *q){
  q->head.next = NULL;
  q->tail = &q->head;
}

void enqueue(queue_t *q,data_t *data){
    q->tail = insert_node(&q->head,data);
}

void dequeue(queue_t *q,data_t *data){
    if(q->head.next == NULL){
        printf("dequeue error\n");
        exit(1);
    }
    *data = q->head.next->data;
    remove_node(&q->head);
}