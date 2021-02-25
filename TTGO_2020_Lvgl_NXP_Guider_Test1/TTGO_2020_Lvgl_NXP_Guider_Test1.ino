
//@-ttgo配置
#include "config.h"

#include <stdio.h>
#include "gui_guider.h"




//@-TTGO
TTGOClass *ttgo;



// typedef struct
// {
// 	lv_obj_t *screen;
// 	lv_obj_t *screen_img0;
// 	lv_obj_t *screen_label1;
// }lv_ui;

lv_ui *guider_ui1;

lv_ui guider_ui2;

// void setup_ui(lv_ui *ui){
// 	setup_scr_screen(ui);
// 	// lv_scr_load(ui->screen);
// }




void test1(lv_ui *ui)
{
   //Write codes screen
	ui->screen = lv_obj_create(NULL, NULL);

	//Write codes screen_img0
	ui->screen_img0 = lv_img_create(ui->screen, NULL);

	//Write style LV_IMG_PART_MAIN for screen_img0
	static lv_style_t style_screen_img0_main;
	lv_style_init(&style_screen_img0_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_img0_main
	lv_style_set_image_recolor(&style_screen_img0_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_image_recolor_opa(&style_screen_img0_main, LV_STATE_DEFAULT, 0);
	lv_style_set_image_opa(&style_screen_img0_main, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(ui->screen_img0, LV_IMG_PART_MAIN, &style_screen_img0_main);
	lv_obj_set_pos(ui->screen_img0, 43, 28);
	lv_obj_set_size(ui->screen_img0, 150, 130);
	lv_obj_set_click(ui->screen_img0, true);
	lv_img_set_src(ui->screen_img0,&_pic1_150x130);
	lv_img_set_pivot(ui->screen_img0, 0,0);
	lv_img_set_angle(ui->screen_img0, 0);

	//Write codes screen_label1
	ui->screen_label1 = lv_label_create(ui->screen, NULL);
	lv_label_set_text(ui->screen_label1, "Happy");
	lv_label_set_long_mode(ui->screen_label1, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_label1, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_label1
	static lv_style_t style_screen_label1_main;
	lv_style_init(&style_screen_label1_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_label1_main
	lv_style_set_radius(&style_screen_label1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_label1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_label1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_label1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label1_main, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_screen_label1_main, LV_STATE_DEFAULT, lv_color_make(0x35, 0xe3, 0xcf));
	lv_style_set_text_font(&style_screen_label1_main, LV_STATE_DEFAULT, &lv_font_simsun_12);
	lv_style_set_text_letter_space(&style_screen_label1_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_label1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_label1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_label1_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_label1_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_label1, LV_LABEL_PART_MAIN, &style_screen_label1_main);
	lv_obj_set_pos(ui->screen_label1, 65, 185);
	lv_obj_set_size(ui->screen_label1, 100, 0);

	//Write codes screen_label1_2
	ui->screen_label1_2 = lv_label_create(ui->screen, NULL);
	lv_label_set_text(ui->screen_label1_2, "DING XIAO");
	lv_label_set_long_mode(ui->screen_label1_2, LV_LABEL_LONG_BREAK);
	lv_label_set_align(ui->screen_label1_2, LV_LABEL_ALIGN_CENTER);

	//Write style LV_LABEL_PART_MAIN for screen_label1_2
	static lv_style_t style_screen_label1_2_main;
	lv_style_init(&style_screen_label1_2_main);

	//Write style state: LV_STATE_DEFAULT for style_screen_label1_2_main
	lv_style_set_radius(&style_screen_label1_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_screen_label1_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_color(&style_screen_label1_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_grad_dir(&style_screen_label1_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen_label1_2_main, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_screen_label1_2_main, LV_STATE_DEFAULT, lv_color_make(0x05, 0x05, 0x05));
	lv_style_set_text_font(&style_screen_label1_2_main, LV_STATE_DEFAULT, &lv_font_simsun_12);
	lv_style_set_text_letter_space(&style_screen_label1_2_main, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_screen_label1_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_screen_label1_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_screen_label1_2_main, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_screen_label1_2_main, LV_STATE_DEFAULT, 0);
	lv_obj_add_style(ui->screen_label1_2, LV_LABEL_PART_MAIN, &style_screen_label1_2_main);
	lv_obj_set_pos(ui->screen_label1_2, 67, 213);
	lv_obj_set_size(ui->screen_label1_2, 100, 0);

    lv_scr_load(ui->screen);
}


//@-配置
void setup()
{
    //@-串口初始化
    Serial.begin(115200);

    //@-ttgo初始化
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    // ttgo->motor_begin();
    // ttgo->motor->onec();
    ttgo->bl->adjust( 200 );

    //@-ttgo初始化lvgl库
    ttgo->lvgl_begin();

    // setup_ui(guider_ui1);
    test1(&guider_ui2);
}

//@-主循环
void loop()
{
    lv_task_handler();

    delay(5);
}