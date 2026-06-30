#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include"types.h"
#include <string.h>
#include"common.h"

/* Structure to store information required for
 * decoding secret file from stego Image
 * Info about output and intermediate data is
 * also stored
 */
#define MAX_DECODE_BUF_SIZE 1 //To store the 1 byte of data read from the 8 bytes of image data
#define MAX_IMG_DATA (MAX_DECODE_BUF_SIZE * 8) // To get the 1 byte of data from 8 byte of image data
#define MAX_INT_SIZE (MAX_IMG_DATA * 4)// To get 32 bits(4 byte of info) present across the 32 byte of image data

typedef struct _DecodeInfo
{
    char *image_fname;
    FILE *fptr_enc_image;

    int extn_size;
    char extn[10];
    int file_size;

    char output_fname[30];
    FILE *output;

    char image_data[MAX_IMG_DATA];
    char decoded_data[1024];
    char decode_file_size[MAX_INT_SIZE];
    char key_string[sizeof(MAGIC_STRING)];


}DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status check_magic_string(DecodeInfo *decInfo);
Status decode_byte_from_lsb(char *image_buffer, char *decoded_data);
Status decode_data_from_image(char *img, int size, FILE *fptr_enc_image);
uint get_file_extn_size(FILE *fptr_enc_image, DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
uint get_secret_file_size(FILE *fptr_enc_image, DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif