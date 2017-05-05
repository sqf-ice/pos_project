#include <posapi.h>
#include <posapi_all.h>

#include "appLib.h"
#include "data_define_merchant.h"
#include "framework.h"

#include "calc_merchant.h"
#include "connect.h"
#include "LHBXML.h"
#include "zlib/zlib.h"

#include "Utf8ToAr.h"
#include "printList_merchant.h"

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

static void Pack_Parameter_XML(unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Parameter_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);
// 5.3
static void Pack_ProvisionId_XML(USER_INFO stCur, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_ProvisionId_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);
// 5.2
static void Pack_CategoryPackageList_XML(unsigned int uiRequestIndex, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_CategoryPackageList_XML(unsigned char *pszXml, unsigned int uiLen, unsigned char *IsContinueDownload,\
											RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB,\
											SAV_PARAM *pstSav);

static void Pack_Merchant_OL_Evoucher_XML(TRANS_DATA stCur, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Merchant_OL_Evoucher_XML(unsigned char *pszXml, unsigned int uiLen, TRANS_DATA *pstCur, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_ReStockPins_XML(unsigned char *pszProvisionId, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_ReStockPins_XML(unsigned char *pszXml, unsigned int uiLen, unsigned char *IsContinueDownload, unsigned int *puiCount, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

// Added by Kim_LinHB 2013-12-4 v4.76
static void Pack_ReStockPin_New_XML(unsigned char *pszProvisionId, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_ReStockPin_New_XML(unsigned char *pszXml, unsigned int uiLen, const unsigned char *pszProvisionID, unsigned char *IsContinueDownload, unsigned int *puiCount, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_PendingOrder_XML(unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_PendingOrder_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);
// Add End

static unsigned int Pack_SalesPolling_XML(unsigned char bAuto, unsigned int *puiStart, unsigned int *puiRest, unsigned char *bContinue, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_SalesPolling_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_Claim_XML(unsigned char *pszVoucherSN, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Claim_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_Get_Balance_XML(unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Get_Balance_XML(unsigned char *pszXml, unsigned int uiLen, unsigned char *pszValue, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_Get_Mail_XML(unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Get_Mail_XML(unsigned char *pszXml, unsigned int uiLen, MAIL *pstMail, unsigned short *Nums, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

// 5.15
// Modified by Kim_LinHB 2014-08-14 v4.95
static void Pack_Cancellation_XML(TRANS_DATA stData, unsigned char IsAutomatically, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Cancellation_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_Get_Users_XML(unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Get_Users_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_Change_Pwd_XML(USER_INFO stByWho, USER_INFO stWho, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Change_Pwd_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_Create_User_XML(USER_INFO stWho, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Create_User_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

static void Pack_Del_User_XML(USER_INFO stWho, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_Del_User_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);

// Added by Kim_LinHB 2013-5-22 v4.59
static void Pack_CashOrder_XML(TRANS_DATA_CASH stData, unsigned char *szDate, unsigned char *pBuffer, unsigned int *puiLenInOut);
static unsigned char UnPack_CashOrder_XML(unsigned char *pszXml, unsigned int uiLen, RESPONSE_HEADER_TAG *pstRespH, RESPONSE_BODY_TAG *pstRespB);
// Add End

static int ZipUnzip(unsigned char *psInData, unsigned int iInLen, unsigned char *psOutData, unsigned int *piOutLen, unsigned char compressionMode);

static int Compress_Request(unsigned char *psInData, unsigned char *psOutData, unsigned int *piOutLen, const unsigned char *MsgType);
static int Uncompress_Response(unsigned char *psBuf, unsigned int uiConnLen, unsigned char *psXmlData,unsigned int *puiXmlDataSize, RESPONSE_HEADER_TAG *pstRespH);

static int My_atoi(void *s);
static unsigned char CalCulNbUn(unsigned char ucByte);
static void Set_Header_XML(const REQUEST_HEADER_TAG *stHeader, unsigned char *pszBuffer, unsigned int *puiLenInOut);
static unsigned int Base64Decode(unsigned char *pszInput, unsigned char *pszOutput, unsigned int uiInputLen);
static unsigned int Base64Encode(unsigned char *pszInput, unsigned char *pszOutput, unsigned int uiInputLen);
static unsigned char *GetRandomKey(int *piIndex);
static unsigned char *GetKey(int iIndex);

static unsigned char GetHeader(RESPONSE_HEADER_TAG *pstHeader, xml_t stHeaderXML);

// 5.1
enCodeReturn Parameter_XML(RESPONSE_HEADER_TAG *pstRespH, \
						   RESPONSE_BODY_TAG *pstRespB, \
						   unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;
	
	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Parameter_XML(szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_PARAM);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Parameter_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}
// 5.3
enCodeReturn Get_ProvisionId_XML(USER_INFO stCur, \
								 RESPONSE_HEADER_TAG *pstRespH, \
								 RESPONSE_BODY_TAG *pstRespB, \
								 unsigned char *pszProvisionId, \
								 unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));
		
		Pack_ProvisionId_XML(stCur, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_PROVISIONID);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_ProvisionId_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
					// Modified by Kim_LinHB 2013-8-28 v4.64
					if(atol(gl_ProvisionIdTmp) > 0)
					{
						enCr = OK;
						strcpy(pszProvisionId, gl_ProvisionIdTmp);
					}
					else
					{	enCr = TYPE_MESSAGE_RECU_NOK;}
					// Modify End
				}
				else
				{
					if(NET_ERR_TIMEOUT == enCr)
					{	enCr = NoReponse;}
					else
					{	enCr = IncidentReception;}
				}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}

	return enCr;
}
// 5.2
enCodeReturn CategoryPackageList_XML(unsigned char *IsContinueDownload, \
									 unsigned int RequestIndex,\
									 RESPONSE_HEADER_TAG *pstRespH, \
									 RESPONSE_BODY_TAG *pstRespB, \
									 unsigned char bAutoConnected,\
									 SAV_PARAM *pstSav)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_CategoryPackageList_XML(RequestIndex, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_CATPACKLSIT);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(0 == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_CategoryPackageList_XML(szXmlBuffer, uiXmlLen, IsContinueDownload, pstRespH, pstRespB, pstSav);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}

enCodeReturn Merchant_OL_Evoucher_XML(TRANS_DATA *pstCur, \
									  RESPONSE_HEADER_TAG *pstRespH, \
									  RESPONSE_BODY_TAG *pstRespB, \
									  unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Merchant_OL_Evoucher_XML(*pstCur, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_MEROLEVOUCHER);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(0 == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Merchant_OL_Evoucher_XML(szXmlBuffer, uiXmlLen, pstCur, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}

enCodeReturn ReStockPins_XML(unsigned char *pszProvisionId, \
							 unsigned char *IsContinueDownload, \
							 unsigned int *puiCount, \
							 RESPONSE_HEADER_TAG *pstRespH, \
							 RESPONSE_BODY_TAG *pstRespB, \
							 unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_ReStockPins_XML(pszProvisionId, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_RESTOCKPIN);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(0 == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_ReStockPins_XML(szXmlBuffer, uiXmlLen, IsContinueDownload, puiCount, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}

// Added by Kim_LinHB 2013-12-4 v4.76
enCodeReturn ReStockPin_New_XML(unsigned char *pszProvisionId, \
								unsigned char *IsContinueDownload, \
								unsigned int *puiCount, \
								RESPONSE_HEADER_TAG *pstRespH, \
								RESPONSE_BODY_TAG *pstRespB, \
								unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_ReStockPin_New_XML(pszProvisionId, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_RESTOCKPIN_NEW);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(0 == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_ReStockPin_New_XML(szXmlBuffer, uiXmlLen, pszProvisionId, IsContinueDownload, puiCount, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}

enCodeReturn PendingOrder_XML(RESPONSE_HEADER_TAG *pstRespH, \
							  RESPONSE_BODY_TAG *pstRespB, \
							  unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_PendingOrder_XML(szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_PENDINGORDER);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(0 == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_PendingOrder_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}

// Modified by Kim_LinHB 2013-7-6 v4.62
enCodeReturn SalesPolling_XML(unsigned char bAuto, \
							  unsigned int uiRest, \
							  RESPONSE_HEADER_TAG *pstRespH, \
							  RESPONSE_BODY_TAG *pstRespB, \
							  unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;
	unsigned char bContinue = FALSE;
	unsigned int start = 0;
	
	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	// Modified by Kim_LinHB 2013-7-6 v4.62
	if(OK == enCr)
	{
		do{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));

			Pack_SalesPolling_XML(bAuto, &start, &uiRest, &bContinue, szXmlBuffer, &uiXmlLen);
			// Zip first
			enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_SALASPOLLING);
			if(enCr != OK)
			{	return enCr;}
			enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(0 == enCr)
			{
				memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
				memset(sConnBuffer, 0, sizeof(sConnBuffer));
 				enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
 				if(OK == enCr)
 				{
 					// Unzip first
 					enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
 					if(OK == enCr)
 					{
 						enCr = UnPack_SalesPolling_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
 					}
 				}
 				else
 				{
 					if(NET_ERR_TIMEOUT == enCr)
					{	enCr = NoReponse;}
 					else
					{	enCr = IncidentReception;}
 				}
			}
		}while(OK == enCr && bContinue);
		// Modify End
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}
// Modify End

enCodeReturn Claim_XML(unsigned char  *pszSN, \
					   RESPONSE_HEADER_TAG *pstRespH, \
					   RESPONSE_BODY_TAG *pstRespB, \
					   unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Claim_XML(pszSN, szXmlBuffer, &uiXmlLen);
		// Zip first
		Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_CLAIM);
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));

			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Claim_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}

	return enCr;
}

enCodeReturn Get_Balance_XML(unsigned char *pszValue, \
							 RESPONSE_HEADER_TAG *pstRespH, \
							 RESPONSE_BODY_TAG *pstRespB, \
							 unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Get_Balance_XML(szXmlBuffer, &uiXmlLen);
		// Zip first
		Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_GETBALANCE);
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Get_Balance_XML(szXmlBuffer, uiXmlLen, pszValue, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}

	return enCr;
}

enCodeReturn Get_Mail_XML(MAIL *pstMail, \
						  unsigned short *pusNums, \
						  RESPONSE_HEADER_TAG *pstRespH, \
						  RESPONSE_BODY_TAG *pstRespB, \
						  unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Get_Mail_XML(szXmlBuffer, &uiXmlLen);
		// Zip first
		Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_GETMAIL);
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));

			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Get_Mail_XML(szXmlBuffer, uiXmlLen, pstMail, pusNums, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}

	return enCr;
}

// 5.15
enCodeReturn Cancellation_XML(TRANS_DATA stData, \
							  unsigned char IsAutomatically, \
							  RESPONSE_HEADER_TAG *pstRespH, \
							  RESPONSE_BODY_TAG *pstRespB, \
							  unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Cancellation_XML(stData, IsAutomatically, szXmlBuffer, &uiXmlLen);
		// Zip first
		Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_CANCEL);
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Cancellation_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}

	return enCr;
}

enCodeReturn Get_Users_XML(RESPONSE_HEADER_TAG *pstRespH, \
						   RESPONSE_BODY_TAG *pstRespB, \
						   unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Get_Users_XML(szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_GETUSERS);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Get_Users_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}
enCodeReturn Change_Pwd_XML(USER_INFO stByWho, \
							USER_INFO stWho,\
							RESPONSE_HEADER_TAG *pstRespH, \
							RESPONSE_BODY_TAG *pstRespB, \
							unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Change_Pwd_XML(stByWho, stWho, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_CHANGEUSERPWD);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Change_Pwd_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}
enCodeReturn Create_User_XML(USER_INFO stWho,\
							 RESPONSE_HEADER_TAG *pstRespH, \
							 RESPONSE_BODY_TAG *pstRespB, \
							 unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Create_User_XML(stWho, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_CREATECASHIER);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Create_User_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}
enCodeReturn Del_User_XML(USER_INFO stWho,\
						  RESPONSE_HEADER_TAG *pstRespH, \
						  RESPONSE_BODY_TAG *pstRespB, \
						  unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));

		Pack_Del_User_XML(stWho, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_DELCASHIER);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{
					enCr = UnPack_Del_User_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}
	return enCr;
}


enCodeReturn CashOrder_XML(TRANS_DATA_CASH stData, \
						   unsigned char *pszDate, \
						   RESPONSE_HEADER_TAG *pstRespH, \
						   RESPONSE_BODY_TAG *pstRespB, \
						   unsigned char bAutoConnected)
{
	unsigned char szXmlBuffer[MAX_TCP_BUFFER];
	unsigned char sConnBuffer[MAX_TCP_BUFFER];
	unsigned int uiXmlLen = MAX_TCP_BUFFER;
	unsigned int uiConnLen = MAX_TCP_BUFFER;

	enCodeReturn enCr = OK;

	if(pstRespH)
	{	memset(pstRespH, 0, sizeof(RESPONSE_HEADER_TAG));}
	if(pstRespB)
	{	memset(pstRespB, 0, sizeof(RESPONSE_BODY_TAG));}

	if(TRUE == bAutoConnected)
	{	enCr = Connect(gl_MaintenaceParam_Ext.cConnectionTimeout);}

	if(OK == enCr)
	{
		memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
		memset(sConnBuffer, 0, sizeof(sConnBuffer));
		Pack_CashOrder_XML(stData, pszDate, szXmlBuffer, &uiXmlLen);
		// Zip first
		enCr = Compress_Request(szXmlBuffer, sConnBuffer, &uiConnLen, MSG_MERCASHORDER);
		if(enCr != OK)
		{	return enCr;}
		enCr = Send(sConnBuffer, uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
		if(OK == enCr)
		{
			memset(szXmlBuffer, 0, sizeof(szXmlBuffer));
			memset(sConnBuffer, 0, sizeof(sConnBuffer));
			enCr = Recv(MAX_TCP_BUFFER, sConnBuffer, &uiConnLen, gl_MaintenaceParam_Ext.cConnectionTimeout);
			if(OK == enCr)
			{
				// Unzip first
				enCr = Uncompress_Response(sConnBuffer, uiConnLen, szXmlBuffer, &uiXmlLen, pstRespH);
				if(OK == enCr)
				{	
					enCr = UnPack_CashOrder_XML(szXmlBuffer, uiXmlLen, pstRespH, pstRespB);
				}
			}
			else
			{
				if(NET_ERR_TIMEOUT == enCr)
				{	enCr = NoReponse;}
				else
				{	enCr = IncidentReception;}
			}
		}
	}
	else
	{
		CloseConn();
	}

	if(TRUE == bAutoConnected)
	{	CloseConn();}

	return enCr;
}

#define MAGIC_KEY "GPXGREGABMTECHNO"
void GenerationdPOSKey(unsigned char *pszTermSN,\
					   unsigned char *pEncryptPOSKey)
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

void EncrypteCode(unsigned char *pszIn, \
				  unsigned char *pszKey, \
				  unsigned char *pszOut)
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
		if (ucByte&0x01){ ucNb1++;}
		ucByte>>=1;
	}

	return ucNb1;
}


unsigned short CalcCRC(unsigned char *pData, \
					   unsigned short usLen)
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
				{	usAcc<<=1;}
			}
		}
	}
	usFcs = 0;
	for (i=0;i<16; ++i)
	{
		usFcs<<=1;
		if (usAcc & (1<<i))
		{	usFcs |= 0x0001;}
	}
	return usFcs;
}

void Short2Ascii(unsigned short usNum,\
				 unsigned char *pAscii)
{
	pAscii[0]= (usNum & 0xFF00)>>8;
	pAscii[1]= (unsigned char)(usNum & 0x00FF);	
}

static void Pack_Parameter_XML(unsigned char *pBuffer, \
							   unsigned int *puiLenInOut)
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
	strcpy(header.szSerialNumber, SN);
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

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "Model", body.szModel);
	XmlAddAddr(&bodyXML, "AppVersion", body.szAppVer);
	XmlAddAddr(&bodyXML, "IsSetup", 1 == gl_SavParam.bIsSetup ? "1" : "0");
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Parameter_XML(unsigned char *pszXml, \
										  unsigned int uiLen, \
										  RESPONSE_HEADER_TAG *pstRespH, \
										  RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXML, MainXML, RemoteDownloadXML, PermissionListXML;//,AccessListXML;

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
 		strcpy(gl_MaintenaceParam.stConnParam.szRemotePort, XmlGetValue(&MainXML, "Port"));
		strcpy(gl_MaintenaceParam.stConnParam.szRemoteIP_Bak, XmlGetValue(&MainXML, "BackupHostIp"));
		strcpy(gl_MaintenaceParam.stConnParam.szRemotePort_Bak, XmlGetValue(&MainXML, "BackupPort"));
		// added by Allen 2016/03/08 
		strcpy(gl_MaintenaceParam.stConnParam.stParamGPRS.szAPN, XmlGetValue(&MainXML, "Apn"));
		//add end
		// Added by Kim_LinHB 2013-11-11 v4.70
		gl_MaintenaceParam_Ext.cConnectionTimeout = My_atoi((void *)XmlGetValue(&MainXML, "ConnectionTimeout"));

		strcpy(gl_MaintenaceParam.stConnParam.szNumPad, XmlGetValue(&MainXML, "Tel1"));
		strcpy(gl_MaintenaceParam.stConnParam.szNumPadSec, XmlGetValue(&MainXML, "Tel2"));
		strcpy(gl_MaintenaceParam.szDealerCode, XmlGetValue(&MainXML, "DealerCode"));
		strcpy(gl_MaintenaceParam.szMaintenancePwd, XmlGetValue(&MainXML, "MaintenancePassword"));
		strcpy(gl_MaintenaceParam.szNetworkPwd, XmlGetValue(&MainXML, "NetworkPassword"));
		gl_MaintenaceParam.usMaxRequestVoucherCount = My_atoi((void *)XmlGetValue(&MainXML, "MaxRequestVoucherCount"));
		gl_MaintenaceParam.usMaxNotifyVoucherCount = My_atoi((void *)XmlGetValue(&MainXML, "MaxNotifyVoucherCount"));
		
		// Modified by Kim_LinHB 2013-11-26 v4.74
		{
			unsigned char szSDTimer[4 + 1];
			strcpy(szSDTimer, XmlGetValue(&MainXML, "ParamDownloadTime"));
			GetCurTime(gl_MaintenaceParam.szLastSettDownLoad_Time);

			strcpy(gl_MaintenaceParam.szTimeSettDownload, szSDTimer);
			if(memcmp(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, szSDTimer, 4) <= 0)
			{
				// Modified by Kim_LinHB 2014-2-10 v4.84
				AdjustDate(gl_MaintenaceParam.szLastSettDownLoad_Time, -1);
			}
			memcpy(gl_MaintenaceParam.szLastSettDownLoad_Time + 6, szSDTimer, 4);
		}
		// Modify End
		
		// Removed by Kim_LinHB 2013-11-14 v4.73
		//strcpy(gl_MaintenaceParam.szTimeSalesPolling, XmlGetValue(&MainXML, "NotifySoldVouchersTime"));
		
		// Added by Kim_LinHB 2013-11-14 v4.73
		strcpy(gl_MaintenaceParam_Ext.szDuration, XmlGetValue(&MainXML, "NotifySoldVouchersPeriod"));
		GetCurTime(gl_MaintenaceParam.szLastSalesPolling_Time);	//service up

		gl_MaintenaceParam.ucMerchantMode = My_atoi((void *)XmlGetValue(&MainXML, "MerchantMode")); // Modified by Kim_LinHB 2013-5-22 v4.59

		// Added by Kim_LinHB 2013-6-19 v4.61
		strcpy(gl_MaintenaceParam_Ext.szHeader1, XmlGetValue(&MainXML, "Header1"));
		strcpy(gl_MaintenaceParam_Ext.szHeader2, XmlGetValue(&MainXML, "Header2"));
		strcpy(gl_MaintenaceParam_Ext.szHeader3, XmlGetValue(&MainXML, "Header3"));
		// Add End
		gl_MaintenaceParam.bPrintMerchantCopy = My_atoi((void *)XmlGetValue(&MainXML, "MerchantCopy")); // Added by Kim_LinHB 2014-3-11

		XmlGetChild(&bodyXML,"PermissionList",0,&PermissionListXML);
		gl_MaintenaceParam.bPrintBarCode = My_atoi((void *)XmlGetValue(&PermissionListXML, "PrintBarCode"));
		gl_MaintenaceParam.bPrintLogo = My_atoi((void *)XmlGetValue(&PermissionListXML, "PrintLogo"));
		gl_MaintenaceParam.bCancelSale = My_atoi((void *)XmlGetValue(&PermissionListXML, "CancelSale"));
//		gl_MaintenaceParam.bChangeDiscount = My_atoi((void *)XmlGetValue(&PermissionListXML, "ChangeDiscount"));
		gl_MaintenaceParam.bScanSerialNumber = My_atoi((void *)XmlGetValue(&PermissionListXML, "ScanSerialNumber"));
		gl_MaintenaceParam.bInputSerial = My_atoi((void *)XmlGetValue(&PermissionListXML, "InputSerial"));
		
		XmlGetChild(&bodyXML,"RemoteDownload",0,&RemoteDownloadXML);
		if(RemoteDownloadXML.start)
		{
			// Modified by Kim_LinHB 2013-11-26 v4.74
			unsigned char szTmpTime[5 + 1];
			strcpy(szTmpTime, XmlGetValue(&RemoteDownloadXML, "DownloadDateTime"));
			if(strlen(szTmpTime) > 0)
			{
				strcpy(szTmpTime + 2, szTmpTime + 3);
				strcpy(gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, szTmpTime);
			}
			GetCurTime(gl_MaintenaceParam.szLastSoftDownLoad_Time);
		
			if(memcmp(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4) <= 0)
			{	
				// Modified by Kim_LinHB 2014-2-10 v4.84
				AdjustDate(gl_MaintenaceParam.szLastSoftDownLoad_Time, -1);
			}
			memcpy(gl_MaintenaceParam.szLastSoftDownLoad_Time + 6, gl_MaintenaceParam.stFTP.szTimeSoftDownLoad, 4);
			// Modify End		

			gl_MaintenaceParam.stFTP.bForceToDownload = My_atoi((void *)XmlGetValue(&RemoteDownloadXML, "ForceTerminalToDownload"));
			// Modified by Kim_LinHB 2013-3-25 v4.56
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

		WriteParam(WRT_MAINTA);
	}
	return ucRet;
}

