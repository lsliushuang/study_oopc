#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//////
//创建一个所有校验器的抽象类
typedef struct _validator
{
	bool (*const validate)(struct _validator *pThis, int value);  //定义一种抽象的校验函数指针类型

}validator_t;
validator_t *pthis;

//派生一个范围校验的类
typedef struct _range_validator
{
	validator_t isa;  //继承自validator_t抽象类
	const int max;
	const int min;

}range_validator_t;
range_validator_t range_validator; //实例化一个范围校验的类

//派生一个偶校验的子类
typedef struct _oddeven_validator
{
	validator_t isa;  //继承自validator_t抽象类
	bool is_odd;

}oddeven_validator_t;
oddeven_validator_t oddeven_validator; //实例化一个偶校验的类

bool range_validate(struct _validator *pThis, int value)
{
	range_validator_t *range_validator = (range_validator_t *)pThis;
	return (value <= range_validator->max) && (value >= range_validator->min);
}
