// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------------- //
// sport_uart_tx //
// ------------- //

#define sport_uart_tx_wrap_target 0
#define sport_uart_tx_wrap 6

static const uint16_t sport_uart_tx_program_instructions[] = {
            //     .wrap_target
    0x9fa0, //  0: pull   block           side 1 [7] 
    0xf727, //  1: set    x, 7            side 0 [7] 
    0x6001, //  2: out    pins, 1                    
    0x0642, //  3: jmp    x--, 2                 [6] 
    0x1f80, //  4: jmp    y--, 0          side 1 [7] 
    0xc000, //  5: irq    nowait 0                   
    0x0000, //  6: jmp    0                          
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program sport_uart_tx_program = {
    .instructions = sport_uart_tx_program_instructions,
    .length = 7,
    .origin = -1,
};

static inline pio_sm_config sport_uart_tx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + sport_uart_tx_wrap_target, offset + sport_uart_tx_wrap);
    sm_config_set_sideset(&c, 2, true, false);
    return c;
}

#include "hardware/clocks.h"
// here the code from elrsoXs for Tx
static inline void sport_uart_tx_program_init(PIO pio, uint sm, uint offset, uint pin_tx, uint baud, bool invert) {
    // Tell PIO to initially drive output-high on the selected pin, then map PIO
    // onto that pin with the IO muxes.
    //pio_sm_set_pins_with_mask(pio, sm, 1u << pin_tx, 1u << pin_tx);
    //pio_sm_set_pindirs_with_mask(pio, sm, 1u << pin_tx, 1u << pin_tx);
    pio_gpio_init(pio, pin_tx);
    if ( invert ) gpio_set_outover(pin_tx,  GPIO_OVERRIDE_INVERT) ; // added by ms to manage inverted UART from Sport
    pio_sm_config c = sport_uart_tx_program_get_default_config(offset);
    // OUT shifts to right, no autopull
    sm_config_set_out_shift(&c, true, false, 32);
    // We are mapping both OUT and side-set to the same pin, because sometimes
    // we need to assert user data onto the pin (with OUT) and sometimes
    // assert constant values (start/stop bit)
    sm_config_set_out_pins(&c, pin_tx, 1);
    sm_config_set_sideset_pins(&c, pin_tx);
    // We only need TX, so get an 8-deep FIFO!
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * baud);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_set_irq0_source_enabled( pio ,pis_interrupt0 , true ); // generate an interrupt pio_irq0 if pio has irq bit 0 set
    //    pio_sm_set_enabled(pio, sm, true); // sm will be enable only on request
}
static inline void sport_uart_tx_program_stop(PIO pio, uint sm, uint pin_tx) {
    pio_sm_set_enabled(pio, sm, false); // disabled
    pio_sm_set_pindirs_with_mask(pio, sm, 0 , 1u << pin_tx); // put pin Tx as input
    // normally other parameters are already configured by init
}
static inline void sport_uart_tx_program_start(PIO pio, uint sm, uint pin_tx , bool invert ) {
    pio_sm_set_pins_with_mask(pio, sm, 1u << pin_tx, 1u << pin_tx);  // put pin TX on  high level
    pio_sm_set_pindirs_with_mask(pio, sm, 1u << pin_tx, 1u << pin_tx); // put pin Tx as output
    if (invert) gpio_set_outover(pin_tx,  GPIO_OVERRIDE_INVERT) ; // added by ms to manage inverted UART from Sport
    // normally other parameters are already configured by init
    pio_sm_restart (pio, sm); // to test if we need an enable after this
    pio_sm_set_enabled(pio, sm, true);
}
static inline void sport_uart_tx_program_set_y(PIO pio, uint sm, uint8_t len){  // load pio reg Y with nbr of bytes
    pio_sm_exec (pio, sm, pio_encode_set(pio_y, len));
    //pio_sm_exec (pio, sm,  pio_encode_jmp(0));  // force to execute from 0
}
static inline void sport_uart_tx_program_putc(PIO pio, uint sm, char c) {
    pio_sm_put_blocking(pio, sm, (uint32_t)c);
}
static inline void sport_uart_tx_program_puts(PIO pio, uint sm, const char *s) {
    while (*s)
        sport_uart_tx_program_putc(pio, sm, *s++);
}

