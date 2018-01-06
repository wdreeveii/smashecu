#include "buffer.h"

/*
 * Name : void buffer_init(buffer_t *buffer)
 *
 * Purpose : This function initialize or reset the buffer's structure. The counter
 *           of characters is clear, input and output pointer are set to the start of the buffer.
 *
 * Input : buffer, pointer on buffer_t structure
 *         tab, pointer to storage space
 *         size, size of storage space
 * Output : void
 *
 * History :
 *   Unspecified changes by Whitham Reeve - changelist lost to time.
 *   04-04-2005 : Creation - Pierrick Calvet
 */
void buffer_init(buffer_t *buffer, unsigned char *tab, unsigned int size) {
    buffer->tab = tab;
    buffer->size = size;

    buffer->count = 0;                      // No chars 
    buffer->p_in = buffer->tab;             // Input and output pointers
    buffer->p_out = buffer->tab;            // at the beginning of array
}


/*
 * Name : char buffer_push(buffer_t *buffer, unsigned char byte)
 *
 * Purpose : Push a byte into the buffer
 *
 * Inputs : buffer, pointer on buffer_t structure
 *          byte, byte to put into the buffer
 *
 * Outputs : 1 if no error occurs
 *           0 if the buffer is full
 *
 * History :
 *   04-04-2005 : Creation - Pierrick Calvet
 */
char buffer_push(buffer_t *buffer, unsigned char byte)
{
	if(buffer->count == buffer->size)                     // Return error
		return 0;                                    // if buffer full

	*(buffer->p_in) = byte;                               // Push byte in the buffer
	buffer->count++;                                      // Increment number of bytes in the buffer

	if(++(buffer->p_in) == (buffer->tab + buffer->size))   // If input pointer at the end of buffer,
		buffer->p_in = buffer->tab;                   // set it at the start

	return 1;                                             // Success
}


/*
 * Name : char buffer_pop(buffer_t *buffer, unsigned char *byte)
 *
 * Purpose : Pop a byte from the buffer. The byte is put at address pointed by *byte.
 *
 * Inputs : buffer, pointer on buffer_t structure
 *          byte, adress of a var where the byte pull from the buffer will be put.
 *
 * Outputs : 1 if no error occurs
 *           0 if the buffer is empty
 *
 * History :
 *   04-04-2005 : Creation - Pierrick Calvet
 */
char buffer_pop(buffer_t *buffer, unsigned char *byte)
{
	if(buffer->count == 0)                                 // Return error
		return 0;                                     // if buffer empty

	*byte = *(buffer->p_out);                              // Return character where are the pointer
	buffer->count--;                                       // Decrement number of characters in the buffer

	if (++(buffer->p_out) == (buffer->tab + buffer->size))  // If output pointer at the end of buffer,
		buffer->p_out = buffer->tab;                   // set it at the start

	return 1;                                              // Success
}

