/**
 * @file
 * @defgroup lavr-wifi AVR ESP8266 interfacing library <avr-wifi.h>
 * @code #include <avr-wifi.h> @endcode
 * @brief Library for UART communication with ESP8266 communication.
 *        
 * 
 * @version 1.1
 * @copyright Piotr Styczyński (C)
 * @author Piotr Styczyński
 */

#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _WIFI_H
#define _WIFI_H


/**
 * The handler for wifi signals, must be defined elsewhere
 * 
 * @param event event type (can be WIFI_EVENT_DATA etc.)
 * @param input input string data
 * @param len   length of data input
 */
extern void wifi_event_handler(int event, const char* input, const int len);

/**
 * @def COMMAND_BUFFER_LEN
 * Determines buffer length of internal command buffer.
 *
 * The responses from ESP8266 normally should never fill this up!
 */
#define COMMAND_BUFFER_LEN 700

/**
 * Internal command buffer.
 * The responses from ESP8266 normally should never fill this up!
 */
char wifi_command_buffer[COMMAND_BUFFER_LEN];


/**
 * @def WIFI_EVENT_UNKNOWN
 * Unknown event type.
 * Occurs when the message from ESP8266 could not be properly categorized.
 */
#define WIFI_EVENT_UNKNOWN 0

/**
 * @def WIFI_EVENT_DATA
 * Data +IPD event.
 * Triggered when ESP8266 receives data.
 */
#define WIFI_EVENT_DATA 1

/**
 * @def WIFI_EVENT_OK
 * OK event.
 * Signalizes ESP8266 module has received and correctly executed the command.
 */
#define WIFI_EVENT_OK 2

/**
 * @def WIFI_EVENT_ERROR
 * ERROR event.
 * Signalizes ESP8266 module has send ERROR command.
 */
#define WIFI_EVENT_ERROR 3

/**
 * @def WIFI_EVENT_WIFI_CONNECTED
 * Event when module connects to wifi.
 */
#define WIFI_EVENT_WIFI_CONNECTED 4

/**
 * @def WIFI_EVENT_WIFI_DISCONNECTED
 * Event when module disconnects from wifi.
 */
#define WIFI_EVENT_WIFI_DISCONNECTED 5

/**
 * @def WIFI_EVENT_WIFI_GOT_IP
 * Event when module gots its ip from the wifi.
 */
#define WIFI_EVENT_WIFI_GOT_IP 6

/**
 * @def WIFI_EVENT_CLOSED
 * Event when link was closed.
 */
#define WIFI_EVENT_CLOSED 7

/**
 * @def WIFI_EVENT_CONNECTED
 * Event when link was created and connected to.
 */
#define WIFI_EVENT_CONNECTED 8

/**
 * @def WIFI_EVENT_SEND_OK
 * Event of confirmation of correctly sending message.
 */
#define WIFI_EVENT_SEND_OK 9

/**
 * @def WIFI_EVENT_TIMEOUT
 * Event when theres not response from module but is should be one.
 * (data / OK status etc. was expected but nothing happened)
 */
#define WIFI_EVENT_TIMEOUT 10

/**
 * @def WIFI_EVENT_ANY
 * Event triggered on any message from ESP8266.
 * This event always precedes any other event.
 */
#define WIFI_EVENT_ANY 11

/**
 * Clear internal command buffer.
 */
static inline void wifi_clear_command_buffer(void);

/**
 * Send command to the ESP8266
 * In normal conditions you should listen for ESP8266 answers.
 * This function is non-blocking and only sends data.
 * It does not check for OK status or anything fancy.
 *
 * About command format:
 * The command is automatically prefixed with AT
 * So to send AT+COMMAND just call @code "+COMMAND" @endcode
 * 
 * @param command Correct AT command without "AT" prefix
 */
static inline void wifi_send_command_no_block(const char* command);

