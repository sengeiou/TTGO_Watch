#pragma mark - Depend ESP8266Audio and ESP8266_Spiram libraries
/*
cd ~/Arduino/libraries
git clone https://github.com/earlephilhower/ESP8266Audio
git clone https://github.com/Gianbacchio/ESP8266_Spiram
*/

//const char* ssid = "8879";
//const char* password = "blackbug381";

// C:\Users\DX\Documents\Arduino\libraries\TTGO_TWatch_Library-master\src

/*
* firmeware version string
*/
#define __FIRMWARE__            "20200922"

// #define USE_ESP_NOW
#define USE_MP3

#define uS_TO_S_FACTOR 1000000

#define DISPLAY_MIN_BRIGHTNESS      8
#define DISPLAY_MAX_BRIGHTNESS      255

// #define MAIN_BACKPIC_NUM_1  //宇航员
// #define MAIN_BACKPIC_NUM_2     //开心最重要
#define MAIN_BACKPIC_NUM_3     //十年饮冰


//@-ttgo配置
#include "config.h"

#ifdef USE_ESP_NOW
#include <esp_now.h>
#endif

//@-wifi
#include <WiFi.h>
// //@-OTA远程更新
// #include <Update.h>
//@-NVS文件系统
#include "TridentTD_ESP32NVS.h"

//@-audio
#include <HTTPClient.h>         //Remove Audio Lib error
#include "AudioFileSourcePROGMEM.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

//@-配置用户音频文件
#include "./audio/geji_44k_mp3.h"
#include "./audio/select05_mp3.h"
#include "./audio/beep_24_mp3.h"
#include "./audio/laozigun_robot.h"

//@-配置用户字体
LV_FONT_DECLARE(myFont);
LV_FONT_DECLARE(dxLED7);
LV_FONT_DECLARE(dxLED7_60);
LV_FONT_DECLARE(myLED_Font);

//@-配置用户图片数据
// LV_IMG_DECLARE(me);
LV_IMG_DECLARE(TTGO_Main);
LV_IMG_DECLARE(rich);
LV_IMG_DECLARE(TTGO_Main_Biaoyu);


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

#ifdef USE_MP3
//@-audio
AudioGeneratorMP3 *mp3;
AudioFileSourcePROGMEM *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;
#endif

//@-lvgl 控件
lv_obj_t *btn;
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
lv_obj_t *label_time_second;
lv_obj_t *label_time_date; 

//@-实时电量显示
lv_obj_t *label_batt;

//@-WIFI连接图标
lv_obj_t * label_wifi;

/*Create a chart*/
lv_obj_t * chart;

lv_obj_t *sw1;
lv_obj_t *sw2;
lv_obj_t *sw3;
lv_obj_t *sw4;

lv_obj_t *tileview;
lv_obj_t* tile_0_0;
lv_obj_t* tile_0_1;
lv_obj_t* tile_0_2;
lv_obj_t* tile_0_3;
lv_obj_t* tile_0_4;
lv_obj_t* tile_1_0;
lv_obj_t* tile_1_1;
lv_obj_t* tile_1_2;
lv_obj_t* tile_1_3;
lv_obj_t* tile_1_4;

//@-定时器
lv_obj_t * lmeter;
//@-定时器数值
lv_obj_t *label_timer;
//@-定时器增减方向
lv_obj_t *sw_timer_dir;
//@-定时器启停
lv_obj_t *sw_timer_run;

//@-十年饮冰标语
lv_obj_t * img_biaoyu;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_label;

static void slider_light_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_light_label;

lv_obj_t * ta;
lv_obj_t * pwd_ta;
lv_obj_t * kb;

//@-WIFI 配置信息
lv_obj_t * wifi_list;
lv_obj_t * wifi_def_info_label;
lv_obj_t * wifi_connect_info_label;
lv_obj_t * wifi_scan_btn;
lv_obj_t * wifi_save_btn;
lv_obj_t * wifi_connect_btn;
lv_obj_t * wifi_bg_bottom;

static lv_obj_t * wifi_kb;            //@-wifi密码输入键盘
static lv_obj_t * wifi_ta_password;   //@-wifi密码输入框
static lv_obj_t * wifi_mbox_connect;  //@-wifi密码输入对话框

//@-固件更新
lv_obj_t * firmware_update_btn;
lv_obj_t * firmwareUpdata_label;

//@-天气预报信息
lv_obj_t * weather_title_label;
lv_obj_t * weather_info_label;
lv_obj_t * weatcher_get_btn;

bool btn2_flag = true;


#ifdef USE_ESP_NOW
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
#endif


char display_buf[128];
int system_tick = 0;
int System_Sleep_Tick = 0;

int display_time_bat_info_tick = 0;


//@-NVS数据
String NVS_WIFI_SSID;
String NVS_WIFI_PASS;
String NVS_OTA_ADD_HOST;
String NVS_OTA_ADD_PATH;
int NVS_Backlight_Value;

//@-定时器0~99min
int Alarm_Timer_Data = 1;
bool Alarm_Timer_SetData_Dir = false;  //false:增加  true:减少
bool Alarm_Timer_Run = false;  
bool rtcIrq = false;
int all_second = 0;
bool Set_Alarm_Run_Flag = false;   
int  Alarm_Run_Time = 5;

//@-WIFI
TaskHandle_t ntWifiScanTaskHandler;
TaskHandle_t ntWifiConnectTaskHandler;
//@-固件更新任务
TaskHandle_t ntFireWarmTaskHandler;  
int WIFI_Scan_Num = 0;
bool wifi_scan_flag = false;
bool wifi_connect_flag = false;
String wifi_ssidName;
String wifi_password;
unsigned long wifi_timeout = 10000; // 10sec
//@-固件更新任务运行标志
bool firmware_begin_flag = false; 

//@-天气预报
//@-天气任务运行标志
TaskHandle_t ntWeatherTaskHandler;
int Getweather_tick = 0;
bool weather_begin_flag = false; 
bool weather_json_flag = false;
float weather_temputer = 0;


/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void Print_Wakeup_Reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); Set_Alarm_Run_Flag = true; break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


#ifdef USE_ESP_NOW
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
#endif

//@-开始wifi连接任务
void beginWIFITask(void *pvParameters) {

    //@-判断wifi是否已连接
    if(WiFi.status() == WL_CONNECTED) 
        WiFi.disconnect();
    {
        vTaskDelay(1000);
    }

    updateBottomStatus(LV_COLOR_TEAL,"Connecting WIFI: " + wifi_ssidName);

    unsigned long startingTime = millis();

    WiFi.begin(wifi_ssidName.c_str(), wifi_password.c_str());
    while (WiFi.status() != WL_CONNECTED && (millis() - startingTime) < wifi_timeout)
    {
        vTaskDelay(250);
    }

    if(WiFi.status() != WL_CONNECTED) 
    {
        updateBottomStatus(LV_COLOR_RED, "Please check your wifi password and try again.");
        WiFi.disconnect();
        wifi_connect_flag = false;
        vTaskDelay(1000);
        vTaskDelete(NULL);
    }
  
    updateBottomStatus(LV_COLOR_GREEN, "WIFI is Connected! Local IP: " +  WiFi.localIP().toString());
    wifi_connect_flag = false;
    vTaskDelay(1000);
    vTaskDelete(NULL);

}

