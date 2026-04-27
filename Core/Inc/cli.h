
#ifndef CLI_HANDLER_H
#define CLI_HANDLER_H


#define CLI_BUF_SIZE 256  // Matches SCPI_INPUT_BUFFER_LENGTH
#define PROMPT "> "       // Command prompt

void cli_send(const char *str, size_t len);
void cli_process_byte(uint8_t byte);

#endif   //CLI_HANDLER_H