/**
 * Send command to the ESP8266
 * Then waits for STATUS return.
 *
 * WARN: 
 *   In some cases the command DOES NOT provide nor OK or ERROR
 *   response and this case can be a bit messy! :(
 *   If you run some command that does not return OK/ERROR
 *   then you MUST USE wifi_send_command_no_block and then manually
 *   listen for anwer or do easier - use wifi_listen functions family :)
 *
 * About command format:
 * The command is automatically prefixed with AT
 * So to send AT+COMMAND just call @code "+COMMAND" @endcode
 * 
 * @param command Correct AT command without "AT" prefix
 */
static inline void wifi_send_command(const char* command);

/**
 * @def WAIT_FOR_STATUS
 *
 * Value signalizator for wifi_listen to listen only for status.
 * Listening would end if and only if:
 *   * status OK/ERROR is returned from ESP8266
 *   * timeout event occurs when waiting for status info 
 */
#define WAIT_FOR_STATUS 0

/**
 * @def WAIT_FOR_DATA
 *
 * Value signalizator for wifi_listen to listen for incoming data.
 * Listening would end if and only if:
 *   * status OK/ERROR is returned from ESP8266
 *   * timeout event occurs when waiting for data
 *   * ESP8266 received data and returns +IPD command
 *     then the data is captured and listening ends.
 */
#define WAIT_FOR_DATA 1

/**
 * Execution-blocking listener.
 * Listens for ESP8266 responses and quit if neccessary.
 * Automatically invokes all needed events.
 *
 * To see working modes see WAIT_FOR_DATA and WAIT_FOR_STATUS.
 * 
 * @param  wait_mode working mode
 * @return zero if no error was reported, non-zero exit code otherwise
 */
static inline int wifi_listen(int wait_mode);

/**
 * Execution-blocking waiting for status code.
 * See wifi_listen and WAIT_FOR_STATUS.
 * 
 * @return zero if no error was reported, non-zero exit code otherwise
 */
static inline int wifi_wait_for_status(void);

/**
 * Execution-blocking waiting for +IPD data.
 * See wifi_listen and WAIT_FOR_DATA.
 * 
 * @return zero if no error was reported, non-zero exit code otherwise
 */
static inline int wifi_wait_for_data(void);

/**
 * Send echo disabling command to ESP8266
 */
static inline void wifi_disable_echo(void);

/**
 * Connects to desired wifi hotspot.
 * 
 * @param id     wifi USSID
 * @param passwd wifi password
 */
static inline void wifi_connect(const char* id, const char* passwd);

/**
 * Creates connection to the desired ip/url.
 * 
 * @param protocol Connection protocol - "TCP" or other supported by ESP8266
 * @param url      IP/URL adress
 * @param port     port
 */
static inline void wifi_link_open(const char* protocol, const char* url, const int port);

/**
 * Closes opened connection.
 */
static inline void wifi_link_close(void);


/**
 * Send text data through ESP8266.
 * Does not listen for ESP8266 status nor data.
 *
 * This function should be used with help of wifi_wait_for_data function.
 * 
 * @param contents String data to send
 */
static inline void wifi_send_no_block(const char* contents);

/**
 * Send text data through ESP8266.
 * Then waits for data response.
 * 
 * @param contents String data to send
 */
static inline void wifi_send(const char* contents);

/**
 * Send basic http GET header through ESP8266.
 * Then wait for response.
 * 
 * The header looks like follows:
 * 
 * GET <route> HTTP/1.1
 * Host: <host>
 * Connection: keep-alive
 * Cache-Control: max-age=0
 * 
 * 
 * @param host server adress to send GET request to
 * @param route server inside adress that you want to get
 */
static inline void wifi_send_http_get(const char* host, const char* route) {
  //TODO: Implement
}

/**
 * Perform basic helpful initialization.
 * This Library DOES NOT need any kind of "big" initializations.
 * This procedure just simplifies everything.
 * It configures ESP8266 a bit to be more friendly :)
 *
 * - it disables ESP8266 echo (otherwise the library is kind of unfriendly)
 */
