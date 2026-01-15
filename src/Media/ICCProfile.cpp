#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "Media/ICCProfile.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

const UInt8 Media::ICCProfile::srgbICC[] = {
	0x00, 0x00, 0x0B, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x6D, 0x6E, 0x74, 0x72, 
	0x52, 0x47, 0x42, 0x20, 0x58, 0x59, 0x5A, 0x20, 0x07, 0xDF, 0x00, 0x02, 0x00, 0x0F, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x61, 0x63, 0x73, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF6, 0xD6, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD3, 0x2D, 
	0x00, 0x00, 0x00, 0x00, 0x3D, 0x0E, 0xB2, 0xDE, 0xAE, 0x93, 0x97, 0xBE, 0x9B, 0x67, 0x26, 0xCE, 
	0x8C, 0x0A, 0x43, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x10, 0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x01, 0x44, 0x00, 0x00, 0x00, 0x63, 
	0x62, 0x58, 0x59, 0x5A, 0x00, 0x00, 0x01, 0xA8, 0x00, 0x00, 0x00, 0x14, 0x62, 0x54, 0x52, 0x43, 
	0x00, 0x00, 0x01, 0xBC, 0x00, 0x00, 0x08, 0x0C, 0x67, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0xBC, 
	0x00, 0x00, 0x08, 0x0C, 0x72, 0x54, 0x52, 0x43, 0x00, 0x00, 0x01, 0xBC, 0x00, 0x00, 0x08, 0x0C, 
	0x64, 0x6D, 0x64, 0x64, 0x00, 0x00, 0x09, 0xC8, 0x00, 0x00, 0x00, 0x88, 0x67, 0x58, 0x59, 0x5A, 
	0x00, 0x00, 0x0A, 0x50, 0x00, 0x00, 0x00, 0x14, 0x6C, 0x75, 0x6D, 0x69, 0x00, 0x00, 0x0A, 0x64, 
	0x00, 0x00, 0x00, 0x14, 0x6D, 0x65, 0x61, 0x73, 0x00, 0x00, 0x0A, 0x78, 0x00, 0x00, 0x00, 0x24, 
	0x62, 0x6B, 0x70, 0x74, 0x00, 0x00, 0x0A, 0x9C, 0x00, 0x00, 0x00, 0x14, 0x72, 0x58, 0x59, 0x5A, 
	0x00, 0x00, 0x0A, 0xB0, 0x00, 0x00, 0x00, 0x14, 0x74, 0x65, 0x63, 0x68, 0x00, 0x00, 0x0A, 0xC4, 
	0x00, 0x00, 0x00, 0x0C, 0x76, 0x75, 0x65, 0x64, 0x00, 0x00, 0x0A, 0xD0, 0x00, 0x00, 0x00, 0x87, 
	0x77, 0x74, 0x70, 0x74, 0x00, 0x00, 0x0B, 0x58, 0x00, 0x00, 0x00, 0x14, 0x63, 0x70, 0x72, 0x74, 
	0x00, 0x00, 0x0B, 0x6C, 0x00, 0x00, 0x00, 0x37, 0x63, 0x68, 0x61, 0x64, 0x00, 0x00, 0x0B, 0xA4, 
	0x00, 0x00, 0x00, 0x2C, 0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 
	0x73, 0x52, 0x47, 0x42, 0x32, 0x30, 0x31, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x59, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x24, 0xA0, 0x00, 0x00, 0x0F, 0x84, 0x00, 0x00, 0xB6, 0xCF, 0x63, 0x75, 0x72, 0x76, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x0A, 0x00, 0x0F, 
	0x00, 0x14, 0x00, 0x19, 0x00, 0x1E, 0x00, 0x23, 0x00, 0x28, 0x00, 0x2D, 0x00, 0x32, 0x00, 0x37, 
	0x00, 0x3B, 0x00, 0x40, 0x00, 0x45, 0x00, 0x4A, 0x00, 0x4F, 0x00, 0x54, 0x00, 0x59, 0x00, 0x5E, 
	0x00, 0x63, 0x00, 0x68, 0x00, 0x6D, 0x00, 0x72, 0x00, 0x77, 0x00, 0x7C, 0x00, 0x81, 0x00, 0x86, 
	0x00, 0x8B, 0x00, 0x90, 0x00, 0x95, 0x00, 0x9A, 0x00, 0x9F, 0x00, 0xA4, 0x00, 0xA9, 0x00, 0xAE, 
	0x00, 0xB2, 0x00, 0xB7, 0x00, 0xBC, 0x00, 0xC1, 0x00, 0xC6, 0x00, 0xCB, 0x00, 0xD0, 0x00, 0xD5, 
	0x00, 0xDB, 0x00, 0xE0, 0x00, 0xE5, 0x00, 0xEB, 0x00, 0xF0, 0x00, 0xF6, 0x00, 0xFB, 0x01, 0x01, 
	0x01, 0x07, 0x01, 0x0D, 0x01, 0x13, 0x01, 0x19, 0x01, 0x1F, 0x01, 0x25, 0x01, 0x2B, 0x01, 0x32, 
	0x01, 0x38, 0x01, 0x3E, 0x01, 0x45, 0x01, 0x4C, 0x01, 0x52, 0x01, 0x59, 0x01, 0x60, 0x01, 0x67, 
	0x01, 0x6E, 0x01, 0x75, 0x01, 0x7C, 0x01, 0x83, 0x01, 0x8B, 0x01, 0x92, 0x01, 0x9A, 0x01, 0xA1, 
	0x01, 0xA9, 0x01, 0xB1, 0x01, 0xB9, 0x01, 0xC1, 0x01, 0xC9, 0x01, 0xD1, 0x01, 0xD9, 0x01, 0xE1, 
	0x01, 0xE9, 0x01, 0xF2, 0x01, 0xFA, 0x02, 0x03, 0x02, 0x0C, 0x02, 0x14, 0x02, 0x1D, 0x02, 0x26, 
	0x02, 0x2F, 0x02, 0x38, 0x02, 0x41, 0x02, 0x4B, 0x02, 0x54, 0x02, 0x5D, 0x02, 0x67, 0x02, 0x71, 
	0x02, 0x7A, 0x02, 0x84, 0x02, 0x8E, 0x02, 0x98, 0x02, 0xA2, 0x02, 0xAC, 0x02, 0xB6, 0x02, 0xC1, 
	0x02, 0xCB, 0x02, 0xD5, 0x02, 0xE0, 0x02, 0xEB, 0x02, 0xF5, 0x03, 0x00, 0x03, 0x0B, 0x03, 0x16, 
	0x03, 0x21, 0x03, 0x2D, 0x03, 0x38, 0x03, 0x43, 0x03, 0x4F, 0x03, 0x5A, 0x03, 0x66, 0x03, 0x72, 
	0x03, 0x7E, 0x03, 0x8A, 0x03, 0x96, 0x03, 0xA2, 0x03, 0xAE, 0x03, 0xBA, 0x03, 0xC7, 0x03, 0xD3, 
	0x03, 0xE0, 0x03, 0xEC, 0x03, 0xF9, 0x04, 0x06, 0x04, 0x13, 0x04, 0x20, 0x04, 0x2D, 0x04, 0x3B, 
	0x04, 0x48, 0x04, 0x55, 0x04, 0x63, 0x04, 0x71, 0x04, 0x7E, 0x04, 0x8C, 0x04, 0x9A, 0x04, 0xA8, 
	0x04, 0xB6, 0x04, 0xC4, 0x04, 0xD3, 0x04, 0xE1, 0x04, 0xF0, 0x04, 0xFE, 0x05, 0x0D, 0x05, 0x1C, 
	0x05, 0x2B, 0x05, 0x3A, 0x05, 0x49, 0x05, 0x58, 0x05, 0x67, 0x05, 0x77, 0x05, 0x86, 0x05, 0x96, 
	0x05, 0xA6, 0x05, 0xB5, 0x05, 0xC5, 0x05, 0xD5, 0x05, 0xE5, 0x05, 0xF6, 0x06, 0x06, 0x06, 0x16, 
	0x06, 0x27, 0x06, 0x37, 0x06, 0x48, 0x06, 0x59, 0x06, 0x6A, 0x06, 0x7B, 0x06, 0x8C, 0x06, 0x9D, 
	0x06, 0xAF, 0x06, 0xC0, 0x06, 0xD1, 0x06, 0xE3, 0x06, 0xF5, 0x07, 0x07, 0x07, 0x19, 0x07, 0x2B, 
	0x07, 0x3D, 0x07, 0x4F, 0x07, 0x61, 0x07, 0x74, 0x07, 0x86, 0x07, 0x99, 0x07, 0xAC, 0x07, 0xBF, 
	0x07, 0xD2, 0x07, 0xE5, 0x07, 0xF8, 0x08, 0x0B, 0x08, 0x1F, 0x08, 0x32, 0x08, 0x46, 0x08, 0x5A, 
	0x08, 0x6E, 0x08, 0x82, 0x08, 0x96, 0x08, 0xAA, 0x08, 0xBE, 0x08, 0xD2, 0x08, 0xE7, 0x08, 0xFB, 
	0x09, 0x10, 0x09, 0x25, 0x09, 0x3A, 0x09, 0x4F, 0x09, 0x64, 0x09, 0x79, 0x09, 0x8F, 0x09, 0xA4, 
	0x09, 0xBA, 0x09, 0xCF, 0x09, 0xE5, 0x09, 0xFB, 0x0A, 0x11, 0x0A, 0x27, 0x0A, 0x3D, 0x0A, 0x54, 
	0x0A, 0x6A, 0x0A, 0x81, 0x0A, 0x98, 0x0A, 0xAE, 0x0A, 0xC5, 0x0A, 0xDC, 0x0A, 0xF3, 0x0B, 0x0B, 
	0x0B, 0x22, 0x0B, 0x39, 0x0B, 0x51, 0x0B, 0x69, 0x0B, 0x80, 0x0B, 0x98, 0x0B, 0xB0, 0x0B, 0xC8, 
	0x0B, 0xE1, 0x0B, 0xF9, 0x0C, 0x12, 0x0C, 0x2A, 0x0C, 0x43, 0x0C, 0x5C, 0x0C, 0x75, 0x0C, 0x8E, 
	0x0C, 0xA7, 0x0C, 0xC0, 0x0C, 0xD9, 0x0C, 0xF3, 0x0D, 0x0D, 0x0D, 0x26, 0x0D, 0x40, 0x0D, 0x5A, 
	0x0D, 0x74, 0x0D, 0x8E, 0x0D, 0xA9, 0x0D, 0xC3, 0x0D, 0xDE, 0x0D, 0xF8, 0x0E, 0x13, 0x0E, 0x2E, 
	0x0E, 0x49, 0x0E, 0x64, 0x0E, 0x7F, 0x0E, 0x9B, 0x0E, 0xB6, 0x0E, 0xD2, 0x0E, 0xEE, 0x0F, 0x09, 
	0x0F, 0x25, 0x0F, 0x41, 0x0F, 0x5E, 0x0F, 0x7A, 0x0F, 0x96, 0x0F, 0xB3, 0x0F, 0xCF, 0x0F, 0xEC, 
	0x10, 0x09, 0x10, 0x26, 0x10, 0x43, 0x10, 0x61, 0x10, 0x7E, 0x10, 0x9B, 0x10, 0xB9, 0x10, 0xD7, 
	0x10, 0xF5, 0x11, 0x13, 0x11, 0x31, 0x11, 0x4F, 0x11, 0x6D, 0x11, 0x8C, 0x11, 0xAA, 0x11, 0xC9, 
	0x11, 0xE8, 0x12, 0x07, 0x12, 0x26, 0x12, 0x45, 0x12, 0x64, 0x12, 0x84, 0x12, 0xA3, 0x12, 0xC3, 
	0x12, 0xE3, 0x13, 0x03, 0x13, 0x23, 0x13, 0x43, 0x13, 0x63, 0x13, 0x83, 0x13, 0xA4, 0x13, 0xC5, 
	0x13, 0xE5, 0x14, 0x06, 0x14, 0x27, 0x14, 0x49, 0x14, 0x6A, 0x14, 0x8B, 0x14, 0xAD, 0x14, 0xCE, 
	0x14, 0xF0, 0x15, 0x12, 0x15, 0x34, 0x15, 0x56, 0x15, 0x78, 0x15, 0x9B, 0x15, 0xBD, 0x15, 0xE0, 
	0x16, 0x03, 0x16, 0x26, 0x16, 0x49, 0x16, 0x6C, 0x16, 0x8F, 0x16, 0xB2, 0x16, 0xD6, 0x16, 0xFA, 
	0x17, 0x1D, 0x17, 0x41, 0x17, 0x65, 0x17, 0x89, 0x17, 0xAE, 0x17, 0xD2, 0x17, 0xF7, 0x18, 0x1B, 
	0x18, 0x40, 0x18, 0x65, 0x18, 0x8A, 0x18, 0xAF, 0x18, 0xD5, 0x18, 0xFA, 0x19, 0x20, 0x19, 0x45, 
	0x19, 0x6B, 0x19, 0x91, 0x19, 0xB7, 0x19, 0xDD, 0x1A, 0x04, 0x1A, 0x2A, 0x1A, 0x51, 0x1A, 0x77, 
	0x1A, 0x9E, 0x1A, 0xC5, 0x1A, 0xEC, 0x1B, 0x14, 0x1B, 0x3B, 0x1B, 0x63, 0x1B, 0x8A, 0x1B, 0xB2, 
	0x1B, 0xDA, 0x1C, 0x02, 0x1C, 0x2A, 0x1C, 0x52, 0x1C, 0x7B, 0x1C, 0xA3, 0x1C, 0xCC, 0x1C, 0xF5, 
	0x1D, 0x1E, 0x1D, 0x47, 0x1D, 0x70, 0x1D, 0x99, 0x1D, 0xC3, 0x1D, 0xEC, 0x1E, 0x16, 0x1E, 0x40, 
	0x1E, 0x6A, 0x1E, 0x94, 0x1E, 0xBE, 0x1E, 0xE9, 0x1F, 0x13, 0x1F, 0x3E, 0x1F, 0x69, 0x1F, 0x94, 
	0x1F, 0xBF, 0x1F, 0xEA, 0x20, 0x15, 0x20, 0x41, 0x20, 0x6C, 0x20, 0x98, 0x20, 0xC4, 0x20, 0xF0, 
	0x21, 0x1C, 0x21, 0x48, 0x21, 0x75, 0x21, 0xA1, 0x21, 0xCE, 0x21, 0xFB, 0x22, 0x27, 0x22, 0x55, 
	0x22, 0x82, 0x22, 0xAF, 0x22, 0xDD, 0x23, 0x0A, 0x23, 0x38, 0x23, 0x66, 0x23, 0x94, 0x23, 0xC2, 
	0x23, 0xF0, 0x24, 0x1F, 0x24, 0x4D, 0x24, 0x7C, 0x24, 0xAB, 0x24, 0xDA, 0x25, 0x09, 0x25, 0x38, 
	0x25, 0x68, 0x25, 0x97, 0x25, 0xC7, 0x25, 0xF7, 0x26, 0x27, 0x26, 0x57, 0x26, 0x87, 0x26, 0xB7, 
	0x26, 0xE8, 0x27, 0x18, 0x27, 0x49, 0x27, 0x7A, 0x27, 0xAB, 0x27, 0xDC, 0x28, 0x0D, 0x28, 0x3F, 
	0x28, 0x71, 0x28, 0xA2, 0x28, 0xD4, 0x29, 0x06, 0x29, 0x38, 0x29, 0x6B, 0x29, 0x9D, 0x29, 0xD0, 
	0x2A, 0x02, 0x2A, 0x35, 0x2A, 0x68, 0x2A, 0x9B, 0x2A, 0xCF, 0x2B, 0x02, 0x2B, 0x36, 0x2B, 0x69, 
	0x2B, 0x9D, 0x2B, 0xD1, 0x2C, 0x05, 0x2C, 0x39, 0x2C, 0x6E, 0x2C, 0xA2, 0x2C, 0xD7, 0x2D, 0x0C, 
	0x2D, 0x41, 0x2D, 0x76, 0x2D, 0xAB, 0x2D, 0xE1, 0x2E, 0x16, 0x2E, 0x4C, 0x2E, 0x82, 0x2E, 0xB7, 
	0x2E, 0xEE, 0x2F, 0x24, 0x2F, 0x5A, 0x2F, 0x91, 0x2F, 0xC7, 0x2F, 0xFE, 0x30, 0x35, 0x30, 0x6C, 
	0x30, 0xA4, 0x30, 0xDB, 0x31, 0x12, 0x31, 0x4A, 0x31, 0x82, 0x31, 0xBA, 0x31, 0xF2, 0x32, 0x2A, 
	0x32, 0x63, 0x32, 0x9B, 0x32, 0xD4, 0x33, 0x0D, 0x33, 0x46, 0x33, 0x7F, 0x33, 0xB8, 0x33, 0xF1, 
	0x34, 0x2B, 0x34, 0x65, 0x34, 0x9E, 0x34, 0xD8, 0x35, 0x13, 0x35, 0x4D, 0x35, 0x87, 0x35, 0xC2, 
	0x35, 0xFD, 0x36, 0x37, 0x36, 0x72, 0x36, 0xAE, 0x36, 0xE9, 0x37, 0x24, 0x37, 0x60, 0x37, 0x9C, 
	0x37, 0xD7, 0x38, 0x14, 0x38, 0x50, 0x38, 0x8C, 0x38, 0xC8, 0x39, 0x05, 0x39, 0x42, 0x39, 0x7F, 
	0x39, 0xBC, 0x39, 0xF9, 0x3A, 0x36, 0x3A, 0x74, 0x3A, 0xB2, 0x3A, 0xEF, 0x3B, 0x2D, 0x3B, 0x6B, 
	0x3B, 0xAA, 0x3B, 0xE8, 0x3C, 0x27, 0x3C, 0x65, 0x3C, 0xA4, 0x3C, 0xE3, 0x3D, 0x22, 0x3D, 0x61, 
	0x3D, 0xA1, 0x3D, 0xE0, 0x3E, 0x20, 0x3E, 0x60, 0x3E, 0xA0, 0x3E, 0xE0, 0x3F, 0x21, 0x3F, 0x61, 
	0x3F, 0xA2, 0x3F, 0xE2, 0x40, 0x23, 0x40, 0x64, 0x40, 0xA6, 0x40, 0xE7, 0x41, 0x29, 0x41, 0x6A, 
	0x41, 0xAC, 0x41, 0xEE, 0x42, 0x30, 0x42, 0x72, 0x42, 0xB5, 0x42, 0xF7, 0x43, 0x3A, 0x43, 0x7D, 
	0x43, 0xC0, 0x44, 0x03, 0x44, 0x47, 0x44, 0x8A, 0x44, 0xCE, 0x45, 0x12, 0x45, 0x55, 0x45, 0x9A, 
	0x45, 0xDE, 0x46, 0x22, 0x46, 0x67, 0x46, 0xAB, 0x46, 0xF0, 0x47, 0x35, 0x47, 0x7B, 0x47, 0xC0, 
	0x48, 0x05, 0x48, 0x4B, 0x48, 0x91, 0x48, 0xD7, 0x49, 0x1D, 0x49, 0x63, 0x49, 0xA9, 0x49, 0xF0, 
	0x4A, 0x37, 0x4A, 0x7D, 0x4A, 0xC4, 0x4B, 0x0C, 0x4B, 0x53, 0x4B, 0x9A, 0x4B, 0xE2, 0x4C, 0x2A, 
	0x4C, 0x72, 0x4C, 0xBA, 0x4D, 0x02, 0x4D, 0x4A, 0x4D, 0x93, 0x4D, 0xDC, 0x4E, 0x25, 0x4E, 0x6E, 
	0x4E, 0xB7, 0x4F, 0x00, 0x4F, 0x49, 0x4F, 0x93, 0x4F, 0xDD, 0x50, 0x27, 0x50, 0x71, 0x50, 0xBB, 
	0x51, 0x06, 0x51, 0x50, 0x51, 0x9B, 0x51, 0xE6, 0x52, 0x31, 0x52, 0x7C, 0x52, 0xC7, 0x53, 0x13, 
	0x53, 0x5F, 0x53, 0xAA, 0x53, 0xF6, 0x54, 0x42, 0x54, 0x8F, 0x54, 0xDB, 0x55, 0x28, 0x55, 0x75, 
	0x55, 0xC2, 0x56, 0x0F, 0x56, 0x5C, 0x56, 0xA9, 0x56, 0xF7, 0x57, 0x44, 0x57, 0x92, 0x57, 0xE0, 
	0x58, 0x2F, 0x58, 0x7D, 0x58, 0xCB, 0x59, 0x1A, 0x59, 0x69, 0x59, 0xB8, 0x5A, 0x07, 0x5A, 0x56, 
	0x5A, 0xA6, 0x5A, 0xF5, 0x5B, 0x45, 0x5B, 0x95, 0x5B, 0xE5, 0x5C, 0x35, 0x5C, 0x86, 0x5C, 0xD6, 
	0x5D, 0x27, 0x5D, 0x78, 0x5D, 0xC9, 0x5E, 0x1A, 0x5E, 0x6C, 0x5E, 0xBD, 0x5F, 0x0F, 0x5F, 0x61, 
	0x5F, 0xB3, 0x60, 0x05, 0x60, 0x57, 0x60, 0xAA, 0x60, 0xFC, 0x61, 0x4F, 0x61, 0xA2, 0x61, 0xF5, 
	0x62, 0x49, 0x62, 0x9C, 0x62, 0xF0, 0x63, 0x43, 0x63, 0x97, 0x63, 0xEB, 0x64, 0x40, 0x64, 0x94, 
	0x64, 0xE9, 0x65, 0x3D, 0x65, 0x92, 0x65, 0xE7, 0x66, 0x3D, 0x66, 0x92, 0x66, 0xE8, 0x67, 0x3D, 
	0x67, 0x93, 0x67, 0xE9, 0x68, 0x3F, 0x68, 0x96, 0x68, 0xEC, 0x69, 0x43, 0x69, 0x9A, 0x69, 0xF1, 
	0x6A, 0x48, 0x6A, 0x9F, 0x6A, 0xF7, 0x6B, 0x4F, 0x6B, 0xA7, 0x6B, 0xFF, 0x6C, 0x57, 0x6C, 0xAF, 
	0x6D, 0x08, 0x6D, 0x60, 0x6D, 0xB9, 0x6E, 0x12, 0x6E, 0x6B, 0x6E, 0xC4, 0x6F, 0x1E, 0x6F, 0x78, 
	0x6F, 0xD1, 0x70, 0x2B, 0x70, 0x86, 0x70, 0xE0, 0x71, 0x3A, 0x71, 0x95, 0x71, 0xF0, 0x72, 0x4B, 
	0x72, 0xA6, 0x73, 0x01, 0x73, 0x5D, 0x73, 0xB8, 0x74, 0x14, 0x74, 0x70, 0x74, 0xCC, 0x75, 0x28, 
	0x75, 0x85, 0x75, 0xE1, 0x76, 0x3E, 0x76, 0x9B, 0x76, 0xF8, 0x77, 0x56, 0x77, 0xB3, 0x78, 0x11, 
	0x78, 0x6E, 0x78, 0xCC, 0x79, 0x2A, 0x79, 0x89, 0x79, 0xE7, 0x7A, 0x46, 0x7A, 0xA5, 0x7B, 0x04, 
	0x7B, 0x63, 0x7B, 0xC2, 0x7C, 0x21, 0x7C, 0x81, 0x7C, 0xE1, 0x7D, 0x41, 0x7D, 0xA1, 0x7E, 0x01, 
	0x7E, 0x62, 0x7E, 0xC2, 0x7F, 0x23, 0x7F, 0x84, 0x7F, 0xE5, 0x80, 0x47, 0x80, 0xA8, 0x81, 0x0A, 
	0x81, 0x6B, 0x81, 0xCD, 0x82, 0x30, 0x82, 0x92, 0x82, 0xF4, 0x83, 0x57, 0x83, 0xBA, 0x84, 0x1D, 
	0x84, 0x80, 0x84, 0xE3, 0x85, 0x47, 0x85, 0xAB, 0x86, 0x0E, 0x86, 0x72, 0x86, 0xD7, 0x87, 0x3B, 
	0x87, 0x9F, 0x88, 0x04, 0x88, 0x69, 0x88, 0xCE, 0x89, 0x33, 0x89, 0x99, 0x89, 0xFE, 0x8A, 0x64, 
	0x8A, 0xCA, 0x8B, 0x30, 0x8B, 0x96, 0x8B, 0xFC, 0x8C, 0x63, 0x8C, 0xCA, 0x8D, 0x31, 0x8D, 0x98, 
	0x8D, 0xFF, 0x8E, 0x66, 0x8E, 0xCE, 0x8F, 0x36, 0x8F, 0x9E, 0x90, 0x06, 0x90, 0x6E, 0x90, 0xD6, 
	0x91, 0x3F, 0x91, 0xA8, 0x92, 0x11, 0x92, 0x7A, 0x92, 0xE3, 0x93, 0x4D, 0x93, 0xB6, 0x94, 0x20, 
	0x94, 0x8A, 0x94, 0xF4, 0x95, 0x5F, 0x95, 0xC9, 0x96, 0x34, 0x96, 0x9F, 0x97, 0x0A, 0x97, 0x75, 
	0x97, 0xE0, 0x98, 0x4C, 0x98, 0xB8, 0x99, 0x24, 0x99, 0x90, 0x99, 0xFC, 0x9A, 0x68, 0x9A, 0xD5, 
	0x9B, 0x42, 0x9B, 0xAF, 0x9C, 0x1C, 0x9C, 0x89, 0x9C, 0xF7, 0x9D, 0x64, 0x9D, 0xD2, 0x9E, 0x40, 
	0x9E, 0xAE, 0x9F, 0x1D, 0x9F, 0x8B, 0x9F, 0xFA, 0xA0, 0x69, 0xA0, 0xD8, 0xA1, 0x47, 0xA1, 0xB6, 
	0xA2, 0x26, 0xA2, 0x96, 0xA3, 0x06, 0xA3, 0x76, 0xA3, 0xE6, 0xA4, 0x56, 0xA4, 0xC7, 0xA5, 0x38, 
	0xA5, 0xA9, 0xA6, 0x1A, 0xA6, 0x8B, 0xA6, 0xFD, 0xA7, 0x6E, 0xA7, 0xE0, 0xA8, 0x52, 0xA8, 0xC4, 
	0xA9, 0x37, 0xA9, 0xA9, 0xAA, 0x1C, 0xAA, 0x8F, 0xAB, 0x02, 0xAB, 0x75, 0xAB, 0xE9, 0xAC, 0x5C, 
	0xAC, 0xD0, 0xAD, 0x44, 0xAD, 0xB8, 0xAE, 0x2D, 0xAE, 0xA1, 0xAF, 0x16, 0xAF, 0x8B, 0xB0, 0x00, 
	0xB0, 0x75, 0xB0, 0xEA, 0xB1, 0x60, 0xB1, 0xD6, 0xB2, 0x4B, 0xB2, 0xC2, 0xB3, 0x38, 0xB3, 0xAE, 
	0xB4, 0x25, 0xB4, 0x9C, 0xB5, 0x13, 0xB5, 0x8A, 0xB6, 0x01, 0xB6, 0x79, 0xB6, 0xF0, 0xB7, 0x68, 
	0xB7, 0xE0, 0xB8, 0x59, 0xB8, 0xD1, 0xB9, 0x4A, 0xB9, 0xC2, 0xBA, 0x3B, 0xBA, 0xB5, 0xBB, 0x2E, 
	0xBB, 0xA7, 0xBC, 0x21, 0xBC, 0x9B, 0xBD, 0x15, 0xBD, 0x8F, 0xBE, 0x0A, 0xBE, 0x84, 0xBE, 0xFF, 
	0xBF, 0x7A, 0xBF, 0xF5, 0xC0, 0x70, 0xC0, 0xEC, 0xC1, 0x67, 0xC1, 0xE3, 0xC2, 0x5F, 0xC2, 0xDB, 
	0xC3, 0x58, 0xC3, 0xD4, 0xC4, 0x51, 0xC4, 0xCE, 0xC5, 0x4B, 0xC5, 0xC8, 0xC6, 0x46, 0xC6, 0xC3, 
	0xC7, 0x41, 0xC7, 0xBF, 0xC8, 0x3D, 0xC8, 0xBC, 0xC9, 0x3A, 0xC9, 0xB9, 0xCA, 0x38, 0xCA, 0xB7, 
	0xCB, 0x36, 0xCB, 0xB6, 0xCC, 0x35, 0xCC, 0xB5, 0xCD, 0x35, 0xCD, 0xB5, 0xCE, 0x36, 0xCE, 0xB6, 
	0xCF, 0x37, 0xCF, 0xB8, 0xD0, 0x39, 0xD0, 0xBA, 0xD1, 0x3C, 0xD1, 0xBE, 0xD2, 0x3F, 0xD2, 0xC1, 
	0xD3, 0x44, 0xD3, 0xC6, 0xD4, 0x49, 0xD4, 0xCB, 0xD5, 0x4E, 0xD5, 0xD1, 0xD6, 0x55, 0xD6, 0xD8, 
	0xD7, 0x5C, 0xD7, 0xE0, 0xD8, 0x64, 0xD8, 0xE8, 0xD9, 0x6C, 0xD9, 0xF1, 0xDA, 0x76, 0xDA, 0xFB, 
	0xDB, 0x80, 0xDC, 0x05, 0xDC, 0x8A, 0xDD, 0x10, 0xDD, 0x96, 0xDE, 0x1C, 0xDE, 0xA2, 0xDF, 0x29, 
	0xDF, 0xAF, 0xE0, 0x36, 0xE0, 0xBD, 0xE1, 0x44, 0xE1, 0xCC, 0xE2, 0x53, 0xE2, 0xDB, 0xE3, 0x63, 
	0xE3, 0xEB, 0xE4, 0x73, 0xE4, 0xFC, 0xE5, 0x84, 0xE6, 0x0D, 0xE6, 0x96, 0xE7, 0x1F, 0xE7, 0xA9, 
	0xE8, 0x32, 0xE8, 0xBC, 0xE9, 0x46, 0xE9, 0xD0, 0xEA, 0x5B, 0xEA, 0xE5, 0xEB, 0x70, 0xEB, 0xFB, 
	0xEC, 0x86, 0xED, 0x11, 0xED, 0x9C, 0xEE, 0x28, 0xEE, 0xB4, 0xEF, 0x40, 0xEF, 0xCC, 0xF0, 0x58, 
	0xF0, 0xE5, 0xF1, 0x72, 0xF1, 0xFF, 0xF2, 0x8C, 0xF3, 0x19, 0xF3, 0xA7, 0xF4, 0x34, 0xF4, 0xC2, 
	0xF5, 0x50, 0xF5, 0xDE, 0xF6, 0x6D, 0xF6, 0xFB, 0xF7, 0x8A, 0xF8, 0x19, 0xF8, 0xA8, 0xF9, 0x38, 
	0xF9, 0xC7, 0xFA, 0x57, 0xFA, 0xE7, 0xFB, 0x77, 0xFC, 0x07, 0xFC, 0x98, 0xFD, 0x29, 0xFD, 0xBA, 
	0xFE, 0x4B, 0xFE, 0xDC, 0xFF, 0x6D, 0xFF, 0xFF, 0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x2E, 0x49, 0x45, 0x43, 0x20, 0x36, 0x31, 0x39, 0x36, 0x36, 0x2D, 0x32, 0x2D, 
	0x31, 0x20, 0x44, 0x65, 0x66, 0x61, 0x75, 0x6C, 0x74, 0x20, 0x52, 0x47, 0x42, 0x20, 0x43, 0x6F, 
	0x6C, 0x6F, 0x75, 0x72, 0x20, 0x53, 0x70, 0x61, 0x63, 0x65, 0x20, 0x2D, 0x20, 0x73, 0x52, 0x47, 
	0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x58, 0x59, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x99, 0x00, 0x00, 0xB7, 0x85, 
	0x00, 0x00, 0x18, 0xDA, 0x58, 0x59, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6D, 0x65, 0x61, 0x73, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x58, 0x59, 0x5A, 0x20, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xA4, 0x00, 0x00, 0x00, 0x87, 
	0x58, 0x59, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6F, 0xA2, 0x00, 0x00, 0x38, 0xF5, 
	0x00, 0x00, 0x03, 0x90, 0x73, 0x69, 0x67, 0x20, 0x00, 0x00, 0x00, 0x00, 0x43, 0x52, 0x54, 0x20, 
	0x64, 0x65, 0x73, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2D, 0x52, 0x65, 0x66, 0x65, 
	0x72, 0x65, 0x6E, 0x63, 0x65, 0x20, 0x56, 0x69, 0x65, 0x77, 0x69, 0x6E, 0x67, 0x20, 0x43, 0x6F, 
	0x6E, 0x64, 0x69, 0x74, 0x69, 0x6F, 0x6E, 0x20, 0x69, 0x6E, 0x20, 0x49, 0x45, 0x43, 0x20, 0x36, 
	0x31, 0x39, 0x36, 0x36, 0x2D, 0x32, 0x2D, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x59, 0x5A, 0x20, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xF6, 0xD6, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0xD3, 0x2D, 0x74, 0x65, 0x78, 0x74, 
	0x00, 0x00, 0x00, 0x00, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x49, 0x6E, 
	0x74, 0x65, 0x72, 0x6E, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x61, 0x6C, 0x20, 0x43, 0x6F, 0x6C, 0x6F, 
	0x72, 0x20, 0x43, 0x6F, 0x6E, 0x73, 0x6F, 0x72, 0x74, 0x69, 0x75, 0x6D, 0x2C, 0x20, 0x32, 0x30, 
	0x31, 0x35, 0x00, 0x00, 0x73, 0x66, 0x33, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0C, 0x44, 
	0x00, 0x00, 0x05, 0xDF, 0xFF, 0xFF, 0xF3, 0x26, 0x00, 0x00, 0x07, 0x94, 0x00, 0x00, 0xFD, 0x8F, 
	0xFF, 0xFF, 0xFB, 0xA1, 0xFF, 0xFF, 0xFD, 0xA2, 0x00, 0x00, 0x03, 0xDB, 0x00, 0x00, 0xC0, 0x75
};

