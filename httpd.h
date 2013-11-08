#pragma once

#include <stdlib.h>
#include <string.h>
#include "lan.h"

typedef char prog_char;

#define HTTPD_PORT			htons(80)
#define HTTPD_PACKET_BULK	10
#define HTTPD_MAX_BLOCK		512
#define HTTPD_NAME			"ATmega32"
#define HTTPD_INDEX_FILE	"/index.htm"

typedef enum httpd_state_code {
	HTTPD_CLOSED,
	HTTPD_INIT,
	HTTPD_READ_HEADER,
	HTTPD_WRITE_DATA
} httpd_state_code_t;

// Пул соединений
typedef struct httpd_state {
    const prog_char *data;
    uint16_t numbytes;
} __attribute__ ((__packed__)) httpd_state_t;