//@-wifi连接
void connectWIFI(){

  if(wifi_ssidName == NULL || wifi_ssidName.length() <1 || wifi_password == NULL || wifi_password.length() <1)
  {
    return;
  }
  
  //@-wifi扫描任务结束及连接任务结束
  if((wifi_scan_flag == false) && (wifi_connect_flag == false))
  {
    wifi_connect_flag = true;
    // vTaskDelete(ntWifiScanTaskHandler);
    // vTaskDelay(500);
    xTaskCreate(beginWIFITask,"BeginWIFITask",2048,NULL,0, &ntWifiConnectTaskHandler);   
  }  
  else
  {
    updateBottomStatus(LV_COLOR_RED, "WIFI is working please wait..");
  }
             
}

//@-wifi密码输入消息框事件
static void wifi_mbox_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED)
    {
        lv_obj_move_background(wifi_kb);
        lv_obj_move_background(wifi_mbox_connect);
      
        if(strcmp(lv_msgbox_get_active_btn_text(obj), "Connect")==0)
        {
            wifi_password = lv_textarea_get_text(wifi_ta_password);
            wifi_password.trim();
            connectWIFI();
        }
    }
}

//@-创建wifi密码输入消息框
static void WIFI_makePassWordMsgBox()
{
  wifi_mbox_connect = lv_msgbox_create(lv_scr_act(), NULL);
  static const char * btns[] ={"Connect", "Cancel", ""};
  
  wifi_ta_password = lv_textarea_create(wifi_mbox_connect, NULL);
  lv_obj_set_size(wifi_ta_password, 200, 40);
  lv_textarea_set_text(wifi_ta_password, "");

  lv_msgbox_add_btns(wifi_mbox_connect, btns);
  lv_obj_set_width(wifi_mbox_connect, 200);
  lv_obj_set_event_cb(wifi_mbox_connect, wifi_mbox_event_handler);
  lv_obj_align(wifi_mbox_connect, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_move_background(wifi_mbox_connect);
}

//@-wifi键盘事件
static void wifi_keyboard_event_cb(lv_obj_t * kb, lv_event_t event)
{
  lv_keyboard_def_event_cb(kb, event);

  if(event == LV_EVENT_APPLY){
    lv_obj_move_background(kb);
    
  }else if(event == LV_EVENT_CANCEL){
    lv_obj_move_background(kb);
  }
}

//@-创建wifi键盘
static void WIFI_makeKeyboard()
{
  wifi_kb = lv_keyboard_create(lv_scr_act(), NULL);
  lv_obj_set_size(wifi_kb,  LV_HOR_RES, LV_VER_RES / 2);
  lv_keyboard_set_cursor_manage(wifi_kb, true);
  
  lv_keyboard_set_textarea(wifi_kb, wifi_ta_password);
  lv_obj_set_event_cb(wifi_kb, wifi_keyboard_event_cb);
  lv_obj_move_background(wifi_kb);
}
    

//@-播放MP3
// void Run_MP3_Audio(const void *filename_audio)
// void Run_MP3_Audio(int audio_num)
void Run_MP3_Audio(const void *filename_audio, uint32_t len)
{
    /**
    AudioFileSourcePROGMEM::AudioFileSourcePROGMEM(const void *data, uint32_t len)
    {
    open(data, len);
    }
    */
    if (mp3->isRunning() == false) 
    {
        // if(audio_num == 1)
        // file = new AudioFileSourcePROGMEM(geji_44k_mp3, sizeof(geji_44k_mp3));
        // else if(audio_num == 2)
        // file = new AudioFileSourcePROGMEM(select05_mp3, sizeof(select05_mp3));
        // else if(audio_num == 3)
        // file = new AudioFileSourcePROGMEM(beep_24_mp3, sizeof(beep_24_mp3));
        // else if(audio_num == 4)
        // file = new AudioFileSourcePROGMEM(laozigun_robot, sizeof(laozigun_robot));
        file = new AudioFileSourcePROGMEM(filename_audio, len);
        id3 = new AudioFileSourceID3(file);
        mp3 = new AudioGeneratorMP3();
        mp3->begin(id3, out);
    }
}

//@-Timer告警设置
void Setup_Timer_Alarm(bool run_flag)
{
    int alarm_time_minute = 0;
    int alarm_time_hour = 0;
    int add_hour = 0;
    int all_minutes = 0;

    if(run_flag == true)
    {
        //@-获取当前时间
        RTC_Date current_date = ttgo->rtc->getDateTime();
        int DayOfWeek = ttgo->rtc->getDayOfWeek(current_date.day, current_date.month, current_date.year);

        alarm_time_hour = current_date.hour;

        all_minutes = current_date.minute + Alarm_Timer_Data;
        all_second = Alarm_Timer_Data * 60;

        while((all_minutes > 59))
        {
            all_minutes = all_minutes - 60;
            add_hour++;
        }

        pinMode(RTC_INT, INPUT_PULLUP);
        attachInterrupt(RTC_INT, [] {rtcIrq = 1;}, FALLING);

        alarm_time_minute = all_minutes;
        alarm_time_hour = alarm_time_hour + add_hour;


        ttgo->rtc->disableAlarm();

        ttgo->rtc->resetAlarm();

        ttgo->rtc->setAlarm(alarm_time_hour, alarm_time_minute, current_date.day, DayOfWeek);

        // ttgo->rtc->setAlarmByMinutes(alarm_time_minute);
        // ttgo->rtc->setAlarmByHours(alarm_time_hour);
        // ttgo->rtc->setAlarmByDays(current_date.day);
        // ttgo->rtc->setAlarmByWeekDay(DayOfWeek);

        ttgo->rtc->enableAlarm();

        RTC_Alarm RTC_Temp = ttgo->rtc->getAlarm();
        Serial.print("Hour:");
        Serial.println(RTC_Temp.hour);
        Serial.print("Minutes:");
        Serial.println(RTC_Temp.minute);
        Serial.print("Day:");
        Serial.println(RTC_Temp.day);
        Serial.print("Weekday:");
        Serial.println(RTC_Temp.weekday);


        // esp_sleep_enable_timer_wakeup(all_second * uS_TO_S_FACTOR);
    }

    // Serial.print("Hour:");
    // Serial.println(alarm_time_hour);
    // Serial.print("Minutes:");
    // Serial.println(alarm_time_minute);
    // Serial.print("all second:");
    // Serial.println(all_second);

}

//@-wifi连接
void wifi_popupPWMsgBox(){
  if(NVS_WIFI_SSID == NULL || NVS_WIFI_SSID.length() == 0){
    return;
  }

    // lv_textarea_set_text(ta_password, ""); 
    // lv_msgbox_set_text(mbox_connect, ssidName.c_str());
    // lv_obj_move_foreground(mbox_connect);
    
    // lv_obj_move_foreground(kb);
    // lv_keyboard_set_textarea(kb, ta_password);
}

//@-更新wifi底部信息栏
void updateBottomStatus(lv_color_t color, String text){
  lv_obj_set_style_local_bg_color(wifi_bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,color);
  lv_label_set_text(wifi_connect_info_label, text.c_str());
}

//@-wifi scan 任务
void scanWIFITask(void *pvParameters) 
{
    //@-WIFI断开连接
    // WiFi.disconnect();

    vTaskDelay(500); 

    // while (1) 
    // {
        updateBottomStatus(LV_COLOR_ORANGE, "::: Searching Available WIFI :::");        
        int n = WiFi.scanNetworks();

        vTaskDelay(2000);

        if (n <= 0) 
        {
            updateBottomStatus(LV_COLOR_RED, "Sorry no networks found!");        
        }
        else
        {
            lv_dropdown_clear_options(wifi_list);  
            vTaskDelay(10);
            for (int i = 0; i < n; ++i) 
            {
                                
                String item = WiFi.SSID(i) + " (" + WiFi.RSSI(i) +") " + ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
                lv_dropdown_add_option(wifi_list,item.c_str(),LV_DROPDOWN_POS_LAST);
                vTaskDelay(10);
            }

            updateBottomStatus(LV_COLOR_GREEN, String(n) + " networks found!");                     
        }

        wifi_scan_flag = false;

        vTaskDelete(NULL);

        // vTaskDelay(30000); 
    // } 
}

//@-WIFI密码输入消息框
void WIFI_popupPWMsgBox(){

    if(wifi_ssidName == NULL || wifi_ssidName.length() == 0)
    {
        return;
    }

    lv_textarea_set_text(wifi_ta_password, ""); 
    lv_msgbox_set_text(wifi_mbox_connect, wifi_ssidName.c_str());
    lv_obj_move_foreground(wifi_mbox_connect);
    
    lv_obj_move_foreground(wifi_kb);
    lv_keyboard_set_textarea(wifi_kb, wifi_ta_password);
}

//@-wifi list 事件
void wifi_list_event_handler(lv_obj_t * obj, lv_event_t event){
  
  if(event == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_dropdown_get_selected_str(obj, buf, sizeof(buf));
        wifi_ssidName = String(buf);
        
        for (int i = 0; i < wifi_ssidName.length()-1; i++) {
          if (wifi_ssidName.substring(i, i+2) == " (") {
              wifi_ssidName = wifi_ssidName.substring(0, i);
            break;
          }
        }
        
        WIFI_popupPWMsgBox();
    }
}

//@-lvgl控件事件处理
void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (obj == btn10) {
        #ifdef USE_ESP_NOW
        send_Data.b = 0;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
        #endif
    } 
    else if (obj == btn20) {
        #ifdef USE_ESP_NOW
        send_Data.b = 180;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
        #endif
    } 
    else if (obj == btn30) {
        #ifdef USE_ESP_NOW
        send_Data.b = send_Data.b + 10;
        if(send_Data.b > 180)
        send_Data.b = 180;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
        #endif
    } 
    else if (obj == btn40) {
        #ifdef USE_ESP_NOW
        send_Data.b = send_Data.b - 10;
        if(send_Data.b < 0)
        send_Data.b = 0;
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
        #endif
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
        #ifdef USE_ESP_NOW
        if(send_Data.e == false) 
        send_Data.e = true;
        else if(send_Data.e == true) 
        send_Data.e = false;

        // Serial.printf("dx\n");

        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
        #endif
    }

    else if(obj == sw1)
    {
        //@-播放音效
        // Run_MP3_Audio(1);
        Run_MP3_Audio(&geji_44k_mp3, sizeof(geji_44k_mp3));
    }
    else if(obj == sw2)
    {
        //@-播放音效
        // Run_MP3_Audio(2);
        Run_MP3_Audio(&select05_mp3, sizeof(select05_mp3));
    }
    else if(obj == sw3)
    {
        //@-播放音效
        // Run_MP3_Audio(3);
        Run_MP3_Audio(&beep_24_mp3, sizeof(beep_24_mp3));
    }
    else if(obj == sw4)
    {
        //@-播放音效
        // Run_MP3_Audio(4);
        Run_MP3_Audio(&laozigun_robot, sizeof(laozigun_robot));
    }

    // else if(obj == tile_1_2)
    // {
    //     Serial.printf("tile_1_2\n");
    // }

    //@-定时器
    else if(obj == lmeter)
    {
        //@-定时器数值增加
        if(Alarm_Timer_SetData_Dir == false)
        {
            Alarm_Timer_Data = Alarm_Timer_Data + 1;
            if(Alarm_Timer_Data > 99)
            Alarm_Timer_Data = 99;
        }
        else if(Alarm_Timer_SetData_Dir == true)
        {
            Alarm_Timer_Data = Alarm_Timer_Data - 1;
            if(Alarm_Timer_Data <= 1)
            Alarm_Timer_Data = 1;
        }

        lv_linemeter_set_value(lmeter, Alarm_Timer_Data);

        lv_label_set_text_fmt(label_timer, "%02d", Alarm_Timer_Data);
    }
    //@-定时器数值方向
    else if(obj == sw_timer_dir)
    {
        if(event == LV_EVENT_VALUE_CHANGED) {
            Alarm_Timer_SetData_Dir = lv_switch_get_state(sw_timer_dir);
        }
    }
    //@-定时器启停控制
    else if(obj == sw_timer_run)
    {
        if(event == LV_EVENT_VALUE_CHANGED) {
            Alarm_Timer_Run = lv_switch_get_state(sw_timer_run);

            Setup_Timer_Alarm(Alarm_Timer_Run);

            ttgo->motor->onec();
        }
    }
    //@-wifi扫描-------------------------------------
    else if(obj == wifi_scan_btn)
    {
        //@-wifi扫描任务结束及连接任务结束
        if((wifi_scan_flag == false) && (wifi_connect_flag == false))
        {
            wifi_scan_flag = true;
            // dx_task = lv_task_create(start_wifi_scan_task, 1000, LV_TASK_PRIO_MID, &user_data);
            // vTaskDelay(500);
            xTaskCreate(scanWIFITask,"ScanWIFITask",4096, NULL,1,&ntWifiScanTaskHandler);
        }
    }

    //@-固件更新-----------------------------------
    else if(obj == firmware_update_btn)
    {
        //@-wifi是否连接
        if(WiFi.status() == WL_CONNECTED)
        {
            if(firmware_begin_flag == false)
            {
                firmware_begin_flag = true;

                xTaskCreate(updateFirewarmTask,"updateFirewarmTask",4096, NULL,1,&ntFireWarmTaskHandler);
            }
        }
    }

    //@-获取天气-----------------------------------
    else if(obj == weatcher_get_btn)
    {
        //@-wifi是否连接
        if(WiFi.status() == WL_CONNECTED)
        {
            if(weather_begin_flag == false)
            {

                weather_json_flag = false;

                weather_begin_flag = true;

                xTaskCreate(GetweatherTask,"GetweatherTask",4096, NULL,0,&ntWeatherTaskHandler);
            }
        }
    }
    
    
}

