#include "circle_array.h"

short front=0;
short rear=0;
u16 circle_array[ARRAY_SIZE]={0};
short current_queue_size=0;

uint8_t add_data(u16 x)
{
	if(current_queue_size<ARRAY_SIZE)
	{
		circle_array[rear]=x;
		rear=(rear+1)%ARRAY_SIZE;
		current_queue_size++;
		return 1;
	}
	else
	{
		//printf("\nthe circle_queue is full!can't add!\n");
		return 0;
	}
}

u16 read_data()
{
	if(current_queue_size!=0)
		return circle_array[front];
	else
	{
		//printf("\nthe current queue is empty,no data found!\n");
		return 0;
	}
	
}

uint8_t delete_data()
{
	if(current_queue_size!=0)
	{
		front=(front+1)%ARRAY_SIZE;
		current_queue_size--;
		return 1;
	}
	else
	{
		//printf("\nthe current queue is empty,no data to delete!\n");
		return 0;
	}
}