static void Pack_ProvisionId_XML(USER_INFO stCur, \
								 unsigned char *pBuffer, \
								 unsigned int *puiLenInOut)
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
	strcpy(header.szUserID, stCur.szName);
	strcpy(header.szPWD, stCur.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "UtilityCompanyCode", "111");
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_ProvisionId_XML(unsigned char *pszXml, \
											unsigned int uiLen, \
											RESPONSE_HEADER_TAG *pstRespH, \
											RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	memset(gl_ProvisionIdTmp, 0, sizeof(gl_ProvisionIdTmp));
	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		strcpy(gl_ProvisionIdTmp, XmlGetValue(&bodyXML,"ProvisionId"));
	}
	return ucRet;
}

static void Pack_CategoryPackageList_XML(unsigned int uiRequestIndex, \
										 unsigned char *pBuffer, \
										 unsigned int *puiLenInOut)
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

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	{
		unsigned char szTmp[3];
		sprintf(szTmp, "%u", uiRequestIndex);
		XmlAddAddr(&bodyXML, "RequestIndex", szTmp);
	}
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_CategoryPackageList_XML(unsigned char *pszXml, \
													unsigned int uiLen, \
													unsigned char *IsContinueDownload,\
													RESPONSE_HEADER_TAG *pstRespH, \
													RESPONSE_BODY_TAG *pstRespB,\
													SAV_PARAM *pstSav)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
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
#ifdef FARSI	
				{
					unsigned char szNameProduct_AR[SIZE_MAX_LIBELLE_PRODUCT * 2+1];
					strcpy(szNameProduct_AR, XmlGetValue(&ProductCategory, "ArabicDescription"));
					UTF8ToAr(szNameProduct_AR, strlen(szNameProduct_AR), pstSav->stSavParamCategory[pstSav->ucCategoryNum].szName_AR);
				}
