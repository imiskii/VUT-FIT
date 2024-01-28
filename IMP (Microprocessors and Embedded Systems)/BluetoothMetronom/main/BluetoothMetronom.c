/**
 * @file BluetoothMetronom.c
 * @author Michal Ľaš (xlasmi00)
 * @brief main of Bluetooth Metronome app.
 * @version 1.0.1
 * @date 2023-11-22
 * 
 */

#include "BluetoothMetronom.h"

/*--------------------------------------------------------------------*/
/*------------------------------ MAIN APP ----------------------------*/
/*--------------------------------------------------------------------*/

void app_main(void)
{
    // Init GPIO pins
    init_gpio(GPIO_LED);
    init_gpio(GPIO_BUZZER);

    // Init PWM for buzzer
    pwm_init();

    // Init metronome
    metronome_vars.queue = xQueueCreate(10, sizeof(fnc_t));
    init_metronome_timer(&metronome_vars.gptimer, &metronome_vars.gptimer_conf, &metronome_vars.queue);

    // Init Bluetooth LE
    init_ble();
    

    // Delete timer
    //ESP_ERROR_CHECK(gptimer_del_timer(gptimer));
}

/*--------------------------------------------------------------------*/
/*--------------------- MENTRONOME FUNCTIONALITY ---------------------*/
/*--------------------------------------------------------------------*/

void init_metronome_timer(gptimer_handle_t *gptimer, gptimer_all_config_t *conf, QueueHandle_t *queue)
{
    // Init configurations
    conf->timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    conf->timer_config.direction = GPTIMER_COUNT_UP;
    conf->timer_config.resolution_hz = 1000000; // 1MHz, 1 tick = 1us
    conf->timer_config.intr_priority = 0;

    // Create new timer
    ESP_ERROR_CHECK(gptimer_new_timer(&(conf->timer_config), gptimer));

    // Init alarm configurations
    conf->alarm_config.alarm_count = 750000; // period 750 000 microseconds => 80BPM, this will be later changed
    conf->alarm_config.flags.auto_reload_on_alarm = true; // enable reload on alarm
    conf->alarm_config.reload_count = 0; // on alarm reload to 0

    // Set timer configuration
    ESP_ERROR_CHECK(gptimer_set_alarm_action(*gptimer, &(conf->alarm_config)));

    // Set on alarm event
    conf->cbs.on_alarm = tik_event;

    // bind function triggered on alarm
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(*gptimer, &(conf->cbs), *queue));
}


void pwm_init()
{
    ledc_timer_config_t timer_conf = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RESOLUTION,
        .freq_hz = PWM_FREQ_C4,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_conf));

    ledc_channel_config_t ledc_conf = {
        .gpio_num = GPIO_BUZZER,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_conf));
}


void set_duty_cycle(uint32_t duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}


void set_buzzer_frequency(uint32_t frequency) {
    ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, frequency);
}


static bool IRAM_ATTR tik_event(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)data;

    fnc_t actionFnc = {
        .fncArgs = NULL,
        .fnc = click_metronome,
    };

    if (xQueueSendFromISR(queue, &actionFnc, &high_task_awoken) != pdTRUE)
    {
        fprintf(stderr, "ERROR: xQueueSendFromISR\n");
    }

    return (high_task_awoken == pdTRUE);
}


static void click_metronome()
{
    // Set frequency (rhythm)
    if (metronome_vars.currBeat <= 0)
    {
        set_buzzer_frequency(PWM_FREQ_F4);
        metronome_vars.currBeat = metronome_vars.rythm;
    }
    else
    {
        set_buzzer_frequency(PWM_FREQ_C4);
    }
    metronome_vars.currBeat--;
    
    // Set to 200ms toggle
    set_duty_cycle((metronome_vars.volume * ((1 << PWM_RESOLUTION) - 1)) / 100); // duty is percentage of volume
    vTaskDelay(200 / portTICK_PERIOD_MS);
    set_duty_cycle(0);
}


static void metronome_task(void *fArg)
{
    fnc_t actionFnc;
    QueueHandle_t queue = (QueueHandle_t)fArg;

    while (true)
    {
        if (xQueueReceive(queue, &actionFnc, portMAX_DELAY))
        {
            actionFnc.fnc(actionFnc.fncArgs);
        }
    }
}


void run_metronome(gptimer_handle_t gptimer, TaskHandle_t *metronome_task_handler, QueueHandle_t *queue)
{
    // Create a metronome task
    xTaskCreate(metronome_task, "metronome_task", 2048, *queue, tskIDLE_PRIORITY, metronome_task_handler);

    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}


