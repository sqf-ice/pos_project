#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "data_define_salesman.h"
#include "framework.h"

#include "calc_salesman.h"
#include "connect.h"
#include "LHBXML.h"
#include "zlib/zlib.h"

#include "Utf8ToAr.h"
#include "WlExt.h"

unsigned char *key1;

static unsigned char embedded_keys[][16] = 
{
	{0x0E,0xFF,0xD5,0x54,0xAC,0x6C,0x90,0x4B,0x84,0x3D,0xD2,0xCC,0x4B,0x19,0xE8,0x7B,},
	{0x05,0x0F,0xF6,0x9D,0xE3,0x97,0xB1,0x39,0x63,0x44,0x16,0xED,0xCD,0xC0,0x71,0x2D,},
	{0x5C,0x51,0x96,0xED,0xCD,0xAD,0x89,0x46,0xB0,0xAF,0x64,0x75,0xF6,0x7D,0x01,0x47,},
	{0x51,0xC8,0x7E,0x39,0xDC,0x1A,0xDB,0x76,0x4E,0x9B,0xF1,0x68,0x56,0xA3,0x7E,0xC4,},
	{0xCB,0xA5,0x6F,0xF9,0x82,0xFB,0xAE,0x58,0x85,0xAF,0xED,0x33,0x33,0x3B,0x43,0xE5,},
	{0x42,0xCE,0x9B,0x04,0x41,0x37,0x50,0x42,0x76,0x20,0x9E,0x31,0xBC,0x21,0xF0,0x0C,},
	{0x31,0x9F,0xAA,0x51,0x0D,0x07,0xC4,0x56,0x1A,0x26,0xB4,0x2D,0x2C,0xA7,0x42,0x12,},
	{0x58,0x0B,0x25,0x87,0x08,0x7A,0x83,0x94,0x54,0x2D,0x82,0x62,0x5C,0x7C,0x9B,0x64,},
	{0xBE,0xE1,0x02,0x93,0x91,0xF1,0xD7,0x3B,0x09,0x45,0xEA,0x1A,0x82,0x23,0x16,0xCE,},
	{0xDD,0x8C,0xD9,0xB0,0xFB,0x69,0x04,0xB7,0x1C,0x15,0xFC,0xE0,0x85,0x62,0xFA,0x9D,},
	{0x8B,0x50,0x6E,0x4B,0xA5,0xAF,0x81,0x66,0x34,0xEA,0x7D,0xC2,0x8F,0x68,0x11,0x30,},
	{0x16,0xB3,0x85,0xCF,0xA2,0xEA,0x68,0x55,0xDC,0x4B,0x76,0x7C,0xF4,0xF6,0xB5,0xA7,},
	{0x4A,0x4C,0x8F,0x32,0xB5,0xCB,0x29,0x2D,0x0B,0x73,0x7C,0x11,0xEB,0x41,0x27,0xB3,},
	{0x03,0xCC,0xC9,0xD1,0xDD,0x2A,0xFA,0xA0,0xE7,0x18,0x6E,0x5A,0x38,0x04,0x71,0x3D,},
	{0x34,0xD6,0x78,0x43,0xD9,0xDE,0x7B,0x4B,0x8A,0x57,0xC4,0x2F,0x6E,0x57,0x69,0x9B,},
	{0x23,0xC2,0x78,0x28,0x2D,0xE4,0xC9,0x1F,0x51,0xEE,0x4B,0x9C,0x5C,0xCE,0xF4,0x06,},
	{0x01,0x3F,0xDC,0x15,0xC9,0xE4,0xDA,0x24,0x50,0x7D,0x46,0x85,0x66,0xCA,0x32,0x90,},
	{0xAF,0xD2,0x36,0x75,0x8C,0x48,0x10,0x4F,0x0C,0x58,0xF1,0xEB,0x93,0xDE,0xEF,0xBB,},
	{0x62,0xBD,0x2E,0x2F,0x7A,0x54,0xD2,0x3F,0xE8,0x53,0xD7,0xAF,0x9D,0x44,0xB9,0x87,},
	{0xBB,0xBB,0xED,0x8A,0x98,0x7F,0x79,0xB8,0x2E,0x8C,0x09,0x70,0xF3,0xEE,0x91,0x71,},
	{0x8E,0x8B,0x23,0xA8,0x9C,0x03,0x15,0x5A,0x00,0xF7,0x8D,0x38,0x70,0x60,0xB2,0xC8,},
	{0x9E,0x69,0x30,0xD8,0xF1,0x65,0x23,0xE3,0x43,0x75,0x00,0xED,0x4B,0x61,0x4B,0xFA,},
	{0xC2,0x66,0x20,0x12,0x21,0x10,0xEA,0xB7,0xA5,0x92,0x89,0xA0,0xF1,0x40,0x47,0xCD,},
	{0xDA,0x41,0xD1,0x89,0xBE,0xB8,0x7F,0x29,0x67,0x3F,0x64,0x26,0x03,0x28,0x0F,0x9E,},
	{0xAB,0x93,0x76,0x0A,0x40,0x9F,0xA7,0x8D,0x7F,0x78,0x33,0x5F,0xDB,0x82,0xF1,0x02,},
	{0x2D,0x32,0x12,0x09,0x8F,0xC2,0x3B,0x09,0xE6,0xF7,0xAB,0xE1,0xAC,0xC4,0xB4,0x7C,},
	{0x82,0xEB,0xF4,0x5E,0xCA,0x3D,0x8D,0x3B,0x9A,0x43,0x0A,0x46,0x85,0x20,0x4E,0xE5,},
	{0x4D,0x61,0xF9,0xD0,0x16,0x06,0x0E,0xE9,0x57,0x6C,0x24,0x48,0xF4,0xC5,0xE1,0xBA,},
	{0xC3,0xB6,0xDD,0xEF,0x8D,0x93,0x38,0xF0,0xA4,0xD7,0x6F,0x4C,0xB7,0xF5,0x83,0xC5,},
	{0xA9,0xC5,0x93,0x4A,0xCA,0xE3,0x66,0x7C,0xDD,0x0A,0x06,0x5D,0x18,0x45,0x47,0xD4,},
	{0x61,0x4F,0x82,0xCF,0x42,0x5E,0x57,0xBA,0x29,0x08,0x2C,0x3F,0x96,0xC2,0x32,0x62,},
	{0xE9,0x5E,0x9E,0x2E,0x31,0x97,0xF0,0xE0,0x4F,0x61,0x12,0x82,0x8C,0xDA,0xD7,0x5F,},
	{0x8F,0x9B,0xEA,0x67,0x73,0xB4,0x82,0x4B,0x6E,0xD9,0x20,0xC2,0xB7,0x27,0xE5,0x1A,},
	{0xD8,0xA4,0x74,0xB9,0xA6,0x18,0xA2,0xCA,0x5F,0xA1,0x7F,0xE1,0x4B,0xD4,0x2E,0x4E,},
	{0x31,0xCC,0xAF,0xFB,0xD8,0xCA,0xEF,0x34,0xDB,0x53,0xBF,0x58,0x77,0x2D,0x28,0x27,},
	{0x7B,0xF7,0xAA,0xE7,0x66,0x1D,0xB9,0x77,0xF8,0x20,0x08,0xD1,0x34,0xA1,0xFF,0x9A,},
	{0xAB,0xD1,0x90,0xEA,0x78,0xEA,0x42,0x0D,0xB5,0x3F,0xDD,0x52,0x59,0x4A,0x52,0x31,},
	{0x79,0x57,0x66,0x53,0x2A,0x46,0xE8,0x13,0x13,0x38,0x02,0xB8,0xC2,0x1D,0xFF,0x3A,},
	{0x53,0x25,0xAD,0xA5,0x9C,0x3D,0xAF,0x90,0xB5,0x9C,0x32,0x26,0x40,0x66,0x1D,0xAE,},
	{0xCD,0xD6,0x58,0xDD,0x3A,0xAC,0x70,0x65,0xE9,0x7D,0xD7,0x40,0xE7,0x8A,0x4A,0x97,},
	{0x40,0xA3,0xFE,0x33,0x9D,0xAC,0x94,0x3B,0xA8,0xD5,0x9E,0x8D,0x14,0xC1,0xD3,0xAC,},
	{0x1A,0x54,0xEA,0xEC,0xC5,0x22,0xFF,0xA7,0xB5,0x48,0xBC,0x1F,0x66,0x6F,0xA8,0x2B,},
	{0x52,0x9A,0xE6,0x9D,0x67,0xEA,0x11,0x20,0x41,0x1C,0x34,0x65,0x0E,0xE3,0x50,0x27,},
	{0xAA,0x2A,0x26,0x93,0x9F,0x5E,0xD1,0x1D,0xA3,0x57,0x7D,0x54,0xF0,0x75,0xD4,0x38,},
	{0x2D,0x73,0x3A,0x79,0xDB,0x36,0x45,0x2F,0x43,0xC9,0xAA,0xA8,0xED,0x9A,0x99,0xD2,},
	{0xF7,0x94,0x74,0x3C,0x49,0x15,0x8B,0xF4,0xC8,0xC9,0xF9,0x10,0x6C,0xBC,0x30,0xCC,},
	{0x48,0xFB,0x37,0x9B,0xA8,0xD7,0x4C,0x74,0xE5,0xF6,0x50,0xD5,0xB3,0x28,0x2F,0x08,},
	{0xBD,0xAB,0x5D,0x5B,0x00,0x98,0xEF,0x17,0xE7,0x46,0xB0,0x12,0x3C,0x34,0xC2,0x87,},
	{0x99,0xDE,0x60,0xF1,0x63,0xFE,0x2A,0x1F,0x48,0xFF,0x56,0x52,0xC5,0xB5,0x41,0x9F,},
	{0x99,0xF1,0x52,0xC1,0x5E,0xD4,0xCB,0xAF,0x0C,0x1F,0xB3,0x61,0x26,0x3A,0x38,0xB0,},
	{0x33,0xC8,0x3C,0xD4,0xC7,0x3D,0xA5,0xBF,0x81,0x3D,0x13,0x01,0x41,0x9C,0x52,0x31,},
	{0x05,0x10,0x78,0x4A,0xE2,0x01,0x28,0xC4,0x13,0xB1,0xE5,0x43,0x4A,0xC8,0xF9,0x0D,},
	{0xC7,0xA6,0x1F,0xFC,0x79,0x6C,0xD7,0x5C,0x4C,0x93,0x40,0x20,0xC1,0xCA,0x6D,0x6F,},
	{0xCD,0x81,0x5C,0x15,0xC2,0xED,0xCE,0x31,0xA0,0x77,0xC1,0x16,0x57,0x62,0x74,0xB8,},
	{0x44,0x05,0x39,0x9B,0x7D,0x56,0xB8,0x99,0x38,0x1C,0xFE,0x4E,0xC1,0x08,0x03,0x49,},
	{0x43,0x76,0x75,0xC9,0x6C,0xE3,0x4B,0xD9,0xD8,0xED,0x5B,0x01,0xAB,0xC1,0x04,0xEA,},
	{0x1E,0x87,0xDB,0xBB,0x90,0xD2,0xF2,0x76,0xC9,0xC3,0xDC,0xA6,0xEF,0x7F,0x70,0x98,},
	{0x74,0x77,0xD3,0xE1,0xEC,0xAB,0x4C,0x61,0xB9,0x5F,0xC2,0x90,0xFA,0xB6,0xED,0x77,},
	{0x17,0x13,0x30,0x3A,0x2C,0x47,0xB8,0x70,0x84,0xFF,0x00,0xA9,0x06,0xA5,0xE9,0x59,},
	{0x7C,0xC3,0x8D,0x33,0xED,0x22,0xFE,0x73,0x3A,0x7A,0x02,0x30,0x10,0x61,0x7B,0x12,},
	{0xD5,0x65,0x8E,0xDC,0x4E,0x36,0x6B,0x2C,0xBF,0x62,0x5D,0xEC,0xAB,0xA3,0x5B,0x14,},
	{0x76,0x69,0x13,0x37,0x28,0xBA,0x89,0xF4,0x15,0xBD,0xFF,0xB2,0x45,0x87,0x60,0x22,},
	{0x83,0xED,0xD3,0xA0,0x17,0x0D,0x0A,0x46,0x57,0x61,0x74,0x8B,0xC5,0x11,0xC8,0x97,},
	{0x1E,0xED,0xBA,0xA6,0x4E,0x44,0xE9,0x1B,0xDB,0x99,0xC6,0x91,0x37,0xB9,0xE6,0x37,},
	{0x02,0x3B,0x68,0xBD,0xCF,0xBA,0xFC,0x68,0x5E,0xCC,0xDF,0xC7,0x45,0xFE,0x2F,0x04,},
	{0xF4,0x67,0x93,0x57,0xCD,0xB9,0xD3,0x3F,0x04,0xB8,0xC5,0x7D,0x46,0x78,0x0F,0x9E,},
	{0x7B,0x3D,0x89,0x89,0xA8,0x0B,0xC8,0x64,0xF1,0x51,0x48,0x81,0x65,0x21,0xD5,0xDD,},
	{0x6A,0xC2,0x9C,0xCF,0x25,0x8E,0x69,0x48,0x67,0x29,0xB8,0x16,0xF5,0x7F,0x21,0xE6,},
	{0xBD,0x40,0x4C,0x08,0x75,0xA5,0x5B,0xE9,0x9E,0x2F,0x66,0x7B,0x71,0x51,0xDA,0xE2,},
	{0xE3,0xE7,0x13,0x84,0xDB,0x3F,0x79,0xEC,0xD3,0x40,0x22,0x7E,0x52,0xB2,0x6F,0x71,},
	{0x2F,0x61,0xC4,0x24,0xD5,0xCC,0x68,0xC4,0x66,0xB3,0x31,0xF6,0x28,0xED,0xD6,0x16,},
	{0x4F,0xD5,0x32,0x0D,0x6C,0x9D,0x0A,0x7E,0xC7,0x60,0x35,0x34,0x83,0xDD,0x96,0xEC,},
	{0xBF,0xF4,0x4B,0x0F,0x8D,0xB5,0x0D,0x17,0x7F,0x4E,0x1D,0x28,0x0E,0x82,0xC4,0xE5,},
	{0x6A,0x0C,0x2F,0x7C,0x7F,0x61,0x05,0x92,0x57,0x46,0x5D,0x75,0x97,0x34,0x41,0x89,},
	{0x1C,0x44,0x36,0x33,0xA2,0xC7,0x88,0x99,0xC8,0x1B,0x8E,0x1D,0x7F,0xE1,0x6F,0x44,},
	{0x9B,0x4F,0xCA,0x77,0xAD,0x19,0x6D,0x5E,0xEF,0xB9,0xB8,0x46,0x58,0x97,0x7F,0x4A,},
	{0x0D,0x94,0x0B,0xCA,0x4C,0x90,0x43,0xA3,0x83,0x96,0xB4,0x63,0xD2,0xE7,0x12,0x75,},
	{0xD6,0x18,0xD9,0x47,0x6A,0xC9,0xDC,0x02,0xD6,0xD8,0x43,0xE8,0x0F,0x42,0x41,0x31,},
	{0x9B,0x44,0x10,0xEB,0xD5,0xA5,0xF8,0x0B,0x0B,0x27,0xF4,0x17,0x65,0x09,0x8B,0x07,},
	{0xA7,0x42,0x70,0x3F,0xBD,0x38,0xE0,0x43,0xB8,0x09,0xF3,0x74,0x4C,0x5B,0x93,0x40,},
	{0x54,0x82,0xC8,0x8E,0xD7,0x26,0x9F,0x30,0x20,0xCE,0x3C,0x43,0xD5,0xC4,0x9C,0xCC,},
	{0x4C,0x3C,0x41,0xCC,0xD1,0x01,0x73,0x63,0x63,0xCC,0x33,0xCC,0xB2,0x84,0xBE,0xB0,},
	{0x93,0xB3,0xC2,0xE2,0x41,0xFC,0x2F,0x80,0xA2,0x3F,0x1E,0xEF,0xFB,0x9D,0x6E,0xF3,},
	{0x6C,0x26,0x27,0x10,0x1A,0xC7,0x6F,0x52,0x0F,0x64,0x2B,0x0A,0xF3,0xD3,0x7E,0x9C,},
	{0x10,0x8B,0xB0,0x21,0x01,0xB9,0x8B,0x50,0x0C,0xC0,0x2A,0x92,0xE3,0x84,0x67,0xC6,},
	{0x5E,0x3F,0x5F,0x8B,0x5D,0xAD,0x96,0xCB,0x44,0x70,0xD3,0xDC,0xD1,0x25,0x8C,0xCF,},
	{0xC0,0x82,0xDE,0x70,0xED,0x4C,0xF1,0xB2,0x9B,0xD6,0x03,0x59,0x9C,0xDC,0x6F,0x8B,},
	{0xFD,0x64,0xAC,0xB4,0x73,0xF2,0x3F,0x62,0xF5,0xE4,0xDD,0x91,0xE6,0xAE,0x7F,0xE7,},
	{0x34,0x5B,0xD1,0x60,0xA5,0xF7,0xD9,0x11,0x4D,0xAC,0xF0,0x86,0xA9,0x3D,0x2F,0x67,},
	{0x00,0x47,0xDF,0x6C,0x78,0xD8,0x5B,0xCE,0x43,0x7C,0x7A,0x08,0x6F,0x5F,0xCC,0xEC,},
	{0x57,0x1C,0x24,0x41,0x0B,0x0E,0xE6,0x05,0x8B,0x9A,0x66,0xC6,0x02,0xB9,0xB8,0xB8,},
	{0xB5,0x75,0x91,0x9F,0xCF,0x12,0x6E,0x5C,0xFF,0x03,0x1D,0x28,0x7C,0xD0,0xA2,0xED,},
	{0xE0,0x04,0x2E,0x8D,0x18,0xED,0x5C,0xF1,0x61,0xD9,0xE1,0xDD,0x71,0x94,0xCB,0x8A,},
	{0xC3,0xC2,0x53,0xF9,0xCE,0x2C,0x1C,0x45,0xAD,0xC8,0x41,0x08,0xF1,0xBD,0x89,0xBA,},
	{0xF8,0x78,0xE6,0xFE,0x8B,0x2B,0x9F,0xC7,0x19,0x9F,0x3E,0xF5,0x30,0x90,0xEA,0x6B,},
	{0x37,0x93,0x64,0xB9,0x7C,0xA8,0x1F,0xB3,0xE7,0xE8,0xDF,0x50,0xAB,0x53,0x00,0xA3,},
	{0x4F,0x27,0xE5,0xF9,0xAE,0xCD,0xFF,0x98,0x23,0x8C,0xE0,0x2E,0xEE,0x26,0x14,0x1C,},
	{0xDD,0xD6,0xD0,0x5E,0x58,0xCF,0x3D,0x50,0x59,0x32,0x16,0xA2,0x4C,0x4E,0x1B,0xC5,},
	{0x9F,0x9A,0xA6,0x09,0xAB,0xF1,0xBC,0x69,0x94,0xCC,0x96,0xF0,0xC7,0xF0,0xC5,0xC0,},
	{0xD3,0xC2,0x72,0xD5,0x82,0xA1,0x4A,0x7F,0xE9,0x95,0xA0,0x7C,0xB6,0x5C,0x4C,0xD4,},
	{0x70,0x1A,0x8F,0x53,0x80,0x0B,0x71,0xEC,0x19,0x66,0xCA,0x8C,0x39,0x4D,0x88,0x68,},
	{0x6C,0x55,0xCE,0x89,0xDC,0x45,0x42,0xFD,0x50,0x26,0x1F,0x16,0x51,0x40,0xB2,0x37,},
	{0x80,0xC1,0x7F,0xAC,0x61,0x71,0x3F,0xC8,0x51,0x86,0xBB,0x1B,0xD7,0xBE,0x46,0x9C,},
	{0x0C,0x37,0x0F,0x61,0x8D,0xC2,0xF8,0x2B,0xD8,0x91,0x03,0x5A,0xEB,0x5A,0x9A,0xD9,},
	{0xB4,0x92,0x0B,0x77,0xCD,0xDB,0x07,0xFB,0x16,0x06,0x37,0x55,0x89,0x26,0x25,0x92,},
	{0xDF,0x83,0x09,0x1A,0x30,0x51,0xAA,0xBD,0xC9,0x54,0xD4,0xCF,0x6E,0xE4,0xDC,0xF7,},
	{0xEC,0xE2,0x67,0x45,0xB2,0xCE,0xCE,0x30,0x2F,0x14,0x4A,0x86,0x8A,0xA3,0xA1,0xB1,},
	{0x6B,0x71,0x7B,0x68,0x50,0x80,0xA8,0x62,0xE8,0x4D,0xAE,0x79,0x07,0x38,0x8D,0x31,},
	{0x2A,0x40,0x68,0xA7,0xBF,0xE0,0xDF,0x11,0xB5,0xD9,0xEA,0x1C,0x07,0x23,0x32,0x26,},
	{0xB3,0x23,0x63,0xFC,0x69,0x74,0xA6,0x6B,0x3A,0x05,0x5D,0x65,0x20,0x54,0x29,0xF7,},
	{0x86,0x19,0x5B,0x4A,0x3A,0xF9,0xF2,0x8B,0x91,0x69,0xB3,0x77,0x95,0x8B,0x5A,0xC4,},
	{0x1A,0x87,0xC6,0xE3,0xD4,0x90,0xC7,0xB6,0x9A,0xFD,0x6C,0xB9,0x82,0x7A,0xF0,0x60,},
	{0xAF,0xF3,0x95,0xED,0xC7,0xE8,0xEC,0xB9,0x31,0xD8,0x6B,0x00,0xC0,0x2F,0xD5,0xAE,},
	{0xAA,0x99,0x4A,0x98,0x8E,0x46,0x22,0xF6,0x52,0xA5,0xAF,0x3D,0x8D,0x38,0x08,0x17,},
	{0x78,0x53,0xC8,0xA2,0xF6,0x6D,0xFC,0x32,0x9B,0x0C,0xB2,0xFC,0x25,0xC6,0xF5,0x6B,},
	{0x27,0xBE,0x0E,0xE9,0x2F,0x58,0xD1,0x68,0xD0,0x9F,0x9B,0x22,0x38,0xAE,0x2C,0xF8,},
	{0xCF,0x53,0x87,0x6D,0x66,0x33,0xB8,0x3D,0x9C,0x18,0xFD,0x81,0x93,0x25,0xD1,0x8B,},
	{0xAF,0x44,0x16,0xCF,0x2A,0x1E,0xE8,0x20,0x81,0x5F,0xD1,0x57,0xD2,0xDA,0xF9,0xEC,},
	{0xE0,0xBA,0x2D,0xD2,0x3F,0x87,0x04,0xF4,0x05,0xD0,0x4D,0x16,0x5A,0x4F,0xD5,0xAA,},
	{0xCD,0x0F,0x72,0xF4,0x98,0x5C,0xCD,0xB0,0x6A,0x66,0xEC,0x07,0x61,0x61,0x6B,0xC2,},
	{0x1F,0x10,0xA0,0xD9,0xF8,0xFF,0x4B,0x28,0x40,0x22,0xC9,0x59,0xD1,0xD1,0xC6,0x31,},
	{0x8A,0x26,0x89,0xC2,0x67,0x04,0x54,0x83,0xEC,0x7C,0x67,0xD9,0x9B,0xFC,0x92,0xA4,},
	{0x65,0x8A,0xF7,0x4A,0xAF,0xFB,0xB2,0xCE,0xC4,0x4A,0xB4,0x76,0x93,0x73,0xDE,0x99,},
	{0xA0,0xBB,0x7D,0x3A,0xD7,0xA8,0x5A,0x1A,0xBC,0x1D,0xED,0x7B,0x98,0x72,0x8C,0x43,},
	{0x8F,0x25,0xF4,0x47,0xD9,0x7B,0x82,0x8E,0x74,0xBB,0xFE,0x1D,0x50,0xDD,0xF0,0x78,},
	{0x9E,0xCD,0x39,0xE0,0x7F,0x58,0xCC,0xBC,0x2F,0xDD,0x8E,0x92,0x0E,0x36,0x36,0x51,},
	{0xBD,0x70,0xE7,0x6E,0x7F,0x97,0xBF,0x24,0x38,0xEE,0xC5,0x1C,0xAA,0x3D,0xC9,0xDE,},
	{0x2C,0x74,0xDD,0xCA,0x38,0xAC,0xC3,0x36,0x58,0x37,0x17,0xFC,0xED,0xF0,0x80,0x3D,},
	{0x84,0xAC,0x5F,0xDF,0xF6,0x82,0xF9,0x06,0x0D,0x14,0xA7,0x91,0x92,0xB0,0x12,0xF2,},
	{0x13,0xB0,0x65,0x99,0x58,0x6B,0x18,0x11,0xAD,0xB0,0x15,0x47,0x80,0xC3,0x37,0x48,},
	{0x6D,0x87,0x97,0xFE,0xDE,0xE9,0xE7,0xAA,0x7B,0xD7,0xBD,0x24,0x05,0x32,0xA4,0xA8,},
	{0x25,0x25,0xE9,0x3D,0x8A,0xE7,0xE5,0x19,0x93,0x4E,0x32,0x46,0x6C,0xB1,0x86,0x4A,},
	{0xDD,0x5E,0x23,0x0A,0x18,0x13,0x14,0x29,0xC8,0xBB,0x1D,0x81,0xAE,0xF4,0x0B,0x7B,},
	{0x0B,0x6D,0x21,0x5B,0xDA,0xFA,0xBD,0x85,0xA2,0x7E,0x6A,0x8F,0xB7,0xA0,0x3D,0xCA,},
	{0x50,0x72,0x46,0xA6,0x0E,0x99,0x8F,0x11,0x10,0xBF,0x74,0xCF,0xCA,0x63,0x6A,0x9E,},
	{0x42,0xB6,0x3A,0xFF,0x8F,0x2B,0x70,0xE3,0xA1,0x9D,0x68,0x99,0xA9,0x84,0x72,0x27,},
	{0x7D,0x52,0x31,0x3B,0xAE,0x65,0x6C,0x72,0xDD,0x10,0x4A,0x1A,0x85,0xEE,0xBB,0x3F,},
	{0x13,0xCB,0x4D,0x00,0x65,0x3E,0x3A,0x24,0x02,0x63,0x05,0x70,0x31,0xF5,0x68,0x8C,},
	{0x4D,0x4C,0x51,0x77,0x31,0x2F,0xF0,0xDD,0x4A,0xC8,0x21,0x7F,0xE1,0x55,0x5D,0xB2,},
	{0xE1,0x28,0x86,0xDD,0x37,0x71,0xC2,0x3F,0x18,0x0C,0x39,0x4A,0x60,0xFB,0x40,0xE7,},
	{0xA7,0xBE,0xCD,0x39,0x9C,0x44,0x0D,0xE4,0xDB,0x6F,0xB6,0x9C,0x1D,0xBB,0x43,0xAC,},
	{0x06,0x52,0x8F,0x08,0x6F,0x8A,0x54,0xDC,0x8A,0x12,0x5F,0x25,0xF5,0xA5,0x7D,0x4E,},
	{0x88,0x39,0xD7,0x23,0x88,0x23,0x16,0x9D,0x1A,0xA1,0x02,0xC7,0xBA,0x25,0x22,0x5C,},
	{0xC9,0x8C,0x44,0x0D,0x6C,0x37,0x77,0x1C,0x69,0xF0,0xAF,0x8A,0xE5,0xE8,0xEF,0x35,},
	{0xF2,0x68,0x14,0xE3,0xE3,0x20,0x24,0xE2,0x3C,0xF1,0xED,0xCE,0x88,0x8C,0x21,0x65,},
	{0xEF,0x02,0x5C,0x69,0x61,0x6F,0xE1,0x50,0xC1,0xBB,0xB9,0x97,0x74,0x8B,0xB8,0x32,},
	{0xB2,0xCD,0xD4,0x52,0x10,0x3F,0x78,0xA8,0x68,0x62,0xD1,0x7B,0x49,0xA9,0x94,0xDC,},
	{0xCF,0x49,0x9B,0xE2,0x8B,0x85,0xD3,0x83,0x86,0xC3,0x21,0x28,0x66,0x8C,0x6D,0xB0,},
	{0x39,0xBB,0x1C,0xBB,0xB7,0xCE,0x55,0xB0,0x89,0xB3,0x00,0xFE,0x9E,0x66,0x07,0x18,},
	{0x26,0xDC,0xC8,0x2A,0xF0,0xD5,0xA6,0x2C,0xF9,0x51,0xCB,0x4C,0x1D,0x50,0x41,0xDC,},
	{0xF4,0x20,0x41,0x8E,0x8C,0xD9,0x25,0x00,0xF4,0xEA,0x58,0xD8,0x85,0xD5,0xE5,0x20,},
	{0xBC,0x09,0x01,0x8A,0xC4,0x42,0xC0,0x63,0x14,0xEF,0xC1,0x66,0x02,0x08,0x93,0x39,},
	{0x71,0x24,0x09,0x7D,0x24,0x78,0x8B,0x4D,0xB7,0xC7,0x36,0x05,0x7D,0xCE,0xA6,0x24,},
	{0x6C,0xAB,0x8F,0x13,0x4E,0xB7,0x17,0xB2,0x60,0xDE,0x79,0x9D,0x17,0xBE,0xFD,0xEC,},
	{0x57,0x1E,0x66,0x61,0xCB,0x67,0xFB,0x44,0x7D,0xB3,0xD2,0xFD,0x0C,0xBA,0xE6,0x45,},
	{0x49,0x9D,0xFA,0x57,0x79,0x7F,0xEB,0xB9,0x0C,0x57,0x1D,0x46,0xFE,0xEE,0x36,0xF8,},
	{0xC9,0x4A,0xAE,0x5E,0x67,0x45,0x58,0x04,0xAF,0x4B,0x95,0x9A,0xE5,0x1A,0xA5,0xA5,},
	{0xF7,0x33,0x53,0x79,0xC4,0xA6,0x73,0xD7,0x33,0xAC,0x54,0x73,0x4A,0xB5,0x45,0x8F,},
	{0x7C,0xD9,0x34,0xC2,0x0B,0x87,0xDE,0x03,0x44,0xF5,0xA7,0x2D,0xE9,0x50,0xD4,0xF1,},
	{0xB1,0x77,0x4E,0xCA,0x9A,0xBD,0x34,0x22,0xD6,0xD7,0x2B,0xBB,0xF0,0xE5,0xBA,0xFA,},
	{0x06,0x06,0x1E,0x0B,0xE6,0xE3,0x9A,0x70,0x55,0x50,0x34,0xCE,0xFE,0x45,0xED,0xE3,},
	{0xD8,0x62,0xC2,0xD2,0x67,0x7A,0xF5,0xBF,0xA5,0x11,0xEB,0x34,0x5C,0x23,0x12,0x5F,},
	{0x3B,0xAD,0x27,0xE3,0x32,0xC8,0x0B,0xCE,0xDD,0xDE,0x75,0x49,0x88,0x65,0x24,0x91,},
	{0xE9,0x47,0xE2,0x8D,0x22,0xE8,0x92,0x8C,0x11,0x5E,0x66,0xD5,0xF7,0x65,0x13,0x32,},
	{0x3D,0xD5,0x93,0x50,0x1E,0x0A,0xB1,0x0F,0x57,0xF3,0xF4,0x89,0x05,0xAB,0x37,0x3F,},
	{0xED,0x59,0x4A,0x14,0x40,0x84,0x3C,0x52,0x1A,0x9B,0xD2,0xD3,0x61,0x36,0xD6,0x77,},
	{0xCA,0x42,0x0C,0xB2,0xD2,0xD3,0x73,0xDF,0x96,0xD2,0x36,0x2F,0xF4,0x09,0x87,0x65,},
	{0x26,0x27,0xE8,0x9E,0x1B,0xFE,0xF2,0xB9,0x98,0x40,0x36,0x82,0x37,0xDD,0xAE,0x21,},
	{0x1D,0x7C,0xBE,0xC2,0x9F,0x84,0x6D,0x19,0xE6,0x6A,0x7E,0x6D,0x4E,0x0D,0x5E,0x11,},
	{0x14,0x6C,0xAD,0x3A,0x4E,0x38,0x5C,0xCB,0x50,0x52,0x1A,0x91,0x9D,0xF0,0xA7,0xB9,},
	{0x24,0x14,0x11,0x77,0x4A,0xC4,0xC6,0xA2,0xBA,0xCE,0xB7,0xB0,0x80,0x84,0x6A,0x8A,},
	{0xE1,0x24,0x6E,0xCB,0x30,0x73,0xD3,0xE5,0x1F,0xF3,0x72,0x4C,0x6A,0xDB,0x7C,0xF6,},
	{0xC3,0xC5,0x48,0x39,0xFC,0xE6,0xC6,0x4F,0xA5,0x0A,0x80,0x80,0x80,0xAB,0x4A,0xCB,},
	{0xE8,0x8F,0xAF,0x78,0x81,0xDB,0x88,0xB1,0x41,0x2C,0x57,0x56,0x51,0x7A,0x37,0xDE,},
	{0x51,0xC1,0xED,0xBB,0x3B,0x31,0x8E,0xFA,0x5E,0x1F,0x26,0x25,0xF3,0x53,0x64,0x74,},
	{0xA9,0xA7,0x64,0xDA,0x2B,0x04,0x74,0xE7,0x3C,0x97,0xF8,0xD0,0x8E,0x6F,0xFD,0x2A,},
	{0xD2,0xA2,0x2E,0xBF,0xBE,0x8E,0x8F,0xB7,0x00,0xB5,0x19,0x62,0xB5,0x63,0xBE,0xEE,},
	{0xC7,0xE3,0xAD,0xAA,0x16,0x5D,0xB2,0x4C,0x4C,0x05,0x7F,0xA3,0x38,0xBE,0x6B,0xEF,},
	{0x21,0xFB,0x52,0x50,0x24,0xA5,0xB5,0x1A,0x17,0xAC,0xDA,0x76,0xEB,0x12,0x32,0xD9,},
	{0xD9,0x09,0x08,0xAB,0xC1,0x53,0x14,0x75,0xEF,0xE2,0x72,0xB9,0x0F,0xEB,0x7E,0x42,},
	{0x49,0x2A,0x40,0xBA,0x10,0x6D,0xC9,0x22,0x2E,0x93,0x92,0x69,0x82,0x3C,0x61,0x3A,},
	{0xD3,0xA6,0xCA,0x2E,0xB6,0xC0,0x2D,0xCD,0xE7,0xDC,0x61,0x41,0x32,0xFC,0x0A,0x2B,},
	{0x2E,0x97,0x2D,0xC0,0xD1,0x77,0xC8,0x6C,0x40,0xE5,0x7C,0x2E,0xC0,0xAA,0xF2,0xB3,},
	{0x81,0x37,0xE6,0x69,0x21,0x4F,0x8B,0x88,0xFD,0x73,0xD3,0x33,0x0B,0x87,0xF0,0x32,},
	{0x88,0x67,0x3B,0xEA,0x0E,0xA0,0x69,0x5B,0xDD,0x5D,0xEE,0xD0,0x43,0xFE,0x0D,0x3C,},
	{0xEA,0xAE,0xC0,0xFA,0x16,0xA1,0x0A,0xDF,0x0C,0xA4,0xC3,0x5E,0xA4,0x95,0x60,0xB8,},
	{0xF5,0x4A,0xA6,0x59,0x6F,0x07,0xA4,0xE0,0xCE,0x8A,0x8C,0xC3,0x60,0xBB,0x44,0xFE,},
	{0x65,0x96,0x49,0x5D,0xC7,0xF5,0x1C,0xE7,0x5D,0x5B,0xDE,0x69,0xDC,0x0B,0xB6,0x47,},
	{0xFD,0x36,0xDB,0xF9,0x58,0xB4,0xCD,0xE2,0x09,0xDF,0x2C,0x8B,0x14,0x46,0x7E,0x51,},
	{0x5F,0xC5,0xF8,0x0E,0x4C,0x02,0xF0,0x00,0x2E,0xBE,0xFB,0x13,0x28,0x3A,0x04,0xC2,},
	{0xD3,0x44,0xC6,0x29,0xDF,0x4B,0xA5,0xB1,0xC8,0x66,0x54,0xE7,0xC8,0x90,0xD2,0x16,},
	{0xDC,0x8D,0x09,0x17,0x13,0xA8,0xFB,0xFA,0x46,0xDA,0xCA,0x99,0xB8,0xBA,0xB9,0xCE,},
	{0xDB,0x69,0xB8,0xD0,0x80,0x55,0x71,0x14,0x1F,0x46,0x45,0xE5,0xAE,0x08,0x37,0x9D,},
	{0xEC,0xE4,0x36,0x9B,0x30,0xEC,0xAF,0x2B,0x48,0xCB,0xE3,0x04,0x81,0x0B,0xF9,0x0E,},
	{0xAD,0x86,0x0C,0x5E,0xD8,0x01,0x96,0x87,0x1C,0xF5,0xF7,0xCC,0x62,0x16,0x4C,0x3D,},
	{0xA3,0x2C,0xAD,0xD3,0x83,0x1E,0x9D,0xEF,0xB9,0x8C,0x87,0xB5,0x71,0x6C,0x93,0x14,},
	{0x4D,0x37,0x37,0x27,0xFC,0xD8,0xC5,0xEA,0x4E,0xAE,0x7E,0x3B,0xF4,0xE3,0xC8,0x32,},
	{0x7E,0xA6,0x61,0x54,0x5E,0x25,0x8B,0x8F,0xE8,0x1D,0xA5,0x4B,0x7A,0xE1,0x16,0xB0,},
	{0x62,0xE3,0x7F,0x2B,0xDE,0x24,0x41,0xCA,0x67,0xC2,0x85,0xD4,0x9F,0x62,0xFA,0xD6,},
	{0xC4,0x54,0x36,0xCA,0x0A,0x3E,0xB6,0x27,0xAC,0xA8,0x3F,0xDE,0x33,0x7A,0x70,0x6C,},
	{0x98,0xCE,0xD8,0x11,0x63,0x9D,0x53,0x59,0x65,0x97,0xED,0x9B,0xA0,0xB6,0xEF,0x86,},
	{0x22,0xCE,0x87,0x25,0xAB,0x4B,0xA5,0x23,0x2C,0x57,0x7F,0x36,0xE5,0x62,0x96,0xEC,},
	{0x52,0x19,0x3B,0xD0,0x89,0xE8,0x72,0x26,0x01,0xDD,0x65,0x73,0x51,0x1A,0x87,0xF5,},
	{0xB8,0xB9,0xBB,0x64,0xF2,0x4A,0xC1,0x4D,0x28,0xB4,0xE4,0x0C,0x1E,0x1C,0x74,0x02,},
	{0x00,0x00,0x49,0x86,0x7A,0x1E,0xFC,0x39,0x5C,0x60,0x23,0xCD,0xE5,0x32,0xD2,0x12,},
	{0xD0,0x89,0x7E,0x2C,0xA6,0x31,0xFA,0x07,0x58,0xEE,0xA8,0xD5,0x04,0x66,0x08,0xE5,},
	{0x69,0x62,0x72,0x50,0xD1,0x00,0x7A,0x99,0xBC,0x81,0x08,0x92,0x26,0xF3,0x67,0xF6,},
	{0xE1,0xD2,0x3B,0x94,0x9D,0x47,0x5C,0xCE,0x06,0x41,0x2E,0x8B,0x76,0x27,0x35,0xF5,},
	{0x57,0x3E,0x64,0x82,0xC0,0x82,0x40,0xCF,0x0E,0xE5,0x6F,0x24,0x29,0x67,0xE0,0x64,},
	{0x87,0xB1,0xF3,0x31,0x2A,0x73,0x47,0x21,0x06,0xA3,0x4A,0x8F,0xD2,0xEF,0x22,0x94,},
	{0x4C,0x12,0x3A,0xE8,0x8E,0xE4,0x36,0x5F,0x91,0x6C,0x86,0xB8,0xD7,0x38,0xA5,0x9F,},
	{0x61,0xC9,0x04,0xC5,0x34,0x03,0xCB,0xE3,0xF7,0x43,0x7C,0x1C,0x38,0xB0,0xFD,0x1F,},
	{0xC2,0xDB,0xD8,0x17,0x2C,0xF8,0xD6,0xA2,0x7B,0x93,0x9F,0xBE,0xED,0x8E,0x49,0xCE,},
	{0xFE,0xAB,0x2E,0x08,0xEA,0x5D,0x60,0x48,0x47,0x57,0xF0,0x4B,0x67,0x19,0x27,0xE1,},
	{0x6F,0x66,0xF6,0xFC,0x5F,0x8E,0x72,0x68,0xF3,0x61,0x49,0xA0,0x64,0x0D,0xF6,0x69,},
	{0xF9,0xAE,0x1E,0x8D,0x82,0xF4,0x35,0x65,0x7A,0x8F,0xD0,0xD5,0x08,0x8B,0x3B,0x62,},
	{0x6C,0xBE,0x4E,0x87,0x98,0x4D,0x6E,0x70,0x38,0xC6,0x15,0x88,0x13,0xC0,0xE4,0x39,},
	{0x61,0x86,0x51,0xB9,0xFA,0x9A,0x5E,0x7A,0x31,0x90,0x82,0xD0,0xBE,0x9D,0x60,0x67,},
	{0x26,0xE7,0x33,0xA5,0xBE,0x6F,0xD1,0xAC,0x3F,0xAE,0x54,0xBC,0xDF,0xAC,0x51,0xBA,},
	{0xAB,0x96,0x74,0x81,0x39,0x81,0x67,0xD2,0x5F,0xD4,0x55,0x07,0xCB,0x9E,0xB2,0x9B,},
	{0x66,0xAD,0x62,0x2C,0x8C,0x3E,0x7A,0xF1,0xB5,0xA5,0x79,0x4C,0x45,0xA2,0x9A,0x85,},
	{0x3F,0xC7,0x24,0x27,0x29,0xDE,0x2E,0xA4,0x7F,0x61,0x45,0xEA,0x19,0xCA,0xE0,0xA1,},
	{0xFC,0xB9,0x56,0x32,0x49,0x24,0x2E,0x6C,0x1B,0x82,0xCC,0x93,0x08,0x1B,0x8D,0xBD,},
	{0x39,0xD0,0xC8,0x03,0x79,0x8A,0x71,0x87,0x7E,0xBD,0xAC,0x2A,0xF9,0x90,0xD8,0xEB,},
	{0xC4,0xD8,0x4F,0x8B,0x4C,0x68,0x3B,0x1A,0x99,0xB7,0x0C,0xA6,0x11,0x9A,0x9C,0x64,},
	{0xD3,0x88,0xB0,0x49,0x02,0xDD,0x27,0x72,0x47,0xCE,0xB3,0x8C,0x78,0x03,0x73,0x8A,},
	{0xAA,0xE1,0xCF,0x2F,0xCB,0x01,0x70,0xD0,0x94,0xAD,0x6A,0xC1,0x7E,0xCB,0x76,0xE4,},
	{0x20,0x48,0x56,0x71,0xDF,0x8E,0xE9,0xE7,0xB0,0xDD,0x44,0x7E,0xB5,0xAE,0xA2,0x95,},
	{0x44,0x81,0xFC,0x30,0xCB,0x6C,0x99,0xD2,0x17,0xE0,0xB4,0x54,0x72,0x65,0xF3,0x7C,},
	{0x58,0xCF,0x6C,0x2F,0xAC,0x01,0xAB,0x1B,0xF8,0xE7,0x7F,0xED,0xBD,0xF2,0x1B,0xE6,},
	{0x0B,0xD4,0x7C,0xFC,0xCE,0x45,0x18,0xB3,0xAF,0x84,0x5A,0xFF,0xDA,0x94,0x74,0x4B,},
	{0xEA,0xC7,0x25,0xE5,0x42,0x72,0xE9,0x43,0xD6,0xE1,0x38,0xE3,0xED,0xAC,0x14,0x24,},
	{0xC4,0xCE,0x49,0x9E,0xE8,0xF7,0xAE,0x12,0xB6,0xB8,0x80,0x27,0xA7,0xAB,0x41,0x63,},
	{0x73,0x34,0x03,0xF6,0xCC,0x35,0xA3,0x98,0xEB,0x39,0x26,0xEC,0x0C,0x35,0xC5,0x03,},
	{0x70,0x35,0x3B,0x0B,0x4B,0xD6,0x62,0xF3,0x19,0x13,0x2E,0x89,0xF2,0xC1,0x9C,0x2A,},
	{0xA0,0xD4,0x70,0xB9,0xA8,0x1E,0x57,0x8E,0x2A,0xB0,0xB3,0xAF,0xD0,0xC1,0x05,0x81,},
	{0xF2,0x7D,0xB7,0x71,0x70,0x35,0x17,0x9D,0xD1,0x10,0xDC,0xB9,0x07,0x19,0xA6,0x29,},
	{0x9D,0xFB,0x4C,0x38,0xC4,0x0C,0x5A,0x51,0xDE,0xAC,0xE0,0x9B,0x22,0xB3,0x22,0x58,},
	{0x0D,0xAC,0x96,0x74,0x09,0x2A,0xBA,0x6A,0xBD,0xD2,0xE7,0x98,0x41,0x9E,0x87,0x10,},
	{0x09,0x89,0x11,0xD4,0x74,0xCD,0xBC,0xE6,0x23,0x66,0x93,0xC4,0x54,0x7C,0x7A,0x1E,},
	{0xED,0x84,0xAC,0x5A,0x82,0x31,0x1F,0x72,0x41,0xE1,0x7A,0xF2,0x24,0xC1,0x0F,0x3F,},
	{0x24,0xD0,0x91,0x99,0xA1,0x4E,0xE3,0x3F,0xF0,0xB0,0x51,0xA3,0x97,0xF6,0x16,0x40,},
	{0x58,0xC9,0xAB,0xBC,0xF2,0x2D,0x8D,0xD7,0x6A,0x9F,0x93,0x93,0x52,0xCA,0xC1,0x61,},
	{0x56,0x53,0x9F,0x84,0xEB,0xE1,0x7C,0x9F,0xA1,0x6C,0x6A,0xD2,0xE0,0x7B,0xDB,0x4B,},
	{0xCB,0x3A,0x99,0x5B,0x16,0x52,0x4D,0xF7,0x42,0xB9,0x2F,0x02,0xBA,0x50,0xEC,0x25,},
	{0xEE,0xEF,0x4D,0x43,0x57,0x91,0x55,0x77,0x4C,0x0B,0x1B,0x85,0xC3,0xBB,0xF7,0xEE,},
	{0xC4,0x47,0xC7,0x18,0x30,0x66,0x4B,0x03,0x06,0xA7,0x42,0x31,0xB1,0x7A,0xE4,0x51,},
	{0x7F,0x44,0xE3,0xF0,0x2E,0x97,0xF3,0x39,0xE4,0x22,0x0E,0x0A,0x37,0xC8,0x34,0x86,},
	{0xC1,0x40,0x3E,0x73,0x88,0xD4,0xF4,0x88,0xA4,0x4A,0x3D,0xAF,0x4F,0x24,0xC5,0xD2,},
	{0x27,0xCA,0x05,0x7E,0xD7,0x07,0xC0,0x2C,0x54,0x8E,0x33,0x0E,0x58,0x40,0xCC,0x3E,},
	{0x72,0xBC,0x6F,0x70,0x3B,0x71,0xC8,0xAB,0x50,0x5B,0x4B,0xE4,0x59,0x43,0xEA,0xC0,},
	{0x5D,0x3B,0xE3,0x6C,0xC2,0xB6,0xAE,0xBA,0x80,0x11,0x1E,0xFA,0xA0,0xE5,0x2F,0x63,},
	{0x7D,0xFF,0x3C,0x93,0xB4,0x54,0x96,0xB5,0x01,0x59,0x2D,0x7C,0xBB,0xDC,0xD3,0x10,},
	{0xA3,0x73,0xB1,0x1C,0x5D,0x5F,0x7B,0xBC,0x0D,0xB5,0xF0,0x2A,0x52,0xCE,0x0B,0x6A,},
	{0x2C,0xE8,0x66,0x59,0x07,0x57,0x0C,0x08,0xA4,0x2D,0x6B,0x68,0x88,0x32,0xFE,0xA4,},
	{0x3E,0x0E,0x56,0x2D,0x34,0x12,0x06,0x1E,0x84,0x80,0x85,0xFE,0x64,0x6C,0x63,0x33,}
};

