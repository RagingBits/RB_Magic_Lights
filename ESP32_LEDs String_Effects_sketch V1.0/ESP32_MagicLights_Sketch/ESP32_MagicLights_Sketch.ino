#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <SPIFFS.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include "pins_arduino.h"
#include "EEPROM.h"

/*

pins defined at 
C:\Users\eng_m\Documents\ArduinoData\packages\esp32\hardware\esp32\1.0.6\variants\doitESP32devkitV1\

*/

#define MAX_LEDS_BUFFER_SIZE 900

#define UPDATE_BUFFER_SIZE 256

/* Put your SSID & Password */
const char* ssid_self = "RB_MAGIC_LIGHTS";  // Enter SSID here

const char* request_webpage =  "<!DOCTYPE html>\r\n\
<html>\r\n\
<body>\r\n\
<img src=\"data:image/jpeg;base64,/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAIBAQIBAQICAgICAgICAwUDAwMDAwYEBAMFBwYHBwcGBwcICQsJCAgKCAcHCg0KCgsMDAwMBwkODw0MDgsMDAz/2wBDAQICAgMDAwYDAwYMCAcIDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAwMDAz/wAARCABMAEoDASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD9/KKKKACivzQ/Yh/4LaePvDn7eeufss/tl+E/B/wr+Ld5do/gbXPD4uYfC/jS3l+SGOCS5lkfzJnRzBIXAlffbMkF1EIpv0voAz/FnizS/AXhXUtd13UtP0XRNFtJb/UNQv7hLa1sLeJC8s0srkJHGiKzM7EBQCSQBX44/wDBKT4v69/wXy/4Kf8Aj39ozV/HnxA8MfBn4D6rp8fw6+Glt4ojtc3jWt1CNQ1G3tJlkTdG90zB0dbj7bLafaZ7e0mhk9A/4PJPilr3w/8A+CR+m6TpF99j0/xx8QNL0TW4vJjk+22aW19frFllJTF1Y2sm5CrfutudrMrfl/8A8Gz/APwV507/AIJTfFPXPC/xsvfEHhn4JfFzSjrulalNpd9eW1nqNtNLbLeQRRk5t5/IurWaWCCZ2ns7VGKJBIVAP6nqK/mh/Yq+JGg/8HSP/BXDUJP2mvF//CNeD/C+lSXfw9+F+ma1JZ/a1S5hlmtIpDBi43WsMr3sqSQXkv7t4QkFuVtv6XqACiiigAor88P+C0tp+2N+zX4V8QftAfs+fGrT7vwb4BtINe8Q/CzX/C+lSWs1hZo8l/NBqJjS6aMxRq72xlWYg3LQ3AfyLcflB/wVS/4OjNV/by/4Jc+D/h54Ut9Q+H/xI8Y3dzYfFKLTJ3jtXsIIY1ENq7Izmz1F5yzJ5yzRCymt5RPDMJZgD5g/4Lyf8FU7f/goL/wVGm+KXwx1LUNJ8O/D20sdC8Ea7aRT6XqcqWU0l0NQz5hkjkN5PcSQuBFIsIt90ccoev7Da/jy/wCCFv7Idxrv/Bcb9nfwZ47h1DRZI7ux8f2y2d1A0kiRaKfEelsWAkTy5kS0Z04kEcrofLkBK/Z//BQP/g60/aa+Hvxd+LXwf8SfBXw/8H/+JVqHht9PXVL3/hLPDN5caa8MN9BqsUiQSbLiWO7ikitQskPliOT5luaAPqD/AIK4/FD/AIiCv2cfjh8M/gP4L8QeLvB/7O3l+K9M+JFje7tI8W+KLJXS48P6fax28s2obtNu7to5rd1DXK2oOIJ4Zp/zA+Iln/w0R/wa5/DnxFqWm+H4dW/Z7+NWp+DdEvv7S+x3Mmj6nZrqd3H5Ms4W7uHvri3O2GNpEgtdwVUS5kbQ/wCCL3/BxPqv/BI39kf4qfDaD4f6f4svNeu5PEnhG9e4eKO01mWO1tJEv1DAyWYggSZRCUk8yFoi224E1t5/4w+E/jLwv/wb9fCV9Z8YeD/B/gTx58YPFXiqy0nUrkS6h4ne00S0s7S7t4reGa4WOO4stUsWLmKNZtQsmlAidJ4wDyD45/saeIf2JdR+Cvja1+Iun6LH8RvBVr8SPC+rj7bpOs6RdRQGbyGtERr23k+3QtDZ3wQWt0DDcRzpGJjb/wBRv/Bvf/wUn17/AIKh/wDBOPR/G3jEeZ498L6rc+E/E95HZR2dtql5bpDMl1FGjsB5lrc2zSYWJfP88JEkYQV/Jl8a/CfwX0LwrbzfDnx98UPFWttdqk9p4k8AWPh+1jt9jlpFng1m9dpA4jAjMSghmO8FQrf1O/8ABtDdfs06D/wT0t/Cv7OfjbUPGEmmXcWqeOn1iCSx1mPW7u1hMj3FmzMlvHshWCP7O0tuRaOqz3EiTTMAfofRRRQAV/BH4A+FuvfFH+2l8P2P9pTeH9Kn1u8t45oxcmzg2m4liiZhJP5UZaaRYldo4IZ52CwwSyJ/dZ+0L8FNL/aU+AXjj4c67cahaaJ4/wDD9/4b1CewdI7qG3vLaS3leJnV0WQJIxUsjAEDKkcH+JP4v/Dr4j/8E1/2sfHngPU5P7D8beEf7Y8H6lKtozQ3lne2c9hcSQC5iVmt7qxupGimMasYriOVNpKsAD3/APZX/wCCs3xH8D/tY/s3fFyPQ/8AhZXjb9nLwrdeD4dBi0ZraG58JWNneOJHubeRmNxDY3upK0xtljt4rC2nk+1FrjGf/wAFZP2hvj1/wVF8VXH7WHj74e6h4V+Ft5d23gvwjdC08nTLe336jNBY21y6o+oSK9tqDT3CBgs25WECtBCvkHxH/Zo8A/CL9mDwx4l1j4tafq3xS8aWn9raf4J8L2Ntrdro1gZYkiOsaml4q2V5Kn2iRbKKG4mjEUQn+ztNtT9nv+Dmr/gnj4mi/wCCM37LvjK00rUPD8f7N3h+w8N+IvDd/qFpqV1o9vfWem2e+W+iMUVzJb3dnbWzG3gImN2ZVWOOMigD4w/4K/8A/BIT/hwf/wAMz/Ezwb8UPO+Jt99n1C8025t/t32LxDpn2a5uNQ06VrVIJNPS4lhVYLtFmGYji4V5Rb/qf/wdb/8ABPH4X+Of2G/in+0lrWlahqXxS8IeH/Dnhvw/dvqEsdro1uPEKiV44IyqySTJqcyOZ/MAEcRjWNgzP+Z/7C3xS0H/AIOFv+DiDRfFnx2vv+Ee0m6zqnhnwUsMmtWGoRaUiz2+gM06vFHbvDFc3VyzRJFOy3arHC90mz9QP+DxT9pf/hT/APwSot/Atrd+HzqHxc8V2OlXFjdy/wCnvp1mW1GW5tYw6k+XdW1hHJIVdFW6CkBpI2ABz/8Awbrf8Eq/2cf2iP8AgiV4D1rx18F/h/4u8QfET+3v7b1nVdKS61WTGp3lgv2e7bM9pst7eIJ9meLY6mVdsrM7fN/w+/Yhj/4Nvv8Ag4P+AMHhPXPGGvfBf9oi0fwXFPfw6ddanLcXMkNu9jKy+X+7h1B9Iu2uI4oD5MpiUTGOUSfsd+yd49+Fn7L/AIO+Gn7MX/C3/h/rnxN+HfhXS/DH9h/2xaWuvah9i0uI+f8A2b5zzx77eL7Rs+bbG27cyjcfzx/4OA/E1v8AEv8A4LZf8E5Ph74fj1DWvGXhnxqnirU9Ms7CeaS00uXVtLZbssqFDGqaVqDuQSYo7V3kCIVYgH7HUUUUAFfL/wDwV2/Z3+Dfxg/Ye+Ivin4xfCnw/wDFXT/hh4V1bxPZ2d3K1hfp9kgF7JBa6jCPtNl57WcKSPCRuVQHV1yh+oK5/wCLHwt0H44/CzxN4J8U2P8AanhnxhpV1omr2fnSQfa7O5heGeLzI2WRN0bsu5GVhnIIODQB/Lj/AMEpP+DcW4/4KUf8Eufij8Zl1fxhpPxCt7u7svhvoqWUFvpniF7KFJJDJNcFBNHczu9kkiSRR201vK7tNhok/b//AIIcf8FgtK/4KNfBpvA/jP8AtDw1+0d8LbRdL8f+GtagSz1C6uLcrbz6nFCscQEck4xLCsaG0mfynUKYJJvkD/gh/wDt43H/AAR58Va1+w3+1ze6f8N9U8J3d3rXgLxdqV3BbeF9V0u4d53ijvGSICOScXc8M87Eu8s1s/kTQR27+H+H/hBqXxC+O3xk/aC/ao+P/h/9mf8AaE8M6rpXiH4UfFDw5pFinhXxNoMNvczSHRPLmRvFdvNY3NvA8TpLdxwrpsMsswmls6AP6DtG8J6X4c1HVrzT9M0+xvNeu1v9Tnt7dIpNRuFgit1mmZQDJIIIIIg7ZIjhjXO1FA/EH9kPWfEP/Bw9/wAF1IfjtfaTqEX7MP7LN3Nb+CbhXvV0/X9UguBLZTgSSQPHeTO0F/KI4SI4bKxtrhG3pJJ83/GL/gq/+3F/wVd/Y2l+HfhnwZ4g8SfCnTNVtfA/xH+Jfwo8H3+qar40y1y00traObZkt57FYJJ7ZYrfLtGk0lpDfpaV+gHwa8X/ALbXh/8AZY8EfBP9mX9k/wAP/s++CdP0o6doPxA+KHi3TLi8tNO+yTNFf32i2EQltdYuJminlV4bhEupZVmhcM8iAHl//BWD4N6h/wAErv8Agqh4k/bq/t/9m/WI9YtLG+0rw/441vW9L8XM9pp0ej3+n6Fb2Je3u5Lm3lhBuLmGWO2a5RnjijieaX4f/Yl/Z5j/AOCruo/tIf8ABQb9qb4heMPh74d+HN3DfWGofDy707RNQn1mzggeC1sJbhnMMltAunW9urgSXE15b4uTLHLvLb/gmP4+/wCCr/7aF54HvPjhp/7TfxSurSC68e/FPT7651HwF8HtNlvprtLbS7hXiTUry4QkRWkUcFpbeddxLE7CS6070DQ/gH+0N/wVi/4Jo6H4J/Z2+APg+H9kr4I+NbTVPAOkeN9aurXxd8THhe6hvnmu1uobVo5Jb28mufIe0jiaVre1nke2YAA/RD/g2m/aq+I/xI8HfFj4R/Ejxp4g+KE3w3/4RzxZ4c8Y67dNJquo6H4n0v8AtWxtbtH8yRbiGMFpN1zchHuWgjlaG3id/wBPq/ND/ggS1v8AHz9of9rL9orwf4b0/wAK/Bf4neINC8IfDm0trSezjvtL8M2EumJfW8MltAkdnMjRCONATDJFcQMAYMv+l9ABRRRQB5f+1B+xT8I/21PCq6P8WPhz4P8AH1nDaXVlaSaxpkc91piXSKk5tLgjzrWRwkZ8yB0kBjjYMGRSPnD4W/8ABt5+xL8H/Hdj4i0n4B+H7zUNO8zyotb1bU9csH3xtGfMs725mtpcK5I8yNtrBWXDKrD7fooA+b/2u/2TPjD498K+A9C/Z3+PGn/sz6J4PtJrC50+w+HOmeIbW/twlulnDFFcMiWkdskUiqkQwwmAwBGtflD+0z+wr/wUM8e/ELR9P/ayi8YftUfAHQ7RNX1/RPg94v0vwzHfpHewXE1vc2H2O0utXkSKzLR2scaSNJNF9nu4ZDIK/e6igD+cHxd8DvCvxg8d6d4f8Zfs4/t/2n7NfhXVRrPhv4PeBv2d4fC9gl0kdrbJPqmojUp7vVbh7O2aKa6mK3TNcSvFNbKxjP2h4D/4Jg+Pvj3+zVbfBj4K/DnUP2J/2UfH3iA6546g1rxNc6j8TPGFhPptl5kEVqzXcOkxzGP7LLFNeNNthzJAi+da3P63UUAc/wDCf4W6D8DvhZ4Z8E+FrH+y/DPg/SrXRNIs/Okn+yWdtCkMEXmSM0j7Y0VdzszHGSScmugoooA//9k=\" />\
<h1>Raging Bits\r\nMagic lights.</h1>\r\n\
<h2>WIFI connection information</h2>\r\n\
<form action=\"/action_page.php\" id=\"form1\">\r\n\
<label for=\"ssid\">Wifi SSID:</label>\r\n\
<input type=\"text\" id=\"ssid\" name=\"ssid\"><br><br>\r\n\
<label for=\"password\">Password:</label>\r\n\
<input type=\"text\" id=\"password\" name=\"password\" form=\"form1\"><br><br>\r\n\
<input type=\"submit\" value=\"Connect\">\r\n\
</form>\r\n\
</body>\r\n\
</html>\r\n";

