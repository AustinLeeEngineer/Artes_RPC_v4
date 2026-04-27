
#include "artes_rpc_scpi.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "iwdg.h"
#include <ctype.h>

extern UART_HandleTypeDef huart2;
void safe_printf(const char *format, ...);

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context);



scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];

scpi_t scpi_context;

////////////////////////////
//  CORE FUNCTIONS
////////////////////////////

size_t SCPI_Write(scpi_t * context, const char * data, size_t len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)data, len, HAL_MAX_DELAY);
    return len;
}

scpi_result_t SCPI_Flush(scpi_t * context)
{
//    safe_printf("FLUSH\n");
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err)
{
    //safe_printf("**ERROR: %d, \"%s\"\n", (int16_t) err, SCPI_ErrorTranslate(err));
//    SCPI_Input(context, NULL, 0);
    return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val)
{
    (void) context;

    if (SCPI_CTRL_SRQ == ctrl)
    {
        //safe_printf("**SRQ: 0x%X (%d)\r\n", val, val);
    }
    else
    {
        //safe_printf("**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context)
{
    SCPI_ErrorClear(context);  // Clear error queue
    SCPI_Input(context, NULL, 0);
//    SCPI_Init(&scpi_context,
//            scpi_commands,
//            &scpi_interface,
//            scpi_units_def,
//            SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
//            scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
//            scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);



    //safe_printf("SCPI Reset\n");
    return SCPI_RES_OK;
}


////////////////////////////
//   ARTES RPC FUNCTIONS
////////////////////////////

//------------------------------------//
//   System Commands                     //
//------------------------------------//
scpi_result_t wdt_timeout(scpi_t * context)
{
    //safe_printf("wdt=%lu sec\n", get_iwdg_timeout());
    return SCPI_RES_OK;
}

scpi_result_t sys_frequency(scpi_t * context)
{ 
    //safe_printf("Sys Freq = %lu MHz\n", HAL_RCC_GetSysClockFreq());
    return SCPI_RES_OK;
}

void report_stack_usage(void);
scpi_result_t report_stack_usage_cb(scpi_t * context)
{
    report_stack_usage();
    return SCPI_RES_OK;
}

//------------------------------------//
//   LED Commands                     //
//------------------------------------//
typedef enum {
	LED_TAG_ALL = 1,
	LED_TAG_RUNNING,
	LED_TAG_PASS_FAIL,
}led_tag_t;

// Set LED Color
scpi_result_t scpi_led_color(scpi_t * context)
{
    led_color_t color;
    const char *str;
    size_t len;
    char upper_str[16];

    if (SCPI_ParamCharacters(context, &str, &len, FALSE))
    {
        if (len >= sizeof(upper_str)) return SCPI_RES_ERR;
        for (size_t i = 0; i < len; i++) {
            upper_str[i] = toupper((unsigned char)str[i]);
        }
        upper_str[len] = '\0';

        if(1 == len)
        {
            if (upper_str[0] == '1')      color = LED_COLOR_GREEN;
            else if (upper_str[0] == '2') color = LED_COLOR_RED;
            else if (upper_str[0] == '3') color = LED_COLOR_YELLOW;
            else return SCPI_RES_ERR;
        }
        else
        {
            if (0 == strcmp(upper_str, "GREEN"))       color = LED_COLOR_GREEN;
            else if (0 == strcmp(upper_str, "RED"))    color = LED_COLOR_RED;
            else if (0 == strcmp(upper_str, "YELLOW")) color = LED_COLOR_YELLOW;
            else return SCPI_RES_ERR;
        }
    }
    else return SCPI_RES_ERR;

    led_tag_t tag = (led_tag_t)SCPI_CmdTag(context);
    switch (tag)
    {
        case LED_TAG_ALL:
            set_led_color(LED_RUNNING, color);
            set_led_color(LED_PASS_FAIL, color);
            break;
        case LED_TAG_PASS_FAIL:
            set_led_color(LED_PASS_FAIL, color);
            break;
        case LED_TAG_RUNNING:
            set_led_color(LED_RUNNING, color);
            break;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
            return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

// Query LED Color
scpi_result_t scpi_led_color_q(scpi_t * context)
{
    //safe_printf("scpi_led_color_q");

    led_tag_t tag = (led_tag_t)SCPI_CmdTag(context);
    switch (tag)
    {
        case LED_TAG_ALL:
            /* Handle ALL */
            break;
        case LED_TAG_PASS_FAIL:
            /* Handle SYSTem */
            break;
        case LED_TAG_RUNNING:
            /* Handle TEST */
            break;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
            return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

// Set LED State
scpi_result_t scpi_led_state(scpi_t * context)
{
    led_state_t state;
    const char *str;
    size_t len;
    char upper_str[16]; // Local buffer for conversion

    if (SCPI_ParamCharacters(context, &str, &len, FALSE))
    {
        // Safety check and conversion to Uppercase
        if (len >= sizeof(upper_str)) return SCPI_RES_ERR;
        for (size_t i = 0; i < len; i++) {
            upper_str[i] = toupper((unsigned char)str[i]);
        }
        upper_str[len] = '\0';

        if(1 == len)
        {
            if (upper_str[0] == '0')      state = LED_STATE_OFF;
            else if (upper_str[0] == '1') state = LED_STATE_ON;
            else if (upper_str[0] == '2') state = LED_STATE_BLINK;
            else return SCPI_RES_ERR;
        }
        else
        {
            if (0 == strcmp(upper_str, "OFF"))        state = LED_STATE_OFF;
            else if (0 == strcmp(upper_str, "ON"))    state = LED_STATE_ON;
            else if (0 == strcmp(upper_str, "BLINK")) state = LED_STATE_BLINK;
            else return SCPI_RES_ERR;
        }
    }
    else return SCPI_RES_ERR;

    led_tag_t tag = (led_tag_t)SCPI_CmdTag(context);
    switch (tag)
    {
        case LED_TAG_ALL:
            set_led_state(LED_RUNNING, state);
            set_led_state(LED_PASS_FAIL, state);
            break;
        case LED_TAG_PASS_FAIL:
            set_led_state(LED_PASS_FAIL, state);
            break;
        case LED_TAG_RUNNING:
            set_led_state(LED_RUNNING, state);
            break;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
            return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

// Query LED State
scpi_result_t scpi_led_state_q(scpi_t * context)
{
    //safe_printf("scpi_led_state_q");

    led_tag_t tag = (led_tag_t)SCPI_CmdTag(context);
    switch (tag)
    {
        case LED_TAG_ALL:
            /* Handle ALL */
            break;
        case LED_TAG_PASS_FAIL:
            /* Handle SYSTem */
            break;
        case LED_TAG_RUNNING:
            /* Handle TEST */
            break;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
            return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

// Set LED Frequency
scpi_result_t scpi_led_freq(scpi_t * context)
{
    //safe_printf("scpi_led_freq");

    uint32_t val;
    if (!SCPI_ParamUInt32(context, &val, TRUE))
    {
        return SCPI_RES_ERR;
    }
    if (val < 100 || val > 10000) // Per manual .1-10s, ms on/off
    {
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }

    led_tag_t tag = (led_tag_t)SCPI_CmdTag(context);
    switch (tag)
    {
        case LED_TAG_ALL:
            /* Handle ALL */
            set_led_freq(LED_RUNNING, val);
            set_led_freq(LED_PASS_FAIL, val);
            break;
        case LED_TAG_PASS_FAIL:
            /* Handle SYSTem */
            set_led_freq(LED_PASS_FAIL, val);
            break;
        case LED_TAG_RUNNING:
            /* Handle TEST */

            set_led_freq(LED_RUNNING, val);
            break;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
            return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}

// Query LED Frequency
scpi_result_t scpi_led_freq_q(scpi_t * context)
{
    //safe_printf("scpi_led_freq_q");

    led_tag_t tag = (led_tag_t)SCPI_CmdTag(context);
    switch (tag)
    {
        case LED_TAG_ALL:
            /* Handle ALL */
            break;
        case LED_TAG_PASS_FAIL:
            /* Handle SYSTem */
            break;
        case LED_TAG_RUNNING:
            /* Handle TEST */
            break;
        default:
            SCPI_ErrorPush(context, SCPI_ERROR_SYSTEM_ERROR);
            return SCPI_RES_ERR;
    }
    return SCPI_RES_OK;
}



////////////////////////////
//     SCPI COMMAND
//   CALLBACK FUNCTIONS
////////////////////////////
//------------------------------------//
//   Relay Commands                   //
//------------------------------------//

// Memory array to store mask status: 0 = Unmasked (Allowed), 1 = Masked (Locked)
// Sized to 10 to comfortably hold all 8 relays + ALL tag
static uint8_t relay_mask_status[10] = {0};

// Set Relay State
scpi_result_t RELAY_State(scpi_t * context)
{
    relay_state_t state;
    const char *str;
    size_t len;
    char upper_str[16];

    // 1. Get the ON/OFF text and make it uppercase
    if (SCPI_ParamCharacters(context, &str, &len, FALSE))
    {
        if (len >= sizeof(upper_str)) return SCPI_RES_ERR;
        for (size_t i = 0; i < len; i++) {
            upper_str[i] = toupper((unsigned char)str[i]);
        }
        upper_str[len] = '\0';

        if (0 == strcmp(upper_str, "OFF") || upper_str[0] == '0') {
            state = RELAY_STATE_OFF;
        }
        else if (0 == strcmp(upper_str, "ON") || upper_str[0] == '1') {
            state = RELAY_STATE_ON;
        }
        else return SCPI_RES_ERR;
    }
    else return SCPI_RES_ERR;

    // 2. See which relay they typed
    relay_types_t tag = (relay_types_t)SCPI_CmdTag(context);

    // 3. SAFETY CHECK: Ensure the relay isn't masked
    if (tag == RELAY_ALL) {
        // If they ask for ALL, ensure none are masked before proceeding
        for (int i = 0; i < 8; i++) {
            if (relay_mask_status[i] == 1) {
                SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR);
                return SCPI_RES_ERR; // Abort if any relay is locked
            }
        }
    } else {
        // Check specific relay
        if (relay_mask_status[tag] == 1) {
            SCPI_ErrorPush(context, SCPI_ERROR_EXECUTION_ERROR);
            return SCPI_RES_ERR; // Abort, relay is locked
        }
    }

    // 4. Send the command to main.c to fire the pin!
    set_relay_state(tag, state);

    return SCPI_RES_OK;
}

// Query Relay State
scpi_result_t RELAY_StateQ(scpi_t * context)
{
    relay_types_t tag = (relay_types_t)SCPI_CmdTag(context);

    // You cannot query the physical state of "ALL" at once, return an error
    if (tag == RELAY_ALL) {
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }

    // Read hardware pin and send 1 (ON) or 0 (OFF) back to terminal
    relay_state_t current_state = get_relay_state(tag);
    SCPI_ResultInt32(context, (current_state == RELAY_STATE_ON) ? 1 : 0);

    return SCPI_RES_OK;
}

// Set Relay Mask
scpi_result_t RELAY_Mask(scpi_t * context)
{
    uint8_t mask_val = 0;
    const char *str;
    size_t len;
    char upper_str[16];

    // Parse ON/OFF just like the state command
    if (SCPI_ParamCharacters(context, &str, &len, FALSE))
    {
        if (len >= sizeof(upper_str)) return SCPI_RES_ERR;
        for (size_t i = 0; i < len; i++) {
            upper_str[i] = toupper((unsigned char)str[i]);
        }
        upper_str[len] = '\0';

        if (0 == strcmp(upper_str, "OFF") || upper_str[0] == '0') {
            mask_val = 0; // Unmask
        }
        else if (0 == strcmp(upper_str, "ON") || upper_str[0] == '1') {
            mask_val = 1; // Mask
        }
        else return SCPI_RES_ERR;
    }
    else return SCPI_RES_ERR;

    relay_types_t tag = (relay_types_t)SCPI_CmdTag(context);

    // Apply the mask to the array
    if (tag == RELAY_ALL) {
        for (int i = 0; i < 8; i++) {
            relay_mask_status[i] = mask_val;
        }
    } else {
        relay_mask_status[tag] = mask_val;
    }

    // Optional: If you mask a relay that is currently ON, you might want it to automatically turn OFF for safety.
    // Uncomment the line below if that is the desired behavior:
    // if (mask_val == 1) set_relay_state(tag, RELAY_STATE_OFF);

    return SCPI_RES_OK;
}

// Query Relay Mask
scpi_result_t RELAY_MaskQ(scpi_t * context)
{
    relay_types_t tag = (relay_types_t)SCPI_CmdTag(context);

    if (tag == RELAY_ALL) {
        SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
        return SCPI_RES_ERR;
    }

    // Send the current memory value of the mask (1 or 0)
    SCPI_ResultInt32(context, relay_mask_status[tag]);

    return SCPI_RES_OK;
}

scpi_result_t SYSTEM_InterlockQ(scpi_t * context)
{
    // Read the physical state of the Interlock 1 pin (PA1)
    // Assuming 1 = Safe (Closed), 0 = E-Stop Pressed (Open)
    GPIO_PinState state = HAL_GPIO_ReadPin(B_Interlock1_GPIO_Port, B_Interlock1_Pin);

    // Send the result back to the terminal
    SCPI_ResultInt32(context, (state == GPIO_PIN_SET) ? 1 : 0);

    return SCPI_RES_OK;
}


const scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    { .pattern = "*CLS", .callback = SCPI_CoreCls,},
    { .pattern = "*ESE", .callback = SCPI_CoreEse,},
    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*OPC", .callback = SCPI_CoreOpc,},
    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
    { .pattern = "*SRE", .callback = SCPI_CoreSre,},
    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    { .pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    { .pattern = "*TST?", .callback = SCPI_CoreTstQ,},
    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},

    //=============================
    //     Artes RPC Commands
    //=============================

    {.pattern = "SYSTEM:STACK?", .callback = report_stack_usage_cb,},
    {.pattern = "SYSTEM:FREQ?",  .callback = sys_frequency,},
    {.pattern = "SYSTEM:WDT?",   .callback = wdt_timeout,},

    /* === Command Line Interface Commands (per Programming Manual 2.3) === */
//    {.pattern = "CLI:TIMEOUT",   .callback = cli_timeout,},
//    {.pattern = "CLI:TIMEOUT?",   .callback = cli_timeout_q,},
//    {.pattern = "CLI:ECHO",   .callback = cli_timeout,},  //ON/OFF
    /* === RELAY Commands (per Programming Manual 2.3) === */
//    {.pattern = "RELAY:[ALL|AC1|AC2|ITA|3PHase|INhibit1|INhibit2|INhibit3|INhibit4]:STATE?", .callback = RELAY_StateQ,},
//    {.pattern = "RELAY:[ALL|AC1|AC2|ITA|3PHase|INhibit1|INhibit2|INhibit3|INhibit4]:STATE",  .callback = RELAY_State,},
//    {.pattern = "RELAY:[ALL|AC1|AC2|ITA|3PHase|INhibit1|INhibit2|INhibit3|INhibit4]:MASK?",  .callback = RELAY_MaskQ,},
//    {.pattern = "RELAY:[ALL|AC1|AC2|ITA|3PHase|INhibit1|INhibit2|INhibit3|INhibit4]:MASK",   .callback = RELAY_Mask,},

	/* === RELAY Commands === */
	    // AC1
	    {.pattern = "RELAY:AC1:STATE",   .callback = RELAY_State,  .tag = RELAY_AC1},
	    {.pattern = "RELAY:AC1:STATE?",  .callback = RELAY_StateQ, .tag = RELAY_AC1},
	    {.pattern = "RELAY:AC1:MASK",    .callback = RELAY_Mask,   .tag = RELAY_AC1},
	    {.pattern = "RELAY:AC1:MASK?",   .callback = RELAY_MaskQ,  .tag = RELAY_AC1},

	    // AC2
	    {.pattern = "RELAY:AC2:STATE",   .callback = RELAY_State,  .tag = RELAY_AC2},
	    {.pattern = "RELAY:AC2:STATE?",  .callback = RELAY_StateQ, .tag = RELAY_AC2},
	    {.pattern = "RELAY:AC2:MASK",    .callback = RELAY_Mask,   .tag = RELAY_AC2},
	    {.pattern = "RELAY:AC2:MASK?",   .callback = RELAY_MaskQ,  .tag = RELAY_AC2},

	    // ITA
	    {.pattern = "RELAY:ITA:STATE",   .callback = RELAY_State,  .tag = RELAY_ITA},
	    {.pattern = "RELAY:ITA:STATE?",  .callback = RELAY_StateQ, .tag = RELAY_ITA},
	    {.pattern = "RELAY:ITA:MASK",    .callback = RELAY_Mask,   .tag = RELAY_ITA},
	    {.pattern = "RELAY:ITA:MASK?",   .callback = RELAY_MaskQ,  .tag = RELAY_ITA},

	    // 3PHASE
	    {.pattern = "RELAY:3PHase:STATE",  .callback = RELAY_State,  .tag = RELAY_3PHASE},
	    {.pattern = "RELAY:3PHase:STATE?", .callback = RELAY_StateQ, .tag = RELAY_3PHASE},
	    {.pattern = "RELAY:3PHase:MASK",   .callback = RELAY_Mask,   .tag = RELAY_3PHASE},
	    {.pattern = "RELAY:3PHase:MASK?",  .callback = RELAY_MaskQ,  .tag = RELAY_3PHASE},

		// INHIBIT 1
		{.pattern = "RELAY:INhibit1:STATE",  .callback = RELAY_State,  .tag = RELAY_INH1},
		{.pattern = "RELAY:INhibit1:STATE?", .callback = RELAY_StateQ, .tag = RELAY_INH1},
		{.pattern = "RELAY:INhibit1:MASK",   .callback = RELAY_Mask,   .tag = RELAY_INH1},
		{.pattern = "RELAY:INhibit1:MASK?",  .callback = RELAY_MaskQ,  .tag = RELAY_INH1},

		// INHIBIT 2
		{.pattern = "RELAY:INhibit2:STATE",  .callback = RELAY_State,  .tag = RELAY_INH2},
		{.pattern = "RELAY:INhibit2:STATE?", .callback = RELAY_StateQ, .tag = RELAY_INH2},
		{.pattern = "RELAY:INhibit2:MASK",   .callback = RELAY_Mask,   .tag = RELAY_INH2},
		{.pattern = "RELAY:INhibit2:MASK?",  .callback = RELAY_MaskQ,  .tag = RELAY_INH2},

		// INHIBIT 3
		{.pattern = "RELAY:INhibit3:STATE",  .callback = RELAY_State,  .tag = RELAY_INH3},
		{.pattern = "RELAY:INhibit3:STATE?", .callback = RELAY_StateQ, .tag = RELAY_INH3},
		{.pattern = "RELAY:INhibit3:MASK",   .callback = RELAY_Mask,   .tag = RELAY_INH3},
		{.pattern = "RELAY:INhibit3:MASK?",  .callback = RELAY_MaskQ,  .tag = RELAY_INH3},

		// INHIBIT 4
		{.pattern = "RELAY:INhibit4:STATE",  .callback = RELAY_State,  .tag = RELAY_INH4},
		{.pattern = "RELAY:INhibit4:STATE?", .callback = RELAY_StateQ, .tag = RELAY_INH4},
		{.pattern = "RELAY:INhibit4:MASK",   .callback = RELAY_Mask,   .tag = RELAY_INH4},
		{.pattern = "RELAY:INhibit4:MASK?",  .callback = RELAY_MaskQ,  .tag = RELAY_INH4},

		// ALL (Special Case)
		{.pattern = "RELAY:ALL:STATE",       .callback = RELAY_State,  .tag = RELAY_ALL},
		{.pattern = "RELAY:ALL:MASK",        .callback = RELAY_Mask,   .tag = RELAY_ALL},

    /* === LED Commands (per Programming Manual 3.4,5) === */

    {.pattern = "LED:ALL:COLOr",     .callback = scpi_led_color,   .tag = LED_TAG_ALL},
    {.pattern = "LED:ALL:COLOr?",    .callback = scpi_led_color_q, .tag = LED_TAG_ALL},
    {.pattern = "LED:PASSfail:COLOr",  .callback = scpi_led_color,   .tag = LED_TAG_PASS_FAIL},
    {.pattern = "LED:PASSfail:COLOr?", .callback = scpi_led_color_q, .tag = LED_TAG_PASS_FAIL},
    {.pattern = "LED:RUNNing:COLOr",    .callback = scpi_led_color,   .tag = LED_TAG_RUNNING},
    {.pattern = "LED:RUNNing:COLOr?",   .callback = scpi_led_color_q, .tag = LED_TAG_RUNNING},
    {.pattern = "LED:ALL:STATe",     .callback = scpi_led_state,   .tag = LED_TAG_ALL},
    {.pattern = "LED:ALL:STATe?",    .callback = scpi_led_state_q, .tag = LED_TAG_ALL},
    {.pattern = "LED:PASSfail:STATe",  .callback = scpi_led_state,   .tag = LED_TAG_PASS_FAIL},
    {.pattern = "LED:PASSfail:STATe?", .callback = scpi_led_state_q, .tag = LED_TAG_PASS_FAIL},
    {.pattern = "LED:RUNNing:STATe",    .callback = scpi_led_state,   .tag = LED_TAG_RUNNING},
    {.pattern = "LED:RUNNing:STATe?",   .callback = scpi_led_state_q, .tag = LED_TAG_RUNNING},
    {.pattern = "LED:ALL:FREQ",      .callback = scpi_led_freq,    .tag = LED_TAG_ALL},
    {.pattern = "LED:ALL:FREQ?",     .callback = scpi_led_freq_q,  .tag = LED_TAG_ALL},
    {.pattern = "LED:PASSfail:FREQ",   .callback = scpi_led_freq,    .tag = LED_TAG_PASS_FAIL},
    {.pattern = "LED:PASSfail:FREQ?",  .callback = scpi_led_freq_q,  .tag = LED_TAG_PASS_FAIL},
    {.pattern = "LED:RUNNing:FREQ",     .callback = scpi_led_freq,    .tag = LED_TAG_RUNNING},
    {.pattern = "LED:RUNNing:FREQ?",    .callback = scpi_led_freq_q,  .tag = LED_TAG_RUNNING},

	{.pattern = "SYSTem:INTerlock?", .callback = SYSTEM_InterlockQ,},



    SCPI_CMD_LIST_END
};





