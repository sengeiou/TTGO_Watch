
#include "config.h"

#include <esp_now.h>
#include <WiFi.h>

LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(myLED_Font);

TTGOClass *ttgo;

lv_obj_t * btn;
lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *label;
lv_obj_t *label1;


lv_obj_t *label_data;

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
    if (obj == btn1) {
        Serial.printf("Clicked\n");
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
        snprintf(buf, 20, "角度:%u", lv_slider_get_value(slider));
        lv_label_set_text(slider_label, buf);
    }
}


void lv_ex_tileview_1(void)
{
    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style, LV_STATE_DEFAULT, &myFont);

    // static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,0}, {1,1}};
    static lv_point_t valid_pos[] = {{0,0}, {1, 0}, {0,1}, {1,1}};
    lv_obj_t *tileview;
    tileview = lv_tileview_create(lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(tileview, valid_pos, 4);
    lv_tileview_set_edge_flash(tileview, true);

    lv_obj_t* tile_0_0 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_0_1 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_1_0 = lv_obj_create( tileview, NULL);
    lv_obj_t* tile_1_1 = lv_obj_create( tileview, NULL);

    lv_obj_set_size( tile_0_0, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_0_1, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_0, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_1, LV_HOR_RES,LV_VER_RES);

    lv_obj_set_pos( tile_0_0, 0,   0);
    lv_obj_set_pos( tile_0_1, 0,   LV_VER_RES);
    lv_obj_set_pos( tile_1_0, LV_HOR_RES,   0);
    lv_obj_set_pos( tile_1_1, LV_HOR_RES,   LV_VER_RES);

    lv_tileview_add_element(tileview, tile_0_0 );
    lv_tileview_add_element(tileview, tile_0_1 );
    lv_tileview_add_element(tileview, tile_1_0 );
    lv_tileview_add_element(tileview, tile_1_1 );

    btn = lv_btn_create(tile_0_0, NULL);
    lv_obj_set_event_cb(btn, event_handler);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *label_0_0 = lv_label_create( btn, NULL);
    lv_obj_t *label_0_1 = lv_label_create( tile_0_1, NULL);
    lv_obj_t *label_1_0 = lv_label_create( tile_1_0, NULL);
    lv_obj_t *label_1_1 = lv_label_create( tile_1_1, NULL);

    label_data = lv_label_create( tile_0_0, NULL);

    lv_obj_add_style(label_0_0, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_0_0, "0-中国"); 
    lv_obj_align( label_0_0, NULL, LV_ALIGN_CENTER,0,0);

    lv_obj_add_style(label_data, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text_fmt(label_data, "Value: %d", recv_Data.b);
    lv_obj_align( label_data, NULL, LV_ALIGN_CENTER,0,45);
    

    lv_obj_add_style(label_0_1, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_0_1, "1-杭州"); 
    lv_obj_align( label_0_1, NULL, LV_ALIGN_CENTER,0,0);

    lv_obj_add_style(label_1_0, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_1_0, "3-启动"); 
    lv_obj_align( label_1_0, NULL, LV_ALIGN_CENTER,0,0);

    lv_obj_add_style(label_1_1, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_1_1, "4-关闭"); 
    lv_obj_align( label_1_1, NULL, LV_ALIGN_CENTER,0,0);

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

    btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, event_handler);
    lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    label = lv_label_create(btn1, NULL);
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

}

void loop()
{
    lv_task_handler();

    lv_label_set_text_fmt(label_data, "Value: %d", recv_Data.b);

    delay(5);
}