static void Pack_Parameter_XML(unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_Parameter_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.3
static void Pack_ProvisionId_XML(unsigned char *pwd, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_ProvisionId_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.2
static void Pack_CategoryPackageList_XML(unsigned int RequestIndex, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_CategoryPackageList_XML(unsigned char *xml, unsigned int len, unsigned char *IsContinueDownload, \
													RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB,\
													SAV_PARAM *pstSav);
// 5.4
static void Pack_PhysicalStock_XML(unsigned char *pwd, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_PhysicalStock_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.5
static void Pack_SalesmanInfo_XML(unsigned char *pwd, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_SalesmanInfo_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.6	Save Salesman Visit
static void Pack_SaveSalesmanVisit_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_SaveSalesmanVisit_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.7
static void Pack_SalesmanMerchantList_XML(unsigned char *pwd, unsigned char *Date, unsigned int RequestIndex, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_SalesmanMerchantList_XML(unsigned char *xml, unsigned int len, ROUTE_INFO *pstRoute, \
													 unsigned char *IsContinueDownload, \
													 RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.8 5.9
static void Pack_Order_XML(TRANS_DATA stData, unsigned char *szDate, unsigned char *type, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_CashOrder_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.10
static void Pack_SalesmanConsignOrder_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_SalesmanConsignOrder_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.11
static void Pack_ReturnConsignOrder_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_ReturnConsignOrder_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// 5.12
static void Pack_ConsignInquiry_XML(unsigned char *pwd, unsigned int RequestIndex, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_ConsignInquiry_XML(unsigned char *xml, unsigned int len, unsigned char *IsContinueDownload, \
											   RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB,\
											   SAV_PARAM *pstSav);

static void Pack_PendingOders_XML(TRANS_DATA stCur, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_PendingOders_XML(unsigned char *xml, unsigned int len, PENDING_ORDERS *pendingOrder, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

static void Pack_LastConsignOrder_XML(unsigned char *pwd, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_LastConsignOrder_XML(unsigned char *xml, unsigned int len, SAV_PARAM *record, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

static void Pack_Summary_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_Summary_XML(unsigned char *xml, unsigned int len, xSummary *Summary, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

// 5.15
static void Pack_Cancellation_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_Cancellation_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

static void Pack_Get_User_XML(unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_Get_User_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

static void Pack_Change_Pwd_XML(USER_INFO ByWho, USER_INFO Who, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_Change_Pwd_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);


static void Pack_OrderReport_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, unsigned char Type, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_OrderReport_XML(unsigned char *xml, unsigned int len, OrderReport *pstOR, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

// Added by Kim_LinHB 2013-7-30 v4.60
static void Pack_OrderReport_Prod_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_OrderReport_Prod_XML(unsigned char *xml, unsigned int len, OrderReport_Prod *pstOR, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// Add End

static void Pack_CreditTransfer_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_CreditTransfer_XML(unsigned char *xml, unsigned int len, unsigned char *pDeposit, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

// Added by Kim_LinHB 2013-10-28 v4.68
static void Pack_Inq_For_Consig_Recv_Confirm_XML(unsigned char OrderStatus,unsigned char OperationType, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_Inq_For_Consig_Recv_Confirm_XML(unsigned char *xml, unsigned int len, CONSIGNMENT_LIST *pstOrderList, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);

static void Pack_Update_For_Consign_Recv_Confirm_XML(CONSIGNMENT_LIST stOrderList, int OrderIndex, unsigned char IsConfirmed, unsigned char *pBuffer, unsigned int *lenInOut);
static unsigned char UnPack_Update_For_Consign_Recv_Confirm_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB);
// Add End

static int ZipUnzip(unsigned char *sInData, unsigned int iInLen, unsigned char *sOutData, 
					unsigned int *iOutLen, unsigned char compressionMode);

static int Compress_Request(unsigned char *sInData, unsigned char *sOutData, unsigned int *iOutLen, const unsigned char *MsgType);
static int Uncompress_Response(unsigned char *buf, unsigned int bufSize, unsigned char *sXmlData,unsigned int *iXmlDataSize);

static int My_atoi(void *s);
static unsigned char CalCulNbUn(unsigned char ucByte);
static void Set_Header_XML(const REQUEST_HEADER_TAG *header, unsigned char *buffer, unsigned int *lenInOut);
static unsigned int Base64Decode(unsigned char *szInput, unsigned char *szOutput, unsigned int uiInputLen);
static unsigned int Base64Encode(unsigned char *szInput, unsigned char *szOutput, unsigned int uiInputLen);
static unsigned char *GetRandomKey(int *index);
static unsigned char *GetKey(int index);

static unsigned char GetHeader(RESPONSE_HEADER_TAG *header, xml_t headerXML);
extern struct WlInfo gl_SimMesg; //add by jeff_xiehuan20170106

// 5.1
enCodeReturn Parameter_XML(RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_Parameter_XML(xmlBuffer, &xmlLen);
#if 1
		PubDebugTx("XML=%s",xmlBuffer);
#endif
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_PARAM);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_Parameter_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();
	return enCr;
}
// 5.3
enCodeReturn Get_ProvisionId_XML(unsigned char *pwd, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char *pszProvisionId, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));

		Pack_ProvisionId_XML(pwd, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_PROVISIONID);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_ProvisionId_XML(xmlBuffer, xmlLen, respH, respB);
					if(gl_ProvisionIdTmp[0] != 0)
					{
						enCr = OK;
						strcpy(pszProvisionId, gl_ProvisionIdTmp);
					}
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}
// 5.2
// Modified by Kim_LinHB 2013-4-2 v4.56  to loop mode(RequestIndex,IsContinueDownload)
enCodeReturn CategoryPackageList_XML(unsigned char *IsContinueDownload, unsigned int RequestIndex,\
									 RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected,\
									 SAV_PARAM *pstSav)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));

		Pack_CategoryPackageList_XML(RequestIndex, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_CATPACKLSIT);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(0 == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_CategoryPackageList_XML(xmlBuffer, xmlLen, IsContinueDownload, respH, respB, pstSav);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();
	return enCr;
}// 5.4

enCodeReturn PhysicalStock_XML(unsigned char *pwd, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_PhysicalStock_XML(pwd, xmlBuffer, &xmlLen);
		// Zip first
		PubDebugTx("xml = %s\n", xmlBuffer);
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_PHYSCSTOCK);
		if(enCr != OK)
			return enCr;

	//	connBuffer[connLen] = '\0'; 
		// added by Allen for test 2015-12-18
		//memset(xmlBuffer, 0, sizeof(xmlBuffer));
		//Uncompress_Request(connBuffer, connLen, xmlBuffer, &xmlLen);
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
	//	PubDebugTx(connBuffer);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);

				if(OK == enCr)
				{
					enCr = UnPack_PhysicalStock_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();
	return enCr;
}
// 5.5
enCodeReturn SalesmanInfo_XML(unsigned char *pwd, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));



	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_SalesmanInfo_XML(pwd, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_SALESMANIFO);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_SalesmanInfo_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;

}
// 5.6	Save Salesman Visit
enCodeReturn SaveSalesmanVisit_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_SaveSalesmanVisit_XML(stData, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_SALESVISIT);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_SaveSalesmanVisit_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}
// 5.7
enCodeReturn SalesmanMerchantList_XML(unsigned char *pwd, unsigned char *timeASCII, ROUTE_INFO *pstRoute, \
									  unsigned char *IsContinueDownload, unsigned int RequestIndex,\
									  RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_SalesmanMerchantList_XML(pwd, timeASCII, RequestIndex, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_SALESMERLIST);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_SalesmanMerchantList_XML(xmlBuffer, xmlLen, pstRoute,
						IsContinueDownload, respH, respB);
					if(OK == enCr)
					{
						strcpy(pstRoute->szMerchantListDate, timeASCII);
					}
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}
// 5.8 5.9
enCodeReturn Order_XML(TRANS_DATA stData, unsigned char *szDate, unsigned char *type,
					   RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_Order_XML(stData, szDate, type, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, type);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer)); 
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{	
					enCr = UnPack_CashOrder_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}
// 5.10
enCodeReturn SalesmanConsignOrder_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_SalesmanConsignOrder_XML(stData, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_CONSIGNORD);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_SalesmanConsignOrder_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;

}
// 5.11
enCodeReturn ReturnConsignOrder_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_ReturnConsignOrder_XML(stData, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_RETCONSORD);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_ReturnConsignOrder_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;

}
// 5.12
enCodeReturn ConsignInquiry_XML(unsigned char *pwd, unsigned char *IsContinueDownload, unsigned int RequestIndex,\
								RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, \
								unsigned char bAutoConnected,SAV_PARAM *pstSav)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_ConsignInquiry_XML(pwd, RequestIndex, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_CONSIGNINQ);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_ConsignInquiry_XML(xmlBuffer, xmlLen, IsContinueDownload, respH, respB, pstSav);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();
	return enCr;
}

enCodeReturn PendingOrders_XML(TRANS_DATA stCur, PENDING_ORDERS *pendingOrder, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_PendingOders_XML(stCur, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_PENDORDER);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_PendingOders_XML(xmlBuffer, xmlLen, pendingOrder, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

enCodeReturn LastConsignOrder_XML(unsigned char *pwd, SAV_PARAM *record, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_LastConsignOrder_XML(pwd, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_LSTCONSORD);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_LastConsignOrder_XML(xmlBuffer, xmlLen, record, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

enCodeReturn Summary_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, xSummary *Summary, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_Summary_XML(pwd, from, to, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_SUMMATY);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_Summary_XML(xmlBuffer, xmlLen, Summary, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}
// 5.15
enCodeReturn Cancellation_XML(TRANS_DATA stData, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_Cancellation_XML(stData, xmlBuffer, &xmlLen);
		// Zip first
		memset(connBuffer, 0, sizeof(connBuffer));
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_CANCEL);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_Cancellation_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

enCodeReturn Get_User_XML(RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_Get_User_XML(xmlBuffer, &xmlLen);
		// Zip first
		memset(connBuffer, 0, sizeof(connBuffer));
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_GETUSERS);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_Get_User_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

// Added by Kim_LinHB 2013-7-18 v4.60
enCodeReturn Change_Pwd_XML(USER_INFO ByWho, USER_INFO Who,RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));

		Pack_Change_Pwd_XML(ByWho, Who, xmlBuffer, &xmlLen);
		// Zip first
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_CHANGEUSERPWD);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_Change_Pwd_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();
	return enCr;
}
// Add End

enCodeReturn OrderReports_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, \
							  unsigned char Type,  OrderReport *pstOR, \
							  RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));
	if(pstOR)
		memset(pstOR, 0, sizeof(OrderReport));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_OrderReport_XML(pwd, from, to, Type, xmlBuffer, &xmlLen);
		// Zip first
		memset(connBuffer, 0, sizeof(connBuffer));
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_ORDERREPORT);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_OrderReport_XML(xmlBuffer, xmlLen, pstOR, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

// Added by Kim_LinHB 2013-7-30 v4.60
enCodeReturn OrderReports_Prod_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, \
								   OrderReport_Prod *pstOR, \
								   RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));
	if(pstOR)
		memset(pstOR, 0, sizeof(OrderReport_Prod));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_OrderReport_Prod_XML(pwd, from, to, xmlBuffer, &xmlLen);
		// Zip first
		memset(connBuffer, 0, sizeof(connBuffer));
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_ORDERREPORTPROD);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_OrderReport_Prod_XML(xmlBuffer, xmlLen, pstOR, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}
// Add End

enCodeReturn CreditTransferOrder_XML(TRANS_DATA stData, unsigned char *pDeposit, RESPONSE_HEADER_TAG *respH, \
									 RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_CreditTransfer_XML(stData, xmlBuffer, &xmlLen);
		// Zip first
		memset(connBuffer, 0, sizeof(connBuffer));
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_CREDITTRANSFER);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_CreditTransfer_XML(xmlBuffer, xmlLen, pDeposit, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

// Added by Kim_LinHB 2013-10-28 v4.68
enCodeReturn Inq_For_Consig_Recv_Confirm_XML(unsigned char *pwd, unsigned char OrderStatus,\
											 unsigned char OperationType, RESPONSE_HEADER_TAG *respH, \
											 RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected,\
											 CONSIGNMENT_LIST *pstOrderList)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_Inq_For_Consig_Recv_Confirm_XML(OrderStatus, OperationType, xmlBuffer, &xmlLen);
		// Zip first
		memset(connBuffer, 0, sizeof(connBuffer));
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_INQCONSIGNCONFIRM);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_Inq_For_Consig_Recv_Confirm_XML(xmlBuffer, xmlLen, pstOrderList, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

enCodeReturn Update_For_Consign_Recv_Confirm_XML(CONSIGNMENT_LIST stOrderList, int OrderIndex,\
												 int Operation, RESPONSE_HEADER_TAG *respH, \
												 RESPONSE_BODY_TAG *respB, unsigned char bAutoConnected)
{
	unsigned char xmlBuffer[MAX_TCP_BUFFER];
	unsigned char connBuffer[MAX_TCP_BUFFER];
	unsigned int xmlLen = MAX_TCP_BUFFER;
	unsigned int connLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(respH)
		memset(respH, 0, sizeof(RESPONSE_HEADER_TAG));
	if(respB)
		memset(respB, 0, sizeof(RESPONSE_BODY_TAG));

	if(TRUE == bAutoConnected)
		enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);

	if(OK == enCr)
	{
		memset(xmlBuffer, 0, sizeof(xmlBuffer));
		memset(connBuffer, 0, sizeof(connBuffer));
		Pack_Update_For_Consign_Recv_Confirm_XML(stOrderList, OrderIndex, Operation, xmlBuffer, &xmlLen);
		// Zip first
		memset(connBuffer, 0, sizeof(connBuffer));
		enCr = Compress_Request(xmlBuffer, connBuffer, &connLen, MSG_UPDATECONSIGNCONFIRM);
		if(enCr != OK)
			return enCr;
		enCr = Send(connBuffer, connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(xmlBuffer, 0, sizeof(xmlBuffer));
			memset(connBuffer, 0, sizeof(connBuffer));
			enCr = Recv(MAX_TCP_BUFFER, connBuffer, &connLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(connBuffer, connLen, xmlBuffer, &xmlLen);
				if(OK == enCr)
				{
					enCr = UnPack_Update_For_Consign_Recv_Confirm_XML(xmlBuffer, xmlLen, respH, respB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
					enCr = NoReponse;
				else
					enCr = IncidentReception;
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
		CloseConn();

	return enCr;
}

#define MAGIC_KEY "GPXGREGABMTECHNO"
void GenerationdPOSKey(unsigned char *pszTermSN,unsigned char *pEncryptPOSKey)
{
	unsigned char szPOSKey[16+1];
	unsigned char szMagicCle[16+1];
	unsigned char ucIndex1=0,ucIndex2 = 0;

	memset(szMagicCle,0,sizeof(szMagicCle));
	memcpy(szMagicCle,MAGIC_KEY,16);
	ConvertKey(szMagicCle);

	memset(szPOSKey,0,sizeof(szPOSKey));
	memcpy(szPOSKey,pszTermSN,8);
	ucIndex1 = 8;
	ucIndex2 = 0;

	szPOSKey[ucIndex1++] = (pszTermSN[ucIndex2++] + 'G')%255;
	szPOSKey[ucIndex1++] = (pszTermSN[ucIndex2++] + 'r')%255;
	szPOSKey[ucIndex1++] = (pszTermSN[ucIndex2++] + 'e')%255;
	szPOSKey[ucIndex1++] = (pszTermSN[ucIndex2++] + 'g')%255;
	szPOSKey[ucIndex1++] = (pszTermSN[ucIndex2++] + 'o')%255;
	szPOSKey[ucIndex1++] = (pszTermSN[ucIndex2++] + 'r')%255;
	szPOSKey[ucIndex1++] = (pszTermSN[ucIndex2++] + 'y')%255;
	szPOSKey[ucIndex1]	 = (pszTermSN[1] + '@')%255;

	EncrypteCode(szPOSKey,szMagicCle,pEncryptPOSKey);
	ConvertKey(pEncryptPOSKey);
}

void EncrypteCode(unsigned char *pszIn, unsigned char *pszKey, unsigned char *pszOut)
{
	//3des
	PubDes(TRI_ENCRYPT, pszIn, pszKey, pszOut);
	PubDes(TRI_ENCRYPT, pszIn+ 8, pszKey, pszOut + 8);
}

//对会话密钥进行转换
void ConvertKey(unsigned char *key)
{
	unsigned short i=0;
	for (i=0;i<16;i++)
	{
		//suppression bit de poids forts
		key[i]&=0x7F;
		if ((CalCulNbUn(key[i]) %2)==0)
		{//le nombre de bit a 1 est paire donc on en rajoute 1
			key[i]|=0x80;
		}

	}
}

static unsigned char CalCulNbUn(unsigned char ucByte)
{
	unsigned char i=0;
	unsigned char ucNb1=0;

	for (i=0;i<8;i++)
	{
		if (ucByte&0x01) ucNb1++;
		ucByte>>=1;
	}

	return ucNb1;
}


unsigned short CalcCRC(unsigned char *pData, unsigned short usLen)
{
	unsigned short i,j,usAcc,usFcs;

	usAcc = 0;

	for (i=0; i<usLen ; ++i)
	{
		for (j=0; j<8; ++j)
		{
			if (pData[i] & (1<<j))
			{//le bit  courant est a 1
				if (usAcc & 0x8000)
				{//et le bit 15 est a 1
					usAcc<<=1;
				}
				else
				{
					usAcc ^=0x4002;
					usAcc<<=1;
					usAcc|=0x0001;
				}
			}
			else
			{
				if (usAcc & 0x8000)
				{
					usAcc ^=0x4002;
					usAcc<<=1;
					usAcc|=0x0001;
				}
				else
					usAcc<<=1;
			}
		}
	}
	usFcs = 0;
	for (i=0;i<16; ++i)
	{
		usFcs<<=1;
		if (usAcc & (1<<i))
			usFcs |= 0x0001;
	}
	return usFcs;
}

void Short2Ascii(unsigned short usNum,unsigned char *pAscii)
{
	pAscii[0]= (usNum & 0xFF00)>>8;
	pAscii[1]= (unsigned char)(usNum & 0x00FF);	
}

static void Pack_Parameter_XML(unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_PARAM);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, "");
	strcpy(header.szPWD, "");
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	strcpy(body.szModel, gl_TermName);
	strcpy(body.szAppVer, STR_VERSION_SUB);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "Model", body.szModel);
	XmlAddAddr(&bodyXML, "AppVersion", body.szAppVer);
	XmlAddAddr(&bodyXML, "MACAddress", "");
	if(gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel] == GPRS) //added by jeff_xiehuan20170105
	{
		XmlAddAddr(&bodyXML, "Imsi",gl_SimMesg.imsi);
		XmlAddAddr(&bodyXML, "Iccid",gl_SimMesg.ccid);
	}
	else
	{
		XmlAddAddr(&bodyXML, "Imsi","");
		XmlAddAddr(&bodyXML, "Iccid","");
	}
	XmlAddAddr(&bodyXML, "IsSetup", 1 == gl_SavParam.bIsSetup ? "1" : "0");
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_Parameter_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);

	if(0 == ucRet)
	{
		xml_t bodyXML, MainXML, RemoteDownloadXML, PermissionListXML;

		ucRet = OK;

		XmlGetChild(&reqXML,"Body",0, &bodyXML);
		XmlGetChild(&bodyXML,"Main",0, &MainXML);
		//get attr
		// 		switch(XmlGetValue(&MainXML, "ConnectionType")[0])
		// 		{
		// 		case '4':
		// 			gl_MaintenaceParam.ucConnType[0] = ETHERNET;break;
		// 		case '3':
		// 			gl_MaintenaceParam.ucConnType[0] = GPRS;break;
		// 		default:
		// 			gl_MaintenaceParam.ucConnType[0] = SIMUL;break;
		// 		}
		// 		switch(XmlGetValue(&MainXML, "BackupConnectionType")[0])
		// 		{
		// 		case '4':
		// 			gl_MaintenaceParam.ucConnType[1] = ETHERNET;break;
		// 		case '3':
		// 			gl_MaintenaceParam.ucConnType[1] = GPRS;break;
		// 		default:
		// 			gl_MaintenaceParam.ucConnType[1] = SIMUL;break;
		// 		}
		strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, XmlGetValue(&MainXML, "HostIp"));
#ifdef LHB_TEST
		strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP, "193.169.190.118");
#endif
		strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, XmlGetValue(&MainXML, "Port"));
		strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak, XmlGetValue(&MainXML, "BackupHostIp"));
		strcpy(gl_MaintenaceParam.stConnParam.szRemotePort_Bak, XmlGetValue(&MainXML, "BackupPort"));
		//added by Allen 2016/03/08 
		strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, XmlGetValue(&MainXML, "Apn"));
		//add end
		// Added by Kim_LinHB 2013-11-11 v4.70
		gl_MaintenaceParam_Ext.cConnectionTimeout = My_atoi((void *)XmlGetValue(&MainXML, "ConnectionTimeout"));

		strcpy(gl_MaintenaceParam.stConnParam.szNumPad, XmlGetValue(&MainXML, "Tel1"));
		strcpy(gl_MaintenaceParam.stConnParam.szNumPadSec, XmlGetValue(&MainXML, "Tel2"));
		strcpy(gl_MaintenaceParam.szMaintenancePwd, XmlGetValue(&MainXML, "MaintenancePassword"));
		strcpy(gl_MaintenaceParam.szNetworkPwd, XmlGetValue(&MainXML, "NetworkPassword"));
		strcpy(gl_MaintenaceParam.szSalesmanID, XmlGetValue(&MainXML, "DealerCode"));
		
		// Modified by Kim_LinHB 2013-11-26 v4.74
		{
			unsigned char szSDTimer[4 + 1];
			strcpy(szSDTimer, XmlGetValue(&MainXML, "ParamDownloadTime"));
			GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time);

			strcpy(gl_MaintenaceParam.szTimeSettDownload, szSDTimer);
			if(memcmp(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, szSDTimer, 4) <= 0)
				PubAscDec(gl_MaintenaceParam.szLastSettDownLoad_Time, 6);
			memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, szSDTimer, 4);
		}
		// Modify End

		XmlGetChild(&bodyXML,"RemoteDownload",0,&RemoteDownloadXML);
		if(RemoteDownloadXML.start)
		{
			// Modified by Kim_LinHB 2013-11-26 v4.74
			unsigned char tmpTime[5 + 1];
			strcpy(tmpTime, XmlGetValue(&RemoteDownloadXML, "DownloadDateTime"));
			if(strlen(tmpTime) > 0)
			{
				strcpy(tmpTime + 2, tmpTime + 3);
				strcpy(gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, tmpTime);
			}
			GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);

			if(memcmp(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4) <= 0)
				PubAscDec(gl_MaintenaceParam.szLastSoftDownLoad_Time, 6);
			memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
			// Modify End

			gl_MaintenaceParam.stFTP.bForceToDownload = My_atoi((void *)XmlGetValue(&RemoteDownloadXML, "ForceTerminalToDownload"));
			// Modified by Kim_LinHB 2013-3-25 v4.55
			strcpy(gl_MaintenaceParam.stFTP.szRemoteIP, XmlGetValue(&RemoteDownloadXML, "FtpServerIp"));
			strcpy(gl_MaintenaceParam.stFTP.szRemotePort, XmlGetValue(&RemoteDownloadXML, "FtpServerPort"));
			strcpy(gl_MaintenaceParam.stFTP.szUser, XmlGetValue(&RemoteDownloadXML, "FtpUsername"));
			strcpy(gl_MaintenaceParam.stFTP.szPwd, XmlGetValue(&RemoteDownloadXML, "FtpPassword"));
			strcpy(gl_MaintenaceParam.stFTP.szFilePath, XmlGetValue(&RemoteDownloadXML, "Directory"));
			strcpy(gl_MaintenaceParam.stFTP.stApp.szFileVer, XmlGetValue(&RemoteDownloadXML, "AppVersion"));
			strcpy(gl_MaintenaceParam.stFTP.stApp.szFileName, XmlGetValue(&RemoteDownloadXML, "AppName"));
			strcpy(gl_MaintenaceParam.stFTP.stParam[0].szFileVer, XmlGetValue(&RemoteDownloadXML, "AppVersion"));
			strcpy(gl_MaintenaceParam.stFTP.stParam[0].szFileName, XmlGetValue(&RemoteDownloadXML, "ParamName"));
			// Modify End
		}

		XmlGetChild(&bodyXML,"PermissionList",0,&PermissionListXML);
		gl_MaintenaceParam.bPrintBarCode = My_atoi((void *)XmlGetValue(&PermissionListXML, "PrintBarCode"));
		gl_MaintenaceParam.bPrintLogo = My_atoi((void *)XmlGetValue(&PermissionListXML, "PrintLogo"));
		gl_MaintenaceParam.bCancelSale = My_atoi((void *)XmlGetValue(&PermissionListXML, "CancelSale"));
		//gl_MaintenaceParam.bChangeDiscount = My_atoi((void *)XmlGetValue(&PermissionListXML, "ChangeDiscount"));
		gl_MaintenaceParam.bScanSerialNumber = My_atoi((void *)XmlGetValue(&PermissionListXML, "ScanSerialNumber"));
		gl_MaintenaceParam.bInputSerial = My_atoi((void *)XmlGetValue(&PermissionListXML, "InputSerial"));

		WriteParam(WRT_MAINTA);
	}
	return ucRet;
}

