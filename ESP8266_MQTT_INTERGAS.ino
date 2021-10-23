// ESP8266 MQTT INTERGAS

// Board:
/* Boards:
 * - Generic ESP8266 Module * esp8266:esp8266:generic:led=2,baud=115200,xtal=80,CrystalFreq=26,eesz=1M64,FlashMode=dout,FlashFreq=40,ResetMethod=nodemcu,dbg=Disabled,lvl=None____,ip=lm2f,vt=flash,exception=legacy,wipe=none,sdk=nonosdk_190703,ssl=all,CONSOLEBAUD=115200
 */
 
// Include
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "system.h"
#include "button.h"

// Wifi parameters
const char* wifi_ssid = "MNetwork";
const char* wifi_password = "mXm#010#mXm";

// MQTT parameters
const char* mqtt_server = "10.10.10.10";
const char* mqtt_id = "Intergas";
const char* mqtt_login = "";
const char* mqtt_passw = "";
const char* mqtt_topic_ip = "IP";
const char* mqtt_topic_id = "ID";
const char* mqtt_topic_livecheck = "LiveCheck";
const char* mqtt_topic_msgcheck = "MessageCheck";
const char* mqtt_topic_ledstatus = "LedStatus";
const char* mqtt_topic_relaisstatus = "RelaisStatus";
const char* mqtt_topic_data_in = "DataIn";
const char* mqtt_topic_data_out = "DataOut";

#if MQTT_MAX_PACKET_SIZE < 1024  // If the max message size is too small, throw an error at compile time.
#error "MQTT_MAX_PACKET_SIZE is too small in libraries/PubSubClient/src/PubSubClient.h
#endif

const int mqtt_topic_len = 40;
const int mqtt_msg_len = MQTT_MAX_PACKET_SIZE - 40;

// Serial paramters 
const int serial_msg_len = (MQTT_MAX_PACKET_SIZE - 40) / 5; // 1 byte is transmitted with 5 chars over mqtt

// Instance
SystemData System_Data;
ButtonData Button_Data;
WiFiClient WIFI_Client;
PubSubClient MQTT_Client(WIFI_Client);

// GPIO
#define IO_BUTTON 0
#define IO_BOARD 19
#define IO_LED 2
#define IO_RELAIS 18

// Serial debug print
#define DEBUG_PRINT false
#define DEBUG_PRINT_SERIAL \
  if (DEBUG_PRINT) Serial
  
#define DEBUG_WARNING false
#define DEBUG_WARNING_SERIAL \
  if (DEBUG_WARNING) Serial
  
#define DEBUG_ERROR false
#define DEBUG_ERROR_SERIAL \
  if (DEBUG_ERROR) Serial

// Variabels
char serial_msg_in[serial_msg_len];
int serial_msg_in_count = 0;
bool serial_msg_in_ready = false;
bool serial_msg_in_begin = false;
bool serial_msg_in_end = false;
char serial_msg_out[serial_msg_len];
long serial_millis;

char wifi_mac[20];
char wifi_ip[20];
int wifi_counter = 0;

char mqtt_topic[mqtt_topic_len];
char mqtt_msg[mqtt_msg_len];
char mqtt_msg_in[mqtt_msg_len];
char mqtt_msg_out[mqtt_msg_len];
bool mqtt_msg_in_flag = false;
bool mqtt_msg_out_flag = false;
int mqtt_counter = 0;

int livecheck = 0;

bool data_led = false;
bool data_led_mem = false;
bool data_relais = false;
bool data_relais_mem = false;
int data_in = 0;
int data_out = 0;

long millis_now;
long millis_mem = 0;
long millis_delay = 9000;

int time_out_counter = 0;

// Init
void setup() 
{
  Serial.begin(9600);

  pinMode(IO_BUTTON, INPUT);
  //pinMode(IO_BOARD, OUTPUT);
  pinMode(IO_LED, OUTPUT);
  //pinMode(IO_RELAIS, OUTPUT);

  //Button_Data.ButtonInit(IO_BUTTON);

  wifi_init();
  wifi_connect();

  mqtt_init();
  if (WiFi.status() == WL_CONNECTED)
  {
    mqtt_connect();
  }

  Button_Data.ButtonInit(IO_BUTTON);
}

