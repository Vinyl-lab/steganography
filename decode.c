#include"decode.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "common.h"
#include "types.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2]==NULL)
    {
        printf("bmp file has not been passed\n");
        return e_failure;
    }
    else if(strstr(argv[2],".bmp")==NULL)
    {
        printf("Invalid bmp file\n");
        return e_failure;
    }

    decInfo->image_fname = argv[2];
    if(argv[3]==NULL)
    {
        printf("No magic string is passed\n");
        return e_failure;
    }
    strcpy(decInfo->key_string, argv[3]);
    return e_success;
}

Status decode_byte_from_lsb(char *image_buffer, char *decoded_data)
{
    *decoded_data = 0;
    for(int i=0; i<MAX_IMG_DATA; i++)
    {
        *decoded_data = (image_buffer[i]&1) | (*decoded_data << 1);
    }
    return e_success;
}

Status decode_data_from_image(char *img, int size, FILE *fptr_enc_image)
{
    for(int i=0; i<size; i++)
    {
        char image_buffer[MAX_IMG_DATA];
        fread(image_buffer, 1, MAX_IMG_DATA, fptr_enc_image);
        decode_byte_from_lsb(image_buffer, &img[i]);
    }
    return e_success;
}

Status check_magic_string(DecodeInfo *decInfo)
{
    decInfo->fptr_enc_image = fopen(decInfo->image_fname, "rb");
    fseek(decInfo->fptr_enc_image, 54, SEEK_SET);
    int len = strlen(MAGIC_STRING);
    for(int i=0; i<len; i++)
    {
        char image_buffer[MAX_IMG_DATA];
        fread(image_buffer, 1, MAX_IMG_DATA, decInfo->fptr_enc_image);
        decode_byte_from_lsb(image_buffer, &decInfo->decoded_data[i]);
        if(decInfo->decoded_data[i]!=decInfo->key_string[i])
        {
            printf("Magic string is not matching\n");
            return e_failure;
        }
    }
    return e_success;   
}

uint get_file_extn_size(FILE *fptr_enc_image, DecodeInfo *decInfo)
{
    //char extn[10];
    decode_data_from_image((char *)&decInfo->extn_size, sizeof(int), fptr_enc_image);
    printf("File extension size is %d\n", decInfo->extn_size);
    return decInfo->extn_size;    
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->extn, decInfo->extn_size, decInfo->fptr_enc_image);
    decInfo->extn[decInfo->extn_size] = '\0';
    printf("File extension is %s\n", decInfo->extn);
    return e_success;
}

uint get_secret_file_size(FILE *fptr_enc_image, DecodeInfo *decInfo)
{
    decode_data_from_image((char *)&decInfo->file_size, sizeof(int), fptr_enc_image);
    printf("File size is %d\n", decInfo->file_size);
    return decInfo->file_size;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->decoded_data, decInfo->file_size, decInfo->fptr_enc_image);
    printf("Decoded data:\n");
     for(int i = 0; i < decInfo->file_size; i++)
    {
        printf("%c", decInfo->decoded_data[i]);
    }

    printf("\n");
    fwrite(decInfo->decoded_data, 1, decInfo->file_size, decInfo->output);
    return e_success;
}


Status do_decoding(DecodeInfo *decInfo)
{
    check_magic_string(decInfo);
    get_file_extn_size(decInfo->fptr_enc_image, decInfo);
    decode_secret_file_extn(decInfo);
    sprintf(decInfo->output_fname, "output.%s", decInfo->extn);
    decInfo->output = fopen(decInfo->output_fname, "wb");
    get_secret_file_size(decInfo->fptr_enc_image, decInfo);
    decode_secret_file_data(decInfo);
    fclose(decInfo->fptr_enc_image);
    fclose(decInfo->output);
    return e_success;
}

