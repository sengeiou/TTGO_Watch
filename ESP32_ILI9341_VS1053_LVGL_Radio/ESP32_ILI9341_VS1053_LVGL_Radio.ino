#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <lvgl.h>   //@-使用lv_arduino2.1.4版本，新版3.0.1中文字库调整太麻烦---

// This ESP_VS1053_Library
#include "VS1053.h"

#include <Preferences.h>  //For reading and writing into the ROM memory
Preferences preferences;
unsigned int old_counter,new_counter;
unsigned int counter = 0;
// #include "helloMp3.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <CircularBuffer.h>
#include <Arduino.h>

#include <math.h>
#include <lvgl.h>
#include <Ticker.h>
#include <TFT_eSPI.h>

//--------------------------------------
#include <Adafruit_NeoPixel.h>

#define PIN            21

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      60



#define recv_start 10000
#define recv_avg   500
#define recv_less  50


//@-LVGL的系统tick
#define LVGL_TICK_PERIOD 60

// char ssid[] = "wuyiyi";    //  your network SSID (name) 
// char pass[] = "10238831";   // your network password
char ssid[] = "DX_JS";    //  your network SSID (name) 
char pass[] = "dingxiao";   // your network password
// char ssid[] = "8879";    //  your network SSID (name) 
// char pass[] = "blackbug381";   // your network password


char *host[10] = { 
                  // "http://bbcmedia.ic.llnwd.net", /*FQ可以*/
                  "123.56.16.201",
                  "bbcwssc.ic.llnwd.net",
                  "192.99.8.192",

                  "icecast.omroep.nl",
                  "airspectrum.cdnstream1.com", //good
                  "mp3stream4.abradio.cz", 
                  "ic5.radiomonster.fm",
                  "media-ice.musicradio.com",  //best
                  "gbradio.cdn.tibus.net",
                  "138.201.248.93"
                };

                // http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk
                // http://bbcwssc.ic.llnwd.net/stream/bbcwssc_mp1_ws-eieuk

char *path[10] = {
                  // "/stream/bbcmedia_radio1_mf_p",
                  "/live/fm876/96K/tzwj_video/stream",
                  "/stream/bbcwssc_mp1_ws-eieuk",
                  "/stream",

                  "/radio1-bb-mp3",
                  "/1648_128",
                  "/depeche128.mp3",
                  "/tophits-320",
                  "/CapitalWalesCoastMP3",
                  "/SWAWave",
                  "/lovehits"
                };

int   port[10] = {
                   1953,
                   80,
                   3208,


                   80,
                   8114,
                   80,
                   80,
                   80,
                   80,
                   80
                };

char *sname[10] = {
                       "Beijing Wenyi",
                       "BBC Radio 1 FM",
                       "Radio-Beatle -unknow",
   
                       "Icecast", // New Zeland
                       "Airspectrum",//Spain
                       "AB Fm",// France
                       "Ic5-English",//Swedish English
                       "Capital",// UK
                       "The Wave",//Australia
                       "LoveHits"//Germany
                };

int status = WL_IDLE_STATUS;
WiFiClient  client;
uint8_t mp3buff[128];   // vs1053 likes 32 bytes at a time
uint8_t playbuff[32];   // vs1053 likes 32 bytes at a time

CircularBuffer<uint8_t, 32 * 500> dx_buffer;
            

// Wiring of VS1053 board (SPI connected in a standard way)
#define VS1053_CS    32 //32
#define VS1053_DCS   33  //33
#define VS1053_DREQ  35 //15

// 0 // volume level 0-100
VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);

int VOLUME = 90;


int index1 = 0;
int index2 = 0;
bool play_flag = false;
int system_tick = 0;
bool play_lock = false;

int buff_size = 0;
int buff_size_copy = 0;
int data_stop_tick = 0;

int radio_cannel = 0;
int radio_cannel_copy = 0;

bool cannel_switch = false;

//-----------------------------------------------------
Ticker tick; /* timer for interrupt handler */
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

lv_obj_t * slider_label;
int screenWidth = 320;
int screenHeight = 240;
//-----------------------------------------
uint32_t user_data = 10;
lv_task_t * dx_task;

lv_obj_t * chart;
lv_chart_series_t * ser1;
lv_chart_series_t * ser2;
int tick1 = 0;

float chart_data1 = 0;
float chart_data2 = 0;
float sin_time = 0;

lv_obj_t * led1;
bool LED_flag = false;

lv_obj_t * main_label;

