// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _DRIVER_I2S_H_
#define _DRIVER_I2S_H_
#include "esp_err.h"
#include <esp_types.h>
#include "soc/gpio_reg.h"
#include "soc/soc.h"
#include "soc/i2s_struct.h"
#include "soc/i2s_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/io_mux_reg.h"
#include "rom/gpio.h"
#include "esp_attr.h"
#include "esp_intr_alloc.h"
#include "driver/periph_ctrl.h"
#include "driver/adc.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2S bit width per sample.
 *
 */
typedef enum {
    I2S_BITS_PER_SAMPLE_8BIT    = 8,        /*!< I2S bits per sample: 8-bits*/
    I2S_BITS_PER_SAMPLE_16BIT   = 16,       /*!< I2S bits per sample: 16-bits*/
    I2S_BITS_PER_SAMPLE_24BIT   = 24,       /*!< I2S bits per sample: 24-bits*/
    I2S_BITS_PER_SAMPLE_32BIT   = 32,       /*!< I2S bits per sample: 32-bits*/
} i2s_bits_per_sample_t;

/**
 * @brief I2S channel.
 *
 */
typedef enum {
    I2S_CHANNEL_MONO        = 1,            /*!< I2S 1 channel (mono)*/
    I2S_CHANNEL_STEREO      = 2             /*!< I2S 2 channel (stereo)*/
} i2s_channel_t;

/**
 * @brief I2S communication standard format
 *
 */
typedef enum {
    I2S_COMM_FORMAT_I2S         = 0x01, /*!< I2S communication format I2S*/
    I2S_COMM_FORMAT_I2S_MSB     = 0x02, /*!< I2S format MSB*/
    I2S_COMM_FORMAT_I2S_LSB     = 0x04, /*!< I2S format LSB*/
    I2S_COMM_FORMAT_PCM         = 0x08, /*!< I2S communication format PCM*/
    I2S_COMM_FORMAT_PCM_SHORT   = 0x10, /*!< PCM Short*/
    I2S_COMM_FORMAT_PCM_LONG    = 0x20, /*!< PCM Long*/
} i2s_comm_format_t;


/**
 * @brief I2S channel format type
 */
typedef enum {
    I2S_CHANNEL_FMT_RIGHT_LEFT = 0x00,
    I2S_CHANNEL_FMT_ALL_RIGHT,
    I2S_CHANNEL_FMT_ALL_LEFT,
    I2S_CHANNEL_FMT_ONLY_RIGHT,
    I2S_CHANNEL_FMT_ONLY_LEFT,
} i2s_channel_fmt_t;

/**
 * @brief PDM sample rate ratio, measured in Hz.
 *
 */
typedef enum {
    PDM_SAMPLE_RATE_RATIO_64,
    PDM_SAMPLE_RATE_RATIO_128,
} pdm_sample_rate_ratio_t;

/**
 * @brief PDM PCM convter enable/disable.
 *
 */
typedef enum {
    PDM_PCM_CONV_ENABLE,
    PDM_PCM_CONV_DISABLE,
} pdm_pcm_conv_t;


/**
 * @brief I2S Peripheral, 0 & 1.
 *
 */
typedef enum {
    I2S_NUM_0 = 0x0,  /*!< I2S 0*/
    I2S_NUM_1 = 0x1,  /*!< I2S 1*/
    I2S_NUM_MAX,
} i2s_port_t;

/**
 * @brief I2S Mode, defaut is I2S_MODE_MASTER | I2S_MODE_TX
 *
 * @note PDM and built-in DAC functions are only supported on I2S0 for current ESP32 chip.
 *
 */
typedef enum {
    I2S_MODE_MASTER = 1,
    I2S_MODE_SLAVE = 2,
    I2S_MODE_TX = 4,
    I2S_MODE_RX = 8,
    I2S_MODE_DAC_BUILT_IN = 16,       /*!< Output I2S data to built-in DAC, no matter the data format is 16bit or 32 bit, the DAC module will only take the 8bits from MSB*/
    I2S_MODE_ADC_BUILT_IN = 32,       /*!< Input I2S data from built-in ADC, each data can be 12-bit width at most*/
    I2S_MODE_PDM = 64,
} i2s_mode_t;



