#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/SBFrameDetail.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::DeviceDB::VendorInfo Net::PacketAnalyzerBluetooth::vendorList[] = {
	{0x0000, "Ericsson Technology Licensing"},
	{0x0001, "Nokia Mobile Phones"},
	{0x0002, "Intel Corp."},
	{0x0003, "IBM Corp."},
	{0x0004, "Toshiba Corp."},
	{0x0005, "3Com"},
	{0x0006, "Microsoft"},
	{0x0007, "Lucent"},
	{0x0008, "Motorola"},
	{0x0009, "Infineon Technologies AG"},
	{0x000A, "Cambridge Silicon Radio"},
	{0x000B, "Silicon Wave"},
	{0x000C, "Digianswer A/S"},
	{0x000D, "Texas Instruments Inc."},
	{0x000E, "Parthus Technologies Inc."},
	{0x000F, "Broadcom Corporation"},
	{0x0010, "Mitel Semiconductor"},
	{0x0011, "Widcomm, Inc."},
	{0x0012, "Zeevo, Inc."},
	{0x0013, "Atmel Corporation"},
	{0x0014, "Mitsubishi Electric Corporation"},
	{0x0015, "RTX Telecom A/S"},
	{0x0016, "KC Technology Inc."},
	{0x0017, "Newlogic"},
	{0x0018, "Transilica, Inc."},
	{0x0019, "Rohde & Schwarz GmbH & Co. KG"},
	{0x001A, "TTPCom Limited"},
	{0x001B, "Signia Technologies, Inc."},
	{0x001C, "Conexant Systems Inc."},
	{0x001D, "Qualcomm"},
	{0x001E, "Inventel"},
	{0x001F, "AVM Berlin"},
	{0x0020, "BandSpeed, Inc."},
	{0x0021, "Mansella Ltd"},
	{0x0022, "NEC Corporation"},
	{0x0023, "WavePlus Technology Co., Ltd."},
	{0x0024, "Alcatel"},
	{0x0025, "NXP Semiconductors (formerly Philips Semiconductors)"},
	{0x0026, "C Technologies"},
	{0x0027, "Open Interface"},
	{0x0028, "R F Micro Devices"},
	{0x0029, "Hitachi Ltd"},
	{0x002A, "Symbol Technologies, Inc."},
	{0x002B, "Tenovis"},
	{0x002C, "Macronix International Co. Ltd."},
	{0x002D, "GCT Semiconductor"},
	{0x002E, "Norwood Systems"},
	{0x002F, "MewTel Technology Inc."},
	{0x0030, "ST Microelectronics"},
	{0x0031, "Synopsys, Inc."},
	{0x0032, "Red-M (Communications) Ltd"},
	{0x0033, "Commil Ltd"},
	{0x0034, "Computer Access Technology Corporation (CATC)"},
	{0x0035, "Eclipse (HQ Espana) S.L."},
	{0x0036, "Renesas Electronics Corporation"},
	{0x0037, "Mobilian Corporation"},
	{0x0038, "Terax"},
	{0x0039, "Integrated System Solution Corp."},
	{0x003A, "Matsushita Electric Industrial Co., Ltd."},
	{0x003B, "Gennum Corporation"},
	{0x003C, "BlackBerry Limited (formerly Research In Motion)"},
	{0x003D, "IPextreme, Inc."},
	{0x003E, "Systems and Chips, Inc"},
	{0x003F, "Bluetooth SIG, Inc"},
	{0x0040, "Seiko Epson Corporation"},
	{0x0041, "Integrated Silicon Solution Taiwan, Inc."},
	{0x0042, "CONWISE Technology Corporation Ltd"},
	{0x0043, "PARROT SA"},
	{0x0044, "Socket Mobile"},
	{0x0045, "Atheros Communications, Inc."},
	{0x0046, "MediaTek, Inc."},
	{0x0047, "Bluegiga"},
	{0x0048, "Marvell Technology Group Ltd."},
	{0x0049, "3DSP Corporation"},
	{0x004A, "Accel Semiconductor Ltd."},
	{0x004B, "Continental Automotive Systems"},
	{0x004C, "Apple, Inc."},
	{0x004D, "Staccato Communications, Inc."},
	{0x004E, "Avago Technologies"},
	{0x004F, "APT Ltd."},
	{0x0050, "SiRF Technology, Inc."},
	{0x0051, "Tzero Technologies, Inc."},
	{0x0052, "J&M Corporation"},
	{0x0053, "Free2move AB"},
	{0x0054, "3DiJoy Corporation"},
	{0x0055, "Plantronics, Inc."},
	{0x0056, "Sony Ericsson Mobile Communications"},
	{0x0057, "Harman International Industries, Inc."},
	{0x0058, "Vizio, Inc."},
	{0x0059, "Nordic Semiconductor ASA"},
	{0x005A, "EM Microelectronic-Marin SA"},
	{0x005B, "Ralink Technology Corporation"},
	{0x005C, "Belkin International, Inc."},
	{0x005D, "Realtek Semiconductor Corporation"},
	{0x005E, "Stonestreet One, LLC"},
	{0x005F, "Wicentric, Inc."},
	{0x0060, "RivieraWaves S.A.S"},
	{0x0061, "RDA Microelectronics"},
	{0x0062, "Gibson Guitars"},
	{0x0063, "MiCommand Inc."},
	{0x0064, "Band XI International, LLC"},
	{0x0065, "Hewlett-Packard Company"},
	{0x0066, "9Solutions Oy"},
	{0x0067, "GN Netcom A/S"},
	{0x0068, "General Motors"},
	{0x0069, "A&D Engineering, Inc."},
	{0x006A, "MindTree Ltd."},
	{0x006B, "Polar Electro OY"},
	{0x006C, "Beautiful Enterprise Co., Ltd."},
	{0x006D, "BriarTek, Inc"},
	{0x006E, "Summit Data Communications, Inc."},
	{0x006F, "Sound ID"},
	{0x0070, "Monster, LLC"},
	{0x0071, "connectBlue AB"},
	{0x0072, "ShangHai Super Smart Electronics Co. Ltd."},
	{0x0073, "Group Sense Ltd."},
	{0x0074, "Zomm, LLC"},
	{0x0075, "Samsung Electronics Co. Ltd."},
	{0x0076, "Creative Technology Ltd."},
	{0x0077, "Laird Technologies"},
	{0x0078, "Nike, Inc."},
	{0x0079, "lesswire AG"},
	{0x007A, "MStar Semiconductor, Inc."},
	{0x007B, "Hanlynn Technologies"},
	{0x007C, "A & R Cambridge"},
	{0x007D, "Seers Technology Co., Ltd."},
	{0x007E, "Sports Tracking Technologies Ltd."},
	{0x007F, "Autonet Mobile"},
	{0x0080, "DeLorme Publishing Company, Inc."},
	{0x0081, "WuXi Vimicro"},
	{0x0082, "Sennheiser Communications A/S"},
	{0x0083, "TimeKeeping Systems, Inc."},
	{0x0084, "Ludus Helsinki Ltd."},
	{0x0085, "BlueRadios, Inc."},
	{0x0086, "Equinux AG"},
	{0x0087, "Garmin International, Inc."},
	{0x0088, "Ecotest"},
	{0x0089, "GN ReSound A/S"},
	{0x008A, "Jawbone"},
	{0x008B, "Topcon Positioning Systems, LLC"},
	{0x008C, "Gimbal Inc. (formerly Qualcomm Labs, Inc. and Qualcomm Retail Solutions, Inc.)"},
	{0x008D, "Zscan Software"},
	{0x008E, "Quintic Corp"},
	{0x008F, "Telit Wireless Solutions GmbH (formerly Stollmann E+V GmbH)"},
	{0x0090, "Funai Electric Co., Ltd."},
	{0x0091, "Advanced PANMOBIL systems GmbH & Co. KG"},
	{0x0092, "ThinkOptics, Inc."},
	{0x0093, "Universal Electronics, Inc."},
	{0x0094, "Airoha Technology Corp."},
	{0x0095, "NEC Lighting, Ltd."},
	{0x0096, "ODM Technology, Inc."},
	{0x0097, "ConnecteDevice Ltd."},
	{0x0098, "zero1.tv GmbH"},
	{0x0099, "i.Tech Dynamic Global Distribution Ltd."},
	{0x009A, "Alpwise"},
	{0x009B, "Jiangsu Toppower Automotive Electronics Co., Ltd."},
	{0x009C, "Colorfy, Inc."},
	{0x009D, "Geoforce Inc."},
	{0x009E, "Bose Corporation"},
	{0x009F, "Suunto Oy"},
	{0x00A0, "Kensington Computer Products Group"},
	{0x00A1, "SR-Medizinelektronik"},
	{0x00A2, "Vertu Corporation Limited"},
	{0x00A3, "Meta Watch Ltd."},
	{0x00A4, "LINAK A/S"},
	{0x00A5, "OTL Dynamics LLC"},
	{0x00A6, "Panda Ocean Inc."},
	{0x00A7, "Visteon Corporation"},
	{0x00A8, "ARP Devices Limited"},
	{0x00A9, "Magneti Marelli S.p.A"},
	{0x00AA, "CAEN RFID srl"},
	{0x00AB, "Ingenieur-Systemgruppe Zahn GmbH"},
	{0x00AC, "Green Throttle Games"},
	{0x00AD, "Peter Systemtechnik GmbH"},
	{0x00AE, "Omegawave Oy"},
	{0x00AF, "Cinetix"},
	{0x00B0, "Passif Semiconductor Corp"},
	{0x00B1, "Saris Cycling Group, Inc"},
	{0x00B2, "Bekey A/S"},
	{0x00B3, "Clarinox Technologies Pty. Ltd."},
	{0x00B4, "BDE Technology Co., Ltd."},
	{0x00B5, "Swirl Networks"},
	{0x00B6, "Meso international"},
	{0x00B7, "TreLab Ltd"},
	{0x00B8, "Qualcomm Innovation Center, Inc. (QuIC)"},
	{0x00B9, "Johnson Controls, Inc."},
	{0x00BA, "Starkey Laboratories Inc."},
	{0x00BB, "S-Power Electronics Limited"},
	{0x00BC, "Ace Sensor Inc"},
	{0x00BD, "Aplix Corporation"},
	{0x00BE, "AAMP of America"},
	{0x00BF, "Stalmart Technology Limited"},
	{0x00C0, "AMICCOM Electronics Corporation"},
	{0x00C1, "Shenzhen Excelsecu Data Technology Co.,Ltd"},
	{0x00C2, "Geneq Inc."},
	{0x00C3, "adidas AG"},
	{0x00C4, "LG Electronics"},
	{0x00C5, "Onset Computer Corporation"},
	{0x00C6, "Selfly BV"},
	{0x00C7, "Quuppa Oy."},
	{0x00C8, "GeLo Inc"},
	{0x00C9, "Evluma"},
	{0x00CA, "MC10"},
	{0x00CB, "Binauric SE"},
	{0x00CC, "Beats Electronics"},
	{0x00CD, "Microchip Technology Inc."},
	{0x00CE, "Elgato Systems GmbH"},
	{0x00CF, "ARCHOS SA"},
	{0x00D0, "Dexcom, Inc."},
	{0x00D1, "Polar Electro Europe B.V."},
	{0x00D2, "Dialog Semiconductor B.V."},
	{0x00D3, "Taixingbang Technology (HK) Co,. LTD."},
	{0x00D4, "Kawantech"},
	{0x00D5, "Austco Communication Systems"},
	{0x00D6, "Timex Group USA, Inc."},
	{0x00D7, "Qualcomm Technologies, Inc."},
	{0x00D8, "Qualcomm Connected Experiences, Inc."},
	{0x00D9, "Voyetra Turtle Beach"},
	{0x00DA, "txtr GmbH"},
	{0x00DB, "Biosentronics"},
	{0x00DC, "Procter & Gamble"},
	{0x00DD, "Hosiden Corporation"},
	{0x00DE, "Muzik LLC"},
	{0x00DF, "Misfit Wearables Corp"},
	{0x00E0, "Google"},
	{0x00E1, "Danlers Ltd"},
	{0x00E2, "Semilink Inc"},
	{0x00E3, "inMusic Brands, Inc"},
	{0x00E4, "L.S. Research Inc."},
	{0x00E5, "Eden Software Consultants Ltd."},
	{0x00E6, "Freshtemp"},
	{0x00E7, "KS Technologies"},
	{0x00E8, "ACTS Technologies"},
	{0x00E9, "Vtrack Systems"},
	{0x00EA, "Nielsen-Kellerman Company"},
	{0x00EB, "Server Technology Inc."},
	{0x00EC, "BioResarch Associates"},
	{0x00ED, "Jolly Logic, LLC"},
	{0x00EE, "Above Average Outcomes, Inc."},
	{0x00EF, "Bitsplitters GmbH"},
	{0x00F0, "PayPal, Inc."},
	{0x00F1, "Witron Technology Limited"},
	{0x00F2, "Morse Project Inc."},
	{0x00F3, "Kent Displays Inc."},
	{0x00F4, "Nautilus Inc."},
	{0x00F5, "Smartifier Oy"},
	{0x00F6, "Elcometer Limited"},
	{0x00F7, "VSN Technologies, Inc."},
	{0x00F8, "AceUni Corp., Ltd."},
	{0x00F9, "StickNFind"},
	{0x00FA, "Crystal Code AB"},
	{0x00FB, "KOUKAAM a.s."},
	{0x00FC, "Delphi Corporation"},
	{0x00FD, "ValenceTech Limited"},
	{0x00FE, "Stanley Black and Decker"},
	{0x00FF, "Typo Products, LLC"},
	{0x0100, "TomTom International BV"},
	{0x0101, "Fugoo, Inc."},
	{0x0102, "Keiser Corporation"},
	{0x0103, "Bang & Olufsen S/A"},
	{0x0104, "PLUS Location Systems Pty Ltd"},
	{0x0105, "Ubiquitous Computing Technology Corporation"},
	{0x0106, "Innovative Yachtter Solutions"},
	{0x0107, "William Demant Holding A/S"},
	{0x0108, "Chicony Electronics Co., Ltd."},
	{0x0109, "Atus BV"},
	{0x010A, "Codegate Ltd"},
	{0x010B, "ERi, Inc"},
	{0x010C, "Transducers Direct, LLC"},
	{0x010D, "Fujitsu Ten LImited"},
	{0x010E, "Audi AG"},
	{0x010F, "HiSilicon Technologies Col, Ltd."},
	{0x0110, "Nippon Seiki Co., Ltd."},
	{0x0111, "Steelseries ApS"},
	{0x0112, "Visybl Inc."},
	{0x0113, "Openbrain Technologies, Co., Ltd."},
	{0x0114, "Xensr"},
	{0x0115, "e.solutions"},
	{0x0116, "10AK Technologies"},
	{0x0117, "Wimoto Technologies Inc"},
	{0x0118, "Radius Networks, Inc."},
	{0x0119, "Wize Technology Co., Ltd."},
	{0x011A, "Qualcomm Labs, Inc."},
	{0x011B, "Aruba Networks"},
	{0x011C, "Baidu"},
	{0x011D, "Arendi AG"},
	{0x011E, "Skoda Auto a.s."},
	{0x011F, "Volkwagon AG"},
	{0x0120, "Porsche AG"},
	{0x0121, "Sino Wealth Electronic Ltd."},
	{0x0122, "AirTurn, Inc."},
	{0x0123, "Kinsa, Inc"},
	{0x0124, "HID Global"},
	{0x0125, "SEAT es"},
	{0x0126, "Promethean Ltd."},
	{0x0127, "Salutica Allied Solutions"},
	{0x0128, "GPSI Group Pty Ltd"},
	{0x0129, "Nimble Devices Oy"},
	{0x012A, "Changzhou Yongse Infotech Co., Ltd."},
	{0x012B, "SportIQ"},
	{0x012C, "TEMEC Instruments B.V."},
	{0x012D, "Sony Corporation"},
	{0x012E, "ASSA ABLOY"},
	{0x012F, "Clarion Co. Inc."},
	{0x0130, "Warehouse Innovations"},
	{0x0131, "Cypress Semiconductor"},
	{0x0132, "MADS Inc"},
	{0x0133, "Blue Maestro Limited"},
	{0x0134, "Resolution Products, Ltd."},
	{0x0135, "Aireware LLC"},
	{0x0136, "Seed Labs, Inc. (formerly ETC sp. z.o.o.)"},
	{0x0137, "Prestigio Plaza Ltd."},
	{0x0138, "NTEO Inc."},
	{0x0139, "Focus Systems Corporation"},
	{0x013A, "Tencent Holdings Ltd."},
	{0x013B, "Allegion"},
	{0x013C, "Murata Manufacturing Co., Ltd."},
	{0x013D, "WirelessWERX"},
	{0x013E, "Nod, Inc."},
	{0x013F, "B&B Manufacturing Company"},
	{0x0140, "Alpine Electronics (China) Co., Ltd"},
	{0x0141, "FedEx Services"},
	{0x0142, "Grape Systems Inc."},
	{0x0143, "Bkon Connect"},
	{0x0144, "Lintech GmbH"},
	{0x0145, "Novatel Wireless"},
	{0x0146, "Ciright"},
	{0x0147, "Mighty Cast, Inc."},
	{0x0148, "Ambimat Electronics"},
	{0x0149, "Perytons Ltd."},
	{0x014A, "Tivoli Audio, LLC"},
	{0x014B, "Master Lock"},
	{0x014C, "Mesh-Net Ltd"},
	{0x014D, "HUIZHOU DESAY SV AUTOMOTIVE CO., LTD."},
	{0x014E, "Tangerine, Inc."},
	{0x014F, "B&W Group Ltd."},
	{0x0150, "Pioneer Corporation"},
	{0x0151, "OnBeep"},
	{0x0152, "Vernier Software & Technology"},
	{0x0153, "ROL Ergo"},
	{0x0154, "Pebble Technology"},
	{0x0155, "NETATMO"},
	{0x0156, "Accumulate AB"},
	{0x0157, "Anhui Huami Information Technology Co., Ltd."},
	{0x0158, "Inmite s.r.o."},
	{0x0159, "ChefSteps, Inc."},
	{0x015A, "micas AG"},
	{0x015B, "Biomedical Research Ltd."},
	{0x015C, "Pitius Tec S.L."},
	{0x015D, "Estimote, Inc."},
	{0x015E, "Unikey Technologies, Inc."},
	{0x015F, "Timer Cap Co."},
	{0x0160, "AwoX"},
	{0x0161, "yikes"},
	{0x0162, "MADSGlobalNZ Ltd."},
	{0x0163, "PCH International"},
	{0x0164, "Qingdao Yeelink Information Technology Co., Ltd."},
	{0x0165, "Milwaukee Tool (Formally Milwaukee Electric Tools)"},
	{0x0166, "MISHIK Pte Ltd"},
	{0x0167, "Bayer HealthCare"},
	{0x0168, "Spicebox LLC"},
	{0x0169, "emberlight"},
	{0x016A, "Cooper-Atkins Corporation"},
	{0x016B, "Qblinks"},
	{0x016C, "MYSPHERA"},
	{0x016D, "LifeScan Inc"},
	{0x016E, "Volantic AB"},
	{0x016F, "Podo Labs, Inc"},
	{0x0170, "Roche Diabetes Care AG"},
	{0x0171, "Amazon Fulfillment Service"},
	{0x0172, "Connovate Technology Private Limited"},
	{0x0173, "Kocomojo, LLC"},
	{0x0174, "EveryKey LLC"},
	{0x0175, "Dynamic Controls"},
	{0x0176, "SentriLock"},
	{0x0177, "I-SYST inc."},
	{0x0178, "CASIO COMPUTER CO., LTD."},
	{0x0179, "LAPIS Semiconductor Co., Ltd."},
	{0x017A, "Telemonitor, Inc."},
	{0x017B, "taskit GmbH"},
	{0x017C, "Daimler AG"},
	{0x017D, "BatAndCat"},
	{0x017E, "BluDotz Ltd"},
	{0x017F, "XTel ApS"},
	{0x0180, "Gigaset Communications GmbH"},
	{0x0181, "Gecko Health Innovations, Inc."},
	{0x0182, "HOP Ubiquitous"},
	{0x0183, "Walt Disney"},
	{0x0184, "Nectar"},
	{0x0185, "bel'apps LLC"},
	{0x0186, "CORE Lighting Ltd"},
	{0x0187, "Seraphim Sense Ltd"},
	{0x0188, "Unico RBC"},
	{0x0189, "Physical Enterprises Inc."},
	{0x018A, "Able Trend Technology Limited"},
	{0x018B, "Konica Minolta, Inc."},
	{0x018C, "Wilo SE"},
	{0x018D, "Extron Design Services"},
	{0x018E, "Fitbit, Inc."},
	{0x018F, "Fireflies Systems"},
	{0x0190, "Intelletto Technologies Inc."},
	{0x0191, "FDK CORPORATION"},
	{0x0192, "Cloudleaf, Inc"},
	{0x0193, "Maveric Automation LLC"},
	{0x0194, "Acoustic Stream Corporation"},
	{0x0195, "Zuli"},
	{0x0196, "Paxton Access Ltd"},
	{0x0197, "WiSilica Inc."},
	{0x0198, "VENGIT Korltolt Felelssg Trsasg"},
	{0x0199, "SALTO SYSTEMS S.L."},
	{0x019A, "TRON Forum (formerly T-Engine Forum)"},
	{0x019B, "CUBETECH s.r.o."},
	{0x019C, "Cokiya Incorporated"},
	{0x019D, "CVS Health"},
	{0x019E, "Ceruus"},
	{0x019F, "Strainstall Ltd"},
	{0x01A0, "Channel Enterprises (HK) Ltd."},
	{0x01A1, "FIAMM"},
	{0x01A2, "GIGALANE.CO.,LTD"},
	{0x01A3, "EROAD"},
	{0x01A4, "Mine Safety Appliances"},
	{0x01A5, "Icon Health and Fitness"},
	{0x01A6, "Asandoo GmbH"},
	{0x01A7, "ENERGOUS CORPORATION"},
	{0x01A8, "Taobao"},
	{0x01A9, "Canon Inc."},
	{0x01AA, "Geophysical Technology Inc."},
	{0x01AB, "Facebook, Inc."},
	{0x01AC, "Nipro Diagnostics, Inc."},
	{0x01AD, "FlightSafety International"},
	{0x01AE, "Earlens Corporation"},
	{0x01AF, "Sunrise Micro Devices, Inc."},
	{0x01B0, "Star Micronics Co., Ltd."},
	{0x01B1, "Netizens Sp. z o.o."},
	{0x01B2, "Nymi Inc."},
	{0x01B3, "Nytec, Inc."},
	{0x01B4, "Trineo Sp. z o.o."},
	{0x01B5, "Nest Labs Inc."},
	{0x01B6, "LM Technologies Ltd"},
	{0x01B7, "General Electric Company"},
	{0x01B8, "i+D3 S.L."},
	{0x01B9, "HANA Micron"},
	{0x01BA, "Stages Cycling LLC"},
	{0x01BB, "Cochlear Bone Anchored Solutions AB"},
	{0x01BC, "SenionLab AB"},
	{0x01BD, "Syszone Co., Ltd"},
	{0x01BE, "Pulsate Mobile Ltd."},
	{0x01BF, "Hong Kong HunterSun Electronic Limited"},
	{0x01C0, "pironex GmbH"},
	{0x01C1, "BRADATECH Corp."},
	{0x01C2, "Transenergooil AG"},
	{0x01C3, "Bunch"},
	{0x01C4, "DME Microelectronics"},
	{0x01C5, "Bitcraze AB"},
	{0x01C6, "HASWARE Inc."},
	{0x01C7, "Abiogenix Inc."},
	{0x01C8, "Poly-Control ApS"},
	{0x01C9, "Avi-on"},
	{0x01CA, "Laerdal Medical AS"},
	{0x01CB, "Fetch My Pet"},
	{0x01CC, "Sam Labs Ltd."},
	{0x01CD, "Chengdu Synwing Technology Ltd"},
	{0x01CE, "HOUWA SYSTEM DESIGN, k.k."},
	{0x01CF, "BSH"},
	{0x01D0, "Primus Inter Pares Ltd"},
	{0x01D1, "August Home, Inc"},
	{0x01D2, "Gill Electronics"},
	{0x01D3, "Sky Wave Design"},
	{0x01D4, "Newlab S.r.l."},
	{0x01D5, "ELAD srl"},
	{0x01D6, "G-wearables inc."},
	{0x01D7, "Squadrone Systems Inc."},
	{0x01D8, "Code Corporation"},
	{0x01D9, "Savant Systems LLC"},
	{0x01DA, "Logitech International SA"},
	{0x01DB, "Innblue Consulting"},
	{0x01DC, "iParking Ltd."},
	{0x01DD, "Koninklijke Philips Electronics N.V."},
	{0x01DE, "Minelab Electronics Pty Limited"},
	{0x01DF, "Bison Group Ltd."},
	{0x01E0, "Widex A/S"},
	{0x01E1, "Jolla Ltd"},
	{0x01E2, "Lectronix, Inc."},
	{0x01E3, "Caterpillar Inc"},
	{0x01E4, "Freedom Innovations"},
	{0x01E5, "Dynamic Devices Ltd"},
	{0x01E6, "Technology Solutions (UK) Ltd"},
	{0x01E7, "IPS Group Inc."},
	{0x01E8, "STIR"},
	{0x01E9, "Sano, Inc."},
	{0x01EA, "Advanced Application Design, Inc."},
	{0x01EB, "AutoMap LLC"},
	{0x01EC, "Spreadtrum Communications Shanghai Ltd"},
	{0x01ED, "CuteCircuit LTD"},
	{0x01EE, "Valeo Service"},
	{0x01EF, "Fullpower Technologies, Inc."},
	{0x01F0, "KloudNation"},
	{0x01F1, "Zebra Technologies Corporation"},
	{0x01F2, "Itron, Inc."},
	{0x01F3, "The University of Tokyo"},
	{0x01F4, "UTC Fire and Security"},
	{0x01F5, "Cool Webthings Limited"},
	{0x01F6, "DJO Global"},
	{0x01F7, "Gelliner Limited"},
	{0x01F8, "Anyka (Guangzhou) Microelectronics Technology Co, LTD"},
	{0x01F9, "Medtronic Inc."},
	{0x01FA, "Gozio Inc."},
	{0x01FB, "Form Lifting, LLC"},
	{0x01FC, "Wahoo Fitness, LLC"},
	{0x01FD, "Kontakt Micro-Location Sp. z o.o."},
	{0x01FE, "Radio Systems Corporation"},
	{0x01FF, "Freescale Semiconductor, Inc."},
	{0x0200, "Verifone Systems Pte Ltd. Taiwan Branch"},
	{0x0201, "AR Timing"},
	{0x0202, "Rigado LLC"},
	{0x0203, "Kemppi Oy"},
	{0x0204, "Tapcentive Inc."},
	{0x0205, "Smartbotics Inc."},
	{0x0206, "Otter Products, LLC"},
	{0x0207, "STEMP Inc."},
	{0x0208, "LumiGeek LLC"},
	{0x0209, "InvisionHeart Inc."},
	{0x020A, "Macnica Inc."},
	{0x020B, "Jaguar Land Rover Limited"},
	{0x020C, "CoroWare Technologies, Inc"},
	{0x020D, "Simplo Technology Co., LTD"},
	{0x020E, "Omron Healthcare Co., LTD"},
	{0x020F, "Comodule GMBH"},
	{0x0210, "ikeGPS"},
	{0x0211, "Telink Semiconductor Co. Ltd"},
	{0x0212, "Interplan Co., Ltd"},
	{0x0213, "Wyler AG"},
	{0x0214, "IK Multimedia Production srl"},
	{0x0215, "Lukoton Experience Oy"},
	{0x0216, "MTI Ltd"},
	{0x0217, "Tech4home, Lda"},
	{0x0218, "Hiotech AB"},
	{0x0219, "DOTT Limited"},
	{0x021A, "Blue Speck Labs, LLC"},
	{0x021B, "Cisco Systems, Inc"},
	{0x021C, "Mobicomm Inc"},
	{0x021D, "Edamic"},
	{0x021E, "Goodnet, Ltd"},
	{0x021F, "Luster Leaf Products Inc"},
	{0x0220, "Manus Machina BV"},
	{0x0221, "Mobiquity Networks Inc"},
	{0x0222, "Praxis Dynamics"},
	{0x0223, "Philip Morris Products S.A."},
	{0x0224, "Comarch SA"},
	{0x0225, "Nestl Nespresso S.A."},
	{0x0226, "Merlinia A/S"},
	{0x0227, "LifeBEAM Technologies"},
	{0x0228, "Twocanoes Labs, LLC"},
	{0x0229, "Muoverti Limited"},
	{0x022A, "Stamer Musikanlagen GMBH"},
	{0x022B, "Tesla Motors"},
	{0x022C, "Pharynks Corporation"},
	{0x022D, "Lupine"},
	{0x022E, "Siemens AG"},
	{0x022F, "Huami (Shanghai) Culture Communication CO., LTD"},
	{0x0230, "Foster Electric Company, Ltd"},
	{0x0231, "ETA SA"},
	{0x0232, "x-Senso Solutions Kft"},
	{0x0233, "Shenzhen SuLong Communication Ltd"},
	{0x0234, "FengFan (BeiJing) Technology Co, Ltd"},
	{0x0235, "Qrio Inc"},
	{0x0236, "Pitpatpet Ltd"},
	{0x0237, "MSHeli s.r.l."},
	{0x0238, "Trakm8 Ltd"},
	{0x0239, "JIN CO, Ltd"},
	{0x023A, "Alatech Tehnology"},
	{0x023B, "Beijing CarePulse Electronic Technology Co, Ltd"},
	{0x023C, "Awarepoint"},
	{0x023D, "ViCentra B.V."},
	{0x023E, "Raven Industries"},
	{0x023F, "WaveWare Technologies Inc."},
	{0x0240, "Argenox Technologies"},
	{0x0241, "Bragi GmbH"},
	{0x0242, "16Lab Inc"},
	{0x0243, "Masimo Corp"},
	{0x0244, "Iotera Inc"},
	{0x0245, "Endress+Hauser"},
	{0x0246, "ACKme Networks, Inc."},
	{0x0247, "FiftyThree Inc."},
	{0x0248, "Parker Hannifin Corp"},
	{0x0249, "Transcranial Ltd"},
	{0x024A, "Uwatec AG"},
	{0x024B, "Orlan LLC"},
	{0x024C, "Blue Clover Devices"},
	{0x024D, "M-Way Solutions GmbH"},
	{0x024E, "Microtronics Engineering GmbH"},
	{0x024F, "Schneider Schreibgerte GmbH"},
	{0x0250, "Sapphire Circuits LLC"},
	{0x0251, "Lumo Bodytech Inc."},
	{0x0252, "UKC Technosolution"},
	{0x0253, "Xicato Inc."},
	{0x0254, "Playbrush"},
	{0x0255, "Dai Nippon Printing Co., Ltd."},
	{0x0256, "G24 Power Limited"},
	{0x0257, "AdBabble Local Commerce Inc."},
	{0x0258, "Devialet SA"},
	{0x0259, "ALTYOR"},
	{0x025A, "University of Applied Sciences Valais/Haute Ecole Valaisanne"},
	{0x025B, "Five Interactive, LLC dba Zendo"},
	{0x025C, "NetEaseHangzhouNetwork co.Ltd."},
	{0x025D, "Lexmark International Inc."},
	{0x025E, "Fluke Corporation"},
	{0x025F, "Yardarm Technologies"},
	{0x0260, "SensaRx"},
	{0x0261, "SECVRE GmbH"},
	{0x0262, "Glacial Ridge Technologies"},
	{0x0263, "Identiv, Inc."},
	{0x0264, "DDS, Inc."},
	{0x0265, "SMK Corporation"},
	{0x0266, "Schawbel Technologies LLC"},
	{0x0267, "XMI Systems SA"},
	{0x0268, "Cerevo"},
	{0x0269, "Torrox GmbH & Co KG"},
	{0x026A, "Gemalto"},
	{0x026B, "DEKA Research & Development Corp."},
	{0x026C, "Domster Tadeusz Szydlowski"},
	{0x026D, "Technogym SPA"},
	{0x026E, "FLEURBAEY BVBA"},
	{0x026F, "Aptcode Solutions"},
	{0x0270, "LSI ADL Technology"},
	{0x0271, "Animas Corp"},
	{0x0272, "Alps Electric Co., Ltd."},
	{0x0273, "OCEASOFT"},
	{0x0274, "Motsai Research"},
	{0x0275, "Geotab"},
	{0x0276, "E.G.O. Elektro-Gertebau GmbH"},
	{0x0277, "bewhere inc"},
	{0x0278, "Johnson Outdoors Inc"},
	{0x0279, "steute Schaltgerate GmbH & Co. KG"},
	{0x027A, "Ekomini inc."},
	{0x027B, "DEFA AS"},
	{0x027C, "Aseptika Ltd"},
	{0x027D, "HUAWEI Technologies Co., Ltd. ( )"},
	{0x027E, "HabitAware, LLC"},
	{0x027F, "ruwido austria gmbh"},
	{0x0280, "ITEC corporation"},
	{0x0281, "StoneL"},
	{0x0282, "Sonova AG"},
	{0x0283, "Maven Machines, Inc."},
	{0x0284, "Synapse Electronics"},
	{0x0285, "Standard Innovation Inc."},
	{0x0286, "RF Code, Inc."},
	{0x0287, "Wally Ventures S.L."},
	{0x0288, "Willowbank Electronics Ltd"},
	{0x0289, "SK Telecom"},
	{0x028A, "Jetro AS"},
	{0x028B, "Code Gears LTD"},
	{0x028C, "NANOLINK APS"},
	{0x028D, "IF, LLC"},
	{0x028E, "RF Digital Corp"},
	{0x028F, "Church & Dwight Co., Inc"},
	{0x0290, "Multibit Oy"},
	{0x0291, "CliniCloud Inc"},
	{0x0292, "SwiftSensors"},
	{0x0293, "Blue Bite"},
	{0x0294, "ELIAS GmbH"},
	{0x0295, "Sivantos GmbH"},
	{0x0296, "Petzl"},
	{0x0297, "storm power ltd"},
	{0x0298, "EISST Ltd"},
	{0x0299, "Inexess Technology Simma KG"},
	{0x029A, "Currant, Inc."},
	{0x029B, "C2 Development, Inc."},
	{0x029C, "Blue Sky Scientific, LLC"},
	{0x029D, "ALOTTAZS LABS, LLC"},
	{0x029E, "Kupson spol. s r.o."},
	{0x029F, "Areus Engineering GmbH"},
	{0x02A0, "Impossible Camera GmbH"},
	{0x02A1, "InventureTrack Systems"},
	{0x02A2, "LockedUp"},
	{0x02A3, "Itude"},
	{0x02A4, "Pacific Lock Company"},
	{0x02A5, "Tendyron Corporation ( )"},
	{0x02A6, "Robert Bosch GmbH"},
	{0x02A7, "Illuxtron international B.V."},
	{0x02A8, "miSport Ltd."},
	{0x02A9, "Chargelib"},
	{0x02AA, "Doppler Lab"},
	{0x02AB, "BBPOS Limited"},
	{0x02AC, "RTB Elektronik GmbH & Co. KG"},
	{0x02AD, "Rx Networks, Inc."},
	{0x02AE, "WeatherFlow, Inc."},
	{0x02AF, "Technicolor USA Inc."},
	{0x02B0, "Bestechnic(Shanghai),Ltd"},
	{0x02B1, "Raden Inc"},
	{0x02B2, "JouZen Oy"},
	{0x02B3, "CLABER S.P.A."},
	{0x02B4, "Hyginex, Inc."},
	{0x02B5, "HANSHIN ELECTRIC RAILWAY CO.,LTD."},
	{0x02B6, "Schneider Electric"},
	{0x02B7, "Oort Technologies LLC"},
	{0x02B8, "Chrono Therapeutics"},
	{0x02B9, "Rinnai Corporation"},
	{0x02BA, "Swissprime Technologies AG"},
	{0x02BB, "Koha.,Co.Ltd"},
	{0x02BC, "Genevac Ltd"},
	{0x02BD, "Chemtronics"},
	{0x02BE, "Seguro Technology Sp. z o.o."},
	{0x02BF, "Redbird Flight Simulations"},
	{0x02C0, "Dash Robotics"},
	{0x02C1, "LINE Corporation"},
	{0x02C2, "Guillemot Corporation"},
	{0x02C3, "Techtronic Power Tools Technology Limited"},
	{0x02C4, "Wilson Sporting Goods"},
	{0x02C5, "Lenovo (Singapore) Pte Ltd. ( )"},
	{0x02C6, "Ayatan Sensors"},
	{0x02C7, "Electronics Tomorrow Limited"},
	{0x02C8, "VASCO Data Security International, Inc."},
	{0x02C9, "PayRange Inc."},
	{0x02CA, "ABOV Semiconductor"},
	{0x02CB, "AINA-Wireless Inc."},
	{0x02CC, "Eijkelkamp Soil & Water"},
	{0x02CD, "BMA ergonomics b.v."},
	{0x02CE, "Teva Branded Pharmaceutical Products R&D, Inc."},
	{0x02CF, "Anima"},
	{0x02D0, "3M"},
	{0x02D1, "Empatica Srl"},
	{0x02D2, "Afero, Inc."},
	{0x02D3, "Powercast Corporation"},
	{0x02D4, "Secuyou ApS"},
	{0x02D5, "OMRON Corporation"},
	{0x02D6, "Send Solutions"},
	{0x02D7, "NIPPON SYSTEMWARE CO.,LTD."},
	{0x02D8, "Neosfar"},
	{0x02D9, "Fliegl Agrartechnik GmbH"},
	{0x02DA, "Gilvader"},
	{0x02DB, "Digi International Inc (R)"},
	{0x02DC, "DeWalch Technologies, Inc."},
	{0x02DD, "Flint Rehabilitation Devices, LLC"},
	{0x02DE, "Samsung SDS Co., Ltd."},
	{0x02DF, "Blur Product Development"},
	{0x02E0, "University of Michigan"},
	{0x02E1, "Victron Energy BV"},
	{0x02E2, "NTT docomo"},
	{0x02E3, "Carmanah Technologies Corp."},
	{0x02E4, "Bytestorm Ltd."},
	{0x02E5, "Espressif Incorporated ( () )"},
	{0x02E6, "Unwire"},
	{0x02E7, "Connected Yard, Inc."},
	{0x02E8, "American Music Environments"},
	{0x02E9, "Sensogram Technologies, Inc."},
	{0x02EA, "Fujitsu Limited"},
	{0x02EB, "Ardic Technology"},
	{0x02EC, "Delta Systems, Inc"},
	{0x02ED, "HTC Corporation"},
	{0x02EE, "Citizen Holdings Co., Ltd."},
	{0x02EF, "SMART-INNOVATION.inc"},
	{0x02F0, "Blackrat Software"},
	{0x02F1, "The Idea Cave, LLC"},
	{0x02F2, "GoPro, Inc."},
	{0x02F3, "AuthAir, Inc"},
	{0x02F4, "Vensi, Inc."},
	{0x02F5, "Indagem Tech LLC"},
	{0x02F6, "Intemo Technologies"},
	{0x02F7, "DreamVisions co., Ltd."},
	{0x02F8, "Runteq Oy Ltd"},
	{0x02F9, "IMAGINATION TECHNOLOGIES LTD"},
	{0x02FA, "CoSTAR TEchnologies"},
	{0x02FB, "Clarius Mobile Health Corp."},
	{0x02FC, "Shanghai Frequen Microelectronics Co., Ltd."},
	{0x02FD, "Uwanna, Inc."},
	{0x02FE, "Lierda Science & Technology Group Co., Ltd."},
	{0x02FF, "Silicon Laboratories"},
	{0x0300, "World Moto Inc."},
	{0x0301, "Giatec Scientific Inc."},
	{0x0302, "Loop Devices, Inc"},
	{0x0303, "IACA electronique"},
	{0x0304, "Martians Inc"},
	{0x0305, "Swipp ApS"},
	{0x0306, "Life Laboratory Inc."},
	{0x0307, "FUJI INDUSTRIAL CO.,LTD."},
	{0x0308, "Surefire, LLC"},
	{0x0309, "Dolby Labs"},
	{0x030A, "Ellisys"},
	{0x030B, "Magnitude Lighting Converters"},
	{0x030C, "Hilti AG"},
	{0x030D, "Devdata S.r.l."},
	{0x030E, "Deviceworx"},
	{0x030F, "Shortcut Labs"},
	{0x0310, "SGL Italia S.r.l."},
	{0x0311, "PEEQ DATA"},
	{0x0312, "Ducere Technologies Pvt Ltd"},
	{0x0313, "DiveNav, Inc."},
	{0x0314, "RIIG AI Sp. z o.o."},
	{0x0315, "Thermo Fisher Scientific"},
	{0x0316, "AG Measurematics Pvt. Ltd."},
	{0x0317, "CHUO Electronics CO., LTD."},
	{0x0318, "Aspenta International"},
	{0x0319, "Eugster Frismag AG"},
	{0x031A, "Amber wireless GmbH"},
	{0x031B, "HQ Inc"},
	{0x031C, "Lab Sensor Solutions"},
	{0x031D, "Enterlab ApS"},
	{0x031E, "Eyefi, Inc."},
	{0x031F, "MetaSystem S.p.A."},
	{0x0320, "SONO ELECTRONICS. CO., LTD"},
	{0x0321, "Jewelbots"},
	{0x0322, "Compumedics Limited"},
	{0x0323, "Rotor Bike Components"},
	{0x0324, "Astro, Inc."},
	{0x0325, "Amotus Solutions"},
	{0x0326, "Healthwear Technologies (Changzhou)Ltd"},
	{0x0327, "Essex Electronics"},
	{0x0328, "Grundfos A/S"},
	{0x0329, "Eargo, Inc."},
	{0x032A, "Electronic Design Lab"},
	{0x032B, "ESYLUX"},
	{0x032C, "NIPPON SMT.CO.,Ltd"},
	{0x032D, "BM innovations GmbH"},
	{0x032E, "indoormap"},
	{0x032F, "OttoQ Inc"},
	{0x0330, "North Pole Engineering"},
	{0x0331, "3flares Technologies Inc."},
	{0x0332, "Electrocompaniet A.S."},
	{0x0333, "Mul-T-Lock"},
	{0x0334, "Corentium AS"},
	{0x0335, "Enlighted Inc"},
	{0x0336, "GISTIC"},
	{0x0337, "AJP2 Holdings, LLC"},
	{0x0338, "COBI GmbH"},
	{0x0339, "Blue Sky Scientific, LLC"},
	{0x033A, "Appception, Inc."},
	{0x033B, "Courtney Thorne Limited"},
	{0x033C, "Virtuosys"},
	{0x033D, "TPV Technology Limited"},
	{0x033E, "Monitra SA"},
	{0x033F, "Automation Components, Inc."},
	{0x0340, "Letsense s.r.l."},
	{0x0341, "Etesian Technologies LLC"},
	{0x0342, "GERTEC BRASIL LTDA."},
	{0x0343, "Drekker Development Pty. Ltd."},
	{0x0344, "Whirl Inc"},
	{0x0345, "Locus Positioning"},
	{0x0346, "Acuity Brands Lighting, Inc"},
	{0x0347, "Prevent Biometrics"},
	{0x0348, "Arioneo"},
	{0x0349, "VersaMe"},
	{0x034A, "Vaddio"},
	{0x034B, "Libratone A/S"},
	{0x034C, "HM Electronics, Inc."},
	{0x034D, "TASER International, Inc."},
	{0x034E, "SafeTrust Inc."},
	{0x034F, "Heartland Payment Systems"},
	{0x0350, "Bitstrata Systems Inc."},
	{0x0351, "Pieps GmbH"},
	{0x0352, "iRiding(Xiamen)Technology Co.,Ltd."},
	{0x0353, "Alpha Audiotronics, Inc."},
	{0x0354, "TOPPAN FORMS CO.,LTD."},
	{0x0355, "Sigma Designs, Inc."},
	{0x0356, "Spectrum Brands, Inc."},
	{0x0357, "Polymap Wireless"},
	{0x0358, "MagniWare Ltd."},
	{0x0359, "Novotec Medical GmbH"},
	{0x035A, "Medicom Innovation Partner a/s"},
	{0x035B, "Matrix Inc."},
	{0x035C, "Eaton Corporation"},
	{0x035D, "KYS"},
	{0x035E, "Naya Health, Inc."},
	{0x035F, "Acromag"},
	{0x0360, "Insulet Corporation"},
	{0x0361, "Wellinks Inc."},
	{0x0362, "ON Semiconductor"},
	{0x0363, "FREELAP SA"},
	{0x0364, "Favero Electronics Srl"},
	{0x0365, "BioMech Sensor LLC"},
	{0x0366, "BOLTT Sports technologies Private limited"},
	{0x0367, "Saphe International"},
	{0x0368, "Metormote AB"},
	{0x0369, "littleBits"},
	{0x036A, "SetPoint Medical"},
	{0x036B, "BRControls Products BV"},
	{0x036C, "Zipcar"},
	{0x036D, "AirBolt Pty Ltd"},
	{0x036E, "KeepTruckin Inc"},
	{0x036F, "Motiv, Inc."},
	{0x0370, "Wazombi Labs O"},
	{0x0371, "ORBCOMM"},
	{0x0372, "Nixie Labs, Inc."},
	{0x0373, "AppNearMe Ltd"},
	{0x0374, "Holman Industries"},
	{0x0375, "Expain AS"},
	{0x0376, "Electronic Temperature Instruments Ltd"},
	{0x0377, "Plejd AB"},
	{0x0378, "Propeller Health"},
	{0x0379, "Shenzhen iMCO Electronic Technology Co.,Ltd"},
	{0x037A, "Algoria"},
	{0x037B, "Apption Labs Inc."},
	{0x037C, "Cronologics Corporation"},
	{0x037D, "MICRODIA Ltd."},
	{0x037E, "lulabytes S.L."},
	{0x037F, "Nestec S.A."},
	{0x0380, "LLC MEGA - F service"},
	{0x0381, "Sharp Corporation"},
	{0x0382, "Precision Outcomes Ltd"},
	{0x0383, "Kronos Incorporated"},
	{0x0384, "OCOSMOS Co., Ltd."},
	{0x0385, "Embedded Electronic Solutions Ltd. dba e2Solutions"},
	{0x0386, "Aterica Inc."},
	{0x0387, "BluStor PMC, Inc."},
	{0x0388, "Kapsch TrafficCom AB"},
	{0x0389, "ActiveBlu Corporation"},
	{0x038A, "Kohler Mira Limited"},
	{0x038B, "Noke"},
	{0x038C, "Appion Inc."},
	{0x038D, "Resmed Ltd"},
	{0x038E, "Crownstone B.V."},
	{0x038F, "Xiaomi Inc."},
	{0x0390, "INFOTECH s.r.o."},
	{0x0391, "Thingsquare AB"},
	{0x0392, "T&D"},
	{0x0393, "LAVAZZA S.p.A."},
	{0x0394, "Netclearance Systems, Inc."},
	{0x0395, "SDATAWAY"},
	{0x0396, "BLOKS GmbH"},
	{0x0397, "LEGO System A/S"},
	{0x0398, "Thetatronics Ltd"},
	{0x0399, "Nikon Corporation"},
	{0x039A, "NeST"},
	{0x039B, "South Silicon Valley Microelectronics"},
	{0x039C, "ALE International"},
	{0x039D, "CareView Communications, Inc."},
	{0x039E, "SchoolBoard Limited"},
	{0x039F, "Molex Corporation"},
	{0x03A0, "IVT Wireless Limited"},
	{0x03A1, "Alpine Labs LLC"},
	{0x03A2, "Candura Instruments"},
	{0x03A3, "SmartMovt Technology Co., Ltd"},
	{0x03A4, "Token Zero Ltd"},
	{0x03A5, "ACE CAD Enterprise Co., Ltd. (ACECAD)"},
	{0x03A6, "Medela, Inc"},
	{0x03A7, "AeroScout"},
	{0x03A8, "Esrille Inc."},
	{0x03A9, "THINKERLY SRL"},
	{0x03AA, "Exon Sp. z o.o."},
	{0x03AB, "Meizu Technology Co., Ltd."},
	{0x03AC, "Smablo LTD"},
	{0x03AD, "XiQ"},
	{0x03AE, "Allswell Inc."},
	{0x03AF, "Comm-N-Sense Corp DBA Verigo"},
	{0x03B0, "VIBRADORM GmbH"},
	{0x03B1, "Otodata Wireless Network Inc."},
	{0x03B2, "Propagation Systems Limited"},
	{0x03B3, "Midwest Instruments & Controls"},
	{0x03B4, "Alpha Nodus, inc."},
	{0x03B5, "petPOMM, Inc"},
	{0x03B6, "Mattel"},
	{0x03B7, "Airbly Inc."},
	{0x03B8, "A-Safe Limited"},
	{0x03B9, "FREDERIQUE CONSTANT SA"},
	{0x03BA, "Maxscend Microelectronics Company Limited"},
	{0x03BB, "Abbott Diabetes Care"},
	{0x03BC, "ASB Bank Ltd"},
	{0x03BD, "amadas"},
	{0x03BE, "Applied Science, Inc."},
	{0x03BF, "iLumi Solutions Inc."},
	{0x03C0, "Arch Systems Inc."},
	{0x03C1, "Ember Technologies, Inc."},
	{0x03C2, "Snapchat Inc"},
	{0x03C3, "Casambi Technologies Oy"},
	{0x03C4, "Pico Technology Inc."},
	{0x03C5, "St. Jude Medical, Inc."},
	{0x03C6, "Intricon"},
	{0x03C7, "Structural Health Systems, Inc."},
	{0x03C8, "Avvel International"},
	{0x03C9, "Gallagher Group"},
	{0x03CA, "In2things Automation Pvt. Ltd."},
	{0x03CB, "SYSDEV Srl"},
	{0x03CC, "Vonkil Technologies Ltd"},
	{0x03CD, "Wynd Technologies, Inc."},
	{0x03CE, "CONTRINEX S.A."},
	{0x03CF, "MIRA, Inc."},
	{0x03D0, "Watteam Ltd"},
	{0x03D1, "Density Inc."},
	{0x03D2, "IOT Pot India Private Limited"},
	{0x03D3, "Sigma Connectivity AB"},
	{0x03D4, "PEG PEREGO SPA"},
	{0x03D5, "Wyzelink Systems Inc."},
	{0x03D6, "Yota Devices LTD"},
	{0x03D7, "FINSECUR"},
	{0x03D8, "Zen-Me Labs Ltd"},
	{0x03D9, "3IWare Co., Ltd."},
	{0x03DA, "EnOcean GmbH"},
	{0x03DB, "Instabeat, Inc"},
	{0x03DC, "Nima Labs"},
	{0x03DD, "Andreas Stihl AG & Co. KG"},
	{0x03DE, "Nathan Rhoades LLC"},
	{0x03DF, "Grob Technologies, LLC"},
	{0x03E0, "Actions (Zhuhai) Technology Co., Limited"},
	{0x03E1, "SPD Development Company Ltd"},
	{0x03E2, "Sensoan Oy"},
	{0x03E3, "Qualcomm Life Inc"},
	{0x03E4, "Chip-ing AG"},
	{0x03E5, "ffly4u"},
	{0x03E6, "IoT Instruments Oy"},
	{0x03E7, "TRUE Fitness Technology"},
	{0x03E8, "Reiner Kartengeraete GmbH & Co. KG."},
	{0x03E9, "SHENZHEN LEMONJOY TECHNOLOGY CO., LTD."},
	{0x03EA, "Hello Inc."},
	{0x03EB, "Evollve Inc."},
	{0x03EC, "Jigowatts Inc."},
	{0x03ED, "BASIC MICRO.COM,INC."},
	{0x03EE, "CUBE TECHNOLOGIES"},
	{0x03EF, "foolography GmbH"},
	{0x03F0, "CLINK"},
	{0x03F1, "Hestan Smart Cooking Inc."},
	{0x03F2, "WindowMaster A/S"},
	{0x03F3, "Flowscape AB"},
	{0x03F4, "PAL Technologies Ltd"},
	{0x03F5, "WHERE, Inc."},
	{0x03F6, "Iton Technology Corp."},
	{0x03F7, "Owl Labs Inc."},
	{0x03F8, "Rockford Corp."},
	{0x03F9, "Becon Technologies Co.,Ltd."},
	{0x03FA, "Vyassoft Technologies Inc"},
	{0x03FB, "Nox Medical"},
	{0x03FC, "Kimberly-Clark"},
	{0x03FD, "Trimble Navigation Ltd."},
	{0x03FE, "Littelfuse"},
	{0x03FF, "Withings"},
	{0x0400, "i-developer IT Beratung UG"},
	{0x0402, "Sears Holdings Corporation"},
	{0x0403, "Gantner Electronic GmbH"},
	{0x0404, "Authomate Inc"},
	{0x0405, "Vertex International, Inc."},
	{0x0406, "Airtago"},
	{0x0407, "Swiss Audio SA"},
	{0x0408, "ToGetHome Inc."},
	{0x0409, "AXIS"},
	{0x040A, "Openmatics"},
	{0x040B, "Jana Care Inc."},
	{0x040C, "Senix Corporation"},
	{0x040D, "NorthStar Battery Company, LLC"},
	{0x040E, "SKF (U.K.) Limited"},
	{0x040F, "CO-AX Technology, Inc."},
	{0x0410, "Fender Musical Instruments"},
	{0x0411, "Luidia Inc"},
	{0x0412, "SEFAM"},
	{0x0413, "Wireless Cables Inc"},
	{0x0414, "Lightning Protection International Pty Ltd"},
	{0x0415, "Uber Technologies Inc"},
	{0x0416, "SODA GmbH"},
	{0x0417, "Fatigue Science"},
	{0x0418, "Alpine Electronics Inc."},
	{0x0419, "Novalogy LTD"},
	{0x041A, "Friday Labs Limited"},
	{0x041B, "OrthoAccel Technologies"},
	{0x041C, "WaterGuru, Inc."},
	{0x041D, "Benning Elektrotechnik und Elektronik GmbH & Co. KG"},
	{0x041E, "Dell Computer Corporation"},
	{0x041F, "Kopin Corporation"},
	{0x0420, "TecBakery GmbH"},
	{0x0421, "Backbone Labs, Inc."},
	{0x0422, "DELSEY SA"},
	{0x0423, "Chargifi Limited"},
	{0x0424, "Trainesense Ltd."},
	{0x0425, "Unify Software and Solutions GmbH & Co. KG"},
	{0x0426, "Husqvarna AB"},
	{0x0427, "Focus fleet and fuel management inc"},
	{0x0428, "SmallLoop, LLC"},
	{0x0429, "Prolon Inc."},
	{0x042A, "BD Medical"},
	{0x042B, "iMicroMed Incorporated"},
	{0x042C, "Ticto N.V."},
	{0x042D, "Meshtech AS"},
	{0x042E, "MemCachier Inc."},
	{0x042F, "Danfoss A/S"},
	{0x0430, "SnapStyk Inc."},
	{0x0431, "Amyway Corporation"},
	{0x0432, "Silk Labs, Inc."},
	{0x0433, "Pillsy Inc."},
	{0x0434, "Hatch Baby, Inc."},
	{0x0435, "Blocks Wearables Ltd."},
	{0x0436, "Drayson Technologies (Europe) Limited"},
	{0x0437, "eBest IOT Inc."},
	{0x0438, "Helvar Ltd"},
	{0x0439, "Radiance Technologies"},
	{0x043A, "Nuheara Limited"},
	{0x043B, "Appside co., ltd."},
	{0x043C, "DeLaval"},
	{0x043D, "Coiler Corporation"},
	{0x043E, "Thermomedics, Inc."},
	{0x043F, "Tentacle Sync GmbH"},
	{0x0440, "Valencell, Inc."},
	{0x0441, "iProtoXi Oy"},
	{0x0442, "SECOM CO., LTD."},
	{0x0443, "Tucker International LLC"},
	{0x0444, "Metanate Limited"},
	{0x0445, "Kobian Canada Inc."},
	{0x0446, "NETGEAR, Inc."},
	{0x0447, "Fabtronics Australia Pty Ltd"},
	{0x0448, "Grand Centrix GmbH"},
	{0x0449, "1UP USA.com llc"},
	{0x044A, "SHIMANO INC."},
	{0x044B, "Nain Inc."},
	{0x044C, "LifeStyle Lock, LLC"},
	{0x044D, "VEGA Grieshaber KG"},
	{0x044E, "Xtrava Inc."},
	{0x044F, "TTS Tooltechnic Systems AG & Co. KG"},
	{0x0450, "Teenage Engineering AB"},
	{0x0451, "Tunstall Nordic AB"},
	{0x0452, "Svep Design Center AB"},
	{0x0453, "GreenPeak Technologies BV"},
	{0x0454, "Sphinx Electronics GmbH & Co KG"},
	{0x0455, "Atomation"},
	{0x0456, "Nemik Consulting Inc"},
	{0x0457, "RF INNOVATION"},
	{0x0458, "Mini Solution Co., Ltd."},
	{0x0459, "Lumenetix, Inc"},
	{0x045A, "2048450 Ontario Inc"},
	{0x045B, "SPACEEK LTD"},
	{0x045C, "Delta T Corporation"},
	{0x045D, "Boston Scientific Corporation"},
	{0x045E, "Nuviz, Inc."},
	{0x045F, "Real Time Automation, Inc."},
	{0x0460, "Kolibree"},
	{0x0461, "vhf elektronik GmbH"},
	{0x0462, "Bonsai Systems GmbH"},
	{0x0463, "Fathom Systems Inc."},
	{0x0464, "Bellman & Symfon"},
	{0x0465, "International Forte Group LLC"},
	{0x0466, "CycleLabs Solutions inc."},
	{0x0467, "Codenex Oy"},
	{0x0468, "Kynesim Ltd"},
	{0x0469, "Palago AB"},
	{0x046A, "INSIGMA INC."},
	{0x046B, "PMD Solutions"},
	{0x046C, "Qingdao Realtime Technology Co., Ltd."},
	{0x046D, "BEGA Gantenbrink-Leuchten KG"},
	{0x046E, "Pambor Ltd."},
	{0xFFFF, "SPECIAL USE/DEFAULT"},
};