static void Pack_ProvisionId_XML(unsigned char *pwd, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_PROVISIONID);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, pwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "UtilityCompanyCode", "111");
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_ProvisionId_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	memset(gl_ProvisionIdTmp, 0, sizeof(gl_ProvisionIdTmp));
	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		strcpy(gl_ProvisionIdTmp, XmlGetValue(&bodyXML,"ProvisionId"));
	}
	return ucRet;
}

// Modified by Kim_LinHB 2013-4-2 v4.56  to loop mode(RequestIndex,IsContinueDownload)
static void Pack_CategoryPackageList_XML(unsigned int RequestIndex, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_CATPACKLSIT);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, "");
	strcpy(header.szPWD, "");
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	{
		unsigned char tmp[3];
		sprintf(tmp, "%d", RequestIndex);
		XmlAddAddr(&bodyXML, "RequestIndex", tmp);
	}

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_CategoryPackageList_XML(unsigned char *xml, unsigned int len, unsigned char *IsContinueDownload, \
													RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB,\
													SAV_PARAM *pstSav)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		int i = 0;
		xml_t ProductCategorys, ProductCategory;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		*IsContinueDownload = My_atoi((void *)XmlGetValue(&bodyXML, "IsContinueDownload"));
		XmlGetChild(&bodyXML,"ProductCategorys",0,&ProductCategorys);
		while(ProductCategorys.start)
		{
			XmlGetChild(&ProductCategorys,"ProductCategory",i,&ProductCategory);
			if(ProductCategory.start != NULL)
			{
				int j = 0;
				xml_t Product;
				pstSav->stSavParamCategory[pstSav->ucCategoryNum].uiCodeCategory = My_atoi((void *)XmlGetValue(&ProductCategory, "Id"));
				pstSav->stSavParamCategory[pstSav->ucCategoryNum].ucCategoryType = My_atoi((void *)XmlGetValue(&ProductCategory, "CategoryType"));
				strcpy(pstSav->stSavParamCategory[pstSav->ucCategoryNum].szName, XmlGetValue(&ProductCategory, "Description"));		

				while(pstSav->uiProductsNum < NB_MAX_PRODUCTS)
				{
					XmlGetChild(&ProductCategory,"Product",j,&Product);
					if(Product.start != NULL)
						++j;
					else
						break;
					pstSav->stSavParamProducts[pstSav->uiProductsNum].uiCodeCategory = pstSav->stSavParamCategory[pstSav->ucCategoryNum].uiCodeCategory;
					pstSav->stSavParamProducts[pstSav->uiProductsNum].ucType = pstSav->stSavParamCategory[pstSav->ucCategoryNum].ucCategoryType;
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szCodeProduct, XmlGetValue(&Product, "Id"));
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szNameProduct, XmlGetValue(&Product, "Description"));

					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szAmount, XmlGetValue(&Product, "Amount"));
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szDiscount, XmlGetValue(&Product, "Discount"));
					pstSav->stSavParamProducts[pstSav->uiProductsNum].bCredit = My_atoi((void *)XmlGetValue(&Product, "Credit"));// Added by Kim_LinHB 2013-7-11 v4.60
				/*	PubDebugTx("id = %s\n", pstSav->stSavParamProducts[pstSav->uiProductsNum].szCodeProduct);
					PubDebugTx("name = %s\n", pstSav->stSavParamProducts[pstSav->uiProductsNum].szNameProduct);*/
					++pstSav->uiProductsNum;
				}
				++pstSav->ucCategoryNum;
				++i;
				if(pstSav->ucCategoryNum >= NB_MAX_CATEGORYS)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		//WriteParam();
	}
	return ucRet;
}
// Modify End

