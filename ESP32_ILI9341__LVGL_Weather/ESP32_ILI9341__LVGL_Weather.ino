

#include <SPIFFS.h>
#include <FS.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>


#include <HTTPClient.h>
#include <esp_wifi.h>
#include <CircularBuffer.h>
#include <Arduino.h>

#include <math.h>
#include <lvgl.h>    //该应用使用lv_arduion库V3.0.1对等与LVGL Core 7.0.2
#include <TFT_eSPI.h>
#include <Ticker.h>






//@-配置用户字体
LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(dxLED7);
LV_FONT_DECLARE(dxLED7_60);
LV_FONT_DECLARE(myLED_Font);

#define PIN            21

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60


#define recv_start 10000
#define recv_avg   500
#define recv_less  50


//@-LVGL的系统tick
#define LVGL_TICK_PERIOD 60


TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

/* Add a simple drive to open images */
lv_fs_drv_t fs_drv;                          /*A driver descriptor*/
File f;

// char ssid[] = "wuyiyi";    //  your network SSID (name) 
// char pass[] = "10238831";   // your network password
char ssid[] = "DX_JS";    //  your network SSID (name) 
char pass[] = "dingxiao";   // your network password
// char ssid[] = "8879";    //  your network SSID (name) 
// char pass[] = "blackbug381";   // your network password


//@-wifi客户端
WiFiClient  client;

          
//-----------------------------------------------------
Ticker tick; /* timer for interrupt handler */

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
int screenWidth = 320;
int screenHeight = 240;


int tick1 = 0;

lv_obj_t * btn;
lv_obj_t * home_btn;

lv_obj_t * kb;
lv_obj_t * btn_dis;




//@-显示刷新函数
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  // uint16_t c;

  // tft.startWrite(); /* Start new TFT transaction */
  // tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  // for (int y = area->y1; y <= area->y2; y++) {
  //   for (int x = area->x1; x <= area->x2; x++) {
  //     c = color_p->full;
  //     tft.writeColor(c, 1);
  //     color_p++;
  //   }
  // }
  // tft.endWrite(); /* terminate TFT transaction */
  // lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors(&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);

}

/* Reading input device (simulated encoder here) */
bool read_encoder(lv_indev_drv_t * indev, lv_indev_data_t * data)
{
    static int32_t last_diff = 0;
    int32_t diff = 0; /* Dummy - no movement */
    int btn_state = LV_INDEV_STATE_REL; /* Dummy - no press */

    data->enc_diff = diff - last_diff;;
    data->state = btn_state;

    last_diff = diff;

    return false;
}


//@-触摸接口函数
bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    uint16_t touchX, touchY;

    bool touched = tft.getTouch(&touchX, &touchY, 600);

    if(!touched)
    {
      return false;
    }

    //@-触摸超出边界
    if(touchX>screenWidth || touchY > screenHeight)
    {
      // Serial.println("Y or y outside of expected parameters..");
      // Serial.print("y:");
      // Serial.print(touchX);
      // Serial.print(" x:");
      // Serial.print(touchY);
    }
    else
    {

      data->state = touched ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
  
      /*Save the state and save the pressed coordinate*/
      //if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&last_x, &last_y);
     
      /*Set the coordinates (if released use the last pressed coordinates)*/
      data->point.x = touchX;
      data->point.y = touchY;
  
      // Serial.print("Data x");
      // Serial.println(touchX);
      
      // Serial.print("Data y");
      // Serial.println(touchY);

    }

    return false; /*Return `false` because we are not buffering and no more data to read*/
}


