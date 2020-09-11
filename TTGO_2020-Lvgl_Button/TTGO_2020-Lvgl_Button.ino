
#include "config.h"
#include <esp_now.h>
#include <WiFi.h>

//@-audio
#include <HTTPClient.h>         //Remove Audio Lib error
#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

#include "door1.h"

//@-配置用户字体
LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(dxLED7);
LV_FONT_DECLARE(myLED_Font);

//@-配置用户图片数据
// LV_IMG_DECLARE(me);
LV_IMG_DECLARE(TTGO_BG);
LV_IMG_DECLARE(rich);

//@-TTGO
TTGOClass *ttgo;
//@-TTGO 电源管理
AXP20X_Class *power;
//@-TTGO 电源管理中断
bool irq_power = false;
//@-3轴传感器BMA423
BMA *sensor_bma423;
//@-TTGO 3轴中断
bool irq_bma423 = false;

//@-audio
AudioGeneratorMP3 *mp3;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

//@-lvgl 控件
lv_obj_t * btn;
lv_obj_t *btn2;

lv_obj_t *btn10;
lv_obj_t *btn20;
lv_obj_t *btn30;
lv_obj_t *btn40;

lv_obj_t *label;
lv_obj_t *label1; 

lv_obj_t *label_data;

//@-RTC时间显示
lv_obj_t *label_time; 

//@-实时电量显示
lv_obj_t *label_batt;

/*Create a chart*/
lv_obj_t * chart;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_label;



bool btn2_flag = true;

//@-WIFI NOW数据结构
typedef struct struct_message {
    char a[32];
    int b;
    float c;
    String d;
    bool e;
} struct_message;

// Create a struct_message called myData
struct_message recv_Data;
struct_message send_Data;

uint8_t broadcastAddress[] = {0x84, 0x0D, 0x8E, 0x0B, 0xB2, 0x54};    //ESP32


char display_buf[128];
int system_tick = 0;
int System_Sleep_Tick = 0;


// callback function that will be executed when data is received
//@-wifi now 数据接收函数
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&recv_Data, incomingData, sizeof(recv_Data));
//   Serial.print("Bytes received: ");
//   Serial.println(len);
//   Serial.print("Char: ");
//   Serial.println(recv_Data.a);
//   Serial.print("Int: ");
//   Serial.println(recv_Data.b);
//   Serial.print("Float: ");
//   Serial.println(recv_Data.c);
//   Serial.print("String: ");
//   Serial.println(recv_Data.d);
//   Serial.print("Bool: ");
//   Serial.println(recv_Data.e);
//   Serial.println();
}
    

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (obj == btn10) {
        send_Data.b = 0;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    } 
    else if (obj == btn20) {
        send_Data.b = 180;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    } 
    else if (obj == btn30) {
        send_Data.b = send_Data.b + 10;
        if(send_Data.b > 180)
        send_Data.b = 180;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    } 
    else if (obj == btn40) {
        send_Data.b = send_Data.b - 10;
        if(send_Data.b < 0)
        send_Data.b = 0;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    } 
    else if (obj == btn2) {

        if(btn2_flag == true)
        {
          btn2_flag = false;
          lv_label_set_text(label, "停止");  
        }
        else if(btn2_flag == false)
        {
          btn2_flag = true;
          lv_label_set_text(label, "启动");  
        }
      
        Serial.printf("Toggled\n");
    }
    else if(obj == btn)
    {
        if(send_Data.e == false) 
        send_Data.e = true;
        else if(send_Data.e == true) 
        send_Data.e = false;

        // Serial.printf("dx\n");

        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    }
}

static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        static char buf[20]; /* max 3 bytes for number plus 1 null terminating byte */

        send_Data.b = lv_slider_get_value(slider);

        snprintf(buf, 20, "角度:%u", send_Data.b);
        lv_label_set_text(slider_label, buf);

        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
    }
}


