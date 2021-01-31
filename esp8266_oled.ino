#include <ESP8266WiFi.h>
#include <Wire.h>
#include "time.h"
#include <Ticker.h>
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include <ESP8266_Heweather.h>  
#include "oledfont.h"
#include "selecticon.h"
#include "weathericons.h"
#include "selectweatherfont.h"


SSD1306Wire  display(0x3c, 2, 0);
OLEDDisplayUi ui     ( &display );

const char* WIFI_SSID = "zb";  //WIFI名称及密码
const char* WIFI_PWD = "571388081";

Ticker tick_key; //按键1扫描定时器

const String UserKey = "c2309af516b04492a222943c075683c3";   // 私钥 https://dev.heweather.com/docs/start/get-api-key
const String Location = "101280101"; // 城市代码 https://github.com/heweather/LocationList,表中的 Location_ID
const String Unit = "m";             // 公制-m/英制-i
const String Lang = "en";            // 语言 英文-en/中文-zh
int ROUND = 15 * 60000;              // 更新间隔<分钟>实时天气API 10~20分钟更新一次
WeatherNow weatherNow;

String WEATHER_ICON; //天气图片
String TEMP; //温度
char WEATHER_TYPE[100]; //天气类型
float HUM;
int WIND = 0;



int length = 0;

String IP_ADDRESS; //IP地址
const int DISPLAY_LIGHT_MODE = 1; //显示模式 1：长亮，0：30秒后自动息屏

const String WDAY_NAMES[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //星期
#define TZ              8      // 时区为8
#define DST_MN          0      // 默认为0
#define TZ_MN           ((TZ)*60)   
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

int KEY_DOWN_COUNT = 0; //按键按下计数
int KEY_STATE_NOW =  0; //按键目前状态
int KEY_STATE  =     0; //按键即将判断的状态 1：短按，2：长按
int KEY_NUM  =       0; //按键编号

bool initflag = true;

void drawProgress(OLEDDisplay *display, int percentage, String label) {    //绘制进度
  display->clear();
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(64, 10, label);
  display->drawProgressBar(2, 28, 124, 10, percentage);
  display->display();
}

void upDate(OLEDDisplay *display) {  //首次天气更新
  drawProgress(display, 0, "");
  wificonnect();
  drawProgress(display, 33, "Please wait Updating weather...");
  configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com"); //ntp获取时间同步系统时间戳
  drawProgress(display, 66, "Please wait Updating forecasts...");
  weatherNow.config(UserKey, Location, Unit, Lang);
  if(weatherNow.get()){ // 获取天气更新
    int icon_code = weatherNow.getIcon();
    WEATHER_ICON = getIcons(String(icon_code));
    TEMP =  String(weatherNow.getTemp());
    String type = getWeatherType(icon_code);
    HUM = weatherNow.getHumidity();
    WIND = weatherNow.getWindScale();
    length = type.length() / 3;
    for(int i =0 ; i<type.length(); i++ ) {
      WEATHER_TYPE[i] = type[i];
    }
  }
  drawProgress(display, 100, "Wellcome...");
  delay(200);
}

void drawIpOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  int x,y;
  display->drawHorizontalLine(0, 54, 128);//画横线
  // display->setTextAlignment(TEXT_ALIGN_CENTER);
  // display->setFont(ArialMT_Plain_10);
  // display->drawString(64+x, 54+y, "IP:"+IP_ADDRESS);
}

void drawDateTime(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  time_t now = time(nullptr);
  struct tm* timeInfo = localtime(&now);
  char buff[16];
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  sprintf_P(buff, PSTR("%02d-%02d                   "), timeInfo->tm_mon + 1, timeInfo->tm_mday);
  display->drawString(60 + x, 5 + y, String(buff));
  display->setFont(ArialMT_Plain_24);
  sprintf_P(buff, PSTR("%02d:%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  display->drawString(64 + x, 22 + y, String(buff));
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(62+x, 5+y, 16,16, xing);  
  display->drawXbm(79+x, 5+y, 16,16, qi);
  if (WDAY_NAMES[timeInfo->tm_wday]=="Mon")
    display->drawXbm(96+x, 5+y, 16,16, yi);
  else if (WDAY_NAMES[timeInfo->tm_wday]=="Tue")
    display->drawXbm(96+x, 5+y, 16,16, er);
  else if (WDAY_NAMES[timeInfo->tm_wday]=="Wed")
    display->drawXbm(96+x, 5+y, 16,16, san);
  else if (WDAY_NAMES[timeInfo->tm_wday]=="Thu")
    display->drawXbm(96+x, 5+y, 16,16, si);
  else if (WDAY_NAMES[timeInfo->tm_wday]=="Fri")
    display->drawXbm(96+x, 5+y, 16,16, wu);
  else if (WDAY_NAMES[timeInfo->tm_wday]=="Sat")
    display->drawXbm(96+x, 5+y, 16,16, liu);
  else if (WDAY_NAMES[timeInfo->tm_wday]=="Sun")
    display->drawXbm(96+x, 5+y, 16,16, ri);
}

void drawWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(Meteocons_Plain_36);
  display->drawString(x + 0, y+0  , WEATHER_ICON);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(DSEG7_Classic_Regular_32);
  display->drawString(x + 32, y + 2, TEMP);
  // display->setFont(Cousine_Regular_36);
  // display->drawString(x + 72, y + 2, "°C");
  display->drawXbm(x + 85, y + 2, 40, 40, c);
  int pos;
  if(length == 1) { pos = 16; }
  if(length == 2) { pos = 10; }
  if(length == 3) { pos = 4; }
  if(length >= 4) { pos = 0; }
  drawChinese(display, pos + x, 38 + y, WEATHER_TYPE);
  drawChinese(display, 60 + x, 38 + y, "湿度");
  display->setFont(Dialog_plain_12);
  display->drawString(x + 100, y + 38, ':' + String(int(HUM)) + '%');
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 11 + y, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 33 + y, "Right aligned (128,33)");
}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demo for drawStringMaxWidth:
  // with the third parameter you can define the width after which words will be wrapped.
  // Currently only spaces and "-" are allowed for wrapping
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 10 + y, 128, "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore.");
}

void drawFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->drawXbm(96+x, 5+y, 16,16, ri);
}

FrameCallback frames[] = { drawDateTime, drawWeather, drawFrame3, drawFrame4, drawFrame5 };
int frameCount = 5;

OverlayCallback overlays[] = { drawIpOverlay };
int overlaysCount = 1;

//IP地址转字符串
String ip2Str(IPAddress ip) { 
  String s = "";
  for (int i = 0; i < 4; i++) {
    s += i ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}



void keyScan() {
  if(digitalRead(3)==0 && digitalRead(1)!=0){
    KEY_DOWN_COUNT++;
    KEY_STATE_NOW = 1;
    if (KEY_DOWN_COUNT >= 10 && KEY_DOWN_COUNT < 1000) { //按键1计数
      KEY_STATE = 1;
    }
    if (KEY_DOWN_COUNT >= 1000) {
      KEY_STATE = 2;
    }
    KEY_NUM = 3;
  } else if(digitalRead(1)==0 && digitalRead(3)!=0){
    KEY_DOWN_COUNT++;
    KEY_STATE_NOW = 1;
    if (KEY_DOWN_COUNT >= 10 && KEY_DOWN_COUNT < 1000) { //按键2计数
      KEY_STATE = 1;
    }
    if (KEY_DOWN_COUNT >= 1000 && KEY_DOWN_COUNT < 5000) { 
      KEY_STATE = 2;
    }
    if (KEY_DOWN_COUNT >= 5000) {
      KEY_STATE = 3;
    }
    KEY_NUM = 1;
  } else {
    KEY_DOWN_COUNT = 0;
    KEY_STATE_NOW = 0;
  }
  if (KEY_STATE_NOW == 0 && KEY_STATE == 1){ //按键短按
    if(KEY_NUM == 3){
      ui.previousFrame();
    }
    if(KEY_NUM == 1){
      ui.nextFrame();
    }
    KEY_STATE = 0;
    KEY_NUM = 0;
  }
  if (KEY_STATE_NOW == 0 && KEY_STATE == 2){ //按键长按
    KEY_STATE = 0;
  }
  if(KEY_STATE_NOW == 0 && KEY_STATE == 3){
    display.displayOff();
    KEY_STATE = 0;
  }
}

void drawCNword(OLEDDisplay *display, int x0, int y0, const char chinese[3]){
  int x,y;
  for(int i = 0; i < 45; i++) {
    if(CN12LIST[i].Index[0] == chinese[0] && CN12LIST[i].Index[1] == chinese[1] && CN12LIST[i].Index[2] == chinese[2]) {
      display->setTextAlignment(TEXT_ALIGN_CENTER);
      display->drawXbm(x + x0, y + y0, 12, 12, CN12LIST[i].cn12_id);
    }
  }
}

void drawChinese(OLEDDisplay *display, int x, int y, const char chinese[]) {
  int x0 = x;
  int i = 0;
  while (i < strlen(chinese)){
    drawCNword(display, x0, y, chinese+i);
    x0+=12;
    i+=3;
  }
}

void wificonnect() {  //WIFI连接
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(80);
  }
  delay(500);
}

void setup() {
  display.init();
  display.clear();
  display.display();
  display.flipScreenVertically();
  upDate(&display);
  pinMode(3, INPUT);
  pinMode(1, INPUT);
  
  
  tick_key.attach_ms(1, keyScan);
  
  
  IP_ADDRESS = ip2Str(WiFi.localIP());

  ui.setTargetFPS(45);
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  // ui.disableAllIndicators();
  ui.disableAutoTransition();
  ui.setIndicatorPosition(BOTTOM);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.setFrames(frames, frameCount);
  ui.setOverlays(overlays, overlaysCount);
  ui.init();
  display.flipScreenVertically();
}


void loop() {
  display.clear();
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    delay(remainingTimeBudget);
  }
}