void Net::PacketAnalyzerBluetooth::AddDirection(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt32 dir)
{
	switch (dir)
	{
	case 0:
		frame->AddField(frameOfst, 4, (const UTF8Char*)"Direction", (const UTF8Char*)"Sent");
		break;
	case 1:
		frame->AddField(frameOfst, 4, (const UTF8Char*)"Direction", (const UTF8Char*)"Rcvd");
		break;
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Unknown (0x");
			sb.AppendHex32(dir);
			sb.AppendChar(')', 1);
			frame->AddField(frameOfst, 4, (const UTF8Char*)"Direction", sb.ToString());
		}
		break;
	}
}

void Net::PacketAnalyzerBluetooth::AddHCIPacketType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 packetType)
{
	frame->AddHex8Name(frameOfst, "HCI Packet Type", packetType, HCIPacketTypeGetName(packetType));
}

void Net::PacketAnalyzerBluetooth::AddParamLen(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 len)
{
	frame->AddUInt(frameOfst, 1, "Parameter Total Length", len);
}

void Net::PacketAnalyzerBluetooth::AddCmdOpcode(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 cmd)
{
	frame->AddHex16Name(frameOfst, "Command Opcode", cmd, CmdGetName(cmd));
}

void Net::PacketAnalyzerBluetooth::AddBDAddr(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, const UInt8 *mac, Bool randomAddr)
{
	UInt8 macBuff[6];
	macBuff[0] = mac[5];
	macBuff[1] = mac[4];
	macBuff[2] = mac[3];
	macBuff[3] = mac[2];
	macBuff[4] = mac[1];
	macBuff[5] = mac[0];
	frame->AddMACAddr(frameOfst, name, macBuff, !randomAddr);
}

