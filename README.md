# eSD

The eSD module is an implementation for handling SD card functionality on the ESP32. This module allows you to initialize the SD card, write data to it, and manage file operations.

## Features

- Initialize the SD card in SPI mode.
- Open and close files.
- Write data to files.
- Buffer data for efficient writing.
- Handle errors during SD card operations.

## Main Functions

- `esp_err_t esd_init()`: Initializes the SD card in SPI mode.
- `void esd_open(char *filename)`: Opens a file for writing.
- `void esd_close()`: Closes the currently open file.
- `void esd_write_without_open(char *buffer)`: Writes data to the currently open file without reopening it.
- `bool esd_has_error()`: Checks if there is an error with the SD card.
- `int esd_get_error()`: Returns the error code for the SD card.
- `void esd_add_data(uint64_t data)`: Adds data to the buffer for writing.
- `int esd_check_trigger()`: Checks if the buffer is full and triggers writing to the SD card if necessary.

## Example Usage

```c
#include "eSD.h"

void app_main() {
    // Initialize the SD card
    if (esd_init() != ESP_OK) {
        printf("Failed to initialize SD card\n");
        return;
    }

    // Open a file for writing
    esd_open("/sdcard/data.txt");

    // Add data to the buffer
    for (uint64_t i = 0; i < 100; i++) {
        esd_add_data(i);
    }

    // Check if the buffer needs to be written to the SD card
    esd_check_trigger();

    // Close the file
    esd_close();
}
```
