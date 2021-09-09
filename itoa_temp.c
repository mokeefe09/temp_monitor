#include "itoa_temp.h"

void itoa_temp(int8_t num, char* output, char* degrees_str, uint8_t str_len){
	
	char* degrees_ptr;
	uint8_t digit_to_add;
	uint8_t positive_num;
	int i;


// Zero out the output buffer

	for (i = 0; i < str_len; i++){
		output[i] = '\0';
	}

// Convert the integer to a string, pointed to by output

	// If number is negative, add a negative sign to the output string and 
	// convert the number to its positive representation for the remainder
	// of the itoa operation.
	if (num < 0){
		*output++ = '-';
		positive_num = (uint8_t) (num * -1);
	}
	
	else{
		positive_num = (uint8_t) num;
	}

	// The if, else if block below will be used to determine how many chars
	// we will need to represent the integer, including the space afterwards
	// that will be required before we concatenate "degrees" to the string.
	// Upon determining the # of chars needed, we will work backwards, starting
	// with adding the space char, to convert the integer to a string.

	// If temp is above 100 we need 4 chars: 3 digits and a space
	if (positive_num >= 100){
		output += 3;
	}

	// If temp is 10-99, we need 3 chars: 2 digits and a space
	else if ( (positive_num >= 10) && (positive_num < 99) ){
		output += 2;
	}

	// If temp is 0-9, we need 2 chars: 1 digit and a space
	else{
		output += 1;
	}

	// This ptr will be used to concatenate "degrees" to the integer string
	// later on. Currently points to the char immediately after the space
	// char we add in the line of code below.
	degrees_ptr = output + 1;

	// Add a space at the "end". We will go backwards from here to add
	// the digit characters. 
	*output-- = ' ';


	do{
		// Carve off the right-most digit of the remaining number
		digit_to_add = (positive_num % 10U);

		// Change the number to ascii format 
		*output-- = 48U + digit_to_add;

		// Move to next digit
		positive_num /= 10;

	} while (positive_num != 0);


// Concatenate the "degrees" string to the output string.

	while (*degrees_str){
		*degrees_ptr++ = *degrees_str++;
	}

	// Add a zero to the end of the string just to be sure we have the null-byte
	*degrees_ptr = '\0';
}