void Net::PacketAnalyzerBluetooth::AddScanType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 scanType)
{
	const Char *vName = 0;
	switch (scanType)
	{
	case 1:
		vName = "Active";
		break;
	}
	frame->AddHex8Name(frameOfst, "Scan Type", scanType, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddScanFilterPolicy(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 scanType)
{
	const Char *vName = 0;
	switch (scanType)
	{
	case 0:
		vName = "Accept all advertisements, except directed advertisements not addressed to this device";
		break;
	}
	frame->AddHex8Name(frameOfst, "Scan Filter Policy", scanType, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddScanInterval(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 scanInt)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanInt);
	sb.Append((const UTF8Char*)" (");
	Text::SBAppendF64(&sb, 0.625 * scanInt);
	sb.Append((const UTF8Char*)"ms)");
	frame->AddField(frameOfst, 2, (const UTF8Char*)"Scan Interval", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddScanWindow(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 scanWind)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanWind);
	sb.Append((const UTF8Char*)" (");
	Text::SBAppendF64(&sb, 0.625 * scanWind);
	sb.Append((const UTF8Char*)"ms)");
	frame->AddField(frameOfst, 2, (const UTF8Char*)"Scan Window", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddAddrType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, UInt8 addrType)
{
	const Char *vName = 0;
	switch (addrType)
	{
	case 0:
		vName = "Public Device Address";
		break;
	case 1:
		vName = "Random Address Type";
		break;
	}
	frame->AddHex8Name(frameOfst, name, addrType, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddHCIEvent(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 evt)
{
	frame->AddHex8Name(frameOfst, "Event Code", evt, HCIEventGetName(evt));
}

void Net::PacketAnalyzerBluetooth::AddLESubEvent(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 subEvt)
{
	frame->AddHex8Name(frameOfst, "Sub Event", subEvt, LESubEventGetName(subEvt));
}

void Net::PacketAnalyzerBluetooth::AddAdvEvtType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 evtType)
{
	const Char *vName = 0;
	switch (evtType)
	{
	case 0:
		vName = "Connectable Undirected Advertising";
		break;
	case 3:
		vName = "Non-Connectable Undirected Advertising";
		break;
	case 4:
		vName = "Scan Response";
		break;
	}
	frame->AddHex8Name(frameOfst, "Event Type", evtType, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddRSSI(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, Int8 rssi)
{
	UTF8Char sbuff[16];
	Text::StrConcat(Text::StrInt16(sbuff, rssi), (const UTF8Char*)"dBm");
	frame->AddField(frameOfst, 1, (const UTF8Char*)"RSSI", sbuff);
}

void Net::PacketAnalyzerBluetooth::AddAdvData(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize)
{
	const Char *vName;
	Text::StringBuilderUTF8 sb;
	UInt16 u16;
	UInt8 len;
	UInt32 i = 0;
	UInt32 j;
	while (i < packetSize)
	{
		len = packet[i];
		if (len + i + 1 > packetSize)
		{
			AddUnknown(frame, frameOfst + i, packet + i, packetSize - i);
			return;
		}
		if (len == 0)
		{
			frame->AddField(frameOfst + i, (UInt32)packetSize - i, (const UTF8Char*)"Unused", 0);
			return;
		}
		frame->AddUInt(frameOfst + i, 1, "Adv Item Len", len);
		frame->AddHex8(frameOfst + i + 1, "Adv Item Type", packet[i + 1]);
		if (packet[i + 1] == 1 && len == 2)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"0x");
			sb.AppendHex8(packet[i + 2]);
			Bool found = false;
			if (packet[i + 2] & 1)
			{
				sb.Append((const UTF8Char*)" LE Limited Discoverable Mode");
				found = true;
			}
			if (packet[i + 2] & 2)
			{
				if (found) sb.AppendChar(',', 1);
				sb.Append((const UTF8Char*)" LE General Discoverable Mode");
				found = true;
			}
			if (packet[i + 2] & 4)
			{
				if (found) sb.AppendChar(',', 1);
				sb.Append((const UTF8Char*)" BR/EDR Not Supported");
				found = true;
			}
			if (packet[i + 2] & 8)
			{
				if (found) sb.AppendChar(',', 1);
				sb.Append((const UTF8Char*)" Simultaneous LE and BR/EDR to Same Device Capable (Controller)");
				found = true;
			}
			if (packet[i + 2] & 16)
			{
				if (found) sb.AppendChar(',', 1);
				sb.Append((const UTF8Char*)" Simultaneous LE and BR/EDR to Same Device Capable (Host)");
				found = true;
			}
			frame->AddField(frameOfst + i + 2, 1, (const UTF8Char*)"Flags", sb.ToString());
		}
		else if (packet[i + 1] == 2 && len == 3)
		{
			u16 = ReadUInt16(&packet[i + 2]);
			vName = 0;
			switch (u16)
			{
			case 0xFE24:
				vName = "August Home Inc";
				break;
			}
			frame->AddHex16Name(frameOfst + i + 2, "16-bit Service Class UUIDs (incomplete)", u16, (const UTF8Char*)vName);
		}
		else if (packet[i + 1] == 3 && len >= 3 && len & 1)
		{
			j = 2;
			while (j < len)
			{
				switch (ReadUInt16(&packet[i + j]))
				{
				case 0x0001:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"SDP");
					break;
				case 0x0003:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"RFCOMM");
					break;
				case 0x0100:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"L2CAP");
					break;
				case 0x1101:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"Serial Port");
					break;
				case 0x111F:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"Handsfree Audio Gateway");
					break;
				case 0x1200:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"PnP Information");
					break;
				case 0x1203:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"Generic Audio");
					break;
				case 0xFE8F:
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", (const UTF8Char*)"CSR");
					break;
				default:
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Unknown(0x");
					sb.AppendHex16(ReadUInt16(&packet[i + j]));
					sb.AppendChar(')', 1);
					frame->AddField(frameOfst + i + j, 2, (const UTF8Char*)"16-bit Service Class UUIDs", sb.ToString());
					break;
				}
				j += 2;
			}
		}
		else if (packet[i + 1] == 6 && len == 17)
		{
			sb.ClearStr();
			sb.AppendHex32(ReadUInt32(&packet[i + 14]));
			sb.AppendChar('-', 1);
			sb.AppendHex16(ReadUInt16(&packet[i + 12]));
			sb.AppendChar('-', 1);
			sb.AppendHex16(ReadUInt16(&packet[i + 10]));
			sb.AppendChar('-', 1);
			sb.AppendHex16(ReadUInt16(&packet[i + 8]));
			sb.AppendChar('-', 1);
			sb.AppendHex32(ReadUInt32(&packet[i + 4]));
			sb.AppendHex16(ReadUInt16(&packet[i + 2]));
			frame->AddField(frameOfst + i + 2, 16, (const UTF8Char*)"128-bit Service Class UUIDs (incomplete)", sb.ToString());
		}
		else if (packet[i + 1] == 8)
		{
			sb.ClearStr();
			sb.AppendC(&packet[i + 2], (UOSInt)len - 1);
			frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, (const UTF8Char*)"Device Name (shortened)", sb.ToString());
		}
		else if (packet[i + 1] == 9)
		{
			sb.ClearStr();
			sb.AppendC(&packet[i + 2], (UOSInt)len - 1);
			frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, (const UTF8Char*)"Device Name", sb.ToString());
		}
		else if (packet[i + 1] == 0x0A && len == 2)
		{
			sb.ClearStr();
			sb.AppendU16(packet[i + 2]);
			sb.Append((const UTF8Char*)"dBm");
			frame->AddField(frameOfst + i + 2, 1, (const UTF8Char*)"Tx Power Level", sb.ToString());
		}
		else if (packet[i + 1] == 0xff && len >= 3)
		{
			UInt16 compId = ReadUInt16(&packet[i + 2]);
			sb.ClearStr();
			sb.Append((const UTF8Char*)"0x");
			sb.AppendHex16(compId);
			const UTF8Char *csptr = CompanyGetName(compId);
			if (csptr)
			{
				sb.Append((const UTF8Char*)" (");
				sb.Append(csptr);
				sb.AppendChar(')', 1);
			}
			else
			{
				sb.Append((const UTF8Char*)" (Unknown)");
			}
			if (compId == 0x4C)
			{
				/////////////////// https://github.com/furiousMAC/continuity/tree/master/messages
				frame->AddField(frameOfst + i + 2, 2, (const UTF8Char*)"Manufacturer Specific", sb.ToString());
				j = 4;
				while (j < (UInt32)len - 1)
				{
					vName = 0;
					switch (packet[i + j])
					{
					case 2:
						vName = "Proximity Beacons";
						break;
					case 3:
						vName = "AirPrint Message";
						break;
					case 5:
						vName = "AirDrop Message";
						break;
					case 6:
						vName = "HomeKit Message";
						break;
					case 7:
						vName = "Proximity Pairing Message";
						break;
					case 8:
						vName = "Hey Siri Message";
						break;
					case 9:
						vName = "Airplay Target Message";
						break;
					case 10:
						vName = "Airplay Source Message";
						break;
					case 11:
						vName = "Magic Switch Message";
						break;
					case 12:
						vName = "Handoff Message";
						break;
					case 13:
						vName = "Tethering Target Message";
						break;
					case 14:
						vName = "Tethering Source Message";
						break;
					case 15:
						vName = "Nearby Action";
						break;
					case 16:
						vName = "Nearby Info";
						break;
					case 18:
						vName = "Find My Message";
						break;
					}
					frame->AddHex8Name(frameOfst + i + j, "Apple Type", packet[i + j], (const UTF8Char*)vName);
					frame->AddUInt(frameOfst + i + j + 1, 1, "Apple Length", packet[i + j + 1]);
					UInt8 appLen = packet[i + j + 1] & 0x7F;
					if (appLen + j + 1 <= len)
					{
						if (packet[i + j] == 2 && appLen == 21)
						{
							sb.ClearStr();
							sb.AppendHex32(ReadUInt32(&packet[i + j + 14]));
							sb.AppendChar('-', 1);
							sb.AppendHex16(ReadUInt16(&packet[i + j + 12]));
							sb.AppendChar('-', 1);
							sb.AppendHex16(ReadUInt16(&packet[i + j + 10]));
							sb.AppendChar('-', 1);
							sb.AppendHex16(ReadUInt16(&packet[i + j + 8]));
							sb.AppendChar('-', 1);
							sb.AppendHex32(ReadUInt32(&packet[i + j + 4]));
							sb.AppendHex16(ReadUInt16(&packet[i + j + 2]));
							frame->AddField(frameOfst + i + j + 2, 16, (const UTF8Char*)"Proximity UUID", sb.ToString());
							frame->AddHex16(frameOfst + i + j + 18, "Major", ReadUInt16(&packet[i + j + 18]));
							frame->AddHex16(frameOfst + i + j + 20, "Minor", ReadUInt16(&packet[i + j + 20]));
							frame->AddInt(frameOfst + i + j + 22, 1, "Measured Power", (Int8)packet[i + j + 22]);
						}
						else if (packet[i + j] == 5 && appLen == 18)
						{
							frame->AddHexBuff(frameOfst + i + j + 2, 8, "Zeros", &packet[i + j + 2], false);
							frame->AddUInt(frameOfst + i + j + 10, 1, "AirDrop Version", packet[i + j + 10]);
							frame->AddHex16(frameOfst + i + j + 11, "Short Hash of Apple ID", ReadMUInt16(&packet[i + j + 11]));
							frame->AddHex16(frameOfst + i + j + 13, "Short Hash of Phone Number", ReadMUInt16(&packet[i + j + 13]));
							frame->AddHex16(frameOfst + i + j + 15, "Short Hash of Email", ReadMUInt16(&packet[i + j + 15]));
							frame->AddHex16(frameOfst + i + j + 17, "Short Hash of Email2", ReadMUInt16(&packet[i + j + 17]));
							frame->AddHexBuff(frameOfst + i + j + 19, 1, "Zero", &packet[i + j + 19], false);
						}
						else if (packet[i + j] == 6 && appLen == 13)
						{
							frame->AddHex8(frameOfst + i + j + 2, "Status", packet[i + j + 2]);
							frame->AddMACAddr(frameOfst + i + j + 3, "Device ID", &packet[i + j + 3], false);
							frame->AddUInt(frameOfst + i + j + 9, 2, "Category", ReadUInt16(&packet[i + j + 9]));
							frame->AddUInt(frameOfst + i + j + 11, 2, "Global State Number", ReadUInt16(&packet[i + j + 11]));
							frame->AddUInt(frameOfst + i + j + 13, 1, "Configuration Number", packet[i + j + 13]);
							frame->AddUInt(frameOfst + i + j + 14, 1, "Compatible Version", packet[i + j + 14]);
						}
						else if (packet[i + j] == 7 && appLen >= 9)
						{
							frame->AddHex8(frameOfst + i + j + 2, "Unknown", packet[i + j + 2]);
							frame->AddHex16(frameOfst + i + j + 3, "Device Model", ReadUInt16(&packet[i + j + 3]));
							frame->AddHex8(frameOfst + i + j + 5, "Status", packet[i + j + 5]);
							frame->AddHex16(frameOfst + i + j + 6, "Battery Levels", ReadUInt16(&packet[i + j + 6]));
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 6] & 15) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, (const UTF8Char*)"Left Battery Level", sb.ToString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 6] >> 4) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, (const UTF8Char*)"Right Battery Level", sb.ToString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 15) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, (const UTF8Char*)"Case Battery Level", sb.ToString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x10) >> 4);
							frame->AddSubfield(frameOfst + i + j + 6, 2, (const UTF8Char*)"Left Charging", sb.ToString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x20) >> 5);
							frame->AddSubfield(frameOfst + i + j + 6, 2, (const UTF8Char*)"Right Charging", sb.ToString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x40) >> 6);
							frame->AddSubfield(frameOfst + i + j + 6, 2, (const UTF8Char*)"Case Charging", sb.ToString());
							frame->AddUInt(frameOfst + i + j + 8, 1, "Open Count", packet[i + j + 8]);
							frame->AddHex8(frameOfst + i + j + 9, "Device Color", packet[i + j + 9]);
							frame->AddHex8(frameOfst + i + j + 10, "Unknown", packet[i + j + 10]);
							if (appLen > 9)
							{
								frame->AddHexBuff(frameOfst + i + j + 11, (UOSInt)appLen - 9, "Encrypted Data", &packet[i + j + 11], false);
							}
						}
						else if (packet[i + j] == 9 && appLen == 6)
						{
							frame->AddHex8(frameOfst + i + j + 2, "Flags", packet[i + j + 2]);
							frame->AddHex8(frameOfst + i + j + 3, "Seed", packet[i + j + 3]);
							frame->AddIPv4(frameOfst + i + j + 4, "AirPlay IPv4 Address", &packet[i + j + 4]);
						}
						else if (packet[i + j] == 10 && appLen == 1)
						{
							frame->AddHex8(frameOfst + i + j + 2, "Data", packet[i + j + 2]);
						}
						else if (packet[i + j] == 12 && appLen== 14)
						{
							frame->AddBool(frameOfst + i + j + 2, "Copy/Cut Performed", packet[i + j + 2]);
							frame->AddUInt(frameOfst + i + j + 3, 2, "Sequence Number", ReadUInt16(&packet[i + j + 3]));
							frame->AddHex8(frameOfst + i + j + 5, "AES-GCM Auth Tag", packet[i + j + 5]);
							frame->AddHexBuff(frameOfst + i + j + 6, 10, "Encrypted Handoff Data", &packet[i + j + 6], false);
						}
						else if (packet[i + j] == 15 && appLen >= 2)
						{
							frame->AddHex8(frameOfst + i + j + 2, "Flags", packet[i + j + 2]);
							vName = 0;
							switch (packet[i + j + 3])
							{
							case 1:
								vName = "Apple TV Setup";
								break;
							case 4:
								vName = "Mobile Backup";
								break;
							case 0x05:
								vName = "Watch Setup";
								break;
							case 0x06:
								vName = "Apple TV Pair";
								break;
							case 0x07:
								vName = "Internet Relay";
								break;
							case 0x08:
								vName = "WiFi Password";
								break;
							case 0x09:
								vName = "iOS Setup";
								break;
							case 0x0A:
								vName = "Repair";
								break;
							case 0x0B:
								vName = "Speaker Setupd";
								break;
							case 0x0C:
								vName = "Apple Pay";
								break;
							case 0x0D:
								vName = "Whole Home Audio Setup";
								break;
							case 0x0E:
								vName = "Developer Tools Pairing Request";
								break;
							case 0x0F:
								vName = "Answered Call";
								break;
							case 0x10:
								vName = "Ended Call";
								break;
							case 0x11:
								vName = "DD Ping";
								break;
							case 0x12:
								vName = "DD Pong";
								break;
							case 0x13:
								vName = "Remote Auto Fill";
								break;
							case 0x14:
								vName = "Companion Link Proximity";
								break;
							case 0x15:
								vName = "Remote Management";
								break;
							case 0x16:
								vName = "Remote Auto Fill Pong";
								break;
							case 0x17:
								vName = "Remote Display";
								break;
							}
							frame->AddUIntName(frameOfst + i + j + 3, 1, "Action Type", packet[i + j + 3], (const UTF8Char*)vName);
							if (appLen >= 5)
							{
								frame->AddHexBuff(frameOfst + i + j + 4, 3, "Authentication Tag", &packet[i + j + 4], false);
							}
						}
						else if (packet[i + j] == 16 && appLen >= 2)
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"0x");
							sb.AppendHex8(packet[i + j + 2]);
							switch (packet[i + j + 2] & 15)
							{
							case 0:
								sb.Append((const UTF8Char*)" (Activity Level Unknown)");
								break;
							case 1:
								sb.Append((const UTF8Char*)" (Activity Reporting Disabled )");
								break;
							case 3:
								sb.Append((const UTF8Char*)" (Idle User)");
								break;
							case 5:
								sb.Append((const UTF8Char*)" (Audio playing while screen locked)");
								break;
							case 7:
								sb.Append((const UTF8Char*)" (Active user (screen on) )");
								break;
							case 9:
								sb.Append((const UTF8Char*)" (Screen on with video playing)");
								break;
							case 10:
								sb.Append((const UTF8Char*)" (Watch on wrist and unlocked)");
								break;
							case 11:
								sb.Append((const UTF8Char*)" (Recent user interaction)");
								break;
							case 13:
								sb.Append((const UTF8Char*)" (User is driving a vehicle)");
								break;
							case 14:
								sb.Append((const UTF8Char*)" (Phone or Facetime Call)");
								break;
							default:
								sb.Append((const UTF8Char*)" (Unknown)");
								break;
							}
							if (packet[i + j + 2] & 0x10)
							{
								sb.Append((const UTF8Char*)", primary iCloud account device");
							}
							if (packet[i + j + 2] & 0x20)
							{
								sb.Append((const UTF8Char*)", Unknown");
							}
							if (packet[i + j + 2] & 0x40)
							{
								sb.Append((const UTF8Char*)", AirDrop Receiving is enabled");
							}
							if (packet[i + j + 2] & 0x80)
							{
								sb.Append((const UTF8Char*)", Unknown");
							}
							frame->AddField(frameOfst + i + j + 2, 1, (const UTF8Char*)"Action", sb.ToString());
							frame->AddHex8(frameOfst + i + j + 3, "Status", packet[i + j + 3]);
							if (packet[i + j + 3] & 2)
							{
								if (appLen >= 6)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, 4, "Authentication Tag", &packet[i + j + 4], false);
								}
								else if (appLen > 2)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, (UOSInt)appLen - 2, "Authentication Tag", &packet[i + j + 4], false);
								}
								if (appLen >= 7)
								{
									frame->AddHex8(frameOfst + i + j + 8, "Post Auth Tag Data", packet[i + j + 8]);
								}
							}
							else
							{
								if (appLen >= 5)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, 3, "Authentication Tag", &packet[i + j + 4], false);
								}
								else if (appLen > 2)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, (UOSInt)appLen - 2, "Authentication Tag", &packet[i + j + 4], false);
								}
								if (appLen >= 6)
								{
									frame->AddHex8(frameOfst + i + j + 7, "Post Auth Tag Data", packet[i + j + 7]);
								}
							}
						}
						else if (packet[i + j] == 18 && appLen >= 25)
						{
							frame->AddHex8(frameOfst + i + j + 2, "Status", packet[i + j + 2]);
							frame->AddHexBuff(frameOfst + i + j + 3, 22, "Byte 6-27 of Public Key", &packet[i + j + 3], false);
							frame->AddHex8(frameOfst + i + j + 25, "Byte 0, Bit 6-7 of public key", packet[i + j + 25]);
							frame->AddHex8(frameOfst + i + j + 26, "Hint (Byte 5 of BD_ADDR)", packet[i + j + 26]);
						}
						else if (packet[i + j] == 19)
						{
							frame->AddHexBuff(frameOfst + i + j + 2, appLen, "Unknown", &packet[i + j + 2], false);
						}
					}
					j += (UInt32)appLen + 2;
				}
			}
			else
			{
				sb.Append((const UTF8Char*)", Value=");
				sb.AppendHexBuff(&packet[i + 4], (UOSInt)len - 3, ' ', Text::LBT_NONE);
				frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, (const UTF8Char*)"Manufacturer Specific", sb.ToString());
			}
		}
		else
		{
			sb.ClearStr();
			sb.AppendHexBuff(packet + i + 2, (UOSInt)len - 1, ' ', Text::LBT_NONE);
			frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, (const UTF8Char*)"Adv Item Value", sb.ToString());
		}
		i += (UInt32)len + 1;
	}
}