Media::ICCProfile::ICCProfile(UnsafeArray<const UInt8> iccBuff) : iccBuff(ReadMUInt32(iccBuff.Ptr()))
{
	UOSInt leng = ReadMUInt32(iccBuff.Ptr());
	this->iccBuff.CopyFrom(Data::ByteArrayR(iccBuff, leng));
}

Media::ICCProfile::~ICCProfile()
{
}

Int32 Media::ICCProfile::GetCMMType() const
{
	return ReadMInt32(&this->iccBuff[4]);
}

void Media::ICCProfile::GetProfileVer(OutParam<UInt8> majorVer, OutParam<UInt8> minorVer, OutParam<UInt8> bugFixVer) const
{
	majorVer.Set(this->iccBuff[8]);
	minorVer.Set((UInt8)(this->iccBuff[9] >> 4));
	bugFixVer.Set(this->iccBuff[9] & 0xf);
}

Int32 Media::ICCProfile::GetProfileClass() const
{
	return ReadMInt32(&this->iccBuff[12]);
}

Int32 Media::ICCProfile::GetDataColorspace() const
{
	return ReadMInt32(&this->iccBuff[16]);
}

Int32 Media::ICCProfile::GetPCS() const
{
	return ReadMInt32(&this->iccBuff[20]);
}

