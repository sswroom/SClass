#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/SBFrameDetail.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::DeviceDB::VendorInfo2 Net::PacketAnalyzerBluetooth::vendorList[] = {
	{0x0000, UTF8STRC("Ericsson Technology Licensing")},
	{0x0001, UTF8STRC("Nokia Mobile Phones")},
	{0x0002, UTF8STRC("Intel Corp.")},
	{0x0003, UTF8STRC("IBM Corp.")},
	{0x0004, UTF8STRC("Toshiba Corp.")},
	{0x0005, UTF8STRC("3Com")},
	{0x0006, UTF8STRC("Microsoft")},
	{0x0007, UTF8STRC("Lucent")},
	{0x0008, UTF8STRC("Motorola")},
	{0x0009, UTF8STRC("Infineon Technologies AG")},
	{0x000A, UTF8STRC("Cambridge Silicon Radio")},
	{0x000B, UTF8STRC("Silicon Wave")},
	{0x000C, UTF8STRC("Digianswer A/S")},
	{0x000D, UTF8STRC("Texas Instruments Inc.")},
	{0x000E, UTF8STRC("Parthus Technologies Inc.")},
	{0x000F, UTF8STRC("Broadcom Corporation")},
	{0x0010, UTF8STRC("Mitel Semiconductor")},
	{0x0011, UTF8STRC("Widcomm, Inc.")},
	{0x0012, UTF8STRC("Zeevo, Inc.")},
	{0x0013, UTF8STRC("Atmel Corporation")},
	{0x0014, UTF8STRC("Mitsubishi Electric Corporation")},
	{0x0015, UTF8STRC("RTX Telecom A/S")},
	{0x0016, UTF8STRC("KC Technology Inc.")},
	{0x0017, UTF8STRC("Newlogic")},
	{0x0018, UTF8STRC("Transilica, Inc.")},
	{0x0019, UTF8STRC("Rohde & Schwarz GmbH & Co. KG")},
	{0x001A, UTF8STRC("TTPCom Limited")},
	{0x001B, UTF8STRC("Signia Technologies, Inc.")},
	{0x001C, UTF8STRC("Conexant Systems Inc.")},
	{0x001D, UTF8STRC("Qualcomm")},
	{0x001E, UTF8STRC("Inventel")},
	{0x001F, UTF8STRC("AVM Berlin")},
	{0x0020, UTF8STRC("BandSpeed, Inc.")},
	{0x0021, UTF8STRC("Mansella Ltd")},
	{0x0022, UTF8STRC("NEC Corporation")},
	{0x0023, UTF8STRC("WavePlus Technology Co., Ltd.")},
	{0x0024, UTF8STRC("Alcatel")},
	{0x0025, UTF8STRC("NXP Semiconductors (formerly Philips Semiconductors)")},
	{0x0026, UTF8STRC("C Technologies")},
	{0x0027, UTF8STRC("Open Interface")},
	{0x0028, UTF8STRC("R F Micro Devices")},
	{0x0029, UTF8STRC("Hitachi Ltd")},
	{0x002A, UTF8STRC("Symbol Technologies, Inc.")},
	{0x002B, UTF8STRC("Tenovis")},
	{0x002C, UTF8STRC("Macronix International Co. Ltd.")},
	{0x002D, UTF8STRC("GCT Semiconductor")},
	{0x002E, UTF8STRC("Norwood Systems")},
	{0x002F, UTF8STRC("MewTel Technology Inc.")},
	{0x0030, UTF8STRC("ST Microelectronics")},
	{0x0031, UTF8STRC("Synopsys, Inc.")},
	{0x0032, UTF8STRC("Red-M (Communications) Ltd")},
	{0x0033, UTF8STRC("Commil Ltd")},
	{0x0034, UTF8STRC("Computer Access Technology Corporation (CATC)")},
	{0x0035, UTF8STRC("Eclipse (HQ Espana) S.L.")},
	{0x0036, UTF8STRC("Renesas Electronics Corporation")},
	{0x0037, UTF8STRC("Mobilian Corporation")},
	{0x0038, UTF8STRC("Terax")},
	{0x0039, UTF8STRC("Integrated System Solution Corp.")},
	{0x003A, UTF8STRC("Matsushita Electric Industrial Co., Ltd.")},
	{0x003B, UTF8STRC("Gennum Corporation")},
	{0x003C, UTF8STRC("BlackBerry Limited (formerly Research In Motion)")},
	{0x003D, UTF8STRC("IPextreme, Inc.")},
	{0x003E, UTF8STRC("Systems and Chips, Inc")},
	{0x003F, UTF8STRC("Bluetooth SIG, Inc")},
	{0x0040, UTF8STRC("Seiko Epson Corporation")},
	{0x0041, UTF8STRC("Integrated Silicon Solution Taiwan, Inc.")},
	{0x0042, UTF8STRC("CONWISE Technology Corporation Ltd")},
	{0x0043, UTF8STRC("PARROT SA")},
	{0x0044, UTF8STRC("Socket Mobile")},
	{0x0045, UTF8STRC("Atheros Communications, Inc.")},
	{0x0046, UTF8STRC("MediaTek, Inc.")},
	{0x0047, UTF8STRC("Bluegiga")},
	{0x0048, UTF8STRC("Marvell Technology Group Ltd.")},
	{0x0049, UTF8STRC("3DSP Corporation")},
	{0x004A, UTF8STRC("Accel Semiconductor Ltd.")},
	{0x004B, UTF8STRC("Continental Automotive Systems")},
	{0x004C, UTF8STRC("Apple, Inc.")},
	{0x004D, UTF8STRC("Staccato Communications, Inc.")},
	{0x004E, UTF8STRC("Avago Technologies")},
	{0x004F, UTF8STRC("APT Ltd.")},
	{0x0050, UTF8STRC("SiRF Technology, Inc.")},
	{0x0051, UTF8STRC("Tzero Technologies, Inc.")},
	{0x0052, UTF8STRC("J&M Corporation")},
	{0x0053, UTF8STRC("Free2move AB")},
	{0x0054, UTF8STRC("3DiJoy Corporation")},
	{0x0055, UTF8STRC("Plantronics, Inc.")},
	{0x0056, UTF8STRC("Sony Ericsson Mobile Communications")},
	{0x0057, UTF8STRC("Harman International Industries, Inc.")},
	{0x0058, UTF8STRC("Vizio, Inc.")},
	{0x0059, UTF8STRC("Nordic Semiconductor ASA")},
	{0x005A, UTF8STRC("EM Microelectronic-Marin SA")},
	{0x005B, UTF8STRC("Ralink Technology Corporation")},
	{0x005C, UTF8STRC("Belkin International, Inc.")},
	{0x005D, UTF8STRC("Realtek Semiconductor Corporation")},
	{0x005E, UTF8STRC("Stonestreet One, LLC")},
	{0x005F, UTF8STRC("Wicentric, Inc.")},
	{0x0060, UTF8STRC("RivieraWaves S.A.S")},
	{0x0061, UTF8STRC("RDA Microelectronics")},
	{0x0062, UTF8STRC("Gibson Guitars")},
	{0x0063, UTF8STRC("MiCommand Inc.")},
	{0x0064, UTF8STRC("Band XI International, LLC")},
	{0x0065, UTF8STRC("Hewlett-Packard Company")},
	{0x0066, UTF8STRC("9Solutions Oy")},
	{0x0067, UTF8STRC("GN Netcom A/S")},
	{0x0068, UTF8STRC("General Motors")},
	{0x0069, UTF8STRC("A&D Engineering, Inc.")},
	{0x006A, UTF8STRC("MindTree Ltd.")},
	{0x006B, UTF8STRC("Polar Electro OY")},
	{0x006C, UTF8STRC("Beautiful Enterprise Co., Ltd.")},
	{0x006D, UTF8STRC("BriarTek, Inc")},
	{0x006E, UTF8STRC("Summit Data Communications, Inc.")},
	{0x006F, UTF8STRC("Sound ID")},
	{0x0070, UTF8STRC("Monster, LLC")},
	{0x0071, UTF8STRC("connectBlue AB")},
	{0x0072, UTF8STRC("ShangHai Super Smart Electronics Co. Ltd.")},
	{0x0073, UTF8STRC("Group Sense Ltd.")},
	{0x0074, UTF8STRC("Zomm, LLC")},
	{0x0075, UTF8STRC("Samsung Electronics Co. Ltd.")},
	{0x0076, UTF8STRC("Creative Technology Ltd.")},
	{0x0077, UTF8STRC("Laird Technologies")},
	{0x0078, UTF8STRC("Nike, Inc.")},
	{0x0079, UTF8STRC("lesswire AG")},
	{0x007A, UTF8STRC("MStar Semiconductor, Inc.")},
	{0x007B, UTF8STRC("Hanlynn Technologies")},
	{0x007C, UTF8STRC("A & R Cambridge")},
	{0x007D, UTF8STRC("Seers Technology Co., Ltd.")},
	{0x007E, UTF8STRC("Sports Tracking Technologies Ltd.")},
	{0x007F, UTF8STRC("Autonet Mobile")},
	{0x0080, UTF8STRC("DeLorme Publishing Company, Inc.")},
	{0x0081, UTF8STRC("WuXi Vimicro")},
	{0x0082, UTF8STRC("Sennheiser Communications A/S")},
	{0x0083, UTF8STRC("TimeKeeping Systems, Inc.")},
	{0x0084, UTF8STRC("Ludus Helsinki Ltd.")},
	{0x0085, UTF8STRC("BlueRadios, Inc.")},
	{0x0086, UTF8STRC("Equinux AG")},
	{0x0087, UTF8STRC("Garmin International, Inc.")},
	{0x0088, UTF8STRC("Ecotest")},
	{0x0089, UTF8STRC("GN ReSound A/S")},
	{0x008A, UTF8STRC("Jawbone")},
	{0x008B, UTF8STRC("Topcon Positioning Systems, LLC")},
	{0x008C, UTF8STRC("Gimbal Inc. (formerly Qualcomm Labs, Inc. and Qualcomm Retail Solutions, Inc.)")},
	{0x008D, UTF8STRC("Zscan Software")},
	{0x008E, UTF8STRC("Quintic Corp")},
	{0x008F, UTF8STRC("Telit Wireless Solutions GmbH (formerly Stollmann E+V GmbH)")},
	{0x0090, UTF8STRC("Funai Electric Co., Ltd.")},
	{0x0091, UTF8STRC("Advanced PANMOBIL systems GmbH & Co. KG")},
	{0x0092, UTF8STRC("ThinkOptics, Inc.")},
	{0x0093, UTF8STRC("Universal Electronics, Inc.")},
	{0x0094, UTF8STRC("Airoha Technology Corp.")},
	{0x0095, UTF8STRC("NEC Lighting, Ltd.")},
	{0x0096, UTF8STRC("ODM Technology, Inc.")},
	{0x0097, UTF8STRC("ConnecteDevice Ltd.")},
	{0x0098, UTF8STRC("zero1.tv GmbH")},
	{0x0099, UTF8STRC("i.Tech Dynamic Global Distribution Ltd.")},
	{0x009A, UTF8STRC("Alpwise")},
	{0x009B, UTF8STRC("Jiangsu Toppower Automotive Electronics Co., Ltd.")},
	{0x009C, UTF8STRC("Colorfy, Inc.")},
	{0x009D, UTF8STRC("Geoforce Inc.")},
	{0x009E, UTF8STRC("Bose Corporation")},
	{0x009F, UTF8STRC("Suunto Oy")},
	{0x00A0, UTF8STRC("Kensington Computer Products Group")},
	{0x00A1, UTF8STRC("SR-Medizinelektronik")},
	{0x00A2, UTF8STRC("Vertu Corporation Limited")},
	{0x00A3, UTF8STRC("Meta Watch Ltd.")},
	{0x00A4, UTF8STRC("LINAK A/S")},
	{0x00A5, UTF8STRC("OTL Dynamics LLC")},
	{0x00A6, UTF8STRC("Panda Ocean Inc.")},
	{0x00A7, UTF8STRC("Visteon Corporation")},
	{0x00A8, UTF8STRC("ARP Devices Limited")},
	{0x00A9, UTF8STRC("Magneti Marelli S.p.A")},
	{0x00AA, UTF8STRC("CAEN RFID srl")},
	{0x00AB, UTF8STRC("Ingenieur-Systemgruppe Zahn GmbH")},
	{0x00AC, UTF8STRC("Green Throttle Games")},
	{0x00AD, UTF8STRC("Peter Systemtechnik GmbH")},
	{0x00AE, UTF8STRC("Omegawave Oy")},
	{0x00AF, UTF8STRC("Cinetix")},
	{0x00B0, UTF8STRC("Passif Semiconductor Corp")},
	{0x00B1, UTF8STRC("Saris Cycling Group, Inc")},
	{0x00B2, UTF8STRC("Bekey A/S")},
	{0x00B3, UTF8STRC("Clarinox Technologies Pty. Ltd.")},
	{0x00B4, UTF8STRC("BDE Technology Co., Ltd.")},
	{0x00B5, UTF8STRC("Swirl Networks")},
	{0x00B6, UTF8STRC("Meso international")},
	{0x00B7, UTF8STRC("TreLab Ltd")},
	{0x00B8, UTF8STRC("Qualcomm Innovation Center, Inc. (QuIC)")},
	{0x00B9, UTF8STRC("Johnson Controls, Inc.")},
	{0x00BA, UTF8STRC("Starkey Laboratories Inc.")},
	{0x00BB, UTF8STRC("S-Power Electronics Limited")},
	{0x00BC, UTF8STRC("Ace Sensor Inc")},
	{0x00BD, UTF8STRC("Aplix Corporation")},
	{0x00BE, UTF8STRC("AAMP of America")},
	{0x00BF, UTF8STRC("Stalmart Technology Limited")},
	{0x00C0, UTF8STRC("AMICCOM Electronics Corporation")},
	{0x00C1, UTF8STRC("Shenzhen Excelsecu Data Technology Co.,Ltd")},
	{0x00C2, UTF8STRC("Geneq Inc.")},
	{0x00C3, UTF8STRC("adidas AG")},
	{0x00C4, UTF8STRC("LG Electronics")},
	{0x00C5, UTF8STRC("Onset Computer Corporation")},
	{0x00C6, UTF8STRC("Selfly BV")},
	{0x00C7, UTF8STRC("Quuppa Oy.")},
	{0x00C8, UTF8STRC("GeLo Inc")},
	{0x00C9, UTF8STRC("Evluma")},
	{0x00CA, UTF8STRC("MC10")},
	{0x00CB, UTF8STRC("Binauric SE")},
	{0x00CC, UTF8STRC("Beats Electronics")},
	{0x00CD, UTF8STRC("Microchip Technology Inc.")},
	{0x00CE, UTF8STRC("Elgato Systems GmbH")},
	{0x00CF, UTF8STRC("ARCHOS SA")},
	{0x00D0, UTF8STRC("Dexcom, Inc.")},
	{0x00D1, UTF8STRC("Polar Electro Europe B.V.")},
	{0x00D2, UTF8STRC("Dialog Semiconductor B.V.")},
	{0x00D3, UTF8STRC("Taixingbang Technology (HK) Co,. LTD.")},
	{0x00D4, UTF8STRC("Kawantech")},
	{0x00D5, UTF8STRC("Austco Communication Systems")},
	{0x00D6, UTF8STRC("Timex Group USA, Inc.")},
	{0x00D7, UTF8STRC("Qualcomm Technologies, Inc.")},
	{0x00D8, UTF8STRC("Qualcomm Connected Experiences, Inc.")},
	{0x00D9, UTF8STRC("Voyetra Turtle Beach")},
	{0x00DA, UTF8STRC("txtr GmbH")},
	{0x00DB, UTF8STRC("Biosentronics")},
	{0x00DC, UTF8STRC("Procter & Gamble")},
	{0x00DD, UTF8STRC("Hosiden Corporation")},
	{0x00DE, UTF8STRC("Muzik LLC")},
	{0x00DF, UTF8STRC("Misfit Wearables Corp")},
	{0x00E0, UTF8STRC("Google")},
	{0x00E1, UTF8STRC("Danlers Ltd")},
	{0x00E2, UTF8STRC("Semilink Inc")},
	{0x00E3, UTF8STRC("inMusic Brands, Inc")},
	{0x00E4, UTF8STRC("L.S. Research Inc.")},
	{0x00E5, UTF8STRC("Eden Software Consultants Ltd.")},
	{0x00E6, UTF8STRC("Freshtemp")},
	{0x00E7, UTF8STRC("KS Technologies")},
	{0x00E8, UTF8STRC("ACTS Technologies")},
	{0x00E9, UTF8STRC("Vtrack Systems")},
	{0x00EA, UTF8STRC("Nielsen-Kellerman Company")},
	{0x00EB, UTF8STRC("Server Technology Inc.")},
	{0x00EC, UTF8STRC("BioResarch Associates")},
	{0x00ED, UTF8STRC("Jolly Logic, LLC")},
	{0x00EE, UTF8STRC("Above Average Outcomes, Inc.")},
	{0x00EF, UTF8STRC("Bitsplitters GmbH")},
	{0x00F0, UTF8STRC("PayPal, Inc.")},
	{0x00F1, UTF8STRC("Witron Technology Limited")},
	{0x00F2, UTF8STRC("Morse Project Inc.")},
	{0x00F3, UTF8STRC("Kent Displays Inc.")},
	{0x00F4, UTF8STRC("Nautilus Inc.")},
	{0x00F5, UTF8STRC("Smartifier Oy")},
	{0x00F6, UTF8STRC("Elcometer Limited")},
	{0x00F7, UTF8STRC("VSN Technologies, Inc.")},
	{0x00F8, UTF8STRC("AceUni Corp., Ltd.")},
	{0x00F9, UTF8STRC("StickNFind")},
	{0x00FA, UTF8STRC("Crystal Code AB")},
	{0x00FB, UTF8STRC("KOUKAAM a.s.")},
	{0x00FC, UTF8STRC("Delphi Corporation")},
	{0x00FD, UTF8STRC("ValenceTech Limited")},
	{0x00FE, UTF8STRC("Stanley Black and Decker")},
	{0x00FF, UTF8STRC("Typo Products, LLC")},
	{0x0100, UTF8STRC("TomTom International BV")},
	{0x0101, UTF8STRC("Fugoo, Inc.")},
	{0x0102, UTF8STRC("Keiser Corporation")},
	{0x0103, UTF8STRC("Bang & Olufsen S/A")},
	{0x0104, UTF8STRC("PLUS Location Systems Pty Ltd")},
	{0x0105, UTF8STRC("Ubiquitous Computing Technology Corporation")},
	{0x0106, UTF8STRC("Innovative Yachtter Solutions")},
	{0x0107, UTF8STRC("William Demant Holding A/S")},
	{0x0108, UTF8STRC("Chicony Electronics Co., Ltd.")},
	{0x0109, UTF8STRC("Atus BV")},
	{0x010A, UTF8STRC("Codegate Ltd")},
	{0x010B, UTF8STRC("ERi, Inc")},
	{0x010C, UTF8STRC("Transducers Direct, LLC")},
	{0x010D, UTF8STRC("Fujitsu Ten LImited")},
	{0x010E, UTF8STRC("Audi AG")},
	{0x010F, UTF8STRC("HiSilicon Technologies Col, Ltd.")},
	{0x0110, UTF8STRC("Nippon Seiki Co., Ltd.")},
	{0x0111, UTF8STRC("Steelseries ApS")},
	{0x0112, UTF8STRC("Visybl Inc.")},
	{0x0113, UTF8STRC("Openbrain Technologies, Co., Ltd.")},
	{0x0114, UTF8STRC("Xensr")},
	{0x0115, UTF8STRC("e.solutions")},
	{0x0116, UTF8STRC("10AK Technologies")},
	{0x0117, UTF8STRC("Wimoto Technologies Inc")},
	{0x0118, UTF8STRC("Radius Networks, Inc.")},
	{0x0119, UTF8STRC("Wize Technology Co., Ltd.")},
	{0x011A, UTF8STRC("Qualcomm Labs, Inc.")},
	{0x011B, UTF8STRC("Aruba Networks")},
	{0x011C, UTF8STRC("Baidu")},
	{0x011D, UTF8STRC("Arendi AG")},
	{0x011E, UTF8STRC("Skoda Auto a.s.")},
	{0x011F, UTF8STRC("Volkwagon AG")},
	{0x0120, UTF8STRC("Porsche AG")},
	{0x0121, UTF8STRC("Sino Wealth Electronic Ltd.")},
	{0x0122, UTF8STRC("AirTurn, Inc.")},
	{0x0123, UTF8STRC("Kinsa, Inc")},
	{0x0124, UTF8STRC("HID Global")},
	{0x0125, UTF8STRC("SEAT es")},
	{0x0126, UTF8STRC("Promethean Ltd.")},
	{0x0127, UTF8STRC("Salutica Allied Solutions")},
	{0x0128, UTF8STRC("GPSI Group Pty Ltd")},
	{0x0129, UTF8STRC("Nimble Devices Oy")},
	{0x012A, UTF8STRC("Changzhou Yongse Infotech Co., Ltd.")},
	{0x012B, UTF8STRC("SportIQ")},
	{0x012C, UTF8STRC("TEMEC Instruments B.V.")},
	{0x012D, UTF8STRC("Sony Corporation")},
	{0x012E, UTF8STRC("ASSA ABLOY")},
	{0x012F, UTF8STRC("Clarion Co. Inc.")},
	{0x0130, UTF8STRC("Warehouse Innovations")},
	{0x0131, UTF8STRC("Cypress Semiconductor")},
	{0x0132, UTF8STRC("MADS Inc")},
	{0x0133, UTF8STRC("Blue Maestro Limited")},
	{0x0134, UTF8STRC("Resolution Products, Ltd.")},
	{0x0135, UTF8STRC("Aireware LLC")},
	{0x0136, UTF8STRC("Seed Labs, Inc. (formerly ETC sp. z.o.o.)")},
	{0x0137, UTF8STRC("Prestigio Plaza Ltd.")},
	{0x0138, UTF8STRC("NTEO Inc.")},
	{0x0139, UTF8STRC("Focus Systems Corporation")},
	{0x013A, UTF8STRC("Tencent Holdings Ltd.")},
	{0x013B, UTF8STRC("Allegion")},
	{0x013C, UTF8STRC("Murata Manufacturing Co., Ltd.")},
	{0x013D, UTF8STRC("WirelessWERX")},
	{0x013E, UTF8STRC("Nod, Inc.")},
	{0x013F, UTF8STRC("B&B Manufacturing Company")},
	{0x0140, UTF8STRC("Alpine Electronics (China) Co., Ltd")},
	{0x0141, UTF8STRC("FedEx Services")},
	{0x0142, UTF8STRC("Grape Systems Inc.")},
	{0x0143, UTF8STRC("Bkon Connect")},
	{0x0144, UTF8STRC("Lintech GmbH")},
	{0x0145, UTF8STRC("Novatel Wireless")},
	{0x0146, UTF8STRC("Ciright")},
	{0x0147, UTF8STRC("Mighty Cast, Inc.")},
	{0x0148, UTF8STRC("Ambimat Electronics")},
	{0x0149, UTF8STRC("Perytons Ltd.")},
	{0x014A, UTF8STRC("Tivoli Audio, LLC")},
	{0x014B, UTF8STRC("Master Lock")},
	{0x014C, UTF8STRC("Mesh-Net Ltd")},
	{0x014D, UTF8STRC("HUIZHOU DESAY SV AUTOMOTIVE CO., LTD.")},
	{0x014E, UTF8STRC("Tangerine, Inc.")},
	{0x014F, UTF8STRC("B&W Group Ltd.")},
	{0x0150, UTF8STRC("Pioneer Corporation")},
	{0x0151, UTF8STRC("OnBeep")},
	{0x0152, UTF8STRC("Vernier Software & Technology")},
	{0x0153, UTF8STRC("ROL Ergo")},
	{0x0154, UTF8STRC("Pebble Technology")},
	{0x0155, UTF8STRC("NETATMO")},
	{0x0156, UTF8STRC("Accumulate AB")},
	{0x0157, UTF8STRC("Anhui Huami Information Technology Co., Ltd.")},
	{0x0158, UTF8STRC("Inmite s.r.o.")},
	{0x0159, UTF8STRC("ChefSteps, Inc.")},
	{0x015A, UTF8STRC("micas AG")},
	{0x015B, UTF8STRC("Biomedical Research Ltd.")},
	{0x015C, UTF8STRC("Pitius Tec S.L.")},
	{0x015D, UTF8STRC("Estimote, Inc.")},
	{0x015E, UTF8STRC("Unikey Technologies, Inc.")},
	{0x015F, UTF8STRC("Timer Cap Co.")},
	{0x0160, UTF8STRC("AwoX")},
	{0x0161, UTF8STRC("yikes")},
	{0x0162, UTF8STRC("MADSGlobalNZ Ltd.")},
	{0x0163, UTF8STRC("PCH International")},
	{0x0164, UTF8STRC("Qingdao Yeelink Information Technology Co., Ltd.")},
	{0x0165, UTF8STRC("Milwaukee Tool (Formally Milwaukee Electric Tools)")},
	{0x0166, UTF8STRC("MISHIK Pte Ltd")},
	{0x0167, UTF8STRC("Bayer HealthCare")},
	{0x0168, UTF8STRC("Spicebox LLC")},
	{0x0169, UTF8STRC("emberlight")},
	{0x016A, UTF8STRC("Cooper-Atkins Corporation")},
	{0x016B, UTF8STRC("Qblinks")},
	{0x016C, UTF8STRC("MYSPHERA")},
	{0x016D, UTF8STRC("LifeScan Inc")},
	{0x016E, UTF8STRC("Volantic AB")},
	{0x016F, UTF8STRC("Podo Labs, Inc")},
	{0x0170, UTF8STRC("Roche Diabetes Care AG")},
	{0x0171, UTF8STRC("Amazon Fulfillment Service")},
	{0x0172, UTF8STRC("Connovate Technology Private Limited")},
	{0x0173, UTF8STRC("Kocomojo, LLC")},
	{0x0174, UTF8STRC("EveryKey LLC")},
	{0x0175, UTF8STRC("Dynamic Controls")},
	{0x0176, UTF8STRC("SentriLock")},
	{0x0177, UTF8STRC("I-SYST inc.")},
	{0x0178, UTF8STRC("CASIO COMPUTER CO., LTD.")},
	{0x0179, UTF8STRC("LAPIS Semiconductor Co., Ltd.")},
	{0x017A, UTF8STRC("Telemonitor, Inc.")},
	{0x017B, UTF8STRC("taskit GmbH")},
	{0x017C, UTF8STRC("Daimler AG")},
	{0x017D, UTF8STRC("BatAndCat")},
	{0x017E, UTF8STRC("BluDotz Ltd")},
	{0x017F, UTF8STRC("XTel ApS")},
	{0x0180, UTF8STRC("Gigaset Communications GmbH")},
	{0x0181, UTF8STRC("Gecko Health Innovations, Inc.")},
	{0x0182, UTF8STRC("HOP Ubiquitous")},
	{0x0183, UTF8STRC("Walt Disney")},
	{0x0184, UTF8STRC("Nectar")},
	{0x0185, UTF8STRC("bel'apps LLC")},
	{0x0186, UTF8STRC("CORE Lighting Ltd")},
	{0x0187, UTF8STRC("Seraphim Sense Ltd")},
	{0x0188, UTF8STRC("Unico RBC")},
	{0x0189, UTF8STRC("Physical Enterprises Inc.")},
	{0x018A, UTF8STRC("Able Trend Technology Limited")},
	{0x018B, UTF8STRC("Konica Minolta, Inc.")},
	{0x018C, UTF8STRC("Wilo SE")},
	{0x018D, UTF8STRC("Extron Design Services")},
	{0x018E, UTF8STRC("Fitbit, Inc.")},
	{0x018F, UTF8STRC("Fireflies Systems")},
	{0x0190, UTF8STRC("Intelletto Technologies Inc.")},
	{0x0191, UTF8STRC("FDK CORPORATION")},
	{0x0192, UTF8STRC("Cloudleaf, Inc")},
	{0x0193, UTF8STRC("Maveric Automation LLC")},
	{0x0194, UTF8STRC("Acoustic Stream Corporation")},
	{0x0195, UTF8STRC("Zuli")},
	{0x0196, UTF8STRC("Paxton Access Ltd")},
	{0x0197, UTF8STRC("WiSilica Inc.")},
	{0x0198, UTF8STRC("VENGIT Korltolt Felelssg Trsasg")},
	{0x0199, UTF8STRC("SALTO SYSTEMS S.L.")},
	{0x019A, UTF8STRC("TRON Forum (formerly T-Engine Forum)")},
	{0x019B, UTF8STRC("CUBETECH s.r.o.")},
	{0x019C, UTF8STRC("Cokiya Incorporated")},
	{0x019D, UTF8STRC("CVS Health")},
	{0x019E, UTF8STRC("Ceruus")},
	{0x019F, UTF8STRC("Strainstall Ltd")},
	{0x01A0, UTF8STRC("Channel Enterprises (HK) Ltd.")},
	{0x01A1, UTF8STRC("FIAMM")},
	{0x01A2, UTF8STRC("GIGALANE.CO.,LTD")},
	{0x01A3, UTF8STRC("EROAD")},
	{0x01A4, UTF8STRC("Mine Safety Appliances")},
	{0x01A5, UTF8STRC("Icon Health and Fitness")},
	{0x01A6, UTF8STRC("Asandoo GmbH")},
	{0x01A7, UTF8STRC("ENERGOUS CORPORATION")},
	{0x01A8, UTF8STRC("Taobao")},
	{0x01A9, UTF8STRC("Canon Inc.")},
	{0x01AA, UTF8STRC("Geophysical Technology Inc.")},
	{0x01AB, UTF8STRC("Facebook, Inc.")},
	{0x01AC, UTF8STRC("Nipro Diagnostics, Inc.")},
	{0x01AD, UTF8STRC("FlightSafety International")},
	{0x01AE, UTF8STRC("Earlens Corporation")},
	{0x01AF, UTF8STRC("Sunrise Micro Devices, Inc.")},
	{0x01B0, UTF8STRC("Star Micronics Co., Ltd.")},
	{0x01B1, UTF8STRC("Netizens Sp. z o.o.")},
	{0x01B2, UTF8STRC("Nymi Inc.")},
	{0x01B3, UTF8STRC("Nytec, Inc.")},
	{0x01B4, UTF8STRC("Trineo Sp. z o.o.")},
	{0x01B5, UTF8STRC("Nest Labs Inc.")},
	{0x01B6, UTF8STRC("LM Technologies Ltd")},
	{0x01B7, UTF8STRC("General Electric Company")},
	{0x01B8, UTF8STRC("i+D3 S.L.")},
	{0x01B9, UTF8STRC("HANA Micron")},
	{0x01BA, UTF8STRC("Stages Cycling LLC")},
	{0x01BB, UTF8STRC("Cochlear Bone Anchored Solutions AB")},
	{0x01BC, UTF8STRC("SenionLab AB")},
	{0x01BD, UTF8STRC("Syszone Co., Ltd")},
	{0x01BE, UTF8STRC("Pulsate Mobile Ltd.")},
	{0x01BF, UTF8STRC("Hong Kong HunterSun Electronic Limited")},
	{0x01C0, UTF8STRC("pironex GmbH")},
	{0x01C1, UTF8STRC("BRADATECH Corp.")},
	{0x01C2, UTF8STRC("Transenergooil AG")},
	{0x01C3, UTF8STRC("Bunch")},
	{0x01C4, UTF8STRC("DME Microelectronics")},
	{0x01C5, UTF8STRC("Bitcraze AB")},
	{0x01C6, UTF8STRC("HASWARE Inc.")},
	{0x01C7, UTF8STRC("Abiogenix Inc.")},
	{0x01C8, UTF8STRC("Poly-Control ApS")},
	{0x01C9, UTF8STRC("Avi-on")},
	{0x01CA, UTF8STRC("Laerdal Medical AS")},
	{0x01CB, UTF8STRC("Fetch My Pet")},
	{0x01CC, UTF8STRC("Sam Labs Ltd.")},
	{0x01CD, UTF8STRC("Chengdu Synwing Technology Ltd")},
	{0x01CE, UTF8STRC("HOUWA SYSTEM DESIGN, k.k.")},
	{0x01CF, UTF8STRC("BSH")},
	{0x01D0, UTF8STRC("Primus Inter Pares Ltd")},
	{0x01D1, UTF8STRC("August Home, Inc")},
	{0x01D2, UTF8STRC("Gill Electronics")},
	{0x01D3, UTF8STRC("Sky Wave Design")},
	{0x01D4, UTF8STRC("Newlab S.r.l.")},
	{0x01D5, UTF8STRC("ELAD srl")},
	{0x01D6, UTF8STRC("G-wearables inc.")},
	{0x01D7, UTF8STRC("Squadrone Systems Inc.")},
	{0x01D8, UTF8STRC("Code Corporation")},
	{0x01D9, UTF8STRC("Savant Systems LLC")},
	{0x01DA, UTF8STRC("Logitech International SA")},
	{0x01DB, UTF8STRC("Innblue Consulting")},
	{0x01DC, UTF8STRC("iParking Ltd.")},
	{0x01DD, UTF8STRC("Koninklijke Philips Electronics N.V.")},
	{0x01DE, UTF8STRC("Minelab Electronics Pty Limited")},
	{0x01DF, UTF8STRC("Bison Group Ltd.")},
	{0x01E0, UTF8STRC("Widex A/S")},
	{0x01E1, UTF8STRC("Jolla Ltd")},
	{0x01E2, UTF8STRC("Lectronix, Inc.")},
	{0x01E3, UTF8STRC("Caterpillar Inc")},
	{0x01E4, UTF8STRC("Freedom Innovations")},
	{0x01E5, UTF8STRC("Dynamic Devices Ltd")},
	{0x01E6, UTF8STRC("Technology Solutions (UK) Ltd")},
	{0x01E7, UTF8STRC("IPS Group Inc.")},
	{0x01E8, UTF8STRC("STIR")},
	{0x01E9, UTF8STRC("Sano, Inc.")},
	{0x01EA, UTF8STRC("Advanced Application Design, Inc.")},
	{0x01EB, UTF8STRC("AutoMap LLC")},
	{0x01EC, UTF8STRC("Spreadtrum Communications Shanghai Ltd")},
	{0x01ED, UTF8STRC("CuteCircuit LTD")},
	{0x01EE, UTF8STRC("Valeo Service")},
	{0x01EF, UTF8STRC("Fullpower Technologies, Inc.")},
	{0x01F0, UTF8STRC("KloudNation")},
	{0x01F1, UTF8STRC("Zebra Technologies Corporation")},
	{0x01F2, UTF8STRC("Itron, Inc.")},
	{0x01F3, UTF8STRC("The University of Tokyo")},
	{0x01F4, UTF8STRC("UTC Fire and Security")},
	{0x01F5, UTF8STRC("Cool Webthings Limited")},
	{0x01F6, UTF8STRC("DJO Global")},
	{0x01F7, UTF8STRC("Gelliner Limited")},
	{0x01F8, UTF8STRC("Anyka (Guangzhou) Microelectronics Technology Co, LTD")},
	{0x01F9, UTF8STRC("Medtronic Inc.")},
	{0x01FA, UTF8STRC("Gozio Inc.")},
	{0x01FB, UTF8STRC("Form Lifting, LLC")},
	{0x01FC, UTF8STRC("Wahoo Fitness, LLC")},
	{0x01FD, UTF8STRC("Kontakt Micro-Location Sp. z o.o.")},
	{0x01FE, UTF8STRC("Radio Systems Corporation")},
	{0x01FF, UTF8STRC("Freescale Semiconductor, Inc.")},
	{0x0200, UTF8STRC("Verifone Systems Pte Ltd. Taiwan Branch")},
	{0x0201, UTF8STRC("AR Timing")},
	{0x0202, UTF8STRC("Rigado LLC")},
	{0x0203, UTF8STRC("Kemppi Oy")},
	{0x0204, UTF8STRC("Tapcentive Inc.")},
	{0x0205, UTF8STRC("Smartbotics Inc.")},
	{0x0206, UTF8STRC("Otter Products, LLC")},
	{0x0207, UTF8STRC("STEMP Inc.")},
	{0x0208, UTF8STRC("LumiGeek LLC")},
	{0x0209, UTF8STRC("InvisionHeart Inc.")},
	{0x020A, UTF8STRC("Macnica Inc.")},
	{0x020B, UTF8STRC("Jaguar Land Rover Limited")},
	{0x020C, UTF8STRC("CoroWare Technologies, Inc")},
	{0x020D, UTF8STRC("Simplo Technology Co., LTD")},
	{0x020E, UTF8STRC("Omron Healthcare Co., LTD")},
	{0x020F, UTF8STRC("Comodule GMBH")},
	{0x0210, UTF8STRC("ikeGPS")},
	{0x0211, UTF8STRC("Telink Semiconductor Co. Ltd")},
	{0x0212, UTF8STRC("Interplan Co., Ltd")},
	{0x0213, UTF8STRC("Wyler AG")},
	{0x0214, UTF8STRC("IK Multimedia Production srl")},
	{0x0215, UTF8STRC("Lukoton Experience Oy")},
	{0x0216, UTF8STRC("MTI Ltd")},
	{0x0217, UTF8STRC("Tech4home, Lda")},
	{0x0218, UTF8STRC("Hiotech AB")},
	{0x0219, UTF8STRC("DOTT Limited")},
	{0x021A, UTF8STRC("Blue Speck Labs, LLC")},
	{0x021B, UTF8STRC("Cisco Systems, Inc")},
	{0x021C, UTF8STRC("Mobicomm Inc")},
	{0x021D, UTF8STRC("Edamic")},
	{0x021E, UTF8STRC("Goodnet, Ltd")},
	{0x021F, UTF8STRC("Luster Leaf Products Inc")},
	{0x0220, UTF8STRC("Manus Machina BV")},
	{0x0221, UTF8STRC("Mobiquity Networks Inc")},
	{0x0222, UTF8STRC("Praxis Dynamics")},
	{0x0223, UTF8STRC("Philip Morris Products S.A.")},
	{0x0224, UTF8STRC("Comarch SA")},
	{0x0225, UTF8STRC("Nestl Nespresso S.A.")},
	{0x0226, UTF8STRC("Merlinia A/S")},
	{0x0227, UTF8STRC("LifeBEAM Technologies")},
	{0x0228, UTF8STRC("Twocanoes Labs, LLC")},
	{0x0229, UTF8STRC("Muoverti Limited")},
	{0x022A, UTF8STRC("Stamer Musikanlagen GMBH")},
	{0x022B, UTF8STRC("Tesla Motors")},
	{0x022C, UTF8STRC("Pharynks Corporation")},
	{0x022D, UTF8STRC("Lupine")},
	{0x022E, UTF8STRC("Siemens AG")},
	{0x022F, UTF8STRC("Huami (Shanghai) Culture Communication CO., LTD")},
	{0x0230, UTF8STRC("Foster Electric Company, Ltd")},
	{0x0231, UTF8STRC("ETA SA")},
	{0x0232, UTF8STRC("x-Senso Solutions Kft")},
	{0x0233, UTF8STRC("Shenzhen SuLong Communication Ltd")},
	{0x0234, UTF8STRC("FengFan (BeiJing) Technology Co, Ltd")},
	{0x0235, UTF8STRC("Qrio Inc")},
	{0x0236, UTF8STRC("Pitpatpet Ltd")},
	{0x0237, UTF8STRC("MSHeli s.r.l.")},
	{0x0238, UTF8STRC("Trakm8 Ltd")},
	{0x0239, UTF8STRC("JIN CO, Ltd")},
	{0x023A, UTF8STRC("Alatech Tehnology")},
	{0x023B, UTF8STRC("Beijing CarePulse Electronic Technology Co, Ltd")},
	{0x023C, UTF8STRC("Awarepoint")},
	{0x023D, UTF8STRC("ViCentra B.V.")},
	{0x023E, UTF8STRC("Raven Industries")},
	{0x023F, UTF8STRC("WaveWare Technologies Inc.")},
	{0x0240, UTF8STRC("Argenox Technologies")},
	{0x0241, UTF8STRC("Bragi GmbH")},
	{0x0242, UTF8STRC("16Lab Inc")},
	{0x0243, UTF8STRC("Masimo Corp")},
	{0x0244, UTF8STRC("Iotera Inc")},
	{0x0245, UTF8STRC("Endress+Hauser")},
	{0x0246, UTF8STRC("ACKme Networks, Inc.")},
	{0x0247, UTF8STRC("FiftyThree Inc.")},
	{0x0248, UTF8STRC("Parker Hannifin Corp")},
	{0x0249, UTF8STRC("Transcranial Ltd")},
	{0x024A, UTF8STRC("Uwatec AG")},
	{0x024B, UTF8STRC("Orlan LLC")},
	{0x024C, UTF8STRC("Blue Clover Devices")},
	{0x024D, UTF8STRC("M-Way Solutions GmbH")},
	{0x024E, UTF8STRC("Microtronics Engineering GmbH")},
	{0x024F, UTF8STRC("Schneider Schreibgerte GmbH")},
	{0x0250, UTF8STRC("Sapphire Circuits LLC")},
	{0x0251, UTF8STRC("Lumo Bodytech Inc.")},
	{0x0252, UTF8STRC("UKC Technosolution")},
	{0x0253, UTF8STRC("Xicato Inc.")},
	{0x0254, UTF8STRC("Playbrush")},
	{0x0255, UTF8STRC("Dai Nippon Printing Co., Ltd.")},
	{0x0256, UTF8STRC("G24 Power Limited")},
	{0x0257, UTF8STRC("AdBabble Local Commerce Inc.")},
	{0x0258, UTF8STRC("Devialet SA")},
	{0x0259, UTF8STRC("ALTYOR")},
	{0x025A, UTF8STRC("University of Applied Sciences Valais/Haute Ecole Valaisanne")},
	{0x025B, UTF8STRC("Five Interactive, LLC dba Zendo")},
	{0x025C, UTF8STRC("NetEaseHangzhouNetwork co.Ltd.")},
	{0x025D, UTF8STRC("Lexmark International Inc.")},
	{0x025E, UTF8STRC("Fluke Corporation")},
	{0x025F, UTF8STRC("Yardarm Technologies")},
	{0x0260, UTF8STRC("SensaRx")},
	{0x0261, UTF8STRC("SECVRE GmbH")},
	{0x0262, UTF8STRC("Glacial Ridge Technologies")},
	{0x0263, UTF8STRC("Identiv, Inc.")},
	{0x0264, UTF8STRC("DDS, Inc.")},
	{0x0265, UTF8STRC("SMK Corporation")},
	{0x0266, UTF8STRC("Schawbel Technologies LLC")},
	{0x0267, UTF8STRC("XMI Systems SA")},
	{0x0268, UTF8STRC("Cerevo")},
	{0x0269, UTF8STRC("Torrox GmbH & Co KG")},
	{0x026A, UTF8STRC("Gemalto")},
	{0x026B, UTF8STRC("DEKA Research & Development Corp.")},
	{0x026C, UTF8STRC("Domster Tadeusz Szydlowski")},
	{0x026D, UTF8STRC("Technogym SPA")},
	{0x026E, UTF8STRC("FLEURBAEY BVBA")},
	{0x026F, UTF8STRC("Aptcode Solutions")},
	{0x0270, UTF8STRC("LSI ADL Technology")},
	{0x0271, UTF8STRC("Animas Corp")},
	{0x0272, UTF8STRC("Alps Electric Co., Ltd.")},
	{0x0273, UTF8STRC("OCEASOFT")},
	{0x0274, UTF8STRC("Motsai Research")},
	{0x0275, UTF8STRC("Geotab")},
	{0x0276, UTF8STRC("E.G.O. Elektro-Gertebau GmbH")},
	{0x0277, UTF8STRC("bewhere inc")},
	{0x0278, UTF8STRC("Johnson Outdoors Inc")},
	{0x0279, UTF8STRC("steute Schaltgerate GmbH & Co. KG")},
	{0x027A, UTF8STRC("Ekomini inc.")},
	{0x027B, UTF8STRC("DEFA AS")},
	{0x027C, UTF8STRC("Aseptika Ltd")},
	{0x027D, UTF8STRC("HUAWEI Technologies Co., Ltd. ( )")},
	{0x027E, UTF8STRC("HabitAware, LLC")},
	{0x027F, UTF8STRC("ruwido austria gmbh")},
	{0x0280, UTF8STRC("ITEC corporation")},
	{0x0281, UTF8STRC("StoneL")},
	{0x0282, UTF8STRC("Sonova AG")},
	{0x0283, UTF8STRC("Maven Machines, Inc.")},
	{0x0284, UTF8STRC("Synapse Electronics")},
	{0x0285, UTF8STRC("Standard Innovation Inc.")},
	{0x0286, UTF8STRC("RF Code, Inc.")},
	{0x0287, UTF8STRC("Wally Ventures S.L.")},
	{0x0288, UTF8STRC("Willowbank Electronics Ltd")},
	{0x0289, UTF8STRC("SK Telecom")},
	{0x028A, UTF8STRC("Jetro AS")},
	{0x028B, UTF8STRC("Code Gears LTD")},
	{0x028C, UTF8STRC("NANOLINK APS")},
	{0x028D, UTF8STRC("IF, LLC")},
	{0x028E, UTF8STRC("RF Digital Corp")},
	{0x028F, UTF8STRC("Church & Dwight Co., Inc")},
	{0x0290, UTF8STRC("Multibit Oy")},
	{0x0291, UTF8STRC("CliniCloud Inc")},
	{0x0292, UTF8STRC("SwiftSensors")},
	{0x0293, UTF8STRC("Blue Bite")},
	{0x0294, UTF8STRC("ELIAS GmbH")},
	{0x0295, UTF8STRC("Sivantos GmbH")},
	{0x0296, UTF8STRC("Petzl")},
	{0x0297, UTF8STRC("storm power ltd")},
	{0x0298, UTF8STRC("EISST Ltd")},
	{0x0299, UTF8STRC("Inexess Technology Simma KG")},
	{0x029A, UTF8STRC("Currant, Inc.")},
	{0x029B, UTF8STRC("C2 Development, Inc.")},
	{0x029C, UTF8STRC("Blue Sky Scientific, LLC")},
	{0x029D, UTF8STRC("ALOTTAZS LABS, LLC")},
	{0x029E, UTF8STRC("Kupson spol. s r.o.")},
	{0x029F, UTF8STRC("Areus Engineering GmbH")},
	{0x02A0, UTF8STRC("Impossible Camera GmbH")},
	{0x02A1, UTF8STRC("InventureTrack Systems")},
	{0x02A2, UTF8STRC("LockedUp")},
	{0x02A3, UTF8STRC("Itude")},
	{0x02A4, UTF8STRC("Pacific Lock Company")},
	{0x02A5, UTF8STRC("Tendyron Corporation ( )")},
	{0x02A6, UTF8STRC("Robert Bosch GmbH")},
	{0x02A7, UTF8STRC("Illuxtron international B.V.")},
	{0x02A8, UTF8STRC("miSport Ltd.")},
	{0x02A9, UTF8STRC("Chargelib")},
	{0x02AA, UTF8STRC("Doppler Lab")},
	{0x02AB, UTF8STRC("BBPOS Limited")},
	{0x02AC, UTF8STRC("RTB Elektronik GmbH & Co. KG")},
	{0x02AD, UTF8STRC("Rx Networks, Inc.")},
	{0x02AE, UTF8STRC("WeatherFlow, Inc.")},
	{0x02AF, UTF8STRC("Technicolor USA Inc.")},
	{0x02B0, UTF8STRC("Bestechnic(Shanghai),Ltd")},
	{0x02B1, UTF8STRC("Raden Inc")},
	{0x02B2, UTF8STRC("JouZen Oy")},
	{0x02B3, UTF8STRC("CLABER S.P.A.")},
	{0x02B4, UTF8STRC("Hyginex, Inc.")},
	{0x02B5, UTF8STRC("HANSHIN ELECTRIC RAILWAY CO.,LTD.")},
	{0x02B6, UTF8STRC("Schneider Electric")},
	{0x02B7, UTF8STRC("Oort Technologies LLC")},
	{0x02B8, UTF8STRC("Chrono Therapeutics")},
	{0x02B9, UTF8STRC("Rinnai Corporation")},
	{0x02BA, UTF8STRC("Swissprime Technologies AG")},
	{0x02BB, UTF8STRC("Koha.,Co.Ltd")},
	{0x02BC, UTF8STRC("Genevac Ltd")},
	{0x02BD, UTF8STRC("Chemtronics")},
	{0x02BE, UTF8STRC("Seguro Technology Sp. z o.o.")},
	{0x02BF, UTF8STRC("Redbird Flight Simulations")},
	{0x02C0, UTF8STRC("Dash Robotics")},
	{0x02C1, UTF8STRC("LINE Corporation")},
	{0x02C2, UTF8STRC("Guillemot Corporation")},
	{0x02C3, UTF8STRC("Techtronic Power Tools Technology Limited")},
	{0x02C4, UTF8STRC("Wilson Sporting Goods")},
	{0x02C5, UTF8STRC("Lenovo (Singapore) Pte Ltd. ( )")},
	{0x02C6, UTF8STRC("Ayatan Sensors")},
	{0x02C7, UTF8STRC("Electronics Tomorrow Limited")},
	{0x02C8, UTF8STRC("VASCO Data Security International, Inc.")},
	{0x02C9, UTF8STRC("PayRange Inc.")},
	{0x02CA, UTF8STRC("ABOV Semiconductor")},
	{0x02CB, UTF8STRC("AINA-Wireless Inc.")},
	{0x02CC, UTF8STRC("Eijkelkamp Soil & Water")},
	{0x02CD, UTF8STRC("BMA ergonomics b.v.")},
	{0x02CE, UTF8STRC("Teva Branded Pharmaceutical Products R&D, Inc.")},
	{0x02CF, UTF8STRC("Anima")},
	{0x02D0, UTF8STRC("3M")},
	{0x02D1, UTF8STRC("Empatica Srl")},
	{0x02D2, UTF8STRC("Afero, Inc.")},
	{0x02D3, UTF8STRC("Powercast Corporation")},
	{0x02D4, UTF8STRC("Secuyou ApS")},
	{0x02D5, UTF8STRC("OMRON Corporation")},
	{0x02D6, UTF8STRC("Send Solutions")},
	{0x02D7, UTF8STRC("NIPPON SYSTEMWARE CO.,LTD.")},
	{0x02D8, UTF8STRC("Neosfar")},
	{0x02D9, UTF8STRC("Fliegl Agrartechnik GmbH")},
	{0x02DA, UTF8STRC("Gilvader")},
	{0x02DB, UTF8STRC("Digi International Inc (R)")},
	{0x02DC, UTF8STRC("DeWalch Technologies, Inc.")},
	{0x02DD, UTF8STRC("Flint Rehabilitation Devices, LLC")},
	{0x02DE, UTF8STRC("Samsung SDS Co., Ltd.")},
	{0x02DF, UTF8STRC("Blur Product Development")},
	{0x02E0, UTF8STRC("University of Michigan")},
	{0x02E1, UTF8STRC("Victron Energy BV")},
	{0x02E2, UTF8STRC("NTT docomo")},
	{0x02E3, UTF8STRC("Carmanah Technologies Corp.")},
	{0x02E4, UTF8STRC("Bytestorm Ltd.")},
	{0x02E5, UTF8STRC("Espressif Incorporated ( () )")},
	{0x02E6, UTF8STRC("Unwire")},
	{0x02E7, UTF8STRC("Connected Yard, Inc.")},
	{0x02E8, UTF8STRC("American Music Environments")},
	{0x02E9, UTF8STRC("Sensogram Technologies, Inc.")},
	{0x02EA, UTF8STRC("Fujitsu Limited")},
	{0x02EB, UTF8STRC("Ardic Technology")},
	{0x02EC, UTF8STRC("Delta Systems, Inc")},
	{0x02ED, UTF8STRC("HTC Corporation")},
	{0x02EE, UTF8STRC("Citizen Holdings Co., Ltd.")},
	{0x02EF, UTF8STRC("SMART-INNOVATION.inc")},
	{0x02F0, UTF8STRC("Blackrat Software")},
	{0x02F1, UTF8STRC("The Idea Cave, LLC")},
	{0x02F2, UTF8STRC("GoPro, Inc.")},
	{0x02F3, UTF8STRC("AuthAir, Inc")},
	{0x02F4, UTF8STRC("Vensi, Inc.")},
	{0x02F5, UTF8STRC("Indagem Tech LLC")},
	{0x02F6, UTF8STRC("Intemo Technologies")},
	{0x02F7, UTF8STRC("DreamVisions co., Ltd.")},
	{0x02F8, UTF8STRC("Runteq Oy Ltd")},
	{0x02F9, UTF8STRC("IMAGINATION TECHNOLOGIES LTD")},
	{0x02FA, UTF8STRC("CoSTAR TEchnologies")},
	{0x02FB, UTF8STRC("Clarius Mobile Health Corp.")},
	{0x02FC, UTF8STRC("Shanghai Frequen Microelectronics Co., Ltd.")},
	{0x02FD, UTF8STRC("Uwanna, Inc.")},
	{0x02FE, UTF8STRC("Lierda Science & Technology Group Co., Ltd.")},
	{0x02FF, UTF8STRC("Silicon Laboratories")},
	{0x0300, UTF8STRC("World Moto Inc.")},
	{0x0301, UTF8STRC("Giatec Scientific Inc.")},
	{0x0302, UTF8STRC("Loop Devices, Inc")},
	{0x0303, UTF8STRC("IACA electronique")},
	{0x0304, UTF8STRC("Martians Inc")},
	{0x0305, UTF8STRC("Swipp ApS")},
	{0x0306, UTF8STRC("Life Laboratory Inc.")},
	{0x0307, UTF8STRC("FUJI INDUSTRIAL CO.,LTD.")},
	{0x0308, UTF8STRC("Surefire, LLC")},
	{0x0309, UTF8STRC("Dolby Labs")},
	{0x030A, UTF8STRC("Ellisys")},
	{0x030B, UTF8STRC("Magnitude Lighting Converters")},
	{0x030C, UTF8STRC("Hilti AG")},
	{0x030D, UTF8STRC("Devdata S.r.l.")},
	{0x030E, UTF8STRC("Deviceworx")},
	{0x030F, UTF8STRC("Shortcut Labs")},
	{0x0310, UTF8STRC("SGL Italia S.r.l.")},
	{0x0311, UTF8STRC("PEEQ DATA")},
	{0x0312, UTF8STRC("Ducere Technologies Pvt Ltd")},
	{0x0313, UTF8STRC("DiveNav, Inc.")},
	{0x0314, UTF8STRC("RIIG AI Sp. z o.o.")},
	{0x0315, UTF8STRC("Thermo Fisher Scientific")},
	{0x0316, UTF8STRC("AG Measurematics Pvt. Ltd.")},
	{0x0317, UTF8STRC("CHUO Electronics CO., LTD.")},
	{0x0318, UTF8STRC("Aspenta International")},
	{0x0319, UTF8STRC("Eugster Frismag AG")},
	{0x031A, UTF8STRC("Amber wireless GmbH")},
	{0x031B, UTF8STRC("HQ Inc")},
	{0x031C, UTF8STRC("Lab Sensor Solutions")},
	{0x031D, UTF8STRC("Enterlab ApS")},
	{0x031E, UTF8STRC("Eyefi, Inc.")},
	{0x031F, UTF8STRC("MetaSystem S.p.A.")},
	{0x0320, UTF8STRC("SONO ELECTRONICS. CO., LTD")},
	{0x0321, UTF8STRC("Jewelbots")},
	{0x0322, UTF8STRC("Compumedics Limited")},
	{0x0323, UTF8STRC("Rotor Bike Components")},
	{0x0324, UTF8STRC("Astro, Inc.")},
	{0x0325, UTF8STRC("Amotus Solutions")},
	{0x0326, UTF8STRC("Healthwear Technologies (Changzhou)Ltd")},
	{0x0327, UTF8STRC("Essex Electronics")},
	{0x0328, UTF8STRC("Grundfos A/S")},
	{0x0329, UTF8STRC("Eargo, Inc.")},
	{0x032A, UTF8STRC("Electronic Design Lab")},
	{0x032B, UTF8STRC("ESYLUX")},
	{0x032C, UTF8STRC("NIPPON SMT.CO.,Ltd")},
	{0x032D, UTF8STRC("BM innovations GmbH")},
	{0x032E, UTF8STRC("indoormap")},
	{0x032F, UTF8STRC("OttoQ Inc")},
	{0x0330, UTF8STRC("North Pole Engineering")},
	{0x0331, UTF8STRC("3flares Technologies Inc.")},
	{0x0332, UTF8STRC("Electrocompaniet A.S.")},
	{0x0333, UTF8STRC("Mul-T-Lock")},
	{0x0334, UTF8STRC("Corentium AS")},
	{0x0335, UTF8STRC("Enlighted Inc")},
	{0x0336, UTF8STRC("GISTIC")},
	{0x0337, UTF8STRC("AJP2 Holdings, LLC")},
	{0x0338, UTF8STRC("COBI GmbH")},
	{0x0339, UTF8STRC("Blue Sky Scientific, LLC")},
	{0x033A, UTF8STRC("Appception, Inc.")},
	{0x033B, UTF8STRC("Courtney Thorne Limited")},
	{0x033C, UTF8STRC("Virtuosys")},
	{0x033D, UTF8STRC("TPV Technology Limited")},
	{0x033E, UTF8STRC("Monitra SA")},
	{0x033F, UTF8STRC("Automation Components, Inc.")},
	{0x0340, UTF8STRC("Letsense s.r.l.")},
	{0x0341, UTF8STRC("Etesian Technologies LLC")},
	{0x0342, UTF8STRC("GERTEC BRASIL LTDA.")},
	{0x0343, UTF8STRC("Drekker Development Pty. Ltd.")},
	{0x0344, UTF8STRC("Whirl Inc")},
	{0x0345, UTF8STRC("Locus Positioning")},
	{0x0346, UTF8STRC("Acuity Brands Lighting, Inc")},
	{0x0347, UTF8STRC("Prevent Biometrics")},
	{0x0348, UTF8STRC("Arioneo")},
	{0x0349, UTF8STRC("VersaMe")},
	{0x034A, UTF8STRC("Vaddio")},
	{0x034B, UTF8STRC("Libratone A/S")},
	{0x034C, UTF8STRC("HM Electronics, Inc.")},
	{0x034D, UTF8STRC("TASER International, Inc.")},
	{0x034E, UTF8STRC("SafeTrust Inc.")},
	{0x034F, UTF8STRC("Heartland Payment Systems")},
	{0x0350, UTF8STRC("Bitstrata Systems Inc.")},
	{0x0351, UTF8STRC("Pieps GmbH")},
	{0x0352, UTF8STRC("iRiding(Xiamen)Technology Co.,Ltd.")},
	{0x0353, UTF8STRC("Alpha Audiotronics, Inc.")},
	{0x0354, UTF8STRC("TOPPAN FORMS CO.,LTD.")},
	{0x0355, UTF8STRC("Sigma Designs, Inc.")},
	{0x0356, UTF8STRC("Spectrum Brands, Inc.")},
	{0x0357, UTF8STRC("Polymap Wireless")},
	{0x0358, UTF8STRC("MagniWare Ltd.")},
	{0x0359, UTF8STRC("Novotec Medical GmbH")},
	{0x035A, UTF8STRC("Medicom Innovation Partner a/s")},
	{0x035B, UTF8STRC("Matrix Inc.")},
	{0x035C, UTF8STRC("Eaton Corporation")},
	{0x035D, UTF8STRC("KYS")},
	{0x035E, UTF8STRC("Naya Health, Inc.")},
	{0x035F, UTF8STRC("Acromag")},
	{0x0360, UTF8STRC("Insulet Corporation")},
	{0x0361, UTF8STRC("Wellinks Inc.")},
	{0x0362, UTF8STRC("ON Semiconductor")},
	{0x0363, UTF8STRC("FREELAP SA")},
	{0x0364, UTF8STRC("Favero Electronics Srl")},
	{0x0365, UTF8STRC("BioMech Sensor LLC")},
	{0x0366, UTF8STRC("BOLTT Sports technologies Private limited")},
	{0x0367, UTF8STRC("Saphe International")},
	{0x0368, UTF8STRC("Metormote AB")},
	{0x0369, UTF8STRC("littleBits")},
	{0x036A, UTF8STRC("SetPoint Medical")},
	{0x036B, UTF8STRC("BRControls Products BV")},
	{0x036C, UTF8STRC("Zipcar")},
	{0x036D, UTF8STRC("AirBolt Pty Ltd")},
	{0x036E, UTF8STRC("KeepTruckin Inc")},
	{0x036F, UTF8STRC("Motiv, Inc.")},
	{0x0370, UTF8STRC("Wazombi Labs O")},
	{0x0371, UTF8STRC("ORBCOMM")},
	{0x0372, UTF8STRC("Nixie Labs, Inc.")},
	{0x0373, UTF8STRC("AppNearMe Ltd")},
	{0x0374, UTF8STRC("Holman Industries")},
	{0x0375, UTF8STRC("Expain AS")},
	{0x0376, UTF8STRC("Electronic Temperature Instruments Ltd")},
	{0x0377, UTF8STRC("Plejd AB")},
	{0x0378, UTF8STRC("Propeller Health")},
	{0x0379, UTF8STRC("Shenzhen iMCO Electronic Technology Co.,Ltd")},
	{0x037A, UTF8STRC("Algoria")},
	{0x037B, UTF8STRC("Apption Labs Inc.")},
	{0x037C, UTF8STRC("Cronologics Corporation")},
	{0x037D, UTF8STRC("MICRODIA Ltd.")},
	{0x037E, UTF8STRC("lulabytes S.L.")},
	{0x037F, UTF8STRC("Nestec S.A.")},
	{0x0380, UTF8STRC("LLC MEGA - F service")},
	{0x0381, UTF8STRC("Sharp Corporation")},
	{0x0382, UTF8STRC("Precision Outcomes Ltd")},
	{0x0383, UTF8STRC("Kronos Incorporated")},
	{0x0384, UTF8STRC("OCOSMOS Co., Ltd.")},
	{0x0385, UTF8STRC("Embedded Electronic Solutions Ltd. dba e2Solutions")},
	{0x0386, UTF8STRC("Aterica Inc.")},
	{0x0387, UTF8STRC("BluStor PMC, Inc.")},
	{0x0388, UTF8STRC("Kapsch TrafficCom AB")},
	{0x0389, UTF8STRC("ActiveBlu Corporation")},
	{0x038A, UTF8STRC("Kohler Mira Limited")},
	{0x038B, UTF8STRC("Noke")},
	{0x038C, UTF8STRC("Appion Inc.")},
	{0x038D, UTF8STRC("Resmed Ltd")},
	{0x038E, UTF8STRC("Crownstone B.V.")},
	{0x038F, UTF8STRC("Xiaomi Inc.")},
	{0x0390, UTF8STRC("INFOTECH s.r.o.")},
	{0x0391, UTF8STRC("Thingsquare AB")},
	{0x0392, UTF8STRC("T&D")},
	{0x0393, UTF8STRC("LAVAZZA S.p.A.")},
	{0x0394, UTF8STRC("Netclearance Systems, Inc.")},
	{0x0395, UTF8STRC("SDATAWAY")},
	{0x0396, UTF8STRC("BLOKS GmbH")},
	{0x0397, UTF8STRC("LEGO System A/S")},
	{0x0398, UTF8STRC("Thetatronics Ltd")},
	{0x0399, UTF8STRC("Nikon Corporation")},
	{0x039A, UTF8STRC("NeST")},
	{0x039B, UTF8STRC("South Silicon Valley Microelectronics")},
	{0x039C, UTF8STRC("ALE International")},
	{0x039D, UTF8STRC("CareView Communications, Inc.")},
	{0x039E, UTF8STRC("SchoolBoard Limited")},
	{0x039F, UTF8STRC("Molex Corporation")},
	{0x03A0, UTF8STRC("IVT Wireless Limited")},
	{0x03A1, UTF8STRC("Alpine Labs LLC")},
	{0x03A2, UTF8STRC("Candura Instruments")},
	{0x03A3, UTF8STRC("SmartMovt Technology Co., Ltd")},
	{0x03A4, UTF8STRC("Token Zero Ltd")},
	{0x03A5, UTF8STRC("ACE CAD Enterprise Co., Ltd. (ACECAD)")},
	{0x03A6, UTF8STRC("Medela, Inc")},
	{0x03A7, UTF8STRC("AeroScout")},
	{0x03A8, UTF8STRC("Esrille Inc.")},
	{0x03A9, UTF8STRC("THINKERLY SRL")},
	{0x03AA, UTF8STRC("Exon Sp. z o.o.")},
	{0x03AB, UTF8STRC("Meizu Technology Co., Ltd.")},
	{0x03AC, UTF8STRC("Smablo LTD")},
	{0x03AD, UTF8STRC("XiQ")},
	{0x03AE, UTF8STRC("Allswell Inc.")},
	{0x03AF, UTF8STRC("Comm-N-Sense Corp DBA Verigo")},
	{0x03B0, UTF8STRC("VIBRADORM GmbH")},
	{0x03B1, UTF8STRC("Otodata Wireless Network Inc.")},
	{0x03B2, UTF8STRC("Propagation Systems Limited")},
	{0x03B3, UTF8STRC("Midwest Instruments & Controls")},
	{0x03B4, UTF8STRC("Alpha Nodus, inc.")},
	{0x03B5, UTF8STRC("petPOMM, Inc")},
	{0x03B6, UTF8STRC("Mattel")},
	{0x03B7, UTF8STRC("Airbly Inc.")},
	{0x03B8, UTF8STRC("A-Safe Limited")},
	{0x03B9, UTF8STRC("FREDERIQUE CONSTANT SA")},
	{0x03BA, UTF8STRC("Maxscend Microelectronics Company Limited")},
	{0x03BB, UTF8STRC("Abbott Diabetes Care")},
	{0x03BC, UTF8STRC("ASB Bank Ltd")},
	{0x03BD, UTF8STRC("amadas")},
	{0x03BE, UTF8STRC("Applied Science, Inc.")},
	{0x03BF, UTF8STRC("iLumi Solutions Inc.")},
	{0x03C0, UTF8STRC("Arch Systems Inc.")},
	{0x03C1, UTF8STRC("Ember Technologies, Inc.")},
	{0x03C2, UTF8STRC("Snapchat Inc")},
	{0x03C3, UTF8STRC("Casambi Technologies Oy")},
	{0x03C4, UTF8STRC("Pico Technology Inc.")},
	{0x03C5, UTF8STRC("St. Jude Medical, Inc.")},
	{0x03C6, UTF8STRC("Intricon")},
	{0x03C7, UTF8STRC("Structural Health Systems, Inc.")},
	{0x03C8, UTF8STRC("Avvel International")},
	{0x03C9, UTF8STRC("Gallagher Group")},
	{0x03CA, UTF8STRC("In2things Automation Pvt. Ltd.")},
	{0x03CB, UTF8STRC("SYSDEV Srl")},
	{0x03CC, UTF8STRC("Vonkil Technologies Ltd")},
	{0x03CD, UTF8STRC("Wynd Technologies, Inc.")},
	{0x03CE, UTF8STRC("CONTRINEX S.A.")},
	{0x03CF, UTF8STRC("MIRA, Inc.")},
	{0x03D0, UTF8STRC("Watteam Ltd")},
	{0x03D1, UTF8STRC("Density Inc.")},
	{0x03D2, UTF8STRC("IOT Pot India Private Limited")},
	{0x03D3, UTF8STRC("Sigma Connectivity AB")},
	{0x03D4, UTF8STRC("PEG PEREGO SPA")},
	{0x03D5, UTF8STRC("Wyzelink Systems Inc.")},
	{0x03D6, UTF8STRC("Yota Devices LTD")},
	{0x03D7, UTF8STRC("FINSECUR")},
	{0x03D8, UTF8STRC("Zen-Me Labs Ltd")},
	{0x03D9, UTF8STRC("3IWare Co., Ltd.")},
	{0x03DA, UTF8STRC("EnOcean GmbH")},
	{0x03DB, UTF8STRC("Instabeat, Inc")},
	{0x03DC, UTF8STRC("Nima Labs")},
	{0x03DD, UTF8STRC("Andreas Stihl AG & Co. KG")},
	{0x03DE, UTF8STRC("Nathan Rhoades LLC")},
	{0x03DF, UTF8STRC("Grob Technologies, LLC")},
	{0x03E0, UTF8STRC("Actions (Zhuhai) Technology Co., Limited")},
	{0x03E1, UTF8STRC("SPD Development Company Ltd")},
	{0x03E2, UTF8STRC("Sensoan Oy")},
	{0x03E3, UTF8STRC("Qualcomm Life Inc")},
	{0x03E4, UTF8STRC("Chip-ing AG")},
	{0x03E5, UTF8STRC("ffly4u")},
	{0x03E6, UTF8STRC("IoT Instruments Oy")},
	{0x03E7, UTF8STRC("TRUE Fitness Technology")},
	{0x03E8, UTF8STRC("Reiner Kartengeraete GmbH & Co. KG.")},
	{0x03E9, UTF8STRC("SHENZHEN LEMONJOY TECHNOLOGY CO., LTD.")},
	{0x03EA, UTF8STRC("Hello Inc.")},
	{0x03EB, UTF8STRC("Evollve Inc.")},
	{0x03EC, UTF8STRC("Jigowatts Inc.")},
	{0x03ED, UTF8STRC("BASIC MICRO.COM,INC.")},
	{0x03EE, UTF8STRC("CUBE TECHNOLOGIES")},
	{0x03EF, UTF8STRC("foolography GmbH")},
	{0x03F0, UTF8STRC("CLINK")},
	{0x03F1, UTF8STRC("Hestan Smart Cooking Inc.")},
	{0x03F2, UTF8STRC("WindowMaster A/S")},
	{0x03F3, UTF8STRC("Flowscape AB")},
	{0x03F4, UTF8STRC("PAL Technologies Ltd")},
	{0x03F5, UTF8STRC("WHERE, Inc.")},
	{0x03F6, UTF8STRC("Iton Technology Corp.")},
	{0x03F7, UTF8STRC("Owl Labs Inc.")},
	{0x03F8, UTF8STRC("Rockford Corp.")},
	{0x03F9, UTF8STRC("Becon Technologies Co.,Ltd.")},
	{0x03FA, UTF8STRC("Vyassoft Technologies Inc")},
	{0x03FB, UTF8STRC("Nox Medical")},
	{0x03FC, UTF8STRC("Kimberly-Clark")},
	{0x03FD, UTF8STRC("Trimble Navigation Ltd.")},
	{0x03FE, UTF8STRC("Littelfuse")},
	{0x03FF, UTF8STRC("Withings")},
	{0x0400, UTF8STRC("i-developer IT Beratung UG")},
	{0x0402, UTF8STRC("Sears Holdings Corporation")},
	{0x0403, UTF8STRC("Gantner Electronic GmbH")},
	{0x0404, UTF8STRC("Authomate Inc")},
	{0x0405, UTF8STRC("Vertex International, Inc.")},
	{0x0406, UTF8STRC("Airtago")},
	{0x0407, UTF8STRC("Swiss Audio SA")},
	{0x0408, UTF8STRC("ToGetHome Inc.")},
	{0x0409, UTF8STRC("AXIS")},
	{0x040A, UTF8STRC("Openmatics")},
	{0x040B, UTF8STRC("Jana Care Inc.")},
	{0x040C, UTF8STRC("Senix Corporation")},
	{0x040D, UTF8STRC("NorthStar Battery Company, LLC")},
	{0x040E, UTF8STRC("SKF (U.K.) Limited")},
	{0x040F, UTF8STRC("CO-AX Technology, Inc.")},
	{0x0410, UTF8STRC("Fender Musical Instruments")},
	{0x0411, UTF8STRC("Luidia Inc")},
	{0x0412, UTF8STRC("SEFAM")},
	{0x0413, UTF8STRC("Wireless Cables Inc")},
	{0x0414, UTF8STRC("Lightning Protection International Pty Ltd")},
	{0x0415, UTF8STRC("Uber Technologies Inc")},
	{0x0416, UTF8STRC("SODA GmbH")},
	{0x0417, UTF8STRC("Fatigue Science")},
	{0x0418, UTF8STRC("Alpine Electronics Inc.")},
	{0x0419, UTF8STRC("Novalogy LTD")},
	{0x041A, UTF8STRC("Friday Labs Limited")},
	{0x041B, UTF8STRC("OrthoAccel Technologies")},
	{0x041C, UTF8STRC("WaterGuru, Inc.")},
	{0x041D, UTF8STRC("Benning Elektrotechnik und Elektronik GmbH & Co. KG")},
	{0x041E, UTF8STRC("Dell Computer Corporation")},
	{0x041F, UTF8STRC("Kopin Corporation")},
	{0x0420, UTF8STRC("TecBakery GmbH")},
	{0x0421, UTF8STRC("Backbone Labs, Inc.")},
	{0x0422, UTF8STRC("DELSEY SA")},
	{0x0423, UTF8STRC("Chargifi Limited")},
	{0x0424, UTF8STRC("Trainesense Ltd.")},
	{0x0425, UTF8STRC("Unify Software and Solutions GmbH & Co. KG")},
	{0x0426, UTF8STRC("Husqvarna AB")},
	{0x0427, UTF8STRC("Focus fleet and fuel management inc")},
	{0x0428, UTF8STRC("SmallLoop, LLC")},
	{0x0429, UTF8STRC("Prolon Inc.")},
	{0x042A, UTF8STRC("BD Medical")},
	{0x042B, UTF8STRC("iMicroMed Incorporated")},
	{0x042C, UTF8STRC("Ticto N.V.")},
	{0x042D, UTF8STRC("Meshtech AS")},
	{0x042E, UTF8STRC("MemCachier Inc.")},
	{0x042F, UTF8STRC("Danfoss A/S")},
	{0x0430, UTF8STRC("SnapStyk Inc.")},
	{0x0431, UTF8STRC("Amyway Corporation")},
	{0x0432, UTF8STRC("Silk Labs, Inc.")},
	{0x0433, UTF8STRC("Pillsy Inc.")},
	{0x0434, UTF8STRC("Hatch Baby, Inc.")},
	{0x0435, UTF8STRC("Blocks Wearables Ltd.")},
	{0x0436, UTF8STRC("Drayson Technologies (Europe) Limited")},
	{0x0437, UTF8STRC("eBest IOT Inc.")},
	{0x0438, UTF8STRC("Helvar Ltd")},
	{0x0439, UTF8STRC("Radiance Technologies")},
	{0x043A, UTF8STRC("Nuheara Limited")},
	{0x043B, UTF8STRC("Appside co., ltd.")},
	{0x043C, UTF8STRC("DeLaval")},
	{0x043D, UTF8STRC("Coiler Corporation")},
	{0x043E, UTF8STRC("Thermomedics, Inc.")},
	{0x043F, UTF8STRC("Tentacle Sync GmbH")},
	{0x0440, UTF8STRC("Valencell, Inc.")},
	{0x0441, UTF8STRC("iProtoXi Oy")},
	{0x0442, UTF8STRC("SECOM CO., LTD.")},
	{0x0443, UTF8STRC("Tucker International LLC")},
	{0x0444, UTF8STRC("Metanate Limited")},
	{0x0445, UTF8STRC("Kobian Canada Inc.")},
	{0x0446, UTF8STRC("NETGEAR, Inc.")},
	{0x0447, UTF8STRC("Fabtronics Australia Pty Ltd")},
	{0x0448, UTF8STRC("Grand Centrix GmbH")},
	{0x0449, UTF8STRC("1UP USA.com llc")},
	{0x044A, UTF8STRC("SHIMANO INC.")},
	{0x044B, UTF8STRC("Nain Inc.")},
	{0x044C, UTF8STRC("LifeStyle Lock, LLC")},
	{0x044D, UTF8STRC("VEGA Grieshaber KG")},
	{0x044E, UTF8STRC("Xtrava Inc.")},
	{0x044F, UTF8STRC("TTS Tooltechnic Systems AG & Co. KG")},
	{0x0450, UTF8STRC("Teenage Engineering AB")},
	{0x0451, UTF8STRC("Tunstall Nordic AB")},
	{0x0452, UTF8STRC("Svep Design Center AB")},
	{0x0453, UTF8STRC("GreenPeak Technologies BV")},
	{0x0454, UTF8STRC("Sphinx Electronics GmbH & Co KG")},
	{0x0455, UTF8STRC("Atomation")},
	{0x0456, UTF8STRC("Nemik Consulting Inc")},
	{0x0457, UTF8STRC("RF INNOVATION")},
	{0x0458, UTF8STRC("Mini Solution Co., Ltd.")},
	{0x0459, UTF8STRC("Lumenetix, Inc")},
	{0x045A, UTF8STRC("2048450 Ontario Inc")},
	{0x045B, UTF8STRC("SPACEEK LTD")},
	{0x045C, UTF8STRC("Delta T Corporation")},
	{0x045D, UTF8STRC("Boston Scientific Corporation")},
	{0x045E, UTF8STRC("Nuviz, Inc.")},
	{0x045F, UTF8STRC("Real Time Automation, Inc.")},
	{0x0460, UTF8STRC("Kolibree")},
	{0x0461, UTF8STRC("vhf elektronik GmbH")},
	{0x0462, UTF8STRC("Bonsai Systems GmbH")},
	{0x0463, UTF8STRC("Fathom Systems Inc.")},
	{0x0464, UTF8STRC("Bellman & Symfon")},
	{0x0465, UTF8STRC("International Forte Group LLC")},
	{0x0466, UTF8STRC("CycleLabs Solutions inc.")},
	{0x0467, UTF8STRC("Codenex Oy")},
	{0x0468, UTF8STRC("Kynesim Ltd")},
	{0x0469, UTF8STRC("Palago AB")},
	{0x046A, UTF8STRC("INSIGMA INC.")},
	{0x046B, UTF8STRC("PMD Solutions")},
	{0x046C, UTF8STRC("Qingdao Realtime Technology Co., Ltd.")},
	{0x046D, UTF8STRC("BEGA Gantenbrink-Leuchten KG")},
	{0x046E, UTF8STRC("Pambor Ltd.")},
	{0xFFFF, UTF8STRC("SPECIAL USE/DEFAULT")},
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
			sb.AppendC(UTF8STRC("Unknown (0x"));
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
	Text::CString vName = {0, 0};
	switch (scanType)
	{
	case 1:
		vName = {UTF8STRC("Active")};
		break;
	}
	frame->AddHex8Name(frameOfst, "Scan Type", scanType, vName);
}