const char* wait_webpage =  "<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<body>\r\n\
<h1>Connecting...</h1>\r\n\
<h1>Please wait while the controller tries to connect to your WiFi.</h1>\r\n\
<h1>(Takes about 10s)</h1>\r\n\
<meta http-equiv=\"refresh\" content=\"1\" >\
<h1> </h1>\r\n\
<p>RagingBits.</p>\r\n\
</body>\r\n\
</html>";

const char* end_webpage_error =  "<h1>Connected!</h1>\r\n\
<h1>Please reconnect to to your WiFi</h1>\r\n\
<h1> </h1>\r\n\
<p>RagingBits.</p>\r\n\
</body>\r\n\
</html>";

const char* ready_webpage =  "<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<body>\r\n\
<h1>Controller IP: %u.%u.%u.%u </h1>\r\n\
<h1>Max mem available: %u bytes </h1>\r\n\
<h1> </h1>\r\n\
<h1>Ready 4Fun!</h1>\r\n\
<h1> </h1>\r\n\
<p>RagingBits.</p>\r\n\
</body>\r\n\
</html>";

uint16_t MAX_DATA_BUFFER1_SIZE = 0; /* 82k */
#define MAX_DATA_BUFFER2_SIZE 85500 /* 82k */


uint8_t led_buffer2[MAX_DATA_BUFFER2_SIZE];
uint8_t *led_buffer1;
uint32_t strip_length = 0;
uint32_t led_buffer_len = 0; /* multiple of strip_length*/