#endif				

				while(pstSav->ucProductsNum < NB_MAX_PRODUCTS)
				{
					XmlGetChild(&ProductCategory,"Product",j,&Product);
					if(Product.start != NULL)
					{	++j;}
					else
					{	break;}
					pstSav->stSavParamProducts[pstSav->ucProductsNum].uiCodeCategory = pstSav->stSavParamCategory[pstSav->ucCategoryNum].uiCodeCategory;
					pstSav->stSavParamProducts[pstSav->ucProductsNum].ucCategory_Index = (unsigned char)pstSav->ucCategoryNum; // Added by Kim_LinHB 2013-3-22 v4.56
					pstSav->stSavParamProducts[pstSav->ucProductsNum].ucType = pstSav->stSavParamCategory[pstSav->ucCategoryNum].ucCategoryType;
					strcpy(pstSav->stSavParamProducts[pstSav->ucProductsNum].szCodeProduct, XmlGetValue(&Product, "Id"));
					strcpy(pstSav->stSavParamProducts[pstSav->ucProductsNum].szNameProduct, XmlGetValue(&Product, "Description"));
#ifdef FARSI
					{
						unsigned char szNameProduct_AR[SIZE_MAX_LIBELLE_PRODUCT * 2+1];
						strcpy(szNameProduct_AR, XmlGetValue(&Product, "ArabicDescription"));
						UTF8ToAr(szNameProduct_AR, strlen(szNameProduct_AR), pstSav->stSavParamProducts[pstSav->ucProductsNum].szNameProduct_AR);
					}
#endif
					strcpy(pstSav->stSavParamProducts[pstSav->ucProductsNum].szAmount, XmlGetValue(&Product, "Amount"));
					strcpy(pstSav->stSavParamProducts[pstSav->ucProductsNum].szDiscount, XmlGetValue(&Product, "Discount"));
					++pstSav->ucProductsNum;
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


static void Pack_Merchant_OL_Evoucher_XML(TRANS_DATA stCur, \
										  unsigned char *pBuffer, \
										  unsigned int *puiLenInOut)
{
	unsigned char SN[32 + 1];

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, headerXML, bodyXML;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_MEROLEVOUCHER);
	strcpy(header.szSerialNumber, SN);

	if(gl_bCashier_Login)
	{
		strcpy(header.szUserID, gl_Cur_Cashier.szName);
		strcpy(header.szPWD, gl_Cur_Cashier.szPwd);
	}
	else
	{
		strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
		strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	}
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////
	XmlGetChild(&reqXML, "Header", 0, &headerXML);

	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stCur.szProvisionID);
	XmlAddAddr(&bodyXML, "PackageId", stCur.stVoucher.szProductId);

	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Merchant_OL_Evoucher_XML(unsigned char *pszXml, \
													 unsigned int uiLen, \
													 TRANS_DATA *pstCur, \
													 RESPONSE_HEADER_TAG *pstRespH, \
													 RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXML;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		if(bodyXML.start)
		{
			strcpy(pstCur->stVoucher.szSlipMsg, XmlGetValue(&bodyXML, "SlipMessage"));
			strcpy(pstCur->stVoucher.szCurrDeposit, XmlGetValue(&bodyXML,"CurrentDeposit"));
			strcpy(pstCur->stVoucher.szSerial, XmlGetValue(&bodyXML,"VoucherSerial"));
			strcpy(pstCur->stVoucher.szNum, XmlGetValue(&bodyXML,"VoucherNumber"));
			strcpy(pstCur->stVoucher.szExpirDate, XmlGetValue(&bodyXML,"VoucherExpireDate"));
		}
	}
	return ucRet;
}

