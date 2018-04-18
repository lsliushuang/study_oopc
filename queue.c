#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define  QUEUE_MAX_SIZE   65

//临时测试用
typedef struct _TEMP_DATA
{
	uint8_t board_num;
	uint8_t temp_warn_flag;
	uint8_t temp_alarm_flag;

}temp_data_t;

temp_data_t *temp_data;
struct queueCDT
{
	uint8_t *data;
	int head; //队列头
	int tail; //队列尾
};

typedef struct queueCDT * queueADT;
queueADT new_queue(void) //新建队列
{
	queueADT queue;
	queue = (queueADT)malloc(sizeof(struct queueCDT));
	queue->data = (uint8_t *)malloc(sizeof(uint8_t) * QUEUE_MAX_SIZE);
	queue->head = 0;
	queue->tail = 0;
	return queue;
}
void free_queue(queueADT queue)  //释放队列
{
	free(queue->data);
	free(queue);
}

bool queue_is_empty(queueADT queue)
{
	return (queue->head == queue->tail); //头尾相等，链表为空
}
bool queue_is_full(queueADT queue)
{
	return (((queue->tail + 1) % QUEUE_MAX_SIZE) == queue->head);
}

bool insert_queue(queueADT queue, uint8_t value)  //入队
{
	if(queue_is_full) //保证队列尾部有空间可用
	{
		return false;
	}
	queue->data[queue->tail] = value;
	queue->tail = (queue->tail + 1) % QUEUE_MAX_SIZE;
	return true;

}
bool out_queue(queueADT queue, int *p_value)  //出队
{
	if(queue_is_empty(queue))
	{
		return false;
	}
	*p_value = queue->data[queue->head];
	queue->head = (queue->head + 1) % QUEUE_MAX_SIZE;
	return true;
}

uint32_t get_queue_length(queueADT queue)
{

	return (queue->tail - queue->head + QUEUE_MAX_SIZE) % QUEUE_MAX_SIZE;
}


