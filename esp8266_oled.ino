#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h> //闪存文件系统
#include <ArduinoJson.h>//json数据处理库（第三方）
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



/* ----------------------------------------------------------全局数据定义以及特殊变量声明--------------------------------------------------------------- */

ESP8266WebServer server(80); //创建Web服务端口为80
SSD1306Wire  display(0x3c, 2, 0);
OLEDDisplayUi ui     ( &display );

/* wifi连接与配网相关定义 */

const char* WIFI_SSID = "zb";  //WIFI名称及密码
const char* WIFI_PWD = "571388081";

/* 天气api相关定义 */

const String UserKey = "c2309af516b04492a222943c075683c3";   // 私钥 https://dev.heweather.com/docs/start/get-api-key
const String Location = "101280101";                         // 城市代码 https://github.com/heweather/LocationList,表中的 Location_ID
const String Unit = "m";                                     // 公制-m/英制-i
const String Lang = "en";                                    // 语言 英文-en/中文-zh
int ROUND = 15 * 60000;                                      // 更新间隔<分钟>实时天气API 10~20分钟更新一次
WeatherNow weatherNow;
String WEATHER_ICON;      //天气图片
char WEATHER_TYPE[100];   //天气类型
String TEMP;              //温度
float HUM;                //湿度
int length = 0;           //天气类型中文长度


/* 日期更新相关宏定义 */

const String WDAY_NAMES[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};  //星期
#define TZ              8 // 时区为8
#define DST_MN          0 // 默认为0
#define TZ_MN           ((TZ)*60)   
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)


Ticker tick_key;           //按键1扫描定时器
int KEY_DOWN_COUNT    = 0; //按键按下计数
int KEY_STATE_NOW     = 0; //按键目前状态
int KEY_STATE         = 0; //按键即将判断的状态 1：短按，2：长按
int KEY_NUM           = 0; //按键编号

/* 功能变量例如按键功能判断IP地址等 */

const int DISPLAY_LIGHT_MODE = 1; //显示模式 1：长亮，0：30秒后自动息屏
String IP_ADDRESS;                //IP地址

/* 进度条 */
int PERCENT = 1;

/* 是否配网 */
bool ISCONFIG = false;


/* ----------------------------------------------------------框架页面函数--------------------------------------------------------------- */

/* 已配网的覆盖层 */
void drawOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  int x,y;
  display->drawHorizontalLine(0, 54, 128);//画横线
}

void drawUnconfigOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  int x,y;
  // display->drawHorizontalLine(0, 54, 128);//画横线
}

/* 配网二维码页面 */
void drawQRCode(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->drawXbm(96+x, 5+y, 16,16, ri);
}

/* 日期页面 */
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

/* 天气页面 */
void drawWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(Meteocons_Plain_36);
  display->drawString(x + 0, y+0  , WEATHER_ICON);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(DSEG7_Classic_Regular_32);
  display->drawString(x + 35, y + 2, TEMP);
  // display->setFont(Cousine_Regular_36);
  // display->drawString(x + 72, y + 2, "°C");
  display->drawXbm(x + 88, y + 2, 40, 40, c);
  int pos;
  if(length == 1) { pos = 12; }
  if(length == 2) { pos = 10; }
  if(length == 3) { pos = 4; }
  if(length >= 4) { pos = 0; }
  drawChinese(display, pos + x, 38 + y, WEATHER_TYPE);
  drawChinese(display, 60 + x, 38 + y, "湿度");
  display->setFont(Dialog_plain_12);
  display->drawString(x + 88, y + 38, ':' + String(int(HUM)) + '%');
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 11 + y, "Left aligned (0,10)");
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Center aligned (64,22)");
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 33 + y, "Right aligned (128,33)");
}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 10 + y, 128, "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore.");
}

void drawFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->drawXbm(96+x, 5+y, 16,16, ri);
}
FrameCallback frames[] = { drawDateTime, drawWeather, drawFrame3, drawFrame4, drawFrame5 }; //已配网的页面框架
FrameCallback unconfigframes[] = { drawFrame5 }; //未配网的页面框架
int frameCount = 5;
OverlayCallback overlays[] = { drawOverlay }; //已配网的页面覆盖层
OverlayCallback unconfigoverlays[] = { drawUnconfigOverlay };
int overlaysCount = 1;



/* ----------------------------------------------------------WiFi配网--------------------------------------------------------------- */

/* 配置网络和初次更新信息 */