static void Pack_ReStockPins_XML(unsigned char *pszProvisionId, \
								 unsigned char *pBuffer, \
								 unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_RESTOCKPIN);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
	strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////

	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", pszProvisionId);
	{
		unsigned char szTmp[5];
		unsigned int uiMaxTmp = FindCodesCnt(CODE_RESTOCK_EMPTY, NULL);

		if(gl_MaintenaceParam.usMaxRequestVoucherCount <= uiMaxTmp)
		{	sprintf(szTmp, "%d", gl_MaintenaceParam.usMaxRequestVoucherCount);}
		else
		{	sprintf(szTmp, "%u", uiMaxTmp);}
		XmlAddAddr(&bodyXML, "MaxRequestVoucherCount", szTmp);
	}
	
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_ReStockPins_XML(unsigned char *pszXml, \
											unsigned int uiLen, \
											unsigned char *IsContinueDownload, \
											unsigned int *puiCount, \
											RESPONSE_HEADER_TAG *pstRespH, \
											RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXML, Vouchers, Voucher;
		unsigned int i = 0;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		if(bodyXML.start)
		{
			STOCK stTemp;
			ReadStock_All(&stTemp);
			*IsContinueDownload = My_atoi((void *)XmlGetValue(&bodyXML, "IsContinueDownload"));
			XmlGetChild(&bodyXML, "Vouchers", 0, &Vouchers);
			while(Vouchers.start)
			{
				XmlGetChild(&Vouchers, "Voucher", i, &Voucher);
				if(Voucher.start)
				{
					unsigned int j;
					for(j = 0; j < NB_MAX_CODE_STOCK; ++j)
					{
						if(CODE_RESTOCK_EMPTY == stTemp.stStock[j].ucProductState)
						{
							strcpy(stTemp.stVoucher[j].szProductId, XmlGetValue(&Voucher, "ProductId"));
							strcpy(stTemp.stVoucher[j].szVoucherId, XmlGetValue(&Voucher, "VoucherId"));
							strcpy(stTemp.stVoucher[j].szSerial, XmlGetValue(&Voucher, "Serial"));
							strcpy(stTemp.stVoucher[j].szNum, XmlGetValue(&Voucher, "Number"));
							strcpy(stTemp.stVoucher[j].szExpirDate, XmlGetValue(&Voucher, "ExpireDate"));
							stTemp.stStock[j].ucProductState = CODE_RESTOCK_DOWNLOADING;
							++*puiCount;
							break;
						}
					}
					++i;
				}
				else
				{
					WriteStock_All(&stTemp);
					if(0 == FindCodesCnt(CODE_RESTOCK_EMPTY, NULL))
					{
						*IsContinueDownload  = 0;
						ucRet = PbSalesPolling;
					}
					//WriteStock();
					break;
				}
			}
		}
		else
		{	ucRet = 1;} // Added by Kim_LinHB 2013-7-4 v4.61
	}
	return ucRet;
}

// Added by Kim_LinHB 2013-12-4 v4.76
static void Pack_ReStockPin_New_XML(unsigned char *pszProvisionId, \
									unsigned char *pBuffer, \
									unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_RESTOCKPIN_NEW);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
	strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////

	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", pszProvisionId);
	{
		unsigned char tmp[5];
		unsigned int MaxTmp = FindCodesCnt(CODE_RESTOCK_EMPTY, NULL);

		if(gl_MaintenaceParam.usMaxRequestVoucherCount <= MaxTmp)
		{	sprintf(tmp, "%d", gl_MaintenaceParam.usMaxRequestVoucherCount);}
		else
		{	sprintf(tmp, "%u", MaxTmp);}
		XmlAddAddr(&bodyXML, "MaxRequestVoucherCount", tmp);
	}

	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_ReStockPin_New_XML(unsigned char *pszXml, \
											   unsigned int uiLen, \
											   const unsigned char *pszProvisionID,\
											   unsigned char *IsContinueDownload, \
											   unsigned int *puiCount, \
											   RESPONSE_HEADER_TAG *pstRespH, \
											   RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXML, Vouchers, Voucher;
		unsigned int i = 0;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		if(bodyXML.start)
		{
			STOCK stStock;
			STOCK_NEW stOrder;

			memset(&stOrder, 0, sizeof(STOCK_NEW));
			ReadStock_All(&stStock);
			*IsContinueDownload = My_atoi((void *)XmlGetValue(&bodyXML, "IsContinueDownload"));
			
			strcpy(stOrder.szOrderId, XmlGetValue(&bodyXML, "OrderId"));
			GetStockOrderById(stOrder.szOrderId, &stOrder); // Added by Kim_LinHB 2013-12-17 v4.78
			strcpy(stOrder.szExpireDate, XmlGetValue(&bodyXML, "ExpireDate"));
			strcpy(stOrder.szOrderDate, XmlGetValue(&bodyXML, "OrderDate"));
			//Added By Kim 20150728 v5.08
			strcpy(stOrder.szProvisionID, pszProvisionID);
			UpdateStockOrder(&stOrder);

			XmlGetChild(&bodyXML, "Vs", 0, &Vouchers);
			while(Vouchers.start)
			{
				XmlGetChild(&Vouchers, "V", i, &Voucher);
				if(Voucher.start)
				{
					unsigned int j;
					for(j = 0; j < NB_MAX_CODE_STOCK; ++j)
					{
						if(CODE_RESTOCK_EMPTY == stStock.stStock[j].ucProductState)
						{
							strcpy(stStock.stVoucher[j].szProductId, XmlGetValue(&Voucher, "PId"));
							strcpy(stStock.stVoucher[j].szVoucherId, XmlGetValue(&Voucher, "VId"));
							strcpy(stStock.stVoucher[j].szSerial, XmlGetValue(&Voucher, "S"));
							strcpy(stStock.stVoucher[j].szNum, XmlGetValue(&Voucher, "N"));
							strcpy(stStock.stVoucher[j].szExpirDate, stOrder.szExpireDate);
							stStock.stStock[j].ucProductState = CODE_RESTOCK_DOWNLOADING;
							memcpy(&stOrder.stVoucher[stOrder.num++], &stStock.stVoucher[j], sizeof(VOUCHER_INFO));
							++*puiCount;
							break;
						}
					}
					++i;
				}
				else
				{
					//1 if power off here, status will be: no downloading, has provid, marched provid order num == 0, just restock pin again
					WriteStock_All(&stStock);
					//2 if power off here, status will be: has downloading, has provid, marched provid order num == 0, downloading -> free, remove provid, need to UpdateStockOrder again, print
					UpdateStockOrder(&stOrder);
					//3 if power off here, status will be: has downloading, has provid, marched provid order num > 0, downloading -> free, remove provid, print
					if(0 == FindCodesCnt(CODE_RESTOCK_EMPTY, NULL))
					{
						*IsContinueDownload  = 0;
						ucRet = PbSalesPolling;
					}  
					//WriteStock();
					break;
				}
			}
		}
		else
		{	ucRet = 1;} // Added by Kim_LinHB 2013-7-4 v4.61
	}
	return ucRet;
}

