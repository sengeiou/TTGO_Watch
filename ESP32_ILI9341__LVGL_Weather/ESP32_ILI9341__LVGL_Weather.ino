

#include <SPIFFS.h>
#include <FS.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <lvgl.h>    //该应用使用lv_arduion库V3.0.1对等与LVGL Core 7.0.2
#include <TFT_eSPI.h>
#include <Ticker.h>






//@-配置用户字体
LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(dxLED7);
LV_FONT_DECLARE(dxLED7_60);
LV_FONT_DECLARE(myLED_Font);


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
lv_obj_t * img1;
lv_obj_t * label2;

int img_flag = 0;
int img_do = 0;


int wifi_connect_tick = 0;
int wifi_connect_time = 60;  //默认wifi连接时间


//@-网络授时
// const char* ntpServer = "pool.ntp.org";   ----->ok
// const char* ntpServer = "cn.pool.ntp.org";  ---->OK
// const char* ntpServer = "time.pool.aliyun.com";   ---->OK
const char* ntpServer = "ntp.aliyun.com";

//@-设置时区参数
const long  gmtOffset_sec = 7 * 3600;
const int   daylightOffset_sec = 3600;

int Dev_SystemTime_Year = 2021;
int Dev_SystemTime_Month = 8;
int Dev_SystemTime_Day = 13;
int Dev_SystemTime_WeekOfDay = 1;
int Dev_SystemTime_Hour = 10;
int Dev_SystemTime_Minute = 23;
int Dev_SystemTime_Second = 0;

hw_timer_t *time1 = NULL;
int tim1_IRQ_count = 0;

int AutoTime_Flag = 0;


//----------------------------------------------------------------
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
  // Serial.println("READ");
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


void tim1Interrupt()
{
  // Serial.println("haha");
  // tim1_IRQ_count++;
  Dev_SystemTime_Second = Dev_SystemTime_Second + 1;
  timerAlarmEnabled(time1);
  // Serial.println(timerAlarmEnabled(time1));
}


/*@--------------------彩屏天气预报控制台---------------------*/
void setup () 
{
    //@-初始化串口
    Serial.begin(115200);
    delay(500);

    WIFI_Connect();
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

    time1 = timerBegin(0, 80, true);
    timerAttachInterrupt(time1, tim1Interrupt, true);
    timerAlarmWrite(time1, 1000000, true);
    timerAlarmEnable(time1);

    //@-设置显示主题   
    dx_gui_init();
}


//@-连接wifi
void WIFI_Connect()
{
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  // Serial.println(ssid);
  // WiFi.begin(ssid.c_str(), password.c_str());
  // WiFi.begin("wuyiyi", "dingxiao");
  WiFi.begin("DX_JS", "dingxiao");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    wifi_connect_tick = wifi_connect_tick + 1;
    if(wifi_connect_tick > wifi_connect_time)
    {
      wifi_connect_tick = 0;
      break;
    }
  }
  if(WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected.");
  
    // Init and get the time
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTime();

    AutoTime_Flag = 0;
  }

    //@-断开wifi链接
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi Disconnected");
}

//@-打印实时时间
void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

  // tm_sec	int	seconds after the minute	0-60*
  // tm_min	int	minutes after the hour	0-59
  // tm_hour	int	hours since midnight	0-23
  // tm_mday	int	day of the month	1-31
  // tm_mon	int	months since January	0-11
  // tm_year	int	years since 1900	
  // tm_wday	int	days since Sunday	0-6

  // Serial.println("----------dx--------------");
  // Serial.println(timeinfo.tm_year);
  // Serial.println(timeinfo.tm_mon);
  // Serial.println(timeinfo.tm_mday);
  // Serial.println(timeinfo.tm_wday);
  // Serial.println(timeinfo.tm_hour);
  // Serial.println(timeinfo.tm_min);
  // Serial.println(timeinfo.tm_sec);


  Dev_SystemTime_Year = timeinfo.tm_year+1900;
  Dev_SystemTime_Month = timeinfo.tm_mon+1;
  Dev_SystemTime_Day = timeinfo.tm_mday;
  Dev_SystemTime_WeekOfDay = timeinfo.tm_wday;
  Dev_SystemTime_Hour = timeinfo.tm_hour;
  Dev_SystemTime_Minute = timeinfo.tm_min;
  Dev_SystemTime_Second = timeinfo.tm_sec;

  Serial.println("----------dx--------------");
  Serial.println(Dev_SystemTime_Year);
  Serial.println(Dev_SystemTime_Month);
  Serial.println(Dev_SystemTime_Day);
  Serial.println(Dev_SystemTime_WeekOfDay);
  Serial.println(Dev_SystemTime_Hour);
  Serial.println(Dev_SystemTime_Minute);
  Serial.println(Dev_SystemTime_Second);

}


//@-主循环
void loop() 
{
    char temp_str[256];

    //@-tick1
    tick1 = tick1 + 1;

    //@-自动对时
    if(AutoTime_Flag == 1)
    {
        WIFI_Connect();
    }

    if(tick1 > 500)
    {
      tick1 = 0;
      // Serial.println("tick");

      if(img_do == 1)
      {
        img_do = 0;
        if(img_flag == 1)
        lv_img_set_src(img1, "D:/pic2.bin");
        else if(img_flag == 0)
        lv_img_set_src(img1, "D:/me.bin");
      }

      //@-自动对时
      if((Dev_SystemTime_Minute == 0) || (Dev_SystemTime_Minute == 10) || (Dev_SystemTime_Minute == 20) ||
         (Dev_SystemTime_Minute == 30) || (Dev_SystemTime_Minute == 40) || (Dev_SystemTime_Minute == 50))
      {
          AutoTime_Flag = 1;
      }
      
      sprintf(temp_str, "%2d-%2d-%2d", Dev_SystemTime_Hour, Dev_SystemTime_Minute, Dev_SystemTime_Second);
      lv_label_set_text(label2, temp_str);

      if(Dev_SystemTime_Second >= 59)
      {
        Dev_SystemTime_Second = 0;
        Dev_SystemTime_Minute = Dev_SystemTime_Minute + 1;
        if(Dev_SystemTime_Minute >= 59)
        {
          Dev_SystemTime_Minute = 0;
          Dev_SystemTime_Hour = Dev_SystemTime_Hour + 1;
          if(Dev_SystemTime_Hour > 23)
          Dev_SystemTime_Hour = 0;
        }
      }
    }

    //@-LVGL任务接口
    lv_task_handler(); /* let the GUI do its work */

    delay(1);
}