//  Routines
void wifi_init()
{
  WiFi.disconnect(true); // delete old config
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void wifi_connect() 
{
  DEBUG_PRINT_SERIAL.println("");
  DEBUG_PRINT_SERIAL.print("WiFi connecting to ");
  DEBUG_PRINT_SERIAL.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);  
  delay(1000);

  if (WiFi.status() == WL_CONNECTED) 
  {
    DEBUG_PRINT_SERIAL.println("");
    DEBUG_PRINT_SERIAL.println("WiFi connected");
  } 
  else 
  {
    DEBUG_WARNING_SERIAL.println("");
    DEBUG_WARNING_SERIAL.println("WiFi connection failed");
  }
}

void wifi_reconnect()
{
  DEBUG_PRINT_SERIAL.println("");
  DEBUG_PRINT_SERIAL.print("WiFi reconnecting to ");
  DEBUG_PRINT_SERIAL.println(wifi_ssid);

  WiFi.disconnect();
  //WiFi.reconnect();
  WiFi.begin(wifi_ssid, wifi_password);  
  delay(1000);

  if (WiFi.status() == WL_CONNECTED) 
  {
    DEBUG_PRINT_SERIAL.println("");
    DEBUG_PRINT_SERIAL.println("WiFi reconnected");
  } 
  else 
  {
    DEBUG_WARNING_SERIAL.println("");
    DEBUG_WARNING_SERIAL.println("WiFi reconnection failed");
  }
}

