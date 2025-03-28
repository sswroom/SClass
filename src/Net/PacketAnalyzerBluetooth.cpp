#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/SBFrameDetail.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::DeviceDB::VendorInfo Net::PacketAnalyzerBluetooth::vendorList[] = {
	{0x0000, CSTR("Ericsson Technology Licensing")},
	{0x0001, CSTR("Nokia Mobile Phones")},
	{0x0002, CSTR("Intel Corp.")},
	{0x0003, CSTR("IBM Corp.")},
	{0x0004, CSTR("Toshiba Corp.")},
	{0x0005, CSTR("3Com")},
	{0x0006, CSTR("Microsoft")},
	{0x0007, CSTR("Lucent")},
	{0x0008, CSTR("Motorola")},
	{0x0009, CSTR("Infineon Technologies AG")},
	{0x000A, CSTR("Cambridge Silicon Radio")},
	{0x000B, CSTR("Silicon Wave")},
	{0x000C, CSTR("Digianswer A/S")},
	{0x000D, CSTR("Texas Instruments Inc.")},
	{0x000E, CSTR("Parthus Technologies Inc.")},
	{0x000F, CSTR("Broadcom Corporation")},
	{0x0010, CSTR("Mitel Semiconductor")},
	{0x0011, CSTR("Widcomm, Inc.")},
	{0x0012, CSTR("Zeevo, Inc.")},
	{0x0013, CSTR("Atmel Corporation")},
	{0x0014, CSTR("Mitsubishi Electric Corporation")},
	{0x0015, CSTR("RTX Telecom A/S")},
	{0x0016, CSTR("KC Technology Inc.")},
	{0x0017, CSTR("Newlogic")},
	{0x0018, CSTR("Transilica, Inc.")},
	{0x0019, CSTR("Rohde & Schwarz GmbH & Co. KG")},
	{0x001A, CSTR("TTPCom Limited")},
	{0x001B, CSTR("Signia Technologies, Inc.")},
	{0x001C, CSTR("Conexant Systems Inc.")},
	{0x001D, CSTR("Qualcomm")},
	{0x001E, CSTR("Inventel")},
	{0x001F, CSTR("AVM Berlin")},
	{0x0020, CSTR("BandSpeed, Inc.")},
	{0x0021, CSTR("Mansella Ltd")},
	{0x0022, CSTR("NEC Corporation")},
	{0x0023, CSTR("WavePlus Technology Co., Ltd.")},
	{0x0024, CSTR("Alcatel")},
	{0x0025, CSTR("NXP Semiconductors (formerly Philips Semiconductors)")},
	{0x0026, CSTR("C Technologies")},
	{0x0027, CSTR("Open Interface")},
	{0x0028, CSTR("R F Micro Devices")},
	{0x0029, CSTR("Hitachi Ltd")},
	{0x002A, CSTR("Symbol Technologies, Inc.")},
	{0x002B, CSTR("Tenovis")},
	{0x002C, CSTR("Macronix International Co. Ltd.")},
	{0x002D, CSTR("GCT Semiconductor")},
	{0x002E, CSTR("Norwood Systems")},
	{0x002F, CSTR("MewTel Technology Inc.")},
	{0x0030, CSTR("ST Microelectronics")},
	{0x0031, CSTR("Synopsys, Inc.")},
	{0x0032, CSTR("Red-M (Communications) Ltd")},
	{0x0033, CSTR("Commil Ltd")},
	{0x0034, CSTR("Computer Access Technology Corporation (CATC)")},
	{0x0035, CSTR("Eclipse (HQ Espana) S.L.")},
	{0x0036, CSTR("Renesas Electronics Corporation")},
	{0x0037, CSTR("Mobilian Corporation")},
	{0x0038, CSTR("Terax")},
	{0x0039, CSTR("Integrated System Solution Corp.")},
	{0x003A, CSTR("Matsushita Electric Industrial Co., Ltd.")},
	{0x003B, CSTR("Gennum Corporation")},
	{0x003C, CSTR("BlackBerry Limited (formerly Research In Motion)")},
	{0x003D, CSTR("IPextreme, Inc.")},
	{0x003E, CSTR("Systems and Chips, Inc")},
	{0x003F, CSTR("Bluetooth SIG, Inc")},
	{0x0040, CSTR("Seiko Epson Corporation")},
	{0x0041, CSTR("Integrated Silicon Solution Taiwan, Inc.")},
	{0x0042, CSTR("CONWISE Technology Corporation Ltd")},
	{0x0043, CSTR("PARROT SA")},
	{0x0044, CSTR("Socket Mobile")},
	{0x0045, CSTR("Atheros Communications, Inc.")},
	{0x0046, CSTR("MediaTek, Inc.")},
	{0x0047, CSTR("Bluegiga")},
	{0x0048, CSTR("Marvell Technology Group Ltd.")},
	{0x0049, CSTR("3DSP Corporation")},
	{0x004A, CSTR("Accel Semiconductor Ltd.")},
	{0x004B, CSTR("Continental Automotive Systems")},
	{0x004C, CSTR("Apple, Inc.")},
	{0x004D, CSTR("Staccato Communications, Inc.")},
	{0x004E, CSTR("Avago Technologies")},
	{0x004F, CSTR("APT Ltd.")},
	{0x0050, CSTR("SiRF Technology, Inc.")},
	{0x0051, CSTR("Tzero Technologies, Inc.")},
	{0x0052, CSTR("J&M Corporation")},
	{0x0053, CSTR("Free2move AB")},
	{0x0054, CSTR("3DiJoy Corporation")},
	{0x0055, CSTR("Plantronics, Inc.")},
	{0x0056, CSTR("Sony Ericsson Mobile Communications")},
	{0x0057, CSTR("Harman International Industries, Inc.")},
	{0x0058, CSTR("Vizio, Inc.")},
	{0x0059, CSTR("Nordic Semiconductor ASA")},
	{0x005A, CSTR("EM Microelectronic-Marin SA")},
	{0x005B, CSTR("Ralink Technology Corporation")},
	{0x005C, CSTR("Belkin International, Inc.")},
	{0x005D, CSTR("Realtek Semiconductor Corporation")},
	{0x005E, CSTR("Stonestreet One, LLC")},
	{0x005F, CSTR("Wicentric, Inc.")},
	{0x0060, CSTR("RivieraWaves S.A.S")},
	{0x0061, CSTR("RDA Microelectronics")},
	{0x0062, CSTR("Gibson Guitars")},
	{0x0063, CSTR("MiCommand Inc.")},
	{0x0064, CSTR("Band XI International, LLC")},
	{0x0065, CSTR("Hewlett-Packard Company")},
	{0x0066, CSTR("9Solutions Oy")},
	{0x0067, CSTR("GN Netcom A/S")},
	{0x0068, CSTR("General Motors")},
	{0x0069, CSTR("A&D Engineering, Inc.")},
	{0x006A, CSTR("MindTree Ltd.")},
	{0x006B, CSTR("Polar Electro OY")},
	{0x006C, CSTR("Beautiful Enterprise Co., Ltd.")},
	{0x006D, CSTR("BriarTek, Inc")},
	{0x006E, CSTR("Summit Data Communications, Inc.")},
	{0x006F, CSTR("Sound ID")},
	{0x0070, CSTR("Monster, LLC")},
	{0x0071, CSTR("connectBlue AB")},
	{0x0072, CSTR("ShangHai Super Smart Electronics Co. Ltd.")},
	{0x0073, CSTR("Group Sense Ltd.")},
	{0x0074, CSTR("Zomm, LLC")},
	{0x0075, CSTR("Samsung Electronics Co. Ltd.")},
	{0x0076, CSTR("Creative Technology Ltd.")},
	{0x0077, CSTR("Laird Technologies")},
	{0x0078, CSTR("Nike, Inc.")},
	{0x0079, CSTR("lesswire AG")},
	{0x007A, CSTR("MStar Semiconductor, Inc.")},
	{0x007B, CSTR("Hanlynn Technologies")},
	{0x007C, CSTR("A & R Cambridge")},
	{0x007D, CSTR("Seers Technology Co., Ltd.")},
	{0x007E, CSTR("Sports Tracking Technologies Ltd.")},
	{0x007F, CSTR("Autonet Mobile")},
	{0x0080, CSTR("DeLorme Publishing Company, Inc.")},
	{0x0081, CSTR("WuXi Vimicro")},
	{0x0082, CSTR("Sennheiser Communications A/S")},
	{0x0083, CSTR("TimeKeeping Systems, Inc.")},
	{0x0084, CSTR("Ludus Helsinki Ltd.")},
	{0x0085, CSTR("BlueRadios, Inc.")},
	{0x0086, CSTR("Equinux AG")},
	{0x0087, CSTR("Garmin International, Inc.")},
	{0x0088, CSTR("Ecotest")},
	{0x0089, CSTR("GN ReSound A/S")},
	{0x008A, CSTR("Jawbone")},
	{0x008B, CSTR("Topcon Positioning Systems, LLC")},
	{0x008C, CSTR("Gimbal Inc. (formerly Qualcomm Labs, Inc. and Qualcomm Retail Solutions, Inc.)")},
	{0x008D, CSTR("Zscan Software")},
	{0x008E, CSTR("Quintic Corp")},
	{0x008F, CSTR("Telit Wireless Solutions GmbH (formerly Stollmann E+V GmbH)")},
	{0x0090, CSTR("Funai Electric Co., Ltd.")},
	{0x0091, CSTR("Advanced PANMOBIL systems GmbH & Co. KG")},
	{0x0092, CSTR("ThinkOptics, Inc.")},
	{0x0093, CSTR("Universal Electronics, Inc.")},
	{0x0094, CSTR("Airoha Technology Corp.")},
	{0x0095, CSTR("NEC Lighting, Ltd.")},
	{0x0096, CSTR("ODM Technology, Inc.")},
	{0x0097, CSTR("ConnecteDevice Ltd.")},
	{0x0098, CSTR("zero1.tv GmbH")},
	{0x0099, CSTR("i.Tech Dynamic Global Distribution Ltd.")},
	{0x009A, CSTR("Alpwise")},
	{0x009B, CSTR("Jiangsu Toppower Automotive Electronics Co., Ltd.")},
	{0x009C, CSTR("Colorfy, Inc.")},
	{0x009D, CSTR("Geoforce Inc.")},
	{0x009E, CSTR("Bose Corporation")},
	{0x009F, CSTR("Suunto Oy")},
	{0x00A0, CSTR("Kensington Computer Products Group")},
	{0x00A1, CSTR("SR-Medizinelektronik")},
	{0x00A2, CSTR("Vertu Corporation Limited")},
	{0x00A3, CSTR("Meta Watch Ltd.")},
	{0x00A4, CSTR("LINAK A/S")},
	{0x00A5, CSTR("OTL Dynamics LLC")},
	{0x00A6, CSTR("Panda Ocean Inc.")},
	{0x00A7, CSTR("Visteon Corporation")},
	{0x00A8, CSTR("ARP Devices Limited")},
	{0x00A9, CSTR("Magneti Marelli S.p.A")},
	{0x00AA, CSTR("CAEN RFID srl")},
	{0x00AB, CSTR("Ingenieur-Systemgruppe Zahn GmbH")},
	{0x00AC, CSTR("Green Throttle Games")},
	{0x00AD, CSTR("Peter Systemtechnik GmbH")},
	{0x00AE, CSTR("Omegawave Oy")},
	{0x00AF, CSTR("Cinetix")},
	{0x00B0, CSTR("Passif Semiconductor Corp")},
	{0x00B1, CSTR("Saris Cycling Group, Inc")},
	{0x00B2, CSTR("Bekey A/S")},
	{0x00B3, CSTR("Clarinox Technologies Pty. Ltd.")},
	{0x00B4, CSTR("BDE Technology Co., Ltd.")},
	{0x00B5, CSTR("Swirl Networks")},
	{0x00B6, CSTR("Meso international")},
	{0x00B7, CSTR("TreLab Ltd")},
	{0x00B8, CSTR("Qualcomm Innovation Center, Inc. (QuIC)")},
	{0x00B9, CSTR("Johnson Controls, Inc.")},
	{0x00BA, CSTR("Starkey Laboratories Inc.")},
	{0x00BB, CSTR("S-Power Electronics Limited")},
	{0x00BC, CSTR("Ace Sensor Inc")},
	{0x00BD, CSTR("Aplix Corporation")},
	{0x00BE, CSTR("AAMP of America")},
	{0x00BF, CSTR("Stalmart Technology Limited")},
	{0x00C0, CSTR("AMICCOM Electronics Corporation")},
	{0x00C1, CSTR("Shenzhen Excelsecu Data Technology Co.,Ltd")},
	{0x00C2, CSTR("Geneq Inc.")},
	{0x00C3, CSTR("adidas AG")},
	{0x00C4, CSTR("LG Electronics")},
	{0x00C5, CSTR("Onset Computer Corporation")},
	{0x00C6, CSTR("Selfly BV")},
	{0x00C7, CSTR("Quuppa Oy.")},
	{0x00C8, CSTR("GeLo Inc")},
	{0x00C9, CSTR("Evluma")},
	{0x00CA, CSTR("MC10")},
	{0x00CB, CSTR("Binauric SE")},
	{0x00CC, CSTR("Beats Electronics")},
	{0x00CD, CSTR("Microchip Technology Inc.")},
	{0x00CE, CSTR("Elgato Systems GmbH")},
	{0x00CF, CSTR("ARCHOS SA")},
	{0x00D0, CSTR("Dexcom, Inc.")},
	{0x00D1, CSTR("Polar Electro Europe B.V.")},
	{0x00D2, CSTR("Dialog Semiconductor B.V.")},
	{0x00D3, CSTR("Taixingbang Technology (HK) Co,. LTD.")},
	{0x00D4, CSTR("Kawantech")},
	{0x00D5, CSTR("Austco Communication Systems")},
	{0x00D6, CSTR("Timex Group USA, Inc.")},
	{0x00D7, CSTR("Qualcomm Technologies, Inc.")},
	{0x00D8, CSTR("Qualcomm Connected Experiences, Inc.")},
	{0x00D9, CSTR("Voyetra Turtle Beach")},
	{0x00DA, CSTR("txtr GmbH")},
	{0x00DB, CSTR("Biosentronics")},
	{0x00DC, CSTR("Procter & Gamble")},
	{0x00DD, CSTR("Hosiden Corporation")},
	{0x00DE, CSTR("Muzik LLC")},
	{0x00DF, CSTR("Misfit Wearables Corp")},
	{0x00E0, CSTR("Google")},
	{0x00E1, CSTR("Danlers Ltd")},
	{0x00E2, CSTR("Semilink Inc")},
	{0x00E3, CSTR("inMusic Brands, Inc")},
	{0x00E4, CSTR("L.S. Research Inc.")},
	{0x00E5, CSTR("Eden Software Consultants Ltd.")},
	{0x00E6, CSTR("Freshtemp")},
	{0x00E7, CSTR("KS Technologies")},
	{0x00E8, CSTR("ACTS Technologies")},
	{0x00E9, CSTR("Vtrack Systems")},
	{0x00EA, CSTR("Nielsen-Kellerman Company")},
	{0x00EB, CSTR("Server Technology Inc.")},
	{0x00EC, CSTR("BioResarch Associates")},
	{0x00ED, CSTR("Jolly Logic, LLC")},
	{0x00EE, CSTR("Above Average Outcomes, Inc.")},
	{0x00EF, CSTR("Bitsplitters GmbH")},
	{0x00F0, CSTR("PayPal, Inc.")},
	{0x00F1, CSTR("Witron Technology Limited")},
	{0x00F2, CSTR("Morse Project Inc.")},
	{0x00F3, CSTR("Kent Displays Inc.")},
	{0x00F4, CSTR("Nautilus Inc.")},
	{0x00F5, CSTR("Smartifier Oy")},
	{0x00F6, CSTR("Elcometer Limited")},
	{0x00F7, CSTR("VSN Technologies, Inc.")},
	{0x00F8, CSTR("AceUni Corp., Ltd.")},
	{0x00F9, CSTR("StickNFind")},
	{0x00FA, CSTR("Crystal Code AB")},
	{0x00FB, CSTR("KOUKAAM a.s.")},
	{0x00FC, CSTR("Delphi Corporation")},
	{0x00FD, CSTR("ValenceTech Limited")},
	{0x00FE, CSTR("Stanley Black and Decker")},
	{0x00FF, CSTR("Typo Products, LLC")},
	{0x0100, CSTR("TomTom International BV")},
	{0x0101, CSTR("Fugoo, Inc.")},
	{0x0102, CSTR("Keiser Corporation")},
	{0x0103, CSTR("Bang & Olufsen S/A")},
	{0x0104, CSTR("PLUS Location Systems Pty Ltd")},
	{0x0105, CSTR("Ubiquitous Computing Technology Corporation")},
	{0x0106, CSTR("Innovative Yachtter Solutions")},
	{0x0107, CSTR("William Demant Holding A/S")},
	{0x0108, CSTR("Chicony Electronics Co., Ltd.")},
	{0x0109, CSTR("Atus BV")},
	{0x010A, CSTR("Codegate Ltd")},
	{0x010B, CSTR("ERi, Inc")},
	{0x010C, CSTR("Transducers Direct, LLC")},
	{0x010D, CSTR("Fujitsu Ten LImited")},
	{0x010E, CSTR("Audi AG")},
	{0x010F, CSTR("HiSilicon Technologies Col, Ltd.")},
	{0x0110, CSTR("Nippon Seiki Co., Ltd.")},
	{0x0111, CSTR("Steelseries ApS")},
	{0x0112, CSTR("Visybl Inc.")},
	{0x0113, CSTR("Openbrain Technologies, Co., Ltd.")},
	{0x0114, CSTR("Xensr")},
	{0x0115, CSTR("e.solutions")},
	{0x0116, CSTR("10AK Technologies")},
	{0x0117, CSTR("Wimoto Technologies Inc")},
	{0x0118, CSTR("Radius Networks, Inc.")},
	{0x0119, CSTR("Wize Technology Co., Ltd.")},
	{0x011A, CSTR("Qualcomm Labs, Inc.")},
	{0x011B, CSTR("Aruba Networks")},
	{0x011C, CSTR("Baidu")},
	{0x011D, CSTR("Arendi AG")},
	{0x011E, CSTR("Skoda Auto a.s.")},
	{0x011F, CSTR("Volkwagon AG")},
	{0x0120, CSTR("Porsche AG")},
	{0x0121, CSTR("Sino Wealth Electronic Ltd.")},
	{0x0122, CSTR("AirTurn, Inc.")},
	{0x0123, CSTR("Kinsa, Inc")},
	{0x0124, CSTR("HID Global")},
	{0x0125, CSTR("SEAT es")},
	{0x0126, CSTR("Promethean Ltd.")},
	{0x0127, CSTR("Salutica Allied Solutions")},
	{0x0128, CSTR("GPSI Group Pty Ltd")},
	{0x0129, CSTR("Nimble Devices Oy")},
	{0x012A, CSTR("Changzhou Yongse Infotech Co., Ltd.")},
	{0x012B, CSTR("SportIQ")},
	{0x012C, CSTR("TEMEC Instruments B.V.")},
	{0x012D, CSTR("Sony Corporation")},
	{0x012E, CSTR("ASSA ABLOY")},
	{0x012F, CSTR("Clarion Co. Inc.")},
	{0x0130, CSTR("Warehouse Innovations")},
	{0x0131, CSTR("Cypress Semiconductor")},
	{0x0132, CSTR("MADS Inc")},
	{0x0133, CSTR("Blue Maestro Limited")},
	{0x0134, CSTR("Resolution Products, Ltd.")},
	{0x0135, CSTR("Aireware LLC")},
	{0x0136, CSTR("Seed Labs, Inc. (formerly ETC sp. z.o.o.)")},
	{0x0137, CSTR("Prestigio Plaza Ltd.")},
	{0x0138, CSTR("NTEO Inc.")},
	{0x0139, CSTR("Focus Systems Corporation")},
	{0x013A, CSTR("Tencent Holdings Ltd.")},
	{0x013B, CSTR("Allegion")},
	{0x013C, CSTR("Murata Manufacturing Co., Ltd.")},
	{0x013D, CSTR("WirelessWERX")},
	{0x013E, CSTR("Nod, Inc.")},
	{0x013F, CSTR("B&B Manufacturing Company")},
	{0x0140, CSTR("Alpine Electronics (China) Co., Ltd")},
	{0x0141, CSTR("FedEx Services")},
	{0x0142, CSTR("Grape Systems Inc.")},
	{0x0143, CSTR("Bkon Connect")},
	{0x0144, CSTR("Lintech GmbH")},
	{0x0145, CSTR("Novatel Wireless")},
	{0x0146, CSTR("Ciright")},
	{0x0147, CSTR("Mighty Cast, Inc.")},
	{0x0148, CSTR("Ambimat Electronics")},
	{0x0149, CSTR("Perytons Ltd.")},
	{0x014A, CSTR("Tivoli Audio, LLC")},
	{0x014B, CSTR("Master Lock")},
	{0x014C, CSTR("Mesh-Net Ltd")},
	{0x014D, CSTR("HUIZHOU DESAY SV AUTOMOTIVE CO., LTD.")},
	{0x014E, CSTR("Tangerine, Inc.")},
	{0x014F, CSTR("B&W Group Ltd.")},
	{0x0150, CSTR("Pioneer Corporation")},
	{0x0151, CSTR("OnBeep")},
	{0x0152, CSTR("Vernier Software & Technology")},
	{0x0153, CSTR("ROL Ergo")},
	{0x0154, CSTR("Pebble Technology")},
	{0x0155, CSTR("NETATMO")},
	{0x0156, CSTR("Accumulate AB")},
	{0x0157, CSTR("Anhui Huami Information Technology Co., Ltd.")},
	{0x0158, CSTR("Inmite s.r.o.")},
	{0x0159, CSTR("ChefSteps, Inc.")},
	{0x015A, CSTR("micas AG")},
	{0x015B, CSTR("Biomedical Research Ltd.")},
	{0x015C, CSTR("Pitius Tec S.L.")},
	{0x015D, CSTR("Estimote, Inc.")},
	{0x015E, CSTR("Unikey Technologies, Inc.")},
	{0x015F, CSTR("Timer Cap Co.")},
	{0x0160, CSTR("AwoX")},
	{0x0161, CSTR("yikes")},
	{0x0162, CSTR("MADSGlobalNZ Ltd.")},
	{0x0163, CSTR("PCH International")},
	{0x0164, CSTR("Qingdao Yeelink Information Technology Co., Ltd.")},
	{0x0165, CSTR("Milwaukee Tool (Formally Milwaukee Electric Tools)")},
	{0x0166, CSTR("MISHIK Pte Ltd")},
	{0x0167, CSTR("Bayer HealthCare")},
	{0x0168, CSTR("Spicebox LLC")},
	{0x0169, CSTR("emberlight")},
	{0x016A, CSTR("Cooper-Atkins Corporation")},
	{0x016B, CSTR("Qblinks")},
	{0x016C, CSTR("MYSPHERA")},
	{0x016D, CSTR("LifeScan Inc")},
	{0x016E, CSTR("Volantic AB")},
	{0x016F, CSTR("Podo Labs, Inc")},
	{0x0170, CSTR("Roche Diabetes Care AG")},
	{0x0171, CSTR("Amazon Fulfillment Service")},
	{0x0172, CSTR("Connovate Technology Private Limited")},
	{0x0173, CSTR("Kocomojo, LLC")},
	{0x0174, CSTR("EveryKey LLC")},
	{0x0175, CSTR("Dynamic Controls")},
	{0x0176, CSTR("SentriLock")},
	{0x0177, CSTR("I-SYST inc.")},
	{0x0178, CSTR("CASIO COMPUTER CO., LTD.")},
	{0x0179, CSTR("LAPIS Semiconductor Co., Ltd.")},
	{0x017A, CSTR("Telemonitor, Inc.")},
	{0x017B, CSTR("taskit GmbH")},
	{0x017C, CSTR("Daimler AG")},
	{0x017D, CSTR("BatAndCat")},
	{0x017E, CSTR("BluDotz Ltd")},
	{0x017F, CSTR("XTel ApS")},
	{0x0180, CSTR("Gigaset Communications GmbH")},
	{0x0181, CSTR("Gecko Health Innovations, Inc.")},
	{0x0182, CSTR("HOP Ubiquitous")},
	{0x0183, CSTR("Walt Disney")},
	{0x0184, CSTR("Nectar")},
	{0x0185, CSTR("bel'apps LLC")},
	{0x0186, CSTR("CORE Lighting Ltd")},
	{0x0187, CSTR("Seraphim Sense Ltd")},
	{0x0188, CSTR("Unico RBC")},
	{0x0189, CSTR("Physical Enterprises Inc.")},
	{0x018A, CSTR("Able Trend Technology Limited")},
	{0x018B, CSTR("Konica Minolta, Inc.")},
	{0x018C, CSTR("Wilo SE")},
	{0x018D, CSTR("Extron Design Services")},
	{0x018E, CSTR("Fitbit, Inc.")},
	{0x018F, CSTR("Fireflies Systems")},
	{0x0190, CSTR("Intelletto Technologies Inc.")},
	{0x0191, CSTR("FDK CORPORATION")},
	{0x0192, CSTR("Cloudleaf, Inc")},
	{0x0193, CSTR("Maveric Automation LLC")},
	{0x0194, CSTR("Acoustic Stream Corporation")},
	{0x0195, CSTR("Zuli")},
	{0x0196, CSTR("Paxton Access Ltd")},
	{0x0197, CSTR("WiSilica Inc.")},
	{0x0198, CSTR("VENGIT Korltolt Felelssg Trsasg")},
	{0x0199, CSTR("SALTO SYSTEMS S.L.")},
	{0x019A, CSTR("TRON Forum (formerly T-Engine Forum)")},
	{0x019B, CSTR("CUBETECH s.r.o.")},
	{0x019C, CSTR("Cokiya Incorporated")},
	{0x019D, CSTR("CVS Health")},
	{0x019E, CSTR("Ceruus")},
	{0x019F, CSTR("Strainstall Ltd")},
	{0x01A0, CSTR("Channel Enterprises (HK) Ltd.")},
	{0x01A1, CSTR("FIAMM")},
	{0x01A2, CSTR("GIGALANE.CO.,LTD")},
	{0x01A3, CSTR("EROAD")},
	{0x01A4, CSTR("Mine Safety Appliances")},
	{0x01A5, CSTR("Icon Health and Fitness")},
	{0x01A6, CSTR("Asandoo GmbH")},
	{0x01A7, CSTR("ENERGOUS CORPORATION")},
	{0x01A8, CSTR("Taobao")},
	{0x01A9, CSTR("Canon Inc.")},
	{0x01AA, CSTR("Geophysical Technology Inc.")},
	{0x01AB, CSTR("Facebook, Inc.")},
	{0x01AC, CSTR("Nipro Diagnostics, Inc.")},
	{0x01AD, CSTR("FlightSafety International")},
	{0x01AE, CSTR("Earlens Corporation")},
	{0x01AF, CSTR("Sunrise Micro Devices, Inc.")},
	{0x01B0, CSTR("Star Micronics Co., Ltd.")},
	{0x01B1, CSTR("Netizens Sp. z o.o.")},
	{0x01B2, CSTR("Nymi Inc.")},
	{0x01B3, CSTR("Nytec, Inc.")},
	{0x01B4, CSTR("Trineo Sp. z o.o.")},
	{0x01B5, CSTR("Nest Labs Inc.")},
	{0x01B6, CSTR("LM Technologies Ltd")},
	{0x01B7, CSTR("General Electric Company")},
	{0x01B8, CSTR("i+D3 S.L.")},
	{0x01B9, CSTR("HANA Micron")},
	{0x01BA, CSTR("Stages Cycling LLC")},
	{0x01BB, CSTR("Cochlear Bone Anchored Solutions AB")},
	{0x01BC, CSTR("SenionLab AB")},
	{0x01BD, CSTR("Syszone Co., Ltd")},
	{0x01BE, CSTR("Pulsate Mobile Ltd.")},
	{0x01BF, CSTR("Hong Kong HunterSun Electronic Limited")},
	{0x01C0, CSTR("pironex GmbH")},
	{0x01C1, CSTR("BRADATECH Corp.")},
	{0x01C2, CSTR("Transenergooil AG")},
	{0x01C3, CSTR("Bunch")},
	{0x01C4, CSTR("DME Microelectronics")},
	{0x01C5, CSTR("Bitcraze AB")},
	{0x01C6, CSTR("HASWARE Inc.")},
	{0x01C7, CSTR("Abiogenix Inc.")},
	{0x01C8, CSTR("Poly-Control ApS")},
	{0x01C9, CSTR("Avi-on")},
	{0x01CA, CSTR("Laerdal Medical AS")},
	{0x01CB, CSTR("Fetch My Pet")},
	{0x01CC, CSTR("Sam Labs Ltd.")},
	{0x01CD, CSTR("Chengdu Synwing Technology Ltd")},
	{0x01CE, CSTR("HOUWA SYSTEM DESIGN, k.k.")},
	{0x01CF, CSTR("BSH")},
	{0x01D0, CSTR("Primus Inter Pares Ltd")},
	{0x01D1, CSTR("August Home, Inc")},
	{0x01D2, CSTR("Gill Electronics")},
	{0x01D3, CSTR("Sky Wave Design")},
	{0x01D4, CSTR("Newlab S.r.l.")},
	{0x01D5, CSTR("ELAD srl")},
	{0x01D6, CSTR("G-wearables inc.")},
	{0x01D7, CSTR("Squadrone Systems Inc.")},
	{0x01D8, CSTR("Code Corporation")},
	{0x01D9, CSTR("Savant Systems LLC")},
	{0x01DA, CSTR("Logitech International SA")},
	{0x01DB, CSTR("Innblue Consulting")},
	{0x01DC, CSTR("iParking Ltd.")},
	{0x01DD, CSTR("Koninklijke Philips Electronics N.V.")},
	{0x01DE, CSTR("Minelab Electronics Pty Limited")},
	{0x01DF, CSTR("Bison Group Ltd.")},
	{0x01E0, CSTR("Widex A/S")},
	{0x01E1, CSTR("Jolla Ltd")},
	{0x01E2, CSTR("Lectronix, Inc.")},
	{0x01E3, CSTR("Caterpillar Inc")},
	{0x01E4, CSTR("Freedom Innovations")},
	{0x01E5, CSTR("Dynamic Devices Ltd")},
	{0x01E6, CSTR("Technology Solutions (UK) Ltd")},
	{0x01E7, CSTR("IPS Group Inc.")},
	{0x01E8, CSTR("STIR")},
	{0x01E9, CSTR("Sano, Inc.")},
	{0x01EA, CSTR("Advanced Application Design, Inc.")},
	{0x01EB, CSTR("AutoMap LLC")},
	{0x01EC, CSTR("Spreadtrum Communications Shanghai Ltd")},
	{0x01ED, CSTR("CuteCircuit LTD")},
	{0x01EE, CSTR("Valeo Service")},
	{0x01EF, CSTR("Fullpower Technologies, Inc.")},
	{0x01F0, CSTR("KloudNation")},
	{0x01F1, CSTR("Zebra Technologies Corporation")},
	{0x01F2, CSTR("Itron, Inc.")},
	{0x01F3, CSTR("The University of Tokyo")},
	{0x01F4, CSTR("UTC Fire and Security")},
	{0x01F5, CSTR("Cool Webthings Limited")},
	{0x01F6, CSTR("DJO Global")},
	{0x01F7, CSTR("Gelliner Limited")},
	{0x01F8, CSTR("Anyka (Guangzhou) Microelectronics Technology Co, LTD")},
	{0x01F9, CSTR("Medtronic Inc.")},
	{0x01FA, CSTR("Gozio Inc.")},
	{0x01FB, CSTR("Form Lifting, LLC")},
	{0x01FC, CSTR("Wahoo Fitness, LLC")},
	{0x01FD, CSTR("Kontakt Micro-Location Sp. z o.o.")},
	{0x01FE, CSTR("Radio Systems Corporation")},
	{0x01FF, CSTR("Freescale Semiconductor, Inc.")},
	{0x0200, CSTR("Verifone Systems Pte Ltd. Taiwan Branch")},
	{0x0201, CSTR("AR Timing")},
	{0x0202, CSTR("Rigado LLC")},
	{0x0203, CSTR("Kemppi Oy")},
	{0x0204, CSTR("Tapcentive Inc.")},
	{0x0205, CSTR("Smartbotics Inc.")},
	{0x0206, CSTR("Otter Products, LLC")},
	{0x0207, CSTR("STEMP Inc.")},
	{0x0208, CSTR("LumiGeek LLC")},
	{0x0209, CSTR("InvisionHeart Inc.")},
	{0x020A, CSTR("Macnica Inc.")},
	{0x020B, CSTR("Jaguar Land Rover Limited")},
	{0x020C, CSTR("CoroWare Technologies, Inc")},
	{0x020D, CSTR("Simplo Technology Co., LTD")},
	{0x020E, CSTR("Omron Healthcare Co., LTD")},
	{0x020F, CSTR("Comodule GMBH")},
	{0x0210, CSTR("ikeGPS")},
	{0x0211, CSTR("Telink Semiconductor Co. Ltd")},
	{0x0212, CSTR("Interplan Co., Ltd")},
	{0x0213, CSTR("Wyler AG")},
	{0x0214, CSTR("IK Multimedia Production srl")},
	{0x0215, CSTR("Lukoton Experience Oy")},
	{0x0216, CSTR("MTI Ltd")},
	{0x0217, CSTR("Tech4home, Lda")},
	{0x0218, CSTR("Hiotech AB")},
	{0x0219, CSTR("DOTT Limited")},
	{0x021A, CSTR("Blue Speck Labs, LLC")},
	{0x021B, CSTR("Cisco Systems, Inc")},
	{0x021C, CSTR("Mobicomm Inc")},
	{0x021D, CSTR("Edamic")},
	{0x021E, CSTR("Goodnet, Ltd")},
	{0x021F, CSTR("Luster Leaf Products Inc")},
	{0x0220, CSTR("Manus Machina BV")},
	{0x0221, CSTR("Mobiquity Networks Inc")},
	{0x0222, CSTR("Praxis Dynamics")},
	{0x0223, CSTR("Philip Morris Products S.A.")},
	{0x0224, CSTR("Comarch SA")},
	{0x0225, CSTR("Nestl Nespresso S.A.")},
	{0x0226, CSTR("Merlinia A/S")},
	{0x0227, CSTR("LifeBEAM Technologies")},
	{0x0228, CSTR("Twocanoes Labs, LLC")},
	{0x0229, CSTR("Muoverti Limited")},
	{0x022A, CSTR("Stamer Musikanlagen GMBH")},
	{0x022B, CSTR("Tesla Motors")},
	{0x022C, CSTR("Pharynks Corporation")},
	{0x022D, CSTR("Lupine")},
	{0x022E, CSTR("Siemens AG")},
	{0x022F, CSTR("Huami (Shanghai) Culture Communication CO., LTD")},
	{0x0230, CSTR("Foster Electric Company, Ltd")},
	{0x0231, CSTR("ETA SA")},
	{0x0232, CSTR("x-Senso Solutions Kft")},
	{0x0233, CSTR("Shenzhen SuLong Communication Ltd")},
	{0x0234, CSTR("FengFan (BeiJing) Technology Co, Ltd")},
	{0x0235, CSTR("Qrio Inc")},
	{0x0236, CSTR("Pitpatpet Ltd")},
	{0x0237, CSTR("MSHeli s.r.l.")},
	{0x0238, CSTR("Trakm8 Ltd")},
	{0x0239, CSTR("JIN CO, Ltd")},
	{0x023A, CSTR("Alatech Tehnology")},
	{0x023B, CSTR("Beijing CarePulse Electronic Technology Co, Ltd")},
	{0x023C, CSTR("Awarepoint")},
	{0x023D, CSTR("ViCentra B.V.")},
	{0x023E, CSTR("Raven Industries")},
	{0x023F, CSTR("WaveWare Technologies Inc.")},
	{0x0240, CSTR("Argenox Technologies")},
	{0x0241, CSTR("Bragi GmbH")},
	{0x0242, CSTR("16Lab Inc")},
	{0x0243, CSTR("Masimo Corp")},
	{0x0244, CSTR("Iotera Inc")},
	{0x0245, CSTR("Endress+Hauser")},
	{0x0246, CSTR("ACKme Networks, Inc.")},
	{0x0247, CSTR("FiftyThree Inc.")},
	{0x0248, CSTR("Parker Hannifin Corp")},
	{0x0249, CSTR("Transcranial Ltd")},
	{0x024A, CSTR("Uwatec AG")},
	{0x024B, CSTR("Orlan LLC")},
	{0x024C, CSTR("Blue Clover Devices")},
	{0x024D, CSTR("M-Way Solutions GmbH")},
	{0x024E, CSTR("Microtronics Engineering GmbH")},
	{0x024F, CSTR("Schneider Schreibgerte GmbH")},
	{0x0250, CSTR("Sapphire Circuits LLC")},
	{0x0251, CSTR("Lumo Bodytech Inc.")},
	{0x0252, CSTR("UKC Technosolution")},
	{0x0253, CSTR("Xicato Inc.")},
	{0x0254, CSTR("Playbrush")},
	{0x0255, CSTR("Dai Nippon Printing Co., Ltd.")},
	{0x0256, CSTR("G24 Power Limited")},
	{0x0257, CSTR("AdBabble Local Commerce Inc.")},
	{0x0258, CSTR("Devialet SA")},
	{0x0259, CSTR("ALTYOR")},
	{0x025A, CSTR("University of Applied Sciences Valais/Haute Ecole Valaisanne")},
	{0x025B, CSTR("Five Interactive, LLC dba Zendo")},
	{0x025C, CSTR("NetEaseHangzhouNetwork co.Ltd.")},
	{0x025D, CSTR("Lexmark International Inc.")},
	{0x025E, CSTR("Fluke Corporation")},
	{0x025F, CSTR("Yardarm Technologies")},
	{0x0260, CSTR("SensaRx")},
	{0x0261, CSTR("SECVRE GmbH")},
	{0x0262, CSTR("Glacial Ridge Technologies")},
	{0x0263, CSTR("Identiv, Inc.")},
	{0x0264, CSTR("DDS, Inc.")},
	{0x0265, CSTR("SMK Corporation")},
	{0x0266, CSTR("Schawbel Technologies LLC")},
	{0x0267, CSTR("XMI Systems SA")},
	{0x0268, CSTR("Cerevo")},
	{0x0269, CSTR("Torrox GmbH & Co KG")},
	{0x026A, CSTR("Gemalto")},
	{0x026B, CSTR("DEKA Research & Development Corp.")},
	{0x026C, CSTR("Domster Tadeusz Szydlowski")},
	{0x026D, CSTR("Technogym SPA")},
	{0x026E, CSTR("FLEURBAEY BVBA")},
	{0x026F, CSTR("Aptcode Solutions")},
	{0x0270, CSTR("LSI ADL Technology")},
	{0x0271, CSTR("Animas Corp")},
	{0x0272, CSTR("Alps Electric Co., Ltd.")},
	{0x0273, CSTR("OCEASOFT")},
	{0x0274, CSTR("Motsai Research")},
	{0x0275, CSTR("Geotab")},
	{0x0276, CSTR("E.G.O. Elektro-Gertebau GmbH")},
	{0x0277, CSTR("bewhere inc")},
	{0x0278, CSTR("Johnson Outdoors Inc")},
	{0x0279, CSTR("steute Schaltgerate GmbH & Co. KG")},
	{0x027A, CSTR("Ekomini inc.")},
	{0x027B, CSTR("DEFA AS")},
	{0x027C, CSTR("Aseptika Ltd")},
	{0x027D, CSTR("HUAWEI Technologies Co., Ltd. ( )")},
	{0x027E, CSTR("HabitAware, LLC")},
	{0x027F, CSTR("ruwido austria gmbh")},
	{0x0280, CSTR("ITEC corporation")},
	{0x0281, CSTR("StoneL")},
	{0x0282, CSTR("Sonova AG")},
	{0x0283, CSTR("Maven Machines, Inc.")},
	{0x0284, CSTR("Synapse Electronics")},
	{0x0285, CSTR("Standard Innovation Inc.")},
	{0x0286, CSTR("RF Code, Inc.")},
	{0x0287, CSTR("Wally Ventures S.L.")},
	{0x0288, CSTR("Willowbank Electronics Ltd")},
	{0x0289, CSTR("SK Telecom")},
	{0x028A, CSTR("Jetro AS")},
	{0x028B, CSTR("Code Gears LTD")},
	{0x028C, CSTR("NANOLINK APS")},
	{0x028D, CSTR("IF, LLC")},
	{0x028E, CSTR("RF Digital Corp")},
	{0x028F, CSTR("Church & Dwight Co., Inc")},
	{0x0290, CSTR("Multibit Oy")},
	{0x0291, CSTR("CliniCloud Inc")},
	{0x0292, CSTR("SwiftSensors")},
	{0x0293, CSTR("Blue Bite")},
	{0x0294, CSTR("ELIAS GmbH")},
	{0x0295, CSTR("Sivantos GmbH")},
	{0x0296, CSTR("Petzl")},
	{0x0297, CSTR("storm power ltd")},
	{0x0298, CSTR("EISST Ltd")},
	{0x0299, CSTR("Inexess Technology Simma KG")},
	{0x029A, CSTR("Currant, Inc.")},
	{0x029B, CSTR("C2 Development, Inc.")},
	{0x029C, CSTR("Blue Sky Scientific, LLC")},
	{0x029D, CSTR("ALOTTAZS LABS, LLC")},
	{0x029E, CSTR("Kupson spol. s r.o.")},
	{0x029F, CSTR("Areus Engineering GmbH")},
	{0x02A0, CSTR("Impossible Camera GmbH")},
	{0x02A1, CSTR("InventureTrack Systems")},
	{0x02A2, CSTR("LockedUp")},
	{0x02A3, CSTR("Itude")},
	{0x02A4, CSTR("Pacific Lock Company")},
	{0x02A5, CSTR("Tendyron Corporation ( )")},
	{0x02A6, CSTR("Robert Bosch GmbH")},
	{0x02A7, CSTR("Illuxtron international B.V.")},
	{0x02A8, CSTR("miSport Ltd.")},
	{0x02A9, CSTR("Chargelib")},
	{0x02AA, CSTR("Doppler Lab")},
	{0x02AB, CSTR("BBPOS Limited")},
	{0x02AC, CSTR("RTB Elektronik GmbH & Co. KG")},
	{0x02AD, CSTR("Rx Networks, Inc.")},
	{0x02AE, CSTR("WeatherFlow, Inc.")},
	{0x02AF, CSTR("Technicolor USA Inc.")},
	{0x02B0, CSTR("Bestechnic(Shanghai),Ltd")},
	{0x02B1, CSTR("Raden Inc")},
	{0x02B2, CSTR("JouZen Oy")},
	{0x02B3, CSTR("CLABER S.P.A.")},
	{0x02B4, CSTR("Hyginex, Inc.")},
	{0x02B5, CSTR("HANSHIN ELECTRIC RAILWAY CO.,LTD.")},
	{0x02B6, CSTR("Schneider Electric")},
	{0x02B7, CSTR("Oort Technologies LLC")},
	{0x02B8, CSTR("Chrono Therapeutics")},
	{0x02B9, CSTR("Rinnai Corporation")},
	{0x02BA, CSTR("Swissprime Technologies AG")},
	{0x02BB, CSTR("Koha.,Co.Ltd")},
	{0x02BC, CSTR("Genevac Ltd")},
	{0x02BD, CSTR("Chemtronics")},
	{0x02BE, CSTR("Seguro Technology Sp. z o.o.")},
	{0x02BF, CSTR("Redbird Flight Simulations")},
	{0x02C0, CSTR("Dash Robotics")},
	{0x02C1, CSTR("LINE Corporation")},
	{0x02C2, CSTR("Guillemot Corporation")},
	{0x02C3, CSTR("Techtronic Power Tools Technology Limited")},
	{0x02C4, CSTR("Wilson Sporting Goods")},
	{0x02C5, CSTR("Lenovo (Singapore) Pte Ltd. ( )")},
	{0x02C6, CSTR("Ayatan Sensors")},
	{0x02C7, CSTR("Electronics Tomorrow Limited")},
	{0x02C8, CSTR("VASCO Data Security International, Inc.")},
	{0x02C9, CSTR("PayRange Inc.")},
	{0x02CA, CSTR("ABOV Semiconductor")},
	{0x02CB, CSTR("AINA-Wireless Inc.")},
	{0x02CC, CSTR("Eijkelkamp Soil & Water")},
	{0x02CD, CSTR("BMA ergonomics b.v.")},
	{0x02CE, CSTR("Teva Branded Pharmaceutical Products R&D, Inc.")},
	{0x02CF, CSTR("Anima")},
	{0x02D0, CSTR("3M")},
	{0x02D1, CSTR("Empatica Srl")},
	{0x02D2, CSTR("Afero, Inc.")},
	{0x02D3, CSTR("Powercast Corporation")},
	{0x02D4, CSTR("Secuyou ApS")},
	{0x02D5, CSTR("OMRON Corporation")},
	{0x02D6, CSTR("Send Solutions")},
	{0x02D7, CSTR("NIPPON SYSTEMWARE CO.,LTD.")},
	{0x02D8, CSTR("Neosfar")},
	{0x02D9, CSTR("Fliegl Agrartechnik GmbH")},
	{0x02DA, CSTR("Gilvader")},
	{0x02DB, CSTR("Digi International Inc (R)")},
	{0x02DC, CSTR("DeWalch Technologies, Inc.")},
	{0x02DD, CSTR("Flint Rehabilitation Devices, LLC")},
	{0x02DE, CSTR("Samsung SDS Co., Ltd.")},
	{0x02DF, CSTR("Blur Product Development")},
	{0x02E0, CSTR("University of Michigan")},
	{0x02E1, CSTR("Victron Energy BV")},
	{0x02E2, CSTR("NTT docomo")},
	{0x02E3, CSTR("Carmanah Technologies Corp.")},
	{0x02E4, CSTR("Bytestorm Ltd.")},
	{0x02E5, CSTR("Espressif Incorporated ( () )")},
	{0x02E6, CSTR("Unwire")},
	{0x02E7, CSTR("Connected Yard, Inc.")},
	{0x02E8, CSTR("American Music Environments")},
	{0x02E9, CSTR("Sensogram Technologies, Inc.")},
	{0x02EA, CSTR("Fujitsu Limited")},
	{0x02EB, CSTR("Ardic Technology")},
	{0x02EC, CSTR("Delta Systems, Inc")},
	{0x02ED, CSTR("HTC Corporation")},
	{0x02EE, CSTR("Citizen Holdings Co., Ltd.")},
	{0x02EF, CSTR("SMART-INNOVATION.inc")},
	{0x02F0, CSTR("Blackrat Software")},
	{0x02F1, CSTR("The Idea Cave, LLC")},
	{0x02F2, CSTR("GoPro, Inc.")},
	{0x02F3, CSTR("AuthAir, Inc")},
	{0x02F4, CSTR("Vensi, Inc.")},
	{0x02F5, CSTR("Indagem Tech LLC")},
	{0x02F6, CSTR("Intemo Technologies")},
	{0x02F7, CSTR("DreamVisions co., Ltd.")},
	{0x02F8, CSTR("Runteq Oy Ltd")},
	{0x02F9, CSTR("IMAGINATION TECHNOLOGIES LTD")},
	{0x02FA, CSTR("CoSTAR TEchnologies")},
	{0x02FB, CSTR("Clarius Mobile Health Corp.")},
	{0x02FC, CSTR("Shanghai Frequen Microelectronics Co., Ltd.")},
	{0x02FD, CSTR("Uwanna, Inc.")},
	{0x02FE, CSTR("Lierda Science & Technology Group Co., Ltd.")},
	{0x02FF, CSTR("Silicon Laboratories")},
	{0x0300, CSTR("World Moto Inc.")},
	{0x0301, CSTR("Giatec Scientific Inc.")},
	{0x0302, CSTR("Loop Devices, Inc")},
	{0x0303, CSTR("IACA electronique")},
	{0x0304, CSTR("Martians Inc")},
	{0x0305, CSTR("Swipp ApS")},
	{0x0306, CSTR("Life Laboratory Inc.")},
	{0x0307, CSTR("FUJI INDUSTRIAL CO.,LTD.")},
	{0x0308, CSTR("Surefire, LLC")},
	{0x0309, CSTR("Dolby Labs")},
	{0x030A, CSTR("Ellisys")},
	{0x030B, CSTR("Magnitude Lighting Converters")},
	{0x030C, CSTR("Hilti AG")},
	{0x030D, CSTR("Devdata S.r.l.")},
	{0x030E, CSTR("Deviceworx")},
	{0x030F, CSTR("Shortcut Labs")},
	{0x0310, CSTR("SGL Italia S.r.l.")},
	{0x0311, CSTR("PEEQ DATA")},
	{0x0312, CSTR("Ducere Technologies Pvt Ltd")},
	{0x0313, CSTR("DiveNav, Inc.")},
	{0x0314, CSTR("RIIG AI Sp. z o.o.")},
	{0x0315, CSTR("Thermo Fisher Scientific")},
	{0x0316, CSTR("AG Measurematics Pvt. Ltd.")},
	{0x0317, CSTR("CHUO Electronics CO., LTD.")},
	{0x0318, CSTR("Aspenta International")},
	{0x0319, CSTR("Eugster Frismag AG")},
	{0x031A, CSTR("Amber wireless GmbH")},
	{0x031B, CSTR("HQ Inc")},
	{0x031C, CSTR("Lab Sensor Solutions")},
	{0x031D, CSTR("Enterlab ApS")},
	{0x031E, CSTR("Eyefi, Inc.")},
	{0x031F, CSTR("MetaSystem S.p.A.")},
	{0x0320, CSTR("SONO ELECTRONICS. CO., LTD")},
	{0x0321, CSTR("Jewelbots")},
	{0x0322, CSTR("Compumedics Limited")},
	{0x0323, CSTR("Rotor Bike Components")},
	{0x0324, CSTR("Astro, Inc.")},
	{0x0325, CSTR("Amotus Solutions")},
	{0x0326, CSTR("Healthwear Technologies (Changzhou)Ltd")},
	{0x0327, CSTR("Essex Electronics")},
	{0x0328, CSTR("Grundfos A/S")},
	{0x0329, CSTR("Eargo, Inc.")},
	{0x032A, CSTR("Electronic Design Lab")},
	{0x032B, CSTR("ESYLUX")},
	{0x032C, CSTR("NIPPON SMT.CO.,Ltd")},
	{0x032D, CSTR("BM innovations GmbH")},
	{0x032E, CSTR("indoormap")},
	{0x032F, CSTR("OttoQ Inc")},
	{0x0330, CSTR("North Pole Engineering")},
	{0x0331, CSTR("3flares Technologies Inc.")},
	{0x0332, CSTR("Electrocompaniet A.S.")},
	{0x0333, CSTR("Mul-T-Lock")},
	{0x0334, CSTR("Corentium AS")},
	{0x0335, CSTR("Enlighted Inc")},
	{0x0336, CSTR("GISTIC")},
	{0x0337, CSTR("AJP2 Holdings, LLC")},
	{0x0338, CSTR("COBI GmbH")},
	{0x0339, CSTR("Blue Sky Scientific, LLC")},
	{0x033A, CSTR("Appception, Inc.")},
	{0x033B, CSTR("Courtney Thorne Limited")},
	{0x033C, CSTR("Virtuosys")},
	{0x033D, CSTR("TPV Technology Limited")},
	{0x033E, CSTR("Monitra SA")},
	{0x033F, CSTR("Automation Components, Inc.")},
	{0x0340, CSTR("Letsense s.r.l.")},
	{0x0341, CSTR("Etesian Technologies LLC")},
	{0x0342, CSTR("GERTEC BRASIL LTDA.")},
	{0x0343, CSTR("Drekker Development Pty. Ltd.")},
	{0x0344, CSTR("Whirl Inc")},
	{0x0345, CSTR("Locus Positioning")},
	{0x0346, CSTR("Acuity Brands Lighting, Inc")},
	{0x0347, CSTR("Prevent Biometrics")},
	{0x0348, CSTR("Arioneo")},
	{0x0349, CSTR("VersaMe")},
	{0x034A, CSTR("Vaddio")},
	{0x034B, CSTR("Libratone A/S")},
	{0x034C, CSTR("HM Electronics, Inc.")},
	{0x034D, CSTR("TASER International, Inc.")},
	{0x034E, CSTR("SafeTrust Inc.")},
	{0x034F, CSTR("Heartland Payment Systems")},
	{0x0350, CSTR("Bitstrata Systems Inc.")},
	{0x0351, CSTR("Pieps GmbH")},
	{0x0352, CSTR("iRiding(Xiamen)Technology Co.,Ltd.")},
	{0x0353, CSTR("Alpha Audiotronics, Inc.")},
	{0x0354, CSTR("TOPPAN FORMS CO.,LTD.")},
	{0x0355, CSTR("Sigma Designs, Inc.")},
	{0x0356, CSTR("Spectrum Brands, Inc.")},
	{0x0357, CSTR("Polymap Wireless")},
	{0x0358, CSTR("MagniWare Ltd.")},
	{0x0359, CSTR("Novotec Medical GmbH")},
	{0x035A, CSTR("Medicom Innovation Partner a/s")},
	{0x035B, CSTR("Matrix Inc.")},
	{0x035C, CSTR("Eaton Corporation")},
	{0x035D, CSTR("KYS")},
	{0x035E, CSTR("Naya Health, Inc.")},
	{0x035F, CSTR("Acromag")},
	{0x0360, CSTR("Insulet Corporation")},
	{0x0361, CSTR("Wellinks Inc.")},
	{0x0362, CSTR("ON Semiconductor")},
	{0x0363, CSTR("FREELAP SA")},
	{0x0364, CSTR("Favero Electronics Srl")},
	{0x0365, CSTR("BioMech Sensor LLC")},
	{0x0366, CSTR("BOLTT Sports technologies Private limited")},
	{0x0367, CSTR("Saphe International")},
	{0x0368, CSTR("Metormote AB")},
	{0x0369, CSTR("littleBits")},
	{0x036A, CSTR("SetPoint Medical")},
	{0x036B, CSTR("BRControls Products BV")},
	{0x036C, CSTR("Zipcar")},
	{0x036D, CSTR("AirBolt Pty Ltd")},
	{0x036E, CSTR("KeepTruckin Inc")},
	{0x036F, CSTR("Motiv, Inc.")},
	{0x0370, CSTR("Wazombi Labs O")},
	{0x0371, CSTR("ORBCOMM")},
	{0x0372, CSTR("Nixie Labs, Inc.")},
	{0x0373, CSTR("AppNearMe Ltd")},
	{0x0374, CSTR("Holman Industries")},
	{0x0375, CSTR("Expain AS")},
	{0x0376, CSTR("Electronic Temperature Instruments Ltd")},
	{0x0377, CSTR("Plejd AB")},
	{0x0378, CSTR("Propeller Health")},
	{0x0379, CSTR("Shenzhen iMCO Electronic Technology Co.,Ltd")},
	{0x037A, CSTR("Algoria")},
	{0x037B, CSTR("Apption Labs Inc.")},
	{0x037C, CSTR("Cronologics Corporation")},
	{0x037D, CSTR("MICRODIA Ltd.")},
	{0x037E, CSTR("lulabytes S.L.")},
	{0x037F, CSTR("Nestec S.A.")},
	{0x0380, CSTR("LLC MEGA - F service")},
	{0x0381, CSTR("Sharp Corporation")},
	{0x0382, CSTR("Precision Outcomes Ltd")},
	{0x0383, CSTR("Kronos Incorporated")},
	{0x0384, CSTR("OCOSMOS Co., Ltd.")},
	{0x0385, CSTR("Embedded Electronic Solutions Ltd. dba e2Solutions")},
	{0x0386, CSTR("Aterica Inc.")},
	{0x0387, CSTR("BluStor PMC, Inc.")},
	{0x0388, CSTR("Kapsch TrafficCom AB")},
	{0x0389, CSTR("ActiveBlu Corporation")},
	{0x038A, CSTR("Kohler Mira Limited")},
	{0x038B, CSTR("Noke")},
	{0x038C, CSTR("Appion Inc.")},
	{0x038D, CSTR("Resmed Ltd")},
	{0x038E, CSTR("Crownstone B.V.")},
	{0x038F, CSTR("Xiaomi Inc.")},
	{0x0390, CSTR("INFOTECH s.r.o.")},
	{0x0391, CSTR("Thingsquare AB")},
	{0x0392, CSTR("T&D")},
	{0x0393, CSTR("LAVAZZA S.p.A.")},
	{0x0394, CSTR("Netclearance Systems, Inc.")},
	{0x0395, CSTR("SDATAWAY")},
	{0x0396, CSTR("BLOKS GmbH")},
	{0x0397, CSTR("LEGO System A/S")},
	{0x0398, CSTR("Thetatronics Ltd")},
	{0x0399, CSTR("Nikon Corporation")},
	{0x039A, CSTR("NeST")},
	{0x039B, CSTR("South Silicon Valley Microelectronics")},
	{0x039C, CSTR("ALE International")},
	{0x039D, CSTR("CareView Communications, Inc.")},
	{0x039E, CSTR("SchoolBoard Limited")},
	{0x039F, CSTR("Molex Corporation")},
	{0x03A0, CSTR("IVT Wireless Limited")},
	{0x03A1, CSTR("Alpine Labs LLC")},
	{0x03A2, CSTR("Candura Instruments")},
	{0x03A3, CSTR("SmartMovt Technology Co., Ltd")},
	{0x03A4, CSTR("Token Zero Ltd")},
	{0x03A5, CSTR("ACE CAD Enterprise Co., Ltd. (ACECAD)")},
	{0x03A6, CSTR("Medela, Inc")},
	{0x03A7, CSTR("AeroScout")},
	{0x03A8, CSTR("Esrille Inc.")},
	{0x03A9, CSTR("THINKERLY SRL")},
	{0x03AA, CSTR("Exon Sp. z o.o.")},
	{0x03AB, CSTR("Meizu Technology Co., Ltd.")},
	{0x03AC, CSTR("Smablo LTD")},
	{0x03AD, CSTR("XiQ")},
	{0x03AE, CSTR("Allswell Inc.")},
	{0x03AF, CSTR("Comm-N-Sense Corp DBA Verigo")},
	{0x03B0, CSTR("VIBRADORM GmbH")},
	{0x03B1, CSTR("Otodata Wireless Network Inc.")},
	{0x03B2, CSTR("Propagation Systems Limited")},
	{0x03B3, CSTR("Midwest Instruments & Controls")},
	{0x03B4, CSTR("Alpha Nodus, inc.")},
	{0x03B5, CSTR("petPOMM, Inc")},
	{0x03B6, CSTR("Mattel")},
	{0x03B7, CSTR("Airbly Inc.")},
	{0x03B8, CSTR("A-Safe Limited")},
	{0x03B9, CSTR("FREDERIQUE CONSTANT SA")},
	{0x03BA, CSTR("Maxscend Microelectronics Company Limited")},
	{0x03BB, CSTR("Abbott Diabetes Care")},
	{0x03BC, CSTR("ASB Bank Ltd")},
	{0x03BD, CSTR("amadas")},
	{0x03BE, CSTR("Applied Science, Inc.")},
	{0x03BF, CSTR("iLumi Solutions Inc.")},
	{0x03C0, CSTR("Arch Systems Inc.")},
	{0x03C1, CSTR("Ember Technologies, Inc.")},
	{0x03C2, CSTR("Snapchat Inc")},
	{0x03C3, CSTR("Casambi Technologies Oy")},
	{0x03C4, CSTR("Pico Technology Inc.")},
	{0x03C5, CSTR("St. Jude Medical, Inc.")},
	{0x03C6, CSTR("Intricon")},
	{0x03C7, CSTR("Structural Health Systems, Inc.")},
	{0x03C8, CSTR("Avvel International")},
	{0x03C9, CSTR("Gallagher Group")},
	{0x03CA, CSTR("In2things Automation Pvt. Ltd.")},
	{0x03CB, CSTR("SYSDEV Srl")},
	{0x03CC, CSTR("Vonkil Technologies Ltd")},
	{0x03CD, CSTR("Wynd Technologies, Inc.")},
	{0x03CE, CSTR("CONTRINEX S.A.")},
	{0x03CF, CSTR("MIRA, Inc.")},
	{0x03D0, CSTR("Watteam Ltd")},
	{0x03D1, CSTR("Density Inc.")},
	{0x03D2, CSTR("IOT Pot India Private Limited")},
	{0x03D3, CSTR("Sigma Connectivity AB")},
	{0x03D4, CSTR("PEG PEREGO SPA")},
	{0x03D5, CSTR("Wyzelink Systems Inc.")},
	{0x03D6, CSTR("Yota Devices LTD")},
	{0x03D7, CSTR("FINSECUR")},
	{0x03D8, CSTR("Zen-Me Labs Ltd")},
	{0x03D9, CSTR("3IWare Co., Ltd.")},
	{0x03DA, CSTR("EnOcean GmbH")},
	{0x03DB, CSTR("Instabeat, Inc")},
	{0x03DC, CSTR("Nima Labs")},
	{0x03DD, CSTR("Andreas Stihl AG & Co. KG")},
	{0x03DE, CSTR("Nathan Rhoades LLC")},
	{0x03DF, CSTR("Grob Technologies, LLC")},
	{0x03E0, CSTR("Actions (Zhuhai) Technology Co., Limited")},
	{0x03E1, CSTR("SPD Development Company Ltd")},
	{0x03E2, CSTR("Sensoan Oy")},
	{0x03E3, CSTR("Qualcomm Life Inc")},
	{0x03E4, CSTR("Chip-ing AG")},
	{0x03E5, CSTR("ffly4u")},
	{0x03E6, CSTR("IoT Instruments Oy")},
	{0x03E7, CSTR("TRUE Fitness Technology")},
	{0x03E8, CSTR("Reiner Kartengeraete GmbH & Co. KG.")},
	{0x03E9, CSTR("SHENZHEN LEMONJOY TECHNOLOGY CO., LTD.")},
	{0x03EA, CSTR("Hello Inc.")},
	{0x03EB, CSTR("Evollve Inc.")},
	{0x03EC, CSTR("Jigowatts Inc.")},
	{0x03ED, CSTR("BASIC MICRO.COM,INC.")},
	{0x03EE, CSTR("CUBE TECHNOLOGIES")},
	{0x03EF, CSTR("foolography GmbH")},
	{0x03F0, CSTR("CLINK")},
	{0x03F1, CSTR("Hestan Smart Cooking Inc.")},
	{0x03F2, CSTR("WindowMaster A/S")},
	{0x03F3, CSTR("Flowscape AB")},
	{0x03F4, CSTR("PAL Technologies Ltd")},
	{0x03F5, CSTR("WHERE, Inc.")},
	{0x03F6, CSTR("Iton Technology Corp.")},
	{0x03F7, CSTR("Owl Labs Inc.")},
	{0x03F8, CSTR("Rockford Corp.")},
	{0x03F9, CSTR("Becon Technologies Co.,Ltd.")},
	{0x03FA, CSTR("Vyassoft Technologies Inc")},
	{0x03FB, CSTR("Nox Medical")},
	{0x03FC, CSTR("Kimberly-Clark")},
	{0x03FD, CSTR("Trimble Navigation Ltd.")},
	{0x03FE, CSTR("Littelfuse")},
	{0x03FF, CSTR("Withings")},
	{0x0400, CSTR("i-developer IT Beratung UG")},
	{0x0402, CSTR("Sears Holdings Corporation")},
	{0x0403, CSTR("Gantner Electronic GmbH")},
	{0x0404, CSTR("Authomate Inc")},
	{0x0405, CSTR("Vertex International, Inc.")},
	{0x0406, CSTR("Airtago")},
	{0x0407, CSTR("Swiss Audio SA")},
	{0x0408, CSTR("ToGetHome Inc.")},
	{0x0409, CSTR("AXIS")},
	{0x040A, CSTR("Openmatics")},
	{0x040B, CSTR("Jana Care Inc.")},
	{0x040C, CSTR("Senix Corporation")},
	{0x040D, CSTR("NorthStar Battery Company, LLC")},
	{0x040E, CSTR("SKF (U.K.) Limited")},
	{0x040F, CSTR("CO-AX Technology, Inc.")},
	{0x0410, CSTR("Fender Musical Instruments")},
	{0x0411, CSTR("Luidia Inc")},
	{0x0412, CSTR("SEFAM")},
	{0x0413, CSTR("Wireless Cables Inc")},
	{0x0414, CSTR("Lightning Protection International Pty Ltd")},
	{0x0415, CSTR("Uber Technologies Inc")},
	{0x0416, CSTR("SODA GmbH")},
	{0x0417, CSTR("Fatigue Science")},
	{0x0418, CSTR("Alpine Electronics Inc.")},
	{0x0419, CSTR("Novalogy LTD")},
	{0x041A, CSTR("Friday Labs Limited")},
	{0x041B, CSTR("OrthoAccel Technologies")},
	{0x041C, CSTR("WaterGuru, Inc.")},
	{0x041D, CSTR("Benning Elektrotechnik und Elektronik GmbH & Co. KG")},
	{0x041E, CSTR("Dell Computer Corporation")},
	{0x041F, CSTR("Kopin Corporation")},
	{0x0420, CSTR("TecBakery GmbH")},
	{0x0421, CSTR("Backbone Labs, Inc.")},
	{0x0422, CSTR("DELSEY SA")},
	{0x0423, CSTR("Chargifi Limited")},
	{0x0424, CSTR("Trainesense Ltd.")},
	{0x0425, CSTR("Unify Software and Solutions GmbH & Co. KG")},
	{0x0426, CSTR("Husqvarna AB")},
	{0x0427, CSTR("Focus fleet and fuel management inc")},
	{0x0428, CSTR("SmallLoop, LLC")},
	{0x0429, CSTR("Prolon Inc.")},
	{0x042A, CSTR("BD Medical")},
	{0x042B, CSTR("iMicroMed Incorporated")},
	{0x042C, CSTR("Ticto N.V.")},
	{0x042D, CSTR("Meshtech AS")},
	{0x042E, CSTR("MemCachier Inc.")},
	{0x042F, CSTR("Danfoss A/S")},
	{0x0430, CSTR("SnapStyk Inc.")},
	{0x0431, CSTR("Amyway Corporation")},
	{0x0432, CSTR("Silk Labs, Inc.")},
	{0x0433, CSTR("Pillsy Inc.")},
	{0x0434, CSTR("Hatch Baby, Inc.")},
	{0x0435, CSTR("Blocks Wearables Ltd.")},
	{0x0436, CSTR("Drayson Technologies (Europe) Limited")},
	{0x0437, CSTR("eBest IOT Inc.")},
	{0x0438, CSTR("Helvar Ltd")},
	{0x0439, CSTR("Radiance Technologies")},
	{0x043A, CSTR("Nuheara Limited")},
	{0x043B, CSTR("Appside co., ltd.")},
	{0x043C, CSTR("DeLaval")},
	{0x043D, CSTR("Coiler Corporation")},
	{0x043E, CSTR("Thermomedics, Inc.")},
	{0x043F, CSTR("Tentacle Sync GmbH")},
	{0x0440, CSTR("Valencell, Inc.")},
	{0x0441, CSTR("iProtoXi Oy")},
	{0x0442, CSTR("SECOM CO., LTD.")},
	{0x0443, CSTR("Tucker International LLC")},
	{0x0444, CSTR("Metanate Limited")},
	{0x0445, CSTR("Kobian Canada Inc.")},
	{0x0446, CSTR("NETGEAR, Inc.")},
	{0x0447, CSTR("Fabtronics Australia Pty Ltd")},
	{0x0448, CSTR("Grand Centrix GmbH")},
	{0x0449, CSTR("1UP USA.com llc")},
	{0x044A, CSTR("SHIMANO INC.")},
	{0x044B, CSTR("Nain Inc.")},
	{0x044C, CSTR("LifeStyle Lock, LLC")},
	{0x044D, CSTR("VEGA Grieshaber KG")},
	{0x044E, CSTR("Xtrava Inc.")},
	{0x044F, CSTR("TTS Tooltechnic Systems AG & Co. KG")},
	{0x0450, CSTR("Teenage Engineering AB")},
	{0x0451, CSTR("Tunstall Nordic AB")},
	{0x0452, CSTR("Svep Design Center AB")},
	{0x0453, CSTR("GreenPeak Technologies BV")},
	{0x0454, CSTR("Sphinx Electronics GmbH & Co KG")},
	{0x0455, CSTR("Atomation")},
	{0x0456, CSTR("Nemik Consulting Inc")},
	{0x0457, CSTR("RF INNOVATION")},
	{0x0458, CSTR("Mini Solution Co., Ltd.")},
	{0x0459, CSTR("Lumenetix, Inc")},
	{0x045A, CSTR("2048450 Ontario Inc")},
	{0x045B, CSTR("SPACEEK LTD")},
	{0x045C, CSTR("Delta T Corporation")},
	{0x045D, CSTR("Boston Scientific Corporation")},
	{0x045E, CSTR("Nuviz, Inc.")},
	{0x045F, CSTR("Real Time Automation, Inc.")},
	{0x0460, CSTR("Kolibree")},
	{0x0461, CSTR("vhf elektronik GmbH")},
	{0x0462, CSTR("Bonsai Systems GmbH")},
	{0x0463, CSTR("Fathom Systems Inc.")},
	{0x0464, CSTR("Bellman & Symfon")},
	{0x0465, CSTR("International Forte Group LLC")},
	{0x0466, CSTR("CycleLabs Solutions inc.")},
	{0x0467, CSTR("Codenex Oy")},
	{0x0468, CSTR("Kynesim Ltd")},
	{0x0469, CSTR("Palago AB")},
	{0x046A, CSTR("INSIGMA INC.")},
	{0x046B, CSTR("PMD Solutions")},
	{0x046C, CSTR("Qingdao Realtime Technology Co., Ltd.")},
	{0x046D, CSTR("BEGA Gantenbrink-Leuchten KG")},
	{0x046E, CSTR("Pambor Ltd.")},
	{0xFFFF, CSTR("SPECIAL USE/DEFAULT")},
};