/**
 * @brief I2S configuration parameters for i2s_param_config function
 *
 */
typedef struct {
    i2s_mode_t              mode;                   /*!< I2S work mode*/
    int                     sample_rate;            /*!< I2S sample rate*/
    i2s_bits_per_sample_t   bits_per_sample;        /*!< I2S bits per sample*/
    i2s_channel_fmt_t       channel_format;         /*!< I2S channel format */
    i2s_comm_format_t       communication_format;   /*!< I2S communication format */
    int                     intr_alloc_flags;       /*!< Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info */
    int                     dma_buf_count;          /*!< I2S DMA Buffer Count */
    int                     dma_buf_len;            /*!< I2S DMA Buffer Length */
    bool                    use_apll;              /*!< I2S using APLL as main I2S clock, enable it to get accurate clock */
    bool                    tx_desc_auto_clear;     /*!< I2S auto clear tx descriptor if there is underflow condition (helps in avoiding noise in case of data unavailability) */
    int                     fixed_mclk;             /*!< I2S using fixed MCLK output. If use_apll = true and fixed_mclk > 0, then the clock output for i2s is fixed and equal to the fixed_mclk value.*/
} i2s_config_t;

/**
 * @brief I2S event types
 *
 */
typedef enum {
    I2S_EVENT_DMA_ERROR,
    I2S_EVENT_TX_DONE,     /*!< I2S DMA finish sent 1 buffer*/
    I2S_EVENT_RX_DONE,     /*!< I2S DMA finish received 1 buffer*/
    I2S_EVENT_MAX,         /*!< I2S event max index*/
} i2s_event_type_t;

/**
 * @brief I2S DAC mode for i2s_set_dac_mode.
 *
 * @note PDM and built-in DAC functions are only supported on I2S0 for current ESP32 chip.
 */
typedef enum {
    I2S_DAC_CHANNEL_DISABLE  = 0,    /*!< Disable I2S built-in DAC signals*/
    I2S_DAC_CHANNEL_RIGHT_EN = 1,    /*!< Enable I2S built-in DAC right channel, maps to DAC channel 1 on GPIO25*/
    I2S_DAC_CHANNEL_LEFT_EN  = 2,    /*!< Enable I2S built-in DAC left  channel, maps to DAC channel 2 on GPIO26*/
    I2S_DAC_CHANNEL_BOTH_EN  = 0x3,  /*!< Enable both of the I2S built-in DAC channels.*/
    I2S_DAC_CHANNEL_MAX      = 0x4,  /*!< I2S built-in DAC mode max index*/
} i2s_dac_mode_t;

/**
 * @brief Event structure used in I2S event queue
 *
 */
typedef struct {
    i2s_event_type_t    type;   /*!< I2S event type */
    size_t              size;   /*!< I2S data size for I2S_DATA event*/
} i2s_event_t;

#define I2S_PIN_NO_CHANGE (-1) /*!< Use in i2s_pin_config_t for pins which should not be changed */

/**
 * @brief I2S pin number for i2s_set_pin
 *
 */
typedef struct {
    int bck_io_num;     /*!< BCK in out pin*/
    int ws_io_num;      /*!< WS in out pin*/
    int data_out_num;   /*!< DATA out pin*/
    int data_in_num;    /*!< DATA in pin*/
} i2s_pin_config_t;

/**
 * @brief I2S PDM RX downsample mode
 */
typedef enum {
    I2S_PDM_DSR_8S = 0,  /*!< downsampling number is 8 for PDM RX mode*/
    I2S_PDM_DSR_16S,     /*!< downsampling number is 16 for PDM RX mode*/
    I2S_PDM_DSR_MAX,
} i2s_pdm_dsr_t;