void Media::ICCProfile::GetCreateTime(NN<Data::DateTime> createTime) const
{
	ReadDateTimeNumber(&this->iccBuff[24], createTime);
}

Int32 Media::ICCProfile::GetPrimaryPlatform() const
{
	return ReadMInt32(&this->iccBuff[40]);
}

Int32 Media::ICCProfile::GetProfileFlag() const
{
	return ReadMInt32(&this->iccBuff[44]);
}

Int32 Media::ICCProfile::GetDeviceManufacturer() const
{
	return ReadMInt32(&this->iccBuff[48]);
}

Int32 Media::ICCProfile::GetDeviceModel() const
{
	return ReadMInt32(&this->iccBuff[52]);
}

Int32 Media::ICCProfile::GetDeviceAttrib() const
{
	return ReadMInt32(&this->iccBuff[56]);
}

Int32 Media::ICCProfile::GetDeviceAttrib2() const
{
	return ReadMInt32(&this->iccBuff[60]);
}

Int32 Media::ICCProfile::GetRenderingIntent() const
{
	return ReadMInt32(&this->iccBuff[64]);
}

Media::ICCProfile::CIEXYZ Media::ICCProfile::GetPCSIlluminant() const
{
	return ReadXYZNumber(&this->iccBuff[68]);
}