void Net::PacketAnalyzerBluetooth::AddLAP(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt32 lap)
{
	frame->AddHex24(frameOfst, "LAP", lap);
}

void Net::PacketAnalyzerBluetooth::AddInquiryLen(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 len)
{
	const Char *vName = 0;
	switch (len)
	{
	case 4:
		vName = "5.12 sec";
		break;
	}
	frame->AddUIntName(frameOfst, 1, "Inquiry Length", len, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddPageScanMode(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 mode)
{
	const Char *vName = 0;
	switch (mode)
	{
	case 1:
		vName = "R1";
		break;
	}
	frame->AddHex8Name(frameOfst, "Page Scan Repetition Mode", mode, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddClassOfDevice(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt32 cls)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"0x");
	sb.AppendHex24(cls);
	sb.Append((const UTF8Char*)", Class=");
	switch (cls & 31)
	{
	case 2:
		sb.Append((const UTF8Char*)"Phone");
		break;
	default:
		sb.AppendU16(cls & 31);
		sb.Append((const UTF8Char*)" (Unknown)");
		break;
	}
	if (cls & 0x20)
	{
		sb.Append((const UTF8Char*)", Limited Discoverable Mode");
	}
	if (cls & 0x100)
	{
		sb.Append((const UTF8Char*)", Positioning");
	}
	if (cls & 0x200)
	{
		sb.Append((const UTF8Char*)", Networking");
	}
	if (cls & 0x400)
	{
		sb.Append((const UTF8Char*)", Rendering");
	}
	if (cls & 0x800)
	{
		sb.Append((const UTF8Char*)", Capturing");
	}
	if (cls & 0x1000)
	{
		sb.Append((const UTF8Char*)", Object Transfer");
	}
	if (cls & 0x2000)
	{
		sb.Append((const UTF8Char*)", Audio");
	}
	if (cls & 0x4000)
	{
		sb.Append((const UTF8Char*)", Telephony");
	}
	if (cls & 0x8000)
	{
		sb.Append((const UTF8Char*)", Information");
	}
	sb.Append((const UTF8Char*)", Format=");
	sb.AppendU32((cls >> 16) & 3);
	sb.Append((const UTF8Char*)", Minor Class=");
	sb.AppendU32((cls >> 18) & 0x3F);
	frame->AddField(frameOfst, 3, (const UTF8Char*)"Class Of Device", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddClockOffset(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 ofst)
{
	frame->AddHex16(frameOfst, "Clock Offset", ofst);
}

void Net::PacketAnalyzerBluetooth::AddExAdvEvtType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 evtType)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"0x");
	sb.AppendHex16(evtType);
	if (evtType & 1)
	{
		sb.Append((const UTF8Char*)", Connectable");
	}
	if (evtType & 2)
	{
		sb.Append((const UTF8Char*)", Scannable");
	}
	if (evtType & 4)
	{
		sb.Append((const UTF8Char*)", Directed");
	}
	if (evtType & 8)
	{
		sb.Append((const UTF8Char*)", Scan Response");
	}
	if (evtType & 16)
	{
		sb.Append((const UTF8Char*)", Legacy");
	}
	sb.Append((const UTF8Char*)", Data Status=");
	sb.AppendUOSInt((((UOSInt)evtType) >> 5) & 3);
	switch ((((UOSInt)evtType) >> 5) & 3)
	{
	case 0:
		sb.Append((const UTF8Char*)" (Complete)");
		break;
	default:
		sb.Append((const UTF8Char*)" (Unknown)");
		break;
	}
	frame->AddField(frameOfst, 2, (const UTF8Char*)"Event Type", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddPHYType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, UInt8 phyType)
{
	const Char *vName = 0;
	switch (phyType)
	{
	case 0:
		vName = "No packets";
		break;
	case 1:
		vName = "LE 1M";
		break;
	}
	frame->AddHex8Name(frameOfst, name, phyType, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddAdvSID(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 sid)
{
	const Char *vName = 0;
	switch (sid)
	{
	case 0xFF:
		vName = "Not available";
		break;
	}
	frame->AddHex8Name(frameOfst, "Advertising SID", sid, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddTxPower(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, Int8 txPower)
{
	UTF8Char sbuff[16];
	Text::StrConcat(Text::StrInt16(sbuff, txPower), (const UTF8Char*)"dBm");
	frame->AddField(frameOfst, 1, (const UTF8Char*)"TxPower", sbuff);
}

void Net::PacketAnalyzerBluetooth::AddPeriodicAdv(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 interval)
{
	const Char *vName = 0;
	switch (interval)
	{
	case 0:
		vName = "No periodic advertising";
		break;
	}
	frame->AddHex16Name(frameOfst, "Periodic Advertising Interval", interval, (const UTF8Char*)vName);
}

void Net::PacketAnalyzerBluetooth::AddUnknown(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize)
{
	frame->AddHexBuff(frameOfst, (UInt32)packetSize, "Unknown", packet, true);
}

Bool Net::PacketAnalyzerBluetooth::PacketGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UInt8 mac[6];
	const UTF8Char *name;
	switch (packet[4])
	{
	case 1:
		name = CmdGetName(ReadUInt16(&packet[5]));
		if (name)
		{
			sb->Append(name);
		}
		else
		{
			sb->Append((const UTF8Char*)"Bluetooth");
		}
		return true;
	case 2:
		sb->Append((const UTF8Char*)"HCI ACL Packet");
		return true;
	case 4:
		switch (packet[5])
		{
		case 1:
			if (packet[7] == 0)
			{
				sb->Append((const UTF8Char*)"HCI Inquiry Success");
			}
			else
			{
				sb->Append((const UTF8Char*)"HCI Inquiry Complete");
			}
			return true;
		case 0x0E:
			name = CmdGetName(ReadUInt16(&packet[8]));
			if (name)
			{
				sb->Append(name);
				sb->Append((const UTF8Char*)" Accept");
			}
			else
			{
				sb->Append((const UTF8Char*)"Bluetooth");
			}
			return true;
		case 0x0F:
			name = CmdGetName(ReadUInt16(&packet[9]));
			if (name)
			{
				if (packet[7] == 0)
				{
					sb->Append(name);
					sb->Append((const UTF8Char*)" Pending");
				}
				else
				{
					sb->Append((const UTF8Char*)"Bluetooth");
				}
			}
			else
			{
				sb->Append((const UTF8Char*)"Bluetooth");
			}
			return true;
		case 0x2F:
			sb->Append((const UTF8Char*)"HCI Result: ");
			mac[0] = packet[13];
			mac[1] = packet[12];
			mac[2] = packet[11];
			mac[3] = packet[10];
			mac[4] = packet[9];
			mac[5] = packet[8];
			sb->AppendHexBuff(mac, 6, ':', Text::LBT_NONE);
			return true;
		case 0x3E:
			switch (packet[7])
			{
			case 0x2:
				sb->Append((const UTF8Char*)"LE Adv: ");
				mac[0] = packet[16];
				mac[1] = packet[15];
				mac[2] = packet[14];
				mac[3] = packet[13];
				mac[4] = packet[12];
				mac[5] = packet[11];
				sb->AppendHexBuff(mac, 6, ':', Text::LBT_NONE);
				return true;
			case 0xd:
				sb->Append((const UTF8Char*)"LE AdvEx: ");
				mac[0] = packet[17];
				mac[1] = packet[16];
				mac[2] = packet[15];
				mac[3] = packet[14];
				mac[4] = packet[13];
				mac[5] = packet[12];
				sb->AppendHexBuff(mac, 6, ':', Text::LBT_NONE);
				return true;
			default:
				sb->Append((const UTF8Char*)"Bluetooth");
				return true;
			}
		default:
			sb->Append((const UTF8Char*)"Bluetooth");
			return true;
		}
	default:
		sb->Append((const UTF8Char*)"Bluetooth");
		return true;
	}
}

void Net::PacketAnalyzerBluetooth::PacketGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerBluetooth::PacketGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	const Char *vName;
	UInt32 i;
	UInt32 dir = ReadMUInt32(&packet[0]);
	AddDirection(frame, frameOfst + 0, dir);
	AddHCIPacketType(frame, frameOfst + 4, packet[4]);
	switch (packet[4])
	{
	case 1:
		{
			UInt16 cmd = ReadUInt16(&packet[5]);
			UInt8 cmdLen = packet[7];
			AddCmdOpcode(frame, frameOfst + 5, cmd);
			AddParamLen(frame, frameOfst + 7, cmdLen);
			if (8 + (UOSInt)cmdLen <= packetSize)
			{
				switch (cmd)
				{
				case 0x0401:
					if (cmdLen == 5)
					{
						AddLAP(frame, frameOfst + 8, ReadUInt24(&packet[8]));
						AddInquiryLen(frame, frameOfst + 11, packet[11]);
						frame->AddUInt(frameOfst + 12, 1, "Num Response", packet[12]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x2005:
					if (cmdLen == 6)
					{
						AddBDAddr(frame, frameOfst + 8, "BD_ADDR", packet + 8, true);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x200B:
					if (cmdLen == 7)
					{
						AddScanType(frame, frameOfst + 8, packet[8]);
						AddScanInterval(frame, frameOfst + 9, ReadUInt16(&packet[9]));
						AddScanWindow(frame, frameOfst + 11, ReadUInt16(&packet[11]));
						AddAddrType(frame, frameOfst + 13, "Own Address Type", packet[13]);
						AddScanFilterPolicy(frame, frameOfst + 14, packet[14]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x200C:
					if (cmdLen == 2)
					{
						frame->AddBool(frameOfst + 8, "Scan Enable", packet[8]);
						frame->AddBool(frameOfst + 9, "Filter Duplicates", packet[9]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x2041:
					if (cmdLen == 8)
					{
						AddAddrType(frame, frameOfst + 8, "Own Address Type", packet[8]);
						AddScanFilterPolicy(frame, frameOfst + 9, packet[9]);
						AddPHYType(frame, frameOfst + 10, "Scanning PHYs", packet[10]);
						AddScanType(frame, frameOfst + 11, packet[11]);
						AddScanInterval(frame, frameOfst + 12, ReadUInt16(&packet[12]));
						AddScanWindow(frame, frameOfst + 14, ReadUInt16(&packet[14]));
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				default:
					AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					break;
				}
				if (8 + (UOSInt)cmdLen < packetSize)
				{
					AddUnknown(frame, frameOfst + 8 + cmdLen, packet + 8 + cmdLen, packetSize - 8 - cmdLen);
				}
			}
		}
		break;
	case 2:
		frame->AddHex16(frameOfst + 5, "Connection Handle", ReadUInt16(&packet[5]));
		frame->AddUInt(frameOfst + 7, 2, "Data Total Length", ReadUInt16(&packet[7]));
		AddUnknown(frame, frameOfst + 9, packet + 9, packetSize - 9);
		break;
	case 4:
		AddHCIEvent(frame, frameOfst + 5, packet[5]);
		AddParamLen(frame, frameOfst + 6, packet[6]);
		switch (packet[5])
		{
		case 0x01: //Inquiry Complete
			vName = 0;
			switch (packet[7])
			{
			case 0:
				vName = "Success";
				break;
			}
			frame->AddHex8Name(frameOfst + 7, "Status", packet[7], (const UTF8Char*)vName);
			if (packetSize > 8)
			{
				AddUnknown(frame, frameOfst + 8, packet + 8, packetSize + 8);
			}
			break;
		case 0x0E: //Command Complete
			frame->AddUInt(frameOfst + 7, 1, "Number of Allowed Command Packets", packet[7]);
			AddCmdOpcode(frame, frameOfst + 8, ReadUInt16(&packet[8]));
			vName = 0;
			switch (packet[10])
			{
			case 0:
				vName = "Success";
				break;
			}
			frame->AddHex8Name(frameOfst + 10, "Status", packet[10], (const UTF8Char*)vName);
			if (packetSize > 11)
			{
				AddUnknown(frame, frameOfst + 11, packet + 11, packetSize - 11);
			}
			break;
		case 0x0F: //Command Status
			vName = 0;
			switch (packet[7])
			{
			case 0:
				vName = "Pending";
				break;
			}
			frame->AddHex8Name(frameOfst + 7, "Status", packet[7], (const UTF8Char*)vName);
			frame->AddUInt(frameOfst + 8, 1, "Number of Allowed Command Packets", packet[8]);
			AddCmdOpcode(frame, frameOfst + 9, ReadUInt16(&packet[9]));
			break;
		case 0x2F: //Extended Inquery Result
			frame->AddUInt(frameOfst + 7, 1, "Number of responses", packet[7]);
			AddBDAddr(frame, frameOfst + 8, "BD_ADDR", &packet[8], false);
			AddPageScanMode(frame, frameOfst + 14, packet[14]);
			frame->AddHex8(frameOfst + 15, "Reserved", packet[15]);
			AddClassOfDevice(frame, frameOfst + 16, ReadUInt24(&packet[16]));
			AddClockOffset(frame, frameOfst + 19, ReadUInt16(&packet[19]));
			AddRSSI(frame, frameOfst + 21, (Int8)packet[21]);
			AddAdvData(frame, frameOfst + 22, &packet[22], packetSize - 22);
			break;
		case 0x3E: //LE Meta
			AddLESubEvent(frame, frameOfst + 7, packet[7]);
			switch (packet[7])
			{
			case 2:
				frame->AddUInt(frameOfst + 8, 1, "Num Reports", packet[8]);
				if (packet[8] != 1 || packet[6] < 11)
				{
					AddUnknown(frame, frameOfst + 9, packet + 9, packetSize + 9);
					break;
				}
				AddAdvEvtType(frame, frameOfst + 9, packet[9]);
				AddAddrType(frame, frameOfst + 10, "Peer Address Type", packet[10]);
				AddBDAddr(frame, frameOfst + 11, "BD_ADDR", packet + 11, packet[10] == 1);
				frame->AddUInt(frameOfst + 17, 1, "Adv Data Length", packet[17]);
				if (18 + (UOSInt)packet[17] < packetSize)
				{
					AddAdvData(frame, frameOfst + 18, packet + 18, packet[17]);
				}
				else if (packetSize > 18)
				{
					AddUnknown(frame, frameOfst + 18, packet + 18, packetSize - 18);
				}
				i = 18 + (UInt32)packet[17];
				if (i < packetSize)
				{
					AddRSSI(frame, frameOfst + i, (Int8)packet[i]);
					i++;
				}
				if (i < packetSize)
				{
					AddUnknown(frame, frameOfst + i, packet + i, packetSize - i);
				}
				break;
			case 0x0D: //LE Exteded Advertising Report
				frame->AddUInt(frameOfst + 8, 1, "Num Reports", packet[8]);
				if (packet[8] != 1 || packet[6] < 26)
				{
					AddUnknown(frame, frameOfst + 9, packet + 9, packetSize + 9);
					break;
				}
				AddExAdvEvtType(frame, frameOfst + 9, ReadUInt16(&packet[9]));
				AddAddrType(frame, frameOfst + 11, "Peer Address Type", packet[11]);
				AddBDAddr(frame, frameOfst + 12, "BD_ADDR", packet + 12, packet[11] == 1);
				AddPHYType(frame, frameOfst + 18, "Primary PHY", packet[18]);
				AddPHYType(frame, frameOfst + 19, "Secondary PHY", packet[19]);
				AddAdvSID(frame, frameOfst + 20, packet[20]);
				AddTxPower(frame, frameOfst + 21, (Int8)packet[21]);
				AddRSSI(frame, frameOfst + 22, (Int8)packet[22]);
				AddPeriodicAdv(frame, frameOfst + 23, ReadUInt16(&packet[23]));
				AddAddrType(frame, frameOfst + 25, "Direct Address Type", packet[25]);
				AddBDAddr(frame, frameOfst + 26, "Direct BD_ADDR", packet + 26, packet[25] == 1);
				frame->AddUInt(frameOfst + 32, 1, "Adv Data Length", packet[32]);
				if (33 + (UOSInt)packet[32] <= packetSize)
				{
					AddAdvData(frame, frameOfst + 33, packet + 33, packet[32]);
					if (33 + (UOSInt)packet[32] < packetSize)
					{
						AddUnknown(frame, frameOfst + 33 + (UInt32)packet[32], packet + 33 + (UOSInt)packet[32], packetSize - 33 - (UOSInt)packet[32]);
					}
				}
				else if (packetSize > 33)
				{
					AddUnknown(frame, frameOfst + 33, packet + 33, packetSize - 33);
				}
				break;
			default:
				AddUnknown(frame, frameOfst + 8, packet + 8, packetSize + 8);
				break;
			}
			break;
		default:
			AddUnknown(frame, frameOfst + 7, packet + 7, packetSize - 7);
			break;
		}
		break;
	default:
		AddUnknown(frame, frameOfst + 5, packet + 5, packetSize - 5);
		break;		
	}
}

const UTF8Char *Net::PacketAnalyzerBluetooth::CompanyGetName(UInt16 company)
{
	OSInt i = 0;
	OSInt j = (sizeof(vendorList) / sizeof(vendorList[0])) - 1;
	OSInt k;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (vendorList[k].vendorId > company)
		{
			j = k - 1;
		}
		else if (vendorList[k].vendorId < company)
		{
			i = k + 1;
		}
		else
		{
			return (const UTF8Char*)vendorList[k].vendorName;
		}
	}
	return 0;
}

const UTF8Char *Net::PacketAnalyzerBluetooth::CmdGetName(UInt16 cmd)
{
	switch (cmd)
	{
	case 0x0401:
		return (const UTF8Char*)"HCI Inquiry";
	case 0x2005:
		return (const UTF8Char*)"LE Set Random Address";
	case 0x200B:
		return (const UTF8Char*)"LE Set Scan Parameter";
	case 0x200C:
		return (const UTF8Char*)"LE Set Scan Enable";
	default:
		return 0;
	}
}

const UTF8Char *Net::PacketAnalyzerBluetooth::HCIPacketTypeGetName(UInt8 packetType)
{
	switch (packetType)
	{
	case 1:
		return (const UTF8Char*)"HCI Command";
	case 4:
		return (const UTF8Char*)"HCI Event";
	default:
		return 0;	
	}
}

const UTF8Char *Net::PacketAnalyzerBluetooth::HCIEventGetName(UInt8 evt)
{
	switch (evt)
	{
	case 0x0E:
		return (const UTF8Char*)"Command Complete";
	case 0x0F:
		return (const UTF8Char*)"Command Status";
	case 0x2F:
		return (const UTF8Char*)"Extended Inquery Result";
	case 0x3E:
		return (const UTF8Char*)"LE Meta";
	default:
		return 0;
	}
}

const UTF8Char *Net::PacketAnalyzerBluetooth::LESubEventGetName(UInt8 subEvt)
{
	switch (subEvt)
	{
	case 0x02:
		return (const UTF8Char*)"LE Advertising Report";
	case 0x0D:
		return (const UTF8Char*)"LE Extended Advertising Report";
	default:
		return 0;
	}
}