typedef intr_handle_t i2s_isr_handle_t;
/**
 * @brief Set I2S pin number
 *
 * @note
 * The I2S peripheral output signals can be connected to multiple GPIO pads.
 * However, the I2S peripheral input signal can only be connected to one GPIO pad.
 *
 * @param   i2s_num     I2S_NUM_0 or I2S_NUM_1
 *
 * @param   pin         I2S Pin structure, or NULL to set 2-channel 8-bit internal DAC pin configuration (GPIO25 & GPIO26)
 *
 * Inside the pin configuration structure, set I2S_PIN_NO_CHANGE for any pin where
 * the current configuration should not be changed.
 *
 * @note if *pin is set as NULL, this function will initialize both of the built-in DAC channels by default.
 *       if you don't want this to happen and you want to initialize only one of the DAC channels, you can call i2s_set_dac_mode instead.
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_FAIL            IO error
 */
esp_err_t i2s_set_pin(i2s_port_t i2s_num, const i2s_pin_config_t *pin);

/**
 * @brief Set PDM mode down-sample rate
 *        In PDM RX mode, there would be 2 rounds of downsample process in hardware.
 *        In the first downsample process, the sampling number can be 16 or 8.
 *        In the second downsample process, the sampling number is fixed as 8.
 *        So the clock frequency in PDM RX mode would be (fpcm * 64) or (fpcm * 128) accordingly.
 * @param i2s_num I2S_NUM_0, I2S_NUM_1
 * @param dsr i2s RX down sample rate for PDM mode.
 *
 * @note After calling this function, it would call i2s_set_clk inside to update the clock frequency.
 *       Please call this function after I2S driver has been initialized.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_set_pdm_rx_down_sample(i2s_port_t i2s_num, i2s_pdm_dsr_t dsr);

/**
 * @brief Set I2S dac mode, I2S built-in DAC is disabled by default
 *
 * @param dac_mode DAC mode configurations - see i2s_dac_mode_t
 *
 * @note Built-in DAC functions are only supported on I2S0 for current ESP32 chip.
 *       If either of the built-in DAC channel are enabled, the other one can not
 *       be used as RTC DAC function at the same time.
 *
 * @return
 *     - ESP_OK               Success
 *     - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t i2s_set_dac_mode(i2s_dac_mode_t dac_mode);

/**
 * @brief Install and start I2S driver.
 *
 * @param i2s_num         I2S_NUM_0, I2S_NUM_1
 *
 * @param i2s_config      I2S configurations - see i2s_config_t struct
 *
 * @param queue_size      I2S event queue size/depth.
 *
 * @param i2s_queue       I2S event queue handle, if set NULL, driver will not use an event queue.
 *
 * This function must be called before any I2S driver read/write operations.
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_driver_install(i2s_port_t i2s_num, const i2s_config_t *i2s_config, int queue_size, void* i2s_queue);

/**
 * @brief Uninstall I2S driver.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_driver_uninstall(i2s_port_t i2s_num);

/**
 * @brief Write data to I2S DMA transmit buffer.
 *
 * This function is deprecated. Use 'i2s_write' instead.
 * This definition will be removed in a future release.
 *
 * @return
 *     - The amount of bytes written, if timeout, the result will be less than the size passed in.
 *     - ESP_FAIL  Parameter error
 */
int i2s_write_bytes(i2s_port_t i2s_num, const void *src, size_t size, TickType_t ticks_to_wait) __attribute__ ((deprecated));

