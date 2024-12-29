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
#define MOUNT_POINT "/sdcard"

extern char SD_STR[3];

// Prototipos de funciones
esp_err_t esd_init();

void esd_add_data(uint64_t data);

int esd_check_trigger();

int esd_force_trigger();

int esd_uint64_to_str(uint64_t num, char *buffer, int offset);

void esd_append_multiple_to_file(char *filename, uint64_t *data, size_t count);

void esd_open(char *filename);

void esd_close();

void esd_write_without_open(char *buffer);

bool esd_has_error();

int esd_get_error();
