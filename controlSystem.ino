/*
the code for yuanchenglu ControlSystem
 */
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht.h>  

//all port pin Define
int pinBuzzer=8;//pinBuzzer
int pinNTC = A0;    // NTC模拟输入引脚
int pinLM35 = 1;//定义模拟接口0 连接LM35 温度传感器
int pinBMP180 = 2;
int pinPH = 3;
int pinCO2 = 4;
int pinLed = 13;       // led指示灯引脚
#define ONE_WIRE_BUS 2//18B20 Data wire is plugged into port 2 on the Arduino

#define DHT11_PIN 4//put the sensor in the digital pin 4  
int trig = 8;    //ultraSoundDistance引脚定义 触发信号
int echo = 9;    //ultraSoundDistance引脚定义 反馈信号
// 初始化引脚
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const unsigned char simpFreq = 100;//ms


// 18B20 Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup() {
  pinMode(pinBuzzer,OUTPUT);

  pinMode(pinLed, OUTPUT);  
  Serial.begin(9600);  

  //18B20
  sensors.begin();// Start up the library of 18B20

  //DHT11
  Serial.println("DHT TEST PROGRAM ");  
  Serial.print("LIBRARY VERSION: ");  
  Serial.println(DHT_LIB_VERSION);  
  Serial.println();  
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");    

  //untraSoundDistance 触发端口设置为输出，反馈端口设置为输入
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);

  //lcd 设置行列值
  lcd.begin(16, 2);
  // 打印字符串
  lcd.print("Time(s)Dis(cm)Tem(c)Hum(ml)Pre(Pa)PH()CO2 by ycl");
}

void ledLight() {
  digitalWrite(pinLed, HIGH);   // led引脚置高电平
  delay(1000);               // 延时1秒
  digitalWrite(pinLed, LOW);    // led引脚变为低电平
  delay(1000);               // 延时1秒
}

void ledEnd() {
  pinMode(pinLed, OUTPUT);  
}

/*

 演示了如何使用16X2液晶显示屏。该LiquidCrystal
 库适用于所有的兼容HD44780的LCD液晶。常用的是16引脚。
 
 引脚定义，这里使用4位数据模式，只写数据，不读数据:
 * LCD RS pin to digital pin 12      //数据/指令控制引脚
 * LCD Enable pin to digital pin 11  //使能引脚
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground 接GND       //读/写控制引脚，高电平是读数据，低电平是写数据
 * 10K resistor:   //使用LY-51S开发板已经连接好这些硬件，值需要连接对应的端口，非常方便。
 * ends to +5V and ground VO引脚接电位器抽头，用于调节对比度，LY-51S开发板只需要调节W1电位器。
 * wiper to LCD VO pin (pin 3)
 面包板连接电路图和详细说明英文版：
 http://arduino.cc/en/Tutorial/LiquidCrystalDisplay
 使用独立模块开发板LY-51S连接详情：
 http://www.doflye.net/viewthread.php?tid=5293&extra=
 
 */
void lcdShow(float dis,float tem,float hum,float pre,float ph,float co2){
  // 设置光标到第二行第一列
  // (注意：1表示第二行，从0开始数):
  lcd.setCursor(0, 1);
  // 打印复位后的运行秒值
  lcd.print(millis()/1000);
  lcd.print("  ");
  lcd.print(dis);
  lcd.print("  ");
  lcd.print(tem);
  lcd.print("  ");
  lcd.print(hum);
  lcd.print("  ");
  lcd.print(pre);
  lcd.print("  ");
  lcd.print(ph);
  lcd.print("  ");
  lcd.print(co2);
}


void buzzerVoice(unsigned char times) { 
  unsigned char k,i,j;
  for(k=0;k<times;k++) 
  { 
    for(i=0;i<80;i++)
    { 
      digitalWrite(pinBuzzer,HIGH);
      delay(1);
      digitalWrite(pinBuzzer,LOW);
      delay(1);
    } 
    for(i=0;i<100;i++)
    { 
      digitalWrite(pinBuzzer,HIGH);
      delay(2);
      digitalWrite(pinBuzzer,LOW);
      delay(2);
    } 
  } 
}



float temperatureNTC() {
  float value = 0;
  // 读取电位器电压值
  value = analogRead(pinNTC);    
  Serial.print("temperatureNTC:");
  Serial.println(value,1); 
  // 点亮led，LY-51S独立模块开发板上led模块则是熄灭led
  digitalWrite(pinLed, HIGH);  
  // 使用读取的这个模拟量值作为演示时间，单位ms，范围0-1023
  delay(value);          
  // 熄灭led，LY-51S独立模块开发板上led模块则是点亮led 
  digitalWrite(pinLed, LOW);   
  // 使用读取的这个模拟量值作为演示时间，单位ms，范围0-1023
  delay(value);   

  return(value);  
}

float temperature18B20() { 
  float value = 0;
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  value = sensors.getTempCByIndex(0);
  Serial.print("Temperature18B20 for the device 1 (index 0) is: ");
  Serial.println(value);  
  //delay(200);

  return(value);
}