void configUpdate(OLEDDisplay *display) {
  String tag = "normal";
  drawProgress(display, 0, "open spiffs");
  if (SPIFFS.begin()) { // 打开闪存文件系统 
    Serial.println("闪存文件系统打开成功");
  } else {
     tag = "error";
     Serial.println("闪存文件系统打开失败");
  }
  if(SPIFFS.exists("/config.json")){ // 判断有没有config.json这个文件
    ISCONFIG = true;
    Serial.println("存在配置信息，正在自动连接");
    const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 60; //分配一个内存空间
    DynamicJsonDocument doc(capacity);// 声明json处理对象
    File configJson = SPIFFS.open("/config.json", "r");
    deserializeJson(doc, configJson); // json数据序列化
    const char* ssid = doc["ssid"];
    const char* password = doc["password"];
    WiFi.mode(WIFI_STA); // 更换wifi模式
    WiFi.begin(ssid, password); // 连接wifi
    while(WiFi.status()!= WL_CONNECTED){
      delay(500);
      Serial.println(".");  
    }
    configJson.close();
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("不存在配置信息，正在打开web配网模式"); 
    IPAddress softLocal(192,168,1,1);
    IPAddress softGateway(192,168,1,1);
    IPAddress softSubnet(255,255,255,0);
    WiFi.softAPConfig(softLocal, softGateway, softSubnet);
    WiFi.softAP("esp8266", "12345678"); //这里是配网模式下热点的名字和密码
    Serial.println(WiFi.softAPIP());
    ISCONFIG = false;
    frameCount = 1;
  }
  server.on("/", handleRoot);//web首页监听
  server.on("/set", handleConnect); // 配置ssid密码监听，感觉跟express的路由好像
  server.begin();
  delay(500);
  drawProgress(display, 20, tag);
  delay(1000);
  drawProgress(display, 50, "Please wait Updating forecasts...");
  configTime(TZ_SEC, DST_SEC, "ntp.ntsc.ac.cn", "ntp1.aliyun.com"); //ntp获取时间同步系统时间戳
  drawProgress(display, 80, "Updating...");
   weatherNow.config(UserKey, Location, Unit, Lang);
  if(weatherNow.get()){ // 获取天气更新
    int icon_code = weatherNow.getIcon();
    WEATHER_ICON = getIcons(String(icon_code));
    TEMP =  String(weatherNow.getTemp());
    String type = getWeatherType(icon_code);
    HUM = weatherNow.getHumidity();
    length = type.length() / 3;
    for(int i =0 ; i<type.length(); i++ ) {
      WEATHER_TYPE[i] = type[i];
    }
  }
  drawProgress(display, 100, "Wellcome...");
  delay(200);
}




/* ------------------------------------------------------------方法和数据更新----------------------------------------------------------------- */

/* 单个进度条方法 */
void drawProgress(OLEDDisplay *display, int progress, String label) {
  for (int i = PERCENT; i <= progress; i++ ) {
    display->clear();
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);
    display->drawString(64, 10, label);
    display->drawProgressBar(2, 28, 124, 10, i);
    display->display();
    delay(30);
  }
  PERCENT = progress;
}

/* IP地址转字符串 */
String ip2Str(IPAddress ip) { 
  String s = "";
  for (int i = 0; i < 4; i++) {
    s += i ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}

/* 按键扫描以及功能 */
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

/* 显示单个中文字符 */
void drawCNword(OLEDDisplay *display, int x0, int y0, const char chinese[3]){
  int x,y;
  for(int i = 0; i < 45; i++) {
    if(CN12LIST[i].Index[0] == chinese[0] && CN12LIST[i].Index[1] == chinese[1] && CN12LIST[i].Index[2] == chinese[2]) {
      display->drawXbm(x + x0, y + y0, 12, 12, CN12LIST[i].cn12_id);
    }
  }
}

/* 显示多个中文（无标点符号以及英文字母） */
void drawChinese(OLEDDisplay *display, int x, int y, const char chinese[]) {
  int x0 = x;
  int i = 0;
  while (i < strlen(chinese)){
    drawCNword(display, x0, y, chinese+i);
    x0+=12;
    i+=3;
  }
}

void handleRoot() { //展示网页的关键代码
  Serial.println("访问");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()){
    Serial.println(dir.fileName());
  }
  if(SPIFFS.exists("/index.html")){
      File index = SPIFFS.open("/index.html", "r");
      server.streamFile(index, "text/html");
      index.close();
  }  
}

void handleConnect() { //处理配置信息的函数
  String ssid = server.arg("ssid");   //arg是获取请求参数，视频中最后面展示了请求的完整链接
  String password = server.arg("password"); 
  server.send(200, "text/plain", "OK");
  WiFi.mode(WIFI_STA); //改变wifi模式
  WiFi.begin(ssid.c_str(), password.c_str());//String类型直接用会报错，不要问为什么，转成char *就行了。
  while(WiFi.status()!= WL_CONNECTED){
     delay(500);
     Serial.println(".");  
  }
  Serial.println(WiFi.localIP());
  removeConfig(); // 不管有没有配置先删除一次再说。
  String payload; // 拼接构造一段字符串形式的json数据长{"ssid":"xxxxx","password":"xxxxxxxxxxx"}
  payload += "{\"ssid\":\"";
  payload += ssid;
  payload +="\",\"password\":\"";
  payload += password;
  payload += "\"}";
  File wifiConfig = SPIFFS.open("/config.json", "w");
  wifiConfig.println(payload);//将数据写入config.json文件中
  wifiConfig.close();
}

void removeConfig(){
    if(SPIFFS.exists("/config.json")){ // 判断有没有config.json这个文件
      if (SPIFFS.remove("/config.json")){
         Serial.println("删除旧配置");
      } else {
        Serial.println("删除旧配置失败");
      } 
    }
}


/* ----------------------------------------------------------主进程--------------------------------------------------------------- */
void setup() {
  Serial.begin(115200);   
  Serial.println("");
  display.init();
  display.clear();
  display.display();
  display.flipScreenVertically();
  configUpdate(&display);
  // pinMode(3, INPUT_PULLUP);
  // pinMode(1, INPUT_PULLUP);
  tick_key.attach_ms(1, keyScan);
  IP_ADDRESS = ip2Str(WiFi.localIP());
  ui.setTargetFPS(45);
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  ui.disableAutoTransition();
  ui.setIndicatorPosition(BOTTOM);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  if (ISCONFIG) {
    ui.setFrames(frames, frameCount);
    ui.setOverlays(overlays, overlaysCount);
  } else {
    ui.disableAllIndicators();
    ui.setFrames(unconfigframes, frameCount);
    ui.setOverlays(unconfigoverlays, overlaysCount);
  }
  ui.init();
  display.flipScreenVertically();
}


/* ----------------------------------------------------------循环（定时更新数据）--------------------------------------------------------------- */
void loop() {
  display.clear();
  server.handleClient();
  int remainingTimeBudget = ui.update();
  if (remainingTimeBudget > 0) {
    delay(remainingTimeBudget);
  }
}