static inline void wifi_init(void);


static inline void wifi_clear_command_buffer(void) {
  for(int i=0;i<COMMAND_BUFFER_LEN;++i) {
    wifi_command_buffer[i] = '\0';
  }
}

static inline void wifi_send_command_no_block(const char* command) {
  uart_puts("AT");
  uart_puts(command);
  uart_puts("\r\n");
  _delay_ms(10);
}


static inline void wifi_send_command(const char* command) {
  wifi_send_command(command);
  wifi_wait_for_status();
}


static inline int wifi_listen(int wait_mode) {
  
  int i = 0;
  uart_flush();
  
  int command_buffer_iter = 0;
  wifi_command_buffer[0] = '\0';
  
  uint16_t input_count = 0;
  uint16_t input = 0;
  int input_hb = 0;
  int input_lb = 0;
  
  while(1) {
    ++i;
    //lcd_clrscr();
    //lcd_puts("wait\n  :/");
    if(i > F_CPU) {
      wifi_event_handler(WIFI_EVENT_TIMEOUT, wifi_command_buffer, 0);
      return 0;
    }
    if((input_count = uart_available()) > 0) {
      for(int i=0;i<input_count;++i) {
        input = uart_getc();
        input_hb = (input>>8) & 0xFF;
        input_lb = input & 0xFF;
        if(input_lb != '\n') {
          wifi_command_buffer[command_buffer_iter] = input_lb;
          ++command_buffer_iter;
          wifi_command_buffer[command_buffer_iter] = '\0'; 
        } else {
          
          wifi_event_handler(WIFI_EVENT_ANY, wifi_command_buffer, command_buffer_iter);
          
          if(command_buffer_iter <= 1) {
            
          } else if(strcmp(wifi_command_buffer, "OK\r") == 0) {
            wifi_event_handler(WIFI_EVENT_OK, wifi_command_buffer, command_buffer_iter);
            return 1;
          } else if(strcmp(wifi_command_buffer, "ERROR\r") == 0) {
            wifi_event_handler(WIFI_EVENT_ERROR, wifi_command_buffer, command_buffer_iter);
            return 0;
          } else if(strcmp(wifi_command_buffer, "> \r") == 0) {
            
          } else if(strcmp(wifi_command_buffer, "\r") == 0) {
            
          } else if(strcmp(wifi_command_buffer, "WIFI GOT IP\r") == 0) {  
            wifi_event_handler(WIFI_EVENT_WIFI_GOT_IP, wifi_command_buffer, command_buffer_iter);
          } else if(strcmp(wifi_command_buffer, "CONNECT\r") == 0) {  
            wifi_event_handler(WIFI_EVENT_CONNECTED, wifi_command_buffer, command_buffer_iter);
          } else if(strcmp(wifi_command_buffer, "SEND OK\r") == 0) {  
            wifi_event_handler(WIFI_EVENT_SEND_OK, wifi_command_buffer, command_buffer_iter);
          } else if(strcmp(wifi_command_buffer, "CLOSED\r") == 0) {
            wifi_event_handler(WIFI_EVENT_CLOSED, wifi_command_buffer, command_buffer_iter);
            if(wait_mode == WAIT_FOR_DATA) return 0;
          } else if(strcmp(wifi_command_buffer, "WIFI CONNECTED\r") == 0) {  
            wifi_event_handler(WIFI_EVENT_WIFI_CONNECTED, wifi_command_buffer, command_buffer_iter);
          } else if(strcmp(wifi_command_buffer, "WIFI DISCONNECT\r") == 0) { 
            wifi_event_handler(WIFI_EVENT_WIFI_DISCONNECTED, wifi_command_buffer, command_buffer_iter);
          } else if(strcmp(wifi_command_buffer, "busy p...\r") == 0) {  
            _delay_ms(500);
          } else if(strncmp(wifi_command_buffer, "Recv ", 5) == 0) {
            
          } else if(strncmp(wifi_command_buffer, "+IPD,", 5) == 0) {
            
            int rrr = 0;
              while(rrr < 5000) {
                 ++rrr;
                 for(int fff=0;fff<10;++fff) {
                 if((input_count = uart_available()) > 0) {
                   for(int i=0;i<input_count;++i) {
                      input = uart_getc();
                      input_hb = (input>>8) & 0xFF;
                      input_lb = input & 0xFF;
                      wifi_command_buffer[command_buffer_iter] = input_lb;
                      ++command_buffer_iter;
                      wifi_command_buffer[command_buffer_iter] = '\0';
                   }
                 }
                  }
                 _delay_ms(1);
              }
            
            int bytes = 0;
            int j = 4; 
            if(wifi_command_buffer[j] == ',') {
              ++j;
 
              for(;j<command_buffer_iter;++j) {
                  if(wifi_command_buffer[j] == ':' || wifi_command_buffer[j] == '\0') break;
                  bytes *= 10;
                  bytes += wifi_command_buffer[j]-'0';
              }
              if(wifi_command_buffer[j] == ':') ++j;
            
              for(int t=0;t<bytes;++t) {
                wifi_command_buffer[t] = wifi_command_buffer[j+t]; 
                
              }
              wifi_command_buffer[bytes]='\0';
              command_buffer_iter=bytes;
            
              wifi_event_handler(WIFI_EVENT_DATA, wifi_command_buffer, bytes);
              if(wait_mode == WAIT_FOR_DATA) return 1;
            }
            
          } else {
            wifi_event_handler(WIFI_EVENT_UNKNOWN, wifi_command_buffer, command_buffer_iter);
          }
          
          command_buffer_iter = 0;
          wifi_command_buffer[0] = '\0';
        }
      }
    }
  }
  return 0;
}