void Net::PacketAnalyzerBluetooth::AddScanFilterPolicy(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 scanType)
{
	Text::CString vName = {0, 0};
	switch (scanType)
	{
	case 0:
		vName = {UTF8STRC("Accept all advertisements, except directed advertisements not addressed to this device")};
		break;
	}
	frame->AddHex8Name(frameOfst, "Scan Filter Policy", scanType, vName);
}

void Net::PacketAnalyzerBluetooth::AddScanInterval(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 scanInt)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanInt);
	sb.AppendC(UTF8STRC(" ("));
	Text::SBAppendF64(&sb, 0.625 * scanInt);
	sb.AppendC(UTF8STRC("ms)"));
	frame->AddField(frameOfst, 2, (const UTF8Char*)"Scan Interval", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddScanWindow(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 scanWind)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendU16(scanWind);
	sb.AppendC(UTF8STRC(" ("));
	Text::SBAppendF64(&sb, 0.625 * scanWind);
	sb.AppendC(UTF8STRC("ms)"));
	frame->AddField(frameOfst, 2, (const UTF8Char*)"Scan Window", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddAddrType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, UInt8 addrType)
{
	Text::CString vName = {0, 0};
	switch (addrType)
	{
	case 0:
		vName = {UTF8STRC("Public Device Address")};
		break;
	case 1:
		vName = {UTF8STRC("Random Address Type")};
		break;
	}
	frame->AddHex8Name(frameOfst, name, addrType, vName);
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
	Text::CString vName = {0, 0};
	switch (evtType)
	{
	case 0:
		vName = {UTF8STRC("Connectable Undirected Advertising")};
		break;
	case 3:
		vName = {UTF8STRC("Non-Connectable Undirected Advertising")};
		break;
	case 4:
		vName = {UTF8STRC("Scan Response")};
		break;
	}
	frame->AddHex8Name(frameOfst, "Event Type", evtType, vName);
}

void Net::PacketAnalyzerBluetooth::AddRSSI(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, Int8 rssi)
{
	UTF8Char sbuff[16];
	Text::StrConcatC(Text::StrInt16(sbuff, rssi), UTF8STRC("dBm"));
	frame->AddField(frameOfst, 1, (const UTF8Char*)"RSSI", sbuff);
}

void Net::PacketAnalyzerBluetooth::AddAdvData(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize)
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
			frame->AddField(frameOfst + i, (UInt32)packetSize - i, (const UTF8Char*)"Unused", 0);
			return;
		}
		frame->AddUInt(frameOfst + i, 1, "Adv Item Len", len);
		frame->AddHex8(frameOfst + i + 1, "Adv Item Type", packet[i + 1]);
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
				if (found) sb.AppendChar(',', 1);
				sb.AppendC(UTF8STRC(" LE General Discoverable Mode"));
				found = true;
			}
			if (packet[i + 2] & 4)
			{
				if (found) sb.AppendChar(',', 1);
				sb.AppendC(UTF8STRC(" BR/EDR Not Supported"));
				found = true;
			}
			if (packet[i + 2] & 8)
			{
				if (found) sb.AppendChar(',', 1);
				sb.AppendC(UTF8STRC(" Simultaneous LE and BR/EDR to Same Device Capable (Controller)"));
				found = true;
			}
			if (packet[i + 2] & 16)
			{
				if (found) sb.AppendChar(',', 1);
				sb.AppendC(UTF8STRC(" Simultaneous LE and BR/EDR to Same Device Capable (Host)"));
				found = true;
			}
			frame->AddField(frameOfst + i + 2, 1, (const UTF8Char*)"Flags", sb.ToString());
		}
		else if (packet[i + 1] == 2 && len == 3)
		{
			u16 = ReadUInt16(&packet[i + 2]);
			vName = {0, 0};
			switch (u16)
			{
			case 0xFE24:
				vName = {UTF8STRC("August Home Inc")};
				break;
			}
			frame->AddHex16Name(frameOfst + i + 2, "16-bit Service Class UUIDs (incomplete)", u16, vName);
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
					sb.AppendC(UTF8STRC("Unknown(0x"));
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
			sb.AppendC(UTF8STRC("dBm"));
			frame->AddField(frameOfst + i + 2, 1, (const UTF8Char*)"Tx Power Level", sb.ToString());
		}
		else if (packet[i + 1] == 0xff && len >= 3)
		{
			UInt16 compId = ReadUInt16(&packet[i + 2]);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("0x"));
			sb.AppendHex16(compId);
			Text::CString cstr = CompanyGetName(compId);
			if (cstr.v)
			{
				sb.AppendC(UTF8STRC(" ("));
				sb.Append(cstr);
				sb.AppendChar(')', 1);
			}
			else
			{
				sb.AppendC(UTF8STRC(" (Unknown)"));
			}
			if (compId == 0x4C)
			{
				/////////////////// https://github.com/furiousMAC/continuity/tree/master/messages
				frame->AddField(frameOfst + i + 2, 2, (const UTF8Char*)"Manufacturer Specific", sb.ToString());
				j = 4;
				while (j < (UInt32)len - 1)
				{
					vName = {0, 0};
					switch (packet[i + j])
					{
					case 2:
						vName = {UTF8STRC("Proximity Beacons")};
						break;
					case 3:
						vName = {UTF8STRC("AirPrint Message")};
						break;
					case 5:
						vName = {UTF8STRC("AirDrop Message")};
						break;
					case 6:
						vName = {UTF8STRC("HomeKit Message")};
						break;
					case 7:
						vName = {UTF8STRC("Proximity Pairing Message")};
						break;
					case 8:
						vName = {UTF8STRC("Hey Siri Message")};
						break;
					case 9:
						vName = {UTF8STRC("Airplay Target Message")};
						break;
					case 10:
						vName = {UTF8STRC("Airplay Source Message")};
						break;
					case 11:
						vName = {UTF8STRC("Magic Switch Message")};
						break;
					case 12:
						vName = {UTF8STRC("Handoff Message")};
						break;
					case 13:
						vName = {UTF8STRC("Tethering Target Message")};
						break;
					case 14:
						vName = {UTF8STRC("Tethering Source Message")};
						break;
					case 15:
						vName = {UTF8STRC("Nearby Action")};
						break;
					case 16:
						vName = {UTF8STRC("Nearby Info")};
						break;
					case 18:
						vName = {UTF8STRC("Find My Message")};
						break;
					}
					frame->AddHex8Name(frameOfst + i + j, "Apple Type", packet[i + j], vName);
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
							vName = {0, 0};
							switch (packet[i + j + 3])
							{
							case 1:
								vName = {UTF8STRC("Apple TV Setup")};
								break;
							case 4:
								vName = {UTF8STRC("Mobile Backup")};
								break;
							case 0x05:
								vName = {UTF8STRC("Watch Setup")};
								break;
							case 0x06:
								vName = {UTF8STRC("Apple TV Pair")};
								break;
							case 0x07:
								vName = {UTF8STRC("Internet Relay")};
								break;
							case 0x08:
								vName = {UTF8STRC("WiFi Password")};
								break;
							case 0x09:
								vName = {UTF8STRC("iOS Setup")};
								break;
							case 0x0A:
								vName = {UTF8STRC("Repair")};
								break;
							case 0x0B:
								vName = {UTF8STRC("Speaker Setupd")};
								break;
							case 0x0C:
								vName = {UTF8STRC("Apple Pay")};
								break;
							case 0x0D:
								vName = {UTF8STRC("Whole Home Audio Setup")};
								break;
							case 0x0E:
								vName = {UTF8STRC("Developer Tools Pairing Request")};
								break;
							case 0x0F:
								vName = {UTF8STRC("Answered Call")};
								break;
							case 0x10:
								vName = {UTF8STRC("Ended Call")};
								break;
							case 0x11:
								vName = {UTF8STRC("DD Ping")};
								break;
							case 0x12:
								vName = {UTF8STRC("DD Pong")};
								break;
							case 0x13:
								vName = {UTF8STRC("Remote Auto Fill")};
								break;
							case 0x14:
								vName = {UTF8STRC("Companion Link Proximity")};
								break;
							case 0x15:
								vName = {UTF8STRC("Remote Management")};
								break;
							case 0x16:
								vName = {UTF8STRC("Remote Auto Fill Pong")};
								break;
							case 0x17:
								vName = {UTF8STRC("Remote Display")};
								break;
							}
							frame->AddUIntName(frameOfst + i + j + 3, 1, "Action Type", packet[i + j + 3], vName);
							if (appLen >= 5)
							{
								frame->AddHexBuff(frameOfst + i + j + 4, 3, "Authentication Tag", &packet[i + j + 4], false);
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
				sb.AppendC(UTF8STRC(", Value="));
				sb.AppendHexBuff(&packet[i + 4], (UOSInt)len - 3, ' ', Text::LineBreakType::None);
				frame->AddField(frameOfst + i + 2, (UOSInt)len - 1, (const UTF8Char*)"Manufacturer Specific", sb.ToString());
			}
		}
		else
		{
			sb.ClearStr();
			sb.AppendHexBuff(packet + i + 2, (UOSInt)len - 1, ' ', Text::LineBreakType::None);
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
	Text::CString vName = {0, 0};
	switch (len)
	{
	case 4:
		vName = {UTF8STRC("5.12 sec")};
		break;
	}
	frame->AddUIntName(frameOfst, 1, "Inquiry Length", len, vName);
}

void Net::PacketAnalyzerBluetooth::AddPageScanMode(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 mode)
{
	Text::CString vName = {0, 0};
	switch (mode)
	{
	case 1:
		vName = {UTF8STRC("R1")};
		break;
	}
	frame->AddHex8Name(frameOfst, "Page Scan Repetition Mode", mode, vName);
}

void Net::PacketAnalyzerBluetooth::AddClassOfDevice(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt32 cls)
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
	frame->AddField(frameOfst, 3, (const UTF8Char*)"Class Of Device", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddClockOffset(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 ofst)
{
	frame->AddHex16(frameOfst, "Clock Offset", ofst);
}

void Net::PacketAnalyzerBluetooth::AddExAdvEvtType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 evtType)
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
	frame->AddField(frameOfst, 2, (const UTF8Char*)"Event Type", sb.ToString());
}