/*-------------------------------------------------------------*/
void spiffs_drv_init()
{
  lv_fs_drv_init(&fs_drv);                     /*Basic initialization*/

  fs_drv.letter = 'D';                         /*An uppercase letter to identify the drive */
  fs_drv.file_size = sizeof(File);   /*Size required to store a file object*/
  fs_drv.open_cb = my_open_cb;                 /*Callback to open a file */
  fs_drv.close_cb = my_close_cb;               /*Callback to close a file */
  fs_drv.remove_cb = my_remove_cb;             /*Callback to remove a file */
  fs_drv.read_cb = my_read_cb;                 /*Callback to read a file */
  fs_drv.write_cb = my_write_cb;               /*Callback to write a file */
  fs_drv.seek_cb = my_seek_cb;                 /*Callback to seek in a file (Move cursor) */
  fs_drv.tell_cb = my_tell_cb;                 /*Callback to tell the cursor position  */

  lv_fs_drv_register(&fs_drv);                 /*Finally register the drive*/
}

lv_fs_res_t my_open_cb(lv_fs_drv_t *drv, void *file_p, const char *fn, lv_fs_mode_t mode)
{
  // (void) drv; /*Unused*/

  if (f) {
    return LV_FS_RES_OK;
  }

  char buf[100];
  sprintf(buf,"/%s",fn);
  Serial.println(buf);

  f = SPIFFS.open(buf, mode == LV_FS_MODE_WR ? FILE_WRITE : FILE_READ);

  if(!f || f.isDirectory()){
    return LV_FS_RES_UNKNOWN;
  } else{
    return LV_FS_RES_OK;
  }
}

// lv_fs_res_t my_open_cb(lv_fs_drv_t *drv, void *file_p, const char *fn, lv_fs_mode_t mode){
//   (void) drv; /*Unused*/
  
//   File *fp = file_p;
//   if (*fp) {
//     return LV_FS_RES_OK;
//   }

//   char buf[100];
//   sprintf(buf,"/%s",fn);

//   (*fp) = SPIFFS.open(buf, mode == LV_FS_MODE_WR ? FILE_WRITE : FILE_READ);

//   if(!(*fp) || (*fp).isDirectory()){
//     return LV_FS_RES_UNKNOWN;
//   } else{
//     return LV_FS_RES_OK;
//   }
// }

lv_fs_res_t my_close_cb(lv_fs_drv_t *drv, void *file_p)
{
  // (void) drv; /*Unused*/

  f.close();
  return LV_FS_RES_OK;
}

lv_fs_res_t my_read_cb(lv_fs_drv_t *drv, void *file_p, 
                        void *buf, uint32_t btr, uint32_t *br)
{
  // (void) drv; /*Unused*/

  *br = f.read((uint8_t*)buf, btr);
  Serial.println("READ");
  // Serial.println(btr);
  
  return LV_FS_RES_OK;
}

lv_fs_res_t my_write_cb(lv_fs_drv_t *drv, void *file_p, 
                        const void *buf, uint32_t btw, uint32_t *bw){
  // (void) drv; /*Unused*/

  *bw = f.write((const uint8_t*)buf, btw);
  return LV_FS_RES_OK;
}

lv_fs_res_t my_seek_cb(lv_fs_drv_t *drv, void *file_p, uint32_t pos){
  // (void) drv; /*Unused*/

  f.seek(pos);
  return LV_FS_RES_OK;
}

lv_fs_res_t my_tell_cb(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p){
  // (void) drv; /*Unused*/

  uint32_t tmp = f.position();
  pos_p = &tmp;
  
  return LV_FS_RES_OK;
}

lv_fs_res_t my_remove_cb(lv_fs_drv_t *drv, const char *fn){
  // (void) drv; /*Unused*/

  char buf[100];
  sprintf(buf,"/%s",fn);

  if(SPIFFS.remove(buf)){
    return LV_FS_RES_OK;
  } else{
    return LV_FS_RES_UNKNOWN;
  }
}
/*-------------------------------------------------------------------------------------*/

//@-LVGL驱动刷新tick
/* Interrupt driven periodic handler */
static void lv_tick_handler(void)
{
  lv_tick_inc(LVGL_TICK_PERIOD);
}