void Net::PacketAnalyzerBluetooth::AddDirection(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 dir)
{
	switch (dir)
	{
	case 0:
		frame->AddField(frameOfst, 4, CSTR("Direction"), CSTR("Sent"));
		break;
	case 1:
		frame->AddField(frameOfst, 4, CSTR("Direction"), CSTR("Rcvd"));
		break;
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Unknown (0x"));
			sb.AppendHex32(dir);
			sb.AppendUTF8Char(')');
			frame->AddField(frameOfst, 4, CSTR("Direction"), sb.ToCString());
		}
		break;
	}
}

void Net::PacketAnalyzerBluetooth::AddHCIPacketType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 packetType)
{
	frame->AddHex8Name(frameOfst, CSTR("HCI Packet Type"), packetType, HCIPacketTypeGetName(packetType));
}

void Net::PacketAnalyzerBluetooth::AddParamLen(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len)
{
	frame->AddUInt(frameOfst, 1, CSTR("Parameter Total Length"), len);
}

void Net::PacketAnalyzerBluetooth::AddCmdOpcode(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 cmd)
{
	frame->AddHex16Name(frameOfst, CSTR("Command Opcode"), cmd, CmdGetName(cmd));
}

void Net::PacketAnalyzerBluetooth::AddBDAddr(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UnsafeArray<const UInt8> mac, Bool randomAddr)
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