void lv_ex_tileview_1(void)
{
    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style, LV_STATE_DEFAULT, &myFont);

    static lv_style_t led_style;
    lv_style_init(&led_style);
    lv_style_set_text_color(&led_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&led_style, LV_STATE_DEFAULT, &myLED_Font);

    static lv_style_t led7_style;
    lv_style_init(&led7_style);
    lv_style_set_text_color(&led7_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&led7_style, LV_STATE_DEFAULT, &dxLED7);

    // static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,0}, {1,1}};
    static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {0,2}, {1,0}, {1,1}, {1,2}};
    lv_obj_t *tileview;
    tileview = lv_tileview_create(lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(tileview, valid_pos, 6);
    lv_tileview_set_edge_flash(tileview, true);

    lv_obj_t* tile_0_0 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_0_1 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_0_2 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_1_0 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_1_1 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_1_2 = lv_obj_create( tileview, NULL);

    lv_obj_set_size( tile_0_0, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_0_1, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_0_2, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_0, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_1, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_2, LV_HOR_RES,LV_VER_RES);

    lv_obj_set_pos( tile_0_0, 0,   0);
    lv_obj_set_pos( tile_0_1, 0,   LV_VER_RES);
    lv_obj_set_pos( tile_0_2, 0,   2*LV_VER_RES);
    lv_obj_set_pos( tile_1_0, LV_HOR_RES,   0);
    lv_obj_set_pos( tile_1_1, LV_HOR_RES,   LV_VER_RES);
    lv_obj_set_pos( tile_1_2, LV_HOR_RES,   2*LV_VER_RES);

    lv_tileview_add_element(tileview, tile_0_0 );
    lv_tileview_add_element(tileview, tile_0_1 );
    lv_tileview_add_element(tileview, tile_0_2 );
    lv_tileview_add_element(tileview, tile_1_0 );
    lv_tileview_add_element(tileview, tile_1_1 );
    lv_tileview_add_element(tileview, tile_1_2 );

    lv_tileview_set_tile_act(tileview, 1, 2, LV_ANIM_OFF);

    //------------------------------tile_0_0-----------------------------------------------------
    btn = lv_btn_create(tile_0_0, NULL);
    lv_obj_set_event_cb(btn, event_handler);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *label_0_0 = lv_label_create( btn, NULL);
    // lv_obj_t *label_0_1 = lv_label_create( tile_0_1, NULL);
    lv_obj_t *label_1_1 = lv_label_create( tile_1_1, NULL);
    

    label_data = lv_label_create( tile_0_0, NULL);

    lv_obj_add_style(label_0_0, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_0_0, "0-中国"); 
    lv_obj_align( label_0_0, NULL, LV_ALIGN_CENTER,0,0);

    lv_obj_add_style(label_data, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text_fmt(label_data, "Value: %d", recv_Data.b);
    lv_obj_align( label_data, NULL, LV_ALIGN_CENTER,0,45);
    
    //------------------------------tile_0_1-----------------------------------------------------
    btn10 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn10, 60);
    lv_obj_set_event_cb(btn10, event_handler);
    lv_obj_align(btn10, NULL, LV_ALIGN_IN_TOP_MID, -30, 10);

    btn20 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn20, 60);
    lv_obj_set_event_cb(btn20, event_handler);
    lv_obj_align(btn20, btn10, LV_ALIGN_IN_TOP_MID, 65, 0);

    btn30 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn30, 60);
    lv_obj_set_event_cb(btn30, event_handler);
    lv_obj_align(btn30, NULL, LV_ALIGN_IN_TOP_MID, -30, 70);

    btn40 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn40, 60);
    lv_obj_set_event_cb(btn40, event_handler);
    lv_obj_align(btn40, btn30, LV_ALIGN_IN_TOP_MID, 65, 0);

    label = lv_label_create(btn10, NULL);
    lv_obj_add_style(label, LV_OBJ_PART_MAIN, &led_style);
    lv_label_set_text(label, "0");

    label = lv_label_create(btn20, NULL);
    lv_obj_add_style(label, LV_OBJ_PART_MAIN, &led_style);
    lv_label_set_text(label, "180");

    label = lv_label_create(btn30, NULL);
    lv_obj_add_style(label, LV_OBJ_PART_MAIN, &led_style);
    lv_label_set_text(label, "+");

    label = lv_label_create(btn40, NULL);
    lv_obj_add_style(label, LV_OBJ_PART_MAIN, &led_style);
    lv_label_set_text(label, "-");

    /* Create a slider in the center of the display */
    lv_obj_t * slider = lv_slider_create(tile_0_1, NULL);
    lv_obj_set_width(slider, LV_DPI * 1);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_slider_set_range(slider, 0, 180);
    
    /* Create a label below the slider */
    slider_label = lv_label_create(tile_0_1, NULL);
    lv_obj_add_style(slider_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(slider_label, "角度:0");
    lv_obj_set_auto_realign(slider_label, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_CENTER, 0, 15);

    // lv_obj_add_style(label_0_1, LV_OBJ_PART_MAIN, &model_style);
    // lv_label_set_text( label_0_1, "1-杭州"); 
    // lv_obj_align( label_0_1, NULL, LV_ALIGN_CENTER,0,0);

    //------------------------------tile_1_0-----------------------------------------------------
    lv_obj_t * img1 = lv_img_create(tile_1_0, NULL);
    lv_img_set_src(img1, &rich);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);

    //------------------------------tile_1_1-----------------------------------------------------
    lv_obj_add_style(label_1_1, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_1_1, "4-关闭"); 
    lv_obj_align( label_1_1, NULL, LV_ALIGN_CENTER,0,0);

    //------------------------------tile_0_2-----------------------------------------------------
    chart = lv_chart_create(tile_0_2, NULL);
    lv_obj_set_size(chart, 200, 150);
    lv_obj_align(chart, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    lv_chart_series_t * ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, LV_COLOR_GREEN);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 10);
    lv_chart_set_next(chart, ser1, 30);
    lv_chart_set_next(chart, ser1, 70);
    lv_chart_set_next(chart, ser1, 90);

    /*Directly set points on 'ser2'*/
    ser2->points[0] = 90;
    ser2->points[1] = 70;
    ser2->points[2] = 65;
    ser2->points[3] = 65;
    ser2->points[4] = 65;
    ser2->points[5] = 65;
    ser2->points[6] = 65;
    ser2->points[7] = 65;
    ser2->points[8] = 65;
    ser2->points[9] = 65;

    lv_chart_refresh(chart); /*Required after direct set*/

    //------------------------------tile_1_2-----------------------------------------------------
    lv_obj_t * img = lv_img_create(tile_1_2, NULL);
    lv_img_set_src(img, &TTGO_BG);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    label_time = lv_label_create( tile_1_2, NULL);
    lv_obj_add_style(label_time, LV_OBJ_PART_MAIN, &led7_style);
    lv_label_set_text( label_time, "10:23"); 
    lv_obj_align( label_time, NULL, LV_ALIGN_IN_TOP_LEFT,91,91);

    label_batt = lv_label_create( tile_1_2, NULL);
    lv_obj_add_style(label_batt, LV_OBJ_PART_MAIN, &led7_style);
    lv_label_set_text( label_batt, "NONE"); 
    lv_obj_align( label_batt, NULL, LV_ALIGN_IN_TOP_LEFT,91,125);

}