void wifi_info()
{
  sprintf(wifi_mac, "%2X:%2X:%2X:%2X:%2X:%2X", WiFi.macAddress()[5], WiFi.macAddress()[4], WiFi.macAddress()[3], WiFi.macAddress()[2], WiFi.macAddress()[1], WiFi.macAddress()[0]);
  sprintf(wifi_ip, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
}

bool wifi_check()
{
  bool wifi_status = false;

  if (WiFi.status() == WL_CONNECTED)
    wifi_status = true;
  if ((WiFi.localIP()[0] == 0) && (WiFi.localIP()[1] == 0) && (WiFi.localIP()[2] == 0) && (WiFi.localIP()[3] == 0) )
    wifi_status = false;
    
  return(wifi_status);
}

void mqtt_init()
{
  MQTT_Client.setServer(mqtt_server, 1883);
  MQTT_Client.setCallback(mqtt_callback);  
}

void mqtt_connect() 
{
  DEBUG_PRINT_SERIAL.println();
  DEBUG_PRINT_SERIAL.print("MQTT connecting to ");
  DEBUG_PRINT_SERIAL.println(mqtt_server);    

  if (MQTT_Client.connect(mqtt_id, mqtt_login, mqtt_passw)) 
  {
    DEBUG_PRINT_SERIAL.println("");
    DEBUG_PRINT_SERIAL.println("MQTT connected");
    DEBUG_PRINT_SERIAL.print("ID: ");
    DEBUG_PRINT_SERIAL.println(mqtt_id);

    delay(1000);
    
    strcpy(mqtt_topic, mqtt_id);
    strcat(mqtt_topic, "/");
    strcat(mqtt_topic, mqtt_topic_data_in);
    MQTT_Client.subscribe(mqtt_topic);

    strcpy(mqtt_msg_out, mqtt_id);
    MQTT_Client.publish(mqtt_topic_id, mqtt_msg_out);
  } 
  else 
  {
    DEBUG_WARNING_SERIAL.println("");
    DEBUG_WARNING_SERIAL.print("MQTT connection failed, rc=");
    DEBUG_WARNING_SERIAL.println(MQTT_Client.state());
  }
}

void mqtt_reconnect() 
{
  //DEBUG_PRINT_SERIAL.println();
  //DEBUG_PRINT_SERIAL.print("MQTT connecting to ");
  //DEBUG_PRINT_SERIAL.println(mqtt_server);    

  if (MQTT_Client.connect(mqtt_id, mqtt_login, mqtt_passw)) 
  {
    DEBUG_PRINT_SERIAL.println("");
    DEBUG_PRINT_SERIAL.println("MQTT reconnected");
    //DEBUG_PRINT_SERIAL.print("ID: ");
    //DEBUG_PRINT_SERIAL.println(mqtt_id);
    
    strcpy(mqtt_topic, mqtt_id);
    strcat(mqtt_topic, "/");
    strcat(mqtt_topic, mqtt_topic_data_in);
    MQTT_Client.subscribe(mqtt_topic);

    //strcpy(mqtt_msg_out, mqtt_id);
    //MQTT_Client.publish(mqtt_topic_id, mqtt_msg_out);
  } 
  else 
  {
    DEBUG_WARNING_SERIAL.println("");
    DEBUG_WARNING_SERIAL.print("MQTT reconnection failed, rc=");
    DEBUG_WARNING_SERIAL.println(MQTT_Client.state());
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
  mqtt_msg_in_flag = true;

  payload[length] = '\0';
  strcpy(mqtt_msg_in, (char *)payload);

  DEBUG_PRINT_SERIAL.println("");
  DEBUG_PRINT_SERIAL.print("MQTT msessage arrived [");
  DEBUG_PRINT_SERIAL.print(topic);
  DEBUG_PRINT_SERIAL.print("][");
  DEBUG_PRINT_SERIAL.print(mqtt_msg_in);
  DEBUG_PRINT_SERIAL.println("]");
}

void mqtt_publish(const char* topic, char* payload)
{
  mqtt_msg_out_flag = true;
  
  strcpy(mqtt_topic, mqtt_id);
  strcat(mqtt_topic, "/");
  strcat(mqtt_topic, topic);

  strcpy(mqtt_msg_out, (char *)payload);

  MQTT_Client.publish(mqtt_topic, mqtt_msg_out);

  DEBUG_PRINT_SERIAL.println("");
  DEBUG_PRINT_SERIAL.print("MQTT msessage send [");
  DEBUG_PRINT_SERIAL.print(topic);
  DEBUG_PRINT_SERIAL.print("][");
  DEBUG_PRINT_SERIAL.print(mqtt_msg_out);
  DEBUG_PRINT_SERIAL.println("]");
}

void mqtt_info()
{

}

bool mqtt_check()
{
  bool mqtt_status = false;

  if (MQTT_Client.connected())
    mqtt_status = true;
    
  return(mqtt_status);
}

// Main
void loop() 
{
  // system update
  System_Data.SystemUpdate();
  
  // system start
  if (System_Data.system_start_puls)
  {
    ;
  }
  
  // wifi
  if (wifi_check())
  {
    wifi_counter = 0;
  }
  else
  {
    if (System_Data.system_1s_puls)
    {
      wifi_counter++;
    }
    if (wifi_counter >= 30)
    {
      wifi_counter = 0;
      wifi_reconnect();
    }
  }
  
  // mqtt
  if (wifi_check())
  {
    if (mqtt_check())
    {
      mqtt_counter = 0;
    }
    else
    {
      if (System_Data.system_1s_puls)
      {
        mqtt_counter++;
      }
      if (mqtt_counter >= 3)
      {
        mqtt_counter = 0;
        mqtt_reconnect();
      }
    }
        
    if (mqtt_check())
    {
      MQTT_Client.loop();
    }
  
    if (mqtt_msg_in_flag)
    {
      mqtt_msg_in_flag = false;
      
      //Serial data out
      Serial.print(mqtt_msg_in);
              
      strcpy(mqtt_msg, mqtt_msg_in);
      mqtt_publish(mqtt_topic_msgcheck, mqtt_msg);
    }
  
    if (mqtt_msg_out_flag)
    {
      mqtt_msg_out_flag = false;
    }
  }
 
  if (System_Data.system_1s_puls)
  {
    DEBUG_PRINT_SERIAL.print("*");

//  data_out++;

//  if (wifi_check() && mqtt_check())   
//  {
//    itoa(data_out, mqtt_msg, 10);
//    mqtt_publish(mqtt_topic_data_out, mqtt_msg);
//  }
  }

  if (System_Data.system_10s_puls)
  {
    DEBUG_PRINT_SERIAL.print("#");

    if (wifi_check() && mqtt_check())
    {
    itoa(livecheck++, mqtt_msg, 10);
    mqtt_publish(mqtt_topic_livecheck, mqtt_msg);
    }
  }

  if (System_Data.system_30s_puls)
  {
    wifi_info();
    
    DEBUG_PRINT_SERIAL.println("");
    DEBUG_PRINT_SERIAL.print("WiFi MAC address: ");
    DEBUG_PRINT_SERIAL.println(wifi_mac);
    DEBUG_PRINT_SERIAL.print("WiFi IP address: ");
    DEBUG_PRINT_SERIAL.println(wifi_ip);
    DEBUG_PRINT_SERIAL.print("WiFi counter: ");
    DEBUG_PRINT_SERIAL.println(wifi_counter);

    if (wifi_check() && mqtt_check())
    {
      strcpy(mqtt_msg, wifi_ip);
      mqtt_publish(mqtt_topic_ip, mqtt_msg);
    }
    
    DEBUG_PRINT_SERIAL.print("MQTT counter: ");
    DEBUG_PRINT_SERIAL.println(mqtt_counter);

//  DEBUG_PRINT_SERIAL.print("IO relais: ");
//  DEBUG_PRINT_SERIAL.println(data_relais ? "on": "off");

//  if (wifi_check() && mqtt_check())
//  {
//    strcpy(mqtt_msg, data_relais ? "on" : "0ff");
//    mqtt_publish(mqtt_topic_relaisstatus, mqtt_msg);
//  }
  }

  // IO
  Button_Data.ButtonUpdate();

  if (!wifi_check() && !mqtt_check())
  {
    data_led = System_Data.system_500ms_block;
  }
  if (wifi_check() && !mqtt_check())
  {
    data_led = System_Data.system_1s_block;
  }
  else if (wifi_check() && mqtt_check())
  {
    data_led = false;
  }
  else
  {
    data_led = true;
  }
  
  digitalWrite(IO_LED, data_led);
  //digitalWrite(IO_RELAIS, data_relais);

  /*
  if (data_led != data_led_mem)
  {
    //mqtt_publish(mqtt_topic_ledstatus, data_led ? "on" : "off");
    if (data_led)
    {
        mqtt_publish(mqtt_topic_ledstatus, "on");
    }
    else
    {
      mqtt_publish(mqtt_topic_ledstatus, "off");    
    }
  }
  data_led_mem = data_led;
  */

  /*
  if (data_relais != data_relais_mem)
  {
    //mqtt_publish(mqtt_topic_relaisstatus, data_relais ? "on" : "0ff");
    if (data_relais)
    {
    mqtt_publish(mqtt_topic_relaisstatus, "on");
    }
    else
    {
      mqtt_publish(mqtt_topic_relaisstatus, "off");    
    }
  }
  data_relais_mem = data_relais;
  */

  //Serial data in
  while ((Serial.available() > 0) and (serial_msg_in_count < serial_msg_len))
  {
    // read the incoming byte:
    serial_msg_in[serial_msg_in_count++] = Serial.read();
  }
  if ((serial_msg_in_count >= 1) and !serial_msg_in_begin)
  {
    serial_millis = millis();
    serial_msg_in_begin = true;
  }
//if (serial_msg_in_count >= 32)
//{
// serial_msg_in_end = true;
//}
  serial_msg_in_ready = false;
  if (serial_msg_in_count > 0) 
  {
    if ((serial_msg_in_begin and serial_msg_in_end) or (serial_msg_in_count >= serial_msg_len - 1) or (serial_msg_in_begin and (millis() - serial_millis > 1000)))
    {
      serial_msg_in[serial_msg_in_count] = '\0';
      serial_msg_in_ready  = true;
      serial_msg_in_begin = false;
      serial_msg_in_end = false;
    }
  }

  millis_now = millis();
  if (millis_now - millis_mem > millis_delay) 
  {
    millis_mem = millis_now;
    snprintf (mqtt_msg, mqtt_msg_len, "Serial data timeout #%ld", ++time_out_counter);
    DEBUG_PRINT_SERIAL.print("Publish message: ");
    DEBUG_PRINT_SERIAL.println(mqtt_msg);
    mqtt_publish(mqtt_topic_data_out, mqtt_msg);
  }
  if (serial_msg_in_ready)
  {
    millis_mem = millis_now;
    //strcpy(mqtt_msg, serial_msg_in);
    for (int i = 0; i < serial_msg_in_count; i++)
    {
      mqtt_msg[i*5+0] = '0';      
      mqtt_msg[i*5+1] = 'x';      
      byte nib1 = (serial_msg_in[i] >> 4) & 0x0F;
      byte nib2 = (serial_msg_in[i] >> 0) & 0x0F;
      mqtt_msg[i*5+2] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
      mqtt_msg[i*5+3] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
      mqtt_msg[i*5+4] = ';';      
    }
    mqtt_msg[serial_msg_in_count*5] = '\0';
    //DEBUG_PRINT_SERIAL.print("Publish message: ");
    //DEBUG_PRINT_SERIAL.print(mqtt_msg); //includes /n
    DEBUG_PRINT_SERIAL.println("Publish message.");
    mqtt_publish(mqtt_topic_data_out, mqtt_msg);
    serial_msg_in_count = 0;
    time_out_counter = 0;
  }
}