Int32 Media::ICCProfile::GetProfileCreator() const
{
	return ReadMInt32(&this->iccBuff[80]);
}

Int32 Media::ICCProfile::GetTagCount() const
{
	return ReadMInt32(&this->iccBuff[128]);
}

Optional<Media::LUT> Media::ICCProfile::CreateRLUT() const
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	Media::LUT *lut;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		UInt32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x72545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				UInt32 i;
				UInt8 *stab;
				UInt8 *dtab;
				valCnt = ReadMUInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt <= 1)
				{
					return nullptr;
				}
				NEW_CLASS(lut, Media::LUT(1, valCnt, 1, Media::LUT::DF_UINT16, CSTR("ICC Profile R")));
				stab = &this->iccBuff[tagOfst + 12];
				dtab = lut->GetTablePtr();
				i = 0;
				while (i < valCnt)
				{
					*(UInt16*)&dtab[i * 2] = ReadMUInt16(&stab[i * 2]);
					i++;
				}
				return lut;
			}
			else
			{
				return nullptr;
			}
		}
		i++;
	}
	return nullptr;
}

Optional<Media::LUT> Media::ICCProfile::CreateGLUT() const
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	Media::LUT *lut;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		UInt32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x67545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				UInt32 i;
				UInt8 *stab;
				UInt8 *dtab;
				valCnt = ReadMUInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt <= 1)
				{
					return nullptr;
				}
				NEW_CLASS(lut, Media::LUT(1, valCnt, 1, Media::LUT::DF_UINT16, CSTR("ICC Profile G")));
				stab = &this->iccBuff[tagOfst + 12];
				dtab = lut->GetTablePtr();
				i = 0;
				while (i < valCnt)
				{
					*(UInt16*)&dtab[i * 2] = ReadMUInt16(&stab[i * 2]);
					i++;
				}
				return lut;
			}
			else
			{
				return nullptr;
			}
		}
		i++;
	}
	return nullptr;
}

Optional<Media::LUT> Media::ICCProfile::CreateBLUT() const
{
	UInt32 cnt = ReadMUInt32(&this->iccBuff[128]);
	UInt32 i = 0;
	Media::LUT *lut;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		UInt32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x62545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				UInt32 i;
				UInt8 *stab;
				UInt8 *dtab;
				valCnt = ReadMUInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt <= 1)
				{
					return nullptr;
				}
				NEW_CLASS(lut, Media::LUT(1, valCnt, 1, Media::LUT::DF_UINT16, CSTR("ICC Profile B")));
				stab = &this->iccBuff[tagOfst + 12];
				dtab = lut->GetTablePtr();
				i = 0;
				while (i < valCnt)
				{
					*(UInt16*)&dtab[i * 2] = ReadMUInt16(&stab[i * 2]);
					i++;
				}
				return lut;
			}
			else
			{
				return nullptr;
			}
		}
		i++;
	}
	return nullptr;
}

Bool Media::ICCProfile::GetRedTransferParam(NN<Media::CS::TransferParam> param) const
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		Int32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x72545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				valCnt = ReadMInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt == 0)
				{
					param->Set(Media::CS::TRANT_LINEAR, 1.0);
				}
				else if (valCnt == 1)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadU8Fixed8Number((UInt8*)&this->iccBuff[tagOfst + 12]));
				}
				else
				{
					NN<Media::LUT> lut;
					if (this->CreateRLUT().SetTo(lut))
					{
						param->Set(lut);
						lut.Delete();
					}
				}

/*				Double gamma;
				Media::CS::TransferType tranType;
				tranType = FindTransferType(valCnt, (UInt16*)&this->iccBuff[tagOfst + 12], &gamma);
				param->Set(tranType, gamma);*/
				return true;
			}
			else if (tagType == 0x70617261) //parametricCurveType
			{
				Int32 funcType = ReadMInt16(&this->iccBuff[tagOfst + 8]);
				Double params[7];
				if (funcType == 0)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]));
					return true;
				}
				else if (funcType == 3)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = 0;
					params[6] = 0;
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
				else if (funcType == 4)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 32]);
					params[6] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 36]);
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	return false;
}

Bool Media::ICCProfile::GetGreenTransferParam(NN<Media::CS::TransferParam> param) const
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		Int32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x67545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				valCnt = ReadMInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt == 0)
				{
					param->Set(Media::CS::TRANT_LINEAR, 1.0);
				}
				else if (valCnt == 1)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadU8Fixed8Number((UInt8*)&this->iccBuff[tagOfst + 12]));
				}
				else
				{
					NN<Media::LUT> lut;
					if (this->CreateGLUT().SetTo(lut))
					{
						param->Set(lut);
						lut.Delete();
					}
				}

/*				Double gamma;
				Media::CS::TransferType tranType;
				tranType = FindTransferType(valCnt, (UInt16*)&this->iccBuff[tagOfst + 12], &gamma);
				param->Set(tranType, gamma);*/
				return true;
			}
			else if (tagType == 0x70617261) //parametricCurveType
			{
				Int32 funcType = ReadMInt16(&this->iccBuff[tagOfst + 8]);
				Double params[7];
				if (funcType == 0)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]));
					return true;
				}
				else if (funcType == 3)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = 0;
					params[6] = 0;
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
				else if (funcType == 4)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 32]);
					params[6] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 36]);
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	return false;
}

Bool Media::ICCProfile::GetBlueTransferParam(NN<Media::CS::TransferParam> param) const
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		Int32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x62545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				valCnt = ReadMInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt == 0)
				{
					param->Set(Media::CS::TRANT_LINEAR, 1.0);
				}
				else if (valCnt == 1)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadU8Fixed8Number((UInt8*)&this->iccBuff[tagOfst + 12]));
				}
				else
				{
					NN<Media::LUT> lut;
					if (this->CreateBLUT().SetTo(lut))
					{
						param->Set(lut);
						lut.Delete();
					}
				}

/*				Double gamma;
				Media::CS::TransferType tranType;
				tranType = FindTransferType(valCnt, (UInt16*)&this->iccBuff[tagOfst + 12], &gamma);
				param->Set(tranType, gamma);*/
				return true;
			}
			else if (tagType == 0x70617261) //parametricCurveType
			{
				Int32 funcType = ReadMInt16(&this->iccBuff[tagOfst + 8]);
				Double params[7];
				if (funcType == 0)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]));
					return true;
				}
				else if (funcType == 3)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = 0;
					params[6] = 0;
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
				else if (funcType == 4)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 32]);
					params[6] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 36]);
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	return false;
}

