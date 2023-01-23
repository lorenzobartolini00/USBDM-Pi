#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "commands.h"

// Count how many commands are in a given string
uint count_commands(char *command_str, char delimiter);

uint count_char(char *str, char c);

bool is_input_data_valid(char *str);

bool is_output_data_valid(char *str);

// Convert from string to hexadecimal
uint convert_to_hex(char *str);

// Convert a char rapresenting a hex value to an int rapresenting a decimal value
int hex2int(char ch);

uint nibble2byte(uint nibble_1, uint nibble_2);

uint array2dec(uint *array, uint length, bool reversed);

// Return whether a command is valid or not
bool is_command_valid(uint command);

// Determine when delay cycles should take place
uint get_delay_position(uint command);

// Get string from stdio
void get_string(char *buffer);

// Check if command is SYNC
bool is_command_sync(char *token);