#include <stdio.h>
#include <string.h>

#include "base64.h"

const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

int base64_char_value(char c)
{
    if ('A' <= c && c <= 'Z')
        return c - 'A';
    if ('a' <= c && c <= 'z')
        return c - 'a' + 26;
    if ('0' <= c && c <= '9')
        return c - '0' + 52;
    if (c == '+')
        return 62;
    if (c == '/')
        return 63;
    return -1;
}

char *base64_encode(const unsigned char *input, size_t len, size_t *out_len)
{
    char *output = (char *)malloc(((len + 2) / 3) * 4 + 1);
    if (!output)
        return NULL;

    size_t i, j;
    for (i = 0, j = 0; i < len;)
    {
        __UINT32_TYPE__ octet_a = i < len ? input[i++] : 0;
        __UINT32_TYPE__ octet_b = i < len ? input[i++] : 0;
        __UINT32_TYPE__ octet_c = i < len ? input[i++] : 0;

        __UINT32_TYPE__ triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        output[j++] = base64_chars[(triple >> 18) & 0x3F];
        output[j++] = base64_chars[(triple >> 12) & 0x3F];
        output[j++] = i > len + 1 ? '=' : base64_chars[(triple >> 6) & 0x3F];
        output[j++] = i > len ? '=' : base64_chars[triple & 0x3F];
    }

    output[j] = '\0';

    *out_len = j;

    return output;
}

unsigned char *base64_decode(const char *input, size_t len, size_t *out_len)
{
    int padding = 0;

    if (input[len - 1] == '=')
        padding++;
    if (input[len - 2] == '=')
        padding++;

    size_t decoded_len = (len / 4) * 3 - padding;
    unsigned char *decoded_data = (unsigned char *)malloc(decoded_len + 1);

    if (!decoded_data)
        return NULL;

    for (size_t i = 0, j = 0; i < len;)
    {
        __UINT32_TYPE__ sextet_a = input[i] == '=' ? 0 & i++ : base64_char_value(input[i++]);
        __UINT32_TYPE__ sextet_b = input[i] == '=' ? 0 & i++ : base64_char_value(input[i++]);
        __UINT32_TYPE__ sextet_c = input[i] == '=' ? 0 & i++ : base64_char_value(input[i++]);
        __UINT32_TYPE__ sextet_d = input[i] == '=' ? 0 & i++ : base64_char_value(input[i++]);

        __UINT32_TYPE__ triple = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;

        if (j < decoded_len)
            decoded_data[j++] = (triple >> 16) & 0xFF;
        if (j < decoded_len)
            decoded_data[j++] = (triple >> 8) & 0xFF;
        if (j < decoded_len)
            decoded_data[j++] = triple & 0xFF;
    }

    decoded_data[decoded_len] = '\0';
    *out_len = decoded_len;

    return decoded_data;
}
