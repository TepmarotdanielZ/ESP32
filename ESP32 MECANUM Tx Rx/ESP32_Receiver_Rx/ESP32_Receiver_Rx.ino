// ROBOT MECANUM ESP32 RECEIVER RX


#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>

//----------------------------------------

#define Motor1_GPIO_1 27
#define Motor1_GPIO_2 26
#define Motor2_GPIO_1 4
#define Motor2_GPIO_2 17

#define Motor3_GPIO_1 19
#define Motor3_GPIO_2 18
#define Motor4_GPIO_1 25
#define Motor4_GPIO_2 33

int PWM1_Ch_1 = 0;
int PWM1_Ch_2 = 1;

int PWM2_Ch_1 = 2;
int PWM2_Ch_2 = 3;

int PWM3_Ch_1 = 4;
int PWM3_Ch_2 = 5;

int PWM4_Ch_1 = 6;
int PWM4_Ch_2 = 7;

#define PWM1_Res 8
#define PWM2_Res 8
#define PWM3_Res 8
#define PWM4_Res 8

#define PWM1_Freq 1000
#define PWM2_Freq 1000
#define PWM3_Freq 1000
#define PWM4_Freq 1000

// Neopixel------------------------------

#define NUM_LEDS 8
#define DATA_PIN 5

CRGB leds[NUM_LEDS];

#define MAX_LED 8
#define USE_BRIGHTNESS 0

uint32_t lastTime = 0;
uint32_t last_time = 0;

uint8_t LED_Data[MAX_LED][8];
uint8_t LED_Mod[MAX_LED][8];  // for brightness
uint16_t effStep = 0;
uint32_t K[3] = {0, 4, 8};
int GREEN[3] = {255, 0, 0};
int BLUE[3] = {0, 255, 0};
int RED[3] = {0, 0, 255};
int L = 4, M = 8,z;
int datasentflag=0;
int RGB_Blue = 0;
int RGB_Red = 255;
int RGB_Green =0;
int RGB_state = 0;
int RGB_Brightness = 255;


//---------------------------------------
float W1, W2, W3, W4;
float Vx, Vy, Omega;
float throttle;
float steering;
float motorDirection;

int yAxisValue;
//---------------------------------------

// int Omega = 0;
int State_button = 0;
int Switch = 0;

// Define variables to store incoming readings

int receive_rnd_val_1;
int receive_rnd_val_2;
int receive_rnd_val_3;

//----------------------------------------

// struct PacketData
// {
//   byte xAxisValue;
//   byte yAxisValue;
//   byte switchPressed;
// };
// PacketData receiverData;

// void throttleAndSteeringMovements() {
//   int throttle = map(receiverData.yAxisValue, 254, 0, -255, 255);
//   int steering = map(receiverData.xAxisValue, 0, 254, -255, 255);
//   int motorDirection = 1;
// }

// Define a data structure
typedef struct struct_message {
  int rnd_1;
  int rnd_2;
  int rnd_3;
} struct_message;

// Create a structured object
struct_message myData;


// Callback function executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Data received: ");
  Serial.println(len);
  receive_rnd_val_1 = myData.rnd_1;
  receive_rnd_val_2 = myData.rnd_2;
  receive_rnd_val_3 = myData.rnd_3;
}