int DX_lv_Index = 0;

lv_obj_t *mbox, *info;

// char welcome_info[] = "杭州 to the modal message box demo!\n"
//                                    "Press the button to display a message box.";
char welcome_info[] = "杭州 to the modal message box demo!";

char in_msg_info[] = "中国 that you cannot touch the button again while the message box is open.";


lv_obj_t * kb;

lv_obj_t * btn_dis;
lv_obj_t * btn_wifi_back;
lv_obj_t * btn_channle_plus;
lv_obj_t * btn_channle_minus;
lv_obj_t * btn_volum_pluse;
lv_obj_t * btn_volum_minus;
lv_obj_t * label_cannel;
lv_obj_t * label_buffer;
lv_obj_t * label_vloum;

//@13-多任务
TaskHandle_t Task_radio;


//-------------------------------------
lv_obj_t * wifi_kb;
lv_obj_t * list1;
lv_obj_t * list_btn[10];
lv_obj_t * list_btn_dx_down;
lv_obj_t * list_btn_dx_up;
lv_obj_t * list_btn_dx_back;
lv_obj_t * btn_dx_wifi_scan;
lv_obj_t * btn_dx_radio;
lv_obj_t * pwd_ta1;
lv_obj_t * wifi_connect_btn_back;

lv_obj_t * colorPicker;

bool task_start_flag = false;
int wifi_scan_num;
int wifi_connect_index = 0;

struct DX_WIFI_S{
    String WIFI_SSID;
    String WIFI_RSSI;
    String WIFI_TYPE;
    String WIFI_PASS;
};

struct Connect_WIFI_Status_S{
    int WIFI_Connect_Status;    //0:Not Scan 1:Scaning 
    int WIFI_Status;            //0:Not Connect 1:Connecting 2:Connected
    String WIFI_Connect_IP;
};

DX_WIFI_S DX_WIFI[20]; 
Connect_WIFI_Status_S Connect_Wifi_Status;

int Radio_Mode = 0;  //@-进入radio模式  0：没有  1：进入

const lv_coord_t pickerSize = 200;


//---------------------------------------------------------
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int show_red = 0;
int show_green = 0;
int show_blue = 0;


//@-显示刷新函数
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint16_t c;

  tft.startWrite(); /* Start new TFT transaction */
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); /* terminate TFT transaction */
  lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
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


//@-LVGL驱动刷新tick
/* Interrupt driven periodic handler */
static void lv_tick_handler(void)
{
  lv_tick_inc(LVGL_TICK_PERIOD);
}



//@-初始化
void setup () 
{
    //@-initialize SPI
    Serial.begin(115200);
    delay(500);

    //@-lvgl init
    lv_init();
    delay(500);

    //@-initialize SPI bus;
    SPI.begin();

    //@-vs1053 init
    // player.begin();
    // player.switchToMp3Mode(); // optional, some boards require this
    // player.setVolume(VOLUME);

    //@------------------LVGL初始化----------------

    //@-tft驱动初始化
    tft.begin(); /* TFT init */
    tft.setRotation(3);

    //@-设置touch参数
    uint16_t calData[5] = {555, 3151, 234, 3442, 1};
    tft.setTouch(calData);

    //@-初始化lv显存
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    //@-初始化显示驱动
    /*Initialize the display*/
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

    //@-设置显示tick
    /*Initialize the graphics library's tick*/
    tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

    //@-设置显示主题
    //Set the theme..
    lv_theme_t * th = lv_theme_night_init(210, NULL);     //Set a HUE value and a Font for the Night Theme   
    // lv_theme_t * th = lv_theme_zen_init(100, NULL);
    //  lv_theme_t * th = lv_theme_alien_init(100, NULL);     
    // lv_theme_t * th = lv_theme_material_init(100, NULL);     
    dx_gui_init(th);

    //---------------------------------------
    // pixels.begin(); // This initializes the NeoPixel library. 

    //@-创建曲线数据生成任务-50ms
    // dx_task = lv_task_create(my_task, 50, LV_TASK_PRIO_MID, &user_data);


    // //@-wifi init
    // WiFi.begin(ssid, pass);
    // while (WiFi.status() != WL_CONNECTED) 
    // {
    // delay(500);
    // Serial.print(".");
    // }
    // Serial.println("WiFi connected");  
    // Serial.print("IP address:");  
    // Serial.println(WiFi.localIP());

    // station_connect(radio_cannel);  //count

    // xTaskCreatePinnedToCore(
    //   Get_Radio_Data,          /* Task function. */
    //   "Get_Radio_Data",        /* String with name of task. */
    //   10000,            /* Stack size in bytes. */
    //   NULL,             /* Parameter passed as input of the task */
    //   1,                /* Priority of the task. */
    //   &Task_radio,
    //   0);      //core 0

    // dx_task = lv_task_create(Get_Radio_Data, 1, LV_TASK_PRIO_HIGHEST, &user_data);
}

