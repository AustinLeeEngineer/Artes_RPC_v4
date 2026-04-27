
#ifndef ARTES_RPC_SCPI_H
#define ARTES_RPC_SCPI_H

#include "scpi/scpi.h"

#define SCPI_INPUT_BUFFER_LENGTH 512
#define SCPI_ERROR_QUEUE_SIZE 128
#define SCPI_IDN1 "DuoTech RPC"
#define SCPI_IDN2 "DEV"
#define SCPI_IDN3 NULL
#define SCPI_IDN4 "v0.01"


//------------------------//
//  LED Control variables //
//------------------------//
// Enums for LED color and state
typedef enum {
    LED_PASS_FAIL,
    LED_RUNNING
}led_types_t;

typedef enum {
    LED_COLOR_GREEN  = 1, // Green only (red=0, green=1)
    LED_COLOR_RED    = 2, // Red only (GPIO: red=1, green=0)
    LED_COLOR_YELLOW = 3  // Both (red=1, green=1)
} led_color_t;

typedef enum {
    LED_STATE_OFF = 0,    // No output (red=0, green=0)
    LED_STATE_ON = 1,     // Steady on in selected color
    LED_STATE_BLINK = 2   // Flashing in selected color (handled via timer/ISR)
} led_state_t;

// Struct for a single bi-color LED (System or Test)
typedef struct led_config {
    led_color_t color;     // Current color setting
    led_state_t state;     // Current state (off/on/blink)
    uint32_t blink_half_period_ms;
    uint32_t counter;
} led_config_t;

//------------------------//
//  Relay Control variables //
//------------------------//

// Define the relay names
typedef enum {
    RELAY_AC1,
    RELAY_AC2,
    RELAY_ITA,
    RELAY_3PHASE,
    RELAY_INH1,
    RELAY_INH2,
    RELAY_INH3,
    RELAY_INH4,
    RELAY_ALL
} relay_types_t;

// Define the relay states
typedef enum {
    RELAY_STATE_OFF = 0,
    RELAY_STATE_ON = 1
} relay_state_t;

// Tell the system this function exists in main.c
void set_relay_state(relay_types_t relay, relay_state_t state);
// Get the current physical state of a relay pin
relay_state_t get_relay_state(relay_types_t relay);

extern scpi_interface_t scpi_interface;
extern const scpi_command_t scpi_commands[];
extern char scpi_input_buffer[];
extern scpi_error_t scpi_error_queue_data[];
extern scpi_t scpi_context;

size_t        SCPI_Write  (scpi_t * context, const char * data, size_t len);
scpi_result_t SCPI_Flush  (scpi_t * context);
int           SCPI_Error  (scpi_t * context, int_fast16_t err);
scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
scpi_result_t SCPI_Reset  (scpi_t * context);



#endif  //  ARTES_RPC_SCPI_H