void lv_ex_keyboard_1(void)
{
    /*Create a keyboard and apply the styles*/
    lv_obj_t *kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_keyboard_set_cursor_manage(kb, true);

    /*Create a text area. The keyboard will write here*/
    lv_obj_t *ta = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, LV_DPI / 16);
    lv_textarea_set_text(ta, "");
    lv_coord_t max_h = LV_VER_RES / 2 - LV_DPI / 8;
    if(lv_obj_get_height(ta) > max_h) lv_obj_set_height(ta, max_h);

    /*Assign the text area to the keyboard*/
    lv_keyboard_set_textarea(kb, ta);
}


void lv_ex_img_1(void)
{
    // lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    // lv_img_set_src(img1, &me);
    // lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, -20);

    // lv_obj_t * img2 = lv_img_create(lv_scr_act(), NULL);
    // lv_img_set_src(img2, LV_SYMBOL_OK "Accept");
    // lv_obj_align(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

//@-配置电源管理irq
void setup_power_irq()
{
    //@-配置电源管理pin
    pinMode(AXP202_INT, INPUT_PULLUP);

    //@-挂载电源管理中断
    attachInterrupt(AXP202_INT, [] {irq_power = true;}, FALLING);

    // Must be enabled first, and then clear the interrupt status,
    // otherwise abnormal
    //@-允许电源管理中断
    power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ,true);

    //  Clear interrupt status
    //@-清空电源管理中断
    power->clearIRQ();
}