//@-lvgl生成曲线数据任务
void my_task(lv_task_t * task)
{
  /*Use the user_data*/
  // uint32_t *user_data = task->user_data;
  // printf("my_task called with user data: %d\n", task->user_data);

  // /*Do something with LittlevGL*/
  // if(something_happened) {
  //   something_happened = false;
  //   lv_btn_create(lv_scr_act(), NULL);
  // }

    //@-https://www.shuxuele.com/algebra/amplitude-period-frequency-phase-shift.html

    // chart_data = 40 * sin(0.5709 * sin_time) + 50;   //0.5709 = 2pi/T  T=11s
    // chart_data = 40 * sin(2.0933 * sin_time) + 50;      //0.5709 = 2pi/T  T=3s
    // chart_data = ((40 * sin(12.56 * sin_time)) + 50);      //0.5709 = 2pi/T  T=0.5s
    // chart_data = ((40 * sin(31.4 * sin_time)) + 50);      //0.5709 = 2pi/T  T=0.2s

    if(DX_lv_Index == 1)
    {
      chart_data1 = ((40 * sin(6.28 * sin_time)) + 50);      //0.5709 = 2pi/T  T=1s
      chart_data2 = ((40 * sin(6.28 * sin_time + 1)) + 50);      //0.5709 = 2pi/T  T=1s

      sin_time=sin_time + 0.05;
      if(sin_time == 1)
      sin_time = 0;

      lv_chart_set_next(chart, ser1, chart_data1);
      lv_chart_set_next(chart, ser2, chart_data2);

      lv_label_set_text_fmt(main_label, "俯仰角: %3.2f", chart_data1);
    }
    
}


//@-VS1053获得数据任务
// void Get_Radio_Data(lv_task_t * task)
void Get_Radio_Data(void * prameter)
{
    while(1)
    {
        if((Connect_Wifi_Status.WIFI_Status == 2) && (Radio_Mode == 1))
        {
          if(WiFi.status() == WL_CONNECTED) 
          {
              if ((client.available() > 0) && (cannel_switch == false)) 
              {
                uint8_t bytesread = client.read(mp3buff, 128);

                // btstack_ring_buffer_write(dx_ring_buff, mp3buff, 32);

                if( dx_buffer.available() > bytesread)
                {
                  for(index1 = 0; index1 < bytesread; index1++)
                  dx_buffer.push(mp3buff[index1]);
                }


                // player.playChunk(mp3buff, bytesread);

                  // player.playChunk(hello2, sizeof(hello2)); //VS1053 is wake up & running
                // Serial.printf("Recv:0x%x-0x%x-0x%x-0x%x-0x%x\n",mp3buff[0],mp3buff[1],mp3buff[2],mp3buff[3],mp3buff[4]);

              }

            system_tick = system_tick + 1;
            if(system_tick > 200)
            {
              system_tick = 0;
              buff_size = dx_buffer.size();
              // Serial.printf("c:%d-->s:%d\n",radio_cannel,buff_size);
              // lv_label_set_text_fmt(label_buffer, "Buffer: %d", buff_size);

              // String taskMessage = "Task data running on core ";
              // taskMessage = taskMessage + xPortGetCoreID();
              // Serial.println(taskMessage);  //log para o serial monitor
            }
          }
      }

      delay(5);
        // yield();
    }
}

