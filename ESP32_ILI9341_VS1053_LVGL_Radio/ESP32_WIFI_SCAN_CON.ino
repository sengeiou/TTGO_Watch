#include <lvgl.h>


// lv_obj_t * wifi_kb;
// lv_obj_t * list1;
// lv_obj_t * list_btn[10];
// lv_obj_t * list_btn_dx_down;
// lv_obj_t * list_btn_dx_up;
// lv_obj_t * list_btn_dx_back;
// lv_obj_t * btn_dx_wifi_scan;
// lv_obj_t * pwd_ta1;
// lv_obj_t * wifi_connect_btn_back;

// bool task_start_flag = false;
// int wifi_scan_num;
// int wifi_connect_index = 0;

// struct DX_WIFI_S{
//     String WIFI_SSID;
//     String WIFI_RSSI;
//     String WIFI_TYPE;
//     String WIFI_PASS;
// };

// struct Connect_WIFI_Status_S{
//     int WIFI_Connect_Status;    //0:Not Scan 1:Scaning 
//     int WIFI_Status;            //0:Not Connect 1:Connecting 2:Connected
//     String WIFI_Connect_IP;
// };

// DX_WIFI_S DX_WIFI[20]; 
// Connect_WIFI_Status_S Connect_Wifi_Status;



void wifi_kb_event_cb(lv_obj_t * event_kb, lv_event_t event)
{
    if(event == LV_EVENT_APPLY ) {

      Serial.println("keyboard--->");
      DX_WIFI[wifi_connect_index].WIFI_PASS = lv_ta_get_text(pwd_ta1);
      Serial.println(DX_WIFI[wifi_connect_index].WIFI_PASS);

      // lv_ta_get_text(pwd_label);

      lv_wifi_5_connect();

    }
    
    /* Just call the regular event handler */
    lv_kb_def_event_cb(event_kb, event);
    
}

void wifi_ta_event_cb(lv_obj_t * ta, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        /* Focus on the clicked text area */
        if(kb != NULL)
            lv_kb_set_ta(kb, ta);
    }

    // else if(event == LV_EVENT_INSERT) {
    //     char * str = lv_event_get_data();
    //     // if(str[0] == '\n') {
    //     //     // printf("Ready\n");
    //     // }
    // }
}

void dx_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {

      if(obj == list_btn[0])
      {
         Serial.println("list_btn1");  
         wifi_connect_index = 0;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[1])
      {
         Serial.println("list_btn2");  
         wifi_connect_index = 1;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[2])
      {
         Serial.println("list_btn3");  
         wifi_connect_index = 2;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[3])
      {
         Serial.println("list_btn4");  
         wifi_connect_index = 3;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[4])
      {
         Serial.println("list_btn5");  
         wifi_connect_index = 4;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[5])
      {
         Serial.println("list_btn6");  
         wifi_connect_index = 5;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[6])
      {
         Serial.println("list_btn7");  
         wifi_connect_index = 6;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[7])
      {
         Serial.println("list_btn8");  
         wifi_connect_index = 7;
         lv_wifi_4_input_pass();
      }
      else if(obj == list_btn[8])
      {
         Serial.println("list_btn9"); 
         wifi_connect_index = 8;
         lv_wifi_4_input_pass(); 
      }
      else if(obj == list_btn[9])
      {
         Serial.println("list_btn10");  
         wifi_connect_index = 9;
         lv_wifi_4_input_pass();
      }

      //--------------------------------------------
      else if(obj == list_btn_dx_down)
      {
         lv_list_up(list1);
      }

      else if(obj == list_btn_dx_up)
      {
         lv_list_down(list1);
      }

      else if(obj == list_btn_dx_back)
      {
         lv_wifi_1_scan();
      }

      //--------------------------------------------
      else if(obj == btn_dx_wifi_scan)
      {
         lv_wifi_2_wait_scan();
         dx_task = lv_task_create(start_wifi_scan_task, 1000, LV_TASK_PRIO_MID, &user_data);
      }

      //--------------------------------------------
      else if(obj == wifi_connect_btn_back)
      {
        lv_wifi_1_scan();
      }

      //--------------------------------------------
      else if(obj == btn_dx_radio)
      {
        if(Connect_Wifi_Status.WIFI_Status == 2)
        {
            Radio_Mode = 1;
            station_connect(radio_cannel);  
            lv_ex_mbox_2();
        }
      }
      
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
        // printf("Toggled\n");
    }
}


