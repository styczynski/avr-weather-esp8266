/**
// @file 
// ESP8266 weather fetcher
// 
// Copyright by Piotr Stczyński 2017 (C)
/*

/**
// Configure UART RX buffer
*/
#define UART_RX1_BUFFER_SIZE 1024
#define USART1_LARGE_BUFFER

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "uart.h"
#include "lcd.h"
#include "avr-wifi.h"

char* USSID = "StyczynWIFI";
char* PASSWD = "qwertyui";

/**
// Some containers for weather data
*/
char weather_data_buf[25];

char weather_descr[15] = "\0";
char weather_name[15] = "\0";

int weather_pressure = 0;
double weather_temp = 0.0f;
double weather_humidity = 0.0f;

/**
// Simple JSON parser based on string finding
// This MAY easily break on invalid JSON
// But who cares
// Fully-loaded JSON parser for AVR is an overkill!
// So lets use old school methods :)
*/
void search_json(char* dest, const char* input, const char* prop) {
   char* loc = strstr(input, prop);
   const int propLen = strlen(prop);
   loc += propLen;
   int i = 0;
   while(*loc == '\"' || *loc == ':') ++loc;
   while(1) {
      if(*loc == '\"' || *loc == '\0') {
        break; 
      }
      dest[i] = *loc;
      ++i;
      ++loc;
   }
   dest[i] = '\0';
}

char info1[16];
char info2[100];

/** Handle ESP8266 events */
void wifi_event_handler(int event, const char* input, const int len) {

  switch(event) {
    case WIFI_EVENT_ANY: {
       // May display message (debug purposes )
       //lcd_clrscr();
       //lcd_putc('$');
       //lcd_puts(input);
       //_delay_ms(1000);
       return;
    } break;

    case WIFI_EVENT_ERROR: {
       lcd_clrscr();
       lcd_puts("ERROR\n  :<");
    } break;

    // ESP8266 is not responding
    case WIFI_EVENT_TIMEOUT: {
       lcd_clrscr();
       lcd_puts("TIMEOUT\n  :<");
    } break;

    // Wifi was connected
    case WIFI_EVENT_WIFI_CONNECTED: {
       lcd_clrscr();
       lcd_puts("Wifi connected\n  :)");
    } break;

    // Wifi was disconnected
    case WIFI_EVENT_WIFI_DISCONNECTED: {
       lcd_clrscr();
       lcd_puts("Wifi disconnected\n  :(");
    } break;

    // Connect to host
    case WIFI_EVENT_CONNECTED: {
       lcd_clrscr();
       lcd_puts("Fetch data...");
    } break;

    // Host sends data
    case WIFI_EVENT_DATA: {
      //
      // Look for JSON data in response
      //
      search_json(weather_descr, input, "description");
      search_json(weather_name, input, "name");
      
      search_json(weather_data_buf, input, "humidity");
      sscanf(weather_data_buf, "%lf", &weather_humidity);
      
      search_json(weather_data_buf, input, "pressure");
      sscanf(weather_data_buf, "%d", &weather_pressure);
      
      search_json(weather_data_buf, input, "temp");
      sscanf(weather_data_buf, "%lf", &weather_temp);

      
      // We do not need this connection anymore
      wifi_link_close();

      // Format received data
      sprintf(info1, "%s Weather:", weather_name);
      sprintf(info2, "%s | Temp: %.1lf C | Humidity: %.0lf %% | Pressure: %d HPa", weather_name, weather_temp-273.15, weather_humidity, weather_pressure );

      // 
      // Display data continously in text shifting manner
      //  
      int shift = 0;
      const int info_len = strlen(info2);
      int cycles = 0;
  
      while(1) {
         lcd_clrscr();
         lcd_gotoxy(0, 0);
	       lcd_nputs(info1, 16);
	       lcd_gotoxy(0, 1);
	       lcd_nputs(info2+shift, 16);
	       _delay_ms(500);
	       ++shift;
	       if(shift > info_len - 4) {
             shift = 0;
             ++cycles;   
         }
         //
         // End - return to main and then prepare next fetch request
         //
         if(cycles > 15) break;
      }
      
      lcd_nputs("Update...", 16);
      lcd_clrscr();
    } break;
  }
}

/*
// Function checking what button is pressed
*/
static inline int check_click(void) {
  int cnt = 0;
  int oncnt7 = 0;
  int oncnt6 = 0;
  
  while(1) {
    ++cnt;
    if(!(PIND & (1<<7)))  {
      ++oncnt7;
    }
    if(!(PIND & (1<<6)))  {
      ++oncnt6;
    }
    if(cnt > 20){
      if(oncnt7 > cnt/2) {
        return 7;
      }
      if(oncnt6 > cnt/2) {
        return 6;
      }
      return 0;
      cnt = 0;
      oncnt7 = oncnt6 = 0;
    }
    _delay_ms(5);
  }
}