static void Pack_PhysicalStock_XML(unsigned char *pwd, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned int i;

	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML, productStockXML, productXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_PHYSCSTOCK);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, "");
	strcpy(header.szPWD, "");
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddChildNULL("ProductStock", &bodyXML, &productStockXML);
	for(i = 0; i < gl_SavParam.uiProductsNum; ++i)
	{
		if (1 == gl_SavParam.stSavParamProducts[i].ucType)
		{
			//if (0 == memcmp("2545",gl_SavParam.stSavParamProducts[i].szCodeProduct,4))
			//{
			//	break;
			//}
			memset(&productXML, 0, sizeof(xml_t));
			XmlAddChildNULL("Product", &productStockXML, &productXML);
			XmlAddAddr(&productXML, "Id", 
				gl_SavParam.stSavParamProducts[i].szCodeProduct);
			if(0 == strlen(gl_SavParam.stSavParamProducts[i].szQuantity))
				XmlAddAddr(&productXML, "Quantity", "0");
			else
				XmlAddAddr(&productXML, "Quantity", 
				gl_SavParam.stSavParamProducts[i].szQuantity);
		}
	}

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_PhysicalStock_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		xml_t ProductStock;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		XmlGetChild(&bodyXML,"ProductStock",0,&ProductStock);
		if(ProductStock.start != NULL)
		{
			int i = 0;
			xml_t Product;
			while(1)
			{
				XmlGetChild(&ProductStock,"Product",i,&Product);
				if(Product.start != NULL)
				{
					int j;
					int id = My_atoi((void *)XmlGetValue(&Product,"Id"));
					for(j = 0; j <gl_SavParam.uiProductsNum; ++j)
					{
						if(id == atoi(gl_SavParam.stSavParamProducts[j].szCodeProduct))
						{
							strcpy(gl_SavParam.stSavParamProducts[j].szQuantity, XmlGetValue(&Product,"Quantity"));
							break;
						}
					}
					++i;
				}
				else
					break;
			}
		}
		WriteParam(WRT_SAVE);
	}

	return ucRet;
}

