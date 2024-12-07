#include "eSD.h"

int error_sd_spi = 0;
char filename[40] = "";
TaskHandle_t task_sd_spi_handle = NULL;
uint64_t buff_sd[MAX_BUFF_SD + 5]; 
uint64_t buff_sd_task[MAX_BUFF_SD + 5]; 
int count_buff_sd = 0;
int count_buff_sd_task = 0;
FILE* f;

esp_err_t init_sd_spi() {
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
        // printf("Error inicializando el bus SPI: %d\n", ret);
        lcd_print_string_at(18,3,"E1");
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
            //printf("Error: Falló montar el sistema de archivos.\n");
            lcd_print_string_at(18,3,"E2");
            error_sd_spi = 2;
        } else {
            // printf("Error: Falló inicializar la tarjeta SD (%s).\n", esp_err_to_name(ret));
            lcd_print_string_at(18,3,"E3");
            error_sd_spi = 3;
        }
        return ret;
    }
    //sdmmc_card_print_info(stdout, card);
    rtc_data data = rtc_read();
    snprintf(BufferRtcI2C, 25, "%02d-%02d-%02d_%02d-%02d-%02d", data.year,data.month, data.day_of_month, data.hours, data.minutes, data.seconds);
    strcpy(filename,mount_point);
    strcat(filename,"/");
    strcat(filename, BufferRtcI2C);
    strcat(filename, ".txt");


    char buffer[50];
    f = fopen(filename, "a");
    if (f == NULL) {
        lcd_print_string_at(18, 3, "E4");
        error_sd_spi = 4;
        return ESP_FAIL;
    }
    
    sprintf(buffer, "---------------------------------------\n");      
    fwrite(buffer, sizeof(char),strlen(buffer), f);
    sprintf(buffer, "[DATE]\n%02d-%02d-%02d %02d:%02d:%02d\n",data.year,data.month, data.day_of_month, data.hours, data.minutes, data.seconds);      
    fwrite(buffer, sizeof(char), strlen(buffer), f);
    sprintf(buffer, "---------------------------------------\n");      
    fwrite(buffer, sizeof(char),strlen(buffer), f);
    sprintf(buffer, "[TRIGGER THRESHOLD]\n%05f => %llu\n",40e6/MAIN_PARAMETRERS.threshold , MAIN_PARAMETRERS.threshold);      
    fwrite(buffer, sizeof(char), strlen(buffer), f);
    sprintf(buffer, "---------------------------------------\n");      
    fwrite(buffer, sizeof(char),strlen(buffer), f);
    sprintf(buffer, "[DRIFT THRESHOLD]\n%05f\n", MAIN_PARAMETRERS.drift);      
    fwrite(buffer, sizeof(char), strlen(buffer), f);
    sprintf(buffer, "---------------------------------------\n");      
    fwrite(buffer, sizeof(char),strlen(buffer), f);
    sprintf(buffer, "[TRIGGER THRESHOLD TIME]\n%05f => %llu\n", 40e6/MAIN_PARAMETRERS.threshold_time,MAIN_PARAMETRERS.threshold_time);      
    fwrite(buffer, sizeof(char), strlen(buffer), f);
    sprintf(buffer, "---------------------------------------\n");      
    fwrite(buffer, sizeof(char),strlen(buffer), f);
    sprintf(buffer, "[TRIGGER DRIFT TIME]\n%05f => %llu\n", 40e6/MAIN_PARAMETRERS.drift_time,MAIN_PARAMETRERS.drift_time);      
    fwrite(buffer, sizeof(char), strlen(buffer), f);
    sprintf(buffer, "---------------------------------------\n");      
    fwrite(buffer, sizeof(char),strlen(buffer), f);
    sprintf(buffer, "[DISPLAY TIME]\n%05f => %llu\n", 40e6/MAIN_PARAMETRERS.display_time,MAIN_PARAMETRERS.display_time);      
    fwrite(buffer, sizeof(char), strlen(buffer), f);
    sprintf(buffer, "---------------------------------------\n[DATA]\n");      
    fwrite(buffer, sizeof(char),strlen(buffer), f);
    fclose(f);
    lcd_print_string_at(18,3,"SD");
    error_sd_spi = 0;
    return ESP_OK;
}

bool has_error_sd_spi(){
    return error_sd_spi != 0;
}

int uint64_to_str(uint64_t num, char* buffer,int offset) {
    char*p = buffer;
    if (num == 0) {
        *(p + offset) = '0';
        p++;
        return 1;
    }

    char temp[6]; 
    int ii = 0;
    while (num > 0) {
        temp[ii++] = (num % 10) + '0'; 
        num /= 10; 
    }
    int strlen = ii;
    while (ii > 0) {
        *(p + offset) = temp[--ii];
        p++;
    }
    return strlen;
}

void append_multiple_to_file(uint64_t* data, size_t count) {
    
    f = fopen(filename, "a");
    if (f == NULL) {
        lcd_print_string_at(18, 3, "E4");
        error_sd_spi = 4;
        return;
    }

    char buffer[BUFFER_SIZE];
    uint64_t code_accum = 0;
    int count_buffer = 0;

    for (size_t i = 0; i < count; i++) {
        if (data[i] < 10)
            code_accum = code_accum * 10 + data[i];
        else {
            count_buffer += uint64_to_str(data[i], &buffer,count_buffer );
            if (code_accum){
                buffer[count_buffer++] = ' ';
                count_buffer += uint64_to_str(code_accum,buffer,count_buffer);
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


void task_append_to_multiple_to_file(void* arg){
    append_multiple_to_file(&buff_sd_task,count_buff_sd_task);
    vTaskDelete(NULL);
}

void add_data_sd_spi(uint64_t data) {
    if (!has_error_sd_spi()) {
        buff_sd[count_buff_sd++] = data;
    }
}

void check_data_sd_spi(){
    if (!has_error_sd_spi()) {
        if (count_buff_sd >= MAX_BUFF_SD) {
            if(task_sd_spi_handle != NULL)
                while (eTaskGetState(task_sd_spi_handle) == eRunning)
                    vTaskDelay(pdMS_TO_TICKS(1));
            memcpy(buff_sd_task, buff_sd, count_buff_sd * sizeof(uint64_t));
            count_buff_sd_task = count_buff_sd;
            xTaskCreatePinnedToCore(task_append_to_multiple_to_file, "taskAppMult2", 5000, NULL, 15, &task_sd_spi_handle,1);
            count_buff_sd = 0;
        }
    }
}
