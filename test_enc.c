#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include "opus.h"

typedef unsigned char  uchar;

uchar* map_file(const char* fn, size_t* size) {
    FILE* fp = fopen(fn, "r");
    if(!fp) {
        printf("fail to open file: %s", fn);
        return (uchar*)MAP_FAILED;
    }

    fseek(fp, 0L, SEEK_END);
    long pos = ftell(fp);
    uchar* ptr = (uchar*)mmap(NULL, pos, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileno(fp), 0);
    *size = pos;

    fclose(fp);
    return ptr;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("usage: %s <pcm_file>\n", argv[0]);
        return -1;
    }

    size_t file_size = 0;
    int16_t* input = NULL;
    int ret = 0;
    int sample_rate = 16000;
    int frame_duration_ms = 20;
    int size;
    int frame_size = 0;
    OpusEncoder *encoder = NULL;

    size = opus_encoder_get_size(2); /* max 2 channels */
    printf("size = %d\n", size);

    encoder = malloc(size);
    if (!encoder) {
      printf("fail to create encoder");
      return -1;
    }

    ret = opus_encoder_init(encoder, sample_rate, 1, OPUS_APPLICATION_VOIP);
    if (ret != 0) {
      printf("fail to init %d", ret);
      return ret;
    }

    /* set parameters */
    ret = opus_encoder_ctl(encoder, OPUS_SET_BITRATE(256000));
    if (ret != 0) {
        printf("fail to set bitrate");
        return ret;
    }

    ret = opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(5));
    if (ret != 0) {
      printf("fail to set complexity");
      return ret;
    }

    ret = opus_encoder_ctl(encoder, OPUS_SET_VBR(0));
    if (ret != 0) {
      printf("fail to set vbr");
      return ret;
    }

    ret = opus_encoder_ctl(encoder, OPUS_SET_SIGNAL(3001));
    if (ret != 0) {
      printf("fail to set signal");
      return ret;
    }

    uchar* ptr = map_file(argv[1], &file_size);
    if (ptr == MAP_FAILED) {
        printf("fail to map file");
        return -1;
    }

    input = (int16_t*)ptr;
    uchar* out = (uchar*)malloc(file_size);
    FILE* fp = fopen("enc.raw", "w");
    if (!fp) {
        printf("fail to open file out.opus");
        return -1;
    }

    frame_size = sample_rate/1000*frame_duration_ms;

    while((uchar*)input < ptr + file_size) {
        ret = opus_encode(encoder, input, frame_size, out, frame_size);
        if (ret <= 0) {
            printf("fail to encode");
            break;
        }

        fwrite(out, 1, ret, fp);
        input += frame_size;
    }
    fclose(fp);

    munmap(ptr, file_size);
    free(encoder);

    printf("enc: done\n");

    return 0;
}
