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

enum SD_PARAMETERS{
    TRIGGER_THRESHOLD,
    WARNING_THRESHOLD,
    TRIGGER_DRIFT,
    WARNING_DRIFT,
    INVALID_VALUE,
};

extern char SD_STR[3];

// Prototipos de funciones
esp_err_t init_sd_spi();

void append_multiple_to_file(uint64_t* data, size_t count);

void sd_add_data(uint64_t data);

bool sd_check_trigger();

int uint64_to_str(uint64_t num, char *buffer, int offset);

void sd_open(char *filename);

void sd_close();

void sd_write_without_open(char *buffer);

bool has_error_sd_spi();

int get_error_sd_spi();