//@-主循环
void loop() 
{
    tick1 = tick1 + 1;

    // if(tick1> 1000)
    // {
    //   tick1 = 0;
    //   String taskMessage = "Task main running on core ";
    //   taskMessage = taskMessage + xPortGetCoreID();
    //   Serial.println(taskMessage);  //log para o serial monitor
    // }

    //@-LVGL任务接口
    lv_task_handler(); /* let the GUI do its work */

    if(DX_lv_Index == 1)
    {
      if(tick1 > 20)
      {
        tick1 = 0;
        lv_chart_refresh(chart); /*Required after direct set*/
      }
    }

    if((Connect_Wifi_Status.WIFI_Status == 2) && (Radio_Mode == 1))
    {
      lv_label_set_text_fmt(label_buffer, "Buffer: %d", buff_size);

      if(radio_cannel_copy != radio_cannel)
      {
        radio_cannel_copy = radio_cannel;
        station_connect(radio_cannel);
      }

    
      if(dx_buffer.size() >= recv_start)
      {
        play_lock = true;
      }

      if(play_lock == true)
      {
        if(dx_buffer.size() > recv_avg)
        {
          play_flag = true;
        }
        else if (dx_buffer.size() < recv_less)
        {
          play_flag = false;
          play_lock = false;
        }
      }

      if(play_flag == true)
      {
        for(index2 = 0; index2 < 32; index2++)
        playbuff[index2] = dx_buffer.shift();

        player.playChunk(playbuff, 32);
      }
    }


    //------------------------
    // for(int i=0;i<NUMPIXELS;i++)
    // {
    //     pixels.setPixelColor(i, pixels.Color(show_red, show_green, show_blue)); // Moderately bright green color.
    //     pixels.show(); // This sends the updated pixel color to the hardware.
    // }

    // char buf[50];
    // sprintf(buf, "C1:[0x%x]-C2:[0x%x]-C3:[0x%x]",  show_red, show_green, show_blue);
    // Serial.println(buf);

    delay(1);

}


//@-初始化界面
void dx_gui_init(lv_theme_t * th)
{
    lv_theme_set_current(th);
    th = lv_theme_get_current();    /*If `LV_THEME_LIVE_UPDATE  1` `th` is not used directly so get the real theme after set*/
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    static lv_style_t dx_style1;   //@-add-20200505-dx
    lv_style_copy(&dx_style1, &lv_style_transp);  //@-add-20200505-dx
    dx_style1.text.font = &myFont;  //@-add-20200505-dx
    dx_style1.text.color = LV_COLOR_WHITE ;

    // lv_ex_tileview_1();
    // lv_ex_win_1();
    // lv_ex_chart_1();
    // lv_ex_imgbtn_1();

    // lv_ex_spinbox_1();
    // lv_ex_tabview_1();
    // lv_ex_ta_2();
    // lv_ex_page_1();

    // lv_ex_mbox_2();
    lv_wifi_1_scan();

    // lv_dx_color_pick();

}

void lv_dx_color_pick(void)
{
  /* Set the style of the color ring */
    static lv_style_t styleMain;
    lv_style_copy(&styleMain, &lv_style_plain);
    styleMain.line.width = 30;
	/* Make the background white */
    styleMain.body.main_color = styleMain.body.grad_color = LV_COLOR_WHITE;

	/* Set the style of the knob */
    static lv_style_t styleIndicator;
    lv_style_copy(&styleIndicator, &lv_style_pretty);
    styleIndicator.body.border.color = LV_COLOR_WHITE;
	/* Ensure that the knob is fully opaque */
    styleIndicator.body.opa = LV_OPA_COVER;
    styleIndicator.body.border.opa = LV_OPA_COVER;

    lv_obj_t * scr = lv_scr_act();

    colorPicker = lv_cpicker_create(scr, NULL);
    lv_obj_set_size(colorPicker, pickerSize, pickerSize);
	/* Choose the 'DISC' type */
    lv_cpicker_set_type(colorPicker, LV_CPICKER_TYPE_DISC);
    lv_obj_align(colorPicker, NULL, LV_ALIGN_CENTER, 0, 0);
	/* Set the styles */
    lv_cpicker_set_style(colorPicker, LV_CPICKER_STYLE_MAIN, &styleMain);
    lv_cpicker_set_style(colorPicker, LV_CPICKER_STYLE_INDICATOR, &styleIndicator);
	/* Change the knob's color to that of the selected color */
    lv_cpicker_set_indic_colored(colorPicker, true);

    lv_obj_set_event_cb(colorPicker, btn_event_cb);

}


static void mbox_event_cb(lv_obj_t *obj, lv_event_t evt)
{
	if(evt == LV_EVENT_DELETE && obj == mbox) {
		/* Delete the parent modal background */
		lv_obj_del_async(lv_obj_get_parent(mbox));
		mbox = NULL; /* happens before object is actually deleted! */
		// lv_label_set_text(info, welcome_info);
	} else if(evt == LV_EVENT_VALUE_CHANGED) {
		/* A button was clicked */
		lv_mbox_start_auto_close(mbox, 0);
	}
}