void Net::PacketAnalyzerBluetooth::AddPHYType(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const Char *name, UInt8 phyType)
{
	Text::CString vName = {0, 0};
	switch (phyType)
	{
	case 0:
		vName = {UTF8STRC("No packets")};
		break;
	case 1:
		vName = {UTF8STRC("LE 1M")};
		break;
	}
	frame->AddHex8Name(frameOfst, name, phyType, vName);
}

void Net::PacketAnalyzerBluetooth::AddAdvSID(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt8 sid)
{
	Text::CString vName = {0, 0};
	switch (sid)
	{
	case 0xFF:
		vName = {UTF8STRC("Not available")};
		break;
	}
	frame->AddHex8Name(frameOfst, "Advertising SID", sid, vName);
}

void Net::PacketAnalyzerBluetooth::AddTxPower(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, Int8 txPower)
{
	UTF8Char sbuff[16];
	Text::StrConcatC(Text::StrInt16(sbuff, txPower), UTF8STRC("dBm"));
	frame->AddField(frameOfst, 1, (const UTF8Char*)"TxPower", sbuff);
}

void Net::PacketAnalyzerBluetooth::AddPeriodicAdv(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, UInt16 interval)
{
	Text::CString vName = {0, 0};
	switch (interval)
	{
	case 0:
		vName = {UTF8STRC("No periodic advertising")};
		break;
	}
	frame->AddHex16Name(frameOfst, "Periodic Advertising Interval", interval, vName);
}

