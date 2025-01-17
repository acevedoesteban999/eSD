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
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5
#define SPI_MAX_FREC_KHZ 25000
#define MAX_BUFF_SD 256
#define BUFFER_SIZE 10 * MAX_BUFF_SD
#define SD_CORE 1
#define ESD_MOUNT_POINT "/sdcard"

extern char SD_STR[3];
extern FILE* esd_file;

typedef struct {
    float value;
    bool is_command;
} esd_data;

// Prototipos de funciones
esp_err_t esd_init();

void esd_open(char *filename, char *type);

void esd_add_data(esd_data data);

int esd_check_trigger();

int esd_force_trigger();

int esd_float_to_str(float num, char *buffer, int offset);

void esd_append_multiple_to_file(char *filename, esd_data *data, size_t count);


void esd_close();

void esd_write_str_without_open(char *buffer);

void esd_write_data_without_open(void * buffer, size_t size, size_t len);

void esd_fflush();

bool esd_has_error();

int esd_get_error();
