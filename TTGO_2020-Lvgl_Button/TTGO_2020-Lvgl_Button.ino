
#include "config.h"

LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(myLED_Font);

TTGOClass *ttgo;

lv_obj_t *btn1;
lv_obj_t *btn2;
lv_obj_t *label;
lv_obj_t *label1;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_label;

bool btn2_flag = true;
    

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (obj == btn1) {
        Serial.printf("Clicked\n");
    } else if (obj == btn2) {

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
    // static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,0}, {1,1}};
    static lv_point_t valid_pos[] = {{0,0}, {1, 0}, {0,1}, {1,1}};
    lv_obj_t *tileview;
    tileview = lv_tileview_create(lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(tileview, valid_pos, 4);
    lv_tileview_set_edge_flash(tileview, true);

    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style, LV_STATE_DEFAULT, &myFont);

    /*Tile1: just a label*/
    lv_obj_t * tile1 = lv_obj_create(tileview, NULL);
    lv_obj_set_size(tile1, LV_HOR_RES, LV_VER_RES);
    lv_tileview_add_element(tileview, tile1);

    // lv_obj_t * label = lv_label_create(tile1, NULL);
    // lv_obj_add_style(label, LV_OBJ_PART_MAIN, &model_style);
    // lv_label_set_text(label, "中国");
    // lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    
    /*Create a line meter */
    lv_obj_t * lmeter;
    lmeter = lv_linemeter_create(tile1, NULL);
    lv_linemeter_set_range(lmeter, 0, 100);                   /*Set the range*/
    lv_linemeter_set_value(lmeter, 80);                       /*Set the current value*/
    lv_linemeter_set_scale(lmeter, 240, 21);                  /*Set the angle and number of lines*/
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Tile2: a list*/
    lv_obj_t * list = lv_list_create(tileview, NULL);
    lv_obj_add_style(list, LV_OBJ_PART_MAIN, &model_style);
    lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_pos(list, 0, LV_VER_RES);
    lv_list_set_scroll_propagation(list, true);
    lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    lv_list_add_btn(list, NULL, "启动");
    lv_list_add_btn(list, NULL, "Two");
    lv_list_add_btn(list, NULL, "Three");
    lv_list_add_btn(list, NULL, "Four");
    lv_list_add_btn(list, NULL, "Five");
    lv_list_add_btn(list, NULL, "Six");
    lv_list_add_btn(list, NULL, "Seven");
    lv_list_add_btn(list, NULL, "Eight");

    /*Tile3: a button*/
    lv_obj_t * tile3 = lv_obj_create(tileview, tile1);
    lv_obj_set_pos(tile3, LV_HOR_RES, LV_VER_RES);
    lv_tileview_add_element(tileview, tile3);

    lv_obj_t * label3 = lv_label_create(tile3, NULL);
    lv_obj_add_style(label3, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label3, "杭州");
    lv_obj_align(label3, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Tile4: a button*/
    lv_obj_t * tile4 = lv_obj_create(tileview, tile3);
    lv_obj_set_pos(tile4, LV_HOR_RES, LV_VER_RES);
    lv_tileview_add_element(tileview, tile4);

    lv_obj_t * btn1 = lv_btn_create(tile4, NULL);
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_tileview_add_element(tileview, btn1);
    label1 = lv_label_create(btn1, NULL);
    lv_obj_add_style(label1, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label1, "旋回角度");
}

void setup()
{
    Serial.begin(115200);
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

}

void loop()
{
    lv_task_handler();
    delay(5);
}