void Net::PacketAnalyzerBluetooth::AddScanType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType)
{
	Text::CString vName = CSTR_NULL;
	switch (scanType)
	{
	case 1:
		vName = CSTR("Active");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Scan Type"), scanType, vName);
}

void Net::PacketAnalyzerBluetooth::AddScanFilterPolicy(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 scanType)
{
	Text::CString vName = CSTR_NULL;
	switch (scanType)
	{
	case 0:
		vName = CSTR("Accept all advertisements, except directed advertisements not addressed to this device");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Scan Filter Policy"), scanType, vName);
}

void Net::PacketAnalyzerBluetooth::AddScanInterval(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanInt)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanInt);
	sb.AppendC(UTF8STRC(" ("));
	sb.AppendDouble(0.625 * scanInt);
	sb.AppendC(UTF8STRC("ms)"));
	frame->AddField(frameOfst, 2, CSTR("Scan Interval"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddScanWindow(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 scanWind)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanWind);
	sb.AppendC(UTF8STRC(" ("));
	sb.AppendDouble(0.625 * scanWind);
	sb.AppendC(UTF8STRC("ms)"));
	frame->AddField(frameOfst, 2, CSTR("Scan Window"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddAddrType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 addrType)
{
	Text::CString vName = CSTR_NULL;
	switch (addrType)
	{
	case 0:
		vName = CSTR("Public Device Address");
		break;
	case 1:
		vName = CSTR("Random Address Type");
		break;
	}
	frame->AddHex8Name(frameOfst, name, addrType, vName);
}

void Net::PacketAnalyzerBluetooth::AddHCIEvent(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evt)
{
	frame->AddHex8Name(frameOfst, CSTR("Event Code"), evt, HCIEventGetName(evt));
}

void Net::PacketAnalyzerBluetooth::AddLESubEvent(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 subEvt)
{
	frame->AddHex8Name(frameOfst, CSTR("Sub Event"), subEvt, LESubEventGetName(subEvt));
}

void Net::PacketAnalyzerBluetooth::AddAdvEvtType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 evtType)
{
	Text::CString vName = CSTR_NULL;
	switch (evtType)
	{
	case 0:
		vName = CSTR("Connectable Undirected Advertising");
		break;
	case 3:
		vName = CSTR("Non-Connectable Undirected Advertising");
		break;
	case 4:
		vName = CSTR("Scan Response");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Event Type"), evtType, vName);
}

void Net::PacketAnalyzerBluetooth::AddRSSI(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 rssi)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(Text::StrInt16(sbuff, rssi), UTF8STRC("dBm"));
	frame->AddField(frameOfst, 1, CSTR("RSSI"), CSTRP(sbuff, sptr));
}

void Net::PacketAnalyzerBluetooth::AddAdvData(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UnsafeArray<const UInt8> packet, UOSInt packetSize)
{
	Text::CString vName;
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
			frame->AddField(frameOfst + i, (UInt32)packetSize - i, CSTR("Unused"), CSTR_NULL);
			return;
		}
		frame->AddUInt(frameOfst + i, 1, CSTR("Adv Item Len"), len);
		frame->AddHex8(frameOfst + i + 1, CSTR("Adv Item Type"), packet[i + 1]);
		if (packet[i + 1] == 1 && len == 2)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex8(packet[i + 2]);
			Bool found = false;
			if (packet[i + 2] & 1)
			{
				sb.AppendC(UTF8STRC(" LE Limited Discoverable Mode"));
				found = true;
			}
			if (packet[i + 2] & 2)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" LE General Discoverable Mode"));
				found = true;
			}
			if (packet[i + 2] & 4)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" BR/EDR Not Supported"));
				found = true;
			}
			if (packet[i + 2] & 8)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" Simultaneous LE and BR/EDR to Same Device Capable (Controller)"));
				found = true;
			}
			if (packet[i + 2] & 16)
			{
				if (found) sb.AppendUTF8Char(',');
				sb.AppendC(UTF8STRC(" Simultaneous LE and BR/EDR to Same Device Capable (Host)"));
				found = true;
			}
			frame->AddField(frameOfst + i + 2, 1, CSTR("Flags"), sb.ToCString());
		}
		else if (packet[i + 1] == 2 && len == 3)
		{
			u16 = ReadUInt16(&packet[i + 2]);
			vName = CSTR_NULL;
			switch (u16)
			{
			case 0xFE24:
				vName = CSTR("August Home Inc");
				break;
			}
			frame->AddHex16Name(frameOfst + i + 2, CSTR("16-bit Service Class UUIDs (incomplete)"), u16, vName);
		}
		else if (packet[i + 1] == 3 && len >= 3 && len & 1)
		{
			j = 2;
			while (j < len)
			{
				switch (ReadUInt16(&packet[i + j]))
				{
				case 0x0001:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("SDP"));
					break;
				case 0x0003:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("RFCOMM"));
					break;
				case 0x0100:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("L2CAP"));
					break;
				case 0x1101:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("Serial Port"));
					break;
				case 0x111F:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("Handsfree Audio Gateway"));
					break;
				case 0x1200:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("PnP Information"));
					break;
				case 0x1203:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("Generic Audio"));
					break;
				case 0xFE8F:
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), CSTR("CSR"));
					break;
				default:
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Unknown(0x"));
					sb.AppendHex16(ReadUInt16(&packet[i + j]));
					sb.AppendUTF8Char(')');
					frame->AddField(frameOfst + i + j, 2, CSTR("16-bit Service Class UUIDs"), sb.ToCString());
					break;
				}
				j += 2;
			}
		}
		else if (packet[i + 1] == 6 && len == 17)
		{
			sb.ClearStr();
			sb.AppendHex32(ReadUInt32(&packet[i + 14]));
			sb.AppendUTF8Char('-');
			sb.AppendHex16(ReadUInt16(&packet[i + 12]));
			sb.AppendUTF8Char('-');
			sb.AppendHex16(ReadUInt16(&packet[i + 10]));
			sb.AppendUTF8Char('-');
			sb.AppendHex16(ReadUInt16(&packet[i + 8]));
			sb.AppendUTF8Char('-');
			sb.AppendHex32(ReadUInt32(&packet[i + 4]));
			sb.AppendHex16(ReadUInt16(&packet[i + 2]));
			frame->AddField(frameOfst + i + 2, 16, CSTR("128-bit Service Class UUIDs (incomplete)"), sb.ToCString());
		}
		else if (packet[i + 1] == 8)
		{
			sb.ClearStr();
			sb.AppendC(&packet[i + 2], (UOSInt)len - 1);
			frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, CSTR("Device Name (shortened)"), sb.ToCString());
		}
		else if (packet[i + 1] == 9)
		{
			sb.ClearStr();
			sb.AppendC(&packet[i + 2], (UOSInt)len - 1);
			frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, CSTR("Device Name"), sb.ToCString());
		}
		else if (packet[i + 1] == 0x0A && len == 2)
		{
			sb.ClearStr();
			sb.AppendU16(packet[i + 2]);
			sb.AppendC(UTF8STRC("dBm"));
			frame->AddField(frameOfst + i + 2, 1, CSTR("Tx Power Level"), sb.ToCString());
		}
		else if (packet[i + 1] == 0xff && len >= 3)
		{
			UInt16 compId = ReadUInt16(&packet[i + 2]);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex16(compId);
			Text::CStringNN cstr;
			if (CompanyGetName(compId).SetTo(cstr))
			{
				sb.AppendC(UTF8STRC(" ("));
				sb.Append(cstr);
				sb.AppendUTF8Char(')');
			}
			else
			{
				sb.AppendC(UTF8STRC(" (Unknown)"));
			}
			if (compId == 0x4C)
			{
				/////////////////// https://github.com/furiousMAC/continuity/tree/master/messages
				frame->AddField(frameOfst + i + 2, 2, CSTR("Manufacturer Specific"), sb.ToCString());
				j = 4;
				while (j < (UInt32)len - 1)
				{
					vName = CSTR_NULL;
					switch (packet[i + j])
					{
					case 2:
						vName = CSTR("Proximity Beacons");
						break;
					case 3:
						vName = CSTR("AirPrint Message");
						break;
					case 5:
						vName = CSTR("AirDrop Message");
						break;
					case 6:
						vName = CSTR("HomeKit Message");
						break;
					case 7:
						vName = CSTR("Proximity Pairing Message");
						break;
					case 8:
						vName = CSTR("Hey Siri Message");
						break;
					case 9:
						vName = CSTR("Airplay Target Message");
						break;
					case 10:
						vName = CSTR("Airplay Source Message");
						break;
					case 11:
						vName = CSTR("Magic Switch Message");
						break;
					case 12:
						vName = CSTR("Handoff Message");
						break;
					case 13:
						vName = CSTR("Tethering Target Message");
						break;
					case 14:
						vName = CSTR("Tethering Source Message");
						break;
					case 15:
						vName = CSTR("Nearby Action");
						break;
					case 16:
						vName = CSTR("Nearby Info");
						break;
					case 18:
						vName = CSTR("Find My Message");
						break;
					}
					frame->AddHex8Name(frameOfst + i + j, CSTR("Apple Type"), packet[i + j], vName);
					frame->AddUInt(frameOfst + i + j + 1, 1, CSTR("Apple Length"), packet[i + j + 1]);
					UInt8 appLen = packet[i + j + 1] & 0x7F;
					if (appLen + j + 1 <= len)
					{
						if (packet[i + j] == 2 && appLen == 21)
						{
							sb.ClearStr();
							sb.AppendHex32(ReadUInt32(&packet[i + j + 14]));
							sb.AppendUTF8Char('-');
							sb.AppendHex16(ReadUInt16(&packet[i + j + 12]));
							sb.AppendUTF8Char('-');
							sb.AppendHex16(ReadUInt16(&packet[i + j + 10]));
							sb.AppendUTF8Char('-');
							sb.AppendHex16(ReadUInt16(&packet[i + j + 8]));
							sb.AppendUTF8Char('-');
							sb.AppendHex32(ReadUInt32(&packet[i + j + 4]));
							sb.AppendHex16(ReadUInt16(&packet[i + j + 2]));
							frame->AddField(frameOfst + i + j + 2, 16, CSTR("Proximity UUID"), sb.ToCString());
							frame->AddHex16(frameOfst + i + j + 18, CSTR("Major"), ReadUInt16(&packet[i + j + 18]));
							frame->AddHex16(frameOfst + i + j + 20, CSTR("Minor"), ReadUInt16(&packet[i + j + 20]));
							frame->AddInt(frameOfst + i + j + 22, 1, CSTR("Measured Power"), (Int8)packet[i + j + 22]);
						}
						else if (packet[i + j] == 5 && appLen == 18)
						{
							frame->AddHexBuff(frameOfst + i + j + 2, 8, CSTR("Zeros"), &packet[i + j + 2], false);
							frame->AddUInt(frameOfst + i + j + 10, 1, CSTR("AirDrop Version"), packet[i + j + 10]);
							frame->AddHex16(frameOfst + i + j + 11, CSTR("Short Hash of Apple ID"), ReadMUInt16(&packet[i + j + 11]));
							frame->AddHex16(frameOfst + i + j + 13, CSTR("Short Hash of Phone Number"), ReadMUInt16(&packet[i + j + 13]));
							frame->AddHex16(frameOfst + i + j + 15, CSTR("Short Hash of Email"), ReadMUInt16(&packet[i + j + 15]));
							frame->AddHex16(frameOfst + i + j + 17, CSTR("Short Hash of Email2"), ReadMUInt16(&packet[i + j + 17]));
							frame->AddHexBuff(frameOfst + i + j + 19, 1, CSTR("Zero"), &packet[i + j + 19], false);
						}
						else if (packet[i + j] == 6 && appLen == 13)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Status"), packet[i + j + 2]);
							frame->AddMACAddr(frameOfst + i + j + 3, CSTR("Device ID"), &packet[i + j + 3], false);
							frame->AddUInt(frameOfst + i + j + 9, 2, CSTR("Category"), ReadUInt16(&packet[i + j + 9]));
							frame->AddUInt(frameOfst + i + j + 11, 2, CSTR("Global State Number"), ReadUInt16(&packet[i + j + 11]));
							frame->AddUInt(frameOfst + i + j + 13, 1, CSTR("Configuration Number"), packet[i + j + 13]);
							frame->AddUInt(frameOfst + i + j + 14, 1, CSTR("Compatible Version"), packet[i + j + 14]);
						}
						else if (packet[i + j] == 7 && appLen >= 9)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Unknown"), packet[i + j + 2]);
							frame->AddHex16(frameOfst + i + j + 3, CSTR("Device Model"), ReadUInt16(&packet[i + j + 3]));
							frame->AddHex8(frameOfst + i + j + 5, CSTR("Status"), packet[i + j + 5]);
							frame->AddHex16(frameOfst + i + j + 6, CSTR("Battery Levels"), ReadUInt16(&packet[i + j + 6]));
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 6] & 15) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Left Battery Level"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 6] >> 4) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Right Battery Level"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 15) * 10);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Case Battery Level"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x10) >> 4);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Left Charging"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x20) >> 5);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Right Charging"), sb.ToCString());
							sb.ClearStr();
							sb.AppendU32((UInt32)(packet[i + j + 7] & 0x40) >> 6);
							frame->AddSubfield(frameOfst + i + j + 6, 2, CSTR("Case Charging"), sb.ToCString());
							frame->AddUInt(frameOfst + i + j + 8, 1, CSTR("Open Count"), packet[i + j + 8]);
							frame->AddHex8(frameOfst + i + j + 9, CSTR("Device Color"), packet[i + j + 9]);
							frame->AddHex8(frameOfst + i + j + 10, CSTR("Unknown"), packet[i + j + 10]);
							if (appLen > 9)
							{
								frame->AddHexBuff(frameOfst + i + j + 11, (UOSInt)appLen - 9, CSTR("Encrypted Data"), &packet[i + j + 11], false);
							}
						}
						else if (packet[i + j] == 9 && appLen == 6)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Flags"), packet[i + j + 2]);
							frame->AddHex8(frameOfst + i + j + 3, CSTR("Seed"), packet[i + j + 3]);
							frame->AddIPv4(frameOfst + i + j + 4, CSTR("AirPlay IPv4 Address"), &packet[i + j + 4]);
						}
						else if (packet[i + j] == 10 && appLen == 1)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Data"), packet[i + j + 2]);
						}
						else if (packet[i + j] == 12 && appLen== 14)
						{
							frame->AddBool(frameOfst + i + j + 2, CSTR("Copy/Cut Performed"), packet[i + j + 2]);
							frame->AddUInt(frameOfst + i + j + 3, 2, CSTR("Sequence Number"), ReadUInt16(&packet[i + j + 3]));
							frame->AddHex8(frameOfst + i + j + 5, CSTR("AES-GCM Auth Tag"), packet[i + j + 5]);
							frame->AddHexBuff(frameOfst + i + j + 6, 10, CSTR("Encrypted Handoff Data"), &packet[i + j + 6], false);
						}
						else if (packet[i + j] == 15 && appLen >= 2)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Flags"), packet[i + j + 2]);
							vName = CSTR_NULL;
							switch (packet[i + j + 3])
							{
							case 1:
								vName = CSTR("Apple TV Setup");
								break;
							case 4:
								vName = CSTR("Mobile Backup");
								break;
							case 0x05:
								vName = CSTR("Watch Setup");
								break;
							case 0x06:
								vName = CSTR("Apple TV Pair");
								break;
							case 0x07:
								vName = CSTR("Internet Relay");
								break;
							case 0x08:
								vName = CSTR("WiFi Password");
								break;
							case 0x09:
								vName = CSTR("iOS Setup");
								break;
							case 0x0A:
								vName = CSTR("Repair");
								break;
							case 0x0B:
								vName = CSTR("Speaker Setupd");
								break;
							case 0x0C:
								vName = CSTR("Apple Pay");
								break;
							case 0x0D:
								vName = CSTR("Whole Home Audio Setup");
								break;
							case 0x0E:
								vName = CSTR("Developer Tools Pairing Request");
								break;
							case 0x0F:
								vName = CSTR("Answered Call");
								break;
							case 0x10:
								vName = CSTR("Ended Call");
								break;
							case 0x11:
								vName = CSTR("DD Ping");
								break;
							case 0x12:
								vName = CSTR("DD Pong");
								break;
							case 0x13:
								vName = CSTR("Remote Auto Fill");
								break;
							case 0x14:
								vName = CSTR("Companion Link Proximity");
								break;
							case 0x15:
								vName = CSTR("Remote Management");
								break;
							case 0x16:
								vName = CSTR("Remote Auto Fill Pong");
								break;
							case 0x17:
								vName = CSTR("Remote Display");
								break;
							}
							frame->AddUIntName(frameOfst + i + j + 3, 1, CSTR("Action Type"), packet[i + j + 3], vName);
							if (appLen >= 5)
							{
								frame->AddHexBuff(frameOfst + i + j + 4, 3, CSTR("Authentication Tag"), &packet[i + j + 4], false);
							}
						}
						else if (packet[i + j] == 16 && appLen >= 2)
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("0x"));
							sb.AppendHex8(packet[i + j + 2]);
							switch (packet[i + j + 2] & 15)
							{
							case 0:
								sb.AppendC(UTF8STRC(" (Activity Level Unknown)"));
								break;
							case 1:
								sb.AppendC(UTF8STRC(" (Activity Reporting Disabled )"));
								break;
							case 3:
								sb.AppendC(UTF8STRC(" (Idle User)"));
								break;
							case 5:
								sb.AppendC(UTF8STRC(" (Audio playing while screen locked)"));
								break;
							case 7:
								sb.AppendC(UTF8STRC(" (Active user (screen on) )"));
								break;
							case 9:
								sb.AppendC(UTF8STRC(" (Screen on with video playing)"));
								break;
							case 10:
								sb.AppendC(UTF8STRC(" (Watch on wrist and unlocked)"));
								break;
							case 11:
								sb.AppendC(UTF8STRC(" (Recent user interaction)"));
								break;
							case 13:
								sb.AppendC(UTF8STRC(" (User is driving a vehicle)"));
								break;
							case 14:
								sb.AppendC(UTF8STRC(" (Phone or Facetime Call)"));
								break;
							default:
								sb.AppendC(UTF8STRC(" (Unknown)"));
								break;
							}
							if (packet[i + j + 2] & 0x10)
							{
								sb.AppendC(UTF8STRC(", primary iCloud account device"));
							}
							if (packet[i + j + 2] & 0x20)
							{
								sb.AppendC(UTF8STRC(", Unknown"));
							}
							if (packet[i + j + 2] & 0x40)
							{
								sb.AppendC(UTF8STRC(", AirDrop Receiving is enabled"));
							}
							if (packet[i + j + 2] & 0x80)
							{
								sb.AppendC(UTF8STRC(", Unknown"));
							}
							frame->AddField(frameOfst + i + j + 2, 1, CSTR("Action"), sb.ToCString());
							frame->AddHex8(frameOfst + i + j + 3, CSTR("Status"), packet[i + j + 3]);
							if (packet[i + j + 3] & 2)
							{
								if (appLen >= 6)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, 4, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								else if (appLen > 2)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, (UOSInt)appLen - 2, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								if (appLen >= 7)
								{
									frame->AddHex8(frameOfst + i + j + 8, CSTR("Post Auth Tag Data"), packet[i + j + 8]);
								}
							}
							else
							{
								if (appLen >= 5)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, 3, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								else if (appLen > 2)
								{
									frame->AddHexBuff(frameOfst + i + j + 4, (UOSInt)appLen - 2, CSTR("Authentication Tag"), &packet[i + j + 4], false);
								}
								if (appLen >= 6)
								{
									frame->AddHex8(frameOfst + i + j + 7, CSTR("Post Auth Tag Data"), packet[i + j + 7]);
								}
							}
						}
						else if (packet[i + j] == 18 && appLen >= 25)
						{
							frame->AddHex8(frameOfst + i + j + 2, CSTR("Status"), packet[i + j + 2]);
							frame->AddHexBuff(frameOfst + i + j + 3, 22, CSTR("Byte 6-27 of Public Key"), &packet[i + j + 3], false);
							frame->AddHex8(frameOfst + i + j + 25, CSTR("Byte 0, Bit 6-7 of public key"), packet[i + j + 25]);
							frame->AddHex8(frameOfst + i + j + 26, CSTR("Hint (Byte 5 of BD_ADDR)"), packet[i + j + 26]);
						}
						else if (packet[i + j] == 19)
						{
							frame->AddHexBuff(frameOfst + i + j + 2, appLen, CSTR("Unknown"), &packet[i + j + 2], false);
						}
					}
					j += (UInt32)appLen + 2;
				}
			}
			else
			{
				sb.AppendC(UTF8STRC(", Value="));
				sb.AppendHexBuff(&packet[i + 4], (UOSInt)len - 3, ' ', Text::LineBreakType::None);
				frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, CSTR("Manufacturer Specific"), sb.ToCString());
			}
		}
		else
		{
			sb.ClearStr();
			sb.AppendHexBuff(packet + i + 2, (UOSInt)len - 1, ' ', Text::LineBreakType::None);
			frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, CSTR("Adv Item Value"), sb.ToCString());
		}
		i += (UInt32)len + 1;
	}
}