float temperatureLM35() {
  int val;//定义变量
  float dat;//定义变量
  val=analogRead(pinLM35);// 读取传感器的模拟值并赋值给val
  dat=(125*val)>>8;//温度计算公式
  Serial.print("temperatureLM35:");//原样输出显示Tep 字符串代表温度
  Serial.print(dat);//输出显示dat 的值
  Serial.println("C");//原样输出显示C 字符串
  //delay(500);//延时0.5 秒

  return(dat);
}


dht DHT;  
float temperatureDHT11() {  
  // READ DATA  
  Serial.print("DHT11, \t");  
  int chk = DHT.read11(DHT11_PIN);  
  switch (chk)  
  {  
  case 0:  
    Serial.print("OK,\t"); 
    break;  
  case -1: 
    Serial.print("Checksum error,\t"); 
    break;  
  case -2: 
    Serial.print("Time out error,\t"); 
    break;  
  default: 
    Serial.print("Unknown error,\t"); 
    break;  
  }  
  // DISPLAT DATA 
  Serial.print("temperatureDHT11:");
  Serial.println(DHT.temperature,1);    

  //delay(500);  
  float temp = 0;
  temp = DHT.temperature;
  return(temp);
}  

float humdidityDHT11() {  
  // READ DATA  
  Serial.print("DHT11, \t");  
  int chk = DHT.read11(DHT11_PIN);  
  switch (chk)  
  {  
  case 0:  
    Serial.print("OK,\t"); 
    break;  
  case -1: 
    Serial.print("Checksum error,\t"); 
    break;  
  case -2: 
    Serial.print("Time out error,\t"); 
    break;  
  default: 
    Serial.print("Unknown error,\t"); 
    break;  
  }  
  // DISPLAT DATA  
  Serial.print("humdidityDHT11:");
  Serial.println(DHT.humidity,1);  

  //delay(500);  
  float humd = 0;
  humd = DHT.temperature;
  return(humd);
}  

float ultraSoundDistance() {
  long IntervalTime=0; //定义一个时间变量
  digitalWrite(trig, 1);//置高电平
  delayMicroseconds(15);//延时15us
  digitalWrite(trig, 0);//设为低电平
  IntervalTime=pulseIn(echo, HIGH);//用自带的函数采样反馈的高电平的宽度，单位us
  float S=IntervalTime/58.00; //使用浮点计算出距离，单位cm
  Serial.println(S);//通过串口输出距离数值
  S=0;
  IntervalTime=0;//对应的数值清零。
  //delay(100);//延时间隔决定采样的频率，根据实际需要变换参数
  return(S);
}

/*
http://wiki.dfrobot.com.cn/index.php?title=(SKU:TOY0058)BMP180_%E5%A4%A7%E6%B0%94%E5%8E%8B%E5%BC%BA%E4%BC%A0%E6%84%9F%E5%99%A8
 */
float pressureBMP180(){
  int val;//定义变量
  float dat;//定义变量
  val=analogRead(pinBMP180);// 读取传感器的模拟值并赋值给val
  dat=(125*val)>>8;//温度计算公式
  Serial.print("pressureBMP180:");//原样输出显示Tep 字符串代表温度
  Serial.print(dat);//输出显示dat 的值
  Serial.println("Pa");//原样输出显示C 字符串
  //delay(500);//延时0.5 秒

  return(dat);
}

/*
http://wiki.dfrobot.com.cn/index.php?title=(SKU:SEN0169)Analog_pH_Meter_Pro_%E6%A8%A1%E6%8B%9FpH%E8%AE%A1%E4%B8%93%E4%B8%9A%E7%89%88
 */
float phSensor(){
  int val;//定义变量
  float dat;//定义变量
  val=analogRead(pinPH);// 读取传感器的模拟值并赋值给val
  dat=(125*val)>>8;//温度计算公式
  Serial.print("phSensor:");//原样输出显示Tep 字符串代表温度
  Serial.print(dat);//输出显示dat 的值
  Serial.println("m2");//原样输出显示C 字符串
  //delay(500);//延时0.5 秒

  return(dat);
}

/*
http://wiki.dfrobot.com.cn/index.php?title=(SKU:SEN0159)CO2_%E4%BA%8C%E6%B0%A7%E5%8C%96%E7%A2%B3%E4%BC%A0%E6%84%9F%E5%99%A8%E6%A8%A1%E5%9D%97_V1
 */
float co2Sensor(){
  int val;//定义变量
  float dat;//定义变量
  val=analogRead(pinCO2);// 读取传感器的模拟值并赋值给val
  dat=(125*val)>>8;//温度计算公式
  Serial.print("co2Sensor:");//原样输出显示Tep 字符串代表温度
  Serial.print(dat);//输出显示dat 的值
  Serial.println("m2");//原样输出显示C 字符串
  //delay(500);//延时0.5 秒

  return(dat);
}

void loop() {
  ledLight();

  float tempNTC = temperatureNTC();
  float temp18B20 = temperature18B20();
  float tempLM35 = temperatureLM35();
  float tempDHT11 = temperatureDHT11();
  float humDHT11 = humdidityDHT11();
  float distance = ultraSoundDistance();
  float preBMP180 = pressureBMP180();
  float ph = phSensor();
  float co2 = co2Sensor();
  lcdShow(distance,temp18B20,humDHT11,preBMP180,ph,co2);
  if(temp18B20 > 15 || distance < 5){
    buzzerVoice(3);
  }

  delay(simpFreq);
}