static void Pack_PendingOrder_XML(unsigned char *pBuffer, \
								  unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_PENDINGORDER);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
	strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////

	XmlAddChildNULL("Body", &reqXML, &bodyXML);

	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_PendingOrder_XML(unsigned char *pszXml, \
											 unsigned int uiLen, \
											 RESPONSE_HEADER_TAG *pstRespH, \
											 RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXML;
		unsigned int i = 0;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		if(bodyXML.start)
		{
			xml_t Orders;

			XmlGetChild(&bodyXML, "Orders", 0, &Orders);
			while(Orders.start)
			{
				unsigned int j = 0;
				xml_t Order;
				PENDING_ORD stPendingOrder;

				XmlGetChild(&Orders, "Order", i, &Order);
				memset(&stPendingOrder, 0, sizeof(stPendingOrder));

				if(Order.start)
				{
					strcpy(stPendingOrder.szOrderId, XmlGetValue(&Order, "Id"));
					strcpy(stPendingOrder.szDate, XmlGetValue(&Order, "Date"));

					while(Order.start)
					{
						xml_t product;
						
						XmlGetChild(&Order, "Product", j, &product);
						if (product.start)
						{
							strcpy(stPendingOrder.stOrder[stPendingOrder.num].PId, XmlGetValue(&product, "Id"));
							strcpy(stPendingOrder.stOrder[stPendingOrder.num].szName, XmlGetValue(&product, "Name"));
							stPendingOrder.stOrder[stPendingOrder.num].num = My_atoi((void*)XmlGetValue(&product, "RemindQuantity"));
							++stPendingOrder.num;
							++j;
						}
						else
						{	break;}
					}
					if(stPendingOrder.num > 0)
					{	WritePendingOrder(stPendingOrder);}
					++i;
				}
				else
				{	break;}
			}
		}
		else
		{	ucRet = 1;}
	}
	return ucRet;
}

// Add End

// Modified by Kim_LinHB 2013-7-6 v4.62
static unsigned int Pack_SalesPolling_XML(unsigned char bAuto, \
										  unsigned int *puiStart, \
										  unsigned int *puiRest, \
										  unsigned char *bContinue, \
										  unsigned char *pBuffer, \
										  unsigned int *puiLenInOut)
{
	unsigned int i, iCnt;
	unsigned char SN[32 + 1];
	unsigned char szStatus[4];
	STOCK_INFO stStockInfo;
	VOUCHER_INFO stVoucherInfo;

	REQUEST_HEADER_TAG	header;
	REQUEST_BODY_TAG	body;

	xml_t reqXML, bodyXML, Vouchers, Voucher;
	xml_t root = {"", pBuffer, pBuffer + MAX_TCP_BUFFER, NULL};

	memset(&header, 0, sizeof(REQUEST_HEADER_TAG));
	memset(&body, 0, sizeof(REQUEST_BODY_TAG));

	memset(SN, 0, sizeof (SN));
	GetSerialNumber(SN);

	strcpy(header.szMsgType, MSG_SALASPOLLING);
	strcpy(header.szSerialNumber, SN);

	strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
	strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////

	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "IsAutomatically", bAuto ? "1" : "0");

	XmlAddChildNULL("Vouchers", &bodyXML, &Vouchers);
	*bContinue = FALSE;
	for(i = *puiStart, iCnt = 0; i < NB_MAX_CODE_STOCK; ++i)
	{
		ReadStock(i, &stStockInfo, &stVoucherInfo);
		if(CODE_RESTOCK_SOLD == stStockInfo.ucProductState ||
			CODE_RESTOCK_EXPIRED == stStockInfo.ucProductState ||
			CODE_RESTOCK_SALESPOLLING == stStockInfo.ucProductState ||
			CODE_RESTOCK_SELLING == stStockInfo.ucProductState)
		{
			// Added by Kim_LinHB 2013-5-3 v4.58
			if(0 == strlen(stVoucherInfo.szVoucherId))
			{	continue;}
			// Add End
			++iCnt;
			++*puiStart;
			XmlAddChildNULL("Voucher", &Vouchers, &Voucher);
			// Modified by Kim_LinHB 2013-7-2 v4.61
			if(strstr(stVoucherInfo.szVoucherId, stVoucherInfo.szSerial) != NULL) // means the data is overflow
			{
				XmlAddAddr(&Voucher, "VoucherId", stVoucherInfo.szSerial);
				if(CODE_RESTOCK_SALESPOLLING == stStockInfo.ucProductState)
				{	sprintf(szStatus, "%d", 50 + CODE_RESTOCK_SOLD);}
				else if(CODE_RESTOCK_SELLING == stStockInfo.ucProductState)
				{	sprintf(szStatus, "%d", 50 + 12);}
				else
				{	sprintf(szStatus, "%d", 50 + stStockInfo.ucProductState);}
			}
			else
			{
				XmlAddAddr(&Voucher, "VoucherId", stVoucherInfo.szVoucherId);
				if(CODE_RESTOCK_SALESPOLLING == stStockInfo.ucProductState)
				{	sprintf(szStatus, "%d", CODE_RESTOCK_SOLD);}
				else if(CODE_RESTOCK_SELLING == stStockInfo.ucProductState)
				{	sprintf(szStatus, "%d", 12);}
				else
				{	sprintf(szStatus, "%d", stStockInfo.ucProductState);}
			}
				
			XmlAddAddr(&Voucher, "DVStatus", szStatus);
			// Modified by Kim_LinHB 2013-3-25 v4.56
			if(CODE_RESTOCK_SOLD == stStockInfo.ucProductState || 
				CODE_RESTOCK_SALESPOLLING == stStockInfo.ucProductState ||
				CODE_RESTOCK_SELLING == stStockInfo.ucProductState)
			{	XmlAddAddr(&Voucher, "SoldDate", stStockInfo.szSoldDate);}
			else if(CODE_RESTOCK_EXPIRED == stStockInfo.ucProductState)
			{	XmlAddAddr(&Voucher, "SoldDate", "");}
			// Modify End
		
			if(gl_MaintenaceParam.usMaxNotifyVoucherCount == iCnt)
			{
				*bContinue = TRUE;
				break;
			}
			// Modify End
		}
	}

	*puiRest -= iCnt;
	if(0 == *puiRest)
	{	*bContinue = FALSE;}
	*puiLenInOut = root.end - root.start;
	return iCnt;
}
static unsigned char UnPack_SalesPolling_XML(unsigned char *pszXml, \
											 unsigned int uiLen, \
											 RESPONSE_HEADER_TAG *pstRespH, \
											 RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_Claim_XML(unsigned char *pszVoucherSN, \
						   unsigned char *pBuffer, \
						   unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_CLAIM);
	strcpy(header.szSerialNumber, SN);
	if(gl_bCashier_Login)
	{
		strcpy(header.szUserID, gl_Cur_Cashier.szName);
		strcpy(header.szPWD, gl_Cur_Cashier.szPwd);
	}
	else
	{
		strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
		strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	}
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "VoucherSerialNumber", pszVoucherSN);

	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Claim_XML(unsigned char *pszXml, \
									  unsigned int uiLen, \
									  RESPONSE_HEADER_TAG *pstRespH, \
									  RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_Get_Balance_XML(unsigned char *pBuffer, \
								 unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_GETBALANCE);
	strcpy(header.szSerialNumber, SN);
	if(gl_bCashier_Login)
	{
		strcpy(header.szUserID, gl_Cur_Cashier.szName);
		strcpy(header.szPWD, gl_Cur_Cashier.szPwd);
	}
	else
	{
		strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
		strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	}
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////
	/////////////////////////////////////
	XmlAddChildNULL("Body", &reqXML, &bodyXML);

	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Get_Balance_XML(unsigned char *pszXml, \
											unsigned int uiLen, \
											unsigned char *pszValue, \
											RESPONSE_HEADER_TAG *pstRespH, \
											RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};
	unsigned char szTmp[12 + 1];

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXML;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		if(bodyXML.start)
		{
			strcpy(szTmp, XmlGetValue(&bodyXML, "Balance"));
			strcpy(pszValue, szTmp);
		}
	}
	return ucRet;
}