Bool Media::ICCProfile::GetColorPrimaries(NN<Media::ColorProfile::ColorPrimaries> color) const
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	Bool foundR = false;
	Bool foundG = false;
	Bool foundB = false;
	Bool foundW = false;
	CIEXYZ rxyz;
	CIEXYZ gxyz;
	CIEXYZ bxyz;
	CIEXYZ wxyz;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x7258595A)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				rxyz = ReadXYZNumber(&this->iccBuff[tagOfst + 8]);
				foundR = true;
			}
			else
			{
				return false;
			}
		}
		else if (tagSign == 0x6758595A)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				gxyz = ReadXYZNumber(&this->iccBuff[tagOfst + 8]);
				foundG = true;
			}
			else
			{
				return false;
			}
		}
		else if (tagSign == 0x6258595A)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				bxyz = ReadXYZNumber(&this->iccBuff[tagOfst + 8]);
				foundB = true;
			}
			else
			{
				return false;
			}
		}
		else if (tagSign == 0x77747074)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				wxyz = ReadXYZNumber(&this->iccBuff[tagOfst + 8]);
				foundW = true;
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	if (foundR && foundG && foundB && foundW)
	{
		Math::Matrix3 mat;
		Math::Matrix3 mat2;
		Math::Matrix3 mat3;
		Math::Vector3 vec1;
		Math::Vector3 vec2;

		Media::ColorProfile::ColorPrimaries::GetMatrixBradford(mat);
		mat2.Set(mat);
		mat3.SetIdentity();

		vec1 = mat.Multiply(Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(Media::ColorProfile::WPT_D50));
		vec2 = mat.Multiply(wxyz);
		mat.Inverse();
		mat3.vec[0].val[0] = vec2.val[0] / vec1.val[0];
		mat3.vec[1].val[1] = vec2.val[1] / vec1.val[1];
		mat3.vec[2].val[2] = vec2.val[2] / vec1.val[2];
		mat.Multiply(mat3);
		mat.Multiply(mat2);

		color->colorType = Media::ColorProfile::CT_CUSTOM;
		color->r = Media::ColorProfile::ColorPrimaries::XYZToxyY(mat.Multiply(rxyz)).GetXY();
		color->g = Media::ColorProfile::ColorPrimaries::XYZToxyY(mat.Multiply(gxyz)).GetXY();
		color->b = Media::ColorProfile::ColorPrimaries::XYZToxyY(mat.Multiply(bxyz)).GetXY();
		color->w = Media::ColorProfile::ColorPrimaries::XYZToxyY(wxyz).GetXY();
		return true;
	}
	else
	{
		color->SetColorType(Media::ColorProfile::CT_PUNKNOWN);
		return false;
	}
}

Bool Media::ICCProfile::SetToColorProfile(NN<Media::ColorProfile> colorProfile)
{
	if (this->GetRedTransferParam(colorProfile->GetRTranParam()) &&
		this->GetGreenTransferParam(colorProfile->GetGTranParam()) &&
		this->GetBlueTransferParam(colorProfile->GetBTranParam()) &&
		this->GetColorPrimaries(colorProfile->GetPrimaries()))
	{
		colorProfile->SetRAWICC(this->iccBuff.Arr().Ptr());
		return true;	
	}
	return false;
}

void Media::ICCProfile::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UInt8 majorVer;
	UInt8 minorVer;
	UInt8 bugFixVer;
	Data::DateTime dt;
	Int32 val;

	sb->AppendC(UTF8STRC("Preferred CMM Type = "));
	sb->Append(GetNameCMMType(this->GetCMMType()));

	this->GetProfileVer(majorVer, minorVer, bugFixVer);
	sb->AppendC(UTF8STRC("\r\nProfile version number = "));
	sb->AppendU16(majorVer);
	sb->AppendC(UTF8STRC("."));
	sb->AppendU16(minorVer);
	sb->AppendC(UTF8STRC("."));
	sb->AppendU16(bugFixVer);

	sb->AppendC(UTF8STRC("\r\nProfile/Device class = "));
	sb->Append(GetNameProfileClass(this->GetProfileClass()));

	sb->AppendC(UTF8STRC("\r\nData colour space = "));
	sb->Append(GetNameDataColorspace(this->GetDataColorspace()));

	sb->AppendC(UTF8STRC("\r\nPCS (profile connection space) = "));
	sb->Append(GetNameDataColorspace(this->GetPCS()));

	this->GetCreateTime(dt);
	sb->AppendC(UTF8STRC("\r\nCreate Time = "));
	sb->AppendDateTime(dt);

	sb->AppendC(UTF8STRC("\r\nPrimary Platform = "));
	sb->Append(GetNamePrimaryPlatform(this->GetPrimaryPlatform()));

	val = this->GetProfileFlag();
	if (val & 1)
	{
		sb->AppendC(UTF8STRC("\r\nEmbedded profile = True"));
	}
	else
	{
		sb->AppendC(UTF8STRC("\r\nEmbedded profile = False"));
	}
	if (val & 2)
	{
		sb->AppendC(UTF8STRC("\r\nProfile cannot be used independently of the embedded colour data = True"));
	}
	else
	{
		sb->AppendC(UTF8STRC("\r\nProfile cannot be used independently of the embedded colour data = False"));
	}

	sb->AppendC(UTF8STRC("\r\nDevice manufacturer = "));
	sb->Append(GetNameDeviceManufacturer(this->GetDeviceManufacturer()));

	sb->AppendC(UTF8STRC("\r\nDevice model = "));
	sb->Append(GetNameDeviceModel(this->GetDeviceModel()));

	sb->AppendC(UTF8STRC("\r\nPCS illuminant = "));
	GetDispCIEXYZ(sb, this->GetPCSIlluminant());

	sb->AppendC(UTF8STRC("\r\nProfile creator = "));
	sb->Append(GetNameDeviceManufacturer(this->GetProfileCreator()));

	UInt32 cnt = ReadMUInt32(&this->iccBuff[128]);
	UInt32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
		UInt32 tagLeng;
		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
		tagLeng = ReadMUInt32(&this->iccBuff[140 + 12 * i]);

		sb->AppendC(UTF8STRC("\r\n"));
		sb->Append(GetNameTag(tagSign));
		sb->AppendC(UTF8STRC(" = "));
		GetDispTagType(sb, &this->iccBuff[tagOfst], tagLeng);
		i++;
	}
}

Optional<Media::ICCProfile> Media::ICCProfile::Parse(Data::ByteArrayR buff)
{
	NN<Media::ICCProfile> profile;
	if (buff.ReadMU32(0) != buff.GetSize())
		return nullptr;
	if (buff.ReadMI32(36) != 0x61637370)
		return nullptr;
	
	NEW_CLASSNN(profile, Media::ICCProfile(buff.Arr()));

	return profile;
}

Bool Media::ICCProfile::ParseFrame(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt ofst, UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (ReadMUInt32(buff.Ptr()) != buffSize)
		return false;
	if (ReadMInt32(&buff[36]) != 0x61637370)
		return false;

	Data::DateTime dt;
	frame->AddUInt(ofst, 4, CSTR("ICC Size"), buffSize);
	frame->AddHex32Name(ofst + 4, CSTR("Preferred CMM Type"), ReadMUInt32(&buff[4]), GetNameCMMType(ReadMInt32(&buff[4])));
	frame->AddUInt(ofst + 8, 1, CSTR("Major Version"), buff[8]);
	frame->AddUInt(ofst + 9, 1, CSTR("Minor Version"), (UOSInt)buff[9] >> 4);
	frame->AddUInt(ofst + 9, 1, CSTR("Bug Fix Version"), buff[9] & 15);
	frame->AddUInt(ofst + 10, 1, CSTR("Subclass Major Version"), buff[10]);
	frame->AddUInt(ofst + 11, 1, CSTR("Subclass Minor Version"), buff[11]);
	frame->AddHex32Name(ofst + 12, CSTR("Profile/Device class"), ReadMUInt32(&buff[12]), GetNameProfileClass(ReadMInt32(&buff[12])));
	frame->AddHex32Name(ofst + 16, CSTR("Data colour space"), ReadMUInt32(&buff[16]), GetNameDataColorspace(ReadMInt32(&buff[16])));
	frame->AddHex32Name(ofst + 20, CSTR("PCS (profile connection space)"), ReadMUInt32(&buff[20]), GetNameDataColorspace(ReadMInt32(&buff[20])));
	ReadDateTimeNumber(&buff[24], dt);
	sptr = dt.ToString(sbuff);
	frame->AddField(ofst + 24, 12, CSTR("Create Time"), CSTRP(sbuff, sptr));
	frame->AddStrC(ofst + 36, 4, CSTR("Profile file signature"), &buff[36]);
	frame->AddHex32Name(ofst + 40, CSTR("Primary Platform"), ReadMUInt32(&buff[40]), GetNamePrimaryPlatform(ReadMInt32(&buff[40])));
	UInt32 flags = ReadMUInt32(&buff[44]);
	frame->AddHex32(ofst + 44, CSTR("Profile flags"), flags);
	frame->AddBool(ofst + 47, CSTR("Embedded profile"), (UInt8)(flags & 1));
	frame->AddBool(ofst + 47, CSTR("Profile cannot be used independently of the embedded colour data"), (UInt8)(flags & 2));
	frame->AddBool(ofst + 47, CSTR("MCS channels in this profile shall be a subset of the MCS channels in the profile it is connected to"), (UInt8)(flags & 4));
	frame->AddHex32Name(ofst + 48, CSTR("Device manufacturer"), ReadMUInt32(&buff[48]), GetNameDeviceManufacturer(ReadMInt32(&buff[48])));
	frame->AddHex32Name(ofst + 52, CSTR("Device model"), ReadMUInt32(&buff[52]), GetNameDeviceModel(ReadMInt32(&buff[52])));
	frame->AddHex64(ofst + 56, CSTR("Device attributes"), ReadMUInt64(&buff[56]));
	frame->AddBool(ofst + 63, CSTR("Transparency"), buff[63] & 1);
	frame->AddBool(ofst + 63, CSTR("Matte"), buff[63] & 2);
	frame->AddBool(ofst + 63, CSTR("Media Negative"), buff[63] & 4);
	frame->AddBool(ofst + 63, CSTR("Color Media White"), buff[63] & 8);
	frame->AddBool(ofst + 63, CSTR("Non-paper-based"), buff[63] & 16);
	frame->AddBool(ofst + 63, CSTR("Textured"), buff[63] & 32);
	frame->AddBool(ofst + 63, CSTR("Non-isotropic"), buff[63] & 64);
	frame->AddBool(ofst + 63, CSTR("Self-luminous"), buff[63] & 128);
	frame->AddUInt(ofst + 64, 4, CSTR("Rendering intent"), ReadMUInt32(&buff[64]));
	FrameAddXYZNumber(frame, ofst + 68, CSTR("PCS illuminant"), &buff[68]);
	frame->AddHex32Name(ofst + 80, CSTR("Profile creator"), ReadMUInt32(&buff[80]), GetNameDeviceManufacturer(ReadMInt32(&buff[80])));
	frame->AddHexBuff(ofst + 84, 16, CSTR("Profile ID"), &buff[84], ' ', false);
	frame->AddHex32(ofst + 100, CSTR("Spectral PCS"), ReadMUInt32(&buff[100]));
	frame->AddUInt(ofst + 104, 2, CSTR("Spectral PCS start wavelength (S)"), ReadMUInt16(&buff[104]));
	frame->AddUInt(ofst + 106, 2, CSTR("Spectral PCS end wavelength (E)"), ReadMUInt16(&buff[106]));
	frame->AddUInt(ofst + 108, 2, CSTR("Spectral PCS number of steps (n)"), ReadMUInt16(&buff[108]));
	frame->AddUInt(ofst + 110, 2, CSTR("Bi-Spectral PCS start wavelength (S)"), ReadMUInt16(&buff[110]));
	frame->AddUInt(ofst + 112, 2, CSTR("Bi-Spectral PCS end wavelength (E)"), ReadMUInt16(&buff[112]));
	frame->AddUInt(ofst + 114, 2, CSTR("Bi-Spectral PCS number of steps (n)"), ReadMUInt16(&buff[114]));
	frame->AddHex32(ofst + 116, CSTR("MCS"), ReadMUInt32(&buff[116]));
	frame->AddHex32(ofst + 120, CSTR("Profile/device sub-class"), ReadMUInt32(&buff[120]));
	frame->AddHex32(ofst + 124, CSTR("Reserved"), ReadMUInt32(&buff[124]));
	UInt32 cnt = ReadMUInt32(&buff[128]);
	frame->AddUInt(ofst + 128, 4, CSTR("Tag count"), cnt);
	UInt32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		UInt32 tagOfst;
		UInt32 tagLeng;
		Text::CStringNN tagName;
		tagSign = ReadMInt32(&buff[132 + 12 * i]);
		tagOfst = ReadMUInt32(&buff[136 + 12 * i]);
		tagLeng = ReadMUInt32(&buff[140 + 12 * i]);
		tagName = GetNameTag(tagSign);
		frame->AddHex32Name(ofst + 132 + 12 * i, CSTR("Tag signature"), (UInt32)tagSign, tagName);
		frame->AddUInt(ofst + 136 + 12 * i, 4, CSTR("Offset to beginning of tag data element"), tagOfst);
		frame->AddUInt(ofst + 140 + 12 * i, 4, CSTR("Size of tag data element"), tagLeng);

		FrameDispTagType(frame, ofst + tagOfst, tagName, &buff[tagOfst], tagLeng);
		i++;
	}
	return true;
}

