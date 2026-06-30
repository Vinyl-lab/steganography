#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    int ret= check_operation_type(argv);

    if(ret == e_encode)
    {
        //encoding
        int ret = read_and_validate_encode_args(argv, &encInfo);
        if(ret == e_failure)
        {
            printf("Validation is wrong\n");
            return 0;
        }
        ret = do_encoding(&encInfo);
        if(ret == e_failure)
        {
            printf("encoding is failed\n");
            return 0;
        }
        else
        {
            printf("Encoding done successfully\n");
        }
    }
    else if(ret == e_decode)
    {
        if(read_and_validate_decode_args(argv, &decInfo) == e_failure)
        {
            printf("Validation is wrong\n");
            return 0;
        }
        if(do_decoding(&decInfo) == e_failure)
        {
            printf("Decoding is failed\n");
            return 0;
        }
        else
        {
            printf("Decoding done successfully\n");
        }       
        return 0;
    }
    else
    {
        printf("Invalid Operation type\n");
        return 0;
    }
    

    return 0;
}
