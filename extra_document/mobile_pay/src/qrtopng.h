/*
 ============================================================================
 Name        : qrtopng.h
 Author      : PAX
 Version     : 
 Copyright   : PAX Computer Technology(Shenzhen) CO., LTD
 Description : PAX POS Shared Library
 ============================================================================
 */
 
 
#ifndef SHARED_LIBRARY_H_qrtopng
#define SHARED_LIBRARY_H_qrtopng

#ifdef __cplusplus
extern "C"
{
#endif


int qrToPNG(QRcode *qrcode, const char *outfile, int margin_val, int size_val);


#ifdef __cplusplus
};
#endif

#endif /* SHARED_LIBRARY_H_qrtopng */

