/**
 * @file BluetoothMetronom.h
 * @author Michal Ľaš (xlasmi00)
 * @brief header file for BluetoothMetronom.c
 * @version 1.0.1
 * @date 2023-11-22
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_heap_caps.h"
#include "soc/clk_tree_defs.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

/* BLE */
#include "esp_nimble_hci.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_uuid.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"


#include "common.h"

#ifndef BLUETOOTH_METRONOME_H
#define BLUETOOTH_METRONOME_H


/****************** DATATYPES *******************/


/**
 * @brief Type pointer to function
 * 
 */
typedef void (*FunctionPtr)(void *fArgs);


/**
 * @brief Pointer to funcion with arguments
 * 
 */
typedef struct {
    void *fncArgs;      // function arguments
    FunctionPtr fnc;    // pointer to function
} fnc_t;


/**
 * @brief Configuration for gptimer
 * 
 */
typedef struct {
    gptimer_config_t timer_config;          // GPTimer configuration
    gptimer_alarm_config_t alarm_config;    // GPTimer alarm configuration
    gptimer_event_callbacks_t cbs;          // GPTimer event callbacks
} gptimer_all_config_t;


/**
 * @brief Variables for handeling metronome actions
 * 
 */
typedef struct {
    gptimer_handle_t gptimer;               // GPTimer handler
    gptimer_all_config_t gptimer_conf;      // GPTimer configurations
    QueueHandle_t queue;                    // Task queue
    TaskHandle_t metronome_task_handler;    // Metronome task handler
    int volume;                             // metronome volume
    int rythm;                              // metronome rhythm configuration
    int currBeat;                           // metronome current rhythm
} metronome_vars_t;


/**
 * @brief Values needed to initialize the metronome before start
 * 
 */
typedef struct {
    int bpm;    // metronome BPM
    int volume; // metronome volume
    int rythm;  // metronome rhythm
} metronome_init_t;

/****************** FUNCTIONS *******************/


/*--------------------- MENTRONOME FUNCTIONALITY ---------------------*/


/**
 * @brief Initialize GPTimer configurations for metronome
 * 
 * @param gptimer GPTimer handler
 * @param conf GPTimer configurations
 * @param queue Metronome tasks queue
 */
void init_metronome_timer(gptimer_handle_t *gptimer, gptimer_all_config_t *conf, QueueHandle_t *queue);


/**
 * @brief Initialize Pulse Width Modulation (PWM) for buzzer pin
 * 
 */
void pwm_init();


/**
 * @brief Set the duty cycle of buzzer output pin (starts signal with given duty)
 * 
 * @param duty duty cycle (this determines the volume of the buzzer)
 */
void set_duty_cycle(uint32_t duty);


/**
 * @brief Set the buzzer frequency (set note played by buzzer)
 * 
 * @param frequency frequency of desired sound
 */
void set_buzzer_frequency(uint32_t frequency);


/**
 * @brief Handler for gptimer alarm event, 
 * 
 * @param timer Timer handle created by `gptimer_new_timer`
 * @param edata Alarm event data, fed by driver
 * @param data User data, passed from `gptimer_register_event_callbacks`
 * @return true if high priority task has been waken up by this function
 * @return false if high priority task has not been waken up by this function
 */
static bool tik_event(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *data);


/**
 * @brief Toggles metronome on and off to generate buzzer 'beep' effect
 * 
 */
static void click_metronome();


/**
 * @brief Function for handeling events genereted by GPTimer (periodic clicking)
 * 
 * @param fArg arguments of function which was invoked by the event
 */
static void metronome_task(void *fArg);


/**
 * @brief Starts metronome task
 * 
 * @param gptimer GPTimer handler
 * @param metronome_task_handler Metronome task handler
 * @param queue Metronome task queue
 */
void run_metronome(gptimer_handle_t gptimer, TaskHandle_t *metronome_task_handler, QueueHandle_t *queue);


/**
 * @brief Stops metronome task
 * 
 * @param gptimer GPTimer handler
 * @param metronome_task_handler Metronome task handler
 * @param queue Metronome task queue
 */
void stop_metronome(gptimer_handle_t gptimer, TaskHandle_t *metronome_task_handler, QueueHandle_t *queue);


/**
 * @brief Set the metronome tempo
 * 
 * @param bpm desired Beats Per Minute (BPM) value
 */
void set_metronome_tempo(int bpm);


/*--------------------- BLUETOOTH FUNCTIONALITY ----------------------*/


