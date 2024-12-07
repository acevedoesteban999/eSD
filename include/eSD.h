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

// Prototipos de funciones
esp_err_t init_sd_spi();
void write_data_to_sd(const char* filename, const char* data);
void append_to_file(const char* filename, const char* data);
void append_multiple_to_file(uint64_t* data, size_t count);

void append_multiple_to_file1(uint64_t *data, size_t count);

int uint64_to_str(uint64_t num, char *buffer, int offset);

bool has_error_sd_spi();

void add_data_sd_spi(uint64_t data);

void check_data_sd_spi();

void task_append_to_multiple_to_file(void *arg);

void append_multiple_to_file2(uint64_t *data, size_t count);

void append_multiple_to_file3(uint64_t *data, size_t count);

int uint64_to_str1(uint64_t num, char *buffer);