void Net::PacketAnalyzerBluetooth::AddUnknown(IO::FileAnalyse::FrameDetailHandler *frame, UInt32 frameOfst, const UInt8 *packet, UOSInt packetSize)
{
	frame->AddHexBuff(frameOfst, (UInt32)packetSize, "Unknown", packet, true);
}

Bool Net::PacketAnalyzerBluetooth::PacketGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	UInt8 mac[6];
	Text::CString name;
	switch (packet[4])
	{
	case 1:
		name = CmdGetName(ReadUInt16(&packet[5]));
		if (name.v)
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
			name = CmdGetName(ReadUInt16(&packet[8]));
			if (name.v)
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
			name = CmdGetName(ReadUInt16(&packet[9]));
			if (name.v)
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

void Net::PacketAnalyzerBluetooth::PacketGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerBluetooth::PacketGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
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
			vName = {0, 0};
			switch (packet[7])
			{
			case 0:
				vName = {UTF8STRC("Success")};
				break;
			}
			frame->AddHex8Name(frameOfst + 7, "Status", packet[7], vName);
			if (packetSize > 8)
			{
				AddUnknown(frame, frameOfst + 8, packet + 8, packetSize + 8);
			}
			break;
		case 0x0E: //Command Complete
			frame->AddUInt(frameOfst + 7, 1, "Number of Allowed Command Packets", packet[7]);
			AddCmdOpcode(frame, frameOfst + 8, ReadUInt16(&packet[8]));
			vName = {0, 0};
			switch (packet[10])
			{
			case 0:
				vName = {UTF8STRC("Success")};
				break;
			}
			frame->AddHex8Name(frameOfst + 10, "Status", packet[10], vName);
			if (packetSize > 11)
			{
				AddUnknown(frame, frameOfst + 11, packet + 11, packetSize - 11);
			}
			break;
		case 0x0F: //Command Status
			vName = {0, 0};
			switch (packet[7])
			{
			case 0:
				vName = {UTF8STRC("Pending")};
				break;
			}
			frame->AddHex8Name(frameOfst + 7, "Status", packet[7], vName);
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
			return {vendorList[k].vendorName, vendorList[k].nameLen};
		}
	}
	return {0, 0};
}