/**
 * @brief Initialize Bluetooth Low Energy (BLE)
 * 
 */
void init_ble();


/**
 * @brief Determines the address type for BLE and advertise the bluetooth service
 * 
 */
static void ble_app_sync();


/**
 * @brief Define GAP configuration and adverise application via BLE
 * 
 */
void ble_app_advertise();


/**
 * @brief Starts bluetooth task thread
 * 
 */
static void bt_task_run();


/**
 * @brief Stops bluetooth task thread
 * 
 */
static void bt_task_stop();


/**
 * @brief Handler for GAP events
 * 
 * @param event the type of event being signalled
 * @param arg additional arguments
 */
static int ble_gap_event(struct ble_gap_event *event, void *arg);


/**
 * @brief Handler for GATT events
 * 
 * @param conn_handle Connection handler
 * @param attr_handle Attribute handler
 * @param ctxt Transefer message
 * @param arg additional arguments
 * @return int 0 for success 1 for failure
 */
static int ble_gatt_event(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);


/**
 * @brief Handeling for GATT write events
 * 
 * @param code message code (unique code of operation)
 * @param data message data
 * @param data_len length of the given data
 */
static void write_client(char code, char* data, uint16_t data_len);


/**
 * @brief Function for parsing given message data
 * 
 * @param data given message data
 * @param data_len length of the given data
 * @return int data value
 */
int parse_data(char *data, uint16_t data_len);


/**
 * @brief Function for parsing data from init code message 
 * 
 * @param data message data
 * @param data_len length of the data
 * @param ret pointer to structure which will be filled with parsed data
 */
void parse_init_data(char *data, uint16_t data_len, metronome_init_t *ret);


/**
 * @brief 10^exponent
 * 
 * @param exponent 
 * @return int 10 power to exponent
 */
int powerOf10(int exponent);


/************** MACROS AND GLOBALS **************/


// LOG TAG
static const char *TAG = "BluetoothMetronome";

// Bluetooth Low Energy Address Type => set by `ble_hs_id_infer_auto()` in `ble_app_sync()`
static uint8_t ble_addr_type;

// Global variable for handeling metronome parameters (timer, tempo, volume, rythm)
static metronome_vars_t metronome_vars;

// PIN for build in led output
#define GPIO_LED BUILD_IN_LED_PIN
// PIN for buzzer output
#define GPIO_BUZZER GPIO_NUM_14
// nomber of microseconds in on minute
#define MICROSECONDS_IN_MINUTE 60000000
// PWM frequency of tone C4 in Hz
#define PWM_FREQ_C4 261
// PWM frequency of tone F4 in Hz
#define PWM_FREQ_F4 349
// PWM resolution
#define PWM_RESOLUTION LEDC_TIMER_10_BIT


// UUID => 31e8370e-26c8-4a46-bd51-f77873cc72eb
// 0xeb, 0x72, 0xcc, 0x73, 0x78, 0xf7, 0x51, 0xbd, 0x46, 0x4a, 0xc8, 0x26, 0x0e, 0x37, 0xe8, 0x31
static const ble_uuid128_t gatt_svr_svc_uuid = BLE_UUID128_INIT(0xeb, 0x72, 0xcc, 0x73, 0x78, 0xf7, 0x51, 0xbd, 0x46, 0x4a, 0xc8, 0x26, 0x0e, 0x37, 0xe8, 0x31);

// UUID => 84cd9b4d-00e0-454f-bd2c-12f54ea14f8b
// 0x8b, 0x4f, 0xa1, 0x4e, 0xf5, 0x12, 0x2c, 0xbd, 0x4f, 0x45, 0xe0, 0x00, 0x4d, 0x9b, 0xcd, 0x84
static const ble_uuid128_t gatt_svr_chr_uuid = BLE_UUID128_INIT(0x8b, 0x4f, 0xa1, 0x4e, 0xf5, 0x12, 0x2c, 0xbd, 0x4f, 0x45, 0xe0, 0x00, 0x4d, 0x9b, 0xcd, 0x84);

// Define GATT service
const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,                       // Service UUID
        .characteristics = (struct ble_gatt_chr_def[]){
        {.uuid = &gatt_svr_chr_uuid.u,                      // Characteristi UUID
            //flags set permissions. User can read this characterstic, can write to it,and get notified
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY,
            .access_cb = ble_gatt_event},
        {0,}},},
    {0,}};


#endif // BLUETOOTH_METRONOME_H