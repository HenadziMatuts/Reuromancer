#include <afxwin.h>
#include <stdint.h>
#include <neuro_routines.h>

#pragma pack(2)
typedef struct bmp_hdr_t {
    uint16_t type;
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t comp;
    uint32_t size_image;
    uint32_t xppm;
    uint32_t yppm;
    uint32_t colors_used;
    uint32_t colors_important;
} bmp_hdr_t;

typedef struct wav_header_t {
    char riff_header[4];    /* "RIFF" */
    int wav_size;           /* File size - 8 */
    char wave_header[4];    /* "WAVE" */
    char fmt_header[4];     /* "fmt " */
    int fmt_chunk_size;     /* 16 for PCM */
    short audio_format;     /* 1 for PCM */
    short num_channels;
    int sample_rate;
    int byte_rate;
    short sample_alignment; /* num_channels * byte_rate */
    short bit_depth;
    char data_header[4];    /* "data" */
    int data_bytes;
} wav_header_t;

extern uint8_t DosPal[1024];
extern bmp_hdr_t BmpHdrTemplate;
extern wav_header_t WavHdrTemplate;

BOOL Convert8bppTo32bpp(CBitmap *src, uint8_t *pal, CBitmap *dst);
int DecompressResource(FILE *f, resource_t *src, uint8_t *dst);

void StoreWaveAsFile(uint8_t *bytes, CString filename);
void StoreBitmapAsFile(CBitmap *bitmap, CString filename);