void Net::PacketAnalyzerBluetooth::AddLAP(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 lap)
{
	frame->AddHex24(frameOfst, CSTR("LAP"), lap);
}

void Net::PacketAnalyzerBluetooth::AddInquiryLen(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 len)
{
	Text::CString vName = CSTR_NULL;
	switch (len)
	{
	case 4:
		vName = CSTR("5.12 sec");
		break;
	}
	frame->AddUIntName(frameOfst, 1, CSTR("Inquiry Length"), len, vName);
}

void Net::PacketAnalyzerBluetooth::AddPageScanMode(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 mode)
{
	Text::CString vName = CSTR_NULL;
	switch (mode)
	{
	case 1:
		vName = CSTR("R1");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Page Scan Repetition Mode"), mode, vName);
}

void Net::PacketAnalyzerBluetooth::AddClassOfDevice(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt32 cls)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex24(cls);
	sb.AppendC(UTF8STRC(", Class="));
	switch (cls & 31)
	{
	case 2:
		sb.AppendC(UTF8STRC("Phone"));
		break;
	default:
		sb.AppendU16(cls & 31);
		sb.AppendC(UTF8STRC(" (Unknown)"));
		break;
	}
	if (cls & 0x20)
	{
		sb.AppendC(UTF8STRC(", Limited Discoverable Mode"));
	}
	if (cls & 0x100)
	{
		sb.AppendC(UTF8STRC(", Positioning"));
	}
	if (cls & 0x200)
	{
		sb.AppendC(UTF8STRC(", Networking"));
	}
	if (cls & 0x400)
	{
		sb.AppendC(UTF8STRC(", Rendering"));
	}
	if (cls & 0x800)
	{
		sb.AppendC(UTF8STRC(", Capturing"));
	}
	if (cls & 0x1000)
	{
		sb.AppendC(UTF8STRC(", Object Transfer"));
	}
	if (cls & 0x2000)
	{
		sb.AppendC(UTF8STRC(", Audio"));
	}
	if (cls & 0x4000)
	{
		sb.AppendC(UTF8STRC(", Telephony"));
	}
	if (cls & 0x8000)
	{
		sb.AppendC(UTF8STRC(", Information"));
	}
	sb.AppendC(UTF8STRC(", Format="));
	sb.AppendU32((cls >> 16) & 3);
	sb.AppendC(UTF8STRC(", Minor Class="));
	sb.AppendU32((cls >> 18) & 0x3F);
	frame->AddField(frameOfst, 3, CSTR("Class Of Device"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddClockOffset(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 ofst)
{
	frame->AddHex16(frameOfst, CSTR("Clock Offset"), ofst);
}

void Net::PacketAnalyzerBluetooth::AddExAdvEvtType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 evtType)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("0x"));
	sb.AppendHex16(evtType);
	if (evtType & 1)
	{
		sb.AppendC(UTF8STRC(", Connectable"));
	}
	if (evtType & 2)
	{
		sb.AppendC(UTF8STRC(", Scannable"));
	}
	if (evtType & 4)
	{
		sb.AppendC(UTF8STRC(", Directed"));
	}
	if (evtType & 8)
	{
		sb.AppendC(UTF8STRC(", Scan Response"));
	}
	if (evtType & 16)
	{
		sb.AppendC(UTF8STRC(", Legacy"));
	}
	sb.AppendC(UTF8STRC(", Data Status="));
	sb.AppendUOSInt((((UOSInt)evtType) >> 5) & 3);
	switch ((((UOSInt)evtType) >> 5) & 3)
	{
	case 0:
		sb.AppendC(UTF8STRC(" (Complete)"));
		break;
	default:
		sb.AppendC(UTF8STRC(" (Unknown)"));
		break;
	}
	frame->AddField(frameOfst, 2, CSTR("Event Type"), sb.ToCString());
}

