# Weather fetching on Atmega32 + ESP8266 + LCD1602

### About

[See the full docs!](http://styczynski.ml/avr-weather-esp8266/)

![Scheme](http://styczynski.ml/avr-weather-esp8266/scheme.svg)

### Short description

This project tends to implement:

* ESP8266 module messaging from AVR controller
* Basic Http request + JSON parsing
* LCD support

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

The AVR connections are basic and listed below:
  
  
  
* `(ATMEGA32) PA0 -> D4 (LCD)` 

* `(ATMEGA32) PA1 -> D5 (LCD)` 

* `(ATMEGA32) PA2 -> D6 (LCD)` 

* `(ATMEGA32) PA3 -> D7 (LCD)`  

* `(ATMEGA32) PA4 -> RS (LCD)` 

* `(ATMEGA32) PA5 -> RW (LCD)` 

* `(ATMEGA32) PA6 -> E  (LCD)` 

* `(ATMEGA32) RXD -> TX (ESP8266)` 

* `(ATMEGA32) TXD -> RX (ESP8266)` 

* `(ESP8266) EN -> (ESP8266) VCC` 





And of course `V0`, `VCC`, `GND`, `VSS`, `VDD`, `A`, `K` but they are trivial.

And not mentioned in layout two buttons for settings screen:
* `PD6 -(BUTTON)-> GND` 

* `PD7 -(BUTTON)-> GND`





In settings mode:
  
* `PD6` is used have next character functionality. 

* `PD7` changes current character.




### About building

To build the project do the following: 
  
* Create release folder e.g. `mkdir release` 

* Go into that folder `cd release` 

* Run CMAKE to generate needed makefiles `cmake ..` 

* Build app using make `make` 

* Optionally update documentation throught `make doc` 

* Optionally setup manually the needed fuse bits using `avrdude`! 

* Flash controller with `make upload` 

* Optionally clean build with `make clean` 




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

```c

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
```
