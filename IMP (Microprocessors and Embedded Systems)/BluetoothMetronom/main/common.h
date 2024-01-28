/**
 * @file common.h
 * @author Michal Ľaš (xlasmi00)
 * @brief header file for common.c
 * @version 1.0.1
 * @date 2023-11-23
 * 
 */


#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef COMMON_H
#define COMMON_H


/******************** MACROS ********************/

#define BUILD_IN_LED_PIN GPIO_NUM_2


/****************** DATATYPES *******************/


/****************** FUNCTIONS *******************/


/**
 * @brief initialize a led to GPIO OUTPUT
 * 
 * @param gpio_num GPIO number. If you want to the output level of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16)
 */
void init_gpio(gpio_num_t gpio_num);


/**
 * @brief Build In Led flash - flash the build in led. The build in led used to be on GPIO_NUM_2, if it is on different port change the macro BUILD_IN_LED_PIN!
 * 
 * @param time time of the led flash in `ms`
 */
void flash_BIL(unsigned time);


/**
 * @brief For given time the given GPIO pin will turn on `1` and than back off to off `0`
 * 
 * @param time ON in `ms`
 * @param gpio_num GPIO number. If you want to the output level of e.g. GPIO16, gpio_num should be GPIO_NUM_16 (16)
 */
void toggle_gpio(unsigned time, gpio_num_t gpio_num);


/**
 * @brief Do the same as toggle_gpio but for each element in gpios list
 * 
 * @param time ON in `ms`
 * @param gpio_nums list of GPIO number. If you want to the output level of e.g. GPIO16, gpio_num should include GPIO_NUM_16 (16)
 * @param gpio_nums_size length of gpio_nums list
 */
void toggle_gpios(unsigned time, gpio_num_t gpio_nums[], unsigned gpio_nums_size);


#endif // COMMON_H