void Net::PacketAnalyzerBluetooth::AddPHYType(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Text::CStringNN name, UInt8 phyType)
{
	Text::CString vName = CSTR_NULL;
	switch (phyType)
	{
	case 0:
		vName = CSTR("No packets");
		break;
	case 1:
		vName = CSTR("LE 1M");
		break;
	}
	frame->AddHex8Name(frameOfst, name, phyType, vName);
}

void Net::PacketAnalyzerBluetooth::AddAdvSID(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt8 sid)
{
	Text::CString vName = CSTR_NULL;
	switch (sid)
	{
	case 0xFF:
		vName = CSTR("Not available");
		break;
	}
	frame->AddHex8Name(frameOfst, CSTR("Advertising SID"), sid, vName);
}

void Net::PacketAnalyzerBluetooth::AddTxPower(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, Int8 txPower)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(Text::StrInt16(sbuff, txPower), UTF8STRC("dBm"));
	frame->AddField(frameOfst, 1, CSTR("TxPower"), CSTRP(sbuff, sptr));
}

void Net::PacketAnalyzerBluetooth::AddPeriodicAdv(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UInt16 interval)
{
	Text::CString vName = CSTR_NULL;
	switch (interval)
	{
	case 0:
		vName = CSTR("No periodic advertising");
		break;
	}
	frame->AddHex16Name(frameOfst, CSTR("Periodic Advertising Interval"), interval, vName);
}

