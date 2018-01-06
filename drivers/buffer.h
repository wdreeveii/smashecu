
#ifndef _BUFFER_H
#define _BUFFER_H

/*  Struct buffer_t  */
typedef struct{
  unsigned char *tab;
  unsigned int  size;
  unsigned char *p_in, *p_out;
  unsigned int  count;
}buffer_t;

/*  Function's prototype  */
void buffer_init(buffer_t *buffer, unsigned char *tab, unsigned int size);
char buffer_push(buffer_t *buffer, unsigned char byte);
char buffer_pop(buffer_t *buffer, unsigned char *byte);

#endif