static void Pack_SalesmanInfo_XML(unsigned char *pwd ,unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_SALESMANIFO);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, pwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_SalesmanInfo_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);

	if(0 == ucRet)
	{
		xml_t bodyXML;
		ucRet = OK;
		XmlGetChild(&reqXML, "Body", 0, &bodyXML);
		strcpy(gl_MaintenaceParam.szSalesmanName, XmlGetValue(&bodyXML, "SalesmanName"));
		strcpy(gl_MaintenaceParam.szSalesmanCredit, XmlGetValue(&bodyXML, "SalesmanCredit"));
		strcpy(gl_MaintenaceParam.szPhysicalStock, XmlGetValue(&bodyXML, "PhysicalStock"));
		WriteParam(WRT_MAINTA);
	}
	return ucRet;
}

static void Pack_SaveSalesmanVisit_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML, VisitListXML, VisitXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_SALESVISIT);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, gl_MaintenaceParam.stUser.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, stData.szDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);
	XmlAddAddr(&bodyXML, "IsOffline", ID_OfflineVisit == stData.ucTransType? "1" : "0");
	XmlAddChildNULL("VisitList", &bodyXML, &VisitListXML);

	if(ID_OfflineVisit != stData.ucTransType) // online
	{
		XmlAddChildNULL("Visit", &VisitListXML, &VisitXML);
		XmlAddAddr(&VisitXML, "VisitDateTime", stData.szDateTime);
		XmlAddAddr(&VisitXML, "MerchantId", stData.szMerchantCode);
		XmlAddAddr(&VisitXML, "Status", "1");
	}
	else // offline
	{
		TRANS_DATA tmp;
		int fd = open(FILE_OFFLINE, O_RDWR);
		long len = filesize(FILE_OFFLINE);
		if(fd < 0)
		{
			*lenInOut = 0;
			return;
		}
		seek(fd, 0, SEEK_SET);

		while(1)
		{
			if(len <= 0) // done
			{
				close(fd);
				break;
			}
			read(fd, (void *)&tmp, sizeof(TRANS_DATA) - sizeof(tmp.stProducts));
			len -= sizeof(TRANS_DATA) - sizeof(tmp.stProducts);
			if(ID_OfflineVisit == tmp.ucTransType)
			{
				XmlAddChildNULL("Visit", &VisitListXML, &VisitXML);
				XmlAddAddr(&VisitXML, "VisitDateTime", tmp.szDateTime);
				XmlAddAddr(&VisitXML, "MerchantId", tmp.szMerchantCode);
				XmlAddAddr(&VisitXML, "Status", "1");
			}
			else
			{
				len -= tmp.uiProductNum * sizeof(PRODUCT_INFO);
				seek(fd, tmp.uiProductNum * sizeof(PRODUCT_INFO),SEEK_CUR);
			}
		}
	}

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_SaveSalesmanVisit_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_SalesmanMerchantList_XML(unsigned char *pwd, unsigned char *Date, unsigned int RequestIndex, unsigned char *pBuffer, unsigned int *lenInOut)
{

	unsigned char SN[32 + 1];
	unsigned char RouteDate[8 + 1]; //yyyymmdd

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_SALESMERLIST);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, "");
	strcpy(header.szPWD, "");
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	sprintf(RouteDate, "%s%.6s", PRE_YEAR, Date);
	XmlAddAddr(&bodyXML, "RouteDate", RouteDate);
	{
		unsigned char tmp[3];
		sprintf(tmp, "%d", RequestIndex);
		XmlAddAddr(&bodyXML, "RequestIndex", tmp);
	}

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_SalesmanMerchantList_XML(unsigned char *xml, unsigned int len, ROUTE_INFO *pstRoute, \
													 unsigned char *IsContinueDownload, \
													 RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		unsigned int i = 0;
		xml_t bodyXML, MerchantListXML, MerchantXML;
		xMerchantList stMerchant;
		memset(&stMerchant, 0, sizeof(xMerchantList));
		ucRet = OK;
		XmlGetChild(&reqXML, "Body", 0, &bodyXML);
		*IsContinueDownload = My_atoi((void *)XmlGetValue(&bodyXML, "IsContinueDownload"));

		XmlGetChild(&bodyXML, "MerchantList", 0, &MerchantListXML);
		while(MerchantListXML.start)
		{
			XmlGetChild(&MerchantListXML, "Merchant", i, &MerchantXML);
			if(MerchantXML.start != NULL)
			{
				strcpy(stMerchant.szMerchantID, XmlGetValue(&MerchantXML, "Id"));
				strcpy(stMerchant.szName, XmlGetValue(&MerchantXML, "Name"));
				strcpy(stMerchant.szAddress, XmlGetValue(&MerchantXML, "Address"));
				strcpy(stMerchant.szMerchantCode, XmlGetValue(&MerchantXML, "MerchantCode"));
				strcpy(stMerchant.szSerial, XmlGetValue(&MerchantXML, "SerialNumber"));
				strcpy(stMerchant.szTermCode, XmlGetValue(&MerchantXML, "TerminalCode"));
				WriteMerchant(FILE_MERCHANT, stMerchant);
				++pstRoute->uiMerchantListNum;
				++i;
				if(pstRoute->uiMerchantListNum >= NB_MAX_MERCHANT)
				{
					*IsContinueDownload = FALSE;
					break;
				}
			}
			else
			{
				break;
			}
		}
		//WriteParam();
	}
	return ucRet;
}

