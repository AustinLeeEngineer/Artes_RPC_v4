#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "scpi/scpi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "usart.h"  // For HAL_UART_Transmit
#include "cli.h"


// Escape sequence states
typedef enum {
    CLI_STATE_NORMAL,
    CLI_STATE_ESC,
    CLI_STATE_CSI  // Control Sequence Introducer (ESC [)
} cli_state_t;

// CLI structure for line editing
typedef struct {
    char buffer[CLI_BUF_SIZE];
    uint16_t len;     // Current length of input
    uint16_t pos;     // Cursor position
    cli_state_t state;
} cli_t;


static cli_t cli = { .len = 0, .pos = 0, .state = CLI_STATE_NORMAL };

// External SCPI context (from artes_rpc_scpi.c)
extern scpi_t scpi_context;
void safe_printf(const char *format, ...);

// Function to send string over UART
void cli_send(const char *str, size_t len) {
    HAL_UART_Transmit(&huart2, (uint8_t *)str, len, HAL_MAX_DELAY);
}

// Send single char
static void cli_send_char(char c) {
    cli_send(&c, 1);
}

// Clear current line (ANSI: move to start, clear to end)
static void cli_clear_line(void) {
    cli_send("\r\033[2K", 4);  // \r to start, ESC[2K clear line
}

// Redraw the current line from cursor
static void cli_redraw(void) {
    cli_clear_line();
    cli_send(PROMPT, sizeof(PROMPT) - 1);
    cli_send(cli.buffer, cli.len);
    // Move cursor back to pos
    if (cli.pos < cli.len) {
        char back[16];
        size_t back_len = snprintf(back, sizeof(back), "\033[%dD", cli.len - cli.pos);
        cli_send(back, back_len);
    }
}

// Insert char at current pos
static void cli_insert_char(char c) {
    if (cli.len >= CLI_BUF_SIZE - 1) return;  // Buffer full
    if (cli.pos < cli.len) {
        memmove(&cli.buffer[cli.pos + 1], &cli.buffer[cli.pos], cli.len - cli.pos);
    }
    cli.buffer[cli.pos++] = c;
    cli.len++;
    // Echo inserted char and rest of line
    cli_send(&cli.buffer[cli.pos - 1], cli.len - (cli.pos - 1));
    // Move cursor back if needed
    if (cli.pos < cli.len) {
        char back[16];
        size_t back_len = snprintf(back, sizeof(back), "\033[%dD", cli.len - cli.pos);
        cli_send(back, back_len);
    }
}

// Delete char at current pos (like forward delete) or backspace
static void cli_delete_char(uint8_t backspace) {
    if (backspace) {
        if (cli.pos == 0) return;
        cli.pos--;
    } else {
        if (cli.pos >= cli.len) return;
    }
    memmove(&cli.buffer[cli.pos], &cli.buffer[cli.pos + 1], cli.len - cli.pos - (backspace ? 0 : 1));
    cli.len--;
    // Redraw from pos
    cli_send("\033[0K", 4);  // Clear to end of line
    cli_send(&cli.buffer[cli.pos], cli.len - cli.pos);
    // Move cursor back if needed
    if (cli.pos < cli.len) {
        char back[16];
        size_t back_len = snprintf(back, sizeof(back), "\033[%dD", cli.len - cli.pos);
        cli_send(back, back_len);
    }
}

// Handle home (cursor to start)
static void cli_home(void) {
    char left[16];
    size_t left_len = snprintf(left, sizeof(left), "\033[%dD", cli.pos);
    cli_send(left, left_len);
    cli.pos = 0;
}

// Handle end (cursor to end)
static void cli_end(void) {
    char right[16];
    size_t right_len = snprintf(right, sizeof(right), "\033[%dC", cli.len - cli.pos);
    cli_send(right, right_len);
    cli.pos = cli.len;
}

// Handle left arrow (cursor left)
static void cli_left(void) {
    if (cli.pos > 0) {
        cli_send("\033[1D", 4);
        cli.pos--;
    }
}

// Handle right arrow (cursor right)
static void cli_right(void) {
    if (cli.pos < cli.len) {
        cli_send("\033[1C", 4);
        cli.pos++;
    }
}

// Process single input byte
void cli_process_byte(uint8_t byte) {
	//safe_printf("Received byte in cli.c: \r\n");

	switch (cli.state) {
        case CLI_STATE_NORMAL:
            if (byte == 0x1B) {  // ESC
                cli.state = CLI_STATE_ESC;
            } else if (byte == 0x7F || byte == 0x08) {  // Backspace (DEL or BS)
                cli_delete_char(1);
            } else if (byte == '\r' || byte == '\n') {  // Enter
            	cli_send("\r\n", 2);
                if (cli.len > 0) {
                    cli.buffer[cli.len] = '\0';  // Null-terminate
                    SCPI_Input(&scpi_context, cli.buffer, cli.len);
                    // Send the termination characters to force execution!
                    SCPI_Input(&scpi_context, "\r\n", 2);
                }
                memset(&cli, 0, sizeof(cli));  // Reset
                cli_send(PROMPT, sizeof(PROMPT) - 1);
            } else if (byte >= 32 && byte <= 126) {  // Printable
                cli_insert_char(byte);
            }
            break;

        case CLI_STATE_ESC:
            if (byte == '[') {
                cli.state = CLI_STATE_CSI;
            } else {
                // Single ESC: clear input
                memset(cli.buffer, 0, CLI_BUF_SIZE);
                cli.len = 0;
                cli.pos = 0;
                cli_redraw();
                cli.state = CLI_STATE_NORMAL;
            }
            break;

        case CLI_STATE_CSI:
            switch (byte) {
                case 'A':  // Up arrow (ignore or history if implemented)
                case 'B':  // Down arrow (ignore or history)
                    break;
                case 'C':  // Right arrow
                    cli_right();
                    break;
                case 'D':  // Left arrow
                    cli_left();
                    break;
                case 'H':  // Home (ESC [ H)
                case '1':  // Home (ESC [ 1 ~)
                    cli_home();
                    if (byte == '1') {  // Expect '~' next, but handle here
                        // For simplicity, assume next byte is '~'; in real, buffer if needed
                    }
                    break;
                case 'F':  // End (ESC [ F)
                case '4':  // End (ESC [ 4 ~)
                    cli_end();
                    break;
                case '~':  // Terminator for Home/End
                    break;
                default:
                    // Unknown, ignore
                    break;
            }
            cli.state = CLI_STATE_NORMAL;
            break;
    }
}

