
 // ROBOT MECANUM ESP32 RECEIVER RX 

#include <esp_now.h>
#include <WiFi.h>
 
#define X_AXIS_PIN 32
#define Y_AXIS_PIN 33
#define SWITCH_PIN 25

int Vx;
int Vy;
int Omega = 0;
int yAxisValue = 0;
int k = 0;
int buttonState = 0;
int previousButtonState = HIGH;
bool isButtonPressed = false;

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0x2E, 0x0C, 0xF8};
 
struct PacketData
{
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
};
PacketData data;

int mapAndAdjustJoystickDeadBandValues(int value, bool reverse)
{
  if (value >= 2200)
  {
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1800)
  {
    value = map(value, 1800, 0, 127, 0);  
  }
  else
  {
    value = 127;
  }

  if (reverse)
  {
    value = 254 - value;
  }
  return value;

}
// Variables to accommodate the data to be sent.
int send_rnd_val_1;
int send_rnd_val_2;
char send_rnd_val_3;
//----------------------------------------

String success;

// Define a data structure
typedef struct struct_message {
    int rnd_1;
    int rnd_2;
    char rnd_3;
} struct_message;
 
// Create a structured object
struct_message myData;
 
// Peer info
esp_now_peer_info_t peerInfo;
 
// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // Set up Serial Monitor
  Serial.begin(115200);
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
 
  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
 
  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {

  buttonState = digitalRead(SWITCH_PIN);  // Read the current state of the button
  
  if (buttonState == HIGH && previousButtonState == LOW) {  // Button was released

    k = !k;
    // Perform any desired actions when the button is released
    
    isButtonPressed = false;  // Reset the button pressed flag
  } else if (buttonState == LOW && previousButtonState == HIGH) {  // Button was pressed

    // Perform any desired actions when the button is pressed
    
    isButtonPressed = true;  // Set the button pressed flag
  }
  Serial.println(k);
  previousButtonState = buttonState;  // Update the previous button state
  
//----------------------------------------
  data.xAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(X_AXIS_PIN), true);
  data.yAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(Y_AXIS_PIN), true);  
  data.switchPressed = false; 

//----------------------------------------
 if (k == 0)
	  {
		  Vx = data.xAxisValue;
      Vy = data.yAxisValue;
		  Omega = 127;
	  }
	  else
	  {
		  Vx     = data.xAxisValue;
      Vy    = 127;
		  Omega = data.yAxisValue;
	  }

//----------------------------------------
  // if (digitalRead(SWITCH_PIN) == LOW)
  // {
  //   data.switchPressed = true;
  // }
//----------------------------------------
  // Create test data
 
  // send_rnd_val_1 = 100;
  // send_rnd_val_2 = 200;
  // send_rnd_val_3 = 'A';

  myData.rnd_1 = Vx;
  myData.rnd_2 = Vy;
  myData.rnd_3 = Omega;
  
  Serial.println();
  Serial.print(">>>>> ");
  Serial.println("Send data");
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sending confirmed");

    Serial.print(" Vx   : ");
    Serial.println(Vx);
    Serial.print(" Vy   : ");
    Serial.println(Vy);
    Serial.print("Omega : ");
    Serial.println(Omega);
  }
  else {
    Serial.println("Sending error");
  }
  delay(100);
}