Text::CString Net::PacketAnalyzerBluetooth::CmdGetName(UInt16 cmd)
{
	switch (cmd)
	{
	case 0x0401:
		return {UTF8STRC("HCI Inquiry")};
	case 0x2005:
		return {UTF8STRC("LE Set Random Address")};
	case 0x200B:
		return {UTF8STRC("LE Set Scan Parameter")};
	case 0x200C:
		return {UTF8STRC("LE Set Scan Enable")};
	default:
		return {0, 0};
	}
}

Text::CString Net::PacketAnalyzerBluetooth::HCIPacketTypeGetName(UInt8 packetType)
{
	switch (packetType)
	{
	case 1:
		return {UTF8STRC("HCI Command")};
	case 4:
		return {UTF8STRC("HCI Event")};
	default:
		return {0, 0};	
	}
}

Text::CString Net::PacketAnalyzerBluetooth::HCIEventGetName(UInt8 evt)
{
	switch (evt)
	{
	case 0x0E:
		return {UTF8STRC("Command Complete")};
	case 0x0F:
		return {UTF8STRC("Command Status")};
	case 0x2F:
		return {UTF8STRC("Extended Inquery Result")};
	case 0x3E:
		return {UTF8STRC("LE Meta")};
	default:
		return {0, 0};
	}
}

Text::CString Net::PacketAnalyzerBluetooth::LESubEventGetName(UInt8 subEvt)
{
	switch (subEvt)
	{
	case 0x02:
		return {UTF8STRC("LE Advertising Report")};
	case 0x0D:
		return {UTF8STRC("LE Extended Advertising Report")};
	default:
		return {0, 0};
	}
}