/**
 * @brief Write data to I2S DMA transmit buffer.
 *
 * @param i2s_num             I2S_NUM_0, I2S_NUM_1
 *
 * @param src                 Source address to write from
 *
 * @param size                Size of data in bytes
 *
 * @param[out] bytes_written  Number of bytes written, if timeout, the result will be less than the size passed in.
 *
 * @param ticks_to_wait       TX buffer wait timeout in RTOS ticks. If this
 * many ticks pass without space becoming available in the DMA
 * transmit buffer, then the function will return (note that if the
 * data is written to the DMA buffer in pieces, the overall operation
 * may still take longer than this timeout.) Pass portMAX_DELAY for no
 * timeout.
 *
 * @return
 *     - ESP_OK               Success
 *     - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t i2s_write(i2s_port_t i2s_num, const void *src, size_t size, size_t *bytes_written, TickType_t ticks_to_wait);

/**
 * @brief Write data to I2S DMA transmit buffer while expanding the number of bits per sample. For example, expanding 16-bit PCM to 32-bit PCM.
 *
 * @param i2s_num             I2S_NUM_0, I2S_NUM_1
 *
 * @param src                 Source address to write from
 *
 * @param size                Size of data in bytes
 *
 * @param src_bits            Source audio bit
 *
 * @param aim_bits            Bit wanted, no more than 32, and must be greater than src_bits
 *
 * @param[out] bytes_written  Number of bytes written, if timeout, the result will be less than the size passed in.
 *
 * @param ticks_to_wait       TX buffer wait timeout in RTOS ticks. If this
 * many ticks pass without space becoming available in the DMA
 * transmit buffer, then the function will return (note that if the
 * data is written to the DMA buffer in pieces, the overall operation
 * may still take longer than this timeout.) Pass portMAX_DELAY for no
 * timeout.
 *
 * Format of the data in source buffer is determined by the I2S
 * configuration (see i2s_config_t).
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_write_expand(i2s_port_t i2s_num, const void *src, size_t size, size_t src_bits, size_t aim_bits, size_t *bytes_written, TickType_t ticks_to_wait);

/**
 * @brief Read data from I2S DMA receive buffer
 *
 * This function is deprecated. Use 'i2s_read' instead.
 * This definition will be removed in a future release.
 *
 * @return
 *     - The amount of bytes read, if timeout, bytes read will be less than the size passed in
 *     - ESP_FAIL  Parameter error
 */
int i2s_read_bytes(i2s_port_t i2s_num, void *dest, size_t size, TickType_t ticks_to_wait) __attribute__ ((deprecated));

/**
 * @brief Read data from I2S DMA receive buffer
 *
 * @param i2s_num         I2S_NUM_0, I2S_NUM_1
 *
 * @param dest            Destination address to read into
 *
 * @param size            Size of data in bytes
 *
 * @param[out] bytes_read Number of bytes read, if timeout, bytes read will be less than the size passed in.
 *
 * @param ticks_to_wait   RX buffer wait timeout in RTOS ticks. If this many ticks pass without bytes becoming available in the DMA receive buffer, then the function will return (note that if data is read from the DMA buffer in pieces, the overall operation may still take longer than this timeout.) Pass portMAX_DELAY for no timeout.
 *
 * @note If the built-in ADC mode is enabled, we should call i2s_adc_start and i2s_adc_stop around the whole reading process,
 *       to prevent the data getting corrupted.
 *
 * @return
 *     - ESP_OK               Success
 *     - ESP_ERR_INVALID_ARG  Parameter error
 */
esp_err_t i2s_read(i2s_port_t i2s_num, void *dest, size_t size, size_t *bytes_read, TickType_t ticks_to_wait);

/**
 * @brief Write a single sample to the I2S DMA TX buffer.
 *
 * This function is deprecated. Use 'i2s_write' instead.
 * This definition will be removed in a future release.
 *
 * @param i2s_num         I2S_NUM_0, I2S_NUM_1
 *
 * @param sample          Buffer to read data. Size of buffer (in bytes) = bits_per_sample / 8.
 *
 * @param ticks_to_wait   Timeout in RTOS ticks. If a sample is not available in the DMA buffer within this period, no data is read and function returns zero.
 *
 * @return
 *     - Number of bytes successfully pushed to DMA buffer, will be either zero or the size of configured sample buffer (in bytes).
 *     - ESP_FAIL Parameter error
 */
int i2s_push_sample(i2s_port_t i2s_num, const void *sample, TickType_t ticks_to_wait) __attribute__ ((deprecated));

