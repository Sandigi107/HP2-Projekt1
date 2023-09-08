#include <gpiod_utils.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdbool.h> 

#define SEMAPHORE_ID_PRINT 0 


struct thread_args
{
    struct gpiod_line* led_a;
    struct gpiod_line* led_b;
    struct gpiod_line* button;
    uint16_t blink_speed_ms;
    bool blink_enabled;
    uint8_t previous_value;
};




void* run_thread(void* arg) 
{
struct thread_args* self  = (struct thread_args*)(arg);

    while(1)
    {
      if (gpiod_line_event_detected(self->button, GPIOD_LINE_EDGE_RISING, &self->previous_value)) {
            self->blink_enabled = !self->blink_enabled;
            semaphore_reserve(SEMAPHORE_ID_PRINT);
            if (self->blink_enabled == true) {
                printf("Leds connected to pin %hu and %hu now enabled!\n", 
                       gpiod_line_offset(self->led_a), gpiod_line_offset(self->led_b));
            } else {
                printf("Leds connected to pin %hu and %hu now disabled!\n", 
                       gpiod_line_offset(self->led_a), gpiod_line_offset(self->led_b));
            }
            delay_ms(10);
            semaphore_release(SEMAPHORE_ID_PRINT);
        }

    if (self->blink_enabled == true) 
    { 
            gpiod_line_set_value(self->led_a, 1); 
            gpiod_line_set_value(self->led_b, 0); 
            delay_ms(self->blink_speed_ms); 

            gpiod_line_set_value(self->led_a, 0); 
            gpiod_line_set_value(self->led_b, 1); 
            delay_ms(self->blink_speed_ms); 
        } else 
        { 
            gpiod_line_set_value(self->led_a, 0);
            gpiod_line_set_value(self->led_b, 0); 
        }

    }

return 0;
}


int main (void)
{  
    struct gpiod_line* led1 = gpiod_line_new(17, GPIOD_LINE_DIRECTION_OUT);
    struct gpiod_line* led2 = gpiod_line_new(22, GPIOD_LINE_DIRECTION_OUT);
    struct gpiod_line* led3 = gpiod_line_new(23, GPIOD_LINE_DIRECTION_OUT);
    struct gpiod_line* led4 = gpiod_line_new(24, GPIOD_LINE_DIRECTION_OUT);
    struct gpiod_line* button1 = gpiod_line_new(27, GPIOD_LINE_DIRECTION_IN);
    struct gpiod_line* button2 = gpiod_line_new(25, GPIOD_LINE_DIRECTION_IN);

    struct thread_args args1 = {led1, led2, button1, 100, false };
    struct thread_args args2 = {led3, led4, button2, 500, false };
    pthread_t t1, t2;
    pthread_create(&t1, 0, run_thread, &args1);
    pthread_create(&t2, 0, run_thread, &args2);
    pthread_join(t1, 0);
    pthread_join(t2, 0);

    return 0;
}
