#include "stdafx.h"
#include "Utilities.h"
#include <stdio.h>

uint8_t DosPal[1024] = {
    0x00, 0x00, 0x00, 0x00, // black
    0x80, 0x00, 0x00, 0x00, // blue
    0x00, 0x80, 0x00, 0x00, // green
    0x80, 0x80, 0x00, 0x00, // cyan
    0x00, 0x00, 0x80, 0x00, // red
    0x80, 0x00, 0x80, 0x00, // magnetta
    0x00, 0x80, 0x80, 0x00, // brown
    0xC0, 0xC0, 0xC0, 0x00, // light gray
    0x80, 0x80, 0x80, 0x00, // dark gray
    0xFF, 0x00, 0x00, 0x00, // light blue
    0x00, 0xFF, 0x00, 0x00, // light green
    0xFF, 0xFF, 0x00, 0x00, // light cyan
    0x00, 0x00, 0xFF, 0x00, // light red
    0xFF, 0x00, 0xFF, 0x00, // light magnetta
    0x00, 0xFF, 0xFF, 0x00, // yellow
    0xFF, 0xFF, 0xFF, 0x00, // white
    0x00,
};

wav_header_t WavHdrTemplate = {
    {'R', 'I', 'F', 'F'},
    0,
    {'W', 'A', 'V', 'E'},
    {'f', 'm', 't', ' '},
    16, 1, 1, 44100, 44100, 1, 8,
    {'d', 'a', 't', 'a'},
    0
};

BOOL Convert8bppTo32bpp(CBitmap *src, uint8_t *pal, CBitmap *dst)
{
    BITMAP bm8bpp;
    uint8_t bpp8bits[64000] = { 0, }, *bpp32bits = NULL, *p8 = bpp8bits, *p32 = NULL;
    int bpp8size = 0;

    src->GetBitmap(&bm8bpp);
    bpp8size = bm8bpp.bmWidth * bm8bpp.bmHeight;
    assert(bpp32bits = new uint8_t[bpp8size * 4]);
    if (!bpp32bits)
    {
        return 0;
    }
    src->GetBitmapBits(bpp8size, bpp8bits);

    p32 = bpp32bits;
    for (int i = 0; i < bpp8size; i++)
    {
        int c = *p8++;
        memmove(p32, &pal[c * 4], 4);
        p32 += 4;
    }

    dst->CreateBitmap(bm8bpp.bmWidth, bm8bpp.bmHeight, 1, 32, bpp32bits);
    delete[] bpp32bits;

    return TRUE;
}

int DecompressResource(FILE *f, resource_t *src, uint8_t *dst)
{
    uint8_t compd[64000];

    if (strstr(src->name, ".PIC") || strstr(src->name, ".IMH"))
    {
        fseek(f, src->offset + 32, SEEK_SET);
    }
    else
    {
        fseek(f, src->offset, SEEK_SET);
    }
    fread(compd, 1, src->size, f);

    if (strstr(src->name, ".IMH"))
    {
        return decompress_imh(compd, dst);
    }
    else if (strstr(src->name, ".PIC"))
    {
        return decompress_pic(compd, dst);
    }
    else if (strstr(src->name, ".BIH"))
    {
        return decompress_bih(compd, dst);
    }
    else
    {
        return decompress_anh(compd, dst);
    }
}