/**
 * @brief Read a single sample from the I2S DMA RX buffer.
 *
 * This function is deprecated. Use 'i2s_read' instead.
 * This definition will be removed in a future release.
 *
 * @param i2s_num         I2S_NUM_0, I2S_NUM_1
 *
 * @param sample          Buffer to write data. Size of buffer (in bytes) = bits_per_sample / 8.
 *
 * @param ticks_to_wait   Timeout in RTOS ticks. If a sample is not available in the DMA buffer within this period, no data is read and function returns zero.
 *
 * @return
 *     - Number of bytes successfully read from DMA buffer, will be either zero or the size of configured sample buffer (in bytes).
 *     - ESP_FAIL Parameter error
 */
int i2s_pop_sample(i2s_port_t i2s_num, void *sample, TickType_t ticks_to_wait) __attribute__ ((deprecated));

/**
 * @brief Set sample rate used for I2S RX and TX.
 *
 * The bit clock rate is determined by the sample rate and i2s_config_t configuration parameters (number of channels, bits_per_sample).
 *
 * `bit_clock = rate * (number of channels) * bits_per_sample`
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param rate I2S sample rate (ex: 8000, 44100...)
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_set_sample_rates(i2s_port_t i2s_num, uint32_t rate);

/**
 * @brief Stop I2S driver
 *
 * Disables I2S TX/RX, until i2s_start() is called.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_stop(i2s_port_t i2s_num);

/**
 * @brief Start I2S driver
 *
 * It is not necessary to call this function after i2s_driver_install() (it is started automatically), however it is necessary to call it after i2s_stop().
 *
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
* @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_start(i2s_port_t i2s_num);

/**
 * @brief Zero the contents of the TX DMA buffer.
 *
 * Pushes zero-byte samples into the TX DMA buffer, until it is full.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_zero_dma_buffer(i2s_port_t i2s_num);

/**
 * @brief Set clock & bit width used for I2S RX and TX.
 *
 * Similar to i2s_set_sample_rates(), but also sets bit width.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @param rate I2S sample rate (ex: 8000, 44100...)
 *
 * @param bits I2S bit width (I2S_BITS_PER_SAMPLE_16BIT, I2S_BITS_PER_SAMPLE_24BIT, I2S_BITS_PER_SAMPLE_32BIT)
 *
 * @param ch I2S channel, (I2S_CHANNEL_MONO, I2S_CHANNEL_STEREO)
 *
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 *     - ESP_ERR_NO_MEM      Out of memory
 */
esp_err_t i2s_set_clk(i2s_port_t i2s_num, uint32_t rate, i2s_bits_per_sample_t bits, i2s_channel_t ch);

/**
 * @brief get clock set on particular port number.
 *
 * @param i2s_num  I2S_NUM_0, I2S_NUM_1
 *
 * @return
 *     - actual clock set by i2s driver
 */
float i2s_get_clk(i2s_port_t i2s_num);

/**
 * @brief Set built-in ADC mode for I2S DMA, this function will initialize ADC pad,
 *        and set ADC parameters.
 * @param adc_unit    SAR ADC unit index
 * @param adc_channel ADC channel index
 * @return
 *     - ESP_OK              Success
 *     - ESP_ERR_INVALID_ARG Parameter error
 */
esp_err_t i2s_set_adc_mode(adc_unit_t adc_unit, adc1_channel_t adc_channel);

/**
 * @brief Start to use I2S built-in ADC mode
 * @note This function would acquire the lock of ADC to prevent the data getting corrupted
 *       during the I2S peripheral is being used to do fully continuous ADC sampling.
 *
 * @param i2s_num i2s port index
 * @return
 *     - ESP_OK                Success
 *     - ESP_ERR_INVALID_ARG   Parameter error
 *     - ESP_ERR_INVALID_STATE Driver state error
 */
esp_err_t i2s_adc_enable(i2s_port_t i2s_num);

/**
 * @brief Stop to use I2S built-in ADC mode
 * @param i2s_num i2s port index
 * @note This function would release the lock of ADC so that other tasks can use ADC.
 * @return
 *     - ESP_OK                 Success
 *     - ESP_ERR_INVALID_ARG    Parameter error
 *     - ESP_ERR_INVALID_STATE  Driver state error
 */
esp_err_t i2s_adc_disable(i2s_port_t i2s_num);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_I2S_H_ */
