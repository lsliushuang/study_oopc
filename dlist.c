#include <stdio.h>
#include <stdlib.h>

//双向链表节点
typedef struct _dlist_node
{
	struct _dlist_node  *p_next;
	struct _dlist_node  *p_prev;

}dlist_node_t;

typedef dlist_node_t dlist_head_t;

dlist_head_t head;//定义一个头结点

//初始化头结点
int dlist_init(dlist_head_t *p_head)
{
	if(p_head == NULL)
	{
		return -1;
	}
	p_head->p_next = p_head;
	p_head->p_prev = p_head;
}

//寻找某个节点的前一个节点
dlist_node_t *dlist_pre_get(dlist_head_t *head , dlist_node_t *pos)
{
//	dlist_node_t *tmp_p;
//	tmp_p = head;
//	while((tmp_p != NULL) && (tmp_p->p_next != pos))
//	{
//		tmp_p = tmp_p->p_next;
	int dlist_del(dlist_head_t *head, dlist_node_t *node)
	{
		if((head == NULL) || (node == NULL) || (head == node))
		{
			return -1;
		}
	    node->p_prev->p_next = node->p_next;
	    node->p_next->p_prev = node->p_prev;

	    node->p_next = NULL;
	    node->p_prev = NULL;
	    return 0;
	}
//	}
//	return tmp_p;
	if(pos != NULL)
	{
		return pos->p_prev;
	}
	return NULL;
}

//寻找某一节点的后一节点
dlist_node_t *dlist_next_get(dlist_head_t *head , dlist_node_t *pos)
{
	if(pos != NULL)
	{
		return pos->p_next;
	}
	return NULL;
}
//获取尾节点
dlist_node_t *dlist_tail_get(dlist_head_t *head)
{
	if(head != NULL)
	{
		return head->p_prev;
	}
	return NULL;
}

//获取第一个用户节点
dlist_node_t *dlist_begin_get(dlist_head_t *head)
{
	if(head != NULL)
	{
		return head->p_next;
	}
	return NULL;
}

int dlist_tail_add(dlist_head_t *head,dlist_node_t *pnode)
{
	if(head == NULL)
	{
		return -1;
	}
	pnode->p_prev = head->p_prev;
	pnode->p_next = head;
	head->p_prev->p_next = pnode;
	head->p_prev = pnode;
}

int dlist_add(dlist_head_t *head , dlist_node_t *pos, dlist_node_t *node)
{
	if((head == NULL) || (pos == NULL) || (node == NULL))
	{
		return -1;
	}
	node->p_next = pos->p_next;
	node->p_prev = pos;
	pos->p_next->p_prev = node;
	pos->p_next = node;
	return 0;
}

int dlist_del(dlist_head_t *head, dlist_node_t *node)
{
	if((head == NULL) || (node == NULL) || (head == node))
	{
		return -1;
	}
    node->p_prev->p_next = node->p_next;
    node->p_next->p_prev = node->p_prev;

    node->p_next = NULL;
    node->p_prev = NULL;
    return 0;
}