int main(void) {

  // Init UART
  uart_init(UART_BAUD_SELECT(2400, F_CPU));

  // Init buttons
  DDRD = DDRD & ~(1<<7);
  PORTD = PORTD | (1<<7); 
  
  DDRD = DDRD & ~(1<<6);
  PORTD = PORTD | (1<<6); 

  // Init LCD
  lcd_init(LCD_DISP_ON);
	lcd_gotoxy(0, 0);
	lcd_clrscr();

  // Init wifi and connect to the desired network
  lcd_clrscr();
  lcd_puts("Initializing...");
  _delay_ms(1000);
  wifi_init();

  /*
   * Check if any button was pressed
   * then go into settings mode :)
   */
  for(int i=0;i<25;++i) {
    int boot_click = check_click();
    if(boot_click != 0) {
      
      lcd_clrscr();
      lcd_puts("Settings...");
      _delay_ms(2000);
      lcd_clrscr();

      lcd_clrscr();
      lcd_gotoxy(0, 0);
      lcd_puts("Please provide\nwifi data :)");
      _delay_ms(1500);
      lcd_clrscr();


      /*
       * Section that enables long press when boot -> goes into settings mode
       * with wifi creds input :)
       */
      
      char inputUSSID[16] = "                ";
      char inputPASSWD[16] = "                ";
    
      int cnt = 0;
      int lcnt = 0;
      int oncnt7 = 0;
      int oncnt6 = 0;
      int loncnt7 = 0;
      int loncnt6 = 0;
      int pos = 0;
      int blinkc = 0;
      int blinkt = 0;
      
      int input_mode = 0;

      
      while(1) {
        ++cnt;
        ++lcnt;
        ++blinkc;
        if(!(PIND & (1<<7)))  {
          ++oncnt7;
          ++loncnt7;
        }
        if(!(PIND & (1<<6)))  {
          ++oncnt6;
          ++loncnt6;
        }
        if(lcnt > 120) {
          if(loncnt7 > lcnt/2) {
            // Long click 7
          
          }
          if(loncnt6 > lcnt/2) {
            // Long click 6
            if(input_mode == 0) {
              input_mode = 1;
              pos = 0;
            } else if(input_mode == 1) {
              for(int i=0;i<16;++i) {
                if(inputUSSID[i] == ' ') inputUSSID[i]='\0';
                if(inputPASSWD[i] == ' ') inputPASSWD[i]='\0';
              }
              strcpy(USSID, inputUSSID);
              strcpy(PASSWD, inputPASSWD);
              goto normal_boot;
            } 
          }
          loncnt6 = loncnt7 = lcnt = 0;
        }
        if(cnt > 20){
          if(oncnt7 > cnt/2) {
            // Click 7
            if(input_mode) {
              inputPASSWD[pos]++;
              inputPASSWD[pos] = (inputPASSWD[pos]-' ')%('z'-' '+1)+' ';
            } else {
              inputUSSID[pos]++;
              inputUSSID[pos] = (inputUSSID[pos]-' ')%('z'-' '+1)+' ';
            }
            lcd_clrscr();
            lcd_gotoxy(0, 0);
            if(input_mode) {
              lcd_puts("Passwd:\n");
              lcd_puts(inputPASSWD);
            } else {
              lcd_puts("USSID:\n");
              lcd_puts(inputUSSID);
            }
          }
          if(oncnt6 > cnt/2) {
            // Click 6
            ++pos;
            pos%=16;
          }
          cnt = 0;
          oncnt7 = oncnt6 = 0;
        }
        _delay_ms(5);
        if(blinkc>60) {
          blinkc = 0;
          if(input_mode) {
            if(blinkt) {
              char c = inputPASSWD[pos];
              inputPASSWD[pos] = '_';
              lcd_clrscr();
              lcd_puts("Passwd:\n");
              lcd_puts(inputPASSWD);
              inputPASSWD[pos] = c;
            } else {
              lcd_clrscr();
              lcd_puts("Passwd:\n");
              lcd_puts(inputPASSWD);
            }
          } else {
            if(blinkt) {
              char c = inputUSSID[pos];
              inputUSSID[pos] = '_';
              lcd_clrscr();
              lcd_puts("USSID:\n");
              lcd_puts(inputUSSID);
              inputUSSID[pos] = c;
            } else {
              lcd_clrscr();
              lcd_puts("USSID:\n");
              lcd_puts(inputUSSID);
            }
          }
          blinkt = !blinkt;
        }
      }
      
      lcd_puts("...");
      _delay_ms(2000);
    }
  }

  /*
   *  Normal boot sequence
   */
  normal_boot:
  
  // Display greeting on lcd
  lcd_puts("Hello");
  _delay_ms(1000);
  lcd_clrscr();
  _delay_ms(1500);


  lcd_clrscr();
  lcd_puts("Conecting...");
  _delay_ms(1000);

  lcd_clrscr();
  lcd_puts("ID: ");
  lcd_puts(USSID);
  lcd_puts("\nPASS: ");
  lcd_puts(PASSWD);
  _delay_ms(1500);
  
  wifi_connect(USSID, PASSWD);
  
  while(1) {
    uart_flush();

    // Connect to openweathermap
    wifi_link_open("TCP", "api.openweathermap.org", 80);
    
    // Request data from REST api
    wifi_send("GET /data/2.5/weather?q=warsaw&APPID=cf24c94ac2550178e2ea4e970cd0f416 HTTP/1.1\r\nHost: api.openweathermap.org\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\n\r\n\r\n");
  }
  
  return 0;
}