static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        #ifdef USE_ESP_NOW
        static char buf[20]; /* max 3 bytes for number plus 1 null terminating byte */

        send_Data.b = lv_slider_get_value(slider);

        snprintf(buf, 20, "角度:%u", send_Data.b);
        lv_label_set_text(slider_label, buf);

        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
        #endif
    }
}

static void slider_light_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        static char buf[20]; /* max 3 bytes for number plus 1 null terminating byte */
        static int  value;

        value = lv_slider_get_value(slider);

        snprintf(buf, 20, "杭州:%u", value);
        lv_label_set_text(slider_light_label, buf);

        ttgo->bl->adjust( value );

        //@-写入NVS
        Setup_NVS(4,"backlight_value", "NULL", value);   

    }
}

//@-textare 
void ta_event_cb(lv_obj_t * ta, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        /* Focus on the clicked text area */
        if(kb != NULL)
        {
            lv_obj_set_hidden(kb,false);
            lv_keyboard_set_textarea(kb, ta);
        }

    }

    // else if(event == LV_EVENT_INSERT) {
    //     char * str = lv_event_get_data();
    //     // if(str[0] == '\n') {
    //     //     // printf("Ready\n");
    //     // }
    // }
}

//@-keyboard
void kb_event_cb(lv_obj_t * event_kb, lv_event_t event)
{
    if(event == LV_EVENT_APPLY ) {
      Serial.println("keyboard---> LV_EVENT_APPLY\n");
    }
    else if(event == LV_EVENT_CANCEL ) {
      Serial.println("keyboard---> LV_EVENT_CANCEL\n");
    //   lv_obj_del(kb);
      lv_obj_set_hidden(kb,true);
    }
    
    /* Just call the regular event handler */
    lv_keyboard_def_event_cb(event_kb, event);
    
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

    static lv_style_t led7_style_black;
    lv_style_init(&led7_style_black);
    lv_style_set_text_color(&led7_style_black, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    // lv_style_set_text_color(&led7_style_black, LV_STATE_DEFAULT, lv_color_hex(0x4E7AC5));
    lv_style_set_text_font(&led7_style_black, LV_STATE_DEFAULT, &dxLED7);

    static lv_style_t led7_style_red;
    lv_style_init(&led7_style_red);
    lv_style_set_text_color(&led7_style_red, LV_STATE_DEFAULT, lv_color_hex(0x2ADD9C));
    lv_style_set_text_font(&led7_style_red, LV_STATE_DEFAULT, &dxLED7);

    static lv_style_t led7_big_style;
    lv_style_init(&led7_big_style);
    lv_style_set_text_color(&led7_big_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&led7_big_style, LV_STATE_DEFAULT, &dxLED7_60);

    static lv_style_t style_cont1;
    lv_style_init(&style_cont1);
    lv_style_set_bg_opa(&style_cont1, LV_STATE_DEFAULT, LV_OPA_COVER);
    // lv_style_set_bg_color(&style_cont1, LV_STATE_DEFAULT, lv_color_hex(0x00E500));
    lv_style_set_bg_color(&style_cont1, LV_STATE_DEFAULT, lv_color_hex(0xFFC64B));

    static lv_style_t style_cont2;
    lv_style_init(&style_cont2);
    lv_style_set_bg_opa(&style_cont2, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_cont2, LV_STATE_DEFAULT, lv_color_hex(0x0AA344));

    static lv_style_t style_cont3;
    lv_style_init(&style_cont3);
    lv_style_set_bg_opa(&style_cont3, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_cont3, LV_STATE_DEFAULT, lv_color_hex(0xF47983));

    static lv_style_t style_cont4;
    lv_style_init(&style_cont4);
    lv_style_set_bg_opa(&style_cont4, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_bg_color(&style_cont4, LV_STATE_DEFAULT, lv_color_hex(0x801DAE));


    // static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {1,0}, {1,1}};
    static lv_point_t valid_pos[] = {{0,0}, {0, 1}, {0,2}, {0,3}, {0,4},{1,0}, {1,1}, {1,2}, {1,3}, {1,4}};
    tileview = lv_tileview_create(lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(tileview, valid_pos, 10);
    lv_tileview_set_edge_flash(tileview, true);

    tile_0_0 = lv_obj_create( tileview, NULL);
    tile_0_1 = lv_obj_create( tileview, NULL);
    tile_0_2 = lv_obj_create( tileview, NULL);
    tile_0_3 = lv_obj_create( tileview, NULL);
    tile_0_4 = lv_obj_create( tileview, NULL);
    tile_1_0 = lv_obj_create( tileview, NULL);
    tile_1_1 = lv_obj_create( tileview, NULL);
    tile_1_2 = lv_obj_create( tileview, NULL);
    tile_1_3 = lv_obj_create( tileview, NULL);
    tile_1_4 = lv_obj_create( tileview, NULL);

    lv_obj_set_size( tile_0_0, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_0_1, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_0_2, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_0_3, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_0_4, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_0, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_1, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_2, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_3, LV_HOR_RES,LV_VER_RES);
    lv_obj_set_size( tile_1_4, LV_HOR_RES,LV_VER_RES);

    lv_obj_set_pos( tile_0_0, 0,   0);
    lv_obj_set_pos( tile_0_1, 0,   LV_VER_RES);
    lv_obj_set_pos( tile_0_2, 0,   2*LV_VER_RES);
    lv_obj_set_pos( tile_0_3, 0,   3*LV_VER_RES);
    lv_obj_set_pos( tile_0_4, 0,   4*LV_VER_RES);
    lv_obj_set_pos( tile_1_0, LV_HOR_RES,   0);
    lv_obj_set_pos( tile_1_1, LV_HOR_RES,   LV_VER_RES);
    lv_obj_set_pos( tile_1_2, LV_HOR_RES,   2*LV_VER_RES);
    lv_obj_set_pos( tile_1_3, LV_HOR_RES,   3*LV_VER_RES);
    lv_obj_set_pos( tile_1_4, LV_HOR_RES,   4*LV_VER_RES);

    // lv_obj_set_event_cb(tile_1_2, event_handler);

    lv_tileview_add_element(tileview, tile_0_0 );
    lv_tileview_add_element(tileview, tile_0_1 );
    lv_tileview_add_element(tileview, tile_0_2 );
    lv_tileview_add_element(tileview, tile_0_3 );
    lv_tileview_add_element(tileview, tile_0_4 );
    lv_tileview_add_element(tileview, tile_1_0 );
    lv_tileview_add_element(tileview, tile_1_1 );
    lv_tileview_add_element(tileview, tile_1_2 );
    lv_tileview_add_element(tileview, tile_1_3 );
    lv_tileview_add_element(tileview, tile_1_4 );

    if(Set_Alarm_Run_Flag == false)
    lv_tileview_set_tile_act(tileview, 1, 2, LV_ANIM_OFF);
    else if(Set_Alarm_Run_Flag == true)
    lv_tileview_set_tile_act(tileview, 1, 3, LV_ANIM_OFF);

    //------------------------------tile_0_0-----------------------------------------------------
    /* Create the text area */
    ta = lv_textarea_create(tile_0_0, NULL);
    lv_obj_set_size(ta,  100, 30);
    lv_obj_align(ta, NULL, LV_ALIGN_CENTER, -60, -60);
    lv_obj_set_event_cb(ta, ta_event_cb);
    // lv_textarea_set_accepted_chars(ta, "0123456789:");
    lv_textarea_set_max_length(ta, 10);
    lv_textarea_set_one_line(ta, true);
    lv_textarea_set_text(ta, "");

    /* Create the password box */
    pwd_ta = lv_textarea_create(tile_0_0, NULL);
    lv_obj_set_size(pwd_ta, 100, 30);
    lv_obj_align(pwd_ta, NULL, LV_ALIGN_CENTER, 50, -60);
    lv_obj_set_event_cb(pwd_ta, ta_event_cb);
    lv_textarea_set_text(pwd_ta, "");
    lv_textarea_set_pwd_mode(pwd_ta, true);
    lv_textarea_set_one_line(pwd_ta, true);
    lv_textarea_set_cursor_hidden(pwd_ta, true);

    /* Create a keyboard*/
    kb = lv_keyboard_create(tile_0_0, NULL);
    lv_obj_set_size(kb,  LV_HOR_RES, 120);
    lv_obj_align(kb, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_event_cb(kb, kb_event_cb); /* Setting a custom event handler stops the keyboard from closing automatically */
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUM);
    lv_keyboard_set_textarea(kb, ta);/* Focus it on one of the text areas to start */
    lv_keyboard_set_cursor_manage(kb, true); /* Automatically show/hide cursors on text areas */


    btn = lv_btn_create(tile_0_0, NULL);
    lv_obj_set_event_cb(btn, event_handler);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 10);

    lv_obj_t *label_0_0 = lv_label_create( btn, NULL);
    lv_obj_add_style(label_0_0, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text( label_0_0, "0-中国"); 
    lv_obj_align( label_0_0, NULL, LV_ALIGN_CENTER,0,0);

    label_data = lv_label_create( tile_0_0, NULL);
    lv_obj_add_style(label_data, LV_OBJ_PART_MAIN, &model_style);
    #ifdef USE_ESP_NOW
    lv_label_set_text_fmt(label_data, "Value: %d", recv_Data.b);
    #else
    lv_label_set_text(label_data, "启动");
    #endif

    lv_obj_align( label_data, NULL, LV_ALIGN_CENTER,0,45);
    
    //------------------------------tile_0_1-----------------------------------------------------
    btn10 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn10, 60);
    lv_obj_set_event_cb(btn10, event_handler);
    lv_obj_align(btn10, NULL, LV_ALIGN_IN_TOP_MID, -30, 10);  //0

    btn20 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn20, 60);
    lv_obj_set_event_cb(btn20, event_handler);
    lv_obj_align(btn20, btn10, LV_ALIGN_IN_TOP_MID, 65, 0);  //180

    btn30 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn30, 60);
    lv_obj_set_event_cb(btn30, event_handler);
    lv_obj_align(btn30, NULL, LV_ALIGN_IN_TOP_MID, -30, 70);  //+

    btn40 = lv_btn_create(tile_0_1, NULL);
    lv_obj_set_width(btn40, 60);
    lv_obj_set_event_cb(btn40, event_handler);
    lv_obj_align(btn40, btn30, LV_ALIGN_IN_TOP_MID, 65, 0);  //-

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

    //@-调整屏幕背光
    lv_obj_t * slider_light = lv_slider_create(tile_0_1, NULL);
    lv_obj_set_width(slider_light, LV_DPI * 1);
    lv_obj_align(slider_light, slider_label, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_event_cb(slider_light, slider_light_event_cb);
    lv_slider_set_range(slider_light, DISPLAY_MIN_BRIGHTNESS, DISPLAY_MAX_BRIGHTNESS);

    /* Create a label below the slider */
    slider_light_label = lv_label_create(tile_0_1, NULL);
    lv_obj_add_style(slider_light_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(slider_light_label, "杭州:0");
    lv_obj_set_auto_realign(slider_light_label, true);
    lv_obj_align(slider_light_label, slider_light, LV_ALIGN_CENTER, 0, 15);

    //------------------------------tile_0_2-----------------------------------------------------
    //@-wifi
    /*Create a normal drop down list*/
    wifi_list = lv_dropdown_create(tile_0_2, NULL);
    lv_dropdown_set_show_selected(wifi_list, false);
    lv_dropdown_set_text(wifi_list, "WIFI");
    lv_dropdown_set_options(wifi_list, "...Searching...");
    lv_obj_align(wifi_list, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 20);
    lv_obj_set_event_cb(wifi_list, wifi_list_event_handler);

    wifi_def_info_label = lv_label_create(tile_0_2, NULL);
    // lv_obj_add_style(wifi_def_info_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text_fmt(wifi_def_info_label, "#ff7500 saved wifi ssid is: %s", NVS_WIFI_SSID);
    lv_label_set_recolor(wifi_def_info_label, true); 
    lv_label_set_long_mode(wifi_def_info_label, LV_LABEL_LONG_SROLL_CIRC); /* Make sure text will wrap */
    lv_obj_set_width(wifi_def_info_label, LV_HOR_RES - 100);
    lv_obj_align(wifi_def_info_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 60);

    wifi_scan_btn = lv_btn_create(tile_0_2, NULL);
    lv_obj_set_width(wifi_scan_btn, 80);
    lv_obj_set_event_cb(wifi_scan_btn, event_handler);
    lv_obj_align(wifi_scan_btn, NULL, LV_ALIGN_IN_TOP_MID, -40, 90);  
    lv_obj_set_hidden(wifi_scan_btn,false); 

    wifi_save_btn = lv_btn_create(tile_0_2, NULL);
    lv_obj_set_width(wifi_save_btn, 80);
    lv_obj_set_event_cb(wifi_save_btn, event_handler);
    lv_obj_align(wifi_save_btn, NULL, LV_ALIGN_IN_TOP_MID, -40, 150);  
    lv_obj_set_hidden(wifi_save_btn,false); 

    wifi_connect_btn = lv_btn_create(tile_0_2, NULL);
    lv_obj_set_width(wifi_connect_btn, 80);
    lv_obj_set_height(wifi_connect_btn, 100);
    lv_obj_set_event_cb(wifi_connect_btn, event_handler);
    lv_obj_align(wifi_connect_btn, NULL, LV_ALIGN_IN_TOP_MID, 50, 90); 
    lv_obj_set_hidden(wifi_connect_btn,false); 

    lv_obj_t * label_temp = lv_label_create(wifi_scan_btn, NULL);
    lv_obj_add_style(label_temp, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label_temp, "SCAN");

    label_temp = lv_label_create(wifi_save_btn, NULL);
    lv_obj_add_style(label_temp, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label_temp, "SAVE");

    label_temp = lv_label_create(wifi_connect_btn, NULL);
    lv_obj_add_style(label_temp, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label_temp, "CONNT");

    wifi_bg_bottom = lv_obj_create(tile_0_2, NULL);
    lv_obj_clean_style_list(wifi_bg_bottom, LV_OBJ_PART_MAIN);
    lv_obj_set_style_local_bg_opa(wifi_bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_COVER);
    lv_obj_set_style_local_bg_color(wifi_bg_bottom, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,LV_COLOR_ORANGE);
    lv_obj_align(wifi_bg_bottom, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 10);
    // lv_obj_set_pos(wifi_bg_bottom, 0, 450);
    lv_obj_set_size(wifi_bg_bottom, LV_HOR_RES, 20);

    wifi_connect_info_label = lv_label_create(tile_0_2, NULL);
    // lv_obj_add_style(wifi_connect_info_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(wifi_connect_info_label, "#34495e wifi not connect please scan the wifi to connect...");
    lv_label_set_recolor(wifi_connect_info_label, true); 
    lv_label_set_long_mode(wifi_connect_info_label, LV_LABEL_LONG_SROLL_CIRC); /* Make sure text will wrap */
    lv_obj_set_width(wifi_connect_info_label, LV_HOR_RES - 100);
    lv_obj_align(wifi_connect_info_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -13);

    //------------------------------tile_0_3-----------------------------------------------------
    weather_title_label = lv_label_create(tile_0_3, NULL);
    // lv_obj_add_style(weather_title_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(weather_title_label, "#34495e Hangzhou Weather");
    lv_label_set_recolor(weather_title_label, true); 
    // lv_label_set_long_mode(weather_title_label, LV_LABEL_LONG_SROLL_CIRC); /* Make sure text will wrap */
    // lv_obj_set_width(weather_title_label, LV_HOR_RES - 100);
    lv_obj_align(weather_title_label, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    weather_info_label = lv_label_create(tile_0_3, NULL);
    // lv_obj_add_style(weather_info_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(weather_info_label, " NULL ");
    lv_label_set_recolor(weather_info_label, true); 
    lv_obj_align(weather_info_label, NULL, LV_ALIGN_CENTER, 0, 0);

    weatcher_get_btn = lv_btn_create(tile_0_3, NULL);
    lv_obj_set_width(weatcher_get_btn, 160);
    lv_obj_set_event_cb(weatcher_get_btn, event_handler);
    lv_obj_align(weatcher_get_btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 180);  

    lv_obj_t * weather_title_label = lv_label_create(weatcher_get_btn, NULL);
    lv_label_set_text(weather_title_label, "Get Weather Data");

    //------------------------------tile_0_4-----------------------------------------------------
    chart = lv_chart_create(tile_0_4, NULL);
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

    //------------------------------tile_1_0-----------------------------------------------------
    lv_obj_t * img1 = lv_img_create(tile_1_0, NULL);
    lv_img_set_src(img1, &rich);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);

    //------------------------------tile_1_1-----------------------------------------------------
    lv_obj_t * cont1;
    cont1 = lv_cont_create(tile_1_1, NULL);
    // lv_obj_set_auto_realign(cont1, true);                    /*Auto realign when the size changes*/
    lv_obj_set_size(cont1, 100, 100);
    lv_obj_align_origo(cont1, NULL, LV_ALIGN_IN_TOP_LEFT, 60, 60);  /*This parametrs will be sued when realigned*/
    lv_obj_add_style(cont1, LV_OBJ_PART_MAIN, &style_cont1);
    // lv_cont_set_fit(cont1, LV_FIT_TIGHT);
    // lv_cont_set_layout(cont1, LV_LAYOUT_COLUMN_MID);
    // lv_cont_set_style(cont1,LV_CONT_PART_MAIN,&style_cont1);
    sw1 = lv_switch_create(cont1, NULL);
    lv_obj_align(sw1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(sw1, event_handler);

    lv_obj_t * cont2;
    cont2 = lv_cont_create(tile_1_1, NULL);
    // lv_obj_set_auto_realign(cont2, true);                    /*Auto realign when the size changes*/
    lv_obj_set_size(cont2, 100, 100);
    lv_obj_align_origo(cont2, NULL, LV_ALIGN_IN_TOP_RIGHT, -60, 60);  /*This parametrs will be sued when realigned*/
    lv_obj_add_style(cont2, LV_OBJ_PART_MAIN, &style_cont2);
    sw2 = lv_switch_create(cont2, NULL);
    lv_obj_align(sw2, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(sw2, event_handler);

    lv_obj_t * cont3;
    cont3 = lv_cont_create(tile_1_1, NULL);
    // lv_obj_set_auto_realign(cont3, true);                    /*Auto realign when the size changes*/
    lv_obj_set_size(cont3, 100, 100);
    lv_obj_align_origo(cont3, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 60, -60);  /*This parametrs will be sued when realigned*/
    lv_obj_add_style(cont3, LV_OBJ_PART_MAIN, &style_cont3);
    sw3 = lv_switch_create(cont3, NULL);
    lv_obj_align(sw3, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(sw3, event_handler);

    lv_obj_t * cont4;
    cont4 = lv_cont_create(tile_1_1, NULL);
    // lv_obj_set_auto_realign(cont3, true);                    /*Auto realign when the size changes*/
    lv_obj_set_size(cont4, 100, 100);
    lv_obj_align_origo(cont4, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -60, -60);  /*This parametrs will be sued when realigned*/
    lv_obj_add_style(cont4, LV_OBJ_PART_MAIN, &style_cont4);
    sw4 = lv_switch_create(cont4, NULL);
    lv_obj_align(sw4, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(sw4, event_handler);


    // lv_obj_add_style(label_1_1, LV_OBJ_PART_MAIN, &model_style);
    // lv_label_set_text( label_1_1, "4-关闭"); 
    // lv_obj_align( label_1_1, NULL, LV_ALIGN_CENTER,0,0);

    //------------------------------tile_1_2-----------------------------------------------------
    lv_obj_t * img = lv_img_create(tile_1_2, NULL);
    lv_img_set_src(img, &TTGO_Main);
    lv_obj_align(img, NULL, LV_ALIGN_CENTER, 0, 0);

    label_wifi = lv_label_create(tile_1_2, NULL);
    lv_label_set_text(label_wifi, LV_SYMBOL_WIFI);
    lv_obj_align( label_wifi, NULL, LV_ALIGN_IN_TOP_RIGHT,-2,5); 
    lv_obj_set_hidden(label_wifi,true);  

    label_time = lv_label_create( tile_1_2, NULL);
    lv_obj_add_style(label_time, LV_OBJ_PART_MAIN, &led7_big_style);
    lv_label_set_text( label_time, "--:--"); 
    #ifdef MAIN_BACKPIC_NUM_1
    lv_obj_align( label_time, NULL, LV_ALIGN_IN_TOP_LEFT,0,91);
    #endif
    #ifdef MAIN_BACKPIC_NUM_2
    lv_obj_align( label_time, NULL, LV_ALIGN_IN_TOP_LEFT,60,150);  //90,150
    #endif
    #ifdef MAIN_BACKPIC_NUM_3
    lv_obj_align( label_time, NULL, LV_ALIGN_IN_TOP_LEFT,35,60);  
    #endif

    label_time_second = lv_label_create( tile_1_2, NULL);
    lv_obj_add_style(label_time_second, LV_OBJ_PART_MAIN, &led7_style_black);
    lv_label_set_text( label_time_second, "--"); 
    #ifdef MAIN_BACKPIC_NUM_3
    lv_obj_align( label_time_second, NULL, LV_ALIGN_IN_TOP_LEFT,160,85);  
    #endif

    label_time_date = lv_label_create( tile_1_2, NULL);
    lv_obj_add_style(label_time_date, LV_OBJ_PART_MAIN, &led7_style_black);
    // lv_label_set_text( label_time_date, "----/--/--  --");
    lv_label_set_text( label_time_date, "--/-- --"); 
    #ifdef MAIN_BACKPIC_NUM_1
    lv_obj_align( label_time_date, NULL, LV_ALIGN_IN_TOP_LEFT,20,140);
    #endif
    #ifdef MAIN_BACKPIC_NUM_2
    lv_obj_align( label_time_date, NULL, LV_ALIGN_IN_TOP_LEFT,110,10);
    #endif
    #ifdef MAIN_BACKPIC_NUM_3
    lv_obj_align( label_time_date, NULL, LV_ALIGN_IN_TOP_LEFT,60,117);
    #endif

    label_batt = lv_label_create( tile_1_2, NULL);
    lv_obj_add_style(label_batt, LV_OBJ_PART_MAIN, &led7_style_red);
    lv_label_set_text( label_batt, "--"); 
    #ifdef MAIN_BACKPIC_NUM_1
    lv_obj_align( label_batt, NULL, LV_ALIGN_IN_BOTTOM_RIGHT,-25,-10);
    #endif
    #ifdef MAIN_BACKPIC_NUM_2
    lv_obj_align( label_batt, NULL, LV_ALIGN_IN_BOTTOM_RIGHT,-20,-85);
    #endif
    #ifdef MAIN_BACKPIC_NUM_3
    lv_obj_align( label_batt, NULL, LV_ALIGN_IN_TOP_LEFT,5,210);
    #endif

    #ifdef MAIN_BACKPIC_NUM_3
    img_biaoyu = lv_img_create(tile_1_2, NULL);
    lv_img_set_src(img_biaoyu, &TTGO_Main_Biaoyu);
    lv_obj_align(img_biaoyu, NULL, LV_ALIGN_IN_TOP_LEFT, 29, 116);
    lv_obj_set_hidden(img_biaoyu,true);
    #endif

    //------------------------------tile_1_3-----------------------------------------------------
    /*Create a line meter */
    lmeter = lv_linemeter_create(tile_1_3, NULL);
    lv_linemeter_set_range(lmeter, 0, 99);                   /*Set the range*/
    lv_linemeter_set_value(lmeter, Alarm_Timer_Data);                       /*Set the current value*/
    lv_linemeter_set_scale(lmeter, 240, 20);                  /*Set the angle and number of lines*/
    lv_obj_set_size(lmeter, 150, 150);
    lv_obj_set_event_cb(lmeter, event_handler);
    lv_obj_align(lmeter, NULL, LV_ALIGN_CENTER, 0, 0);

    label_timer = lv_label_create( tile_1_3, NULL);
    lv_obj_add_style(label_timer, LV_OBJ_PART_MAIN, &led7_style_red);
    lv_label_set_text_fmt(label_timer, "%02d", Alarm_Timer_Data);
    lv_obj_align(label_timer, NULL, LV_ALIGN_CENTER, 0, 0);

    sw_timer_dir = lv_switch_create(tile_1_3, NULL);
    lv_obj_align(sw_timer_dir, NULL, LV_ALIGN_CENTER, 90, 50);
    lv_obj_set_event_cb(sw_timer_dir, event_handler);

    sw_timer_run = lv_switch_create(tile_1_3, NULL);
    lv_obj_align(sw_timer_run, NULL, LV_ALIGN_CENTER, 90, -50);
    lv_obj_set_event_cb(sw_timer_run, event_handler);

    //------------------------------tile_1_4-----------------------------------------------------
    //@-远程升级
    lv_obj_t * label_updateTemp = lv_label_create(tile_1_4, NULL);
    lv_obj_add_style(label_updateTemp, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label_updateTemp, "OnLine Firmware Update");
    lv_obj_align(label_updateTemp, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    firmware_update_btn = lv_btn_create(tile_1_4, NULL);
    lv_obj_set_width(firmware_update_btn, 100);
    lv_obj_set_height(firmware_update_btn, 60);
    lv_obj_set_event_cb(firmware_update_btn, event_handler);
    lv_obj_align(firmware_update_btn, NULL, LV_ALIGN_CENTER, 0, 0); 
    lv_obj_set_hidden(firmware_update_btn,false); 

    label_updateTemp = lv_label_create(firmware_update_btn, NULL);
    lv_obj_add_style(label_updateTemp, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text(label_updateTemp, "Update");

    firmwareUpdata_label = lv_label_create(tile_1_4, NULL);
    lv_obj_add_style(firmwareUpdata_label, LV_OBJ_PART_MAIN, &model_style);
    lv_label_set_text_fmt(firmwareUpdata_label, "Version:%s", __FIRMWARE__);
    lv_obj_align(firmwareUpdata_label, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

    /*Create a Preloader object*/
    // lv_obj_t * preload = lv_spinner_create(lv_scr_act(), NULL);
    // lv_obj_set_size(preload, 130, 130);
    // lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);



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
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        // esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ANY_HIGH);

        if(Alarm_Timer_Run == true)
        {
            // RTC_INT定义在twatch2020_v1.h #define RTC_INT 37
            esp_sleep_enable_ext0_wakeup(DX_RTC_INT, 0);
            // esp_sleep_enable_timer_wakeup(all_second * uS_TO_S_FACTOR);
        }

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
        // esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
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
        //@-显示复位
        display_time_bat_info_tick = 0;
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
        esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
        // esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ANY_HIGH);

        if(Alarm_Timer_Run == true)
        {
            // RTC_INT定义在twatch2020_v1.h #define RTC_INT 37
            esp_sleep_enable_ext0_wakeup(DX_RTC_INT, 0);
            // esp_sleep_enable_timer_wakeup(all_second * uS_TO_S_FACTOR);
        }

        //@-ttgo深度sleep模式工作
        esp_deep_sleep_start();
    }
}

//@-配置打开audio功能
void Turn_On_Audio()
{
    // file = new AudioFileSourcePROGMEM(image, sizeof(image));
    file = new AudioFileSourcePROGMEM(laozigun_robot, sizeof(laozigun_robot));
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

//@-设置RTC
void Setup_RTC()
{
   ttgo->rtc->disableAlarm();

   ttgo->rtc->setDateTime(2020, 9, 17, 9, 57, 10);

   ttgo->rtc->enableAlarm();
}

//@-配置NVS存储信息
void Setup_NVS(int opt, String key, String key_StrValue, int key_IntValue)
{
    //@-正常模式
    if(opt == 1)
    {
        NVS.begin();
        String NVSTime = NVS.getString("NVSTime");

        //@-初始化
        if(NVSTime.equals("dx-error"))
        {
            NVS.setString("NVSTime", "1");
            NVS.setString("version", "V1.0");
            NVS.setString("fireware_version", "TTGO.ino.esp32.bin");
            NVS.setString("wifi_ssid", "wuyiyi");
            NVS.setString("wifi_pass", "10238831");
            NVS.setString("oat_add_host", "www.dingxiao1023.com");
            NVS.setString("oat_add_path", "/media/images/TTGO.ino.esp32.bin");
            NVS.setInt("backlight_value", 30);
        }

        //@-读取NVS数据
        else
        {
            NVS_WIFI_SSID = NVS.getString("wifi_ssid");
            NVS_WIFI_PASS = NVS.getString("wifi_pass");
            NVS_OTA_ADD_HOST = NVS.getString("oat_add_host");
            NVS_OTA_ADD_PATH = NVS.getString("oat_add_path");
            NVS_Backlight_Value = NVS.getInt("backlight_value");
        }

        NVS.close();
    }

    //@-擦除NVS
    else if(opt == 2)
    {
        //@-擦除NVS
        NVS.begin();
        NVS.eraseAll();
        NVS.close();
    }

    //@-写入String Key
    else if(opt == 3)
    {
        NVS.begin();
        NVS.setString(key, key_StrValue);
        NVS.close();
    }

    //@-写入Int Key
    else if(opt == 4)
    {
        NVS.begin();
        NVS.setInt(key, key_IntValue);
        NVS.close();
    }

}

#ifdef USE_ESP_NOW
void Setup_ESP_NOW()
{
    //@-配置wifi的now功能 Payload is limited to 250 bytes. 
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
}
#endif


//@-配置
void setup()
{
    //@-串口初始化
    Serial.begin(115200);

    //@打印wakeup_reason
    Print_Wakeup_Reason();

    //@-配置NVS
    Setup_NVS(1, "NULL", "NULL", 0);

    Serial.println(NVS_Backlight_Value);

    #ifdef USE_ESP_NOW
    Setup_ESP_NOW();
    #endif

    //@-配置WIFI模式
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    //@-ttgo初始化
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();
    ttgo->bl->adjust( NVS_Backlight_Value );

    ttgo->motor_begin();
    ttgo->motor->onec();

    //@-设置RTC
    // Setup_RTC();

    #ifdef USE_MP3
    //!Turn on the audio power
    ttgo->enableLDO3();
    Turn_On_Audio();
    #endif

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

    //@-静态创建wifi消息框
    WIFI_makePassWordMsgBox();
    //@-静态创建wifi密码输入键盘
    WIFI_makeKeyboard();

    //@-进入lvgl主页面
    lv_ex_tileview_1();
    // lv_ex_keyboard_1();
    // lv_ex_img_1();
    // lv_ex_keyboard_1();
}

void Display_TimeBAT_Info()
{
    //@-显示年月日
    RTC_Date dt = ttgo->rtc->getDateTime();
    int DayOfWeek = ttgo->rtc->getDayOfWeek(dt.day, dt.month, dt.year);

    switch (DayOfWeek)
    {
    case 0:
        snprintf(display_buf, sizeof(display_buf), "%02d-%02d %s", dt.month, dt.day, "SUN");
        break;
    case 1:
        // snprintf(display_buf, sizeof(display_buf), "%02d-%02d-%02d %s", dt.year, dt.month, dt.day, "MON");
        snprintf(display_buf, sizeof(display_buf), "%02d-%02d %s", dt.month, dt.day, "MON");
        break;
    case 2:
        snprintf(display_buf, sizeof(display_buf), "%02d-%02d %s", dt.month, dt.day, "TUES");
        break;
    case 3:
        snprintf(display_buf, sizeof(display_buf), "%02d-%02d %s", dt.month, dt.day, "WED");
        break;
    case 4:
        snprintf(display_buf, sizeof(display_buf), "%02d-%02d %s", dt.month, dt.day, "THUR");
        break;
    case 5:
        snprintf(display_buf, sizeof(display_buf), "%02d-%02d %s", dt.month, dt.day, "FRI");
        break;
    case 6:
        snprintf(display_buf, sizeof(display_buf), "%02d-%02d %s", dt.month, dt.day, "SAT");
        break;

    default:
        break;
    }
    // sprintf(display_buf, "%s", ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_YYYY_MM_DD));
    lv_label_set_text_fmt( label_time_date, "%s", display_buf); 

    //@-显示实时电量
    if (power->isChargeing() == false)
    {
        lv_label_set_text_fmt( label_batt, "%d%%", power->getBattPercentage()); 
    }
    else
    {
        lv_label_set_text( label_batt, "IN"); 
    }
}

//@-检测闹钟
void check_alarm()
{
   if (rtcIrq) 
   {
       rtcIrq = 0;
       detachInterrupt(RTC_INT);
       ttgo->rtc->resetAlarm();
       Set_Alarm_Run_Flag = true;
       Serial.println("alarm.....");
   }

   if(Set_Alarm_Run_Flag == true)
   {
       if((mp3->isRunning()) == false)
       {
        Alarm_Run_Time = Alarm_Run_Time - 1;
        if(Alarm_Run_Time <= 0)
        Set_Alarm_Run_Flag = false;

        Run_MP3_Audio(&beep_24_mp3, sizeof(beep_24_mp3));
        ttgo->motor->onec();
       }
   }
}

//@-主循环
void loop()
{
    system_tick = system_tick + 1;
    display_time_bat_info_tick = display_time_bat_info_tick + 1;

    #ifdef USE_MP3
    if (mp3->isRunning()) 
    {
        if (!mp3->loop()) 
        mp3->stop();
    } 
    #endif

    //@-查询电源管理中断
    check_power_irq();

    //@-检测触摸功能
    check_touch_pro();

    //@-检测闹钟
    check_alarm();

    if(system_tick > 100)
    {
        system_tick = 0;

        // PCF_TIMEFORMAT_HM,
        // PCF_TIMEFORMAT_HMS,
        //@-显示实时时间
        sprintf(display_buf, "%s", ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_HM));
        lv_label_set_text_fmt( label_time, "%s", display_buf); 
        //@-显示实时时间-秒
        RTC_Date dt_temp = ttgo->rtc->getDateTime();
        sprintf(display_buf, "%02d", dt_temp.second);
        lv_label_set_text_fmt(label_time_second, "%s", display_buf); 

        //@-显示年月日及电量
        if(display_time_bat_info_tick < 5000)
        {
            #ifdef MAIN_BACKPIC_NUM_3
            lv_obj_set_hidden(img_biaoyu,true); 
            #endif

            
            Display_TimeBAT_Info();
        }
        else 
        {
            lv_label_set_text( label_time_date, " "); 
            lv_label_set_text( label_batt, " "); 

            #ifdef MAIN_BACKPIC_NUM_3
            lv_obj_set_hidden(img_biaoyu,false); 
            #endif
        }
        
        //@-显示WIFI连接状态
        if(WiFi.status() == WL_CONNECTED)
        lv_obj_set_hidden(label_wifi,false); 
        else
        lv_obj_set_hidden(label_wifi,true); 

        sprintf(display_buf, "%d--temp:%.2f", Getweather_tick,weather_temputer);
        lv_label_set_text(weather_info_label, display_buf); 
    }

    // lv_label_set_text_fmt(label_data, "Value: %d", recv_Data.b);

    lv_task_handler();

    delay(5);
}
