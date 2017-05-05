#ifndef __BARCODE_PRINT_H__
#define __BARCODE_PRINT_H__

typedef struct {
	int width;
	int height;
	int bitsPerPixel;
	int size;
	unsigned char *data;
} BITMAP_T;

int PrnQRCode(const char* string, int length, int sizelevel, int correctionlevel);
int PrnPDF417(const char* string, int length, int sizelevel, int correctionlevel, int column);
int PrnGMCode(const char* string, int length, int sizelevel);
int PrnCode128(const char* string, int length, int height, int sizelevel);
int PrnCode39(const char* string, int length, int height, int sizelevel);
int PrnEAN13(const char* string, int height, int sizelevel);
int PrnEAN128(const char* string, int length, int height, int sizelevel);
int GetGMCode(const char* string, int length, int sizelevel, BITMAP_T * ptBitmap);
int GetQRCode(const char* string, int length, int sizelevel, int correctionlevel, BITMAP_T * ptBitmap);
int GetPDF417(const char* string, int length, int sizelevel, int correctionlevel, int column, BITMAP_T * ptBitmap);
int GetCode128(const char* string, int length, int height, int sizelevel, BITMAP_T * ptBitmap);
int GetCode39(const char* string, int length, int height, int sizelevel, BITMAP_T * ptBitmap);
int GetEAN13(const char* string, int height, int sizelevel, BITMAP_T * ptBitmap);
int GetEAN128(const char* string, int length, int height, int sizelevel, BITMAP_T * ptBitmap);
int DrawBarcode(int x, int y, BITMAP_T * ptBitmap);

#endif /* __BARCODE_PRINT_TEST_H__ */
