
# Weather fetching on Atmega32 + ESP8266 + LCD1602

### About

[See the full docs!](http://styczynski.ml/avr-weather-esp8266/)
[And also see this on git!](https://github.com/isis97/avr-weather-esp8266)


![Video](http://styczynski.ml/avr-weather-esp8266/view.gif)

### Short description

This project tends to implement:

* ESP8266 module messaging from AVR controller
* Basic Http request + JSON parsing
* LCD support

### Why it has been made?

Because of the microcontroller exercises we did on our studies.

### What does it do?

* Boots up (if you press any key) the avr enters

`settings mode` and asks for wifi creditentials

* If no key was pressed during boot then normal procedure takes place

* AVR tries to connect to the wifi

* Then connects to `openweathermap.com`

* Asks politely for weather data for `Warsaw`

* Then fetches `JSON` with the most trivial ways

* Displays the data

* And sometimes refreshes the collected data :)

### Pinout

#### Main wirings

The AVR connections are basic and listed below:

```javascript

'ATMEGA32'   PA0 -> D4   'LCD'
'ATMEGA32'   PA1 -> D5   'LCD'
'ATMEGA32'   PA2 -> D6   'LCD'
'ATMEGA32'   PA3 -> D7   'LCD'
'ATMEGA32'   PA4 -> RS   'LCD'
'ATMEGA32'   PA5 -> RW   'LCD'
'ATMEGA32'   PA6 -> E    'LCD'
'ATMEGA32'   RXD -> TX   'ESP8266'
'ATMEGA32'   TXD -> RX   'ESP8266'
'ESP8266'    EN  -> VCC  'ESP8266'

```

And of course `V0`, `VCC`, `GND`, `VSS`, `VDD`, `A`, `K` but they are trivial.

#### Buttons

And not mentioned in layout two buttons for settings screen:

```javascript

'ATMEGA32'  PD6  -(BUTTON)->  GND
'ATMEGA32'  PD7  -(BUTTON)->  GND

```


In settings mode:

* `PD6` is used have next character functionality.

* `PD7` changes current character.

Schematic of connections (pretty small - try to enlarge with zooming hand):

![Scheme](http://styczynski.ml/avr-weather-esp8266/scheme.svg)

**Notice:**

*The `ESP8266` module was connected to the `5V USB` plug.
The module is designed to work for 3V but working on higher
voltage is possible (as demonstrated in this case).*

*It's straightforward method and in normal conidtions
you should redesign scheme a little and provide 3V voltage source
for your wifi module.*

*You will probably need logic level shifting
form `5V (atemga)` to `3V (esp8266)` or you can just run atmega on 3V
(notice that LCD needs standard 5V anyway!)*


#### Mounting

You can mount the wires propably on pcb plate I used universal brakboard
that does not require soldering but it's rather a messy pile of cables, huh!


### About building

#### Build process

**Warnning!**

*Before building make sure all it's configured correctly (see configuration for more details)!*

To build the project do the following:

* Create release folder e.g. `mkdir release`

* Go into that folder `cd release`

* Run CMAKE to generate needed makefiles `cmake ..`

* Build app using make `make`

* Optionally update documentation throught `make doc`

* Optionally setup manually the needed fuse bits using `avrdude`!

* Flash controller with `make upload`

* Optionally clean build with `make clean`


#### Configuration

Before continouing make sure that `USSID` and `PASSWD` Variables
in `src/main.c` are configured to use with your wifi access point.

Then make sure that `APIKEY` in line with `wifi_send` is matching
apikey for your openweathermap account.
And the url is for location desired by you.
See `openweathermap api documentation` for more details :)


The main controller config is placed in `CMakeLists.txt` file.

Please change these as you wish:

```bash
# Variables regarding the AVR chip
set(MCU   atmega32)
set(F_CPU 8000000)
set(BAUD  9600)
set(PROG_TYPE usbasp)
set(PROG_ARGS )
```


`BAUD` is not really needed and `PROG_ARGS` are additional paramters for avrdude.



Basic example config for `Atmega328P-PU` running (by default) at 1MHz:

```bash
# Variables regarding the AVR chip
set(MCU   atmega328p)
set(F_CPU 1000000)
set(BAUD  2400)
set(PROG_TYPE usbasp)
set(PROG_ARGS )
```

### Some source code

This snippet presents piece of source code from `main.c`
(and is good introduction to the project insides as well as wifi.h module):

```cpp

// Handle ESP8266 events
void wifi_event_handler(int event, const char* input, const int len) {

  switch(event) {
    case WIFI_EVENT_ANY: {
       //
       // You can display the input
       // To provide debug info
       // This kind of event (ANY)
       // is called for any event
       //
       return;
    } break;

    case WIFI_EVENT_ERROR: {
       // We got error!
    } break;

    // ESP8266 is not responding
    case WIFI_EVENT_TIMEOUT: {
       // We got timeout!
    } break;

    // Wifi was connected
    case WIFI_EVENT_WIFI_CONNECTED: {
       // We are conneted to the access point!
    } break;

    // Wifi was disconnected
    case WIFI_EVENT_WIFI_DISCONNECTED: {
       // We are disconneted to the access point!
    } break;

    // Connect to host
    case WIFI_EVENT_CONNECTED: {
       // We are connected to the ip/url specified earlier throug wifi_link_open
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


int main(void) {

  // Init UART
  uart_init(UART_BAUD_SELECT(2400, F_CPU));

  // Init buttons

  //
  // ...
  //

  // Init LCD
  lcd_init(LCD_DISP_ON);
	lcd_gotoxy(0, 0);
	lcd_clrscr();

  // Init wifi and connect to the desired network
  lcd_clrscr();
  lcd_puts("Initializing...");
  _delay_ms(1000);
  wifi_init();

  //
  // Check if any button was pressed
  // then go into settings mode :)
  //

  //
  //  ...
  //

  //
  //  Normal boot sequence
  //
  normal_boot:

  // Display greeting on lcd
  lcd_puts("Hello");
  _delay_ms(1000);
  lcd_clrscr();
  _delay_ms(1500);


  lcd_clrscr();
  lcd_puts("Conecting...");
  _delay_ms(1000);

  //
  // Display USSID/PASSWD data on the screen
  //
  lcd_clrscr();
  lcd_puts("ID: ");
  lcd_puts(USSID);
  lcd_puts("\nPASS: ");
  lcd_puts(PASSWD);
  _delay_ms(1500);

  // Try to connect to the access point
  wifi_connect(USSID, PASSWD);

  while(1) {
    // To make sure nothing dumb trash does not come
    // through UART interface
    uart_flush();

    // Connect to openweathermap
    wifi_link_open("TCP", "api.openweathermap.org", 80);

    // Request data from REST api
    wifi_send("GET /data/2.5/weather?q=warsaw&APPID=cf24c94ac2550178e2ea4e970cd0f416 HTTP/1.1\r\nHost: api.openweathermap.org\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\n\r\n\r\n");

    // Wifi functions will call wifi_event_handler function on any event (it's callback function)
  }

  return 0;
}
```