void Net::PacketAnalyzerBluetooth::AddUnknown(NN<IO::FileAnalyse::FrameDetailHandler> frame, UInt32 frameOfst, UnsafeArray<const UInt8> packet, UOSInt packetSize)
{
	frame->AddHexBuff(frameOfst, (UInt32)packetSize, CSTR("Unknown"), packet, true);
}

Bool Net::PacketAnalyzerBluetooth::PacketGetName(UnsafeArray<const UInt8> packet, UOSInt packetSize, NN<Text::StringBuilderUTF8> sb)
{
	UInt8 mac[6];
	Text::CStringNN name;
	switch (packet[4])
	{
	case 1:
		if (CmdGetName(ReadUInt16(&packet[5])).SetTo(name))
		{
			sb->Append(name);
		}
		else
		{
			sb->AppendC(UTF8STRC("Bluetooth"));
		}
		return true;
	case 2:
		sb->AppendC(UTF8STRC("HCI ACL Packet"));
		return true;
	case 4:
		switch (packet[5])
		{
		case 1:
			if (packet[7] == 0)
			{
				sb->AppendC(UTF8STRC("HCI Inquiry Success"));
			}
			else
			{
				sb->AppendC(UTF8STRC("HCI Inquiry Complete"));
			}
			return true;
		case 0x0E:
			if (CmdGetName(ReadUInt16(&packet[8])).SetTo(name))
			{
				sb->Append(name);
				sb->AppendC(UTF8STRC(" Accept"));
			}
			else
			{
				sb->AppendC(UTF8STRC("Bluetooth"));
			}
			return true;
		case 0x0F:
			if (CmdGetName(ReadUInt16(&packet[9])).SetTo(name))
			{
				if (packet[7] == 0)
				{
					sb->Append(name);
					sb->AppendC(UTF8STRC(" Pending"));
				}
				else
				{
					sb->AppendC(UTF8STRC("Bluetooth"));
				}
			}
			else
			{
				sb->AppendC(UTF8STRC("Bluetooth"));
			}
			return true;
		case 0x2F:
			sb->AppendC(UTF8STRC("HCI Result: "));
			mac[0] = packet[13];
			mac[1] = packet[12];
			mac[2] = packet[11];
			mac[3] = packet[10];
			mac[4] = packet[9];
			mac[5] = packet[8];
			sb->AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
			return true;
		case 0x3E:
			switch (packet[7])
			{
			case 0x2:
				sb->AppendC(UTF8STRC("LE Adv: "));
				mac[0] = packet[16];
				mac[1] = packet[15];
				mac[2] = packet[14];
				mac[3] = packet[13];
				mac[4] = packet[12];
				mac[5] = packet[11];
				sb->AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
				return true;
			case 0xd:
				sb->AppendC(UTF8STRC("LE AdvEx: "));
				mac[0] = packet[17];
				mac[1] = packet[16];
				mac[2] = packet[15];
				mac[3] = packet[14];
				mac[4] = packet[13];
				mac[5] = packet[12];
				sb->AppendHexBuff(mac, 6, ':', Text::LineBreakType::None);
				return true;
			default:
				sb->AppendC(UTF8STRC("Bluetooth"));
				return true;
			}
		default:
			sb->AppendC(UTF8STRC("Bluetooth"));
			return true;
		}
	default:
		sb->AppendC(UTF8STRC("Bluetooth"));
		return true;
	}
}