void stop_metronome(gptimer_handle_t gptimer, TaskHandle_t *metronome_task_handler, QueueHandle_t *queue)
{
    // Delete the metronome task
    vTaskDelete(*metronome_task_handler);

    // Remove items from queue
    xQueueReset(*queue);

    // Stop the timer
    ESP_ERROR_CHECK(gptimer_stop(gptimer));

    // Reset the timer value to 0
    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer, 0));

    // Shut the buzzer
    set_duty_cycle(0);
}


void set_metronome_tempo(int bpm)
{
    uint64_t bpm_u = MICROSECONDS_IN_MINUTE/bpm; // beats per minute in microseconds
    metronome_vars.gptimer_conf.alarm_config.alarm_count = bpm_u;

    // Set timer new configuration
    ESP_ERROR_CHECK(gptimer_set_alarm_action(metronome_vars.gptimer, &(metronome_vars.gptimer_conf.alarm_config)));
    // Reset timer
    ESP_ERROR_CHECK(gptimer_set_raw_count(metronome_vars.gptimer, 0));
    // reset beat counter
    metronome_vars.currBeat = 0;
}

/*--------------------------------------------------------------------*/
/*--------------------- BLUETOOTH FUNCTIONALITY ----------------------*/
/*--------------------------------------------------------------------*/


void init_ble()
{
    ESP_LOGI(TAG, "Starting BLE");

    // Init. NVS flash using
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_flash_init();
    }

    // Init. the host stack
    ret = nimble_port_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "nimble_port_init() failed with error: %d", ret);
    }

    ble_svc_gap_device_name_set("Bluetooth Metronome"); // Init NimBLE conf. server name
    ble_svc_gap_init();                     // Init. NimBLE conf. gap service
    ble_svc_gatt_init();                    // Init. NimBLE conf. gatt service
    ble_gatts_count_cfg(gatt_svcs);         // Init. NimBLE conf. config gatt services
    ble_gatts_add_svcs(gatt_svcs);          // Init. NimBLE conf. queues gatt services
    // Init. NimBLE host conf.
    ble_hs_cfg.sync_cb = ble_app_sync;
    nimble_port_freertos_init(bt_task_run); // Run the Bluetooth LE thread
}


static void ble_app_sync()
{
    ble_hs_id_infer_auto(0, &ble_addr_type);
    ble_app_advertise();
}


void ble_app_advertise()
{
    ESP_LOGI(TAG, "BLE start advertise");

    int ret;
    // Set GAP device name
    struct ble_hs_adv_fields fields;
    const char *app_name;
    memset(&fields, 0, sizeof(fields));
    app_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *) app_name;
    fields.name_len = strlen(app_name);
    fields.name_is_complete = 1;
    // Discoverability in forthcoming advertisement (general) | BLE-only (BR/EDR unsupported)
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    ret = ble_gap_adv_set_fields(&fields);
    if (ret != 0)
    {
        ESP_LOGE(TAG, "error setting advertisement data: %d", ret);
        return;
    }

    // Set GAP connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    ret = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
    if (ret != 0)
    {
        ESP_LOGE(TAG, "error enabling advertisement: %d", ret);
        return;
    }
}


static void bt_task_run()
{
    ESP_LOGI(TAG, "BLE host task started");
    nimble_port_run(); // Returns only when nimble_port_stop() is executed
    nimble_port_freertos_deinit();
}


static void bt_task_stop()
{
    ESP_LOGI(TAG, "BLE stopping");
    int ret = nimble_port_stop();
    if (ret == 0)
    {
        ret = nimble_port_deinit();
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_nimble_hci_and_controller_deinit() failed with error: %d", ret);
        }
    }
}


static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI(TAG, "BLE GAP event connected %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        // if failed Advertise
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        else
        {
            // Make signal 3x blink build in led
            for (int i = 0; i < 3; i++){
                flash_BIL(200);
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }
            // Enable timer
            ESP_ERROR_CHECK(gptimer_enable(metronome_vars.gptimer));
            ESP_LOGI(TAG, "TIMER enabled");
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "BLE GAP event disconnected");
        // Disable timer
        uint64_t timerCount;
        ESP_ERROR_CHECK(gptimer_get_raw_count(metronome_vars.gptimer, &timerCount));
        if (timerCount != 0)
            ESP_ERROR_CHECK(gptimer_stop(metronome_vars.gptimer));
        ESP_ERROR_CHECK(gptimer_disable(metronome_vars.gptimer));
        ESP_LOGI(TAG, "TIMER disabled");
        // Advertise again
        ble_app_advertise();
        break;
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI(TAG, "BLE GAP event");
        ble_app_advertise();
        break;
    default:
        break;
    }
    return 0;
}