static void Pack_Get_Mail_XML(unsigned char *pBuffer, \
							  unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_GETMAIL);
	strcpy(header.szSerialNumber, SN);
	if(gl_bCashier_Login)
	{
		strcpy(header.szUserID, gl_Cur_Cashier.szName);
		strcpy(header.szPWD, gl_Cur_Cashier.szPwd);
	}
	else
	{
		strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
		strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	}
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////
	/////////////////////////////////////
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	{
		unsigned char szTmp[5];
		sprintf(szTmp, "%d", gl_MaintenaceParam.ucLastReceivedMailId);
		XmlAddAddr(&bodyXML, "LastReceivedMailId", szTmp);
	}
	
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Get_Mail_XML(unsigned char *pszXml, \
										 unsigned int uiLen, \
										 MAIL *pstMail, \
										 unsigned short *pusNums, \
										 RESPONSE_HEADER_TAG *pstRespH, \
										 RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		xml_t bodyXML, MailList, MailXML, LineXML;
		unsigned int i = 0;
		unsigned char Ar_Buffer[1024];
		ucRet = OK;
		memset(pstMail, 0, sizeof(MAIL));
		XmlGetChild(&reqXML, "Body", 0, &bodyXML);
		XmlGetChild(&bodyXML, "MailList", 0, &MailList);
		while(MailList.start)
		{
			unsigned int j = 0;
			XmlGetChild(&MailList, "Mail", i, &MailXML);
			if(MailXML.start)
			{
				gl_MaintenaceParam.ucLastReceivedMailId = My_atoi((void *)XmlGetValue(&MailXML, "Id"));
				strcpy(pstMail[i].szModifiedOn, XmlGetValue(&MailXML, "ModifiedOn"));
				strcpy(Ar_Buffer, XmlGetValue(&MailXML, "Caption"));
				UTF8ToAr(Ar_Buffer, strlen(Ar_Buffer), pstMail[i].szCaption);
				while(1)
				{
					XmlGetChild(&MailXML, "Line", j, &LineXML);
					if (LineXML.start)
					{
						strcpy(Ar_Buffer, XmlGetText(&LineXML));
						UTF8ToAr(Ar_Buffer, strlen(Ar_Buffer), pstMail[i].szContent[j]);
						++j;
					}
					else
					{
						pstMail[i].usContentNums = j;
						break;
					}
				}
				++i;
			}
			else
			{
				*pusNums = i;
				//WriteParam();
				break;
			}
		}
	}
	return ucRet;
}


static void Pack_Cancellation_XML(TRANS_DATA stData, \
								  unsigned char IsAutomatically, \
								  unsigned char *pBuffer, \
								  unsigned int *puiLenInOut)
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
	strcpy(header.szSerialNumber, SN);
	if((gl_bCashier_Login || strlen(stData.szCashier) > 0) && 
		OK == ReadUsersPwd(stData.szCashier,header.szPWD)) // Modified by Kim_LinHB 2014-5-12
	{
		strcpy(header.szUserID, stData.szCashier);
		//strcpy(header.szPWD, gl_Cur_Cashier.szPwd);
	}
	else
	{
		strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
		strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	}
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, stData.szDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	/////////////////////////////////////
	
	/////////////////////////////////////
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);
	switch(stData.ucTransType)
	{
		case ID_SaleCodes:XmlAddAddr(&bodyXML, "RefMessageType", MSG_MEROLEVOUCHER);break;
		case ID_CashOrder:XmlAddAddr(&bodyXML, "RefMessageType", MSG_MERCASHORDER);break;
	}
	XmlAddAddr(&bodyXML, "IsAutomatically", IsAutomatically ? "1" : "0"); // Added by Kim_LinHB 2014-08-14 v4.95

	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Cancellation_XML(unsigned char *pszXml, \
											 unsigned int uiLen, \
											 RESPONSE_HEADER_TAG *pstRespH, \
											 RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}


static void Pack_Get_Users_XML(unsigned char *pBuffer, \
							   unsigned int *puiLenInOut)
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
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, "");
	strcpy(header.szPWD, "");
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Get_Users_XML(unsigned char *pszXml, \
										  unsigned int uiLen, \
										  RESPONSE_HEADER_TAG *pstRespH, \
										  RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		int i = 0;
		xml_t bodyXML, Users, User;
		USER_INFO szTmp;
		ucRet = OK;
		XmlGetChild(&reqXML,"Body",0,&bodyXML);
		XmlGetChild(&bodyXML,"Users",0,&Users);
		while(Users.start)
		{
			XmlGetChild(&Users,"User",i,&User);
			if(User.start != NULL)
			{
				strcpy(szTmp.szName, XmlGetValue(&User, "Username"));
				strcpy(szTmp.szPwd, XmlGetValue(&User, "Password"));

				if(0 == strcmp(szTmp.szName, "manager"))
				{	memcpy(&gl_MaintenaceParam.stManage, &szTmp, sizeof(USER_INFO));}
				else
				{	WriteUsers(szTmp.szName, szTmp.szPwd);}
				++i;
			}
			else
			{	break;}
		}
		WriteParam(WRT_MAINTA);
	}
	return ucRet;
}

static void Pack_Change_Pwd_XML(USER_INFO stByWho, \
								USER_INFO stWho, \
								unsigned char *pBuffer, \
								unsigned int *puiLenInOut)
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
	strcpy(header.szUserID, stByWho.szName);
	strcpy(header.szPWD, stByWho.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "Username", stWho.szName);
	XmlAddAddr(&bodyXML, "NewPassword", stWho.szPwd);
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Change_Pwd_XML(unsigned char *pszXml, \
										   unsigned int uiLen, \
										   RESPONSE_HEADER_TAG *pstRespH, \
										   RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_Create_User_XML(USER_INFO stWho, \
								 unsigned char *pBuffer, \
								 unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_CREATECASHIER);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
	strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "Username", stWho.szName);
	XmlAddAddr(&bodyXML, "Password", stWho.szPwd);
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Create_User_XML(unsigned char *pszXml, \
											unsigned int uiLen, \
											RESPONSE_HEADER_TAG *pstRespH, \
											RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_Del_User_XML(USER_INFO stWho, \
							  unsigned char *pBuffer, \
							  unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_DELCASHIER);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
	strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", gl_SavParam.usTraceNo);
	GetCurTime(header.szTermDateTime);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "Username", stWho.szName);
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_Del_User_XML(unsigned char *pszXml, \
										 unsigned int uiLen, \
										 RESPONSE_HEADER_TAG *pstRespH, \
										 RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}