//@-配置3轴传感器irq
void setup_bma_irq()
{
     // Accel parameter structure
    Acfg cfg;
    /*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */

    // //@-配置touch硬件中断管脚
    // pinMode(BMA423_INT1, INPUT_PULLUP);
    // // attachInterrupt(BMA423_INT1, [] {
    // //     irq_bma423 = true;
    // // }, FALLING);
    // attachInterrupt(BMA423_INT1, [] {
    // irq_bma423 = true;
    // }, RISING);


    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    cfg.range = BMA4_ACCEL_RANGE_2G;
    /*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor_bma423->accelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    sensor_bma423->enableAccel();

    // Disable BMA423 isStepCounter feature
    sensor_bma423->enableFeature(BMA423_STEP_CNTR, false);
    // Enable BMA423 isTilt feature
    sensor_bma423->enableFeature(BMA423_TILT, false);
    // Enable BMA423 isDoubleClick feature -- 采用双击屏幕唤醒中断
    sensor_bma423->enableFeature(BMA423_WAKEUP, true);

    // Reset steps
    sensor_bma423->resetStepCounter();

    // Turn off feature interrupt
    // sensor_bma423->enableStepCountInterrupt();

//    sensor_bma423->enableTiltInterrupt();

    // It corresponds to isDoubleClick interrupt
    sensor_bma423->enableWakeupInterrupt();
}


//@-检测电源管理中断
void check_power_irq()
{
    //@-电源中断触发
    if(irq_power == true)
    {
        //@-清除电源中断
        power->clearIRQ();

        // Set screen and touch to sleep mode
        //@-设置屏幕及触摸功能进入sleep模式
        ttgo->displaySleep();

        //@-ttgo断电
        ttgo->powerOff();

        //GPIO_SEL_35为电源键 - 已测试ok
        //GPIO_SEL_38为touch中断 - 已测试ok
        //GPIO_SEL_39为3轴中断 - 已测试ok
        //@-配置ttgo唤醒模式
        // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ANY_HIGH);


        //@-ttgo深度sleep模式工作
        esp_deep_sleep_start();
    }
}

//@-检测3轴中断
void check_bma_irq()
{
    //@-3轴中断触发
    if(irq_bma423 == true)
    {
        Serial.println("check bma");
        //@-清除中断
        // sensor_bma423->clearIRQ();

        // Set screen and touch to sleep mode
        //@-设置屏幕及触摸功能进入sleep模式
        ttgo->displaySleep();

        //@-ttgo断电
        ttgo->powerOff();

        //GPIO_SEL_35为电源键 - 已测试ok
        //GPIO_SEL_38为touch中断 - 已测试ok
        //GPIO_SEL_39为3轴中断 - 已测试ok
        //@-配置ttgo唤醒模式
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ANY_HIGH);

        //@-ttgo深度sleep模式工作
        esp_deep_sleep_start();
    }
}

//@-检测触摸功能
void check_touch_pro()
{
    int16_t x, y;

    if(ttgo->getTouch(x, y))
    {
        System_Sleep_Tick = 0;
    }

    System_Sleep_Tick = System_Sleep_Tick + 1;

    //@-用户没有操作
    if(System_Sleep_Tick > 10000)
    {
        //@-设置屏幕及触摸功能进入sleep模式
        ttgo->displaySleep();

        //@-ttgo断电
        ttgo->powerOff();

        //GPIO_SEL_35为电源键 - 已测试ok
        //GPIO_SEL_38为touch中断 - 已测试ok
        //GPIO_SEL_39为3轴中断 - 已测试ok
        //@-配置ttgo唤醒模式
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ANY_HIGH);

        //@-ttgo深度sleep模式工作
        esp_deep_sleep_start();
    }
}

//@-配置打开audio功能
void Turn_On_Audio()
{
    // file = new AudioFileSourcePROGMEM(image, sizeof(image));
    file = new AudioFileSourcePROGMEM(door1, sizeof(door1));
    id3 = new AudioFileSourceID3(file);

    #if defined(STANDARD_BACKPLANE)
    out = new AudioOutputI2S(0, 1);
    #elif defined(EXTERNAL_DAC_BACKPLANE)
    out = new AudioOutputI2S();
    //External DAC decoding
    out->SetPinout(TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);
    #endif
    mp3 = new AudioGeneratorMP3();
    mp3->begin(id3, out);
}

//@-配置
void setup()
{
    //@-串口初始化
    Serial.begin(115200);

    //@-配置wifi的now功能
    WiFi.mode(WIFI_MODE_STA);
    // Serial.println(WiFi.macAddress());   //8C:AA:B5:82:EA:58

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Register peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
    }

    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(OnDataRecv);

    send_Data.e = false;

    //@-ttgo初始化
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();

    //!Turn on the audio power
    ttgo->enableLDO3();
    Turn_On_Audio();

    //@-ttgo初始化电源管理并开启电源监控
    power = ttgo->power;
    // ADC monitoring must be enabled to use the AXP202 monitoring function
    power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    setup_power_irq();

    //@-初始化3轴传感器
    sensor_bma423 = ttgo->bma;
    //@-配置3轴传感器BMA中断
    setup_bma_irq();

    //@-ttgo初始化lvgl库
    ttgo->lvgl_begin();

    //@-进入lvgl主页面
    lv_ex_tileview_1();
    // lv_ex_keyboard_1();
    // lv_ex_img_1();

}

//@-主循环
void loop()
{
    system_tick = system_tick + 1;

    //@-查询电源管理中断
    check_power_irq();

    //@-检测触摸功能
    check_touch_pro();

    if(system_tick > 100)
    {
        system_tick = 0;


        // PCF_TIMEFORMAT_HM,
        // PCF_TIMEFORMAT_HMS,
        //@-显示实时时间
        sprintf(display_buf, "%s", ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_HM));
        lv_label_set_text_fmt( label_time, "%s", display_buf); 

        //@-显示实时电量
        if (power->isChargeing() == false)
        {
            lv_label_set_text_fmt( label_batt, "Power:%d%%", power->getBattPercentage()); 
        }
        else
        {
            lv_label_set_text( label_batt, "Change..."); 
        }
        
    }

    lv_label_set_text_fmt(label_data, "Value: %d", recv_Data.b);

    lv_task_handler();

    delay(5);
}