static void Pack_Order_XML(TRANS_DATA stData, unsigned char *szDate, unsigned char *type, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned int iNum = 0;

	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML, orderItemsXML, orderItemXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, type);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, gl_MaintenaceParam.stUser.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, szDate);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);
	XmlAddAddr(&bodyXML, "IsOffline", ID_OfflineOrder == stData.ucTransType ? "1" : "0");

	XmlAddAddr(&bodyXML, "MerchantCode", stData.szMerchantCode);
	XmlAddAddr(&bodyXML, "MerchantId", stData.szMerchantID);
	XmlAddAddr(&bodyXML, "TerminalId", stData.szSerial);
	XmlAddAddr(&bodyXML, "TerminalCode", stData.szTermCode);

	XmlAddChildNULL("OrderItems", &bodyXML, &orderItemsXML);
	for(iNum = 0; iNum < stData.uiProductNum; ++iNum)
	{
		unsigned int k;
		XmlAddChildNULL("OrderItem", &orderItemsXML, &orderItemXML);
		if(ID_OfflineOrder == stData.ucTransType) //Online
			XmlAddAddr(&orderItemXML, "TranDateTime", stData.szDateTime);
		XmlAddAddr(&orderItemXML, "ProductId", stData.stProducts[iNum].szCodeProduct);
		XmlAddAddr(&orderItemXML, "Quantity", stData.stProducts[iNum].szQuantity);
		for(k =0; k < gl_SavParam.uiProductsNum; ++k)
		{
			if(0 == strcmp(gl_SavParam.stSavParamProducts[k].szCodeProduct, stData.stProducts[iNum].szCodeProduct))
			{
				XmlAddAddr(&orderItemXML, "Discount", gl_SavParam.stSavParamProducts[k].szDiscount);
				break;
			}
		}
	}
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_CashOrder_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}


static void Pack_SalesmanConsignOrder_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned int iNum = 0;

	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML, orderItemsXML, orderItemXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_CONSIGNORD);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, stData.szSalesman_PWD);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, stData.szDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);

	XmlAddChildNULL("OrderItems", &bodyXML, &orderItemsXML);
	for(iNum = 0; iNum < stData.uiProductNum; ++iNum)
	{
		XmlAddChildNULL("OrderItem", &orderItemsXML, &orderItemXML);
		XmlAddAddr(&orderItemXML, "ProductId", stData.stProducts[iNum].szCodeProduct);
		XmlAddAddr(&orderItemXML, "Quantity", stData.stProducts[iNum].szQuantity);
	}

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_SalesmanConsignOrder_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_ReturnConsignOrder_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned int iNum = 0;

	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML, orderItemsXML, orderItemXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_RETCONSORD);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, stData.szSalesman_PWD);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, stData.szDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);

	XmlAddChildNULL("OrderItems", &bodyXML, &orderItemsXML);
	for(iNum = 0; iNum < stData.uiProductNum; ++iNum)
	{
		XmlAddChildNULL("OrderItem", &orderItemsXML, &orderItemXML);
		XmlAddAddr(&orderItemXML, "ProductId", stData.stProducts[iNum].szCodeProduct);
		XmlAddAddr(&orderItemXML, "Quantity", stData.stProducts[iNum].szQuantity);
	}

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_ReturnConsignOrder_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_ConsignInquiry_XML(unsigned char *pwd, unsigned int RequestIndex, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_CONSIGNINQ);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, pwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	{
		unsigned char tmp[3];
		sprintf(tmp, "%d", RequestIndex);
		XmlAddAddr(&bodyXML, "RequestIndex", tmp);
	}
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_ConsignInquiry_XML(unsigned char *xml, unsigned int len, unsigned char *IsContinueDownload, \
											   RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB,\
											   SAV_PARAM *pstSav)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		int i = 0;
		xml_t ProductCategorys, ProductCategory;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		*IsContinueDownload = My_atoi((void *)XmlGetValue(&bodyXML, "IsContinueDownload"));
		XmlGetChild(&bodyXML,"ProductCategorys",0,&ProductCategorys);
		while(ProductCategorys.start)
		{
			XmlGetChild(&ProductCategorys,"ProductCategory",i,&ProductCategory);
			if(ProductCategory.start != NULL)
			{
				int j = 0;
				unsigned int kk;
				xml_t Product;
				pstSav->stSavParamCategory[pstSav->ucCategoryNum].uiCodeCategory = My_atoi((void *)XmlGetValue(&ProductCategory, "Id"));
				pstSav->stSavParamCategory[pstSav->ucCategoryNum].ucCategoryType = My_atoi((void *)XmlGetValue(&ProductCategory, "CategoryType"));
				strcpy(pstSav->stSavParamCategory[pstSav->ucCategoryNum].szName, XmlGetValue(&ProductCategory, "Description"));

				for(kk = 0; kk < gl_SavParam.ucCategoryNum; ++kk)
				{
					if(gl_SavParam.stSavParamCategory[kk].uiCodeCategory == 
						pstSav->stSavParamCategory[pstSav->ucCategoryNum].uiCodeCategory)
					{
						pstSav->stSavParamCategory[pstSav->ucCategoryNum].ucCategoryType = 
							gl_SavParam.stSavParamCategory[kk].ucCategoryType;
						break;
					}
				}

				while(pstSav->uiProductsNum < NB_MAX_PRODUCTS)
				{
					unsigned int k;
					unsigned char szCodeProduct[4 + 1];
					memset(szCodeProduct, 0, sizeof(szCodeProduct));
					XmlGetChild(&ProductCategory,"Product",j,&Product);
					if(Product.start != NULL)	
						++j;
					else
						break;
					pstSav->stSavParamProducts[pstSav->uiProductsNum].uiCodeCategory = pstSav->stSavParamCategory[pstSav->ucCategoryNum].uiCodeCategory;
					strcpy(szCodeProduct, XmlGetValue(&Product, "Id"));
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szCodeProduct, szCodeProduct);
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szNameProduct, XmlGetValue(&Product, "Description"));
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szAverage, XmlGetValue(&Product, "Average"));
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szQuantity, XmlGetValue(&Product, "Current"));
					strcpy(pstSav->stSavParamProducts[pstSav->uiProductsNum].szOrderNum, XmlGetValue(&Product, "Order"));

					for(k = 0; k < gl_SavParam.uiProductsNum; ++k)
					{
						if(0 == strcmp(gl_SavParam.stSavParamProducts[k].szCodeProduct, szCodeProduct))
						{
							strcpy(gl_SavParam.stSavParamProducts[k].szQuantity, pstSav->stSavParamProducts[pstSav->uiProductsNum].szQuantity);
							strcpy(gl_SavParam.stSavParamProducts[k].szAverage, pstSav->stSavParamProducts[pstSav->uiProductsNum].szAverage);
							strcpy(gl_SavParam.stSavParamProducts[k].szOrderNum,pstSav->stSavParamProducts[pstSav->uiProductsNum].szOrderNum);
							break;
						}
					}

					++pstSav->uiProductsNum;
				}
				++pstSav->ucCategoryNum;
				++i;
				if(pstSav->ucCategoryNum >= NB_MAX_CATEGORYS)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		//WriteParam();
	}
	return ucRet;
}

static void Pack_PendingOders_XML(TRANS_DATA stCur, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_PENDORDER);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, stCur.szSalesman_PWD);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "MerchantId", stCur.szMerchantID);
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_PendingOders_XML(unsigned char *xml, unsigned int len, PENDING_ORDERS *pendingOrder, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		int i = 0;
		xml_t PendingOrdersXML, OrderXML;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		XmlGetChild(&bodyXML,"PendingOrders",0,&PendingOrdersXML);
		pendingOrder->ucOrderNum = 0;
		while(PendingOrdersXML.start)
		{
			XmlGetChild(&PendingOrdersXML,"Order",i,&OrderXML);

			if(OrderXML.start != NULL)
			{
				strcpy(pendingOrder->PendingOrdersInfo[i].szId ,XmlGetValue(&OrderXML, "Id"));
				strcpy(pendingOrder->PendingOrdersInfo[i].szTotAmt ,XmlGetValue(&OrderXML, "TotalAmount"));
				if(0 == strlen(pendingOrder->PendingOrdersInfo[i].szTotAmt))
					strcpy(pendingOrder->PendingOrdersInfo[i].szTotAmt, "0");
				strcpy(pendingOrder->PendingOrdersInfo[i].szTermID ,XmlGetValue(&OrderXML, "TerminalId"));
				++pendingOrder->ucOrderNum;
				++i;
			}
			else
				break;
		}
	}
	return ucRet;
}

static void Pack_LastConsignOrder_XML(unsigned char *pwd, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_LSTCONSORD);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, pwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_LastConsignOrder_XML(unsigned char *xml, unsigned int len, SAV_PARAM *record, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		int i = 0;
		int Cnt = 0;
		xml_t ProductCategories, ProductCategory;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		strcpy(record->szOrderId,XmlGetValue(&bodyXML, "OrderId"));
		XmlGetChild(&bodyXML,"ProductCategories",0,&ProductCategories);
		record->ucCategoryNum = 0;
		record->uiProductsNum = 0;
		while(ProductCategories.start)
		{
			XmlGetChild(&ProductCategories,"ProductCategory",i,&ProductCategory);

			if(ProductCategory.start)
			{
				xml_t Products, Product;
				int k = 0;
				strcpy(record->stSavParamCategory[i].szName,XmlGetValue(&ProductCategory, "Title"));
				XmlGetChild(&ProductCategory,"Products", 0, &Products);
				while (Products.start)
				{
					XmlGetChild(&Products, "Product", k, &Product);
					if(Product.start)
					{
						unsigned char *p = NULL;
						strcpy(record->stSavParamProducts[Cnt].szNameProduct,XmlGetValue(&Product, "Name"));
						if(p = strchr(record->stSavParamProducts[Cnt].szNameProduct, '('))
							*p = '\0';
						strcpy(record->stSavParamProducts[Cnt].szQuantity,XmlGetValue(&Product, "Quantity"));
						strcpy(record->stSavParamProducts[Cnt].szAmount,XmlGetValue(&Product, "Amount"));
						record->stSavParamProducts[Cnt].uiCodeCategory = i;
						++Cnt;
						++k;
					}else
						break;
				}
				++i;
			}
			else
			{
				record->ucCategoryNum = i;
				record->uiProductsNum = Cnt;
				break;
			}

		}
	}
	return ucRet;
}

