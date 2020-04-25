#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <inttypes.h>
#include <signal.h>
#include "logger.h"

#define BUF_SIZE   0xFFFFFF
#define SIGINFO 29

// Action: decode 2, encode 1. bitwise.
#define ENCODE 1
#define DECODE 2

float status = 0;

void sig_handler(int sig) { infof("%d%% of file processed\n", (int) status); }

//Code retrieved from: https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};
static char *decoding_table = NULL;
static int mod_table[] = {0, 2, 1};

void build_decoding_table() {

    decoding_table = malloc(256);
    int i;
    for (i = 0; i < 64; i++)
        decoding_table[(unsigned char) encoding_table[i]] = i;
}

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = malloc(*output_length);
    if (encoded_data == NULL) return NULL;
    int i, j;
    for (i = 0, j = 0; i < input_length;) {
        status = (float) i / (float) input_length * 100;
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}

unsigned char *base64_decode(const char *data,
                             size_t input_length,
                             size_t *output_length) {
    if (decoding_table == NULL) build_decoding_table();
    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;
    int i, j;
    for (i = 0, j = 0; i < input_length;) {
        status = (float) i / (float) input_length * 100;
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6)
        + (sextet_b << 2 * 6)
        + (sextet_c << 1 * 6)
        + (sextet_d << 0 * 6);

        if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }
    return decoded_data;
}

void base64_cleanup() {
    free(decoding_table);
}
//Here starts my code


int main(int argc, char *argv[]) {

    signal(SIGINT, sig_handler); 
    signal(SIGUSR1, sig_handler);
    // Action: decode 2, encode 1. bitwise.
    int action=0;
    char *output_file = calloc(1024, sizeof(char));

     if(argc!=3){
        errorf("Error in number of parameters. Aborted\n");
        return -1;
     } 
    if(strcmp(argv[1], "--encode")==0){
        strcpy(output_file,"encoded.txt");
        action|=ENCODE;
    }else if(strcmp(argv[1], "--decode")==0){
        action|=DECODE;
        strcpy(output_file,"decoded.txt");            
    }

    long size;
    char *buffer;
    long read_size = 0;

    FILE *file;
    file = fopen(argv[2],"r");
    if (file == NULL) {
        errorf("Couldn't open input file. Aborted\n");
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    buffer = malloc(size);
    read_size = fread(buffer, 1, size, file);
    fclose(file);

    size_t output_size = 0;

    char *result;
    // Action: decode 2, encode 1. bitwise.
    if (action == ENCODE) {
        infof("Encoding file\n");
        result = base64_encode(buffer, strlen(buffer), &output_size);
    } else if (action == DECODE) {
        infof("Decoding file\n");
        result=(char*)  base64_decode(buffer, strlen(buffer), &output_size);
    } else {
        errorf("Action flag incorrect. Set as flag --encode or --decode. Aborted\n");
        return -1;
    }    
    base64_cleanup();

    FILE *output;
    output= fopen(output_file,"w");
    if (output == NULL) {
        panicf("Error creando archivo '%s'\n", output_file);
        return -1;
    }
    fwrite(result, output_size, 1, output);
    infof("Creando archivo '%s'\n", output_file);
    fclose(output);
    return 0;
}