static void Pack_CashOrder_XML(TRANS_DATA_CASH stData, \
							   unsigned char *pszDate, \
							   unsigned char *pBuffer, \
							   unsigned int *puiLenInOut)
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

	strcpy(header.szMsgType, MSG_MERCASHORDER);
	strcpy(header.szSerialNumber, SN);
	strcpy(header.szUserID, gl_MaintenaceParam.stManage.szName);
	strcpy(header.szPWD, gl_MaintenaceParam.stManage.szPwd);
	strcpy(header.szParamVer, PARAMVER);
	sprintf(header.szBatchNo, "%d", gl_SavParam.usBatchNo);
	sprintf(header.szTraceNo, "%d", stData.usTraceNo);
	strcpy(header.szTermDateTime, pszDate);
	strcpy(header.szTermId, gl_MaintenaceParam.szTermID);
	strcpy(header.szLangCode, c_LANG[en_US]);

	Set_Header_XML(&header, pBuffer, puiLenInOut);
	root.end = root.start + *puiLenInOut;

	XmlGetChild(&root,"Request",0,&reqXML);
	XmlAddChildNULL("Body", &reqXML, &bodyXML);
	XmlAddAddr(&bodyXML, "ProvisionId", stData.szProvisionID);

	XmlAddChildNULL("OrderItems", &bodyXML, &orderItemsXML);
	for(iNum = 0; iNum < stData.ucProductNum; ++iNum)
	{
		unsigned int k;
		XmlAddChildNULL("OrderItem", &orderItemsXML, &orderItemXML);
		XmlAddAddr(&orderItemXML, "ProductId", stData.stProducts[iNum].szCodeProduct);
		XmlAddAddr(&orderItemXML, "Quantity", stData.stProducts[iNum].szQuantity);
		for(k =0; k < gl_SavParam.ucProductsNum; ++k)
		{
			if(0 == strcmp(gl_SavParam.stSavParamProducts[k].szCodeProduct, stData.stProducts[iNum].szCodeProduct))
			{
				XmlAddAddr(&orderItemXML, "Discount", gl_SavParam.stSavParamProducts[k].szDiscount);
				break;
			}
		}
	}
	*puiLenInOut = root.end - root.start;
}
static unsigned char UnPack_CashOrder_XML(unsigned char *pszXml, \
										  unsigned int uiLen, \
										  RESPONSE_HEADER_TAG *pstRespH, \
										  RESPONSE_BODY_TAG *pstRespB)
{
	unsigned char ucRet = OK;
	xml_t reqXML, headerXML;
	xml_t root = {"", pszXml, pszXml + uiLen, NULL};

	XmlGetChild(&root,"Response",0,&reqXML);
	XmlGetChild(&reqXML,"Header",0,&headerXML);

	ucRet = GetHeader(pstRespH, headerXML);
	if(0 == ucRet)
	{
		ucRet = OK;
	}
	return ucRet;
}


//////////////////////////////////////////////////////////////////////////

static int ZipUnzip(unsigned char *psInData, \
					unsigned int iInLen, \
					unsigned char *psOutData, \
					unsigned int *piOutLen, \
					unsigned char compressionMode)
{
	int iRetVal=0;

	switch (compressionMode)
	{
	case 0:
		{
			int extra =0;
			int i;

			iRetVal = compress(psOutData, piOutLen, psInData, iInLen);
			if(iRetVal!=0)
			{
				vTrace("compress error", (void *)&iRetVal, sizeof(int), TRACE_ERROR, TYPE_N); // Added by Kim_LinHB 2013-3-28 v4.56
				iRetVal = 152; // Modified by Kim_LinHB 2014-5-8
			}
			if(iRetVal == 0)
			{
				extra = 8 - *piOutLen % 8;
				for ( i = 0; i < extra; i++)
				{	psOutData[*piOutLen + i] = (unsigned char)extra;}
				*piOutLen += extra;
			}
		}
		break;
	case 1:
		*piOutLen = MAX_TCP_BUFFER;
		iRetVal = uncompress(psOutData, (uLongf)piOutLen,psInData, iInLen);
		if(iRetVal!=0)
		{
			vTrace("uncompress error", (void *)&iRetVal, sizeof(int), TRACE_ERROR, TYPE_N); // Added by Kim_LinHB 2013-3-28 4.56
			iRetVal = 153; // Modified by Kim_LinHB 2014-5-8
		}
		break;
	}

	return iRetVal;
}

static int Compress_Request(unsigned char *psInData, \
							unsigned char *psOutData, \
							unsigned int *piOutLen, \
							const unsigned char *MsgType)
{
	int iRet;
	unsigned char *key;int iGenerateKeyIndex;
	int pos = 0;
	unsigned char sCompressedDataSection[MAX_TCP_BUFFER - 27];
	int sCompressedDataSection_Length=sizeof(sCompressedDataSection);
	int i,j;
	unsigned char ss[32 + 1];
	unsigned char sEncryptedHeader[16];
	unsigned char szB64enCodedEncryptedHeader[200];
	unsigned char szConnectionType[1 + 1];

#if defined(LHB_DEBUG) && defined(LHB_TEST)
	{
#ifdef _WIN32
		PubDebugOutput("",sInData,strlen(sInData),DEVICE_COM2,ASC_MODE);
#else
		PubDebugOutput("",sInData,strlen(sInData),DEVICE_COM1,ASC_MODE);
#endif
	}
#endif

	key=GetRandomKey(&iGenerateKeyIndex);
	memset(sCompressedDataSection,0,sizeof(sCompressedDataSection));
	iRet = ZipUnzip(psInData, strlen(psInData), sCompressedDataSection, &sCompressedDataSection_Length, 0);
	if(iRet!=0)
 	{
 		return iRet;
 	}

	psOutData[pos++] = STX; 
	psOutData[pos++] =(unsigned char)iGenerateKeyIndex;

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

	//note:gl_TermSN = 11111111, 22222222, can cause gateway error 2014-5-8
	sprintf(ss,"%02.2s%01.1s%012.12s%04.4s%04.4s%02.2s%05.5d%02.2s",
		"05", szConnectionType, gl_TermSN, "0111", "0000",
		MsgType, sCompressedDataSection_Length + 1, "0000");

	PubAsc2Bcd(ss, 32, ss);
	EncrypteCode(ss, key, sEncryptedHeader);

	memset(szB64enCodedEncryptedHeader,0,sizeof(szB64enCodedEncryptedHeader));
	Base64Encode(sEncryptedHeader,szB64enCodedEncryptedHeader,sizeof(sEncryptedHeader));
	strcpy(psOutData + pos,szB64enCodedEncryptedHeader);
	pos += 24;
	j=sCompressedDataSection_Length/8;
	for(i = 0; i < j; ++i)
	{	PubDes(TRI_ENCRYPT, sCompressedDataSection+i*8, key, psOutData+pos+i*8);}
	
	pos+=sCompressedDataSection_Length;
	psOutData[pos++] = ETX; 
	*piOutLen =pos + 1;
	return OK;
}

static int Uncompress_Response(unsigned char *psBuf, \
							   unsigned int uiConnLen, \
							   unsigned char *psXmlData,\
							   unsigned int *puiXmlDataSize,\
							   RESPONSE_HEADER_TAG *pstRespH)
{
#define LHB_BLOCK  512
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
	
	if (psBuf[offset] != STX)
	{
		printf("Invalid message header!\n");
		return ERREUR_STX;
	}
	++offset;
	iKeyIndex = psBuf[offset];
	key=GetKey(iKeyIndex);
	++offset;
	memset(sHeader,0,sizeof(sHeader));
	memcpy(sHeader,psBuf+offset,12);//copy 12 bytes to sHeader
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
		// Modified by Kim_LinHB 2014-5-8
		int nErrMsgLen;
		int nError = 151;
		unsigned char sBuff[6];
		strncpy(sBuff, psBuf + 1, 5);
		sBuff[5] = 0;
		nErrMsgLen = atoi(sBuff);
		strncpy(sBuff, psBuf + 1 + 7, 2);
		sBuff[2] = 0;
		nError = atoi(sBuff);
		strncpy(pstRespH->szResponseCode, psBuf + 1 + 7, 2);
		pstRespH->szResponseCode[2] = 0;
		strncpy(pstRespH->szMessage, psBuf + 1 + 9, nErrMsgLen);
		pstRespH->szMessage[nErrMsgLen] = 0;

		return nError;
	}

	psBuf+=offset;//buf现在指向content部分
	j=MessageLength/8;
	for(i = 0; i < j; ++i)
	{
		//memcpy(sTempFor3Des,content+i*8,8);
		//TDesDecrypt(sTempFor3Des,key,buf+i*8);
		PubDes(TRI_DECRYPT, psBuf+i*8, key, psBuf+i*8);
	}

	//解压存放 到sXmlData
	iRet = ZipUnzip(psBuf, MessageLength, psXmlData, puiXmlDataSize, 1);

	if(iRet!=0)
	{
		return iRet;
	}

