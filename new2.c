/*
 ============================================================================
 Name        : new2.c
 Author      : ls
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>


//int compare_float(const void *e1 , const void *e2)
//{
//	return  *((const float*)e1) - *((const float*)e2);
//}
int compare_str(const void *e1 , const void *e2)
{
	return   *((const char*)e2) - *((const char*)e1);
}

struct list_node //单链表节点
{

	int age;
	//char *name;
	struct list_node *p_next;
};
typedef struct _slist_node
{
	struct _slist_node  *p_next;

}slist_node_t;
typedef struct __dlist_node
{
	struct _dlist_node  *p_next;
	struct _dlist_node  *p_prev;

}dlist_node_t;

typedef struct _slist_int
{
	slist_node_t node; //把链表节点包含进来
	int age;
}slist_int;

int slist_node_process(void *p_arg , slist_node_t *node)
{
	printf("age = %d \r\n",((slist_int*)node)->age);
	return 0;

}
typedef int (*slist_node_process_t)(void *p_arg , slist_node_t *node);
//int slist_foreach(slist_node_t *p_head , slist_node_process_t slist_node_process,void *p_arg)
//{
//
//
//}

int list_add_head(slist_node_t *p_head , slist_node_t *p_node)
{
	p_node->p_next = p_head->p_next;
	p_head->p_next = p_node;
	return 0;
}
int list_add_tail(slist_node_t *p_head , slist_node_t *p_node)
{
	 //printf("222222222222222222222222222222\r\n");
	if(p_head == NULL) //链表为空
	{
		p_head = p_node;
		 p_node->p_next = NULL;
		 //printf("111111111111111\r\n");
	}
	else
	{
		//printf("22222222222222222\r\n");
		slist_node_t *p_tmp = p_head;
		while(p_tmp->p_next != NULL)
		{
			p_tmp = p_tmp->p_next; //遍历链表到末尾
		}
		p_tmp->p_next = p_node;
		p_node->p_next = NULL;

	}
	return 0;
}
int compare_float(float *e1 , float *e2)
{
	return (*e1 - *e2);
}
int main(void)
{
	float aa[10] = {10,25,36,12,29,45,10,36,59,49};
	slist_node_t p_head = {NULL}; //头节点指针
	slist_int node1,node2,node3,node4;
	slist_node_t *tmp_p; //临时指针

	node1.age = 25;
	node2.age = 29;
	node3.age = 30;
	node4.age = 90;
	list_add_tail(&p_head,&node1.node);
	list_add_tail(&p_head,&node2.node);
	list_add_tail(&p_head,&node3.node);
	list_add_head(&p_head,&(node4.node));

	tmp_p = p_head.p_next;//用临时指针去访问链表元素
	slist_node_process(NULL,tmp_p);
	tmp_p = tmp_p->p_next;
	slist_node_process(NULL,tmp_p);
	tmp_p = tmp_p->p_next;
	slist_node_process(NULL,tmp_p);
    qsort(aa,10,sizeof(aa[0]),compare_float);
    int i = 0;
    for(i=0;i<10;i++)
    {
    	printf("aa = %f \r\n",aa[i]);
    }
   return EXIT_SUCCESS;

}