static void Pack_Summary_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_SUMMATY);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, pwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "TransactionStartDate", from);
	XmlAddAddr(&bodyXML, "TransactionEndDate", to);
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_Summary_XML(unsigned char *xml, unsigned int len, xSummary *Summary, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		xml_t cashXML, offXML, creditXML;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		XmlGetChild(&bodyXML,"CashOrder",0, &cashXML);
		if(cashXML.start)
		{
			strcpy(Summary->szCashTotalAmt_Gross,XmlGetValue(&cashXML, "TotalGrossAmount"));
			strcpy(Summary->szCashTotalAmt,XmlGetValue(&cashXML, "TotalNetAmount"));
		}

		XmlGetChild(&bodyXML, "OfflineCashOrder", 0, &offXML);
		if(offXML.start)
		{
			strcpy(Summary->szOffTotalAmt_Gross,XmlGetValue(&offXML, "TotalGrossAmount"));
			strcpy(Summary->szOffTotalAmt,XmlGetValue(&offXML, "TotalNetAmount"));
		}

		// Added by Kim_LinHB 2013-7-22 v4.60
		XmlGetChild(&bodyXML, "CreditOrder", 0, &creditXML);
		if(creditXML.start)
		{
			strcpy(Summary->szCreditTotalAmt_Gross,XmlGetValue(&creditXML, "TotalGrossAmount"));
			strcpy(Summary->szCreditTotalAmt,XmlGetValue(&creditXML, "TotalNetAmount"));
		}
		// Add End
	}
	return ucRet;
}


static void Pack_Cancellation_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_CANCEL);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, stData.szSalesman_PWD);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, stData.szDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////

	/////////////////////////////////////
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);
	switch(stData.ucTransType)
	{
	case ID_CashOrder:
	case ID_OfflineOrder:
		XmlAddAddr(&bodyXML, "RefMessageType", MSG_CASHORDER);break;
	case ID_ConsignOrder:XmlAddAddr(&bodyXML, "RefMessageType", MSG_CONSIGNORD);break;
	case ID_ReturnConsignOrder:XmlAddAddr(&bodyXML, "RefMessageType", MSG_RETCONSORD);break;
	case ID_CreditTransfer:XmlAddAddr(&bodyXML, "RefMessageType", MSG_CREDITTRANSFER);break; // Added by Kim_LinHB 2013-5-22
	case ID_CreditOrder:XmlAddAddr(&bodyXML, "RefMessageType", MSG_CREDITORDER);break; // Added by Kim_LinHB 2013-7-11 v4.60
	}
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_Cancellation_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_Get_User_XML(unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_GETUSERS);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, "");
	strcpy(header.szPWD, "");
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);

	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_Get_User_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXml, Users, User;
		ucRet = OK;
		XmlGetChild(&reqXML, "Body", 0, &bodyXml);

		if(bodyXml.start)
		{
			int i = 0;
			XmlGetChild(&bodyXml, "Users", 0, &Users);

			while(Users.start)
			{
				XmlGetChild(&Users, "User", i, &User);
				if(User.start)
				{
					USER_INFO tmp;
					strcpy(tmp.szName, XmlGetValue(&User, "Username"));
					strcpy(tmp.szPwd, XmlGetValue(&User, "Password"));
					if(0 == strcmp(tmp.szName, "salesman"))
					{
						strcpy(gl_MaintenaceParam.stUser.szName, tmp.szName);
						strcpy(gl_MaintenaceParam.stUser.szPwd, tmp.szPwd);
					}
					++i;		// temp , do not use local file to save user&pwd, cuz there is only one user.
				}
				else
					break;
			}
		}

	}
	return ucRet;
}

// Added by Kim_LinHB 2013-7-18 v4.60
static void Pack_Change_Pwd_XML(USER_INFO ByWho, USER_INFO Who, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_CHANGEUSERPWD);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ByWho.szName);
	strcpy(header.szPWD, ByWho.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "Username", Who.szName);
	XmlAddAddr(&bodyXML, "NewPassword", Who.szPwd);
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_Change_Pwd_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}
// Add End

static void Pack_OrderReport_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, unsigned char Type, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_ORDERREPORT);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, pwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "StartDate", from);
	XmlAddAddr(&bodyXML, "EndDate", to);
	{ // Added by Kim_LinHB 2013-6-17
		unsigned char tmp[3];
		sprintf(tmp, "%d", Type);
		XmlAddAddr(&bodyXML, "OperationType", tmp);
	}
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_OrderReport_XML(unsigned char *xml, unsigned int len, OrderReport *pstOR, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXml, Orders, Order;
		ucRet = OK;
		XmlGetChild(&reqXML, "Body", 0, &bodyXml);

		if(bodyXml.start)
		{
			int i = 0;
			int iProductCnt = 0;
			strcpy(pstOR->szMerchantName, XmlGetValue(&bodyXml, "MerchantName"));
			strcpy(pstOR->szMerchantId, XmlGetValue(&bodyXml, "MerchantId"));
			strcpy(pstOR->szMerchantTownName, XmlGetValue(&bodyXml, "MerchantTownName"));
			strcpy(pstOR->szSalesmanId, XmlGetValue(&bodyXml, "SalesmanId"));
			strcpy(pstOR->szSalesmanName, XmlGetValue(&bodyXml, "SalesmanName"));
			strcpy(pstOR->szTotAmt, XmlGetValue(&bodyXml, "TotalAmount"));

			XmlGetChild(&bodyXml, "Orders", 0, &Orders);
			while(Orders.start)
			{
				XmlGetChild(&Orders, "Order", i, &Order);
				if(Order.start)
				{
					strcpy(pstOR->Order[iProductCnt].szId, XmlGetValue(&Order, "Id"));
					strcpy(pstOR->Order[iProductCnt].szMerchantId, XmlGetValue(&Order, "MerchantId"));
					strcpy(pstOR->Order[iProductCnt].szMerchantName, XmlGetValue(&Order, "MerchantName"));
					strcpy(pstOR->Order[iProductCnt].szTotalAmt, XmlGetValue(&Order, "TotalAmount"));
					++iProductCnt;
					++i;
				}
				else
				{
					pstOR->uiOrderCnt = iProductCnt;
					break;
				}
			}
		}

	}
	return ucRet;
}

// Added by Kim_LinHB 2013-7-30 v4.60
static void Pack_OrderReport_Prod_XML(unsigned char *pwd, unsigned char *from, unsigned char *to, \
									  unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_ORDERREPORTPROD);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, pwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "StartDate", from);
	XmlAddAddr(&bodyXML, "EndDate", to);
	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_OrderReport_Prod_XML(unsigned char *xml, unsigned int len, \
												 OrderReport_Prod *pstOR, \
												 RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXml, Groups, Group;
		ucRet = OK;
		memset(pstOR, 0, sizeof(OrderReport_Prod));
		XmlGetChild(&reqXML, "Body", 0, &bodyXml);

		if(bodyXml.start)
		{
			int i = 0;
			int iProductCnt = 0;
			strcpy(pstOR->szSalesmanId, XmlGetValue(&bodyXml, "SalesmanId"));
			strcpy(pstOR->szSalesmanName, XmlGetValue(&bodyXml, "SalesmanName"));
			strcpy(pstOR->szCredit, XmlGetValue(&bodyXml, "Credit"));
			pstOR->uiTotalOrders = My_atoi((void *)XmlGetValue(&bodyXml, "TotalOrders"));
			strcpy(pstOR->szTotAmt, XmlGetValue(&bodyXml, "TotalAmount"));
			strcpy(pstOR->szDiscount, XmlGetValue(&bodyXml, "TotalDiscountAmount"));

			XmlGetChild(&bodyXml, "ProductGroups", 0, &Groups);
			while(Groups.start)
			{
				XmlGetChild(&Groups, "ProductGroup", i, &Group);
				if(Group.start)
				{
					int j = 0;
					xml_t Products, Product;
					strcpy(pstOR->Groups[i].szTitle, XmlGetValue(&Group, "Title"));
					strcpy(pstOR->Groups[i].szName, XmlGetValue(&Group, "Network"));
					pstOR->Groups[i].uiTotalOrders = My_atoi((void *)XmlGetValue(&Group, "TotalOrder"));
					strcpy(pstOR->Groups[i].szTotAmt, XmlGetValue(&Group, "TotalAmount"));
					strcpy(pstOR->Groups[i].szDiscount, XmlGetValue(&Group, "TotalDiscountAmount"));

					XmlGetChild(&Group, "Products", 0, &Products);
					while(Products.start)
					{
						XmlGetChild(&Products, "Product", j, &Product);
						if(Product.start)
						{
							pstOR->Products[iProductCnt].groupIndex = i;
							strcpy(pstOR->Products[iProductCnt].szName, XmlGetValue(&Product, "Name"));
							pstOR->Products[iProductCnt].uiTotalOrders = My_atoi((void *)XmlGetValue(&Product, "Quantity"));
							strcpy(pstOR->Products[iProductCnt].szTotAmt, XmlGetValue(&Product, "TotalAmount"));
							++iProductCnt;
							++j;
						}
						else
						{
							pstOR->uiProductCnt += j; // Added by Kim_LinHB 2013-8-27 v4.62
							pstOR->Groups[i].uiProductTypes = j;
							break;
						}
					}
					++i;
				}
				else
				{
					pstOR->uiGroupCnt = i; // Added by Kim_LinHB 2013-8-27 v4.62
					break;
				}
			}
		}

	}
	return ucRet;
}
// Add End

static void Pack_CreditTransfer_XML(TRANS_DATA stData, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_CREDITTRANSFER);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, gl_MaintenaceParam.stUser.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, stData.szDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);
	XmlAddAddr(&bodyXML, "Amount", stData.szAmountWithoutDiscount);

	XmlAddAddr(&bodyXML, "MerchantCode", stData.szMerchantCode);
	XmlAddAddr(&bodyXML, "TerminalCode", stData.szTermCode);

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_CreditTransfer_XML(unsigned char *xml, unsigned int len, unsigned char *pDeposit, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		strcpy(pDeposit, XmlGetValue(&bodyXML,"MerchantDepositAmount"));
	}
	return ucRet;
}

// Added by Kim_LinHB 2013-10-28 v4.68
static void Pack_Inq_For_Consig_Recv_Confirm_XML(unsigned char OrderStatus,unsigned char OperationType, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char szBuffer[10];
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_INQCONSIGNCONFIRM);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, gl_MaintenaceParam.stUser.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	sprintf(szBuffer, "%d", OrderStatus);
	XmlAddAddr(&bodyXML, "OrderStatus", szBuffer);
	sprintf(szBuffer, "%d", OperationType);
	XmlAddAddr(&bodyXML, "OperationType", szBuffer);

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_Inq_For_Consig_Recv_Confirm_XML(unsigned char *xml, unsigned int len, CONSIGNMENT_LIST *pstOrderList, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXml, Orders, Order;
		ucRet = OK;
		XmlGetChild(&reqXML, "Body", 0, &bodyXml);

		if(bodyXml.start)
		{
			int i = 0;
			XmlGetChild(&bodyXml, "Orders", 0, &Orders);
			pstOrderList->num = 0;
			while(Orders.start)
			{
				XmlGetChild(&Orders, "Order", i, &Order);
				if(Order.start)
				{
					strcpy(pstOrderList->ConsigOrderList[pstOrderList->num].Id, XmlGetValue(&Order, "Id"));
					pstOrderList->ConsigOrderList[pstOrderList->num].Type = My_atoi((void *)XmlGetValue(&Order, "OperationType"));
					strcpy(pstOrderList->ConsigOrderList[pstOrderList->num].CreateDate, XmlGetValue(&Order, "CreateDate"));
					strcpy(pstOrderList->ConsigOrderList[pstOrderList->num].TotalPrice, XmlGetValue(&Order, "TotalPrice"));

					++pstOrderList->num;
					++i;
				}
				else
					break;
			}
		}

	}
	return ucRet;
}