static inline int wifi_wait_for_status(void) {
  return wifi_listen(WAIT_FOR_STATUS);
}


static inline int wifi_wait_for_data(void) {
  return wifi_listen(WAIT_FOR_DATA);
}


static inline void wifi_disable_echo(void) {
  wifi_send_command_no_block("E0");
  wifi_wait_for_status();
}


static inline void wifi_connect(const char* id, const char* passwd) {
  wifi_send_command_no_block("+CWMODE=1");
  wifi_wait_for_status();

  wifi_send_command_no_block("+CIPMUX=0");
  wifi_wait_for_status();

  wifi_clear_command_buffer();
  sprintf(wifi_command_buffer, "+CWJAP=\"%s\",\"%s\"", id, passwd);
  wifi_send_command_no_block(wifi_command_buffer);
  wifi_wait_for_status();
}


static inline void wifi_link_open(const char* protocol, const char* url, const int port) {
  //wifi_clear_command_buffer();
  sprintf(wifi_command_buffer, "+CIPSTART=\"%s\",\"%s\",%d", protocol, url, port);
  wifi_send_command_no_block(wifi_command_buffer);
  //wifi_send_command_no_block("+CIPSTART=\"TCP\",\"api.openweathermap.org\",80");
  wifi_wait_for_status();
}


static inline void wifi_link_close(void) {
  wifi_send_command_no_block("+CIPCLOSE");
  wifi_wait_for_status();
}


static inline void wifi_send_no_block(const char* contents) {
  const int len = strlen(contents);
  wifi_clear_command_buffer();
  sprintf(wifi_command_buffer, "+CIPSEND=%d", len);
  wifi_send_command_no_block(wifi_command_buffer);
  wifi_wait_for_status();
  _delay_ms(500);
  uart_puts(contents);
}


static inline void wifi_send(const char* contents) {
  wifi_send_no_block(contents);
  wifi_wait_for_data();
}


static inline void wifi_init(void) {
  _delay_ms(500);
  wifi_send_command_no_block("");
  wifi_wait_for_status();
  wifi_disable_echo();
  _delay_ms(500);
}

#endif /* _WIFI_H */