void Media::ICCProfile::ReadDateTimeNumber(UnsafeArray<const UInt8> buff, NN<Data::DateTime> dt)
{
	dt->SetValue(ReadMUInt16(&buff[0]), (UInt8)ReadMUInt16(&buff[2]), (UInt8)ReadMUInt16(&buff[4]), (UInt8)ReadMUInt16(&buff[6]), (UInt8)ReadMUInt16(&buff[8]), (UInt8)ReadMUInt16(&buff[10]), 0);
}

Media::ICCProfile::CIEXYZ Media::ICCProfile::ReadXYZNumber(UnsafeArray<const UInt8> buff)
{
	return CIEXYZ(ReadS15Fixed16Number(&buff[0]), ReadS15Fixed16Number(&buff[4]), ReadS15Fixed16Number(&buff[8]));
}

Double Media::ICCProfile::ReadS15Fixed16Number(UnsafeArray<const UInt8> buff)
{
	return ReadMInt32(&buff[0]) / 65536.0;
}

Double Media::ICCProfile::ReadU16Fixed16Number(UnsafeArray<const UInt8> buff)
{
	return ((UInt32)ReadMInt32(&buff[0])) / 65536.0;
}

Double Media::ICCProfile::ReadU8Fixed8Number(UnsafeArray<const UInt8> buff)
{
	return ((UInt16)ReadMInt16(&buff[0])) / 256.0;
}