void btn_event_cb(lv_obj_t * obj, lv_event_t event)
{
  switch(event) 
  {
      case LV_EVENT_PRESSED:
            // Serial.println("Pressed\n");
            if(obj == btn)
            {
                if(img_flag == 0)
                {
                  img_flag = 1;
                  img_do = 1;
                  Serial.println("pic2.bin");
                }
                else if(img_flag == 1)
                {
                  img_flag = 0;
                  img_do = 1;
                  Serial.println("me.bin");
                }
            }
            break;
      // case LV_EVENT_SHORT_CLICKED:
      //       Serial.println("Short clicked\n");
      //       break;
      // case LV_EVENT_CLICKED:
      //       Serial.println("Clicked\n");
      //       break;
      // case LV_EVENT_LONG_PRESSED:
      //       Serial.println("Long press\n");
      //       break;
      // case LV_EVENT_LONG_PRESSED_REPEAT:
      //       Serial.println("Long press repeat\n");
      //       break;
      // case LV_EVENT_RELEASED:
      //       Serial.println("Released\n");
      //       break;
  }
}

//@-初始化界面
void dx_gui_init()
{
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);


    // LV_FONT_DECLARE(myFont);
    // LV_FONT_DECLARE(dxLED7);
    // LV_FONT_DECLARE(dxLED7_60);
    // LV_FONT_DECLARE(myLED_Font);

    static lv_style_t model_style;
    lv_style_init(&model_style);
    lv_style_set_text_color(&model_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style, LV_STATE_DEFAULT, &myFont);

    static lv_style_t model_style1;
    lv_style_init(&model_style1);
    lv_style_set_text_color(&model_style1, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&model_style1, LV_STATE_DEFAULT, &dxLED7_60);


    /*Darken the button when pressed*/
    // static lv_style_t style_imagebtn;
    // lv_style_init(&style_imagebtn);
    // lv_style_set_image_recolor_opa(&style_imagebtn, LV_STATE_PRESSED, LV_OPA_30);
    // lv_style_set_image_recolor(&style_imagebtn, LV_STATE_PRESSED, LV_COLOR_BLACK);
    // lv_style_set_text_color(&style_imagebtn, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    /*Create an Image button---*/
    // // lv_obj_t * imgbtn1 = lv_imgbtn_create(lv_scr_act(), NULL);
    // lv_obj_t * imgbtn1 = lv_imgbtn_create(scr, NULL);
    // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_RELEASED, "D:/me.bin");   //&imgbtn_green
    // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_PRESSED, "D:/me.bin");    //&imgbtn_green
    // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_CHECKED_RELEASED, "D:/me.bin");  //&imgbtn_blue
    // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_CHECKED_PRESSED, "D:/me.bin");   //&imgbtn_blue

    // // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_RELEASED, "D:/me1.png");   //&imgbtn_green
    // // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_PRESSED, "D:/me1.png");    //&imgbtn_green
    // // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_CHECKED_RELEASED, "D:/me1.png");  //&imgbtn_blue
    // // lv_imgbtn_set_src(imgbtn1, LV_BTN_STATE_CHECKED_PRESSED, "D:/me1.png");   //&imgbtn_blue
    
    // lv_imgbtn_set_checkable(imgbtn1, true);
    // lv_obj_add_style(imgbtn1, LV_IMGBTN_PART_MAIN, &style_imagebtn);
    // lv_obj_align(imgbtn1, NULL, LV_ALIGN_CENTER, 0, 0);

    // /*Create a label on the Image button*/
    // lv_obj_t * label = lv_label_create(imgbtn1, NULL);
    // lv_label_set_text(label, "Button");


    img1 = lv_img_create(scr, NULL);
    lv_img_set_src(img1, "D:/310.bin");
    lv_obj_align(img1, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);


    btn = lv_btn_create(scr, NULL);
    lv_obj_set_event_cb(btn, btn_event_cb);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *label_0_0 = lv_label_create( btn, NULL);
    lv_obj_add_style(label_0_0, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_0_0, "0-中国"); 
    lv_obj_align( label_0_0, NULL, LV_ALIGN_CENTER,0,0);

    lv_obj_t * label1 = lv_label_create(scr, NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK); /*Break the long lines*/
    lv_label_set_recolor(label1, true); /*Enable re-coloring by␣
    ,!commands in the text*/
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER); /*Center aligned lines*/
    lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label "
    "and wrap long text automatically.");
    lv_obj_set_width(label1, 150);
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 60);

    label2 = lv_label_create(scr, NULL);
    lv_obj_add_style(label2, LV_OBJ_PART_MAIN, &model_style1);
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SROLL_CIRC); /*Circular scroll*/
    lv_obj_set_width(label2, 320);
    lv_label_set_text(label2, "It is a circularly scrolling text. ");
    lv_obj_align(label2, NULL, LV_ALIGN_CENTER, 0, 100);

}