/*@--------------------彩屏天气预报控制台---------------------*/
void setup () 
{
    //@-初始化串口
    Serial.begin(115200);
    delay(500);

    //@-lvgl 初始化
    lv_init();
    delay(500);

    if(!SPIFFS.begin(true)){
      
      Serial.println("SPIFFS False！\n");
      while(true){
        delay(1000);
      }
    }
    spiffs_drv_init();

    //@-初始化SPI接口
    SPI.begin();

    //@------------------LVGL显示驱动初始化----------------
    //@-tft驱动初始化
    tft.begin(); /* TFT init */
    tft.setRotation(1);

    //@-设置touch参数
    uint16_t calData[5] = {555, 3151, 234, 3442, 1};
    tft.setTouch(calData);

    //@-初始化lv显存
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    //@-初始化显示驱动
    lv_disp_drv_t disp_drv;   
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    //@-初始化显示触摸驱动
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

    /*Initialize the (dummy) input device driver*/
    // lv_indev_drv_t indev_drv;
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.type = LV_INDEV_TYPE_ENCODER;
    // indev_drv.read_cb = read_encoder;
    // lv_indev_drv_register(&indev_drv);

    //@-设置显示tick
    tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

    //@-设置显示主题   
    dx_gui_init();
}


//@-主循环
void loop() 
{
    //@-tick1
    tick1 = tick1 + 1;

    //@-LVGL任务接口
    lv_task_handler(); /* let the GUI do its work */

    delay(1);
}

void btn_event_cb(lv_obj_t * obj, lv_event_t event)
{
  switch(event) 
  {
      case LV_EVENT_PRESSED:
            Serial.println("Pressed\n");
            break;
      case LV_EVENT_SHORT_CLICKED:
            Serial.println("Short clicked\n");
            break;
      case LV_EVENT_CLICKED:
            Serial.println("Clicked\n");
            break;
      case LV_EVENT_LONG_PRESSED:
            Serial.println("Long press\n");
            break;
      case LV_EVENT_LONG_PRESSED_REPEAT:
            Serial.println("Long press repeat\n");
            break;
      case LV_EVENT_RELEASED:
            Serial.println("Released\n");
            break;
  }
}

//@-初始化界面
void dx_gui_init()
{
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style, LV_STATE_DEFAULT, &myFont);


    /*Darken the button when pressed*/
    static lv_style_t style_imagebtn;
    lv_style_init(&style_imagebtn);
    lv_style_set_image_recolor_opa(&style_imagebtn, LV_STATE_PRESSED, LV_OPA_30);
    lv_style_set_image_recolor(&style_imagebtn, LV_STATE_PRESSED, LV_COLOR_BLACK);
    lv_style_set_text_color(&style_imagebtn, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    /*Create an Image button*/
    // lv_obj_t * imgbtn1 = lv_imgbtn_create(lv_scr_act(), NULL);
    lv_obj_t * imgbtn1 = lv_imgbtn_create(scr, NULL);
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_RELEASED, "D:/DX.bin");   //&imgbtn_green
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_PRESSED, "D:/DX.bin");    //&imgbtn_green
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_CHECKED_RELEASED, "D:/DX.bin");  //&imgbtn_blue
    lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_CHECKED_PRESSED, "D:/DX.bin");   //&imgbtn_blue
    lv_imgbtn_set_checkable(imgbtn1, true);
    lv_obj_add_style(imgbtn1, LV_IMGBTN_PART_MAIN, &style_imagebtn);
    lv_obj_align(imgbtn1, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Create a label on the Image button*/
    lv_obj_t * label = lv_label_create(imgbtn1, NULL);
    lv_label_set_text(label, "Button");


    // btn = lv_btn_create(scr, NULL);
    // // lv_obj_set_event_cb(btn, btn_event_cb);
    // lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 10);

    // lv_obj_t *label_0_0 = lv_label_create( btn, NULL);
    // lv_obj_add_style(label_0_0, LV_OBJ_PART_MAIN, &model_style);
    // lv_label_set_text( label_0_0, "0-中国"); 
    // lv_obj_align( label_0_0, NULL, LV_ALIGN_CENTER,0,0);

}

