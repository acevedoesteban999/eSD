#pragma once

#include <stdio.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <string.h>

// Definición de pines
#define SPI_MAX_FREC_KHZ 25000
#define MAX_BUFF_SD 256
#define BUFFER_SIZE 10 * MAX_BUFF_SD
#define SD_CORE 1
#define ESD_MOUNT_POINT "/sdcard"

#define ESD_DEFAULT_GPIO (esd_gpio) {\
    .MISO = GPIO_NUM_19, \
    .MOSI = GPIO_NUM_23, \
    .CS = GPIO_NUM_5, \
    .SCLK = GPIO_NUM_18, \
}


extern char SD_STR[3];
extern FILE* esd_file;

typedef struct{
    unsigned char MISO;
    unsigned char MOSI;
    unsigned char SCLK;
    unsigned char CS;
}esd_gpio;

typedef struct {
    float value;
    bool is_command;
} esd_data;

extern esd_gpio ESD_GPIO;

void esd_set_gpio(esd_gpio gpio);

// Prototipos de funciones
esp_err_t esd_init();

void esd_open(char *filename, char *type);

void esd_add_data(esd_data data);

int esd_check_trigger();

int esd_force_trigger();

int esd_float_to_str(float num, char *buffer, int offset);

void esd_append_multiple_to_file(char *filename, esd_data *data, size_t count);

void esd_close();

bool esd_delete_file(const char *filename);

void esd_write_str_without_open(char *buffer);

void esd_write_data_without_open(void * buffer, size_t size, size_t len);

void esd_fflush();

bool esd_has_error();

int esd_get_error();
