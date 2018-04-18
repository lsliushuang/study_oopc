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


typedef void *iterator;
typedef void (*iterator_next_t)(iterator *ptr);
typedef void (*iterator_prev_t)(iterator *ptr);

typedef struct _iterator_if //迭代器接口
{
	iterator_next_t pf_next; //函数指针，后移函数，类似p++
	iterator_prev_t pf_prev;  //前移函数，类似p--

}iterator_if_t;

static void __dlist_iterator_next(iterator* ptr)//迭代器指向容器下个数据
{
	*ptr = ((dlist_node_t*)*ptr)->p_next;
}
static void __dlist_iterator_prev(iterator* ptr)
{
	*ptr = ((dlist_node_t*)*ptr)->p_prev;
}

iterator_if_t *dlist_iterator_if_get (void)
{
	static iterator_if_t iterator_if;
	iterator_if.pf_next = __dlist_iterator_next;
	iterator_if.pf_prev = __dlist_iterator_prev;
	return &iterator_if;
}