#if defined(LHB_DEBUG) && defined(LHB_TEST)
	{
		unsigned int block = *iXmlDataSize / LHB_BLOCK, block_Cnt = 1;
#ifdef _WIN32
		if(block > 0)
		{
			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,LHB_BLOCK,DEVICE_COM2,HEX_MODE);
			while(1)
			{
				if(block_Cnt < block)
					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM2,HEX_MODE);
				else
				{
					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, *iXmlDataSize - block * LHB_BLOCK,DEVICE_COM2,HEX_MODE);
					break;
				}
				++block_Cnt;
			}
		}
		else
			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData, *iXmlDataSize,DEVICE_COM2,HEX_MODE);
		PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,*iXmlDataSize,DEVICE_COM2,ASC_MODE);
#else
		if(block > 0)
		{
			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,LHB_BLOCK,DEVICE_COM1,HEX_MODE);
			DelayMs(200);
			while(1)
			{
				if(block_Cnt < block)
					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, LHB_BLOCK,DEVICE_COM1,HEX_MODE);
				else
				{
					PubDebugOutput("",sXmlData + block_Cnt * LHB_BLOCK, *iXmlDataSize - block * LHB_BLOCK,DEVICE_COM1,HEX_MODE);
					break;
				}
				DelayMs(200);
				++block_Cnt;
			}
		}
		else
			PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData, *iXmlDataSize,DEVICE_COM1,HEX_MODE);
		DelayMs(200);
		PubDebugOutput("\x0d\x0a" "After Uncompress",sXmlData,*iXmlDataSize,DEVICE_COM1,ASC_MODE);
#endif
	}
#endif
#undef LHB_BLOCK
	return iRet;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static unsigned char *GetRandomKey(int *piIndex)
{
	int iRand;
	unsigned char timeACSII[SIZE_DATE_TIME + 1 + 2];
	unsigned long ltime;

	// Modified by Kim_LinHB 2013-3-20 v4.56
	strcpy(timeACSII, PRE_YEAR);
	GetCurTime(timeACSII + 2);
	// Modify End

	ltime = PubTime2Long(timeACSII); //since 1980.1.1.0.0.0

	//srand( time(NULL) ); ////time (null)  can not be used 
	srand(ltime);
	iRand=rand();

	*piIndex=iRand & 0xFF;
	return embedded_keys[*piIndex];
}

static unsigned char *GetKey(int iIndex)
{
	return embedded_keys[iIndex];
}


static unsigned char GetHeader(RESPONSE_HEADER_TAG *pstHeader, \
							   xml_t stHeaderXML)
{
	unsigned char ucRet = 1;
	if(pstHeader)
	{
		unsigned char timeBCD[6];
		strcpy(pstHeader->szResponseCode, XmlGetValue(&stHeaderXML,"ResponseCode"));
		strcpy(pstHeader->szStatus, XmlGetValue(&stHeaderXML,"Status"));
		strcpy(pstHeader->szMessage, XmlGetValue(&stHeaderXML,"Message"));
		strcpy(pstHeader->szHoseDateTime, XmlGetValue(&stHeaderXML, "HostDateTime"));
		PubAsc2Bcd(pstHeader->szHoseDateTime, SIZE_DATE_TIME, timeBCD);
		SetTime(timeBCD);
		if(strlen(pstHeader->szStatus) > 0) // Added by Kim_LinHB 2013-7-4 v4.61
		{	ucRet = atoi(pstHeader->szStatus);}
	}

	return ucRet;
}


static void Set_Header_XML(const REQUEST_HEADER_TAG *stHeader, \
						   unsigned char *pszBuffer, \
						   unsigned int *puiLenInOut)
{
	xml_t reqXML, headerXML;

	xml_t root = {"", pszBuffer, pszBuffer + *puiLenInOut, NULL};

	XmlAddChildNULL("Request", &root, &reqXML);
	XmlAddChildNULL("Header", &reqXML, &headerXML);

	XmlAddAddr(&headerXML, "MessageType", stHeader->szMsgType);
	XmlAddAddr(&headerXML, "SerialNumber", stHeader->szSerialNumber);
	XmlAddAddr(&headerXML, "Username", stHeader->szUserID);
	XmlAddAddr(&headerXML, "Password", stHeader->szPWD);
	XmlAddAddr(&headerXML, "ParameterVersion", stHeader->szParamVer);
	{
		sprintf(stHeader->szBatchNo, "%d", atoi(stHeader->szBatchNo));
		XmlAddAddr(&headerXML, "BatchNo", stHeader->szBatchNo);
	}
	XmlAddAddr(&headerXML, "TraceNo", stHeader->szTraceNo);
	XmlAddAddr(&headerXML, "TerminalDateTime", stHeader->szTermDateTime);
	XmlAddAddr(&headerXML, "TerminalId", stHeader->szTermId);
	XmlAddAddr(&headerXML, "LanguageCode", stHeader->szLangCode);

	*puiLenInOut = root.end - root.start;
}

static char base64_alphabet[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="};
static unsigned int Base64Decode(unsigned char *pszInput, \
								 unsigned char *pszOutput, \
								 unsigned int uiInputLen)
{
	unsigned char szCode[5];
	unsigned int i,j,k,m;
	unsigned char szASC[4];
	for(i = 0,j=0; i<uiInputLen; i+=4, j+=3)
	{
		memset(szCode, 0, 5);
		szCode[0] = pszInput[i];
		szCode[1] = pszInput[i+1];
		szCode[2] = pszInput[i+2];
		szCode[3] = pszInput[i+3];
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
			pszOutput[j] = (szASC[0]<<2) + ((szASC[1]<<2)>>6);
		}
		else if(szCode[3] == '=')
		{
			pszOutput[j] = (szASC[0]<<2) + ((szASC[1]<<2)>>6);
			pszOutput[j+1] = (szASC[1]<<4) + ((szASC[2]<<2)>>4);
		}
		else
		{
			pszOutput[j] = (szASC[0]<<2) + ((szASC[1]<<2)>>6);
			pszOutput[j+1] = (szASC[1]<<4) + ((szASC[2]<<2)>>4);
			pszOutput[j+2] = (szASC[2]<<6) + szASC[3];
		}
	}
	return (j+3);
}
static unsigned int Base64Encode(unsigned char *pszInput, \
								 unsigned char *pszOutput, \
								 unsigned int uiInputLen)
{
	unsigned char szCode[4];
	unsigned int i,j;
	unsigned int uiCodeLen=0;
	for(i = 0,j=0; i<uiInputLen; i+=3, j+=4)
	{
		memset(szCode, 0, 4);
		uiCodeLen = 0;
		szCode[0] = pszInput[i];
		uiCodeLen += 1;
		if(i < (uiInputLen - 1))
		{
			szCode[1] = pszInput[i+1];
			uiCodeLen += 1;
		}
		if(i < (uiInputLen - 2))
		{
			szCode[2] = pszInput[i+2];
			uiCodeLen += 1;
		}
		if(uiCodeLen == 3)
		{
			pszOutput[j] = base64_alphabet[(szCode[0]&0xfc)>>2];
			pszOutput[j+1] = base64_alphabet[((szCode[0]&0x03)<<4) + ((szCode[1]&0xf0)>>4)];
			pszOutput[j+2] = base64_alphabet[((szCode[1]&0x0f)<<2) + ((szCode[2]&0xc0)>>6)];
			pszOutput[j+3] = base64_alphabet[szCode[2]&0x3f];
		}
		else if(uiCodeLen == 2)
		{
			pszOutput[j] = base64_alphabet[(szCode[0]&0xfc)>>2];
			pszOutput[j+1] = base64_alphabet[((szCode[0]&0x03)<<4) + ((szCode[1]&0xf0)>>4)];
			pszOutput[j+2] = base64_alphabet[(szCode[1]&0x0f)<<2];
			pszOutput[j+3] = base64_alphabet[64];
		}
		else
		{
			pszOutput[j] = base64_alphabet[(szCode[0]&0xfc)>>2];
			pszOutput[j+1] = base64_alphabet[(szCode[0]&0x03)<<4];
			pszOutput[j+2] = base64_alphabet[64];
			pszOutput[j+3] = base64_alphabet[64];
		}
	}
	return (j+4);
}


static int My_atoi(void *s)
{
	if(NULL == s)
		return 0;
	else
		return atoi(s);
}