/*************************************************
	Function:    temp_tzdata_deal
	Description: 温度算法判断
	Input:  温度数据：temp
	Output:
	Return:
	Others:
*************************************************/
void temp_tzdata_deal(uint8_t* temp)
{
	 int i,k;
	 uint8_t save_buf[6] = {0};
	 static uint16_t temp_sum = 0;
	 static uint8_t j=3,board_count1,board_count2;
	 static uint8_t temp_5s_average[8][13]; //5s求一个均值，一分钟之后计算温升，要多5s数据
	 static uint8_t temp_current[8][1], temp_rise[8][1],temp_diff[8][1]; //绝温,温升,温差
	 static queueADT temp_queue[8];
	 for(i = 0; i < 8; i++)
	 {
		 temp_queue[i] = new_queue(); //新建8个温度数据队列
	 }
	 printf("*******start  temp tz data_deal ***********\r\n");
	 save_buf[0] = 0x55; save_buf[1] = 0xaa;
	 if(temp_data->board_num == 1)
	 {
		 if(board_count1 < 65)
		 {
			for(i = 0; i < 4; i++)
			{
				insert_queue(temp_queue[i],temp[i]);
			}
			board_count1++;
		 }
		 else if(board_count1 == 65)
		 {
			for(k=0;k<4;k++)
			{
				out_queue(temp_queue[k],&temp_queue[k]->data[temp_queue[k]->head]); //出去一个
				insert_queue(temp_queue[k],temp[k]); //进来一个
			}
			board_count1++;
		 }
	 }
	 else if(temp_data->board_num == 2)
	 {
		 if(board_count2 < 65)
		 {
			for(i = 0; i < 4; i++)
			{
				insert_queue(temp_queue[i+4],temp[i]);
			}
			board_count2++;
		 }
		 else if(board_count2 == 65)
		 {
			for(k=4;k<8;k++)
			{
			    out_queue(temp_queue[k],&temp_queue[k]->data[temp_queue[k]->head]); //出去一个
				insert_queue(temp_queue[k],temp[k-4]); //进来一个
			}
   	        board_count2++;
		 }
	 }
	 //进行温度计算和判断
	if(board_count1 == 66 && board_count2 == 66) //存满一分钟的数据，进行一次计算，以后每秒进行一次
    {
		printf("start diagnosisc temp!\r\n");
		//DEBUG("board1 = %d , board2 = %d  \r\n",board1_count,board2_count);
		board_count1 = 65; board_count2 = 65;
		//求各个通道均值
		for(k=0;k<8;k++)
		{
		   for(i=0;i<13;i++)
		   {
			   temp_5s_average[k][i] = (temp_queue[k]->data[temp_queue[k]->head + i*5] + temp_queue[k]->data[temp_queue[k]->head + i*5 + 1]
										+ temp_queue[k]->data[temp_queue[k]->head + i*5 + 2] + temp_queue[k]->data[temp_queue[k]->head + i*5 + 3]
										  + temp_queue[k]->data[temp_queue[k]->head + i*5 + 4])/5;
		   }

	     }
		//求各个通道温升和绝温
	    for(i=0;i<8;i++)
		{
		      temp_rise[i][0] =  (uint8_t)abs((int)temp_5s_average[i][12] - (int)temp_5s_average[i][1]);
		      temp_current[i][0] =  temp_queue[i]->data[temp_queue[i]->tail];//每个通道当前最新的温度
			  printf("temp_rise = %d \r\n",temp_rise[i][0]);
			  printf("temp_current = %d \r\n",temp_current[i][0]);
	    }
		//求同侧各通道温差
	    for(i=0;i<8;i++)
		{
		   k=i;
		   switch (k)
		   {
			   case 0:
				   if((temp_data->temp_warn_flag&0x02)||(temp_data->temp_alarm_flag&0x02))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[1][11];
				   }
				   if((temp_data->temp_warn_flag&0x04)||(temp_data->temp_alarm_flag&0x04))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[2][11];
				   }
				   if((temp_data->temp_warn_flag&0x08)||(temp_data->temp_alarm_flag&0x08))
				   {
					    j--;
					}
				   else
				   {
					   temp_sum += temp_5s_average[3][11];
				   }
				   if(j==0)
				   {
					   temp_diff[0][0]=0;
				   }
				   else
				   {
						temp_diff[0][0] = abs(temp_5s_average[0][11] - temp_sum/j);
				   }
				   j=3;
		        break;
				case 1:
				   if((temp_data->temp_warn_flag&0x01)||(temp_data->temp_alarm_flag&0x01))
				   {
					    j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[0][11];
				   }
				   if((temp_data->temp_warn_flag&0x04)||(temp_data->temp_alarm_flag&0x04))
				   {
					   j--;

				   }
				   else
				   {
					   temp_sum += temp_5s_average[2][11];
				   }
				   if((temp_data->temp_warn_flag&0x08)||(temp_data->temp_alarm_flag&0x08))
				   {
					   j--;

				   }
				   else
				   {
					   temp_sum += temp_5s_average[3][11];
				   }
				   if(j==0)
				   {
					   temp_diff[1][0]=0;
				   }
				   else
				   {
					   temp_diff[1][0] = abs(temp_5s_average[1][11] - temp_sum/j);
				   }
				   j=3;

				break;
			    case 2:
				   if((temp_data->temp_warn_flag&0x01)||(temp_data->temp_alarm_flag&0x01))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[0][11];
				   }
				   if((temp_data->temp_warn_flag&0x02)||(temp_data->temp_alarm_flag&0x02))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[1][11];
				   }
				   if((temp_data->temp_warn_flag&0x08)||(temp_data->temp_alarm_flag&0x08))
				   {
					   j--;

				   }
				   else
				   {
					   temp_sum += temp_5s_average[3][11];
				   }

				   if(j==0)
				   {
					   temp_diff[2][0]=0;
				   }
				   else
				   {
					   temp_diff[2][0] = abs(temp_5s_average[2][11] - temp_sum/j);
				   }
				   j=3;
				break;
				case 3:
				   if((temp_data->temp_warn_flag&0x01)||(temp_data->temp_alarm_flag&0x01))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[0][11];
				   }
				   if((temp_data->temp_warn_flag&0x02)||(temp_data->temp_alarm_flag&0x02))
				   {
					   j--;

				   }
				   else
				   {
					   temp_sum += temp_5s_average[1][11];
				   }
				   if((temp_data->temp_warn_flag&0x04)||(temp_data->temp_alarm_flag&0x04))
				   {
					   j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[2][11];
				   }
				   if(j==0)
				   {
					  temp_diff[3][0]=0;
				   }
			       else
			       {
					   temp_diff[3][0] = abs(temp_5s_average[3][11] - temp_sum/j);
			       }
				   j=3;
				break;
				case 4:
				   if((temp_data->temp_warn_flag&0x20)||(temp_data->temp_alarm_flag&0x20))
				   {
					   j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[5][11];
				   }
				   if((temp_data->temp_warn_flag&0x40)||(temp_data->temp_alarm_flag&0x40))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[6][11];
				   }
				   if((temp_data->temp_warn_flag&0x80)||(temp_data->temp_alarm_flag&0x80))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[7][11];
				   }
				   if(j==0)
				   {
					   temp_diff[4][0]=0;
				   }
				   else
				   {
					   temp_diff[4][0] = abs(temp_5s_average[4][11] - temp_sum/j);
				   }
				   j=3;
			   break;
			   case 5:
				   if((temp_data->temp_warn_flag&0x10)||(temp_data->temp_alarm_flag&0x10))
				   {
					   j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[4][11];
				   }
				   if((temp_data->temp_warn_flag&0x40)||(temp_data->temp_alarm_flag&0x40))
				   {
					   j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[6][11];
				   }
				   if((temp_data->temp_warn_flag&0x80)||(temp_data->temp_alarm_flag&0x80))
				   {
					   j--;

				   }
				   else
				   {
					   temp_sum += temp_5s_average[7][11];
				   }
				   if(j==0)
				   {
						temp_diff[5][0]=0;
				   }
				   else
				   {
						temp_diff[5][0] = abs(temp_5s_average[5][11] - temp_sum/j);
				   }
				   j=3;
				break;
				case 6:
				   if((temp_data->temp_warn_flag&0x10)||(temp_data->temp_alarm_flag&0x10))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[4][11];
				   }
				   if((temp_data->temp_warn_flag&0x20)||(temp_data->temp_alarm_flag&0x20))
				   {
					   j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[5][11];
				   }
				   if((temp_data->temp_warn_flag&0x80)||(temp_data->temp_alarm_flag&0x80))
				   {
						j--;
				   }
				   else
				   {
					   temp_sum += temp_5s_average[7][11];
				   }
				   if(j==0)
				   {
						temp_diff[6][0]=0;
				   }
				   else
				   {
						temp_diff[6][0] = abs(temp_5s_average[6][11] - temp_sum/j);
				   }
			    	j=3;
			    break;
				case 7:
				   if((temp_data->temp_warn_flag&0x10)||(temp_data->temp_alarm_flag&0x10))
				   {
					    j--;

				   }
				   else
				   {
					   temp_sum += temp_5s_average[4][11];
				   }
				   if((temp_data->temp_warn_flag&0x20)||(temp_data->temp_alarm_flag&0x20))
				   {
						j--;
					}
				   else
				   {
					   temp_sum += temp_5s_average[5][11];
				   }
				   if((temp_data->temp_warn_flag&0x40)||(temp_data->temp_alarm_flag&0x40))
				   {
						j--;

				   }
				   else
				   {
					   temp_sum += temp_5s_average[6][11];
				   }
				   if(j==0)
				   {
						temp_diff[7][0]=0;
				   }
				   else
				   {
						temp_diff[7][0] = abs(temp_5s_average[7][11] - temp_sum/j);
				   }
				   j=3;
				break;
				default:
				break;
			}
				temp_sum = 0;
		}
	 temp_data->temp_warn_flag = 0x00;
	 temp_data->temp_alarm_flag = 0x00;
            /*******************下面进行温度预报警判断*****************************/
	 for(i=0;i<8;i++)
	 {
			if(temp_current[i][0]>=160 || (temp_diff[i][0]>=50&&temp_diff[i][0]<65))//温度值加40之后进行预报警判断
			{
				  if(i==0)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x01;
				  }
				  if(i==1)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x02;
				  }
				  if(i==2)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x04;
				  }
				  if(i==3)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x08;
				  }
				  if(i==4)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x10;
				  }
				  if(i==5)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x20;
				  }
				  if(i==6)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x40;
				  }
				  if(i==7)
				  {
					  temp_data->temp_warn_flag = temp_data->temp_warn_flag|0x80;
				  }
			}

			if(temp_diff[i][0]>=65 ||temp_current[i][0]>=180 || (temp_rise[i][0]>=15 && temp_current[i][0]>=130)||(temp_current[i][0]==255))
			{
				  if(i==0)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x01;
				  }
				  if(i==1)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x02;
				  }
				  if(i==2)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x04;
				  }
				  if(i==3)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x08;
				  }
				  if(i==4)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x10;
				  }
				  if(i==5)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x20;
				  }
				  if(i==6)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x40;
				  }
				  if(i==7)
				  {
					  temp_data->temp_alarm_flag = temp_data->temp_alarm_flag|0x80;
				  }
			}
	  }

   }

}
