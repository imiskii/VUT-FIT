/**
 * @file common.c
 * @author Michal Ľaš (xlasmi00)
 * @brief common constructions for esp-idf (ESP32)
 * @version 1.0.1
 * @date 2023-11-23
 * 
 */

#include "common.h"


void init_gpio(gpio_num_t gpio_num)
{
    gpio_reset_pin(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
}


void flash_BIL(unsigned time)
{
    gpio_set_level(BUILD_IN_LED_PIN, 1);
    vTaskDelay(time / portTICK_PERIOD_MS);
    gpio_set_level(BUILD_IN_LED_PIN, 0);
}


void toggle_gpio(unsigned time, gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 1);
    vTaskDelay(time / portTICK_PERIOD_MS);
    gpio_set_level(gpio_num, 0);
}


void toggle_gpios(unsigned time, gpio_num_t gpio_nums[], unsigned gpio_nums_size)
{
    for(unsigned i = 0; i <= gpio_nums_size; i++)
    {
        gpio_set_level(gpio_nums[i], 1);
    }

    vTaskDelay(time / portTICK_PERIOD_MS);

    for(unsigned i = 0; i <= gpio_nums_size; i++)
    {
        gpio_set_level(gpio_nums[i], 0);
    }
}