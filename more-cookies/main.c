#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base64.h"

#define URL "http://mercury.picoctf.net:<YOUR_PORT>/flag"
#define SEARCH_STRING "picoCTF{"

unsigned char *decode_cookie(char *data, size_t data_len, size_t *decoded_len)
{
    // twice decoding because the actual string encoded twice
    size_t first_decoded_len;
    char *first_decoded = base64_decode(data, data_len, &first_decoded_len);

    unsigned char *raw = base64_decode(first_decoded, first_decoded_len, decoded_len);

    free(first_decoded);

    return raw;
}

char *bit_flip(short position, int bit, unsigned char *raw_bytes, size_t raw_len)
{
    // Allocate memory for the copy
    unsigned char *data = (unsigned char *)malloc(raw_len * sizeof(unsigned char));
    if (data == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Copy the contents of the original array to the copy
    memcpy(data, raw_bytes, raw_len);

    data[position] = data[position] ^ bit;

    // twice encoding
    size_t first_encoded_len;
    char *first_encoded = base64_encode(data, raw_len, &first_encoded_len);

    size_t output_len;
    char *output = base64_encode(first_encoded, first_encoded_len, &output_len);

    free(first_encoded);

    return output;
}

int getFlag(const char *auth_cookie)
{
    // Construct the curl command
    char curl_command[1024];
    snprintf(curl_command, sizeof(curl_command), "curl -s -H \"Cookie: auth_name=%s\" \"%s\"", auth_cookie, URL);

    // Execute the curl command and capture its output
    FILE *curl_output = popen(curl_command, "r");
    if (!curl_output)
    {
        fprintf(stderr, "Failed to execute curl command\n");
        return 1;
    }

    // Read the output of the curl command
    char response_buffer[4096] = {0};
    size_t bytes_read = fread(response_buffer, 1, sizeof(response_buffer) - 1, curl_output);
    pclose(curl_output);

    if (bytes_read == 0)
    {
        fprintf(stderr, "Failed to read curl command output\n");
        return 1;
    }

    // Check if the response contains the search string
    if (strstr(response_buffer, SEARCH_STRING) != NULL)
    {
        printf(response_buffer);
        fflush(stdout);
        return 0;
    }
    else
    {
        return 1;
    }
}

int main()
{
    char cookie[] = "<YOUR_COOKIE>";
    size_t cookie_len = strlen(cookie);
    int total = cookie_len * 128;

    size_t raw_len;
    unsigned char *raw_bytes = decode_cookie(cookie, cookie_len, &raw_len);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            char *auth_cookie = bit_flip(i, j, raw_bytes, raw_len);
            int result = getFlag(auth_cookie);

            if (result == 0)
                break;
            else
            {
                free(auth_cookie);
            }

            printf("Tested i(%d), j(%d)\n", i, j);
            fflush(stdout);
        }
    }

    return 0;
}