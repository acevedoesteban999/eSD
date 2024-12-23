#include "eSD.h"

int error_sd_spi = -1;
char SD_STR[3] = "E?";
char *FILENAME;
TaskHandle_t task_sd_spi_handle = NULL;
uint64_t SD_BUFFER[MAX_BUFF_SD + 5]; 
uint64_t SD_BUFFER_COPY[MAX_BUFF_SD + 5]; 
int count_buff_sd = 0;
int count_buff_sd_cpy = 0;
FILE* f;

esp_err_t esd_init() {
    // Definir los pines SPI
    esp_err_t ret;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = SPI_MAX_FREC_KHZ;

    // Configurar el bus SPI para la tarjeta SD
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,

    };

    // Inicializar el bus SPI
    ret = spi_bus_initialize(host.slot, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        strcpy(SD_STR,"E1");
        error_sd_spi = 1;
        return ret;
    }

    // Configurar el dispositivo SPI (microSD)
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS; // Pin CS para microSD
    slot_config.host_id = host.slot;
    
    // Montar el sistema de archivos FAT
    const char mount_point[] = "/sdcard";
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t* card;
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            strcpy(SD_STR,"E2");
            error_sd_spi = 2;
        } else {
            strcpy(SD_STR,"E3");
            error_sd_spi = 3;
        }
        return ret;
    }

    // sdmmc_card_print_info(stdout, card);
    
    strcpy(SD_STR,"SD");
    error_sd_spi = 0;
    return ESP_OK;
}

//Open and Set Filename 
void esd_open(char*filename){

    f = fopen(filename, "a");
    if (f == NULL) {
        error_sd_spi = 4;
    }
    FILENAME = filename;
}

void esd_close(){
    fclose(f);
}

void esd_write_without_open(char*buffer){
    if(!esd_has_error())
        fwrite(buffer, sizeof(char),strlen(buffer), f);
}

bool esd_has_error(){
    return error_sd_spi != 0;
}

int esd_get_error(){
    return error_sd_spi;
}

int esd_uint64_to_str(uint64_t num, char* buffer, int offset) {
    if (buffer == NULL) {
        return -1;
    }

    char* p = buffer + offset;
    if (num == 0) {
        *p = '0';
        return 1;
    }

    char temp[20];
    int ii = 0;
    while (num > 0) {
        temp[ii++] = (num % 10) + '0';
        num /= 10;
    }

    int strlen = ii;
    while (ii > 0) {
        *p++ = temp[--ii];
    }
    return strlen;
}

void esd_append_multiple_to_file(char*filename,uint64_t* data, size_t count) {
    
    f = fopen(filename, "a");
    if (f == NULL) {
        error_sd_spi = 4;
        strcpy(SD_STR,"E4");
        return;
    }

    char buffer[BUFFER_SIZE];
    uint64_t code_accum = 0;
    int count_buffer = 0;

    for (size_t i = 0; i < count; i++) {
        if (data[i] < 10)
            code_accum = code_accum * 10 + data[i];
        else {
            count_buffer += esd_uint64_to_str(data[i], &buffer,count_buffer );
            if (code_accum){
                buffer[count_buffer++] = ' ';
                count_buffer += esd_uint64_to_str(code_accum,buffer,count_buffer);
                buffer[count_buffer++] = '\n';
                code_accum = 0;
            }
            else
                buffer[count_buffer++] = '\n';
        }
    }
    if (count_buffer > 0) {
        size_t aa=  fwrite(buffer, sizeof(char), count_buffer, f);
    }
    // sprintf(buffer, "---------------------------------------\n");      
    // fwrite(buffer, sizeof(char),strlen(buffer), f);
    fclose(f);
}

void esd_add_data(uint64_t data) {
    if (esd_has_error()) 
        return;
    
    if(count_buff_sd < MAX_BUFF_SD){
        SD_BUFFER[count_buff_sd++] = data;

    }else{
        esd_check_trigger();
        esd_add_data(data);
    }
}

void _task_trigger_sd(void* arg){
    esd_append_multiple_to_file(FILENAME,&SD_BUFFER_COPY,count_buff_sd_cpy);
    vTaskDelete(NULL);
}


/*
RETURN:
1 ok 
0 no
-1 error
*/
int esd_check_trigger(){
    if (esd_has_error()) {
        return -1;
    }
    
    if (count_buff_sd >= MAX_BUFF_SD) {
        if(task_sd_spi_handle != NULL)
            while (eTaskGetState(task_sd_spi_handle) == eRunning)
                vTaskDelay(pdMS_TO_TICKS(1));
        memcpy(SD_BUFFER_COPY, SD_BUFFER, count_buff_sd * sizeof(uint64_t));
        
        count_buff_sd_cpy = count_buff_sd;
        count_buff_sd = 0;
        xTaskCreatePinnedToCore(_task_trigger_sd, "xtask_sd", 5000, NULL, 15, &task_sd_spi_handle,1);
        return 1;
    }
    return 0;
    
}