Text::CStringNN Media::ICCProfile::GetNameCMMType(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("(not used)");
	case 0x33324254:
		return CSTR("the imaging factory CMM");
	case 0x41434D53:
		return CSTR("Agfa CMM");
	case 0x41444245:
		return CSTR("Adobe CMM");
	case 0x43434D53:
		return CSTR("ColorGear CMM");
	case 0x45464920:
		return CSTR("EFI CMM");
	case 0x46462020:
		return CSTR("Fuji Film CMM");
	case 0x48434d4d:
		return CSTR("Harlequin RIP CMM");
	case 0x48444d20:
		return CSTR("Heidelberg CMM");
	case 0x4b434d53:
		return CSTR("Kodak CMM");
	case 0x4C676f53:
		return CSTR("LogoSync CMM");
	case 0x4d434d44:
		return CSTR("Konica Minolta CMM");
	case 0x52474d53:
		return CSTR("DeviceLink CMM");
	case 0x53494343:
		return CSTR("SampleICC CMM");
	case 0x5349474E:
		return CSTR("Mutoh CMM");
	case 0x5543434D:
		return CSTR("ColorGear CMM Lite");
	case 0x55434D53:
		return CSTR("ColorGear CMM C");
	case 0x57544720:
		return CSTR("Ware to Go CMM");
	case 0x6170706C:
		return CSTR("Apple CMM");
	case 0x6172676C:
		return CSTR("Argyll CMS CMM");
	case 0x7a633030:
		return CSTR("Zoran CMM");
	case 0x6C636d73:
		return CSTR("Little CMS CMM");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Media::ICCProfile::GetNameProfileClass(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("(not used)");
	case 0x73636E72:
		return CSTR("Input device profile");
	case 0x6D6E7472:
		return CSTR("Display device profile");
	case 0x70727472:
		return CSTR("Output device profile");
	case 0x6C696E6B:
		return CSTR("DeviceLink profile");
	case 0x73706163:
		return CSTR("ColorSpace profile");
	case 0x61627374:
		return CSTR("Abstract profile");
	case 0x6E6D636C:
		return CSTR("NamedColor profile");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Media::ICCProfile::GetNameDataColorspace(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("(not used)");
	case 0x58595A20:
		return CSTR("nCIEXYZ or PCSXYZ");
	case 0x4C616220:
		return CSTR("CIELAB or PCSLAB");
	case 0x4C757620:
		return CSTR("CIELUV");
	case 0x59436272:
		return CSTR("YCbCr");
	case 0x59787920:
		return CSTR("CIEYxy");
	case 0x52474220:
		return CSTR("RGB");
	case 0x47524159:
		return CSTR("Gray");
	case 0x48535620:
		return CSTR("HSV");
	case 0x484C5320:
		return CSTR("HLS");
	case 0x434D594B:
		return CSTR("CMYK");
	case 0x434D5920:
		return CSTR("CMY");
	case 0x32434C52:
		return CSTR("2 colour");
	case 0x33434C52:
		return CSTR("3 colour");
	case 0x34434C52:
		return CSTR("4 colour");
	case 0x35434C52:
		return CSTR("5 colour");
	case 0x36434C52:
		return CSTR("6 colour");
	case 0x37434C52:
		return CSTR("7 colour");
	case 0x38434C52:
		return CSTR("8 colour");
	case 0x39434C52:
		return CSTR("9 colour");
	case 0x41434C52:
		return CSTR("10 colour");
	case 0x42434C52:
		return CSTR("11 colour");
	case 0x43434C52:
		return CSTR("12 colour");
	case 0x44434C52:
		return CSTR("13 colour");
	case 0x45434C52:
		return CSTR("14 colour");
	case 0x46434C52:
		return CSTR("15 colour");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Media::ICCProfile::GetNamePrimaryPlatform(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("(not used)");
	case 0x4150504C:
		return CSTR("Apple Computer, Inc.");
	case 0x4D534654:
		return CSTR("Microsoft Corporation");
	case 0x53474920:
		return CSTR("Silicon Graphics, Inc.");
	case 0x53554E57:
		return CSTR("Sun Microsystems, Inc.");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Media::ICCProfile::GetNameDeviceManufacturer(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("(not used)");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Media::ICCProfile::GetNameDeviceModel(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("(not used)");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Media::ICCProfile::GetNameTag(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("(not used)");
	case 0x41324230:
		return CSTR("AToB0Tag");
	case 0x41324231:
		return CSTR("AToB1Tag");
	case 0x41324232:
		return CSTR("AToB2Tag");
	case 0x42324130:
		return CSTR("BToA0Tag");
	case 0x42324131:
		return CSTR("BToA1Tag");
	case 0x42324132:
		return CSTR("BToA2Tag");
	case 0x42324430:
		return CSTR("BToD0Tag");
	case 0x42324431:
		return CSTR("BToD1Tag");
	case 0x42324432:
		return CSTR("BToD2Tag");
	case 0x42324433:
		return CSTR("BToD3Tag");
	case 0x44324230:
		return CSTR("DToB0Tag");
	case 0x44324231:
		return CSTR("DToB1Tag");
	case 0x44324232:
		return CSTR("DToB2Tag");
	case 0x44324233:
		return CSTR("DToB3Tag");
	case 0x62545243:
		return CSTR("blueTRCTag");
	case 0x6258595A:
		return CSTR("blueMatrixColumnTag");
	case 0x626B7074:
		return CSTR("mediaBlackPointTag");
	case 0x63616C74:
		return CSTR("calibrationDateTimeTag");
	case 0x63686164:
		return CSTR("chromaticAdaptationTag");
	case 0x63696973:
		return CSTR("colorimetricIntentImageStateTag");
	case 0x636C6F74:
		return CSTR("colorantTableOutTag");
	case 0x6368726D:
		return CSTR("chromaticityTag");
	case 0x636C726F:
		return CSTR("colorantOrderTag");
	case 0x636C7274:
		return CSTR("colorantTableTag");
	case 0x63707274:
		return CSTR("copyrightTag");
	case 0x64657363:
		return CSTR("profileDescriptionTag");
	case 0x646D6464:
		return CSTR("deviceModelDescTag");
	case 0x646D6E64:
		return CSTR("deviceMfgDescTag");
	case 0x67545243:
		return CSTR("greenTRCTag");
	case 0x6758595A:
		return CSTR("greenMatrixColumnTag");
	case 0x67616D74:
		return CSTR("gamutTag");
	case 0x6B545243:
		return CSTR("grayTRCTag");
	case 0x6C756D69:
		return CSTR("luminanceTag");
	case 0x6D656173:
		return CSTR("measurementTag");
	case 0x6E636C32:
		return CSTR("namedColor2Tag");
	case 0x70726530:
		return CSTR("preview0Tag");
	case 0x70726531:
		return CSTR("preview1Tag");
	case 0x70726532:
		return CSTR("preview2Tag");
	case 0x70736571:
		return CSTR("profileSequenceDescTag");
	case 0x70736964:
		return CSTR("profileSequenceIdentifierTag");
	case 0x72545243:
		return CSTR("redTRCTag");
	case 0x7258595A:
		return CSTR("redMatrixColumnTag");
	case 0x72657370:
		return CSTR("outputResponseTag");
	case 0x72696730:
		return CSTR("perceptualRenderingIntentGamutTag");
	case 0x72696732:
		return CSTR("saturationRenderingIntentGamutTag");
	case 0x74617267:
		return CSTR("charTargetTag");
	case 0x74656368:
		return CSTR("technologyTag");
	case 0x76696577:
		return CSTR("viewingConditionsTag");
	case 0x76756564:
		return CSTR("viewingCondDescTag");
	case 0x77747074:
		return CSTR("mediaWhitePointTag");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Media::ICCProfile::GetNameStandardObserver(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("Unknown");
	case 1:
		return CSTR("CIE 1931 standard colorimetric observer");
	case 2:
		return CSTR("CIE 1964 standard colorimetric observer");
	default:
		return CSTR("Not defined");
	}
}

Text::CStringNN Media::ICCProfile::GetNameStandardIlluminent(Int32 val)
{
	switch (val)
	{
	case 0:
		return CSTR("Unknown");
	case 1:
		return CSTR("D50");
	case 2:
		return CSTR("D65");
	case 3:
		return CSTR("D93");
	case 4:
		return CSTR("F2");
	case 5:
		return CSTR("D55");
	case 6:
		return CSTR("A");
	case 7:
		return CSTR("Equi-Power (E)");
	case 8:
		return CSTR("F8");
	default:
		return CSTR("Not defined");
	}
}

void Media::ICCProfile::GetDispCIEXYZ(NN<Text::StringBuilderUTF8> sb, const CIEXYZ &xyz)
{
	sb->AppendC(UTF8STRC("X = "));
	Text::SBAppendF64(sb, xyz.val[0]);
	sb->AppendC(UTF8STRC(", Y = "));
	Text::SBAppendF64(sb, xyz.val[1]);
	sb->AppendC(UTF8STRC(", Z = "));
	Text::SBAppendF64(sb, xyz.val[2]);

	Double sum = xyz.val[0] + xyz.val[1] + xyz.val[2];
	if (sum != 0)
	{
		sb->AppendC(UTF8STRC(", x = "));
		Text::SBAppendF64(sb, xyz.val[0] / sum);
		sb->AppendC(UTF8STRC(", y = "));
		Text::SBAppendF64(sb, xyz.val[1] / sum);
	}
}

void Media::ICCProfile::GetDispTagType(NN<Text::StringBuilderUTF8> sb, UnsafeArray<UInt8> buff, UInt32 leng)
{
	Int32 typ = ReadMInt32(&buff[0]);
	Int32 nCh;
	Int32 val;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Media::CS::TransferType tt;
	Double gamma;
	switch(typ)
	{
	case 0:
		sb->AppendC(UTF8STRC("(not used)"));
		break;
	case 0x6368726D:
		nCh = ReadMInt16(&buff[8]);
		val = ReadMInt16(&buff[10]);
		sb->AppendI32(val);
		sb->AppendC(UTF8STRC(" {"));
		val = 0;
		while (val < nCh)
		{
			if (val > 0)
			{
				sb->AppendC(UTF8STRC(", "));
			}
			sb->AppendC(UTF8STRC("("));
			Text::SBAppendF64(sb, ReadU16Fixed16Number(&buff[val * 8 + 12]));
			sb->AppendC(UTF8STRC(", "));
			Text::SBAppendF64(sb, ReadU16Fixed16Number(&buff[val * 8 + 16]));
			sb->AppendC(UTF8STRC(")"));
			val++;
		}
		sb->AppendC(UTF8STRC("}"));
		break;
	case 0x74657874: //textType
		{
			if (buff[leng - 1])
			{
				sb->AppendC((const UTF8Char*)&buff[8], leng - 8);
			}
			else
			{
				sb->AppendC((const UTF8Char*)&buff[8], leng - 9);
			}
		}
		break;
	case 0x58595A20: //XYZType
		val = 8;
		nCh = 0;
		while ((UInt32)val <= leng - 12)
		{
			if (nCh)
				sb->AppendC(UTF8STRC("  "));
			GetDispCIEXYZ(sb, ReadXYZNumber(&buff[val]));
			val += 12;
			nCh++;
		}
		break;
	case 0x76696577: //viewingConditionsTag
		sb->AppendC(UTF8STRC("Illuminant: {"));
		GetDispCIEXYZ(sb, ReadXYZNumber(&buff[8]));

		sb->AppendC(UTF8STRC("}, Surround: {"));
		GetDispCIEXYZ(sb, ReadXYZNumber(&buff[20]));
		sb->AppendC(UTF8STRC("}, Illuminant type = "));
		sb->AppendI32(ReadMInt32(&buff[32]));
		break;
	case 0x6D656173: //measurementType
		sb->AppendC(UTF8STRC("Standard observer = "));
		sb->Append(GetNameStandardObserver(ReadMInt32(&buff[8])));
		sb->AppendC(UTF8STRC(", Measurement backing: {"));
		GetDispCIEXYZ(sb, ReadXYZNumber(&buff[12]));
		sb->AppendC(UTF8STRC("}, Measurement geometry = "));
		sb->AppendI32(ReadMInt32(&buff[24]));
		sb->AppendC(UTF8STRC(", Measurement flare = "));
		sb->AppendI32(ReadMInt32(&buff[28]));
		sb->AppendC(UTF8STRC(", Standard illuminent = "));
		sb->Append(GetNameStandardIlluminent(ReadMInt32(&buff[32])));
		break;
	case 0x64657363: //desc
		{
			val = ReadMInt32(&buff[8]);
			if (buff[12 + val - 1])
			{
				sb->AppendC((const UTF8Char*)&buff[12], (UInt32)val);
			}
			else
			{
				sb->AppendC((const UTF8Char*)&buff[12], (UInt32)val - 1);
			}
		}
		break;
	case 0x73696720: //signatureType
		sb->AppendHex32(ReadMUInt32(&buff[8]));
		break;
	case 0x63757276: //curveType
		val = ReadMInt32(&buff[8]);
		sb->AppendC(UTF8STRC("Curve: "));
		if (val > 1)
		{
			sb->AppendI32(val);
			sb->AppendC(UTF8STRC(" entries, "));
			sb->AppendC(UTF8STRC("Closed to "));
		}
		tt = FindTransferType((UInt32)val, (UInt16*)&buff[12], gamma);
		sb->Append(Media::CS::TransferTypeGetName(tt));
		if (tt == Media::CS::TRANT_GAMMA)
		{
			sb->AppendC(UTF8STRC(", gamma = "));
			Text::SBAppendF64(sb, gamma);
		}
		break;
	case 0x70617261: //parametricCurveType
		sb->AppendC(UTF8STRC("CurveType: "));
		sb->AppendI16(ReadMInt16(&buff[8]));
		{
			Double g;
			Double a;
			Double b;
			Double c;
			Double d;
			Double e;
			Double f;

			switch (ReadMInt16(&buff[8]))
			{
			case 0:
				g = ReadS15Fixed16Number(&buff[12]);
				sb->AppendC(UTF8STRC(" Y = X ^ "));
				Text::SBAppendF64(sb, g);
				break;
			case 1:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				break;
			case 2:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				break;
			case 3:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				d = ReadS15Fixed16Number(&buff[28]);
				sb->AppendC(UTF8STRC(" if (X >= "));
				Text::SBAppendF64(sb, d);
				sb->AppendC(UTF8STRC(") Y = ("));
				Text::SBAppendF64(sb, a);
				sb->AppendC(UTF8STRC(" * X + "));
				Text::SBAppendF64(sb, b);
				sb->AppendC(UTF8STRC(") ^ "));
				Text::SBAppendF64(sb, g);
				sb->AppendC(UTF8STRC(" else Y = "));
				Text::SBAppendF64(sb, c);
				sb->AppendC(UTF8STRC(" * X"));
				break;
			case 4:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				d = ReadS15Fixed16Number(&buff[28]);
				e = ReadS15Fixed16Number(&buff[32]);
				f = ReadS15Fixed16Number(&buff[36]);
				sb->AppendC(UTF8STRC(" if (X >= "));
				Text::SBAppendF64(sb, d);
				sb->AppendC(UTF8STRC(") Y = ("));
				Text::SBAppendF64(sb, a);
				sb->AppendC(UTF8STRC(" * X + "));
				Text::SBAppendF64(sb, b);
				sb->AppendC(UTF8STRC(") ^ "));
				Text::SBAppendF64(sb, g);
				sb->AppendC(UTF8STRC(" + "));
				Text::SBAppendF64(sb, e);
				sb->AppendC(UTF8STRC(" else Y = "));
				Text::SBAppendF64(sb, c);
				sb->AppendC(UTF8STRC(" * X + "));
				Text::SBAppendF64(sb, f);
				break;
			default:
				break;
			}
		}
		break;
	case 0x6d667431: //lut8Type
		sb->AppendC(UTF8STRC("LUT8"));
		break;
	case 0x6d667432: //lut16Type
		sb->AppendC(UTF8STRC("LUT16"));
		break;
	case 0x75693332: //uInt32ArrayType
		sb->AppendC(UTF8STRC("uInt32 Array ("));
		sb->AppendU32((leng - 8) >> 2);
		sb->AppendC(UTF8STRC(")"));
		break;
	case 0x75693038: //uInt8ArrayType
		sb->AppendC(UTF8STRC("uInt8 Array ("));
		sb->AppendU32((leng - 8));
		sb->AppendC(UTF8STRC(")"));
		break;
	case 0x73663332: //s15Fixed16ArrayType
		sb->AppendC(UTF8STRC("s15Fixed16 Array ("));
		sb->AppendU32((leng - 8) >> 2);
		sb->AppendC(UTF8STRC(")"));
		break;
	case 0x6D6C7563: //multiLocalizedUnicodeType
		{
			Text::Encoding enc(1201);
			OSInt i;
			OSInt j;
			i = ReadMInt32(&buff[8]);
			j = 16;
			while (i-- > 0)
			{
				sptr = enc.UTF8FromBytes(sbuff, &buff[ReadMInt32(&buff[j + 8])], ReadMUInt32(&buff[j + 4]), 0);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				if (i > 0)
				{
					sb->AppendC(UTF8STRC(", "));
				}
				j += 12;
			}
		}
		break;
	case 0x6d6d6f64:
		sb->AppendC(UTF8STRC("Unknown (mmod)"));
		break;
	case 0x6D414220:
		sb->AppendC(UTF8STRC("lutAToBType"));
		break;
	case 0x6D424120:
		sb->AppendC(UTF8STRC("lutBToAType"));
		break;
	default:
		sb->AppendC(UTF8STRC("Unknown"));
		break;
	}
}

Media::CS::TransferType Media::ICCProfile::FindTransferType(UOSInt colorCount, UnsafeArray<UInt16> curveColors, OutParam<Double> gamma)
{
	Media::CS::TransferType trans[] = {Media::CS::TRANT_sRGB, Media::CS::TRANT_BT709, Media::CS::TRANT_GAMMA, Media::CS::TRANT_LINEAR, Media::CS::TRANT_SMPTE240};
	UOSInt tranCnt = sizeof(trans) / sizeof(trans[0]);
	if (colorCount == 0)
	{
		gamma.Set(1.0);
		return Media::CS::TRANT_LINEAR;
	}
	else if (colorCount == 1)
	{
		gamma.Set(ReadU8Fixed8Number((UInt8*)&curveColors[0]));
		return Media::CS::TRANT_GAMMA;
	}

	NN<Media::CS::TransferFunc> *funcs = MemAlloc(NN<Media::CS::TransferFunc>, tranCnt);
	Double *diffSqrSum = MemAlloc(Double, tranCnt);
	UOSInt i = tranCnt;
	while (i-- > 0)
	{
		Media::CS::TransferParam param(trans[i], 2.2);
		funcs[i] = Media::CS::TransferFunc::CreateFunc(param);
		diffSqrSum[i] = 0;
	}

	Double mulVal = 1.0 / UOSInt2Double(colorCount - 1);
	Double colVal = 1.0 / 65535.0;
	Double v;
	Double tv;

	UOSInt j = 0;
	while (j < colorCount)
	{
		v = ReadMInt16((UInt8*)&curveColors[j]) * colVal;
		i = tranCnt;
		while (i-- > 0)
		{
			tv = funcs[i]->InverseTransfer(UOSInt2Double(j) * mulVal);
			diffSqrSum[i] += (tv - v) * (tv - v);
		}
		j++;
	}
	Double minVal = diffSqrSum[0];
	Media::CS::TransferType minType = trans[0];
	i = tranCnt;
	while (i-- > 1)
	{
		if (diffSqrSum[i] < minVal)
		{
			minType = trans[i];
			minVal = diffSqrSum[i];
		}
		funcs[i].Delete();
	}

	MemFree(funcs);
	MemFree(diffSqrSum);
	gamma.Set(2.2);
	return minType;
}

UnsafeArray<UTF8Char> Media::ICCProfile::GetProfilePath(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UTF8Char> sptr = sbuff;
#if defined(WIN32)
	sptr = IO::Path::GetOSPath(sptr).Or(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\\System32\\spool\\drivers\\color"));
#endif
	return sptr;
}

Optional<Media::ICCProfile> Media::ICCProfile::NewSRGBProfile()
{
	return Parse(Data::ByteArrayR(srgbICC, sizeof(srgbICC)));
}

UnsafeArray<const UInt8> Media::ICCProfile::GetSRGBICCData()
{
	return srgbICC;
}

void Media::ICCProfile::FrameAddXYZNumber(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt ofst, Text::CStringNN fieldName, UnsafeArray<const UInt8> xyzBuff)
{
	Text::StringBuilderUTF8 sb;
	GetDispCIEXYZ(sb, ReadXYZNumber(xyzBuff));
	frame->AddField(ofst, 12, fieldName, sb.ToCString());
}


void Media::ICCProfile::FrameDispTagType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UOSInt ofst, Text::CStringNN fieldName, UnsafeArray<const UInt8> buff, UInt32 leng)
{
	UInt32 typ = ReadMUInt32(&buff[0]);
	Int32 nCh;
	Int32 val;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Media::CS::TransferType tt;
	Double gamma;
	frame->AddStrC(ofst, 4, CSTR("Tag Type"), buff);
	frame->AddUInt(ofst + 4, 4, CSTR("Reserved"), ReadMUInt32(&buff[4]));
	switch(typ)
	{
	case 0:
		frame->AddField(ofst + 8, leng - 8, fieldName, CSTR("(not used)"));
		break;
	case 0x6368726D:
		{
			nCh = ReadMInt16(&buff[8]);
			val = ReadMInt16(&buff[10]);
			Text::StringBuilderUTF8 sb;
			sb.AppendI32(val);
			sb.AppendC(UTF8STRC(" {"));
			val = 0;
			while (val < nCh)
			{
				if (val > 0)
				{
					sb.AppendC(UTF8STRC(", "));
				}
				sb.AppendC(UTF8STRC("("));
				sb.AppendDouble(ReadU16Fixed16Number(&buff[val * 8 + 12]));
				sb.AppendC(UTF8STRC(", "));
				sb.AppendDouble(ReadU16Fixed16Number(&buff[val * 8 + 16]));
				sb.AppendC(UTF8STRC(")"));
				val++;
			}
			sb.AppendC(UTF8STRC("}"));
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x74657874: //textType
		{
			if (buff[leng - 1])
			{
				frame->AddStrC(ofst + 8, leng - 8, fieldName, &buff[8]);
			}
			else
			{
				frame->AddStrS(ofst + 8, leng - 8, fieldName, &buff[8]);
			}
		}
		break;
	case 0x58595A20: //XYZType
		{
			val = 8;
			nCh = 0;
			Text::StringBuilderUTF8 sb;
			while ((UInt32)val <= leng - 12)
			{
				if (nCh)
					sb.AppendC(UTF8STRC("  "));
				GetDispCIEXYZ(sb, ReadXYZNumber(&buff[val]));
				val += 12;
				nCh++;
			}
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x76696577: //viewingConditionsTag
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Illuminant: {"));
			GetDispCIEXYZ(sb, ReadXYZNumber(&buff[8]));

			sb.AppendC(UTF8STRC("}, Surround: {"));
			GetDispCIEXYZ(sb, ReadXYZNumber(&buff[20]));
			sb.AppendC(UTF8STRC("}, Illuminant type = "));
			sb.AppendI32(ReadMInt32(&buff[32]));
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x6D656173: //measurementType
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Standard observer = "));
			sb.Append(GetNameStandardObserver(ReadMInt32(&buff[8])));
			sb.AppendC(UTF8STRC(", Measurement backing: {"));
			GetDispCIEXYZ(sb, ReadXYZNumber(&buff[12]));
			sb.AppendC(UTF8STRC("}, Measurement geometry = "));
			sb.AppendI32(ReadMInt32(&buff[24]));
			sb.AppendC(UTF8STRC(", Measurement flare = "));
			sb.AppendI32(ReadMInt32(&buff[28]));
			sb.AppendC(UTF8STRC(", Standard illuminent = "));
			sb.Append(GetNameStandardIlluminent(ReadMInt32(&buff[32])));
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x64657363: //desc
		{
			val = ReadMInt32(&buff[8]);
			frame->AddUInt(ofst + 8, 4, CSTR("Desc Leng"), (UInt32)val);
			if (buff[12 + val - 1])
			{
				frame->AddStrC(ofst + 12, (UInt32)val, fieldName, (const UTF8Char*)&buff[12]);
			}
			else
			{
				frame->AddStrS(ofst + 12, (UInt32)val, fieldName, (const UTF8Char*)&buff[12]);
			}
		}
		break;
	case 0x73696720: //signatureType
		frame->AddHex32(ofst + 8, fieldName, ReadMUInt32(&buff[8]));
		break;
	case 0x63757276: //curveType
		{
			val = ReadMInt32(&buff[8]);
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Curve: "));
			if (val > 1)
			{
				sb.AppendI32(val);
				sb.AppendC(UTF8STRC(" entries, "));
				sb.AppendC(UTF8STRC("Closed to "));
			}
			tt = FindTransferType((UInt32)val, (UInt16*)&buff[12], gamma);
			sb.Append(Media::CS::TransferTypeGetName(tt));
			if (tt == Media::CS::TRANT_GAMMA)
			{
				sb.AppendC(UTF8STRC(", gamma = "));
				sb.AppendDouble(gamma);
			}
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x70617261: //parametricCurveType
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("CurveType: "));
			sb.AppendI16(ReadMInt16(&buff[8]));
			Double g;
			Double a;
			Double b;
			Double c;
			Double d;
			Double e;
			Double f;

			switch (ReadMInt16(&buff[8]))
			{
			case 0:
				g = ReadS15Fixed16Number(&buff[12]);
				sb.AppendC(UTF8STRC(" Y = X ^ "));
				sb.AppendDouble(g);
				break;
			case 1:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				break;
			case 2:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				break;
			case 3:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				d = ReadS15Fixed16Number(&buff[28]);
				sb.AppendC(UTF8STRC(" if (X >= "));
				sb.AppendDouble(d);
				sb.AppendC(UTF8STRC(") Y = ("));
				sb.AppendDouble(a);
				sb.AppendC(UTF8STRC(" * X + "));
				sb.AppendDouble(b);
				sb.AppendC(UTF8STRC(") ^ "));
				sb.AppendDouble(g);
				sb.AppendC(UTF8STRC(" else Y = "));
				sb.AppendDouble(c);
				sb.AppendC(UTF8STRC(" * X"));
				break;
			case 4:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				d = ReadS15Fixed16Number(&buff[28]);
				e = ReadS15Fixed16Number(&buff[32]);
				f = ReadS15Fixed16Number(&buff[36]);
				sb.AppendC(UTF8STRC(" if (X >= "));
				sb.AppendDouble(d);
				sb.AppendC(UTF8STRC(") Y = ("));
				sb.AppendDouble(a);
				sb.AppendC(UTF8STRC(" * X + "));
				sb.AppendDouble(b);
				sb.AppendC(UTF8STRC(") ^ "));
				sb.AppendDouble(g);
				sb.AppendC(UTF8STRC(" + "));
				sb.AppendDouble(e);
				sb.AppendC(UTF8STRC(" else Y = "));
				sb.AppendDouble(c);
				sb.AppendC(UTF8STRC(" * X + "));
				sb.AppendDouble(f);
				break;
			default:
				break;
			}
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x6d667431: //lut8Type
		frame->AddField(ofst + 8, leng - 8, fieldName, CSTR("LUT8"));
		break;
	case 0x6d667432: //lut16Type
		frame->AddField(ofst + 8, leng - 8, fieldName, CSTR("LUT16"));
		break;
	case 0x75693332: //uInt32ArrayType
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("uInt32 Array ("));
			sb.AppendU32((leng - 8) >> 2);
			sb.AppendC(UTF8STRC(")"));
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x75693038: //uInt8ArrayType
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("uInt8 Array ("));
			sb.AppendU32((leng - 8));
			sb.AppendC(UTF8STRC(")"));
			frame->AddField(ofst + 8, leng - 8, fieldName, sb.ToCString());
		}
		break;
	case 0x73663332: //s15Fixed16ArrayType
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("s15Fixed16 Array ("));
			sb.AppendU32((leng - 8) >> 2);
			sb.AppendC(UTF8STRC(")"));
			frame->AddField(ofst, 4, fieldName, sb.ToCString());

			UOSInt i = 8;
			UOSInt j = 0;
			while (i < leng)
			{
				sb.ClearStr();
				sb.Append(fieldName);
				sb.AppendUTF8Char('[');
				sb.AppendUOSInt(j);
				sb.AppendUTF8Char(']');

				sptr = Text::StrDouble(sbuff, ReadS15Fixed16Number(&buff[i]));
				frame->AddField(ofst + i, 4, sb.ToCString(), CSTRP(sbuff, sptr));
				i += 4;
				j++;
			}
		}
		break;
	case 0x6D6C7563: //multiLocalizedUnicodeType
		{
			Text::StringBuilderUTF8 sb;
			Text::Encoding enc(1201);
			UOSInt i;
			UInt32 cnt;
			UOSInt j;
			UOSInt strOfst;
			UOSInt strLen;
			cnt = ReadMUInt32(&buff[8]);
			frame->AddUInt(ofst + 8, 4, CSTR("Number of records (n)"), cnt);
			frame->AddUInt(ofst + 12, 4, CSTR("Record Size"), ReadMUInt32(&buff[12]));
			i = 0;
			j = 16;
			while (i < cnt)
			{
				frame->AddStrC(ofst + j, 2, CSTR("Record Language Code"), &buff[j]);
				frame->AddStrC(ofst + j + 2, 2, CSTR("Record Country Code"), &buff[j + 2]);
				frame->AddUInt(ofst + j + 4, 4, CSTR("Record String Length"), strLen = ReadMUInt32(&buff[j + 4]));
				frame->AddUInt(ofst + j + 8, 4, CSTR("Record String Offset"), strOfst = ReadMUInt32(&buff[j + 8]));
				sptr = enc.UTF8FromBytes(sbuff, &buff[strOfst], strLen, 0);
				sb.ClearStr();
				sb.Append(fieldName);
				sb.AppendUTF8Char('[');
				sb.AppendUOSInt(i);
				sb.AppendUTF8Char(']');
				frame->AddField(ofst + strOfst, strLen, sb.ToCString(), CSTRP(sbuff, sptr));
				j += 12;
				i++;
			}
		}
		break;
	case 0x6d6d6f64:
		frame->AddField(ofst + 8, leng - 8, fieldName, CSTR("Unknown (mmod)"));
		break;
	case 0x6D414220:
		frame->AddField(ofst + 8, leng - 8, fieldName, CSTR("lutAToBType"));
		break;
	case 0x6D424120:
		frame->AddField(ofst + 8, leng - 8, fieldName, CSTR("lutBToAType"));
		break;
	default:
		frame->AddField(ofst + 8, leng - 8, fieldName, CSTR("Unknown"));
		break;
	}
}
