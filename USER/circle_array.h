#ifndef __CIRCLE_ARRAY_H
#define __CIRCLE_ARRAY_H
#include "stm32f10x.h"
#define ARRAY_SIZE 128

extern short front;
extern short rear;
extern u16 circle_array[ARRAY_SIZE];
extern short current_queue_size;

u8 add_data(u16 x);
u16 read_data(void);
u8 delete_data(void);

#endif
