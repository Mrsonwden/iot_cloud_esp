// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SpiFlash_H_
#define _SpiFlash_H_

#include <stdbool.h>

#include "esp_err.h"
#include "rom/spi_flash.h"

/**
* @brief This class is used to emulate flash devices.
*
*/
class SpiFlash
{

public:
    SpiFlash();
    ~SpiFlash();

    void init(uint32_t chip_size, uint32_t block_size, uint32_t sector_size, uint32_t page_size, const char* partitions_bin);

    uint32_t get_chip_size();
    uint32_t get_block_size();
    uint32_t get_sector_size();
    uint32_t get_page_size();

    esp_rom_spiflash_result_t erase_block(uint32_t block);
    esp_rom_spiflash_result_t erase_sector(uint32_t sector);
    esp_rom_spiflash_result_t erase_page(uint32_t page);

    esp_rom_spiflash_result_t write(uint32_t dest_addr, const void *src, uint32_t size);
    esp_rom_spiflash_result_t read(uint32_t src_addr, void *dest, uint32_t size);

    uint32_t get_erase_cycles(uint32_t sector);
    uint32_t get_total_erase_cycles();
    
    void set_erase_cycles_limit(uint32_t limit);
    void set_total_erase_cycles_limit(uint32_t limit);

    void reset_erase_cycles();
    void reset_total_erase_cycles();

    uint8_t* get_memory_ptr(uint32_t src_address);

private:
    uint32_t chip_size;
    uint32_t block_size;
    uint32_t sector_size;
    uint32_t page_size;

    uint32_t blocks;
    uint32_t sectors;
    uint32_t pages;

    uint8_t* memory;

    bool* erase_states;

    uint32_t* erase_cycles;
    uint32_t erase_cycles_limit;
    uint32_t total_erase_cycles;
    uint32_t total_erase_cycles_limit;

    void deinit();
};

#endif // _SpiFlash_H_