static void btn_event_cb(lv_obj_t *btnX, lv_event_t evt)
{

	if(evt == LV_EVENT_CLICKED) {
    
      if(btnX == btn_dis)
      {
        static lv_style_t modal_style;
        /* Create a full-screen background */
        lv_style_copy(&modal_style, &lv_style_plain_color);
      /* Set the background's style */
      modal_style.body.main_color = modal_style.body.grad_color = LV_COLOR_BLACK;
      modal_style.body.opa = LV_OPA_50;

      static lv_style_t dx_style;   //@-add-20200505-dx
      lv_style_copy(&dx_style, &lv_style_plain_color);  //@-add-20200505-dx
      dx_style.text.font = &myFont;  //@-add-20200505-dx
      dx_style.text.color = LV_COLOR_GREEN ;

      /* Create a base object for the modal background */
      lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
      lv_obj_set_style(obj, &modal_style);
      lv_obj_set_pos(obj, 0, 0);
      lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);
      lv_obj_set_opa_scale_enable(obj, true); /* Enable opacity scaling for the animation */

      static const char * btns2[] = {"OK", "CANCEL", ""};

      /* Create the message box as a child of the modal background */
      mbox = lv_mbox_create(obj, NULL);
      lv_obj_set_style(mbox, &dx_style);
      lv_mbox_add_btns(mbox, btns2);
      lv_mbox_set_text(mbox, "杭州航海仪器有限公司");
      lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
      lv_obj_set_event_cb(mbox, mbox_event_cb);

      /* Fade the message box in with an animation */
      lv_anim_t a;
      lv_anim_init(&a);
      lv_anim_set_time(&a, 500, 0);
      lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
      lv_anim_set_exec_cb(&a, obj, (lv_anim_exec_xcb_t)lv_obj_set_opa_scale);
      lv_anim_create(&a);

      // lv_label_set_text(info, in_msg_info);
      // lv_obj_align(info, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5);
      }

      else if(btnX == btn_wifi_back)
      {
        Radio_Mode = 0;
        lv_wifi_1_scan();
      }

      else if(btnX == colorPicker)
      {
        lv_color_t temp_color = lv_cpicker_get_color(colorPicker);
        lv_color32_t c24;
        char buf[50];
        c24.full = lv_color_to32(temp_color);
        // sprintf(buf, "C0:[0x%x]-C1:[0x%x]-C2:[0x%x]-C3:[0x%x]", c24.full, ((c24.full | 0xff00ffff)>>16), ((c24.full | 0xffff00ff)>>8), (c24.full | 0xffffff00));
        
        show_red = ((c24.full & 0x00ff0000)>>16);
        show_green = ((c24.full & 0x0000ff00)>>8);
        show_blue = ((c24.full & 0x000000ff));

        // sprintf(buf, "C0:[0x%x]-C1:[0x%x]-C2:[0x%x]-C3:[0x%x]", c24.full, show_red, show_green, show_blue);
        // Serial.println(buf);
      }
	}
}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        
        if((obj == btn_channle_plus)||(obj == btn_channle_minus))
        {
          if(obj == btn_channle_plus)
          {
            radio_cannel = radio_cannel + 1;
            if(radio_cannel > 9)
            radio_cannel = 0;
          }
          else if(obj == btn_channle_minus)
          {
            radio_cannel = radio_cannel - 1;
            if(radio_cannel <= 0)
            radio_cannel = 9;
          }
          cannel_switch = true;
          lv_label_set_text_fmt(label_cannel, "Channel: %d", radio_cannel);
        }
        else if((obj == btn_volum_pluse)||(obj == btn_volum_minus))
        {
          if(obj == btn_volum_pluse)
          {
            VOLUME = VOLUME + 10;
            if(VOLUME > 100)
            VOLUME = 100;
          }
          else if(obj == btn_volum_minus)
          {
            VOLUME = VOLUME - 10;
            if(VOLUME <= 0)
            VOLUME = 10;
          }
          player.setVolume(VOLUME);
          lv_label_set_text_fmt(label_vloum, "Volum: %d",VOLUME);
        }
    }
    // else if(event == LV_EVENT_VALUE_CHANGED) {
    //     printf("Toggled\n");
    // }
}

