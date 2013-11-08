Monitor microcontroller's IO pins using ethernet interface.

The hardware:
STM32VL-Discovery
enc28j60 module

The lifelover's ethernet, tcp/ip library is used:
http://we.easyelectronics.ru/electro-and-pc/podklyuchenie-mikrokontrollera-k-lokalnoy-seti-testy-proizvoditelnosti-i-kratkoe-opisanie-api-steka.html

The usage:
This could be used for monitoring events that could be detected with
microcontroller IO, for example: is the door open, etc.
