#include <stdlib.h>

char *base64_encode(const unsigned char *input, size_t len, size_t *out_len);
unsigned char *base64_decode(const char *input, size_t len, size_t *out_len);