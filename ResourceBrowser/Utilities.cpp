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

bmp_hdr_t BmpHdrTemplate = {
    0x4D42,
    0, 0,
    0x0436, 40, 0, 0,
    1, 8, 0, 0, 0,
    256, 256
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

void StoreWaveAsFile(uint8_t *bytes, CString filename)
{
    CFileDialog saveDlg(FALSE, NULL, filename, OFN_OVERWRITEPROMPT, NULL);
    wav_header_t *hdr = (wav_header_t*)bytes;

    if (saveDlg.DoModal() == IDOK)
    {
        FILE *f = NULL;
        char path[2048];
        CT2A fileName(saveDlg.GetFileName(), CP_UTF8);
        CT2A filePath(saveDlg.GetFolderPath(), CP_UTF8);

        sprintf(path, "%s\\%s", filePath.m_psz, fileName.m_psz);

        assert(f = fopen(path, "wb"));
        fwrite(bytes, 1, sizeof(wav_header_t) + hdr->data_bytes, f);
        fclose(f);
    }
}

void StoreBitmapAsFile(CBitmap *bitmap, CString filename)
{
    BITMAP bm;
    bmp_hdr_t hdr;
    uint8_t *bits = NULL;
    CFileDialog saveDlg(FALSE, NULL, filename, OFN_OVERWRITEPROMPT, NULL);

    bitmap->GetBitmap(&bm);
    assert(bits = new uint8_t[bm.bmWidth * bm.bmHeight]);

    memmove(&hdr, &BmpHdrTemplate, sizeof(bmp_hdr_t));
    hdr.size = (sizeof(bmp_hdr_t) + 1024 + (bm.bmWidth * bm.bmHeight));
    hdr.width = bm.bmWidth;
    hdr.height = bm.bmHeight;
    hdr.size_image = bm.bmWidth * bm.bmHeight;

    if (saveDlg.DoModal() == IDOK)
    {
        FILE *f = NULL;
        char path[2048];
        CT2A fileName(saveDlg.GetFileName(), CP_UTF8);
        CT2A filePath(saveDlg.GetFolderPath(), CP_UTF8);

        sprintf(path, "%s\\%s", filePath.m_psz, fileName.m_psz);
        bitmap->GetBitmapBits(bm.bmWidth * bm.bmHeight, bits);

        assert(f = fopen(path, "wb"));
        fwrite(&hdr, 1, sizeof(bmp_hdr_t), f);
        fwrite(DosPal, 1, 1024, f);

        for (int i = bm.bmHeight - 1; i >= 0; i--)
        {
            fwrite(&bits[i * bm.bmWidth], 1, bm.bmWidth, f);
        }
        fclose(f);
    }

    delete[] bits;
}
