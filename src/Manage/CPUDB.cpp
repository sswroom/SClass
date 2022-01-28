#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUDB.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

#if defined(NO_DB)
Manage::CPUDB::CPUSpecX86 Manage::CPUDB::intelCPU[] = {
	{0x04, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel 486 DX"), CSTR_NULL}
};

Manage::CPUDB::CPUSpecX86 Manage::CPUDB::amdCPU[] = {
	{0x04, 0x00,  0,   0,   33,   33,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DXLV-33"), CSTR_NULL}
};

Manage::CPUDB::CPUSpec Manage::CPUDB::cpuList[] = {
	{CSTR("88F6W11"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 370 88F6W11",            0,    0,   "Sheeva ARMv7", 1,  0,    0,    0,   0,  0,               0,  0,    0,    0,   0,  0,               0,  0,    0,    0,   0,  0,    0,     0}
};

Manage::CPUDB::CPUMapping Manage::CPUDB::cpuX86Map[] = {
	{CSTR("AMD A10-9700E RADEON R7, 10 COMPUTE CORES 4C+6G"),  CSTR("AD9700AHM44AB")}
};
#else
Manage::CPUDB::CPUSpecX86 Manage::CPUDB::intelCPU[] = {
	//fam  modl step tcc  freq freqo core Th   tdp   nm  year  Q  name
	{0x04, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel 486 DX"), CSTR_NULL},
	{0x04, 0x01,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel 486 DX"), CSTR_NULL},
	{0x04, 0x02,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel 486 SX"), CSTR_NULL},
	{0x04, 0x03,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel 487"), CSTR_NULL},
	{0x04, 0x03,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel DX2"), CSTR_NULL},
	{0x04, 0x03,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel DX2 Overdrive"), CSTR_NULL},
	{0x04, 0x04,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel 486 SL"), CSTR_NULL},
	{0x04, 0x05,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel SX2"), CSTR_NULL},
	{0x04, 0x07,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("White-Back Enhanced Intel DX2"), CSTR_NULL},
	{0x04, 0x08,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel DX4"), CSTR_NULL},
	{0x05, 0x01,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium 60/66"), CSTR_NULL},
	{0x05, 0x02,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium"), CSTR_NULL},
	{0x05, 0x04,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium with MMX"), CSTR_NULL},
	{0x06, 0x01,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium Pro"), CSTR_NULL},
	{0x06, 0x03,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium II model 03"), CSTR_NULL},
	{0x06, 0x05,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium II model 05"), CSTR_NULL},
	{0x06, 0x06,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Celeron model 06"), CSTR_NULL},
	{0x06, 0x07,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium III model 07"), CSTR_NULL},
	{0x06, 0x08,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium III model 08"), CSTR_NULL},
	{0x06, 0x09,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium M"), CSTR_NULL},
	{0x06, 0x0A,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium III Xeon model 0Ah"), CSTR_NULL},
	{0x06, 0x0B,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium III model 0Bh"), CSTR_NULL},
	{0x06, 0x0D,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium M model 0Dh / 90nm"), CSTR_NULL},
	{0x06, 0x0E,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Core Duo/Core Solo 65nm"), CSTR_NULL},
	{0x06, 0x0F,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Core 2"), CSTR_NULL},
	{0x06, 0x0F, 11, 100, 2400, 2400,  2,  2,   35,  65, 2007, 2, CSTR("Intel Core2 Duo T7700"), CSTR("Intel T7700")},
	{0x06, 0x0F, 13, 100, 1800, 1800,  2,  2,   35,  65, 2007, 2, CSTR("Intel Core2 Duo T7100"), CSTR("Intel T7100")},
	{0x06, 0x15,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Intel EP80579"), CSTR_NULL},
	{0x06, 0x16,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Celeron model 16h"), CSTR_NULL},
	{0x06, 0x17,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Core 2 Extreme"), CSTR_NULL},
	{0x06, 0x1C,  2,  90, 1600, 1600,  1,  2,  2.5,  45, 2008, 2, CSTR("Mobile Intel Atom N270"), CSTR("Intel N270")},
	{0x06, 0x1C,  2,  90, 1600, 1600,  2,  2,    8,  45, 2008, 3, CSTR("Mobile Intel Atom 330"), CSTR("Intel 330")},
	{0x06, 0x3A,  9,  65, 3300, 3300,  2,  3,   55,  22, 2012, 3, CSTR("Intel Core i3-3220"), CSTR("Intel i3-3220")},
	{0x06, 0x3A,  9, 105, 1800, 1800,  2,  2,   17,  22, 2013, 1, CSTR("Intel Celeron 1037U"), CSTR("Intel 1037U")},
	{0x06, 0x3A,  9, 105, 3300, 3700,  4,  4,   77,  22, 2012, 2, CSTR("Intel Core i5-3550"), CSTR("Intel i5-3550")},
	{0x06, 0x3A,  9, 105, 2000, 2000,  2,  4,   17,  22, 2013, 1, CSTR("Intel Core i7-3537U"), CSTR("Intel i7-3537U")},
	{0x06, 0x3C,  3, 100, 3200, 3400,  4,  8,   84,  22, 2014, 2, CSTR("Intel Core i5-4460"), CSTR("Intel i5-4460")},
	{0x06, 0x3C,  3, 100, 3500, 3900,  4,  8,   84,  22, 2013, 3, CSTR("Intel Core i7-4771"), CSTR("Intel i7-4771")},
	{0x06, 0x3E,  4,  75, 2600, 3400,  8, 16,   95,  22, 2013, 3, CSTR("Intel Xeon E5-2650 v2"), CSTR("Intel E5-2650V2")},
	{0x06, 0x3F,  2,  85, 2500, 3300, 12, 24,  120,  22, 2013, 3, CSTR("Intel Xeon E5-2680 v3"), CSTR("Intel E5-2680V3")},
	{0x06, 0x4C,  3,  90, 1440, 1840,  4,  4,    2,  14, 2015, 2, CSTR("Intel Atom x5-Z8300"), CSTR("Intel x5-Z8300")},
	{0x06, 0x4C,  4,  90, 1440, 1920,  4,  4,    2,  14, 2016, 1, CSTR("Intel Atom x5-Z8350"), CSTR("Intel x5-Z8300")},
	{0x06, 0x4C,  4,  90, 1600, 2240,  4,  4,    6,  14, 2016, 1, CSTR("Intel Celeron N3160"), CSTR("Intel N3160")},
	{0x06, 0x55,  4,   0, 2500, 2500, 24, 48,    0,  14, 2017, 1, CSTR("Intel Xeon 8163"), CSTR("Intel 8163")},
	{0x06, 0x7A,  1, 105, 1100, 2600,  2,  2,    6,  14, 2017, 4, CSTR("Intel Celeron N4000"), CSTR("Intel N4000")},
	{0x06, 0x7A,  8, 105, 1100, 2600,  4,  4,    6,  14, 2019, 4, CSTR("Intel Celeron N4120"), CSTR("Intel N4120")},
	{0x06, 0x8C,  1, 100, 2400, 4200,  4,  8,   28,  10, 2020, 3, CSTR("Intel Core i5-1135G7"), CSTR("Intel i5-1135G7")},
	{0x06, 0x8E, 12, 100, 1600, 3900,  4,  8,   15,  14, 2018, 3, CSTR("Intel Core i5-8265U"), CSTR("Intel i5-8265U")},
	{0x06, 0x8E, 12, 100, 1600, 4200,  4,  8,   15,  14, 2019, 3, CSTR("Intel Core i5-10210U"), CSTR("Intel i5-10210U")},
	{0x06, 0x9E,  9,  80, 2700, 3300,  4,  4,   35,  14, 2017, 1, CSTR("Intel Core i5-7500T"), CSTR("Intel i5-7500T")},
	{0x06, 0x9E, 10, 100, 1600, 3400,  4,  8,   15,  14, 2017, 3, CSTR("Intel Core i5-8250U"), CSTR("Intel i5-8250U")},
	{0x0F, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Pentium 4 model 0"), CSTR_NULL}
};

Manage::CPUDB::CPUSpecX86 Manage::CPUDB::amdCPU[] = {
	//fam  modl step tcc  freq freqo core Th   tdp   nm  year  Q  name
	{0x04, 0x00,  0,   0,   33,   33,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DXLV-33"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   40,   40,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DX-40"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   50,   50,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DX2-50"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   50,   50,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486SX2-50N"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DX2-66"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DX2-66N"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    2,   0, 1970, 0, CSTR("AMD A80486DX2-66NV8T"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    2,   0, 1970, 0, CSTR("AMD A80486DX2-66SV8B"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DXL2-66"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486SX2-66"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   75,   75,  1,  1,    2,   0, 1970, 0, CSTR("AMD A80486DX4-75SV8B"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   75,   75,  1,  1,    2,   0, 1970, 0, CSTR("AMD S80486DX4-75SV8B"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   80,   80,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DX2-80"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   80,   80,  1,  1,    2,   0, 1970, 0, CSTR("AMD A80486DX2-80NV8T"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   80,   80,  1,  1,    2, 500, 1970, 0, CSTR("AMD A80486DX2-80SV8B"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   80,   80,  1,  1,    2,   0, 1970, 0, CSTR("AMD A80486DX2-80V8T"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   80,   80,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DXL2-80/3V"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   90,   90,  1,  1,    0,   0, 1970, 0, CSTR("AMD A80486DX4-90NV8T"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    3, 500, 1970, 0, CSTR("AMD A80486DX4-100NV8T"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    3, 500, 1970, 0, CSTR("AMD A80486DX4-100SV8B"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    3, 500, 1970, 0, CSTR("AMD S80486DX4-100SV8B"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    3, 500, 1970, 0, CSTR("AMD A80486DX4-100V8T"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  120,  120,  1,  1,    4, 500, 1970, 0, CSTR("AMD A80486DX4-120NV8T"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  120,  120,  1,  1,    4, 500, 1970, 0, CSTR("AMD A80486DX4-120SV8B"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    2,   0, 1970, 0, CSTR("AMD Am486DE2-66V8THC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    2,   0, 1970, 0, CSTR("AMD Am486DE2-66V8TGC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    1, 350, 1970, 0, CSTR("AMD Am486DX2-66V16BGC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    1, 350, 1970, 0, CSTR("AMD Am486DX2-66V16BGI"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    1, 350, 1970, 0, CSTR("AMD Am486DX2-66V16BHC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,   66,   66,  1,  1,    1, 350, 1970, 0, CSTR("AMD Am486DX2-66V16BHI"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    2, 350, 1970, 0, CSTR("AMD Am486DX4-100V16BGC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    2, 350, 1970, 0, CSTR("AMD Am486DX4-100V16BGI"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    2, 350, 1970, 0, CSTR("AMD Am486DX4-100V16BHC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  100,  100,  1,  1,    2, 350, 1970, 0, CSTR("AMD Am486DX4-100V16BHI"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  133,  133,  1,  1,    3, 350, 1970, 0, CSTR("AMD Am486DX5-133V16BGC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  133,  133,  1,  1,    3, 350, 1970, 0, CSTR("AMD Am486DX5-133V16BHC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  133,  133,  1,  1,    3, 350, 1970, 0, CSTR("AMD Am486DX5-133W16BGC"), CSTR_NULL},
	{0x04, 0x00,  0,   0,  133,  133,  1,  1,    3, 350, 1970, 0, CSTR("AMD Am486DX5-133W16BHC"), CSTR_NULL},
	{0x04, 0x0E,  0,   0,  133,  133,  1,  1,    0, 350, 1995, 4, CSTR("AMD AMD-X5-133ADH"), CSTR_NULL},
	{0x04, 0x0E,  0,   0,  133,  133,  1,  1,    0, 350, 1995, 4, CSTR("AMD AMD-X5-133ADW"), CSTR_NULL},
	{0x04, 0x0E,  0,   0,  133,  133,  1,  1,    0, 350, 1970, 0, CSTR("AMD AMD-X5-133ADY"), CSTR_NULL},
	{0x04, 0x0E,  0,   0,  133,  133,  1,  1,    0, 350, 1995, 4, CSTR("AMD AMD-X5-133ADZ"), CSTR_NULL},
	{0x04, 0x0E,  0,   0,  133,  133,  1,  1,    0, 350, 1995, 4, CSTR("AMD AMD-X5-133SDZ"), CSTR_NULL},
	{0x04, 0x0E,  0,   0,  133,  133,  1,  1,    0, 350, 1995, 4, CSTR("AMD AMD-X5-133SFZ"), CSTR_NULL},
	{0x0F, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("K8 Hammer"), CSTR_NULL},
	{0x0F, 0x6B,  2,  95, 1600, 1600,  2,  2,   18,  65, 2009, 3, CSTR("AMD Turion Neo X2 L625"), CSTR("TMZL625OAX5DY")},
	{0x0F, 0x6B,  2,   0, 3100, 3100,  2,  2,   89,  65, 1970, 0, CSTR("AMD Athlon 64 X2 6000+"), CSTR("ADV6000IAA5DO")},
	{0x10, 0x06,  2,   0, 3000, 3000,  2,  2,    0,   0, 1970, 0, CSTR("AMD Athlon II X2 250"), CSTR_NULL},
	{0x10, 0x02,  3,   0, 2300, 2300,  4,  4,    0,  65, 1970, 0, CSTR("AMD Phenom X4 9650"), CSTR("HD9650WCGHBOX")},
	{0x12, 0x01,  0,   0, 2400, 2400,  2,  2,   65,  32, 2011, 4, CSTR("AMD E2-3200"), CSTR("ED3200OJZ22GX")},
	{0x12, 0x01,  0,   0, 2500, 2500,  2,  2,   65,  32, 2011, 3, CSTR("AMD A4-3300"), CSTR("AD33000OJZ22HX")},
	{0x12, 0x01,  0,   0, 2700, 2700,  2,  2,   65,  32, 2011, 3, CSTR("AMD A4-3400"), CSTR("AD3400OJZ22GX")},
	{0x12, 0x01,  0,   0, 2800, 2800,  2,  2,   65,  32, 2011, 4, CSTR("AMD A4-3420"), CSTR("AD3420OJZ22HX")},
	{0x12, 0x01,  0,   0, 2100, 2400,  3,  3,   65,  32, 2011, 3, CSTR("AMD A6-3500"), CSTR("AD3500OJZ33GX")},
	{0x12, 0x01,  0,   0, 2100, 2400,  4,  4,   65,  32, 2011, 3, CSTR("AMD A6-3600"), CSTR("AD3600OJZ43GX")},
	{0x12, 0x01,  0,   0, 2200, 2500,  4,  4,   65,  32, 2011, 4, CSTR("AMD A6-3620"), CSTR("AD3620OJZ43GX")},
	{0x12, 0x01,  0,   0, 2600, 2600,  4,  4,  100,  32, 2011, 3, CSTR("AMD A6-3650"), CSTR("AD3650WNZ43GX")},
	{0x12, 0x01,  0,   0, 2700, 2700,  4,  4,  100,  32, 2011, 4, CSTR("AMD A6-3670K"), CSTR("AD3670WNZ43GX")},
	{0x12, 0x01,  0,   0, 2400, 2700,  4,  4,   65,  32, 2011, 3, CSTR("AMD A8-3800"), CSTR("AD3800OJZ43GX")},
	{0x12, 0x01,  0,   0, 2500, 2800,  4,  4,   65,  32, 2011, 4, CSTR("AMD A8-3820"), CSTR("AD3820OJZ43GX")},
	{0x12, 0x01,  0,   0, 2900, 2900,  4,  4,  100,  32, 2011, 2, CSTR("AMD A8-3850"), CSTR("AD3850WNZ43GX")},
	{0x12, 0x01,  0,   0, 3000, 3000,  4,  4,  100,  32, 2011, 4, CSTR("AMD A8-3870K"), CSTR("AD3870WNZ43GX")},

	{0x14, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Bobcat"), CSTR_NULL},
	{0x15, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Bulldozer"), CSTR_NULL},
	{0x15, 0x65,  1,   0, 3000, 3500,  4,  4,   35,  28, 2016, 2, CSTR("AMD A10-9700E"), CSTR("AD9700AHM44AB")},
	{0x16, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Jaguar"), CSTR_NULL},

	//fam  modl step tcc  freq freqo core Th   tdp   nm  year  Q  name
	{0x16, 0x00,  0,   0, 2500, 2500,  2,  2,   65,  32, 2012, 1, CSTR("AMD Sempron X2 198"), CSTR("SD198XOJZ22GX")},
	{0x16, 0x00,  0,   0, 2800, 2800,  2,  2,   65,  32, 2012, 1, CSTR("AMD Athlon II X2 221"), CSTR("AD221XOJZ22GX")},
	{0x16, 0x00,  0,   0, 2600, 2600,  4,  4,  100,  32, 2011, 3, CSTR("AMD Athlon II X4 631"), CSTR("AD631XOJZ43GX")},
	{0x16, 0x00,  0,   0, 2700, 2700,  4,  4,   65,  32, 2012, 1, CSTR("AMD Athlon II X4 638"), CSTR("AD638XOJZ43GX")},
	{0x16, 0x00,  0,   0, 2800, 2800,  4,  4,  100,  32, 2012, 1, CSTR("AMD Athlon II X4 641"), CSTR("AD641XWNZ43GX")},
	{0x16, 0x00,  0,   0, 3000, 3000,  4,  4,  100,  32, 2011, 4, CSTR("AMD Athlon II X4 651"), CSTR("AD651XWNZ43GX")},
	{0x16, 0x00,  0,   0, 3000, 3000,  4,  4,  100,  32, 2012, 1, CSTR("AMD Athlon II X4 651K"), CSTR("AD651KWNZ43GX")},

	{0x16, 0x00,  0,   0, 2900, 3300,  2,  2,   65,  32, 1970, 0, CSTR("AMD Sempron X2 240"), CSTR("SD240XOKA23HJ")},
	{0x16, 0x00,  0,   0, 3200, 3600,  2,  2,   65,  32, 2012, 4, CSTR("AMD Athlon X2 340"), CSTR("AD340XOKA23HJ")},
	{0x16, 0x00,  0,   0, 3200, 3700,  4,  4,   65,  32, 2012, 4, CSTR("AMD Athlon X4 740"), CSTR("AD740XOKA44HJ")},
	{0x16, 0x00,  0,   0, 3400, 4000,  4,  4,  100,  32, 2012, 4, CSTR("AMD Athlon X4 750K"), CSTR("AD750KWOA44HJ")},
	{0x16, 0x00,  0,   0, 3400, 4000,  4,  4,   65,  32, 2012, 3, CSTR("AMD FirePro A300"), CSTR("AWA300OKA44HJ")},
	{0x16, 0x00,  0,   0, 3800, 4200,  4,  4,  100,  32, 2012, 3, CSTR("AMD FirePro A320"), CSTR("AWA320WOA44HJ")},
	{0x16, 0x00,  0,   0, 3400, 3700,  2,  2,   65,  32, 2012, 4, CSTR("AMD A4-5300"), CSTR("AD5300OKA23HJ")},
	{0x16, 0x00,  0,   0, 3400, 3700,  2,  2,   65,  32, 1970, 0, CSTR("AMD A4-5300B"), CSTR("AD530BOKA23HJ")},
	{0x16, 0x00,  0,   0, 3600, 3800,  2,  2,   65,  32, 2012, 4, CSTR("AMD A6-5400K"), CSTR("AD540KOKA23HJ")},
	{0x16, 0x00,  0,   0, 3600, 3800,  2,  2,   65,  32, 1970, 0, CSTR("AMD A6-5400B"), CSTR("AD540BOKA23HJ")},
	{0x16, 0x00,  0,   0, 3200, 3700,  4,  4,   65,  32, 2012, 4, CSTR("AMD A8-5500"), CSTR("AD5500OKA44HJ")},
	{0x16, 0x00,  0,   0, 3200, 3700,  4,  4,   65,  32, 1970, 0, CSTR("AMD A8-5500B"), CSTR("AD550BOKA44HJ")},
	{0x16, 0x00,  0,   0, 3600, 3900,  4,  4,  100,  32, 2012, 4, CSTR("AMD A8-5600K"), CSTR("AD560KWOA44HJ")},
	{0x16, 0x00,  0,   0, 3400, 4000,  4,  4,   65,  32, 2012, 4, CSTR("AMD A10-5700"), CSTR("AD5700OKA44HJ")},
	{0x16, 0x00,  0,   0, 3800, 4200,  4,  4,  100,  32, 2012, 4, CSTR("AMD A10-5800K"), CSTR("AD580KWOA44HJ")},
	{0x16, 0x00,  0,   0, 3800, 4200,  4,  4,  100,  32, 1970, 0, CSTR("AMD A10-5800B"), CSTR("AD580BWOA44HJ")},

	{0x16, 0x00,  0,   0, 3000, 3700,  4,  4,   45,  28, 2016, 2, CSTR("AMD FX 9830P"), CSTR("FM983PAEY44AB")},
	{0x16, 0x00,  0,   0, 2700, 3600,  4,  4,   15,  28, 1970, 0, CSTR("AMD FX 9800P"), CSTR("FM980PADY44AB")},
	{0x16, 0x00,  0,   0, 2800, 3500,  4,  4,   45,  28, 1970, 0, CSTR("AMD A12-9730P"), CSTR("AM973PAEY44AB")},
	{0x16, 0x00,  0,   0, 2500, 3400,  4,  4,   15,  28, 1970, 0, CSTR("AMD A12-9700P"), CSTR("AM970PADY44AB")},
	{0x16, 0x00,  0,   0, 2600, 3300,  4,  4,   45,  28, 1970, 0, CSTR("AMD A10-9630P"), CSTR("AM963PAEY44AB")},
	{0x16, 0x00,  0,   0, 2400, 3300,  4,  4,   15,  28, 1970, 0, CSTR("AMD A10-9600P"), CSTR("AM960PADY44AB")},
	{0x16, 0x00,  0,   0, 2900, 3500,  2,  2,   25,   0, 1970, 0, CSTR("AMD A9-9410"), CSTR_NULL},
	{0x16, 0x00,  0,   0, 2400, 2800,  2,  2,   15,   0, 1970, 0, CSTR("AMD A6-9210"), CSTR_NULL},
	{0x16, 0x00,  0,   0, 2000, 2200,  2,  2,   15,   0, 1970, 0, CSTR("AMD E2-9010"), CSTR_NULL},


	{0x17, 0x00,  0,   0,    0,    0,  0,  0,    0,   0, 1970, 0, CSTR("Zen"), CSTR_NULL}
};

Manage::CPUDB::CPUSpec Manage::CPUDB::cpuList[] = {
	//                                                                                                                                  [             clustor 1                ] [               clustor2               ] [                 clustor3             ]
	//model              contextType                       brand                            name                             tdp   nm   Type            cnt freq  max   L1I  L1D Type            cnt freq  max   L1I  L1D Type            cnt freq  max   L1I  L1D L2    L3     GPU
	{CSTR("88F3710"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MARVELL,   "Armada 3710 88F3710",           0,    0,   "Cortex-A53",   1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F3720"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MARVELL,   "Armada 3720 88F3710",           0,    0,   "Cortex-A53",   1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F5082"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Orion 88F5082",                 0,    0,   "88fr331",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F5181"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Orion 88F5181",                 0,    0,   "88fr331",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F5181L"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Orion 88F5181L",                0,    0,   "88fr331",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F5182"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Orion 88F5182",                 0,    0,   "88fr331",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F5281"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Orion 88F5281",                 0,    0,   "88fr531-vd",   1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6180"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Kirkwood 88F6180",              0,    0,   "88fr131",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6182"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Kirkwood 88F6182",              0,    0,   "88fr131",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6183"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Orion 88F6183",                 0,    0,   "88fr331",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6190"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Kirkwood 88F6190",              0,    0,   "88fr131",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6192"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Kirkwood 88F6192",              0,    0,   "88fr131",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6281"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Kirkwood 88F6281",              0,    0,   "88fr131",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6282"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Kirkwood 88F6282/ Armada 300",  0,    0,   "88fr131",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6283"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Kirkwood 88F6283/ Armada 310",  0,    0,   "88fr131",      1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6510"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Avanta 88F6510",                0,    0,   "ARMv5",        1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6530P"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Avanta 88F6530P",               0,    0,   "ARMv5",        1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6550"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Avanta 88F6550",                0,    0,   "ARMv5",        1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6560"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Avanta 88F6560",                0,    0,   "ARMv5",        1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6707"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 370 88F6707",            0,    0,   "Sheeva ARMv7", 1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6710"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 370 88F6710",            0,    0,   "Sheeva ARMv7", 1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6720"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 375 88F6720",            0,    0,   "Cortex-A9",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6810"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 380 88F6810",            0,    0,   "Cortex-A9",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6820"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 385 88F6820",            0,    0,   "Cortex-A9",    2,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6828"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 388 88F6828",            0,    0,   "Cortex-A9",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6920"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 390 88F6920",            0,    0,   "Cortex-A9",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6928"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 398 88F6928",            0,    0,   "Cortex-A9",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F6W11"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada 370 88F6W11",            0,    0,   "Sheeva ARMv7", 1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F7020"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MARVELL,   "Armada 7K 88F7020",             0,    0,   "Cortex-A72",   2,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F7040"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MARVELL,   "Armada 7K 88F7040",             0,    0,   "Cortex-A72",   4,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F8020"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MARVELL,   "Armada 8K 88F7020",             0,    0,   "Cortex-A72",   2,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("88F8040"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MARVELL,   "Armada 8K 88F7040",             0,    0,   "Cortex-A72",   4,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("AD9700AHM44AB"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_AMD,       "A10-9700E",                     35,   28,  "x86",          4,  3000, 3500, 96,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Radeon R7"},
	{CSTR("AL212"),            Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ANNAPURNA, "Alpine AL-212",                 0,    0,   "Cortex-A15",   2,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("AL214"),            Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ANNAPURNA, "Alpine AL-214",                 0,    0,   "Cortex-A15",   2,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("AL314"),            Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ANNAPURNA, "Alpine AL-314",                 0,    0,   "Cortex-A15",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("AM3351"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "Sitara AM3351",                 0,    0,   "Cortex-A8",    1,  300,  600,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("AM3352"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "Sitara AM3352",                 0,    0,   "Cortex-A8",    1,  300,  1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("AM3354"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "Sitara AM3354",                 0,    0,   "Cortex-A8",    1,  600,  1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "SGX530"},
	{CSTR("AM3356"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "Sitara AM3356",                 0,    0,   "Cortex-A8",    1,  300,  800,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("AM3357"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "Sitara AM3357",                 0,    0,   "Cortex-A8",    1,  300,  800,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("AM3358"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "Sitara AM3358",                 0,    0,   "Cortex-A8",    1,  600,  1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "SGX530"},
	{CSTR("AM3359"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "Sitara AM3359",                 0,    0,   "Cortex-A8",    1,  600,  800,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "SGX530"},
	{CSTR("APQ8016"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 410 APQ8016",        0,    28,  "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 306"},
	{CSTR("APQ8026"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 APQ8026",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("APQ8028"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 APQ8028",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("APQ8030AB"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 APQ8030AB",      0,    28,  "Krait 300",    2,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("APQ8060"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "Snapgragon S4 Plus APQ8060",    0,    0,   "Krait 300" ,   2,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 305"},
	{CSTR("APQ8064"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapgragon S4 Pro APQ8064",     0,    28,  "Krait 300" ,   4,  1500, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 320"},
	{CSTR("APQ8064-1AA"),      Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S4 Pro APQ8064-1AA", 0,    28,  "Krait 300" ,   4,  1500, 1500, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 320"},
	{CSTR("APQ8064AB"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 600 APQ8064AB",      0,    28,  "Krait 300" ,   4,  1900, 1900, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 320"},
	{CSTR("APQ8064M"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 600 APQ8064M",       0,    28,  "Krait 300" ,   4,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 320"},
	{CSTR("APQ8064T"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 600 APQ8064T",       0,    28,  "Krait 300" ,   4,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 320"},
	{CSTR("APQ8074"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "Snapgragon 800 APQ8074",        0,    28,  "Krait 300" ,   4,  2200, 2200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 330"},
	{CSTR("APQ8074-AA"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 800 APQ8074-AA",     0,    28,  "Krait 400" ,   4,  2260, 2260, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("APQ8074-AB"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 801 APQ8074-AB",     0,    28,  "Krait 400",    4,  2360, 2360, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("APQ8084"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 805 APQ8084",        0,    28,  "Krait 400",    4,  2450, 2450, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("AT91SAM"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATMEL,     "AT91SAM",                       0,    0,   "ARM926EJ-S",   1,  200,  200,  4,   4,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("AT91SAM9"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATMEL,     "AT91SAM9",                      0,    0,   "ARM926EJ-S",   1,  200,  200,  4,   4,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("AT91SAM9G45"),      Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATMEL,     "AT91SAM9G45",                   0,    0,   "ARM926EJ-S",   1,  400,  400,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("Amlogic S905"),     Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_AMLOGIC,   "S905",                          0,    0,   "Cortex-A53",   4,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450"},
	{CSTR("Amlogic S905D"),    Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_AMLOGIC,   "S905D",                         0,    0,   "Cortex-A53",   4,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450"},
	{CSTR("Amlogic S905L"),    Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_AMLOGIC,   "S905L",                         0,    0,   "Cortex-A53",   4,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450"},
	{CSTR("Amlogic S905X"),    Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_AMLOGIC,   "S905X",                         0,    0,   "Cortex-A53",   4,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450"},
	{CSTR("Amlogic S912"),     Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_AMLOGIC,   "S912",                          0,    0,   "Cortex-A53",   4,  1500, 1500, 0,   0,  "Cortex-A53",   4,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T820 MP3"},
	{CSTR("Amlogic T962"),     Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_AMLOGIC,   "T962",                          0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450"},
	{CSTR("BCM11130"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM11130",                      0,    0,   "Cortex-A9",    2,  900,  900,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM11140"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM11140",                      0,    0,   "Cortex-A9",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM2763"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM2763",                       0,    0,   "ARM1176JZF-S", 1,  600,  600,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM2835"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM2835",                       0,    0,   "ARM1176JZF-S", 1,  700,  700,  0,   0,  0,              0,  0,    0,    16,  16, 0,              0,  0,    0,    0,   0,  128,  0,     "VideoCore IV"},
	{CSTR("BCM2836"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM2836",                       0,    0,   "Cortex-A7",    2,  900,  900,  0,   0,  0,              0,  0,    0,    32,  32, 0,              0,  0,    0,    0,   0,  512,  0,     "VideoCore IV"},
	{CSTR("BCM2837"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_BROADCOM,  "BCM2837",                       0,    0,   "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    32,  32, 0,              0,  0,    0,    0,   0,  512,  0,     "VideoCore IV"},
	{CSTR("BCM4707"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM4707",                       0,    0,   "Cortex-A9",    2,  800,  800,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4708"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM4708",                       0,    0,   "Cortex-A9",    2,  800,  800,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM47081"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM47081",                      0,    0,   "Cortex-A9",    1,  800,  800,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4708A0"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM4708A0",                     0,    0,   "Cortex-A9",    2,  800,  800,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4708C0"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM4708C0",                     0,    0,   "Cortex-A9",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4709"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM4709",                       0,    0,   "Cortex-A9",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM47094"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM47094",                      0,    0,   "Cortex-A9",    2,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4709A0"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM4709A0",                     0,    0,   "Cortex-A9",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4709C0"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM4709C0",                     0,    0,   "Cortex-A9",    2,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4906"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_BROADCOM,  "BCM4906",                       0,    0,   "Cortex-A53",   2,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM4908"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_BROADCOM,  "BCM4908",                       0,    0,   "Cortex-A53",   4,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM49408"),         Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_BROADCOM,  "BCM49408",                      0,    0,   "Cortex-B53",   4,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM7208"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM7208",                       0,    0,   "ARM1176JZF-S", 1,  400,  400,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM7218"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_BROADCOM,  "BCM7218",                       0,    0,   "ARM1176JZF-S", 1,  600,  600,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM7413"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_BROADCOM,  "BCM7413",                       0,    0,   "16e-class",    2,  400,  400,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("BCM7615"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_BROADCOM,  "BCM7615",                       0,    0,   "24Kc",         1,  500,  500,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("Exynos5433"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7 Octa 5433",            0,    20,  "Cortex-A57",   4,  1900, 1900, 0,   0,  "Cortex-A53",   4,  1300, 1300, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T760 MP6"},
	{CSTR("Exynos7420"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7 Octa 7420",            0,    14,  "Cortex-A57",   4,  2100, 2100, 0,   0,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T760 MP8"},
	{CSTR("Exynos7570"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7 Quad 7570",            0,    14,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T720 MP1"},
	{CSTR("Exynos7870"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7 Octa 7870",            0,    14,  "Cortex-A53",   8,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T830 MP1"},
	{CSTR("Exynos7880"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7 Octa 7420",            0,    14,  "Cortex-A53",   8,  1900, 1900, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T830 MP3"},
	{CSTR("Exynos7884"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7884",                   0,    14,  "Cortex-A73",   4,  1600, 1600, 0,   0,  "Cortex-A53",   4,  1350, 1350, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G71 MP2"},
	{CSTR("Exynos7885"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7885",                   0,    14,  "Cortex-A73",   4,  2200, 2200, 0,   0,  "Cortex-A53",   4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G71 MP2"},
	{CSTR("Exynos7904"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 7904",                   0,    14,  "Cortex-A73",   4,  1800, 1800, 0,   0,  "Cortex-A53",   4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G71 MP2"},
	{CSTR("Exynos8890"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 8 Octa 8890",            0,    14,  "Exynos M1",    4,  2300, 2600, 0,   0,  "Cortex-A53",   4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T880 MP12"},
	{CSTR("Exynos8895"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 8895",                   0,    10,  "Exynos M2",    4,  2314, 2314, 0,   0,  "Cortex-A53",   4,  1690, 1690, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G71 MP20"},
	{CSTR("Exynos9611"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 9611",                   0,    10,  "Cortex-A73",   4,  2300, 2300, 0,   0,  "Cortex-A53",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G72 MP3"},
	{CSTR("Exynos9810"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_SAMSUNG,   "Exynos 9810",                   0,    10,  "Exynos M3",    4,  2900, 2900, 0,   0,  "Cortex-A55",   4,  1900, 1900, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G72 MP18"},
	{CSTR("GM8125"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_GRAINMEDIA,"GM8125",                        0,    90,  "FA626TE",      1,  533,  533,  16,  8,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("HD9650WCGHBOX"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_AMD,       "Phenom X4 9650",                95,   65,  "x86",          4,  2300, 2300, 64,  64, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 2048,  0},
	{CSTR("IMX6UL"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_NXP,       "Freescale i.MX6 UltraLite",     0,    0,   "Cortex-A7",    1,  528,  696,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  128,  0,     0},
	{CSTR("IPQ4018"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ4018",                       0,    0,   "Cortex-A7",    4,  710,  710,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("IPQ4019"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ4019",                       0,    0,   "Cortex-A7",    4,  717,  717,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("IPQ4028"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ4028",                       0,    0,   "Cortex-A7",    4,  710,  710,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("IPQ4029"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ4029",                       0,    0,   "Cortex-A7",    4,  717,  717,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("IPQ8062"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ8062",                       0,    0,   "Krait 300",    4,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("IPQ8064"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ8064",                       0,    0,   "Krait 300",    4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("IPQ8065"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ8065",                       0,    0,   "Krait 300",    4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("IPQ8066"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ8066",                       0,    0,   "Krait 300",    4,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("IPQ8068"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ8068",                       0,    0,   "Krait 300",    4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("IPQ8069"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "IPQ8069",                       0,    0,   "Krait 300",    4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("IPQ8072"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ATHEROS,   "IPQ8072",                       0,    0,   "Cortex-A53",   4,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("IPQ8074"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ATHEROS,   "IPQ8074",                       0,    0,   "Cortex-A53",   4,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("Intel 1037U"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron 1037U",                 17,   22,  "x86",          2,  1800, 1800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  2048,  "HD Graphics"},
	{CSTR("Intel 230"),        Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom 230",                      4,    45,  "x86",          1,  1600, 1600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     0},
	{CSTR("Intel 330"),        Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom 330",                      8,    45,  "x86",          2,  1600, 1600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     0},
	{CSTR("Intel 8163"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon Platinum 8163",            0,    14,  "x86 with HT",  24, 2500, 2500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 33792, "HD Graphics"},
	{CSTR("Intel D2500"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D2500",                    10,   32,  "x86",          2,  1870, 1870, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3600"},
	{CSTR("Intel D2550"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D2550",                    10,   32,  "x86 with HT",  2,  1870, 1870, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3650"},
	{CSTR("Intel D2560"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D2560",                    10,   32,  "x86 with HT",  2,  2000, 2000, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3650"},
	{CSTR("Intel D2700"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D2700",                    10,   32,  "x86 with HT",  2,  2133, 2133, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3650"},
	{CSTR("Intel D2701"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D2701",                    10,   32,  "x86 with HT",  2,  2133, 2133, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3650"},
	{CSTR("Intel D410"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D410",                     10,   45,  "x86",          1,  1670, 1670, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3150"},
	{CSTR("Intel D425"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D425",                     10,   45,  "x86",          1,  1830, 1830, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3150"},
	{CSTR("Intel D510"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D510",                     13,   45,  "x86",          2,  1670, 1670, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3150"},
	{CSTR("Intel D525"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom D525",                     13,   45,  "x86",          2,  1830, 1830, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3150"},
	{CSTR("Intel E5-1428LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-1428L v2",              60,   22,  "x86 with HT",  6,  2200, 2700, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-1620V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-1620 v2",               130,  22,  "x86 with HT",  4,  3700, 3900, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 10240, 0},
	{CSTR("Intel E5-1650V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-1650 v2",               130,  22,  "x86 with HT",  6,  3500, 3900, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 12288, 0},
	{CSTR("Intel E5-1660V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-1660 v2",               130,  22,  "x86 with HT",  6,  3700, 4000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2403V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2403 v2",               80,   22,  "x86",          4,  1800, 1800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 10240, 0},
	{CSTR("Intel E5-2407V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2407 v2",               80,   22,  "x86",          4,  2400, 2400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 10240, 0},
	{CSTR("Intel E5-2418LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2418L v2",              50,   22,  "x86 with HT",  6,  2000, 2000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2420V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2420 v2",               80,   22,  "x86 with HT",  6,  2200, 2700, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2428LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2428L v2",              60,   22,  "x86 with HT",  8,  1800, 2300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-2430LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2430L v2",              60,   22,  "x86 with HT",  6,  2400, 2800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2430V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2430 v2",               80,   22,  "x86 with HT",  6,  2500, 3000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2440V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2440 v2",               95,   22,  "x86 with HT",  8,  1900, 2400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-2448LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2448L v2",              70,   22,  "x86 with HT",  10, 1800, 2400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2450LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2450L v2",              60,   22,  "x86 with HT",  10, 1700, 2100, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2450V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2450 v2",               95,   22,  "x86 with HT",  8,  2500, 3300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-2470V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2470 v2",               95,   22,  "x86 with HT",  10, 2400, 3200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2603V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2603 v2",               80,   22,  "x86",          4,  1800, 1800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 10240, 0},
	{CSTR("Intel E5-2609V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2609 v2",               80,   22,  "x86",          4,  2500, 2500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 10240, 0},
	{CSTR("Intel E5-2618LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2618L v2",              50,   22,  "x86 with HT",  6,  2000, 2000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2620V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2620 v2",               80,   22,  "x86 with HT",  6,  2100, 2600, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2628LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2628L v2",              70,   22,  "x86 with HT",  8,  1900, 2400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-2630LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2630L v2",              60,   22,  "x86 with HT",  6,  2400, 2800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2630V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2630 v2",               80,   22,  "x86 with HT",  6,  2600, 3100, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2637V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2637 v2",               130,  22,  "x86 with HT",  4,  3500, 3800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-2640V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2640 v2",               95,   22,  "x86 with HT",  8,  2000, 2500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-2643V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2643 v2",               130,  22,  "x86 with HT",  6,  3500, 3800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2648LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2648L v2",              70,   22,  "x86 with HT",  10, 1900, 2500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2650LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2650L v2",              70,   22,  "x86 with HT",  10, 1700, 2100, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2650V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2650 v2",               95,   22,  "x86 with HT",  8,  2600, 3400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-2658V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2658 v2",               95,   22,  "x86 with HT",  10, 2400, 3000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2660V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2660 v2",               95,   22,  "x86 with HT",  10, 2200, 3000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2667V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2667 v2",               130,  22,  "x86 with HT",  8,  3300, 4000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2670V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2670 v2",               115,  22,  "x86 with HT",  10, 2500, 3300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2680V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2680 v2",               115,  22,  "x86 with HT",  10, 2800, 3600, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2680V3"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2680 v3",               120,  22,  "x86 with HT",  12, 2500, 3300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 30720, 0},
	{CSTR("Intel E5-2687WV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2687W v2",              150,  22,  "x86 with HT",  8,  3400, 4000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2690V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2690 v2",               130,  22,  "x86 with HT",  10, 3000, 3600, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-2695V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2695 v2",               115,  22,  "x86 with HT",  12, 2400, 3200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 30720, 0},
	{CSTR("Intel E5-2697V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-2697 v2",               130,  22,  "x86 with HT",  12, 2700, 3500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 30720, 0},
	{CSTR("Intel E5-4603V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4603 v2",               95,   22,  "x86 with HT",  4,  2200, 2200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 10240, 0},
	{CSTR("Intel E5-4607V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4607 v2",               95,   22,  "x86 with HT",  6,  2600, 2600, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 15360, 0},
	{CSTR("Intel E5-4610V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4610 v2",               95,   22,  "x86 with HT",  8,  2300, 2700, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 16384, 0},
	{CSTR("Intel E5-4620V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4620 v2",               95,   22,  "x86 with HT",  6,  2600, 3000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-4624LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4624L v2",              70,   22,  "x86 with HT",  10, 1900, 2500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-4627V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4627 v2",               130,  22,  "x86",          8,  3300, 3600, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 16384, 0},
	{CSTR("Intel E5-4640V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4640 v2",               95,   22,  "x86 with HT",  10, 2200, 2700, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 20480, 0},
	{CSTR("Intel E5-4650V2"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4650 v2",               95,   22,  "x86 with HT",  10, 2400, 2900, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 25600, 0},
	{CSTR("Intel E5-4657LV2"), Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Xeon E5-4657L v2",              115,  22,  "x86 with HT",  12, 2400, 2900, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 30720, 0},
	{CSTR("Intel N2100"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N2100",                    0,    32,  "x86 with HT",  1,  1600, 1600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3600"},
	{CSTR("Intel N2600"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N2600",                    3.5,  32,  "x86 with HT",  2,  1600, 1600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3650"},
	{CSTR("Intel N270"),       Manage::ThreadContext::CT_X86_32, Manage::CPUVendor::CB_INTEL,     "Atom N270",                     2.5,  45,  "x86 with HT",  1,  1600, 1600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "HD Graphics"},
	{CSTR("Intel N280"),       Manage::ThreadContext::CT_X86_32, Manage::CPUVendor::CB_INTEL,     "Atom N280",                     2.5,  45,  "x86 with HT",  1,  1660, 1660, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "HD Graphics"},
	{CSTR("Intel N2800"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N2800",                    6.5,  32,  "x86 with HT",  2,  1860, 1860, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3650"},
	{CSTR("Intel N2805"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2805",                 4.3,  22,  "x86",          2,  1460, 1460, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2806"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2806",                 4.5,  22,  "x86",          2,  1600, 2000, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2807"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2807",                 4.3,  22,  "x86",          2,  1580, 2160, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2808"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2808",                 4.5,  22,  "x86",          2,  1580, 2250, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2810"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2810",                 7.5,  22,  "x86",          2,  2000, 2000, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2815"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2815",                 7.5,  22,  "x86",          2,  1860, 2130, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2820"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2820",                 7.5,  22,  "x86",          2,  2130, 2390, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2830"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2830",                 7.5,  22,  "x86",          2,  2160, 2410, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2840"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2840",                 7.5,  22,  "x86",          2,  2160, 2580, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "HD Graphics"},
	{CSTR("Intel N2910"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2910",                 7.5,  22,  "x86",          4,  1600, 1600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N2920"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2920",                 7.5,  22,  "x86",          4,  1860, 2000, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N2930"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2930",                 7.5,  22,  "x86",          4,  1830, 2160, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N2940"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N2940",                 7.5,  22,  "x86",          4,  1830, 2250, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N3000"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3000",                 4,    14,  "x86",          2,  1040, 2080, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N3010"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3010",                 4,    14,  "x86",          2,  1040, 2240, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N3050"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3050",                 6,    14,  "x86",          2,  1600, 2160, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N3060"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3060",                 6,    14,  "x86",          2,  1600, 2480, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N3150"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3150",                 6,    14,  "x86",          4,  1600, 2080, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N3160"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3160",                 6,    14,  "x86",          4,  1600, 2240, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics"},
	{CSTR("Intel N3350"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3350",                 6,    14,  "x86",          2,  1100, 2400, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics 500"},
	{CSTR("Intel N3450"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N3450",                 6,    14,  "x86",          4,  1100, 2200, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "HD Graphics 500"},
	{CSTR("Intel N4000"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N4000",                 6,    14,  "x86",          2,  1100, 2600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  4096, 0,     "UHD Graphics 600"},
	{CSTR("Intel N4100"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N4100",                 6,    14,  "x86",          4,  1100, 2400, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  4096, 0,     "UHD Graphics 600"},
	{CSTR("Intel N4120"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Celeron N4120",                 6,    14,  "x86",          4,  1100, 2600, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  4096, 0,     "UHD Graphics 600"},
	{CSTR("Intel N435"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N435",                     5,    45,  "x86 with HT",  1,  1330, 1330, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3150"},
	{CSTR("Intel N450"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N450",                     5.5,  45,  "x86 with HT",  1,  1660, 1660, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3150"},
	{CSTR("Intel N455"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N455",                     6.5,  45,  "x86 with HT",  1,  1660, 1660, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3150"},
	{CSTR("Intel N470"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N470",                     6.5,  45,  "x86 with HT",  1,  1830, 1830, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3150"},
	{CSTR("Intel N475"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N475",                     6.5,  45,  "x86 with HT",  1,  1830, 1830, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "GMA 3150"},
	{CSTR("Intel N550"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N550",                     8.5,  45,  "x86 with HT",  2,  1500, 1500, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3150"},
	{CSTR("Intel N570"),       Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom N570",                     8.5,  45,  "x86 with HT",  2,  1660, 1660, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "GMA 3150"},
	{CSTR("Intel T7100"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core 2 Duo T7100",              34,   65,  "x86",          2,  1800, 1800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     0},
	{CSTR("Intel T7700"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core 2 Duo T7100",              35,   65,  "x86",          2,  2400, 2400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  4096, 0,     0},
	{CSTR("Intel Z3580"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom Z3580",                    0,    22,  "x86",          4,  2330, 2330, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "PowerVR G6430"},
	{CSTR("Intel Z8300"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom x5-Z8300",                 2,    14,  "x86",          4,  1440, 1840, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "UHD Graphics 600"},
	{CSTR("Intel Z8350"),      Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Atom x5-Z8350",                 2,    14,  "x86",          4,  1440, 1920, 32,  24, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "UHD Graphics 600"},
	{CSTR("Intel i3-3220"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i3-3220",                  55,   22,  "x86 with HT",  2,  3300, 3300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  3072,  "HD Graphics 2500"},
	{CSTR("Intel i5-10210U"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i5-10210U",                15,   14,  "x86 with HT",  4,  1600, 4200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 6144,  "HD Graphics 620"},
	{CSTR("Intel i5-1135G7"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i5-1135G7",                28,   10,  "x86 with HT",  4,  2400, 4200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  5120, 8192,  "Iris Xe G7"},
	{CSTR("Intel i5-3550"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i5-3550",                  77,   22,  "x86",          4,  3300, 3700, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  6144,  "HD Graphics 2500"},
	{CSTR("Intel i5-4460"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i5-4460",                  84,   22,  "x86",          4,  3200, 3400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  6144,  "HD Graphics 4600"},
	{CSTR("Intel i5-7500T"),   Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i5-7500T",                 35,   14,  "x86",          4,  2700, 3300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 6144,  "HD Graphics 4600"},
	{CSTR("Intel i5-8250U"),   Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i5-8250U",                 15,   14,  "x86 with HT",  4,  1600, 3400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 6144,  "HD Graphics 620"},
	{CSTR("Intel i5-8265U"),   Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i5-8265U",                 15,   14,  "x86 with HT",  4,  1600, 3900, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 6144,  "HD Graphics 620"},
	{CSTR("Intel i7-1165G7"),  Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i7-1165G7",                28,   10,  "x86 with HT",  4,  2800, 4700, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  5120, 12288, "Iris Xe G7"},
	{CSTR("Intel i7-3537U"),   Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i7-3537U",                 17,   22,  "x86 with HT",  2,  2000, 3100, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  4096,  "HD Graphics 4000"},
	{CSTR("Intel i7-4771"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i7-4771",                  84,   22,  "x86 with HT",  4,  3500, 3900, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  8192,  "HD Graphics 4600"},
	{CSTR("Intel i7-4790"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_INTEL,     "Core i7-4790",                  84,   22,  "x86 with HT",  4,  3600, 4000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  8192,  "HD Graphics 4600"},
	{CSTR("JZ4720"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4720",                 0,    180, "MIPS32 rev1",  1,  240,  240,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("JZ4725B"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4725B",                0,    160, "MIPS32 rev1",  1,  360,  360,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("JZ4730"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4730",                 0,    180, "MIPS32 rev1",  1,  336,  336,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("JZ4740"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4740",                 0,    180, "MIPS32 rev1",  1,  360,  360,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("JZ4750"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4750",                 0,    180, "MIPS32 rev1",  1,  360,  360,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("JZ4755"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4755",                 0,    160, "MIPS32 rev1",  1,  400,  400,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("JZ4760"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4760",                 0,    130, "MIPS32 rev1",  1,  600,  600,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Vivante GC200"},
	{CSTR("JZ4770"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4770",                 0,    65,  "MIPS32 rev2",  1,  1000, 1000, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Vivante GC860"},
	{CSTR("JZ4775"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4775",                 0,    65,  "MIPS32 rev2",  1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "X2D Core"},
	{CSTR("JZ4780"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_INGENIC,   "XBurst JZ4780",                 0,    40,  "MIPS32 rev2",  2,  1200, 1200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX 540"},
	{CSTR("Kirin 620"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 620",                     0,    28,  "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450 MP4"},
	{CSTR("Kirin 650"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 650",                     0,    16,  "Cortex-A53",   4,  2000, 2000, 0,   0,  "Cortex-A53",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T830 MP2"},
	{CSTR("Kirin 658"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 658",                     0,    16,  "Cortex-A53",   4,  2350, 2350, 0,   0,  "Cortex-A53",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T830 MP2"},
	{CSTR("Kirin 659"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 659",                     0,    16,  "Cortex-A53",   4,  2360, 2360, 0,   0,  "Cortex-A53",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T830 MP2"},
	{CSTR("Kirin 710"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 710",                     0,    12,  "Cortex-A73",   4,  2200, 2200, 0,   0,  "Cortex-A53",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G51 MP4"},
	{CSTR("Kirin 710F"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 710F",                    0,    12,  "Cortex-A73",   4,  2200, 2200, 0,   0,  "Cortex-A53",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G51 MP4"},
	{CSTR("Kirin 810"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 810",                     0,    7,   "Cortex-A76",   2,  2200, 2200, 0,   0,  "Cortex-A55",   6,  1900, 1900, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G52 MP6"},
	{CSTR("Kirin 910"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_HISILICON, "Kirin 910",                     0,    28,  "Cortex-A9",    4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450 MP4"},
	{CSTR("Kirin 920"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_HISILICON, "Kirin 920",                     0,    28,  "Cortex-A15",   4,  1700, 1700, 0,   0,  "Cortex-A7",    4,  1300, 1300, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T628 MP4"},
	{CSTR("Kirin 950"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 950",                     0,    16,  "Cortex-A72",   4,  2300, 2300, 0,   0,  "Cortex-A53",   4,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T880 MP4"},
	{CSTR("Kirin 955"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 955",                     0,    16,  "Cortex-A72",   4,  2500, 2500, 0,   0,  "Cortex-A53",   4,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T880 MP4"},
	{CSTR("Kirin 960"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 960",                     0,    16,  "Cortex-A73",   4,  2360, 2360, 0,   0,  "Cortex-A53",   4,  1840, 1840, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G71 MP8"},
	{CSTR("Kirin 970"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 970",                     0,    10,  "Cortex-A73",   4,  2360, 2360, 0,   0,  "Cortex-A53",   4,  1840, 1840, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G72 MP12"},
	{CSTR("Kirin 980"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 980",                     0,    7,   "Cortex-A76",   2,  2600, 2600, 0,   0,  "Cortex-A76",   2,  1920, 1920, 0,   0,  "Cortex-A55",   4,  1800, 1800, 0,   0,  0,    0,     "Mali-G76 MP10"},
	{CSTR("Kirin 990 4G"),     Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 990 4G",                  0,    7,   "Cortex-A76",   2,  2860, 2860, 0,   0,  "Cortex-A76",   2,  2090, 2090, 0,   0,  "Cortex-A55",   4,  1860, 1860, 0,   0,  0,    0,     "Mali-G76 MP16"},
	{CSTR("Kirin 990 5G"),     Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_HISILICON, "Kirin 990 5G",                  0,    7,   "Cortex-A76",   2,  2860, 2860, 0,   0,  "Cortex-A76",   2,  2360, 2360, 0,   0,  "Cortex-A55",   4,  1950, 1950, 0,   0,  0,    0,     "Mali-G76 MP16"},
	{CSTR("MDM9206"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ATHEROS,   "MDM9206",                       0,    0,   "Cortex-A7",    1,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MSM7225"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7225",         0,    65,  "ARM11",        1,  528,  528,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MSM7225A"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7225A",        0,    45,  "Cortex-A5",    1,  800,  800,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 200"},
	{CSTR("MSM7225AB"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7225AB",       0,    45,  "Cortex-A5",    1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 200"},
	{CSTR("MSM7227"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7227",         0,    65,  "ARM11",        1,  800,  800,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 200"},
	{CSTR("MSM7227A"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7227A",        0,    45,  "Cortex-A5",    1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 200"},
	{CSTR("MSM7230"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S2 MSM7230",         0,    45,  "Scorpion",     1,  800,  800,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 205"},
	{CSTR("MSM7625"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7625",         0,    65,  "ARM11",        1,  528,  528,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MSM7625A"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7625A",        0,    45,  "Cortex-A5",    1,  800,  800,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 200"},
	{CSTR("MSM7627"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7627",         0,    65,  "ARM11",        1,  800,  800,  16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 200"},
	{CSTR("MSM7627A"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S1 MSM7627A",        0,    45,  "Cortex-A5",    1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 200"},
	{CSTR("MSM7630"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S2 MSM7630",         0,    45,  "Scorpion",     1,  800,  800,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Adreno 205"},
	{CSTR("MSM8225"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S4 Play MSM8225",    0,    45,  "Cortex-A5",    2,  1200, 1200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 203"},
	{CSTR("MSM8226"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8226",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("MSM8227"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S4 Plus MSM8227",    0,    28,  "Krait",        2,  1000, 1000, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8228"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8228",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("MSM8230"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8230",        0,    28,  "Krait 200",    2,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8230AB"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8230AB",      0,    28,  "Krait 300",    2,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8255"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S2 MSM8255",         0,    45,  "Scorpion",     1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  384,  0,     "Adreno 205"},
	{CSTR("MSM8255T"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S2 MSM8255T",        0,    45,  "Scorpion",     1,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  384,  0,     "Adreno 205"},
	{CSTR("MSM8260"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S3 MSM8260",         0,    45,  "Scorpion",     2,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 220"},
	{CSTR("MSM8274-AA"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 800 MSM8274-AA",     0,    28,  "Krait 400" ,   4,  2260, 2260, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8274-AB"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 800 MSM8274-AB",     0,    28,  "Krait 400",    4,  2360, 2360, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8274-AC"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 801 MSM8274-AC",     0,    28,  "Krait 400",    4,  2450, 2450, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8625"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S4 Play MSM8625",    0,    45,  "Cortex-A5",    4,  1200, 1200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 203"},
	{CSTR("MSM8626"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8626",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("MSM8627"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S4 Plus MSM8627",    0,    28,  "Krait",        2,  1000, 1000, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8628"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8628",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("MSM8630"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8630",        0,    28,  "Krait 200",    2,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8630AB"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8630AB",      0,    28,  "Krait 300",    2,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8655"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S2 MSM8655",         0,    45,  "Scorpion",     1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  384,  0,     "Adreno 205"},
	{CSTR("MSM8655T"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S2 MSM8655T",        0,    45,  "Scorpion",     1,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  384,  0,     "Adreno 205"},
	{CSTR("MSM8660"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon S3 MSM8660",         0,    45,  "Scorpion",     2,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 220"},
	{CSTR("MSM8674-AA"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 800 MSM8274-AA",     0,    28,  "Krait 400" ,   4,  2260, 2260, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8674-AB"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 801 MSM8274-AB",     0,    28,  "Krait 400",    4,  2360, 2360, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8916"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 410 MSM8916",        0,    28,  "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 306"},
	{CSTR("MSM8916v2"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 412 MSM8916v2",      0,    28,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 306"},
	{CSTR("MSM8917"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 425 MSM8917",        0,    28,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 308"},
	{CSTR("MSM8920"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 427 MSM8920",        0,    28,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 308"},
	{CSTR("MSM8926"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8926",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("MSM8928"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8928",        0,    28,  "Cortex-A7",    4,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Adreno 305"},
	{CSTR("MSM8929"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 415 MSM8929",        0,    28,  "Cortex-A53",   8,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 405"},
	{CSTR("MSM8930"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8930",        0,    28,  "Krait 200",    2,  1200, 1200, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8930AA"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8930AA",      0,    28,  "Krait 300",    2,  1400, 1400, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8930AB"),        Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 400 MSM8930AB",      0,    28,  "Krait 300",    2,  1700, 1700, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Adreno 305"},
	{CSTR("MSM8936"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 610 MSM8936",        0,    28,  "Cortex-A53",   4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 405"},
	{CSTR("MSM8937"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 430 MSM8937",        0,    28,  "Cortex-A53",   8,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 505"},
	{CSTR("MSM8939"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 615 MSM8939",        0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  "Cortex-A53",   4,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 405"},
	{CSTR("MSM8939v2"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 616 MSM8939v2",      0,    28,  "Cortex-A53",   4,  1700, 1700, 0,   0,  "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 405"},
	{CSTR("MSM8940"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 435 MSM8940",        0,    28,  "Cortex-A53",   8,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 505"},
	{CSTR("MSM8952"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 617 MSM8952",        0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 405"},
	{CSTR("MSM8953"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 625 MSM8953",        0,    14,  "Cortex-A53",   8,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 506"},
	{CSTR("MSM8953Pro"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 626 MSM8953Pro",     0,    14,  "Cortex-A53",   8,  2200, 2200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 506"},
	{CSTR("MSM8956"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 650 MSM8956",        0,    28,  "Cortex-A72" ,  2,  1800, 1800, 0,   0,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 510"},
	{CSTR("MSM8974-AA"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 800 MSM8974-AA",     0,    28,  "Krait 400" ,   4,  2260, 2260, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8974-AB"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 801 MSM8974-AB",     0,    28,  "Krait 400",    4,  2360, 2360, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8974-AC"),       Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 801 MSM8974-AC",     0,    28,  "Krait 400",    4,  2450, 2450, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8974PRO-AB"),    Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 801 MSM8974PRO-AB",  0,    28,  "Krait 400",    4,  2300, 2300, 16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  2048, 0,     "Adreno 330"},
	{CSTR("MSM8976"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 652 MSM8976",        0,    28,  "Cortex-A72" ,  4,  1800, 1800, 0,   0,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 510"},
	{CSTR("MSM8976-PRO"),      Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 653 MSM8976 Pro",    0,    28,  "Cortex-A72" ,  4,  1950, 1950, 0,   0,  "Cortex-A53",   4,  1440, 1440, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 510"},
	{CSTR("MSM8992"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 808 MSM8992",        0,    20,  "Cortex-A57",   2,  1820, 1820, 0,   0,  "Cortex-A53",   4,  1440, 1440, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 418"},
	{CSTR("MSM8994"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 810 MSM8994",        0,    20,  "Cortex-A57",   4,  2000, 2000, 0,   0,  "Cortex-A53",   4,  1550, 1550, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 430"},
	{CSTR("MSM8996"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 820 MSM8996",        0,    14,  "Kryo",         2,  2150, 2150, 0,   0,  "Kryo",         2,  1593, 1593, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 530"},
	{CSTR("MSM8996Lite"),      Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 820 MSM8996Lite",    0,    14,  "Kryo",         2,  1804, 1804, 0,   0,  "Kryo",         2,  1363, 1363, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 530"},
	{CSTR("MSM8996Pro-AB"),    Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 821 MSM8996Pro-AB",  0,    14,  "Kryo",         2,  2150, 2150, 0,   0,  "Kryo",         2,  1593, 1593, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 530"},
	{CSTR("MSM8996Pro-AC"),    Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 821 MSM8996Pro-AC",  0,    14,  "Kryo",         2,  2342, 2342, 0,   0,  "Kryo",         2,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 530"},
	{CSTR("MSM8998"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 835 MSM8998",        0,    10,  "Kryo 280",     4,  2450, 2450, 0,   0,  "Kryo 280",     4,  1900, 1900, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 540"},
	{CSTR("MT2601"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT2601",                        0,    0,   "Cortex-A7",    2,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("MT2621"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT2621",                        0,    0,   0,              1,  260,  260,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT2625"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT2625",                        0,    0,   "Cortex-M",     1,  104,  104,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6205"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6205",                        0,    0,   "ARM7",         1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6216"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6216",                        0,    0,   "ARM7",         1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6217"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6217",                        0,    0,   "ARM7",         1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6218B"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6218B",                       0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6219"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6219",                        0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6223"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6223",                        0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6225"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6225",                        0,    0,   "ARM7",         1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6226"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6226",                        0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6227"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6227",                        0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6228"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6228",                        0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6229"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6229",                        0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6230"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6230",                        0,    0,   "ARM7",         1,  52,   52,   16,  16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6235"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6235",                        0,    0,   "ARM9",         1,  208,  208,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6236"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6236",                        0,    0,   "ARM9",         1,  208,  208,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6513"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6513",                        0,    65,  "ARM11",        1,  650,  650,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX531"},
	{CSTR("MT6515"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6515",                        0,    40,  "Cortex-A9",    1,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX531 Ultra"},
	{CSTR("MT6516"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6516",                        0,    65,  "ARM9",         1,  416,  416,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT6517"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6517",                        0,    40,  "Cortex-A9",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX531 Ultra"},
	{CSTR("MT6517T"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6517T",                       0,    40,  "Cortex-A9",    2,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX531 Ultra"},
	{CSTR("MT6570"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6570",                        0,    28,  "Cortex-A7",    2,  1300, 1300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP1"},
	{CSTR("MT6571"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6571",                        0,    28,  "Cortex-A7",    2,  1300, 1300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP1"},
	{CSTR("MT6572"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6572",                        0,    28,  "Cortex-A7",    2,  1400, 1400, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP1"},
	{CSTR("MT6572M"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6572M",                       0,    28,  "Cortex-A7",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-400 MP1"},
	{CSTR("MT6573"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6573",                        0,    65,  "ARM11",        1,  650,  650,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX531"},
	{CSTR("MT6575"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6575",                        0,    40,  "Cortex-A9",    1,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "PowerVR SGX531 Ultra"},
	{CSTR("MT6575M"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6575M",                       0,    65,  "Cortex-A9",    1,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "PowerVR SGX531"},
	{CSTR("MT6577"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6577",                        0,    40,  "Cortex-A9",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX531 Ultra"},
	{CSTR("MT6577T"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6577T",                       0,    40,  "Cortex-A9",    2,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX531 Ultra"},
	{CSTR("MT6580"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6580",                        0,    28,  "Cortex-A7",    4,  1300, 1300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP1"},
	{CSTR("MT6582"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6582",                        0,    28,  "Cortex-A7",    4,  1300, 1300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP2"},
	{CSTR("MT6582M"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6582M",                       0,    28,  "Cortex-A7",    4,  1300, 1300, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP2"},
	{CSTR("MT6588"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6588",                        0,    28,  "Cortex-A7",    4,  1700, 1700, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Mali-450 MP4"},
	{CSTR("MT6589"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6589",                        0,    28,  "Cortex-A7",    4,  1200, 1200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "PowerVR SGX544MP"},
	{CSTR("MT6589M"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6589M",                       0,    28,  "Cortex-A7",    4,  1200, 1200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "PowerVR SGX544MP"},
	{CSTR("MT6589T"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT6589T",                       0,    28,  "Cortex-A7",    4,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "PowerVR SGX544MP"},
	{CSTR("MT6732"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6732",                        0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T760 MP2"},
	{CSTR("MT6732M"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6732M",                       0,    28,  "Cortex-A53",   4,  1300, 1300, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T760 MP2"},
	{CSTR("MT6735"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6735",                        0,    28,  "Cortex-A53",   4,  1300, 1300, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T720 MP2"},
	{CSTR("MT6735M"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6735M",                       0,    28,  "Cortex-A53",   4,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T720 MP2"},
	{CSTR("MT6735P"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6735P",                       0,    28,  "Cortex-A53",   4,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T720 MP2"},
	{CSTR("MT6737"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6737",                        0,    28,  "Cortex-A53",   4,  1100, 1300, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T720 MP2"},
	{CSTR("MT6737T"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6737T",                       0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T720 MP2"},
	{CSTR("MT6738"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6738",                        0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T860 MP2"},
	{CSTR("MT6738T"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6738T",                       0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T860 MP2"},
	{CSTR("MT6739"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6739",                        0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR GE8100"},
	{CSTR("MT6739WW"),         Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6739WW",                      0,    28,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR GE8100"},
	{CSTR("MT6750"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6750",                        0,    28,  "Cortex-A53",   4,  1000, 1000, 0,   0,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T860 MP2"},
	{CSTR("MT6750T"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6750T",                       0,    28,  "Cortex-A53",   4,  1000, 1000, 0,   0,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T860 MP2"},
	{CSTR("MT6752"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6752",                        0,    28,  "Cortex-A53",   8,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T760 MP2"},
	{CSTR("MT6752M"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6752M",                       0,    28,  "Cortex-A53",   8,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T760 MP2"},
	{CSTR("MT6753"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "MT6753",                        0,    28,  "Cortex-A53",   4,  1300, 1300, 0,   0,  "Cortex-A53",   4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T720 MP3"},
	{CSTR("MT6755"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello P10 MT6755",              0,    28,  "Cortex-A53",   4,  2000, 2000, 0,   0,  "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T860 MP2"},
	{CSTR("MT6755T"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello P15 MT6755T",             0,    28,  "Cortex-A53",   4,  2200, 2200, 0,   0,  "Cortex-A53",   4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T860 MP2"},
	{CSTR("MT6757"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello P20 MT6757",              0,    16,  "Cortex-A53",   4,  2300, 2300, 0,   0,  "Cortex-A53",   4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T880 MP2"},
	{CSTR("MT6757CD"),         Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello P25 MT6757CD",            0,    16,  "Cortex-A53",   4,  2600, 2600, 0,   0,  "Cortex-A53",   4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-T880 MP2"},
	{CSTR("MT6762"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello P22 MT6762",              0,    12,  "Cortex-A53",   8,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR GE8320"},
	{CSTR("MT6763T"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello P23 MT6763T",             0,    16,  "Cortex-A53",   4,  2300, 2300, 0,   0,  "Cortex-A53",   4,  1650, 1650, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-G71MP2"},
	{CSTR("MT6765"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello P35 MT6765",              0,    12,  "Cortex-A53",   8,  2300, 2300, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR GE8320"},
	{CSTR("MT6785V"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Helio G90T",                    0,    12,  "Cortex-A76",   2,  2050, 2050, 0,   0,  "Cortex-A55",   6,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali G76 MC4"},
	{CSTR("MT6795"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello X10 MT6795",              0,    28,  "Cortex-A53",   8,  2000, 2000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR G6200"},
	{CSTR("MT6797"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello X20 MT6797",              0,    20,  "Cortex-A72",   2,  2100, 2100, 0,   0,  "Cortex-A53",   4,  1850, 1850, 0,   0,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,    0,     "Mali-T880 MP4"},
	{CSTR("MT6797D"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello X23 MT6797D",             0,    20,  "Cortex-A72",   2,  2300, 2300, 0,   0,  "Cortex-A53",   4,  1850, 1850, 0,   0,  "Cortex-A53",   4,  1400, 1400, 0,   0,  0,    0,     "Mali-T880 MP4"},
	{CSTR("MT6797T"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello X25 MT6797T",             0,    20,  "Cortex-A72",   2,  2500, 2500, 0,   0,  "Cortex-A53",   4,  2000, 2000, 0,   0,  "Cortex-A53",   4,  1550, 1550, 0,   0,  0,    0,     "Mali-T880 MP4"},
	{CSTR("MT6797X"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello X27 MT6797X",             0,    20,  "Cortex-A72",   2,  2600, 2600, 0,   0,  "Cortex-A53",   4,  2000, 2000, 0,   0,  "Cortex-A53",   4,  1600, 1600, 0,   0,  0,    0,     "Mali-T880 MP4"},
	{CSTR("MT6799"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_MEDIATEK,  "Hello X30 MT6799",              0,    10,  "Cortex-A72",   2,  2500, 2500, 0,   0,  "Cortex-A53",   4,  2200, 2200, 0,   0,  "Cortex-A53",   4,  1900, 1900, 0,   0,  0,    0,     "PowerVR GT7400 Plus"},
	{CSTR("MT7620"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_MEDIATEK,  "MT7620",                        0,    0,   "MIPS 24kec",   1,  580,  620,  64,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT7621"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_MEDIATEK,  "MT7621",                        0,    0,   "MIPS 1004Kc",  2,  880,  880,  32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("MT7628"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_MEDIATEK,  "MT7628",                        0,    0,   "MIPS 24KEc",   1,  575,  580,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT7688"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_MEDIATEK,  "MT7688",                        0,    0,   "MIPS 24KEc",   1,  580,  580,  64,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MT8117"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT8117",                        0,    28,  "Cortex-A7",    2,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX544"},
	{CSTR("MT8121"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT8121",                        0,    28,  "Cortex-A7",    4,  1300, 1300, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR SGX544"},
	{CSTR("MT8125"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT8125",                        0,    28,  "Cortex-A7",    4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "PowerVR SGX544"},
	{CSTR("MT8127"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT8127",                        0,    28,  "Cortex-A7",    4,  1300, 1300, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-450 MP4"},
	{CSTR("MT8135"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT8135",                        0,    28,  "Cortex-A7",    2,  1200, 1200, 0,   0,  "Cortex-A15",   2,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR G6200"},
	{CSTR("MT8135V"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT8135V",                       0,    28,  "Cortex-A7",    2,  1200, 1200, 0,   0,  "Cortex-A15",   2,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR G6200"},
	{CSTR("MT8151"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MEDIATEK,  "MT8151",                        0,    28,  "Cortex-A7",    8,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-450 MP4"},
	{CSTR("MV76100"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Discovery MV76100",             0,    0,   "88fr571-vd",   1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MV78100"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Discovery MV78100",             0,    0,   "88fr571-vd",   1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MV78200"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Discovery MV78200",             0,    0,   "88fr571-vd",   1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MV78230"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada XP MV78230",             0,    0,   "Sheeva ARMv7", 2,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MV78260"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada XP MV78260",             0,    0,   "Sheeva ARMv7", 2,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("MV78460"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "Armada XP MV78460",             0,    0,   "Sheeva ARMv7", 4,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("OMAP4430"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "OMAP4430",                      0,    45,  "Cortex-A9",    2,  800,  1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     0},
	{CSTR("OMAP4460"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "OMAP4460",                      0,    45,  "Cortex-A9",    2,  1200, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "PowerVR SGX540"},
	{CSTR("OMAP4470"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_TI,        "OMAP4470",                      0,    45,  "Cortex-A9",    2,  1300, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "PowerVR SGX540"},
	{CSTR("PXA988"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_MARVELL,   "PXA988",                        0,    0,   "Cortex-A9",    2,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA4530"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA4530",                       0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA4531"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA4531",                       0,    0,   "MIPS 24Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9531"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9531",                       0,    0,   "MIPS 24Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9533"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9533",                       0,    0,   "MIPS 24Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9550"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9550",                       0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9556"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9556",                       0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9557"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9557",                       0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9558"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9558",                       0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9561"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9561",                       0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("QCA9563"),          Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "QCA9563",                       0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RDA8810"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_RDA,       "RDA8810",                       0,    0,   "Cortex-A5",    1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     0},
	{CSTR("RK2601"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2601",                        0,    180, 0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2602"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2602",                        0,    0,   0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2606"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2606",                        0,    180, 0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2608"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2608",                        0,    0,   0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2610"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2610",                        0,    0,   0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2616"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2616",                        0,    0,   0,              1,  300,  300,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2618"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2618",                        0,    0,   0,              1,  300,  300,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2619"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2619",                        0,    0,   0,              1,  640,  640,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2706"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2706",                        0,    0,   0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2708"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2708",                        0,    0,   0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2710-G"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2710-G",                      0,    0,   "ARM7EJ",       1,  266,  266,  0,   16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2710-M"),         Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2710-M",                      0,    0,   "ARM7EJ",       1,  300,  300,  0,   16, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2718"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2718",                        0,    0,   0,              1,  320,  320,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2728"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2728",                        0,    0,   0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2729"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2729",                        0,    0,   0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2738"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2738",                        0,    0,   0,              1,  600,  600,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2806"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2806",                        0,    0,   0,              1,  600,  600,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2808"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2808",                        0,    65,  0,              1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2808A"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2808A",                       0,    0,   0,              1,  560,  560,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2816"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2816",                        0,    0,   0,              1,  560,  560,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2818"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2818",                        0,    0,   0,              1,  640,  640,  0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("RK2918"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2918",                        0,    55,  "Cortex-A8",    1,  1000, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Vivante GC800"},
	{CSTR("RK2926"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2926",                        0,    55,  "Cortex-A9",    1,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  128,  0,     "Mali-400 MP"},
	{CSTR("RK2928"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK2928",                        0,    55,  "Cortex-A9",    1,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-400 MP"},
	{CSTR("RK3026"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3026",                        0,    40,  "Cortex-A9",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-400 MP2"},
	{CSTR("RK3036"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3036",                        0,    40,  "Cortex-A7",    2,  1000, 1000, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-400 MP"},
	{CSTR("RK3066"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3066",                        0,    40,  "Cortex-A9",    2,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP4"},
	{CSTR("RK3126"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3126",                        0,    40,  "Cortex-A7",    4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP2"},
	{CSTR("RK3128"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3128",                        0,    40,  "Cortex-A7",    4,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP2"},
	{CSTR("RK3168"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3168",                        0,    28,  "Cortex-A9",    2,  1200, 1200, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "PowerVR SGX540"},
	{CSTR("RK3188"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3188",                        0,    28,  "Cortex-A9",    4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP4"},
	{CSTR("RK3188T"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3188T",                       0,    28,  "Cortex-A9",    4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP4"},
	{CSTR("RK3229"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3229",                        0,    28,  "Cortex-A7",    4,  1500, 1500, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP2"},
	{CSTR("RK3288"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ROCKCHIP,  "RK3288",                        0,    28,  "Cortex-A17",   4,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Mali-T760 MP4"},
	{CSTR("RK3328"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ROCKCHIP,  "RK3328",                        0,    28,  "Cortex-A53",   4,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-450 MP2"},
	{CSTR("RK3368"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ROCKCHIP,  "RK3368",                        0,    28,  "Cortex-A53",   8,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "PowerVR G6110"},
	{CSTR("RK3399"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ROCKCHIP,  "RK3399",                        0,    28,  "Cortex-A72",   2,  2000, 2000, 48,  32, "Cortex-A53",   4,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  1024, 0,     "Mali-T860"},
	{CSTR("RTD1293"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_REALTEK,   "RTD1293",                       0,    0,   "Cortex-A53",   2,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Mali-T820 MP3"},
	{CSTR("RTD1294"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_REALTEK,   "RTD1294",                       0,    0,   "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Mali-T820 MP3"},
	{CSTR("RTD1295"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_REALTEK,   "RTD1295",                       0,    0,   "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Mali-T820 MP3"},
	{CSTR("RTD1296"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_REALTEK,   "RTD1296",                       0,    0,   "Cortex-A53",   4,  1400, 1400, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "Mali-T820 MP3"},
	{CSTR("SC9830"),           Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_SPREADTRUM,"SC9830A",                       0,    28,  "Cortex-A7",    4,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Mali-400 MP2"},
	{CSTR("SDM429"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 429 SDM429",         0,    12,  "Cortex-A53",   4,  1950, 1950, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 504"},
	{CSTR("SDM439"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 439 SDM439",         0,    12,  "Cortex-A53",   4,  1950, 1950, 0,   0,  "Cortex-A53",   4,  1450, 1450, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 505"},
	{CSTR("SDM450"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 450 SDM450",         0,    14,  "Cortex-A53",   8,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 506"},
	{CSTR("SDM630"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 630 SDM630",         0,    14,  "Cortex-A53",   4,  2200, 2200, 0,   0,  "Cortex-A53",   4,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 508"},
	{CSTR("SDM636"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 636 SDM636",         0,    14,  "Kryo 260",     4,  1800, 1800, 0,   0,  "Kryo 260",     4,  1600, 1600, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 509"},
	{CSTR("SDM660"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 660 SDM660",         0,    14,  "Kryo 260",     4,  2200, 2200, 0,   0,  "Kryo 260",     4,  1840, 1840, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 512"},
	{CSTR("SDM710"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 710 SDM710",         0,    10,  "Kryo 360",     2,  2200, 2200, 0,   0,  "Kryo 360",     6,  1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 616"},
	{CSTR("SDM845"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 845 SDM845",         0,    10,  "Kryo 385",     4,  2800, 2800, 0,   0,  "Kryo 385",     4,  1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 630"},
	{CSTR("SM6115"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 662 SM6115",         0,    11,  "Kryo 260 Gold",4,  2000, 2000, 0,   0,  "Kryo 260 Silver",4,1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 610"},
	{CSTR("SM6125"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 665 SM6125",         0,    11,  "Kryo 260 Gold",4,  2000, 2000, 0,   0,  "Kryo 260 Silver",4,1800, 1800, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 610"},
	{CSTR("SM6150"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 675 SM6150",         0,    11,  "Kryo 460 Gold",2,  2000, 2000, 0,   0,  "Kryo 460 Silver",6,1700, 1700, 0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     "Adreno 612"},
	{CSTR("SM7150-AA"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 730 SM7150-AA",      0,    8,   "Kryo 470 Gold",2,  2200, 2200, 0,   0,  "Kryo 470 Silver",6,1800, 1800, 0,   0,  "Kryo 485",     4,  1800, 1800, 0,   0,  0,    0,     "Adreno 618"},
	{CSTR("SM7150-AB"),        Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 730G SM7150-AB",     0,    8,   "Kryo 470 Gold",2,  2200, 2200, 0,   0,  "Kryo 470 Silver",6,1800, 1800, 0,   0,  "Kryo 485",     4,  1800, 1800, 0,   0,  0,    0,     "Adreno 618"},
	{CSTR("SM8150"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_QUALCOMM,  "Snapdragon 855 SM8150",         0,    7,   "Kryo 485",     1,  2840, 2840, 0,   0,  "Kryo 485",     3,  2420, 2420, 0,   0,  "Kryo 485",     4,  1800, 1800, 0,   0,  0,    0,     "Adreno 640"},
	{CSTR("SUN4IW1"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "A10",                           0,    55,  "Cortex-A8",    1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400"},
	{CSTR("SUN4IW2"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "A13",                           0,    55,  "Cortex-A8",    1,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400"},
	{CSTR("SUN50IW1P1"),       Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ALLWINNER, "A64",                           0,    40,  "Cortex-A53",   4,  0,    0,    32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP2"},
	{CSTR("SUN50IW2"),         Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ALLWINNER, "H5",                            0,    40,  "Cortex-A53",   4,  1200, 1200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-450 MP4"},
	{CSTR("SUN50IW6"),         Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ALLWINNER, "H6",                            0,    28,  "Cortex-A53",   4,  1800, 1800, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-T720 MP2"},
	{CSTR("SUN8IW1"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "A31",                           0,    40,  "Cortex-A7",    4,  0,    0,    32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "PowerVR SGX544"},
	{CSTR("SUN8IW2"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "A20",                           0,    40,  "Cortex-A7",    2,  1000, 1000, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP2"},
	{CSTR("SUN8IW3"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "A23",                           0,    40,  "Cortex-A7",    2,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  256,  0,     "Mali-400 MP2"},
	{CSTR("SUN8IW5"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "A33",                           0,    40,  "Cortex-A7",    4,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP2"},
	{CSTR("SUN8IW6"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "A83T",                          0,    28,  "Cortex-A7",    8,  1600, 1600, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     "PowerVR SGX544 MP1"},
	{CSTR("SUN8IW7"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "H3",                            0,    40,  "Cortex-A7",    4,  1296, 1296, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP2"},
	{CSTR("SUN8IW8"),          Manage::ThreadContext::CT_ARM,    Manage::CPUVendor::CB_ALLWINNER, "V3",                            0,    0,   "Cortex-A7",    4,  1200, 1200, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  128,  0,     0},
	{CSTR("SUN8IW10"),         Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ALLWINNER, "B288",                          0,    0,   "Cortex-A7",    2,  0,    0,    32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  128,  0,     0},
	{CSTR("SUN8IW11"),         Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ALLWINNER, "V40",                           0,    40,  "Cortex-A7",    4,  1500, 1500, 32,  32, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  512,  0,     "Mali-400 MP2"},
	{CSTR("SUN9IW1"),          Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ALLWINNER, "A80",                           0,    28,  "Cortex-A7",    4,  0,    0,    32,  32, "Cortex-A15",   4,  0,    0,    32,  32, 0,              0,  0,    0,    0,   0,  2560, 0,     "PowerVR G6230"},
	{CSTR("SUNIW1"),           Manage::ThreadContext::CT_ARM64,  Manage::CPUVendor::CB_ALLWINNER, "F1C100",                        0,    0,   "ARM9",         1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("TMZL625OAX5DY"),    Manage::ThreadContext::CT_X86_64, Manage::CPUVendor::CB_AMD,       "Turion Neo X2 L625",            18,   65,  "x86",          2,  1600, 1600, 64,  64, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  1024, 0,     0},
	{CSTR("TP9343"),           Manage::ThreadContext::CT_MIPS,   Manage::CPUVendor::CB_ATHEROS,   "TP9343",                        0,    0,   "MIPS 74Kc",    1,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  0,    0,     0},
	{CSTR("VIA C7-M 771"),     Manage::ThreadContext::CT_X86_32, Manage::CPUVendor::CB_VIA,       "C7-M 771",                      7,    90,  "x86",          1,  1200, 1200, 64,  64, 0,              0,  0,    0,    0,   0,  0,              0,  0,    0,    0,   0,  128,  0,     0},
};

Manage::CPUDB::CPUMapping Manage::CPUDB::cpuX86Map[] = {
	{CSTR("11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz"),   CSTR("Intel i5-1135G7")},
	{CSTR("11th Gen Intel(R) Core(TM) i7-1165G7 @ 2.80GHz"),   CSTR("Intel i7-1165G7")},
	{CSTR("AMD A10-9700E RADEON R7, 10 COMPUTE CORES 4C+6G"),  CSTR("AD9700AHM44AB")},
	{CSTR("AMD Phenom(tm) 9650 Quad-Core Processor"),          CSTR("HD9650WCGHBOX")},
	{CSTR("AMD Turion(tm) Neo X2 Dual Core Processor L625"),   CSTR("TMZL625OAX5DY")},
	{CSTR("Intel(R) Atom(TM) CPU  330   @ 1.60GHz"),           CSTR("Intel 330")},
	{CSTR("Intel(R) Atom(TM) CPU N270   @ 1.60GHz"),           CSTR("Intel N270")},
	{CSTR("Intel(R) Atom(TM) x5-Z8300  CPU @ 1.44GHz"),        CSTR("Intel Z8300")},
	{CSTR("Intel(R) Atom(TM) x5-Z8350  CPU @ 1.44GHz"),        CSTR("Intel Z8350")},
	{CSTR("Intel(R) Celeron(R) CPU  N2840  @ 2.16GHz"),        CSTR("Intel N2840")},
	{CSTR("Intel(R) Celeron(R) CPU  N3160  @ 1.60GHz"),        CSTR("Intel N3160")},
	{CSTR("Intel(R) Celeron(R) N4000 CPU @ 1.10GHz"),          CSTR("Intel N4000")},
	{CSTR("Intel(R) Celeron(R) N4120 CPU @ 1.10GHz"),          CSTR("Intel N4120")},
	{CSTR("Intel(R) Core(TM) i3-3220 CPU @ 3.30GHz"),          CSTR("Intel i3-3220")},
	{CSTR("Intel(R) Core(TM) i5-10210U CPU @ 1.60GHz"),        CSTR("Intel i5-10210U")},
	{CSTR("Intel(R) Core(TM) i5-3550 CPU @ 3.30GHz"),          CSTR("Intel i5-3550")},
	{CSTR("Intel(R) Core(TM) i5-4460  CPU @ 3.20GHz"),         CSTR("Intel i5-4460")},
	{CSTR("Intel(R) Core(TM) i5-7500T CPU @ 2.70GHz"),         CSTR("Intel i5-7500T")},
	{CSTR("Intel(R) Core(TM) i5-8250U CPU @ 1.60GHz"),         CSTR("Intel i5-8250U")},
	{CSTR("Intel(R) Core(TM) i5-8265U CPU @ 1.60GHz"),         CSTR("Intel i5-8265U")},
	{CSTR("Intel(R) Core(TM) i7-3537U CPU @ 2.00GHz"),         CSTR("Intel i7-3537U")},
	{CSTR("Intel(R) Core(TM) i7-4771 CPU @ 3.50GHz"),          CSTR("Intel i7-4771")},
	{CSTR("Intel(R) Core(TM) i7-4790 CPU @ 3.60GHz"),          CSTR("Intel i7-4790")},
	{CSTR("Intel(R) Core(TM)2 Duo CPU     T7700  @ 2.40GHz"),  CSTR("Intel T7700")},
	{CSTR("Intel(R) Xeon(R) Platinum 8163 CPU @ 2.50GHz"),     CSTR("Intel 8163")},
	{CSTR("Intel(R) Xeon(R) CPU E5-2650 v2 @ 2.60GHz"),        CSTR("Intel E5-2650V2")},
	{CSTR("Intel(R) Xeon(R) CPU E5-2680 v3 @ 2.50GHz"),        CSTR("Intel E5-2680V3")},
	{CSTR("VIA C7-M Processor 1200MHz"),                       CSTR("VIA C7-M 771")},
};
#endif

Manage::CPUDB::CPUSpecX86 *Manage::CPUDB::GetCPUSpecX86(Manage::CPUVendor::CPU_BRAND brand, Int32 familyId, Int32 modelId, Int32 steppingId)
{
	if (brand == Manage::CPUVendor::CB_INTEL)
	{
		OSInt i = 0;
		OSInt j = (sizeof(intelCPU) / sizeof(intelCPU[0])) - 1;
		OSInt k;
		Manage::CPUDB::CPUSpecX86 *cpu;

		while (i <= j)
		{
			k = (i + j) >> 1;
			cpu = &intelCPU[k];
			if (cpu->familyId > familyId)
			{
				j = k - 1;
			}
			else if (cpu->familyId < familyId)
			{
				i = k + 1;
			}
			else if (cpu->modelId > modelId)
			{
				j = k - 1;
			}
			else if (cpu->modelId < modelId)
			{
				i = k + 1;
			}
			else if (cpu->steppingId > steppingId)
			{
				j = k - 1;
			}
			else if (cpu->steppingId < steppingId)
			{
				i = k + 1;
			}
			else
			{
				return cpu;
			}
		}
		return 0;
	}
	else if (brand == Manage::CPUVendor::CB_AMD)
	{
		OSInt i = 0;
		OSInt j = (sizeof(amdCPU) / sizeof(amdCPU[0])) - 1;
		OSInt k;
		Manage::CPUDB::CPUSpecX86 *cpu;

		while (i <= j)
		{
			k = (i + j) >> 1;
			cpu = &amdCPU[k];
			if (cpu->familyId > familyId)
			{
				j = k - 1;
			}
			else if (cpu->familyId < familyId)
			{
				i = k + 1;
			}
			else if (cpu->modelId > modelId)
			{
				j = k - 1;
			}
			else if (cpu->modelId < modelId)
			{
				i = k + 1;
			}
			else if (cpu->steppingId > steppingId)
			{
				j = k - 1;
			}
			else if (cpu->steppingId < steppingId)
			{
				i = k + 1;
			}
			else
			{
				return cpu;
			}
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

const Manage::CPUDB::CPUSpec *Manage::CPUDB::GetCPUSpec(Text::CString model)
{
	OSInt i = 0;
	OSInt j = (sizeof(cpuList) / sizeof(cpuList[0])) - 1;
	OSInt k;
	OSInt l;
	Manage::CPUDB::CPUSpec *cpu;

	while (i <= j)
	{
		k = (i + j) >> 1;
		cpu = &cpuList[k];
		l = cpu->model.CompareToFast(model);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return cpu;
		}
	}
	return 0;
}

Text::CString Manage::CPUDB::X86CPUNameToModel(Text::CString x86CPUName)
{
	OSInt i = 0;
	OSInt j = (sizeof(cpuX86Map) / sizeof(cpuX86Map[0])) - 1;
	OSInt k;
	OSInt l;
	CPUMapping *cpu;

	while (i <= j)
	{
		k = (i + j) >> 1;
		cpu = &cpuX86Map[k];
		l = cpu->dispName.CompareToFast(x86CPUName);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return cpu->model;
		}
	}
	return CSTR_NULL;
}

Text::CString Manage::CPUDB::ParseCPUInfo(IO::Stream *stm)
{
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbHW;
	Text::StringBuilderUTF8 sbVID;
	Text::StringBuilderUTF8 sbCPUModel;
	Text::StringBuilderUTF8 sbSysType;
	Text::StringBuilderUTF8 sbModelName;
	Text::StringBuilderUTF8 sbProcessor;
	Int32 cpuPart = 0;
	Int32 cpuFamily = 0;
	Int32 cpuModel = 0;
	Int32 cpuStepping = 0;
	UOSInt i;
	NEW_CLASS(reader, Text::UTF8Reader(stm));
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb, 512))
		{
			break;
		}
		if (sb.StartsWith(UTF8STRC("CPU implementer	: ")))
		{
		}
		else if (sb.StartsWith(UTF8STRC("CPU architecture: ")))
		{
		}
		else if (sb.StartsWith(UTF8STRC("CPU variant	: ")))
		{
		}
		else if (sb.StartsWith(UTF8STRC("CPU part	: ")))
		{
			cpuPart = Text::StrToInt32(sb.ToString() + 11);
		}
		else if (sb.StartsWith(UTF8STRC("CPU revision	: ")))
		{
		}
		else if (sb.StartsWith(UTF8STRC("Hardware	: ")))
		{
			sbHW.ClearStr();
			sbHW.AppendC(sb.ToString() + 11, sb.GetLength() - 11);
		}
		else if (sb.StartsWith(UTF8STRC("vendor_id	: ")))
		{
			sbVID.ClearStr();
			sbVID.AppendC(sb.ToString() + 12, sb.GetLength() - 12);
		}
		else if (sb.StartsWith(UTF8STRC("cpu family	: ")))
		{
			cpuFamily = Text::StrToInt32(sb.ToString() + 13);
		}
		else if (sb.StartsWith(UTF8STRC("model		: ")))
		{
			cpuModel = Text::StrToInt32(sb.ToString() + 9);
		}
		else if (sb.StartsWith(UTF8STRC("stepping	: ")))
		{
			cpuStepping = Text::StrToInt32(sb.ToString() + 11);
		}
		else if (sb.StartsWith(UTF8STRC("system type		: ")))
		{
			sbSysType.ClearStr();
			sbSysType.AppendC(sb.ToString() + 15, sb.GetLength() - 15);
		}
		else if (sb.StartsWith(UTF8STRC("cpu model		: ")))
		{
			sbCPUModel.ClearStr();
			sbCPUModel.AppendC(sb.ToString() + 13, sb.GetLength() - 13);
		}
		else if (sb.StartsWith(UTF8STRC("model name	: ")))
		{
			sbModelName.ClearStr();
			sbModelName.AppendC(sb.ToString() + 13, sb.GetLength() - 13);
		}
	}
	DEL_CLASS(reader);

	if (sbVID.GetLength() > 0 && cpuFamily != 0 && cpuModel != 0 && cpuStepping != 0)
	{
		if (sbVID.Equals(UTF8STRC("GenuineIntel")))
		{
			return X86CPUNameToModel(sbModelName.ToCString());
		}
		else if (sbVID.Equals(UTF8STRC("AuthenticAMD")))
		{
			Manage::CPUDB::CPUSpecX86 *cpu = 0;
			cpu = GetCPUSpecX86(Manage::CPUVendor::CB_AMD, cpuFamily, cpuModel, cpuStepping);
			if (cpu)
			{
				return cpu->partNum;
			}
		}
		else
		{
		}
	}
	else if (sbCPUModel.StartsWith(UTF8STRC("Ingenic Xburst")))
	{
		const Manage::CPUDB::CPUSpec *cpu = 0;
		cpu = GetCPUSpec(sbSysType.ToCString());
		if (cpu)
		{
			return cpu->model;
		}
	}
	else if (sbSysType.StartsWith(UTF8STRC("Qualcomm Atheros ")))
	{
		const Manage::CPUDB::CPUSpec *cpu = 0;
		i = Text::StrIndexOfChar(sbSysType.ToString() + 17, ' ');
		if (i != INVALID_INDEX)
		{
			sbSysType.ToString()[17 + i] = 0;
		}
//		wprintf(L"Qualcomm Atheros: %ls\r\n", sbSysType.ToString() + 17);
		cpu = GetCPUSpec(sbSysType.ToCString().Substring(17));
		if (cpu)
		{
			return cpu->model;
		}
	}
	else if (sbSysType.StartsWith(UTF8STRC("MT76")))
	{
		const Manage::CPUDB::CPUSpec *cpu = 0;
		cpu = GetCPUSpec(sbSysType.ToCString());
		if (cpu)
		{
			return cpu->model;
		}
	}
	else if (sbHW.GetLength() > 0)
	{
		const Manage::CPUDB::CPUSpec *cpu = 0;
		UOSInt i;
		if (sbHW.StartsWith(UTF8STRC("Qualcomm")))
		{
			if ((i = sbHW.IndexOf(UTF8STRC("MSM"))) != INVALID_INDEX)
			{
				cpu = GetCPUSpec(sbHW.ToCString().Substring(i));
			}
			else if ((i = sbHW.IndexOf(UTF8STRC("SDM"))) != INVALID_INDEX)
			{
				cpu = GetCPUSpec(sbHW.ToCString().Substring(i));
			}
			else if ((i = sbHW.IndexOf(UTF8STRC("APQ"))) != INVALID_INDEX)
			{
				cpu = GetCPUSpec(sbHW.ToCString().Substring(i));
			}
		}
		else if (sbHW.StartsWith(UTF8STRC("MT")) && sbHW.ToString()[2] >= '0' && sbHW.ToString()[2] <= '9')
		{
			cpu = GetCPUSpec(sbHW.ToCString());
		}
		else if (sbHW.StartsWith(UTF8STRC("sun")))
		{
			sbHW.ToUpper();
			cpu = GetCPUSpec(sbHW.ToCString());
		}
		else if (sbHW.StartsWith(UTF8STRC("BCM")))
		{
			if (sbHW.Equals(UTF8STRC("BCM2835")))
			{
				if (cpuPart == 0xd03)
				{
					cpu = GetCPUSpec(CSTR("BCM2837"));
				}
				else if (cpuPart == 0xb76)
				{
					cpu = GetCPUSpec(CSTR("BCM2835"));
				}
			}
		}
		else if (sbHW.Equals(UTF8STRC("RK30board")))
		{
		}
		else if (sbHW.Equals(UTF8STRC("Annapurna Labs Alpine")))
		{
			if (sbModelName.IndexOf(UTF8STRC(" AL314 ")) != INVALID_INDEX)
			{
				return CSTR("AL314");
			}
		}
		else if (sbHW.Equals(UTF8STRC("rda8810")))
		{
			return CSTR("RDA8810");
		}
		else if (sbHW.StartsWith(UTF8STRC("Atmel")))
		{
			if (sbHW.Equals(UTF8STRC("Atmel AT91SAM9G45-EKES")))
			{
				return CSTR("AT91SAM9G45");
			}
			else if (sbHW.Equals(UTF8STRC("Atmel AT91SAM9")))
			{
				return CSTR("AT91SAM9");
			}
			else if (sbHW.Equals(UTF8STRC("Atmel AT91SAM (Device Tree)")))
			{
				return CSTR("AT91SAM");
			}
		}
		else if (sbHW.StartsWith(UTF8STRC("Freescale")))
		{
			if (sbHW.Equals(UTF8STRC("Freescale i.MX6 UltraLite (Device Tree)")))
			{
				return CSTR("IMX6UL");
			}
		}
		else
		{
		}

		if (cpu)
		{
			return cpu->model;
		}
	}
	return CSTR_NULL;
}