void Net::PacketAnalyzerBluetooth::PacketGetDetail(UnsafeArray<const UInt8> packet, UOSInt packetSize, NN<Text::StringBuilderUTF8> sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketGetDetail(packet, packetSize, 0, frame);
}

void Net::PacketAnalyzerBluetooth::PacketGetDetail(UnsafeArray<const UInt8> packet, UOSInt packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame)
{
	Text::CString vName;
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
						frame->AddUInt(frameOfst + 12, 1, CSTR("Num Response"), packet[12]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x2005:
					if (cmdLen == 6)
					{
						AddBDAddr(frame, frameOfst + 8, CSTR("BD_ADDR"), packet + 8, true);
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
						AddAddrType(frame, frameOfst + 13, CSTR("Own Address Type"), packet[13]);
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
						frame->AddBool(frameOfst + 8, CSTR("Scan Enable"), packet[8]);
						frame->AddBool(frameOfst + 9, CSTR("Filter Duplicates"), packet[9]);
					}
					else
					{
						AddUnknown(frame, frameOfst + 8, packet + 8, cmdLen);
					}
					break;
				case 0x2041:
					if (cmdLen == 8)
					{
						AddAddrType(frame, frameOfst + 8, CSTR("Own Address Type"), packet[8]);
						AddScanFilterPolicy(frame, frameOfst + 9, packet[9]);
						AddPHYType(frame, frameOfst + 10, CSTR("Scanning PHYs"), packet[10]);
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
		frame->AddHex16(frameOfst + 5, CSTR("Connection Handle"), ReadUInt16(&packet[5]));
		frame->AddUInt(frameOfst + 7, 2, CSTR("Data Total Length"), ReadUInt16(&packet[7]));
		AddUnknown(frame, frameOfst + 9, packet + 9, packetSize - 9);
		break;
	case 4:
		AddHCIEvent(frame, frameOfst + 5, packet[5]);
		AddParamLen(frame, frameOfst + 6, packet[6]);
		switch (packet[5])
		{
		case 0x01: //Inquiry Complete
			vName = CSTR_NULL;
			switch (packet[7])
			{
			case 0:
				vName = CSTR("Success");
				break;
			}
			frame->AddHex8Name(frameOfst + 7, CSTR("Status"), packet[7], vName);
			if (packetSize > 8)
			{
				AddUnknown(frame, frameOfst + 8, packet + 8, packetSize + 8);
			}
			break;
		case 0x0E: //Command Complete
			frame->AddUInt(frameOfst + 7, 1, CSTR("Number of Allowed Command Packets"), packet[7]);
			AddCmdOpcode(frame, frameOfst + 8, ReadUInt16(&packet[8]));
			vName = CSTR_NULL;
			switch (packet[10])
			{
			case 0:
				vName = CSTR("Success");
				break;
			}
			frame->AddHex8Name(frameOfst + 10, CSTR("Status"), packet[10], vName);
			if (packetSize > 11)
			{
				AddUnknown(frame, frameOfst + 11, packet + 11, packetSize - 11);
			}
			break;
		case 0x0F: //Command Status
			vName = CSTR_NULL;
			switch (packet[7])
			{
			case 0:
				vName = CSTR("Pending");
				break;
			}
			frame->AddHex8Name(frameOfst + 7, CSTR("Status"), packet[7], vName);
			frame->AddUInt(frameOfst + 8, 1, CSTR("Number of Allowed Command Packets"), packet[8]);
			AddCmdOpcode(frame, frameOfst + 9, ReadUInt16(&packet[9]));
			break;
		case 0x2F: //Extended Inquery Result
			frame->AddUInt(frameOfst + 7, 1, CSTR("Number of responses"), packet[7]);
			AddBDAddr(frame, frameOfst + 8, CSTR("BD_ADDR"), &packet[8], false);
			AddPageScanMode(frame, frameOfst + 14, packet[14]);
			frame->AddHex8(frameOfst + 15, CSTR("Reserved"), packet[15]);
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
				frame->AddUInt(frameOfst + 8, 1, CSTR("Num Reports"), packet[8]);
				if (packet[8] != 1 || packet[6] < 11)
				{
					AddUnknown(frame, frameOfst + 9, packet + 9, packetSize + 9);
					break;
				}
				AddAdvEvtType(frame, frameOfst + 9, packet[9]);
				AddAddrType(frame, frameOfst + 10, CSTR("Peer Address Type"), packet[10]);
				AddBDAddr(frame, frameOfst + 11, CSTR("BD_ADDR"), packet + 11, packet[10] == 1);
				frame->AddUInt(frameOfst + 17, 1, CSTR("Adv Data Length"), packet[17]);
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
				frame->AddUInt(frameOfst + 8, 1, CSTR("Num Reports"), packet[8]);
				if (packet[8] != 1 || packet[6] < 26)
				{
					AddUnknown(frame, frameOfst + 9, packet + 9, packetSize + 9);
					break;
				}
				AddExAdvEvtType(frame, frameOfst + 9, ReadUInt16(&packet[9]));
				AddAddrType(frame, frameOfst + 11, CSTR("Peer Address Type"), packet[11]);
				AddBDAddr(frame, frameOfst + 12, CSTR("BD_ADDR"), packet + 12, packet[11] == 1);
				AddPHYType(frame, frameOfst + 18, CSTR("Primary PHY"), packet[18]);
				AddPHYType(frame, frameOfst + 19, CSTR("Secondary PHY"), packet[19]);
				AddAdvSID(frame, frameOfst + 20, packet[20]);
				AddTxPower(frame, frameOfst + 21, (Int8)packet[21]);
				AddRSSI(frame, frameOfst + 22, (Int8)packet[22]);
				AddPeriodicAdv(frame, frameOfst + 23, ReadUInt16(&packet[23]));
				AddAddrType(frame, frameOfst + 25, CSTR("Direct Address Type"), packet[25]);
				AddBDAddr(frame, frameOfst + 26, CSTR("Direct BD_ADDR"), packet + 26, packet[25] == 1);
				frame->AddUInt(frameOfst + 32, 1, CSTR("Adv Data Length"), packet[32]);
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

Text::CString Net::PacketAnalyzerBluetooth::CompanyGetName(UInt16 company)
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
			return vendorList[k].vendorName;
		}
	}
	return CSTR_NULL;
}

Text::CString Net::PacketAnalyzerBluetooth::CmdGetName(UInt16 cmd)
{
	switch (cmd)
	{
	case 0x0401:
		return CSTR("HCI Inquiry");
	case 0x2005:
		return CSTR("LE Set Random Address");
	case 0x200B:
		return CSTR("LE Set Scan Parameter");
	case 0x200C:
		return CSTR("LE Set Scan Enable");
	default:
		return CSTR_NULL;
	}
}

Text::CString Net::PacketAnalyzerBluetooth::HCIPacketTypeGetName(UInt8 packetType)
{
	switch (packetType)
	{
	case 1:
		return CSTR("HCI Command");
	case 4:
		return CSTR("HCI Event");
	default:
		return CSTR_NULL;	
	}
}

Text::CString Net::PacketAnalyzerBluetooth::HCIEventGetName(UInt8 evt)
{
	switch (evt)
	{
	case 0x0E:
		return CSTR("Command Complete");
	case 0x0F:
		return CSTR("Command Status");
	case 0x2F:
		return CSTR("Extended Inquery Result");
	case 0x3E:
		return CSTR("LE Meta");
	default:
		return CSTR_NULL;
	}
}

Text::CString Net::PacketAnalyzerBluetooth::LESubEventGetName(UInt8 subEvt)
{
	switch (subEvt)
	{
	case 0x02:
		return CSTR("LE Advertising Report");
	case 0x0D:
		return CSTR("LE Extended Advertising Report");
	default:
		return CSTR_NULL;
	}
}
