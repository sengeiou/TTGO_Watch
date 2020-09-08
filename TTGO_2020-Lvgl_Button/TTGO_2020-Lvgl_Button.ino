
#include "config.h"

#include <esp_now.h>
#include <WiFi.h>



LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(dxLED7);
LV_FONT_DECLARE(myLED_Font);

/* Find the image here: https://github.com/lvgl/lv_examples/tree/master/assets */
LV_IMG_DECLARE(me);
LV_IMG_DECLARE(TTGO_BG);

TTGOClass *ttgo;

lv_obj_t * btn;
lv_obj_t *btn2;

lv_obj_t *btn10;
lv_obj_t *btn20;
lv_obj_t *btn30;
lv_obj_t *btn40;

lv_obj_t *label;
lv_obj_t *label1; 

lv_obj_t *label_data;

/*Create a chart*/
lv_obj_t * chart;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_label;

bool btn2_flag = true;


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

// callback function that will be executed when data is received
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

    //------------------------------tile_0_0-----------------------------------------------------
    btn = lv_btn_create(tile_0_0, NULL);
    lv_obj_set_event_cb(btn, event_handler);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *label_0_0 = lv_label_create( btn, NULL);
    // lv_obj_t *label_0_1 = lv_label_create( tile_0_1, NULL);
    lv_obj_t *label_1_0 = lv_label_create( tile_1_0, NULL);
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
    lv_obj_add_style(label_1_0, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_1_0, "3-启动"); 
    lv_obj_align( label_1_0, NULL, LV_ALIGN_CENTER,0,0);

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

    lv_obj_t *label_1_2 = lv_label_create( tile_1_2, NULL);
    lv_obj_add_style(label_1_2, LV_OBJ_PART_MAIN, &led7_style);
    lv_label_set_text( label_1_2, "10:23"); 
    lv_obj_align( label_1_2, NULL, LV_ALIGN_IN_TOP_LEFT,75,106);

    

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
    lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &me);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, -20);

    lv_obj_t * img2 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img2, LV_SYMBOL_OK "Accept");
    lv_obj_align(img2, img1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

void setup()
{
    Serial.begin(115200);

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

    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->lvgl_begin();

    #if 0
    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style, LV_STATE_DEFAULT, &myFont);

    btn10 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn10, event_handler);
    lv_obj_align(btn10, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    label = lv_label_create(btn10, NULL);
    lv_obj_add_style(label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label, "中国");

    btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn2, event_handler);
    lv_obj_align(btn2, NULL, LV_ALIGN_IN_TOP_MID, 0, 65);
    lv_btn_set_checkable(btn2, true);
    lv_btn_toggle(btn2);
    lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

    label = lv_label_create(btn2, NULL);
    lv_obj_add_style(label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label, "启动");


    /* Create a slider in the center of the display */
    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_width(slider, LV_DPI * 1);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_slider_set_range(slider, 0, 100);
    
    /* Create a label below the slider */
    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(slider_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(slider_label, "角度:0");
    lv_obj_set_auto_realign(slider_label, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_CENTER, 0, 15);
    #endif

    lv_ex_tileview_1();
    // lv_ex_keyboard_1();
    // lv_ex_img_1();

}

void loop()
{
    lv_task_handler();

    lv_label_set_text_fmt(label_data, "Value: %d", recv_Data.b);

    delay(5);
}