static int ble_gatt_event(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch (ctxt->op)
    {
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        ESP_LOGI(TAG, "BLE GATT write access");
        // The code is alway the first byte
        char code = (char) *ctxt->om->om_data;
        char *data = NULL;
        // if there is more than one byte
        if (ctxt->om->om_len > 1)
        {
            // The data are rest of the recived message
            data = (char *) heap_caps_calloc(ctxt->om->om_len-1, sizeof(char), MALLOC_CAP_8BIT);
            if (data == NULL)
            {
                ESP_LOGE(TAG, "Unable to allocate memmory for data");
                return EXIT_FAILURE;
            }
            memcpy(data, ctxt->om->om_data+1, ctxt->om->om_len-1);
        }
        write_client(code, data, ctxt->om->om_len-1);
        break;
    case BLE_GATT_ACCESS_OP_READ_CHR:
        ESP_LOGI(TAG, "BLE GATT read access");
        break;
    default:
        ESP_LOGI(TAG, "BLE GATT unknow access");
        break;
    }

    return EXIT_SUCCESS;
}


int powerOf10(int exponent)
{
    int ret = 1;
    while (exponent > 0)
    {
        ret *= 10;
        exponent--;
    }
    return ret;
}


int parse_data(char *data, uint16_t data_len)
{
    int ret = 0;
    int index = 0;
    for (size_t i = powerOf10(data_len-1); i > 0; i/=10)
    {
        // convert char to int
        ret += ((data[index] - '0')*i);
        index++;
    }
    return ret;
}


void parse_init_data(char *data, uint16_t data_len, metronome_init_t *ret)
{
    uint16_t from = 0;
    uint16_t to = 0;

    for (short rsts = 0; rsts < 3; rsts++)
    {
        while ((data[to] != ' ') && (to < data_len))
        {
            to++;
        }
        switch (rsts)
        {
        case 0:
            ret->bpm = parse_data(data+from, (to-from));
            break;
        case 1:
            ret->volume = parse_data(data+from, (to-from));
            break;
        case 2:
            ret->rythm = parse_data(data+from, (to-from));
            break;
        default:
            break;
        }
        to += 1;
        from = to;
    }
}


static void write_client(char code, char* data, uint16_t data_len)
{
    switch (code)
    {
    case '0': // Set tempo
        int bpm = parse_data(data, data_len);
        set_metronome_tempo(bpm);
        ESP_LOGI(TAG, "Metronome tempo set to: %d BPM", bpm);
        break;
    case '1': // Set rhythm
        int rythm_code = parse_data(data, data_len);
        metronome_vars.rythm = rythm_code;
        metronome_vars.currBeat = 0; // reset beat counter
        // Reset counting
        ESP_ERROR_CHECK(gptimer_set_raw_count(metronome_vars.gptimer, 0));
        ESP_LOGI(TAG, "Metronome rhythm set to mode: %d", rythm_code);
        break;
    case '2': // Set volume
        int volume = parse_data(data, data_len);
        metronome_vars.volume = volume;
        ESP_LOGI(TAG, "Metronome volume set to: %d", volume);
        break;
    case '3': // start metronome
        run_metronome(metronome_vars.gptimer, &metronome_vars.metronome_task_handler, &metronome_vars.queue);
        ESP_LOGI(TAG, "Metronome started");
        break;
    case '4': // stop metronome
        stop_metronome(metronome_vars.gptimer, &metronome_vars.metronome_task_handler, &metronome_vars.queue);
        ESP_LOGI(TAG, "Metronome stopped");
        break;
    case '5': // Init metronome
        metronome_init_t init_vals;
        parse_init_data(data, data_len, &init_vals);
        // Set bpm
        set_metronome_tempo(init_vals.bpm);
        ESP_LOGI(TAG, "BPM initialized to: %d", init_vals.bpm);
        // Set volume
        metronome_vars.volume = init_vals.volume;
        ESP_LOGI(TAG, "Volume initialized to: %d", init_vals.volume);
        // Set rhythm
        metronome_vars.rythm = init_vals.rythm;
        metronome_vars.currBeat = 0; // reset beat counter
        // Reset counting
        ESP_ERROR_CHECK(gptimer_set_raw_count(metronome_vars.gptimer, 0));
        ESP_LOGI(TAG, "Rhythm initialized to: %d", init_vals.rythm);

    default:
        break;
    }
    // free used data
    heap_caps_free(data);
}

/* END OF FILE */