void lv_wifi_1_scan(void)
{
    lv_obj_t * label;
    String str_temp;
    char char_temp[256];

    //@-清屏
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    btn_dx_wifi_scan = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_width(btn_dx_wifi_scan, 150);
    lv_obj_set_event_cb(btn_dx_wifi_scan, dx_event_handler);
    lv_obj_align(btn_dx_wifi_scan, NULL, LV_ALIGN_CENTER, 0, -40);
    label = lv_label_create(btn_dx_wifi_scan, NULL);
    lv_label_set_text(label, LV_SYMBOL_WIFI "  WIFI SCAN");

    btn_dx_radio = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_width(btn_dx_radio, 150);
    lv_obj_set_event_cb(btn_dx_radio, dx_event_handler);
    lv_obj_align(btn_dx_radio, NULL, LV_ALIGN_CENTER, 0, 40);
    label = lv_label_create(btn_dx_radio, NULL);
    lv_label_set_text(label, LV_SYMBOL_AUDIO "  START RADIO");

    // label = lv_label_create(btn_dx_wifi_scan, NULL);
    // lv_label_set_text_fmt(label, LV_SYMBOL_WIFI "  WIFI Num:%d", wifi_scan_num);
    // lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    /* Create a style for the shadow*/
    static lv_style_t label_style;
    lv_style_copy(&label_style, &lv_style_plain);
    label_style.text.opa = LV_OPA_50;
    label_style.text.font = &lv_font_roboto_12 ;  /*Set a larger font*/

    label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_recolor(label,true);
    lv_label_set_style(label, LV_LABEL_STYLE_MAIN, &label_style);

    if(Connect_Wifi_Status.WIFI_Status == 0)
    {
      // sprintf(temp, "[%s]--[%s]--[%s]  ", DX_WIFI[wifi_count].WIFI_SSID, DX_WIFI[wifi_count].WIFI_RSSI, DX_WIFI[wifi_count].WIFI_TYPE);

      sprintf(char_temp,LV_SYMBOL_WIFI,256);
      String mystring(char_temp);      //convert char array to string here.
      str_temp = String("#922B21 "+ mystring + " WIFI Not Connect #");
      str_temp.toCharArray(char_temp, str_temp.length());
      lv_label_set_text(label, char_temp);
    }
    else
    {
      sprintf(char_temp,LV_SYMBOL_WIFI,256);
      String mystring(char_temp);      //convert char array to string here.
      str_temp = String("#F4D0F3 "+mystring+ " WIFI Connected "+Connect_Wifi_Status.WIFI_Connect_IP + "#");
      str_temp.toCharArray(char_temp, str_temp.length());
      lv_label_set_text(label, char_temp);
    }
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 5);

}

void lv_wifi_2_wait_scan()
{
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    /*Create a style for the Preloader*/
    static lv_style_t style;
    lv_style_copy(&style, &lv_style_plain);
    style.line.width = 10;                         /*10 px thick arc*/
    style.line.color = lv_color_hex3(0x258);       /*Blueish arc color*/

    style.body.border.color = lv_color_hex3(0xBBB); /*Gray background color*/
    style.body.border.width = 10;
    style.body.padding.left = 0;

    /*Create a Preloader object*/
    lv_obj_t * preload = lv_preload_create(lv_scr_act(), NULL);
    lv_obj_set_size(preload, 100, 100);
    lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_preload_set_style(preload, LV_PRELOAD_STYLE_MAIN, &style);

    lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_recolor(label, true); 
    lv_label_set_text(label, "#58d68d Wifi Scan...#");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, -60);
}