void setup() {

//----------------------------------------

  // PWM MOTOR 1
  ledcAttachPin(Motor1_GPIO_1, PWM1_Ch_1);
  ledcSetup(PWM1_Ch_1, PWM1_Freq, PWM1_Res);
  ledcAttachPin(Motor1_GPIO_2, PWM1_Ch_2);
  ledcSetup(PWM1_Ch_2, PWM1_Freq, PWM1_Res);

  // PWM MOTOR 3
  ledcAttachPin(Motor2_GPIO_1, PWM2_Ch_1);
  ledcSetup(PWM2_Ch_1, PWM2_Freq, PWM2_Res);
  ledcAttachPin(Motor2_GPIO_2, PWM2_Ch_2);
  ledcSetup(PWM2_Ch_2, PWM2_Freq, PWM2_Res);

  // PWM MOTOR 2
  ledcAttachPin(Motor3_GPIO_1, PWM3_Ch_1);
  ledcSetup(PWM3_Ch_1, PWM3_Freq, PWM3_Res);
  ledcAttachPin(Motor3_GPIO_2, PWM3_Ch_2);
  ledcSetup(PWM3_Ch_2, PWM3_Freq, PWM3_Res);

  // PWM MOTOR 4
  ledcAttachPin(Motor4_GPIO_1, PWM4_Ch_1);
  ledcSetup(PWM4_Ch_1, PWM4_Freq, PWM4_Res);
  ledcAttachPin(Motor4_GPIO_2, PWM4_Ch_2);
  ledcSetup(PWM4_Ch_2, PWM4_Freq, PWM4_Res);

//----------------------------------------

  // Neopixel------------------------------
  {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  }
  //----------------------------------------

  // Set up Serial Monitor
  Serial.begin(115200);

  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {



  // Serial.print("Omega : ");
  // Serial.println(Omega);

  // Serial.print("<<<<<");
  // Serial.println("<<<<<");
  
  // Serial.print(W1);
  // Serial.print("<<<<<");
  // Serial.print(W2);
  // Serial.print("<<<<<");
  // Serial.print(W3);
  // Serial.print("<<<<<");
  // Serial.println(W4); 

//----------------------------------------

  throttle = map(receive_rnd_val_1, 254, 0, -255, 255);
  steering = map(receive_rnd_val_2, 0, 254, -255, 255);  
  Omega    = map(receive_rnd_val_3, 0, 254, -255, 255);

//----------------------------------------
  Serial.println("Receive Data: ");
  // Serial.print(throttle);
  // Serial.print("<<<<<");
  // Serial.print(steering);
  // Serial.print("<<<<<");
  // Serial.println(Omega);

    Serial.print("throttle : ");
    Serial.println(throttle);
    Serial.print("steering : ");
    Serial.println(steering);
    Serial.print("Omega    : ");
    Serial.println(Omega);


  Vx = throttle;
  Vy = steering;


  W1 = Vx - Vy + Omega;
  W2 = Vx + Vy - Omega;
  W3 = Vx + Vy + Omega;
  W4 = Vx - Vy - Omega;

  if (W1 < -255) W1 = -255;
  if (W1 > 255) W1 = 255;

  if (W2 < -255) W2 = -255;
  if (W2 > 255) W2 = 255;

  if (W3 < -255) W3 = -255;
  if (W3 > 255) W3 = 255;

  if (W4 < -255) W4 = -255;
  if (W4 > 255) W4 = 255;

//----------------------------------------

  // MOTOR 1
  if (W1 == 0) {
    ledcWrite(PWM1_Ch_1, 0);
    ledcWrite(PWM1_Ch_2, 0);
  }
  if (W1 < 0 && W1 >= -255) {
    ledcWrite(PWM1_Ch_1, 0);
    ledcWrite(PWM1_Ch_2, (-1) * W1);
  }

  if (W1 > 0 && W1 <= 255) {
    ledcWrite(PWM1_Ch_1, W1);
    ledcWrite(PWM1_Ch_2, 0);
  }
  // MOTOR 2
  if (W2 == 0) {
    ledcWrite(PWM2_Ch_1, 0);
    ledcWrite(PWM2_Ch_2, 0);
  }
  if (W2 < 0 && W2 >= -255) {
    ledcWrite(PWM2_Ch_1, 0);
    ledcWrite(PWM2_Ch_2, (-1) * W2);
  }

  if (W2 > 0 && W2 <= 255) {
    ledcWrite(PWM2_Ch_1, W2);
    ledcWrite(PWM2_Ch_2, 0);
  }
  // MOTOR 3
  if (W3 == 0) {
    ledcWrite(PWM3_Ch_1, 0);
    ledcWrite(PWM3_Ch_2, 0);
  }
  if (W3 < 0 && W3 >= -255) {
    ledcWrite(PWM3_Ch_1, 0);
    ledcWrite(PWM3_Ch_2, (-1)*W3);
  }
  if (W3 > 0 && W3 <= 255) {
    ledcWrite(PWM3_Ch_1, W3);
    ledcWrite(PWM3_Ch_2, 0);
  }
  //MOTOR 4
  if (W4 == 0) {
    ledcWrite(PWM4_Ch_1, 0);
    ledcWrite(PWM4_Ch_2, 0);
  }
  if (W4 < 0 && W4 >= -255) {
    ledcWrite(PWM4_Ch_1, 0);
    ledcWrite(PWM4_Ch_2, (-1) * W4);
  }

  if (W4 > 0 && W4 <= 255) {
    ledcWrite(PWM4_Ch_1, W4);
    ledcWrite(PWM4_Ch_2, 0);
  }

//----------------------------------------

  // // Neopixel------------------------------

  //     // Red
  //     fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
  //     FastLED.show();
  //     delay(5000);

  //     // Green
  //     fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0));
  //     FastLED.show();
  //     delay(5000);

  //     // Blue
  //     fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));
  //     FastLED.show();
  //     delay(5000);

  //     // Green
  //     fill_solid(leds, NUM_LEDS, CRGB(0, 255, 255));
  //     FastLED.show();
  //     delay(5000);

  //     // Green
  //     fill_solid(leds, NUM_LEDS, CRGB(255, 0, 200));
  //     FastLED.show();
  //     delay(5000);

  //     // Green
  //     fill_solid(leds, NUM_LEDS, CRGB(223, 0, 225));
  //     FastLED.show();
  //     delay(5000);

//----------------------------------------

//////RGB
    if(millis() - last_time >= 20 ){
      last_time = millis();

      if (RGB_state == 0){
        RGB_Red = RGB_Brightness;
        RGB_Green ++;
        RGB_Blue = 0;
        if(RGB_Green >= RGB_Brightness){
          RGB_state = 1;
        }
      }
      else if (RGB_state == 1){
        RGB_Red --;
        RGB_Green = RGB_Brightness;
        RGB_Blue = 0;
        if(RGB_Red <= 0){
          RGB_state = 2;
        }
      }
      else if (RGB_state == 2){
        RGB_Red = 0;
        RGB_Green = RGB_Brightness;
        RGB_Blue ++;
        if(RGB_Blue >= RGB_Brightness){
          RGB_state = 3;
        }
      }
      else if (RGB_state == 3){
        RGB_Red = 0;
        RGB_Green --;
        RGB_Blue = RGB_Brightness;
        if(RGB_Green <= 0){
          RGB_state = 4;
        }
      }
      else if (RGB_state == 4){
        RGB_Red ++;
        RGB_Green = 0;
        RGB_Blue = RGB_Brightness;
        if(RGB_Red >= RGB_Brightness){
          RGB_state = 5;
        }      }
      else if (RGB_state == 5){
        RGB_Red = RGB_Brightness;
        RGB_Green = 0;
        RGB_Blue --;
        if(RGB_Blue <= 0){
          RGB_state = 0;
        }      }

      else{

      }
    ///////////////////////
    }

    fill_solid(leds, 1, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    fill_solid(leds, 2, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    fill_solid(leds, 3, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    fill_solid(leds, 4, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    fill_solid(leds, 5, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    fill_solid(leds, 6, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    fill_solid(leds, 7, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    fill_solid(leds, 8, CRGB(RGB_Red, RGB_Green, RGB_Blue));
    FastLED.show();
  
//----------------------------------------

}