#endif

// ------------- //
// sport_uart_rx //
// ------------- //

#define sport_uart_rx_wrap_target 0
#define sport_uart_rx_wrap 8

static const uint16_t sport_uart_rx_program_instructions[] = {
            //     .wrap_target
    0x2020, //  0: wait   0 pin, 0                   
    0xea27, //  1: set    x, 7                   [10]
    0x4001, //  2: in     pins, 1                    
    0x0642, //  3: jmp    x--, 2                 [6] 
    0x00c8, //  4: jmp    pin, 8                     
    0xc014, //  5: irq    nowait 4 rel               
    0x20a0, //  6: wait   1 pin, 0                   
    0x0000, //  7: jmp    0                          
    0x8020, //  8: push   block                      
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program sport_uart_rx_program = {
    .instructions = sport_uart_rx_program_instructions,
    .length = 9,
    .origin = -1,
};

static inline pio_sm_config sport_uart_rx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + sport_uart_rx_wrap_target, offset + sport_uart_rx_wrap);
    return c;
}

#include "hardware/gpio.h"
#include "hardware/clocks.h"
static inline void sport_uart_rx_program_init(PIO pio, uint sm, uint offset, uint pin_rx, uint baud , bool invert) {
    //pio_sm_set_pins_with_mask(pio, sm, 1u << pin_tx, 1u << pin_tx); // was in Tx program but is normally not required for Rx
    pio_sm_set_pindirs_with_mask(pio, sm, 0 , 1u << pin_rx); // set pin as input
    //pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false); // remove by ms
    pio_gpio_init(pio, pin_rx);
    if (invert) {
        gpio_set_inover(pin_rx,  GPIO_OVERRIDE_INVERT) ; // added by ms to manage inverted UART from Sport
        gpio_pull_down(pin_rx); // changed by MS
    } else {
        gpio_pull_up(pin_rx); // changed by MS
    }    
    pio_sm_config c = sport_uart_rx_program_get_default_config(offset);
    sm_config_set_in_pins(&c, pin_rx); // for WAIT, IN
    sm_config_set_jmp_pin(&c, pin_rx); // for JMP
    // Shift to right, autopush disabled
    sm_config_set_in_shift(&c, true, false, 32);
    // Deeper FIFO as we're not doing any TX
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    // SM transmits 1 bit per 8 execution cycles.
    float div = (float)clock_get_hz(clk_sys) / (8 * baud);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_set_irq1_source_enabled(pio ,  pis_sm1_rx_fifo_not_empty , true ); // added by ms to get an irq int0 from pio
    pio_sm_set_enabled(pio, sm, false) ; // do not start the sm immediately
}
static inline void sport_uart_rx_program_stop(PIO pio, uint sm, uint pin_rx) {
    pio_sm_set_enabled(pio, sm, false); // disable sm.    
}
static inline void sport_uart_rx_program_restart(PIO pio, uint sm, uint pin_rx , bool invert) {
    pio_sm_set_pindirs_with_mask(pio, sm, 0 , 1u << pin_rx); // set pin as input
    if (invert) {
        gpio_set_inover(pin_rx,  GPIO_OVERRIDE_INVERT) ; // added by ms to manage inverted UART from Sport
        gpio_pull_down(pin_rx); // changed by MS
    } else {
        gpio_pull_up(pin_rx); // changed by MS
    }    
    pio_sm_restart (pio, sm); // to test if we need an enable after this
    pio_sm_set_enabled(pio, sm, true);
}    
/*
static inline char sport_uart_rx_program_getc(PIO pio, uint sm) {
    // 8-bit read from the uppermost byte of the FIFO, as data is left-justified
    io_rw_8 *rxfifo_shift = (io_rw_8*)&pio->rxf[sm] + 3;
    while (pio_sm_is_rx_fifo_empty(pio, sm))
        tight_loop_contents();
    return (char)*rxfifo_shift;
}
*/

#endif