void lv_wifi_3_scan_result(void)
{ 
    char temp[100];
    int wifi_count = 0;
    String str1;
    String str2;

    lv_obj_t * label;

    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, -60, 2);
    lv_label_set_recolor(label, true); 
    lv_label_set_text_fmt(label, LV_SYMBOL_WIFI"  #58d68d Scan Result Find# #E74C3C %d#", wifi_scan_num);

    /*Create a list*/
    list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 300, 170);
    lv_obj_align(list1, NULL, LV_ALIGN_IN_TOP_MID, 0, 25);
    lv_list_set_edge_flash(list1, true);

    
    for(wifi_count = 0; wifi_count < wifi_scan_num; wifi_count++)
    {
      // sprintf(temp, "[%s]--[%s]--[%s]  ", DX_WIFI[wifi_count].WIFI_SSID, DX_WIFI[wifi_count].WIFI_RSSI, DX_WIFI[wifi_count].WIFI_TYPE);
      // Serial.println(temp); 
      str2 = String((wifi_count+1));
      str1 = String(str2+"-->["+DX_WIFI[wifi_count].WIFI_SSID+"]--"+"["+DX_WIFI[wifi_count].WIFI_TYPE+"]");
      // Serial.println(temp1); 
      str1.toCharArray(temp, str1.length());
      // Serial.println(temp); 
      list_btn[wifi_count] = lv_list_add_btn(list1, LV_SYMBOL_WIFI, temp);
      lv_obj_set_event_cb(list_btn[wifi_count], dx_event_handler);
    }

    // Serial.println(DX_WIFI[1].WIFI_TYPE); 

    list_btn_dx_up = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(list_btn_dx_up, 80,40);
    lv_obj_set_event_cb(list_btn_dx_up, dx_event_handler);
    lv_obj_align(list_btn_dx_up, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 20, -3);
    label = lv_label_create(list_btn_dx_up, NULL);
    lv_label_set_text(label, LV_SYMBOL_UP);

    list_btn_dx_down = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(list_btn_dx_down, 80,40);
    lv_obj_set_event_cb(list_btn_dx_down, dx_event_handler);
    lv_obj_align(list_btn_dx_down, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 120, -3);
    label = lv_label_create(list_btn_dx_down, NULL);
    lv_label_set_text(label, LV_SYMBOL_DOWN);

    list_btn_dx_back = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(list_btn_dx_back, 80,40);
    lv_obj_set_event_cb(list_btn_dx_back, dx_event_handler);
    lv_obj_align(list_btn_dx_back, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -3);
    label = lv_label_create(list_btn_dx_back, NULL);
    lv_label_set_text(label, LV_SYMBOL_HOME);
}

void lv_wifi_4_input_pass(void)
{
    String str1;
    char temp[100];

    //@-清屏
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    /* Create the password box ---------------------------*/
    pwd_ta1 = lv_ta_create(lv_scr_act(), NULL);
    lv_ta_set_text(pwd_ta1, "");
    // lv_ta_set_pwd_mode(pwd_ta1, false);
    lv_ta_set_one_line(pwd_ta1, true);
    // lv_obj_set_width(pwd_ta, LV_HOR_RES / 2 - 20);
    lv_obj_set_width(pwd_ta1, LV_HOR_RES - 30);
    lv_obj_set_pos(pwd_ta1, 15, 25);
    lv_obj_set_event_cb(pwd_ta1, wifi_ta_event_cb);
    
    /* Create a label and position it above the text box */
    lv_obj_t * pwd_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_recolor(pwd_label, true); 
    str1 = String("#2471A3 "+DX_WIFI[wifi_connect_index].WIFI_SSID+"#   Enter Password");
    // Serial.println(str1);
    str1.toCharArray(temp, str1.length());
    lv_label_set_text(pwd_label, temp);
    lv_obj_align(pwd_label, pwd_ta1, LV_ALIGN_OUT_TOP_MID, 0, 0);
    
    /* Create a keyboard and make it fill the width of the above text areas */
    wifi_kb = lv_kb_create(lv_scr_act(), NULL);
    lv_obj_set_pos(wifi_kb, 5, 90);
    lv_obj_set_event_cb(wifi_kb, wifi_kb_event_cb); /* Setting a custom event handler stops the keyboard from closing automatically */
    lv_obj_set_size(wifi_kb,  LV_HOR_RES - 10, 140);
    
    lv_kb_set_ta(wifi_kb, pwd_ta1); /* Focus it on one of the text areas to start */
    lv_kb_set_cursor_manage(wifi_kb, true); /* Automatically show/hide cursors on text areas */
}

void lv_wifi_5_connect(void)
{
    //@-清屏
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_recolor(label, true); 
    lv_label_set_text(label, "#58d68d Wifi Connecting...#");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    // Serial.println(DX_WIFI[wifi_connect_index].WIFI_SSID);
    // Serial.println(DX_WIFI[wifi_connect_index].WIFI_PASS);

    dx_task = lv_task_create(start_wifi_connect_task, 1000, LV_TASK_PRIO_MID, &user_data);

}


