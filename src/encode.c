#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "common.h"
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

OperationType check_operation_type(char *argv[])
{
    if(argv[1]==NULL)
    {
        return e_unsupported;
    }
    else if(strcmp(argv[1], "-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }   
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2]==NULL)
    {
        printf("bmp file has not been passed");
        return e_failure;
    }
    if(strstr(argv[2],".bmp")==NULL)
    {
        printf("Invalid bmp file\n");
        return e_failure;
    }
    //storing argv[2] vmp file name to structure
    encInfo->src_image_fname = argv[2];

    if(argv[3]==NULL)
    {
        printf("sec file is not present\n");
        return e_failure;
    }
    if(strchr(argv[3], '.')==NULL)
    {
        printf("invalid sec fail\n");
        return e_failure;
    }
    //store argv[3] to structure
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file, strrchr(argv[3], '.')+1);
    printf("File extension is %s\n", encInfo->extn_secret_file);
    for(int i = 0; i < strlen(encInfo->extn_secret_file); i++)
    {
        printf("%02X ", (unsigned char)encInfo->extn_secret_file[i]);
    }
printf("\n");

    if(argv[4]==NULL)
    {
        encInfo->stego_image_fname="stego.bmp";
    }
    else
    {
        if(strstr(argv[4], ".bmp")==NULL)
        {
            printf("Invalid file name\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    //To know which position it was pointing to before seeking to 18th byte
    long int pos = ftell(fptr_image);
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    
    //getting back to the position it was pointing to before seeking to 18th byte
    fseek(fptr_image, pos, SEEK_SET);

    // Return image capacity
    return width * height * 3;
    
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0L, SEEK_END);
    uint size = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);
    return size;
}

Status check_capacity(EncodeInfo *encInfo)
{
    //get image capacity
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("image capacity= %u\n", encInfo->image_capacity);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    printf("secret file size= %lu\n", encInfo->size_secret_file);
    if(8*encInfo->size_secret_file > encInfo->image_capacity)
    {
        printf("Error: File size is greater than image capacity\n");
        return e_failure;
    }
    else
    {
        return e_success;
    }
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    fread(header, 1, 54, fptr_src_image);
    fwrite(header, 1, 54, fptr_dest_image);
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7-i)) & 1);
    }
    return e_success;
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    for(int i=0; i<size; i++)
    {
        char image_data[MAX_IMAGE_BUF_SIZE];
        if(fread(image_data, 1, MAX_IMAGE_BUF_SIZE, fptr_src_image)!=MAX_IMAGE_BUF_SIZE)
        {
            printf("Error: Unable to read from source image file\n");
            return e_failure;
        }
        encode_byte_to_lsb(data[i], image_data);
        if(fwrite(image_data, 1, MAX_IMAGE_BUF_SIZE, fptr_stego_image)!=MAX_IMAGE_BUF_SIZE)
        {
            printf("Error: Unable to write to stego image file\n");
            return e_failure;
        }
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image((char *)magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
    encode_data_to_image((char *)&extn_size, sizeof(int), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("File extension is %s\n", file_extn);
    encode_data_to_image((char *)file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_data_to_image((char *)&file_size, sizeof(int), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_data[MAX_SECRET_BUF_SIZE];
    for(int i=0; i<encInfo->size_secret_file; i+=MAX_SECRET_BUF_SIZE)
    {
        if(fread(secret_data, 1, MAX_SECRET_BUF_SIZE, encInfo->fptr_secret)!=MAX_SECRET_BUF_SIZE)
        {
            printf("Unable to read from Secret file\n");
            return e_failure;
        }
        encode_data_to_image(secret_data, 1, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    }
    return e_success;    
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[1024];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, sizeof(buffer), fptr_src_image)))
    {
        if(fwrite(buffer, 1, bytes_read, fptr_stego_image)!=bytes_read)
        {
            printf("Unable to write remaining image data to stego image\n");
            return e_failure;
        }
        
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    int ret = open_files(encInfo);
    if(ret == e_success)
    {
        copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    }
    else
    {
        printf("Error\n");
        return 0;
    }
    get_image_size_for_bmp(encInfo->fptr_src_image);
    get_file_size(encInfo->fptr_secret);
    check_capacity(encInfo);
    encode_magic_string(MAGIC_STRING, encInfo);
    int len = strlen(encInfo->extn_secret_file);
    printf("File extension size is %d\n", len);
    encode_secret_file_extn_size(len, encInfo);
    encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
    encode_secret_file_size(encInfo->size_secret_file, encInfo);
    encode_secret_file_data(encInfo);
    ret = copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    /*if(ret == e_success)
    {
        printf("Encoding done successfully\n");        
    } */
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}