static void Pack_Update_For_Consign_Recv_Confirm_XML(CONSIGNMENT_LIST stOrderList, int OrderIndex, unsigned char IsConfirmed, unsigned char *pBuffer, unsigned int *lenInOut)
{
	unsigned char szBuffer[10];
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_UPDATECONSIGNCONFIRM);
	memcpy(header.szSerialNumber, SN, SIZE_NUM_SERIE_TPE);
	strcpy(header.szUserID, ""/*gl_MaintenaceParam.stUser.szName*/);
	strcpy(header.szPWD, gl_MaintenaceParam.stUser.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, lenInOut);
	root.end = root.start + *lenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "OrderId", stOrderList.ConsigOrderList[OrderIndex].Id);
	sprintf(szBuffer, "%d", IsConfirmed);
	XmlAddAddr(&bodyXML, "IsConfirmed", szBuffer);

	*lenInOut = root.end - root.start;
}
static unsigned char UnPack_Update_For_Consign_Recv_Confirm_XML(unsigned char *xml, unsigned int len, RESPONSE_HEADER_TAG *respH, RESPONSE_BODY_TAG *respB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", xml, xml + len, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(respH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}
// Add End


//////////////////////////////////////////////////////////////////////////

static int ZipUnzip(unsigned char *sInData, unsigned int iInLen, unsigned char *sOutData, 
					unsigned int *iOutLen, unsigned char compressionMode)
{
	int iRetVal=0;

	switch (compressionMode)
	{
	case 0:
		{
#if 0
			memcpy(sOutData,sInData,iInLen);
			*iOutLen=iInLen;
#else
			int extra =0;
			int i;

			iRetVal = compress(sOutData, iOutLen, sInData, iInLen);
			if(iRetVal!=0)
			{
				iRetVal = -104;
			}
			if(iRetVal == 0)
			{
				extra = 8 - *iOutLen % 8;
				for ( i = 0; i < extra; i++)
					sOutData[*iOutLen + i] = (unsigned char)extra;
				*iOutLen += extra;
			}
#endif
		}
		break;
	case 1:
#if 0
		memcpy(sOutData,sInData,iInLen);
		*iOutLen=iInLen;
#else
		*iOutLen = MAX_TCP_BUFFER;
		iRetVal = uncompress(sOutData, (uLongf)iOutLen,sInData, iInLen);
		if(iRetVal!=0)
		{
			iRetVal = -103;
		}
#endif
		break;
	}

	return iRetVal;
}

static int Compress_Request(unsigned char *sInData, unsigned char *sOutData, unsigned int *iOutLen, const unsigned char *MsgType)
{
	int iRet;
	//unsigned char *key1;
	int iGenerateKeyIndex;
	int pos = 0;
	unsigned char sCompressedDataSection[MAX_TCP_BUFFER - 27];
	//unsigned char *sCompressedDataSection = NULL;
	int sCompressedDataSection_Length=sizeof(sCompressedDataSection);
	//int sCompressedDataSection_Length;
	int i,j;
	//int len_tmp;
	unsigned char ss[32 + 1];
	unsigned char sEncryptedHeader[16];
	unsigned char szB64enCodedEncryptedHeader[200];
	unsigned char szConnectionType[1 + 1];
	int len;

	// #if defined(LHB_DEBUG) && defined(LHB_TEST)
	// 	{
	// #ifdef _WIN32
	// 		PubDebugOutput("",sInData,strlen(sInData),DEVICE_COM2,ASC_MODE);
	// #else
	// 		PubDebugOutput("",sInData,strlen(sInData),DEVICE_COM1,ASC_MODE);
	// #endif
	// 	}
	// #endif

	key1=GetRandomKey(&iGenerateKeyIndex);
	
	//added by Allen 
	//len_tmp = strlen(sInData) + 1;
	//sCompressedDataSection_Length = compressBound(len_tmp);
 //   if((sCompressedDataSection = (unsigned char*)malloc(sizeof(char) * sCompressedDataSection_Length)) == NULL)  
 //   {    
 //       return -1;  
 //   } 
	memset(sCompressedDataSection,0,sizeof(sCompressedDataSection));

	iRet = ZipUnzip(sInData, strlen(sInData), sCompressedDataSection, &sCompressedDataSection_Length, 0);
	//memset(sInData, 0, strlen(sInData));
	//ZipUnzip(sCompressedDataSection, sCompressedDataSection_Length, sInData, &len, 1);
	if(iRet!=0)
	{
		return iRet;
	}
	sOutData[pos++] = STX; 
	sOutData[pos++] =(unsigned char)iGenerateKeyIndex;

	switch(gl_MaintenaceParam.ucConnType[gl_MaintenaceParam.ucConnSel])
	{
	case  ETHERNET:
		strcpy((char *)szConnectionType , "4");
		break;
	case GPRS:
		strcpy((char *)szConnectionType , "3");
		break;
		// 	case CONNECT_MODEM:
		// 		strcpy((char *)requestMessageHeader->szConnectionType , "1");
		// 		break;
	default:
		strcpy((char *)szConnectionType , "1");
		break;
	}
	sprintf(ss,"%02.2s%01.1s%012.12s%04.4s%04.4s%02.2s%05.5d%02.2s",
		"05", szConnectionType, gl_TermSN, "0111", "0000",
		MsgType, sCompressedDataSection_Length + 1, "0000");

	PubAsc2Bcd(ss, 32, ss);
	EncrypteCode(ss, key1, sEncryptedHeader); 

	memset(szB64enCodedEncryptedHeader,0,sizeof(szB64enCodedEncryptedHeader));
	Base64Encode(sEncryptedHeader,szB64enCodedEncryptedHeader,sizeof(sEncryptedHeader));
	strcpy(sOutData + pos,szB64enCodedEncryptedHeader);
	pos += 24;
	j=sCompressedDataSection_Length/8;
	for(i = 0; i < j; ++i)
		PubDes(TRI_ENCRYPT, sCompressedDataSection+i*8, key1, sOutData+pos+i*8);

	pos+=sCompressedDataSection_Length;
	sOutData[pos++] = ETX; 
	*iOutLen =pos + 1;
	return OK;
}


//added by Allen for test 20151218
static int Uncompress_Request(unsigned char *buf, unsigned int connLen, unsigned char *sXmlData,unsigned int *iXmlDataSize)
{
	int i,j;
	int len;
	unsigned char tmp1[MAX_TCP_BUFFER];
	memset(tmp1, 0, sizeof(tmp1));
	j = (connLen - 28) / 8;
	for(i = 0; i < j; ++i)
	{
		PubDes(TRI_DECRYPT, buf+26+i*8, key1, tmp1+i*8);
	}

	memset(sXmlData, 0, strlen(sXmlData));
	ZipUnzip(tmp1, connLen - 28, sXmlData, &len, 1);

	*iXmlDataSize = len;
	return 0;
}


static int Uncompress_Response(unsigned char *buf, unsigned int connLen, unsigned char *sXmlData,unsigned int *iXmlDataSize)
{
#define LHB_BLOCK 512
	int iRet;
	unsigned char *key;
	unsigned char sHeader[12];
	unsigned char sB64deCodedEncryptedHeader[8+1];
	unsigned char sDecryptedHeader[8];
	unsigned char sDecryptedHeaderString[16 + 1];
	int offset = 0;
	int iKeyIndex;
	int i,j;

	unsigned char szHeaderVersion[2+1];
	unsigned char szResponseCode[4+1];
	int MessageLength = 0;
	unsigned char szReserved[4+1];

	memset(szHeaderVersion, 0, 3);
	memset(szResponseCode, 0, 5);
	memset(szReserved, 0, 5);

	// #if defined(LHB_DEBUG) && defined(LHB_TEST)
	// 	{
	// 		unsigned int block = connLen / LHB_BLOCK, block_Cnt = 1;
	// #ifdef _WIN32
	// 		if(block > 0)
	// 		{
	// 			PubDebugOutput("\x0d\x0a" "Before BASE64+3DES",buf,LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 			while(1)
	// 			{
	// 				if(block_Cnt < block)
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 				else
	// 				{
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, connLen - block * LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 					break;
	// 				}
	// 				++block_Cnt;
	// 			}
	// 		}
	// 		else
	// 			PubDebugOutput("\x0d\x0a" "Before BASE64+3DES",buf, connLen,DEVICE_COM2,HEX_MODE);
	// #else
	// 		if(block > 0)
	// 		{
	// 			PubDebugOutput("\x0d\x0a" "Before BASE64+3DES",buf,LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 			DelayMs(200);
	// 			while(1)
	// 			{
	// 				if(block_Cnt < block)
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 				else
	// 				{
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, connLen - block * LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 					break;
	// 				}
	// 				DelayMs(200);
	// 				++block_Cnt;
	// 			}
	// 		}
	// 		else
	// 			PubDebugOutput("\x0d\x0a" "Before BASE64+3DES",buf, connLen,DEVICE_COM1,HEX_MODE);
	// 		DelayMs(200);
	// #endif
	// 	}
	// #endif

	if (buf[offset] != STX)
	{
		printf("Invalid message header!\n");
		return ERREUR_STX;
	}
	++offset;
	iKeyIndex = buf[offset];
	key=GetKey(iKeyIndex);
	++offset;
	memset(sHeader,0,sizeof(sHeader));
	memcpy(sHeader,buf+offset,12);//copy 12 bytes to sHeader
	offset += 12;
	memset(sB64deCodedEncryptedHeader,0,sizeof(sB64deCodedEncryptedHeader));
	Base64Decode(sHeader,sB64deCodedEncryptedHeader,sizeof(sHeader));//将12个长的sHeader Base64Decode 解密到 sB64deCodedEncryptedHeader
	PubDes(TRI_DECRYPT, sB64deCodedEncryptedHeader, key, sDecryptedHeader);
	PubBcd2Asc0(sDecryptedHeader, 8, sDecryptedHeaderString);
	memcpy(szHeaderVersion ,sDecryptedHeaderString,2);
	memcpy(szResponseCode , sDecryptedHeaderString+2, 4);
	{
		unsigned char szTemp[6 + 1];
		memset(szTemp,0,sizeof(szTemp));
		memcpy(szTemp,sDecryptedHeaderString+6, 6);
		MessageLength = atoi(szTemp);
	}
	memcpy(szReserved , sDecryptedHeaderString+12, 4);

	if(strcmp(szResponseCode,"0000")!=0)
	{
		/*
		The response code is independent from the transaction type, the value 00 means response message has been processed and transaction is successful.
		If the value is not 00, there is a mistake occurred independent from the content of response message and the section after the header contains the error message. The error message is clear text.
		*/
		return -105;
	}

	buf+=offset;//buf现在指向content部分
	j=MessageLength/8;
	for(i = 0; i < j; ++i)
	{
		PubDes(TRI_DECRYPT, buf+i*8, key, buf+i*8);
	}

	// #if defined(LHB_DEBUG) && defined(LHB_TEST)
	// 	{
	// 		unsigned int block = MessageLength / LHB_BLOCK, block_Cnt = 1;
	// #ifdef _WIN32
	// 		if(block > 0)
	// 		{
	// 			PubDebugOutput("\x0d\x0a" "Before Uncompress(data part)",buf,LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 			while(1)
	// 			{
	// 				if(block_Cnt < block)
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 				else
	// 				{
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, MessageLength - block * LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 					break;
	// 				}
	// 				++block_Cnt;
	// 			}
	// 		}
	// 		else
	// 			PubDebugOutput("\x0d\x0a" "Before Uncompress(data part)",buf, MessageLength,DEVICE_COM2,HEX_MODE);
	// #else
	// 		if(block > 0)
	// 		{
	// 			PubDebugOutput("\x0d\x0a" "Before Uncompress(data part)",buf,LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 			DelayMs(200);
	// 			while(1)
	// 			{
	// 				if(block_Cnt < block)
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 				else
	// 				{
	// 					PubDebugOutput("",buf + block_Cnt * LHB_BLOCK, MessageLength - block * LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 					break;
	// 				}
	// 				DelayMs(200);
	// 				++block_Cnt;
	// 			}
	// 		}
	// 		else
	// 			PubDebugOutput("\x0d\x0a" "Before Uncompress(data part)",buf, MessageLength,DEVICE_COM1,HEX_MODE);
	// 		DelayMs(200);
	// #endif
	// 	}
	// #endif

	//解压存放 到sXmlData
	iRet = ZipUnzip(buf, MessageLength, sXmlData, iXmlDataSize, 1);

	if(iRet!=0)
	{
		return iRet;
	}


	// #if defined(LHB_DEBUG) && defined(LHB_TEST)
	// 	{
	// 		unsigned int block = *iXmlDataSize / LHB_BLOCK, block_Cnt = 1;
	// #ifdef _WIN32
	// 		if(block > 0)
	// 		{
	// 			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 			while(1)
	// 			{
	// 				if(block_Cnt < block)
	// 					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 				else
	// 				{
	// 					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, *iXmlDataSize - block * LHB_BLOCK,DEVICE_COM2,HEX_MODE);
	// 					break;
	// 				}
	// 				++block_Cnt;
	// 			}
	// 		}
	// 		else
	// 			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData, *iXmlDataSize,DEVICE_COM2,HEX_MODE);
	// 		PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,*iXmlDataSize,DEVICE_COM2,ASC_MODE);
	// #else
	// 		if(block > 0)
	// 		{
	// 			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 			DelayMs(200);
	// 			while(1)
	// 			{
	// 				if(block_Cnt < block)
	// 					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 				else
	// 				{
	// 					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, *iXmlDataSize - block * LHB_BLOCK,DEVICE_COM1,HEX_MODE);
	// 					break;
	// 				}
	// 				DelayMs(200);
	// 				++block_Cnt;
	// 			}
	// 		}
	// 		else
	// 			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData, *iXmlDataSize,DEVICE_COM1,HEX_MODE);
	// 		DelayMs(200);
	// 		PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,*iXmlDataSize,DEVICE_COM1,ASC_MODE);
	// #endif
	// 	}
	// #endif
#undef LHB_BLOCK
	return iRet;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static unsigned char *GetRandomKey(int *index)
{
	int iRand;
	unsigned char timeACSII[SIZE_DATE_TIME + 1 + 2];
	unsigned long ltime;
	//int index;

	// Modified by Kim_LinHB 2013-3-20 v4.55
	strcpy(timeACSII, PRE_YEAR);
	GetCurTime(timeACSII + 2);
	// Modify End

	ltime = PubTime2Long(timeACSII); //since 1980.1.1.0.0.0

	//srand( time(NULL) ); ////time (null)  can not be used 
	srand(ltime);
	iRand=rand();

	*index=iRand & 0xFF;
	return embedded_keys[*index];
}

static unsigned char *GetKey(int index)
{
	return embedded_keys[index];
}


static unsigned char GetHeader(RESPONSE_HEADER_TAG *header, xml_t headerXML)
{
	unsigned char ucRet = 1;
	if(header)
	{
		unsigned char timeBCD[6];
		strcpy(header->szResponseCode, XmlGetValue(&headerXML,"ResponseCode"));
		strcpy(header->szStatus, XmlGetValue(&headerXML,"Status"));
		strcpy(header->szMessage, XmlGetValue(&headerXML,"Message"));
		strcpy(header->szHoseDateTime, XmlGetValue(&headerXML, "HostDateTime"));
		PubAsc2Bcd(header->szHoseDateTime, SIZE_DATE_TIME, timeBCD);
		SetTime(timeBCD);
		ucRet = atoi(header->szStatus);
	}

	return ucRet;
}


static void Set_Header_XML(const REQUEST_HEADER_TAG *header, unsigned char *buffer, unsigned int *lenInOut)
{
	xml_t reqXML, headerXML;

	xml_t root = {"", buffer, buffer + *lenInOut, NULL};

	XmlAddChildNULL("Request", &root, &reqXML);
	XmlAddChildNULL("Header", &reqXML, &headerXML);

	XmlAddAddr(&headerXML, "MessageType", header->szMsgType);
	XmlAddAddr(&headerXML, "SerialNumber", header->szSerialNumber);
	XmlAddAddr(&headerXML, "Username", header->szUserID);
	XmlAddAddr(&headerXML, "Password", header->szPWD);
	XmlAddAddr(&headerXML, "ParameterVersion", header->szParamVer);
	{
		sprintf(header->szBatchNo, "%d", atoi(header->szBatchNo));
		XmlAddAddr(&headerXML, "BatchNo", header->szBatchNo);
	}
	XmlAddAddr(&headerXML, "TraceNo", header->szTraceNo);
	XmlAddAddr(&headerXML, "TerminalDateTime", header->szTermDateTime);
	XmlAddAddr(&headerXML, "TerminalId", header->szTermId);
	XmlAddAddr(&headerXML, "LanguageCode", header->szLangCode);
	*lenInOut = root.end - root.start;
}

static char base64_alphabet[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="};
static unsigned int Base64Decode(unsigned char *szInput, unsigned char *szOutput, unsigned int uiInputLen)
{
	unsigned char szCode[5];
	unsigned int i,j,k,m;
	unsigned char szASC[4];
	for(i = 0,j=0; i<uiInputLen; i+=4, j+=3)
	{
		memset(szCode, 0, 5);
		szCode[0] = szInput[i];
		szCode[1] = szInput[i+1];
		szCode[2] = szInput[i+2];
		szCode[3] = szInput[i+3];
		for(k=0; k<4; k++)
		{
			for(m=0; m<65; m++)
			{
				if(szCode[k] == base64_alphabet[m])
				{
					szASC[k] = m;
					break;
				}
			}
		}
		if((szCode[2] == '=') && (szCode[3] == '='))
		{
			szOutput[j] = (szASC[0]<<2) + ((szASC[1]<<2)>>6);
		}
		else if(szCode[3] == '=')
		{
			szOutput[j] = (szASC[0]<<2) + ((szASC[1]<<2)>>6);
			szOutput[j+1] = (szASC[1]<<4) + ((szASC[2]<<2)>>4);
		}
		else
		{
			szOutput[j] = (szASC[0]<<2) + ((szASC[1]<<2)>>6);
			szOutput[j+1] = (szASC[1]<<4) + ((szASC[2]<<2)>>4);
			szOutput[j+2] = (szASC[2]<<6) + szASC[3];
		}
	}
	return (j+3);
}
static unsigned int Base64Encode(unsigned char *szInput, unsigned char *szOutput, unsigned int uiInputLen)
{
	unsigned char szCode[4];
	unsigned int i,j;
	unsigned int uiCodeLen=0;
	for(i = 0,j=0; i<uiInputLen; i+=3, j+=4)
	{
		memset(szCode, 0, 4);
		uiCodeLen = 0;
		szCode[0] = szInput[i];
		uiCodeLen += 1;
		if(i < (uiInputLen - 1))
		{
			szCode[1] = szInput[i+1];
			uiCodeLen += 1;
		}
		if(i < (uiInputLen - 2))
		{
			szCode[2] = szInput[i+2];
			uiCodeLen += 1;
		}
		if(uiCodeLen == 3)
		{
			szOutput[j] = base64_alphabet[(szCode[0]&0xfc)>>2];
			szOutput[j+1] = base64_alphabet[((szCode[0]&0x03)<<4) + ((szCode[1]&0xf0)>>4)];
			szOutput[j+2] = base64_alphabet[((szCode[1]&0x0f)<<2) + ((szCode[2]&0xc0)>>6)];
			szOutput[j+3] = base64_alphabet[szCode[2]&0x3f];
		}
		else if(uiCodeLen == 2)
		{
			szOutput[j] = base64_alphabet[(szCode[0]&0xfc)>>2];
			szOutput[j+1] = base64_alphabet[((szCode[0]&0x03)<<4) + ((szCode[1]&0xf0)>>4)];
			szOutput[j+2] = base64_alphabet[(szCode[1]&0x0f)<<2];
			szOutput[j+3] = base64_alphabet[64];
		}
		else
		{
			szOutput[j] = base64_alphabet[(szCode[0]&0xfc)>>2];
			szOutput[j+1] = base64_alphabet[(szCode[0]&0x03)<<4];
			szOutput[j+2] = base64_alphabet[64];
			szOutput[j+3] = base64_alphabet[64];
		}
	}
	return (j+4);
}


static int My_atoi(void *s)
{
	if(0 == strcmp((char *)s, ""))
		return 0;
	else
		return atoi(s);
}