//@-wifi加密类型转换
String translateEncryptionType(int encryptionType) {
  switch (encryptionType) {
    case (0):
      return "Open";
    case (1):
      return "WEP";
    case (2):
      return "WPA_PSK";
    case (3):
      return "WPA2_PSK";
    case (4):
      return "WPA_WPA2_PSK";
    case (5):
      return "WPA2_ENTERPRISE";
    default:
      return "UNKOWN";
    }
  }

//wifi scan 任务
void start_wifi_scan_task(lv_task_t * task)
{
  int temp = 0;

  wifi_scan_num = 0;

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);


  wifi_scan_num = WiFi.scanNetworks();

  delay(2000);

  if(wifi_scan_num > 10)
  wifi_scan_num = 10;

  if(wifi_scan_num == 0)
  {
    Serial.println("scan: 0");  
  }
  else
  {
    //@-保存扫描的WIFI节点
    for(temp = 0; temp < wifi_scan_num; temp++)
    {
        DX_WIFI[temp].WIFI_SSID = WiFi.SSID(temp);
        DX_WIFI[temp].WIFI_RSSI = WiFi.RSSI(temp);
        DX_WIFI[temp].WIFI_TYPE = translateEncryptionType(WiFi.encryptionType(temp));
        // DX_WIFI[temp].WIFI_TYPE = (WiFi.encryptionType(temp));

        // Serial.println(DX_WIFI[temp].WIFI_TYPE); 
    }

    // lv_wifi_1_scan();
    lv_wifi_3_scan_result();
    // Serial.println(wifi_scan_num);  
    
  }

  //@-任务删除
  lv_task_del(task);

}

//@-wifi 连接任务
void start_wifi_connect_task (lv_task_t * task)
{
   int task_tick = 0;
   String str_temp;
   char char_temp[50];

   Connect_Wifi_Status.WIFI_Connect_Status = 1;  //@-wifi尝试连接中
   Connect_Wifi_Status.WIFI_Status = 1;

   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   delay(100);
   
   DX_WIFI[wifi_connect_index].WIFI_SSID.toCharArray(ssid, (DX_WIFI[wifi_connect_index].WIFI_SSID.length() + 1));
   DX_WIFI[wifi_connect_index].WIFI_PASS.toCharArray(pass, (DX_WIFI[wifi_connect_index].WIFI_PASS.length() + 1));

  // Serial.println(ssid);
  // Serial.println(pass);
   
   WiFi.begin(ssid, pass);
   while (WiFi.status() != WL_CONNECTED) 
   {
    task_tick =  task_tick + 1;
    if(task_tick > 20)  //about 10s
    {
      break;
    }
    delay(500);
    Serial.print(".");
   }

   

  if(WiFi.status() == WL_CONNECTED)
  {
    Connect_Wifi_Status.WIFI_Status = 2;


    Connect_Wifi_Status.WIFI_Connect_IP = IpAddress2String(WiFi.localIP());
  }
  else
  {
    Connect_Wifi_Status.WIFI_Status = 0;
  }

   Connect_Wifi_Status.WIFI_Connect_Status = 0;  //@-wifi尝试连接中结束

   Serial.println(Connect_Wifi_Status.WIFI_Status);

  //@-创建连接提示信息label
  lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(label, true); 
  if(Connect_Wifi_Status.WIFI_Status == 2)
  str_temp = String("#58d68d Connected,IP:# #F4D0F3 "+Connect_Wifi_Status.WIFI_Connect_IP + "#");
  else
  str_temp = String("#E74C3C Connecting Fail#");
  str_temp.toCharArray(char_temp, str_temp.length());
  lv_label_set_text(label, char_temp);
  lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 20);

  //@-创建返回按键
  wifi_connect_btn_back = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_size(wifi_connect_btn_back, 80,40);
  lv_obj_set_event_cb(wifi_connect_btn_back, dx_event_handler);
  lv_obj_align(wifi_connect_btn_back, NULL, LV_ALIGN_CENTER, 0, 60);
  label = lv_label_create(wifi_connect_btn_back, NULL);
  lv_label_set_text(label, LV_SYMBOL_HOME);
  
  // Serial.println("WiFi connected");  
  // Serial.print("IP address:");  
  // Serial.println(WiFi.localIP());

  //@-任务删除
  lv_task_del(task);

}


String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}