void lv_ex_mbox_2(void)
{
  lv_obj_t * label_temp;

  //@-清屏
  lv_obj_t * scr = lv_cont_create(NULL, NULL);
  lv_disp_load_scr(scr);

  static lv_style_t dx_style;   //@-add-20200505-dx
  lv_style_copy(&dx_style, &lv_style_transp);  //@-add-20200505-dx
  dx_style.text.font = &myFont;  //@-add-20200505-dx
  dx_style.text.color = LV_COLOR_RED ;

	/* Create a button, then set its position and event callback */
	btn_dis = lv_btn_create(lv_scr_act(), NULL);
  // lv_obj_set_event_cb(btn_dis, event_handler);
	lv_obj_set_size(btn_dis, 100, 35);
	lv_obj_set_event_cb(btn_dis, btn_event_cb);
	lv_obj_align(btn_dis, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);

	/* Create a label on the button */
	lv_obj_t *label = lv_label_create(btn_dis, NULL);
  lv_obj_set_style(label, &dx_style); 
	lv_label_set_text(label, "状态");
  dx_style.text.color = LV_COLOR_YELLOW ;

  btn_wifi_back = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn_wifi_back, btn_event_cb);
  lv_obj_set_size(btn_wifi_back, 100,35);
  lv_obj_align(btn_wifi_back, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 60);
  label_temp = lv_label_create(btn_wifi_back, NULL);
  lv_label_set_text(label_temp,  LV_SYMBOL_HOME " back" );

  btn_channle_plus = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn_channle_plus, event_handler);
  lv_obj_set_size(btn_channle_plus, 100,35);
  lv_obj_align(btn_channle_plus, NULL, LV_ALIGN_CENTER, -60, 0);
  label_temp = lv_label_create(btn_channle_plus, NULL);
  lv_label_set_text(label_temp, "Channel +");

  btn_channle_minus = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn_channle_minus, event_handler);
  lv_obj_set_size(btn_channle_minus, 100,35);
  lv_obj_align(btn_channle_minus, NULL, LV_ALIGN_CENTER, 60, 0);
  label_temp = lv_label_create(btn_channle_minus, NULL);
  lv_label_set_text(label_temp, "Channel -");


  btn_volum_pluse = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn_volum_pluse, event_handler);
  lv_obj_set_size(btn_volum_pluse, 100,35);
  lv_obj_align(btn_volum_pluse, NULL, LV_ALIGN_CENTER, -60, 50);
  label_temp = lv_label_create(btn_volum_pluse, NULL);
  lv_label_set_text(label_temp, "Volum +");

  btn_volum_minus = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn_volum_minus, event_handler);
  lv_obj_set_size(btn_volum_minus, 100,35);
  lv_obj_align(btn_volum_minus, NULL, LV_ALIGN_CENTER, 60, 50);
  label_temp = lv_label_create(btn_volum_minus, NULL);
  lv_label_set_text(label_temp, "Volum -");


  label_cannel = lv_label_create(lv_scr_act(), NULL);
  // lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  // lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(label_cannel, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(label_cannel, "Channel: 0");
  lv_obj_set_size(label_cannel, 30,35);
  lv_obj_align(label_cannel, NULL, LV_ALIGN_IN_TOP_LEFT, 130, 20);

  label_buffer = lv_label_create(lv_scr_act(), NULL);
  // lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  // lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(label_buffer, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(label_buffer, "Buffer: 0");
  lv_obj_set_size(label_buffer, 30,35);
  lv_obj_align(label_buffer, NULL, LV_ALIGN_IN_TOP_LEFT, 130, 40);


  label_vloum = lv_label_create(lv_scr_act(), NULL);
  // lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
  // lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
  lv_label_set_align(label_vloum, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
  lv_label_set_text(label_vloum, "Volum: 0");
  lv_obj_set_size(label_vloum, 30,35);
  lv_obj_align(label_vloum, NULL, LV_ALIGN_IN_TOP_LEFT, 130, 60);


	/* Create an informative label on the screen */
	info = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style(info, &dx_style); 
	lv_label_set_text(info, in_msg_info);
	lv_label_set_long_mode(info, LV_LABEL_LONG_SROLL_CIRC); /* Make sure text will wrap */
	lv_obj_set_width(info, LV_HOR_RES - 10);
	lv_obj_align(info, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5);

}

void station_connect (int station_no ) 
{
    client.flush();
    dx_buffer.clear();
    delay(100);

    if (client.connect(host[station_no],port[station_no])) 
     {
       Serial.println("Connected now");
     }
    Serial.print(host[station_no]);
    Serial.println(path[station_no]);
    
    client.print(String("GET ") + path[station_no] + " HTTP/1.1\r\n" + "Host: " + host[station_no] + "\r\n" + "Connection: close\r\n\r\n");  

    cannel_switch = false;   
}