#define SEND_LEDS_STATUS_STOP   4
#define SEND_LEDS_STATUS_START  5

uint8_t send_leds_status = SEND_LEDS_STATUS_STOP;

uint32_t system_time = 0;


#define SSID_MAX_LEN  50
#define PASS_MAX_LEN  50
#define SSID_ADDR     0
#define PASS_ADDR     SSID_MAX_LEN

static char work_status = 1;
//static char ssid[50] = "Vodafone-A671CA";
//static char password[50] = "46swA9EtFC";
static char ssid[50] = "";
static char password[50] = "";
/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,0);
IPAddress subnet(255,255,255,0);

WebServer server(80);
WiFiServer update_connection;

Ticker ticker;


int string_compare(char *a, char *b)
{
  bool equal = true;
  while(*a != 0 && *b != 0 && equal)
  {
    if(*a != *b)
    {
      equal = false;
    }
    else
    {
      a++;
      b++;
    }
  }  

  if(equal)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

int string_compare(char *a, char *b, uint32_t len)
{
  bool equal = true;
  while(*a != 0 && *b != 0 && equal && len > 0)
  {
    len--;
    if(*a != *b)
    {
      equal = false;
    }
    else
    {
      a++;
      b++;
    }
  }  

  if(equal)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}


uint32_t max_mem_available=0;

void setup() {

  MAX_DATA_BUFFER1_SIZE = ESP.getMaxAllocHeap();
  led_buffer1 = (uint8_t*)malloc(MAX_DATA_BUFFER1_SIZE);
  //WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  pinMode(LEDS_DRIVER_RST, OUTPUT);
  pinMode(LED_CONNECTED, OUTPUT);
  pinMode(ERASE_MEM_REQUEST_PIN, INPUT_PULLUP);
  pinMode(0, INPUT_PULLUP);
  digitalWrite(LEDS_DRIVER_RST,LOW);
  digitalWrite(LED_CONNECTED,LOW);
  Serial.begin(115200);
  Serial2.begin(500000,SERIAL_8N1, RX2,TX2);

  if(!EEPROM.begin(SSID_MAX_LEN+PASS_MAX_LEN))
  {
    Serial.println("EEPROM init failed!!!");  
  }
  EEPROM.readBytes(SSID_ADDR,ssid,SSID_MAX_LEN);
  EEPROM.readBytes(PASS_ADDR,password,PASS_MAX_LEN);

  Serial.println("Starting hotspot.");  
  Serial.println("Max Mem:"); 
  Serial.println(MAX_DATA_BUFFER1_SIZE+MAX_DATA_BUFFER2_SIZE); 
  max_mem_available = MAX_DATA_BUFFER1_SIZE+MAX_DATA_BUFFER2_SIZE;
  WiFi.softAP(ssid_self);
  
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(1000);
  Serial.println("Hotspot ready.");  
  Serial.println("Starting update listen.");  
  server.on(F("?"), handle_NotFound);
  server.onNotFound(handle_NotFound);
  server.begin();
      /*
  server.on(F("?"), handle_NotFound);
  server.onNotFound(handle_NotFound);
  server.begin();
  */
  update_connection.begin(16281,1000);
  
  ticker.attach_ms(40, send_leds);/* 40 = run at 25hz */
  

  
  Serial.println("Ready to work.");  
}


void handle_update(void)
{
  static uint8_t current_step = 3;
  static uint32_t file_size = 0;
  static uint8_t client_connected = 0;
  static WiFiClient client = 0;
  static uint32_t strip_len_counter = 0;
  static uint32_t file_len_rec1 = 0;
  static uint32_t file_len_rec2 = 0;
  static uint32_t current_time = system_time;
  uint8_t temp_data[100] = "";
  
  if(update_connection.hasClient()/* || client != 0*/)
  {
    if(client == 0)
    {
      /*WiFiClient*/ client = update_connection.available();
      if (client) 
      {
        send_leds_status = SEND_LEDS_STATUS_STOP;
        Serial.println("Connected.");
        client_connected = 1;
        current_time = system_time;
        file_size = 0;
        file_len_rec1 = 0;
        file_len_rec2 = 0;
        strip_len_counter = 0;
        strip_length = 0;
        current_step = 0;
      }
    }
  }


    
  if (client) 
  {
    if(client_connected != 0)
    {
      if((system_time-current_time) > 100)
      {
        client.stop();
      }
    }
    
    if (client_connected && client.connected()) 
    {
      switch(current_step)
      {
        case 0:
        {
          if(client.read(temp_data, 100))
          {
              Serial.println("Receiving new effects file.");
              Serial.println((char*)temp_data);
              if(string_compare("F_NAME:", (char*)temp_data)==0)
              {
                Serial.println("1 OK");
                client.write("OK",2);
                current_step = 1;
                current_time = system_time;
              }
              else
              {
                Serial.println("1 NOK");
                client.write("NOK",3);
                client.stop();
              }
          }
        }  
        break;
    
        case 1:
        {
          if(client.read(temp_data, 100))
          {
            Serial.println((char*)temp_data);
            if(string_compare("F_SIZE:", (char*)temp_data)==0)
            {
              const char *val = (const char*)&temp_data[7];
              file_size = atoi(val);

              if(file_size != 0)
              {
                Serial.println("2 OK");
                current_step = 2;
                client.write("OK",2);
                client.flush();
                current_time = system_time;
              }
              else
              {
                Serial.println("2 NOK");
                client.write("NOK",3);
                client.stop();
              }
            }
          }
        }  
        break;
    
        case 2:
        {
          int value = client.read();
          if(value >= 0)
          {
            current_time = system_time;
            
            while(value >= 0)
            {
              if(strip_len_counter < 2)
              {
                strip_length *= 256;
                strip_length += (value&0x00FF);
                strip_len_counter++;
              }
              else
              if(file_len_rec1 < MAX_DATA_BUFFER1_SIZE)
              {
                if(file_len_rec1 == 0)
                {
                  Serial.println("3 OK : Filling Buffer 1");
                }
                led_buffer1[file_len_rec1++] = value;
              }
              else
              if(file_len_rec2 < MAX_DATA_BUFFER2_SIZE)
              {
                if(file_len_rec2 == 0)
                {
                  Serial.println("3 OK : Filling Buffer 2");
                }
                led_buffer2[file_len_rec2++] = value;
              }
              else
              {
                current_step = 3;
                value = -1;
              }
              
              if((file_len_rec1+file_len_rec2+2) >= file_size)
              {
                 current_step = 3;
                 value = -1;
              }
              else
              {
                value = client.read();
              }
            }

            //Serial.print("3 OK : ");
            //itoa(file_len_rec1+file_len_rec2,(char*)temp_data, 10);
            //Serial.println((char*)temp_data);
            client.write("OK",2);

            if(3 == current_step)
            {
              Serial.println("3 Done.");
              client.stop();
            }
            else
            {
              break;
            }
          }
          else
          {
            break;
          }
        }  
        
        case 3:
        {
          Serial.print("4 Strip Length: ");
          itoa(strip_length,(char*)temp_data,10);
          Serial.println((char*)temp_data);

          Serial.print("4 Total Size saved: ");
          itoa(file_len_rec1+file_len_rec2,(char*)temp_data,10);
          Serial.println((char*)temp_data);
          
          if(strip_length > 0)
          {
            led_buffer_len = ((file_len_rec1+file_len_rec2+2)/strip_length); 
          
            send_leds_status = SEND_LEDS_STATUS_START;
          }
          current_step = 0;
          current_time = system_time;
        }
        break;
      }
    }
  }
  else
  if(client_connected != 0)
  {
    client_connected = 0;
    current_step = 0;
    Serial.println("Disconnected.");
  }    
  
}



#define LEDS_DELAY_RESET 10
void send_leds(void)
{  
  static uint32_t leds_delay = LEDS_DELAY_RESET;
  static uint32_t leds_buffer1_index = 0;
  static uint32_t leds_buffer2_index = 0;
  static uint8_t leds_initialised = 0;
  static uint32_t leds_effect_iterator = 0;
  
  //Serial.print(send_leds_status);
  system_time++;
  
  switch(send_leds_status)
  {
    case 0:
    {
      
      if(Serial2.read() == 'X')
      {
        
        while(Serial2.read() != -1)
        {
          ;
        }
        
        Serial.print("Start Handshake with LED driver.");
        Serial2.print("L");
        uint8_t temp[2] = {strip_length/256,strip_length%256}; /* value '900' in 2 bytes 3 132, msb first. */
        Serial2.write(temp,2);
        Serial2.print("\n");
        send_leds_status = 1;
      }
      else
      {
        Serial2.write("XXXXXXXXXX",10);
      }
    }
    break;

    case 1:
    {
      if(Serial2.read() == 'Y')
      {
        Serial.print("LED driver is ready.");
        leds_buffer1_index = 0;
        leds_buffer2_index = 0;
        send_leds_status = 2;
      }
    }
    break;

    case 2:
    {
      if(Serial2.read() == 'R')
      {
        if(((leds_effect_iterator+1)*strip_length) <= MAX_DATA_BUFFER1_SIZE)
        {
          Serial2.write(&led_buffer1[leds_buffer1_index],strip_length);
          leds_buffer1_index += strip_length;
        }
        else
        if((leds_effect_iterator*strip_length) <= MAX_DATA_BUFFER1_SIZE)
        {
          uint32_t temp = MAX_DATA_BUFFER1_SIZE - (leds_effect_iterator*strip_length);
          
          Serial2.write(&led_buffer1[leds_buffer1_index],temp);
          leds_buffer1_index += temp;

          temp = strip_length-temp;
          
          Serial2.write(&led_buffer2[leds_buffer2_index],temp);
          leds_buffer2_index += temp;
        }
        else
        {
          Serial2.write(&led_buffer2[leds_buffer2_index],strip_length);
          leds_buffer2_index += strip_length;
        }

        
        if(leds_effect_iterator >= (led_buffer_len-1))
        {
          leds_effect_iterator = 0;
          leds_buffer1_index = 0;
          leds_buffer2_index = 0;
        }
        else
        {
          leds_effect_iterator++;
        }
        
        send_leds_status++;
        leds_delay = LEDS_DELAY_RESET;
      }
      else
      {
        if(leds_delay > 0)
        {
          leds_delay--;
        }
        else
        {
          /* Fill in with nulls. */
          Serial2.write("\0",1);
        }
      }
      
    }
    break;
    
    case 3:
     {
      /* All data received, being sent to LEDs. */
      if(Serial2.read() == 'Y')
      {
        send_leds_status--;
        leds_delay = LEDS_DELAY_RESET;
      }
      else
      {
        if(leds_delay > 0)
        {
          leds_delay--;
        }
        else
        {
          /* Fill in with nulls. */
          Serial2.write("\0\0\0\0",4);
        }
      }
      
    }
    break;

    
    case 4:
    {
      ;/* Data file under update. */
    }
    break;

    case 5:
    {
      if(leds_initialised)
      {
        /* Data file finished update. */
        if(Serial2.read() == 'Y')
        {
          send_leds_status = 2;
        }
        else
        {
          Serial2.write("\0\0\0\0",4);
        }
      }
      else
      {
        digitalWrite(LEDS_DRIVER_RST,HIGH);
        send_leds_status = 0;
        leds_initialised = 1;
      }
    }
    break;
    
    default:
    {}
    break;
  }
  
}


uint8_t already_connected = 0;
uint32_t connected_timeout = 6000;
uint16_t save_new_pass = 800;
void loop()
{
  wl_status_t status = WL_IDLE_STATUS;
  
  char temp_ssid[SSID_MAX_LEN];
  char temp_pass[PASS_MAX_LEN];
  delay(10);

  if(digitalRead(ERASE_MEM_REQUEST_PIN) == 0)
  {
      if(save_new_pass > 0)
      {
        save_new_pass--;
      }
      else
      {
        save_new_pass = 800;
        Serial.print("PASS and SSID erase request... ");
        EEPROM.readBytes(SSID_ADDR,temp_ssid,SSID_MAX_LEN);
        if(temp_ssid[0] != 0xFF)
        {
          memset(temp_ssid,0xff,SSID_MAX_LEN);
          memset(temp_pass,0xff,PASS_MAX_LEN);
          EEPROM.writeBytes(SSID_ADDR,temp_ssid,SSID_MAX_LEN);
          EEPROM.writeBytes(PASS_ADDR,temp_pass,PASS_MAX_LEN);
          EEPROM.commit();
          Serial.println("Done.");
        }
        else
        {
          Serial.println("Already cleared.");
        }
      }
      
      
  }
  else
  {
    save_new_pass = 1000;  
  }
  
  handle_update();

  if(0 == already_connected || connected_timeout > 0)
  {
    
/*
    if(0 != already_connected)
    {
      connected_timeout--;
      if(connected_timeout == 0)
      {
        Serial.println("Hotspot turned off.");
        WiFi.softAPdisconnect(true);
      }
    }
*/
    server.handleClient();
    
  }
  
  if(0 == already_connected || connected_timeout > 0)
  {
    

    if(0 != already_connected)
    {
      if(connected_timeout-- == 0)
      {
        Serial.println("Hotspot turned off.");
        WiFi.softAPdisconnect(true);
      }
    }

    server.handleClient();
    
  }
  if(0 == work_status)
  {
    if(WiFi.status() == WL_DISCONNECTED)
    {
      work_status = 1;
    }
  }
  else
  if (1 == work_status)
  {
    digitalWrite(LED_CONNECTED,LOW);
    Serial.println("SSID:");
    Serial.println(ssid);
    Serial.println("Password:");
    Serial.println(password);
    
    uint8_t temp_counter = 1;
    while(temp_counter-- > 0)
    {
      if((0 != password[0])&&(0xff != password[0])) 
      {
        Serial.print("Starting wifi connection");
        
        WiFi.begin(ssid, password);
      }
      else
      {
        WiFi.begin(ssid, NULL);
      }

      uint8_t temp_counter2 = 15;
      while(temp_counter2-- > 0)
      {
        Serial.print(".");
        delay(1000);
        if(WiFi.status() == WL_CONNECTED)
        {
          Serial.println(".");
          work_status = 2;          
          temp_counter = 0;
          temp_counter2 = 0;
        }
      }
    }

    if(WiFi.status() != WL_CONNECTED)
    {
      //server.send(200, "text/html",end_webpage_error); 
      Serial.println(".");
      Serial.println("Connection error.");
      Serial.println("Waiting new setup.");
      if(already_connected)
      {
        work_status = 0;
      }
      else
      {
        /* 
        server.stop();
        server.on(F("?"), handle_NotFound);
        server.onNotFound(handle_NotFound);
        server.begin();
        */
        work_status = 3;
      }
    }
  }
  else
  if (2 == work_status)
  {
    digitalWrite(LED_CONNECTED,HIGH);
    
    EEPROM.readBytes(SSID_ADDR,temp_ssid,SSID_MAX_LEN);
    EEPROM.readBytes(PASS_ADDR,temp_pass,PASS_MAX_LEN);
    
    if((0!=string_compare(ssid,temp_ssid)) || (0!=string_compare(password,temp_pass)))
    {
      Serial.print("Saving new SSID and PASS... ");
      EEPROM.writeBytes(SSID_ADDR,ssid,SSID_MAX_LEN);
      EEPROM.writeBytes(PASS_ADDR,password,PASS_MAX_LEN);
      EEPROM.commit();
      Serial.println("Done.");
    }
    
      
    
    Serial.println("IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("Ready to start.");
    work_status = 0;
  }
  else
  if(3 == work_status)
  {
    
  }
  else
  {
    work_status = 0;
  }
}

void handle_workOnConnect()
{
  handle_NotFound();
}

void handle_workNotFound()
{
  handle_NotFound();
}



void handle_NotFound() {
  //server.sendContent(request_webpage);
  static uint8_t received_ssid = 0;  
  server.arg("ssid").toCharArray(ssid,100);
  server.arg("password").toCharArray(password,100);

  if(ssid[0] != 0 || WiFi.status() == WL_CONNECTED)
  {
    if(work_status == 1)
    {
      Serial.print("Query on connection: ");
      Serial.println("Waiting...");
      server.send(200, "text/html",wait_webpage); 

    }
    else
    {
      Serial.print("Query on connection: ");
      if(WiFi.status() == WL_CONNECTED)
      {
        already_connected = 1;
        Serial.println("IP webpage sent.");
        char data_to_send[500];
        sprintf(data_to_send, ready_webpage, WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3],max_mem_available);
        server.send(200, "text/html",data_to_send); 
        
      }
      else
      if(received_ssid == 1)
      {
        Serial.println("Request SSID and PASS.");
        received_ssid = 0;
        server.send(200, "text/html",request_webpage); 
      }
      else
      {
        Serial.println("Received SSID and PASS.");
        received_ssid = 1;
        server.send(200, "text/html",wait_webpage); 
        work_status = 1;
      }
    }
  }
  else
  {
    Serial.println("New request.");
    server.send(200, "text/html",request_webpage); 
  }
}
