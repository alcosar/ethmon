#include "stm32f10x.h"
#include "httpd.h"
#include "main.h"

httpd_state_t httpd_pool[TCP_MAX_CONNECTIONS];
uint8_t led_state;

// HTML header with HTML page
const char index_page[] =
	"HTTP/1.0 200 OK\r\n"
	"Content-Type: text/plain\r\n"
	"Connection: close\r\n\r\n"
	"Channel 0: ?\r\n"
	"Channel 1: ?\r\n"
	"Channel 2: ?\r\n"
	"Channel 3: ?\r\n"
	"Channel 4: ?\r\n"
	"Channel 5: ?\r\n"
	"Channel 6: ?\r\n"
	"Channel 7: ?\r\n";

#define NUM_MONINITORING_CH 8

// HTML error page
const char error_page[] =
	"HTTP/1.0 404 Not Found\r\n"
	"Content-Type: text/html\r\n"
	"Connection: close\r\n\r\n"
	"<h1>404 - Not Found</h1>";

char tcp_send_buf[sizeof(index_page)];

const prog_char p_pre[] = "<pre>";
const prog_char p_pre_end[] = "</pre>";
const prog_char p_led_off[] = "Led is OFF. Turn <a href='/?led=on'>on</a>.";
const prog_char p_led_on[] =  "Led is ON.Turn <a href='/?led=off'>off</a>.";

// Принимаем подключения на порт 80
uint8_t tcp_listen(uint8_t id, eth_frame_t *frame)
{
	(void) id;

	ip_packet_t *ip = (void*)(frame->data);
	tcp_packet_t *tcp = (void*)(ip->data);

	return (tcp->to_port == htons(80));
}

static inline void fill_monitoring_status(char *buf)
{
	uint8_t status = GPIOA->IDR >> 8;
	int i = NUM_MONINITORING_CH;
	char *p;

	while (i--) {
		p = strchr(buf, '?');
		if (!p) /* TODO */
			while (1);
		*p = (status & 1) + '0';
		status >>= 1;
		buf = p + 1;
	}
}

void tcp_write(uint8_t id, eth_frame_t *frame, uint16_t len)
{
	(void) len;

	httpd_state_t *st = httpd_pool + id;
	ip_packet_t *ip = (ip_packet_t *) frame->data;
	tcp_packet_t *tcp = (tcp_packet_t*) ip->data;
	char *request = (char *) tcp_get_data(tcp);
	char *url;
	char *p, *params;

	if (!st->data) {
		url = request + 4;
		p = strchr(url, ' ');
		if (!memcmp(request, "GET ", 4) && p) {
			*p = 0;

			params = strchr(url, '?');
			if (params)
				*(params++) = 0;

			if (!strcmp(url, "/"))
			{
				memcpy(tcp_send_buf, index_page, sizeof(index_page));
				fill_monitoring_status(tcp_send_buf);

				st->data = tcp_send_buf;
				st->numbytes = sizeof(tcp_send_buf) - 1;
			} else {
				st->data = error_page;
				st->numbytes = sizeof(error_page) - 1;
			}
		}
	}
}

// Отправка данных
void tcp_read(uint8_t id, eth_frame_t *frame, uint8_t re)
{
	httpd_state_t *st = httpd_pool + id;
	ip_packet_t *ip = (void*)(frame->data);
	tcp_packet_t *tcp = (void*)(ip->data);
	char *buf = (void*)(tcp->data);
	uint8_t i, options;
	uint16_t blocksize;

	(void) re;
	// Идёт отправка данных?
	if(st->numbytes)
	{
		// Отправляем пучок пакетов, до 4 штучек за раз
		for(i = 4; i; --i)
		{
			// До 512 байт в 1 пакете
			blocksize = 512;

			options = 0;

			// Если данных осталось <= 512 байт,
			//    отправляем сколько осталось 
			//    и закрываем соединение
			if(st->numbytes <= blocksize)
			{
				options = TCP_OPTION_CLOSE;
				blocksize = st->numbytes;
			}

			// В посдеднем пакете пучка 
			//    выставляем флаг PSH
			else if(i == 1)
			{
				options = TCP_OPTION_PUSH;
			}

			// Заполняем пакет данными
			memcpy(buf, st->data, blocksize);
			st->data += blocksize;
			st->numbytes -= blocksize;

			// И отправляем
			tcp_send(id, frame, blocksize, options);

			// Если данных больше нету, выходим из цикла
			if(options == TCP_OPTION_CLOSE)
				break;
		}
	}
}

// Соединение закрыто/оборвалось
void tcp_closed(uint8_t id, uint8_t hard)
{
	(void) hard;
	httpd_state_t *st = httpd_pool + id;
	st->data = 0;
	st->numbytes = 0;
}
