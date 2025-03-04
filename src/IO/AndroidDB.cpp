#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/AndroidDB.h"
#include "Text/MyString.h"

IO::AndroidDB::AndroidInfo IO::AndroidDB::androids[] = {
//	androidId           brandName      modelName                modelNum        cpuName
//	{UTF8STRCPTR("8S89H_E3"),        "",            "",                      "",             UTF8STRCPTR("")},
	{UTF8STRCPTR("A75B"),            "Evercoss",    "A75B",                  "A75B",         UTF8STRCPTR("MT6735")},
	{UTF8STRCPTR("ALE-CL00"),        "Huawei",      "P8 Lite",               "ALE-CL00",     UTF8STRCPTR("MSM8939")},
	{UTF8STRCPTR("ALP-AL00"),        "Huawei",      "Mate 10",               "ALP-AL00",     UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("ALP-L29"),         "Huawei",      "Mate 10 Dual SIM",      "ALP-L29",      UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("ANE-LX2"),         "Huawei",      "P20 Lite",              "ANE-LX2",      UTF8STRCPTR("Kirin 659")},
	{UTF8STRCPTR("ASK-AL00x"),       "Huawei",      "Honor Play 3 Dual SIM", "ASK-AL00x",    UTF8STRCPTR("Kirin 710")},
	{UTF8STRCPTR("ASUS_I001DC"),     "ASUS",        "ROG PHONE 2",           "ASUS_I001DC",  UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("ASUS_X00QD"),      "ASUS",        "ZenFone 5 (ZE620KL)",   "ASUS_X00QD",   UTF8STRCPTR("SDM636")},
	{UTF8STRCPTR("ASUS_X00RD"),      "ASUS",        "ZenFone Live (ZA550AL)","ASUS_X00RD",   UTF8STRCPTR("MSM8917")},
	{UTF8STRCPTR("ASUS_X018D"),      "ASUS",        "Zenfone Max Plus (ZB570TL)","ASUS_X018D",UTF8STRCPTR("MT6750T")},
	{UTF8STRCPTR("ASUS_Z00AD"),      "ASUS",        "ZenFone 2 (ZE551ML)",   "ASUS_Z00AD",   UTF8STRCPTR("Intel Z3580")},
	{UTF8STRCPTR("ASUS_Z012DA"),     "ASUS",        "ZenFone 3 (ZE552KL)",   "ASUS_Z012DA",  UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("BKL-AL20"),        "Huawei",      "Honor V10",             "BKL-AL20",     UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("CHC-U01"),         "Huawei",      "G Play Mini",           "CHC-U01",      UTF8STRCPTR("Kirin 620")},
	{UTF8STRCPTR("CLT-L29"),         "Huawei",      "P20 Pro",               "CLT-L29",      UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("CPH1611"),         "OPPO",        "R9s Plus",              "CPH1611",      UTF8STRCPTR("MSM8976")},
	{UTF8STRCPTR("CPH1909"),         "OPPO",        "A5s Dual-SIM",          "CPH1909",      UTF8STRCPTR("MT6765")},
	{UTF8STRCPTR("CPH1941"),         "OPPO",        "A9 2020 Dual-SIM",      "CPH1941",      UTF8STRCPTR("SM6125")},
	{UTF8STRCPTR("CPH1969"),         "OPPO",        "F11 Pro",               "CPH1969",      UTF8STRCPTR("MT6771")},
	{UTF8STRCPTR("Che1-CL10"),       "Huawei",      "Honor 4X",              "Che1-CL10",    UTF8STRCPTR("MSM8916")},
	{UTF8STRCPTR("Che2-TL00"),       "Huawei",      "Honor 4X",              "Che2-TL00",    UTF8STRCPTR("Kirin 620")},
	{UTF8STRCPTR("Coolpad Y82-520"), "Coolpad",     "Y82-520",               "Y82-520",      UTF8STRCPTR("MSM8916")},
	{UTF8STRCPTR("E2303"),           "SONY",        "Xperia M4 Aqua",        "E2303",        UTF8STRCPTR("MSM8939")},
	{UTF8STRCPTR("E6633"),           "SONY",        "Xperia Z5 Dual",        "E6633",        UTF8STRCPTR("MSM8994")},
	{UTF8STRCPTR("ELE-L29"),         "Huawei",      "P30 Dual SIM",          "ELE-L29",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("EML-AL00"),        "Huawei",      "P20 Dual SIM",          "EML-AL00",     UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("EML-L29"),         "Huawei",      "P20",                   "EML-L29",      UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("EVA-L19"),         "Huawei",      "P9",                    "EVA-L19",      UTF8STRCPTR("Kirin 955")},
	{UTF8STRCPTR("EVR-L29"),         "Huawei",      "Mate 20 X Dual SIM",    "EVR-L29",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("FIG-LX2"),         "Huawei",      "Y7s",                   "FIG-LX2",      UTF8STRCPTR("Kirin 659")},
	{UTF8STRCPTR("FLA-LX2"),         "Huawei",      "Y9",                    "FLA-LX2",      UTF8STRCPTR("Kirin 659")},
	{UTF8STRCPTR("FRD-AL00"),        "Huawei",      "Honor 8 Dual SIM",      "FRD-AL00",     UTF8STRCPTR("Kirin 950")},
	{UTF8STRCPTR("G3116"),           "SONY",        "Xperia XA1 Dual",       "G3116",        UTF8STRCPTR("MT6757")},
	{UTF8STRCPTR("G8142"),           "SONY",        "Xperia XZ Premium Dual","G8142",        UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("G8232"),           "SONY",        "Xperia XZs",            "G8232",        UTF8STRCPTR("MSM8996")},
	{UTF8STRCPTR("GCE x86 Phone"),   "Unknown",     "Android Simulator",     "",             UTF8STRCPTR("")},
	{UTF8STRCPTR("GIONEE S10CL"),    "Gionee",      "S10C",                  "S10CL",        UTF8STRCPTR("MSM8920")},
	{UTF8STRCPTR("GN5005"),          "Gionee",      "GN5005",                "GN5005",       UTF8STRCPTR("MT6737")},
	{UTF8STRCPTR("H4133"),           "SONY",        "Xperia XA2 Dual",       "H4133",        UTF8STRCPTR("SDM630")},
	{UTF8STRCPTR("H4233"),           "SONY",        "Xperia XA2 Ultra Dual", "H4233",        UTF8STRCPTR("SDM630")},
	{UTF8STRCPTR("HMA-AL00"),        "Huawei",      "Mate 20 Dual Slot",     "HMA-AL00",     UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("HMA-L09"),         "Huawei",      "Mate 20",               "HMA-L09",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("HMA-L29"),         "Huawei",      "Mate 20 Dual SIM",      "HMA-L29",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("HMA-TL00"),        "Huawei",      "Mate 20 Dual Slot",     "HMA-TL00",     UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("HTC 2Q7A100"),     "HTC",         "U19e",                  "2Q7A100",      UTF8STRCPTR("SDM710")},
	{UTF8STRCPTR("HTC 10 evo"),      "HTC",         "10 evo",                "10 evo",       UTF8STRCPTR("MSM8994")},
	{UTF8STRCPTR("HTC One X9 dual sim"),"HTC",      "HTC One X9 dual sim",   "X9 dual sim",  UTF8STRCPTR("MT6795")},
	{UTF8STRCPTR("HTC U12 life"),    "HTC",         "U12 life",              "U12 life",     UTF8STRCPTR("SDM636")},
	{UTF8STRCPTR("HTC_U-2u"),        "HTC",         "U Play (U-2u)",         "U-2u",         UTF8STRCPTR("MT6755")},
	{UTF8STRCPTR("HTC_U-3u"),        "HTC",         "U11",                   "U-3u",         UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("IM-A860K"),        "Pantech",     "Vega No 6",             "IM-A860K",     UTF8STRCPTR("APQ8064")},
	{UTF8STRCPTR("INE-LX2"),         "Huawei",      "nova 3i",               "INE-LX2",      UTF8STRCPTR("Kirin 710")},
	{UTF8STRCPTR("J9110"),           "SONY",        "Xperia 1 Dual SIM",     "J9110",        UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("JMM-AL10"),        "Huawei",      "Honor V9 Play",         "JMM-AL10",     UTF8STRCPTR("MT6750")},
	{UTF8STRCPTR("LDN-AL00"),        "Huawei",      "Enjoy 8 Dual SIM",      "LDN-AL00",     UTF8STRCPTR("MSM8937")},
	{UTF8STRCPTR("LDN-L21"),         "Huawei",      "Y7 2018",               "LDN-L01",      UTF8STRCPTR("MSM8937")},
	{UTF8STRCPTR("LDN-LX1"),         "Huawei",      "Y7 Prime 2018 Dual",    "LDN-LX1",      UTF8STRCPTR("MSM8940")},
	{UTF8STRCPTR("LDN-LX2"),         "Huawei",      "Y7 Prime 2018",         "LDN-LX2",      UTF8STRCPTR("MSM8937")},
	{UTF8STRCPTR("LG-D500"),         "LG",          "Optimus F6",            "LG-D500",      UTF8STRCPTR("MSM8930")},
	{UTF8STRCPTR("LG-D802"),         "LG",          "G2",                    "LG-D802",      UTF8STRCPTR("MSM8974-AA")},
	{UTF8STRCPTR("LG-D838"),         "LG",          "G Pro 2",               "LG-D838",      UTF8STRCPTR("MSM8974-AA")},
	{UTF8STRCPTR("LG-H860"),         "LG",          "G5",                    "LG-H860",      UTF8STRCPTR("MSM8996")},
	{UTF8STRCPTR("LG-H870DS"),       "LG",          "G6 Dual SIM",           "LG-H870DS",    UTF8STRCPTR("MSM8996")},
	{UTF8STRCPTR("LG-H930"),         "LG",          "V30",                   "LG-H930",      UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("LG-H961N"),        "LG",          "V10",                   "LG-H961N",     UTF8STRCPTR("MSM8992")},
	{UTF8STRCPTR("LG-H990"),         "LG",          "V20",                   "LG-H990",      UTF8STRCPTR("MSM8996")},
	{UTF8STRCPTR("LG-K535"),         "LG",          "Stylus 2 Plus",         "LG-K535",      UTF8STRCPTR("MSM8937")},
	{UTF8STRCPTR("LG-M700"),         "LG",          "Q6",                    "LG-M700",      UTF8STRCPTR("MSM8940")},
	{UTF8STRCPTR("LIO-L29"),         "Huawei",      "Mate 30 Pro",           "LIO-L29",      UTF8STRCPTR("Kirin 990 4G")},
	{UTF8STRCPTR("LM-G710"),         "LG",          "G7 ThinQ",              "LM-G710",      UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("LYA-L09"),         "Huawei",      "Mate 20 Pro",           "LYA-L09",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("LYA-L29"),         "Huawei",      "Mate 20 Pro Dual SIM",  "LYA-L29",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("MAR-LX2"),         "Huawei",      "P30 Lite Dual SIM",     "MAR-LX2",      UTF8STRCPTR("Kirin 710")},
	{UTF8STRCPTR("MHA-L29"),         "Huawei",      "Mate 9",                "MHA-L29",      UTF8STRCPTR("Kirin 960")},
	{UTF8STRCPTR("MI 5"),            "Xiaomi",      "Mi 5",                  "MI5",          UTF8STRCPTR("MSM8996")},
	{UTF8STRCPTR("MI 8"),            "Xiaomi",      "Mi 8",                  "M1803E1A",     UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("MI MAX 2"),        "Xiaomi",      "Mi Max 2",              "MDE40",        UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("MI MAX 3"),        "Xiaomi",      "Mi Max 3",              "M1804E4A",     UTF8STRCPTR("SDM636")},
	{UTF8STRCPTR("MLA-AL10"),        "Huawei",      "nova Plus",             "MLA-AL10",     UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("MStar Android TV"),"MStar",       "Android TV",            "Android TV",   UTF8STRCPTR("")},
	{UTF8STRCPTR("Mi 9T"),           "Xiaomi",      "Mi 9T",                 "M1903F10G",    UTF8STRCPTR("SM7150-AA")},
	{UTF8STRCPTR("Mi A1"),           "Xiaomi",      "Mi A1",                 "MDG2",         UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("Mi A3"),           "Xiaomi",      "Mi A3",                 "M1906F9SH",    UTF8STRCPTR("SM6125")},
	{UTF8STRCPTR("Mi MIX 2"),        "Xiaomi",      "Mi MIX 2",              "MDE5",         UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("MiTV4-ANSM0"),     "Xiaomi",      "Mi TV 4S",              "MiTV4-ANSM0",  UTF8STRCPTR("Amlogic T962")},
	{UTF8STRCPTR("Moto E (4) Plus"), "Moto",        "E4 Plus",               "XT176",        UTF8STRCPTR("MT6737")},
	{UTF8STRCPTR("Nexus 5"),         "LG",          "Nexus 5",               "LG-D821",      UTF8STRCPTR("MSM8974-AA")},
	{UTF8STRCPTR("Nexus 5X"),        "LG",          "Nexus 5X",              "LG-H791",      UTF8STRCPTR("MSM8992")},
	{UTF8STRCPTR("Nokia 7 plus"),    "Nokia",       "7 plus",                "TA-1041",      UTF8STRCPTR("SDM660")},
	{UTF8STRCPTR("Nokia 8.1"),       "Nokia",       "8.1",                   "TA-1119",      UTF8STRCPTR("SDM710")},
	{UTF8STRCPTR("Nokia 9"),         "Nokia",       "9",                     "TA-1094",      UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("ONEPLUS A5000"),   "OnePlus",     "5",                     "A5000",        UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("ONEPLUS A5010"),   "OnePlus",     "5T",                    "A5010",        UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("ONEPLUS A6010"),   "OnePlus",     "6T",                    "A6010",        UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("OPPO R11"),        "OPPO",        "R11",                   "R11",          UTF8STRCPTR("SDM660")},
	{UTF8STRCPTR("OPPO R9s"),        "OPPO"  ,      "R9s",                   "R9s",          UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("PAR-TL20"),        "Huawei",      "nova 3",                "PAR-TL20",     UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("POCOPHONE F1"),    "Xiaomi",      "POCOPHONE F1",          "M1805E10A",    UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("Pixel 2 XL"),      "Google",      "Pixel 2 XL",            "Pixel 2 XL",   UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("R7Plusm"),         "OPPO",        "R7 Plus (Mobile)",      "R7Plusm",      UTF8STRCPTR("MSM8939")},
	{UTF8STRCPTR("RNE-L22"),         "Huawei",      "nova 2i",               "RNE-L22",      UTF8STRCPTR("Kirin 659")},
	{UTF8STRCPTR("RVL-AL09"),        "Huawei",      "Honor Note 10",         "RNE-L22",      UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("Redmi 4A"),        "Xiaomi",      "Redmi 4A",              "Redmi 4A",     UTF8STRCPTR("MSM8917")},
	{UTF8STRCPTR("Redmi 5"),         "Xiaomi",      "Redmi 5",               "MDG1",         UTF8STRCPTR("SDM450")},
	{UTF8STRCPTR("Redmi 5 Plus"),    "Xiaomi",      "Redmi 5 Plus",          "MEG7",         UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("Redmi Note 4"),    "Xiaomi",      "Redmi Note 4",          "Redmi Note 4", UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("Redmi Note 5"),    "Xiaomi",      "Redmi Note 5",          "M1803E7SG",    UTF8STRCPTR("SDM636")},
	{UTF8STRCPTR("Redmi Note 5A"),   "Xiaomi",      "Redmi Note 5A",         "MDI6S",        UTF8STRCPTR("MSM8940")},
	{UTF8STRCPTR("Redmi Note 7"),    "Xiaomi",      "Redmi Note 7",          "M1901F7G",     UTF8STRCPTR("SDM660")},
	{UTF8STRCPTR("Redmi Note 8 Pro"),"Xiaomi",      "Redmi Note 8 Pro",      "M1906G7I",     UTF8STRCPTR("MT6785V")},
	{UTF8STRCPTR("Redmi Note 8T"),   "Xiaomi",      "Redmi Note 8T",         "Redmi Note 8T",UTF8STRCPTR("SM6125")},
	{UTF8STRCPTR("S41"),             "Cat",         "S41",                   "S41",          UTF8STRCPTR("MT6757")},
	{UTF8STRCPTR("SM-A202F"),        "Samsung",     "Galaxy A20e",           "SM-A202F",     UTF8STRCPTR("Exynos7884")},
	{UTF8STRCPTR("SM-A205GN"),       "Samsung",     "Galaxy A20",            "SM-A205GN",    UTF8STRCPTR("Exynos7884")},
	{UTF8STRCPTR("SM-A3050"),        "Samsung",     "Galaxy A40s",           "SM-A3050",     UTF8STRCPTR("Exynos7885")},
	{UTF8STRCPTR("SM-A405FN"),       "Samsung",     "Galaxy A40",            "SM-A405FN",    UTF8STRCPTR("Exynos7904")},
	{UTF8STRCPTR("SM-A520F"),        "Samsung",     "Galaxy A5",             "SM-A520F",     UTF8STRCPTR("Exynos7880")},
	{UTF8STRCPTR("SM-A530F"),        "Samsung",     "Galaxy A8",             "SM-A530F",     UTF8STRCPTR("Exynos7885")},
	{UTF8STRCPTR("SM-A5070"),        "Samsung",     "Galaxy A50s Dual SIM",  "SM-A5070",     UTF8STRCPTR("Exynos9611")},
	{UTF8STRCPTR("SM-A600FN"),       "Samsung",     "Galaxy A6",             "SM-A600FN",    UTF8STRCPTR("Exynos7870")},
	{UTF8STRCPTR("SM-A6060"),        "Samsung",     "Galaxy A60",            "SM-A6060",     UTF8STRCPTR("SM6150")},
	{UTF8STRCPTR("SM-A7050"),        "Samsung",     "Galaxy A70",            "SM-A7050",     UTF8STRCPTR("SM6150")},
	{UTF8STRCPTR("SM-A705FN"),       "Samsung",     "Galaxy A70",            "SM-A705FN",    UTF8STRCPTR("SM6150")},
	{UTF8STRCPTR("SM-A705MN"),       "Samsung",     "Galaxy A70",            "SM-A705MN",    UTF8STRCPTR("SM6150")},
	{UTF8STRCPTR("SM-A730F"),        "Samsung",     "Galaxy A8+",            "SM-A730F",     UTF8STRCPTR("Exynos7885")},
	{UTF8STRCPTR("SM-A8050"),        "Samsung",     "Galaxy A80",            "SM-A8050",     UTF8STRCPTR("SM7150-AB")},
	{UTF8STRCPTR("SM-A9000"),        "Samsung",     "Galaxy A9",             "SM-A9000",     UTF8STRCPTR("MSM8976")},
	{UTF8STRCPTR("SM-A9200"),        "Samsung",     "Galaxy A9s",            "SM-A9200",     UTF8STRCPTR("SDM660")},
	{UTF8STRCPTR("SM-A920F"),        "Samsung",     "Galaxy A9s",            "SM-A920F",     UTF8STRCPTR("SDM660")},
	{UTF8STRCPTR("SM-C5010"),        "Samsung",     "Galaxy C5 Pro",         "SM-C5010",     UTF8STRCPTR("MSM8953Pro")},
	{UTF8STRCPTR("SM-C7000"),        "Samsung",     "Galaxy C7",             "SM-C7000",     UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("SM-C7010"),        "Samsung",     "Galaxy C7 Pro",         "SM-C7010",     UTF8STRCPTR("MSM8953Pro")},
	{UTF8STRCPTR("SM-C7100"),        "Samsung",     "Galaxy C8",             "SM-C7100",     UTF8STRCPTR("MT6757CD")},
	{UTF8STRCPTR("SM-C9000"),        "Samsung",     "Galaxy C9 Pro",         "SM-C9000",     UTF8STRCPTR("MSM8976-PRO")},
	{UTF8STRCPTR("SM-G6100"),        "Samsung",     "Galaxy J7 Prime",       "SM-G6100",     UTF8STRCPTR("MSM8953")},
	{UTF8STRCPTR("SM-G610F"),        "Samsung",     "Galaxy J7 Prime",       "SM-G610F",     UTF8STRCPTR("Exynos7870")},
	{UTF8STRCPTR("SM-G610Y"),        "Samsung",     "Galaxy J7 Prime",       "SM-G610Y",     UTF8STRCPTR("Exynos7870")},
	{UTF8STRCPTR("SM-G920V"),        "Samsung",     "Galaxy S6 (Verizon)",   "SM-G920V",     UTF8STRCPTR("Exynos7420")},
	{UTF8STRCPTR("SM-G930F"),        "Samsung",     "Galaxy S7",             "SM-G930F",     UTF8STRCPTR("Exynos8890")},
	{UTF8STRCPTR("SM-G955F"),        "Samsung",     "Galaxy S8+",            "SM-G955F",     UTF8STRCPTR("Exynos8895")},
	{UTF8STRCPTR("SM-G975U"),        "Samsung",     "Galaxy S10+",           "SM-G975U",     UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("SM-G9287"),        "Samsung",     "Galaxy S6 edge+",       "SM-G9287",     UTF8STRCPTR("Exynos7420")},
	{UTF8STRCPTR("SM-G9350"),        "Samsung",     "Galaxy S7 edge",        "SM-G9350",     UTF8STRCPTR("MSM8996")},
	{UTF8STRCPTR("SM-G9550"),        "Samsung",     "Galaxy S8+",            "SM-G9550",     UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("SM-G9600"),        "Samsung",     "Galaxy S9 Duos",        "SM-G9600",     UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("SM-G9650"),        "Samsung",     "Galaxy S9+",            "SM-G9650",     UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("SM-G9700"),        "Samsung",     "Galaxy S10e",           "SM-G9700",     UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("SM-G9730"),        "Samsung",     "Galaxy S10",            "SM-G9730",     UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("SM-G9750"),        "Samsung",     "Galaxy S10+",           "SM-G9750",     UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("SM-J120M"),        "Samsung",     "Galaxy J1",             "SM-J120M",     UTF8STRCPTR("SC9830")},
	{UTF8STRCPTR("SM-J330FN"),       "Samsung",     "Galaxy J3",             "SM-J330FN",    UTF8STRCPTR("Exynos7570")},
	{UTF8STRCPTR("SM-J415GN"),       "Samsung",     "Galaxy J4+",            "SM-J415GN",    UTF8STRCPTR("MSM8917")},
	{UTF8STRCPTR("SM-J5108"),        "Samsung",     "Galaxy J5 2016",        "SM-J5108",     UTF8STRCPTR("MSM8916")},
	{UTF8STRCPTR("SM-J600G"),        "Samsung",     "Galaxy J6",             "SM-J600G",     UTF8STRCPTR("Exynos7870")},
	{UTF8STRCPTR("SM-J710GN"),       "Samsung",     "Galaxy J7",             "SM-J710GN",    UTF8STRCPTR("Exynos7870")},
	{UTF8STRCPTR("SM-J730GM"),       "Samsung",     "Galaxy J7 Pro Duos",    "SM-J730GM",    UTF8STRCPTR("Exynos7870")},
	{UTF8STRCPTR("SM-N9005"),        "Samsung",     "Galaxy Note 3",         "SM-N9005",     UTF8STRCPTR("MSM8974-AA")},
	{UTF8STRCPTR("SM-N910U"),        "Samsung",     "Galaxy Note 4",         "SM-N910U",     UTF8STRCPTR("Exynos5433")},
	{UTF8STRCPTR("SM-N9200"),        "Samsung",     "Galaxy Note 5",         "SM-N9200",     UTF8STRCPTR("Exynos7420")},
	{UTF8STRCPTR("SM-N9500"),        "Samsung",     "Galaxy Note8",          "SM-N9500",     UTF8STRCPTR("MSM8998")},
	{UTF8STRCPTR("SM-N9600"),        "Samsung",     "Galaxy Note9",          "SM-N9600",     UTF8STRCPTR("SDM845")},
	{UTF8STRCPTR("SM-N960F"),        "Samsung",     "Galaxy Note9",          "SM-N960F",     UTF8STRCPTR("Exynos9810")},
	{UTF8STRCPTR("SM-N9700"),        "Samsung",     "Galaxy Note10",         "SM-N9700",     UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("SM-N9750"),        "Samsung",     "Galaxy Note10+",        "SM-N9750",     UTF8STRCPTR("SM8150")},
	{UTF8STRCPTR("SM-N975F"),        "Samsung",     "Galaxy Note10+",        "SM-N975F",     UTF8STRCPTR("Exynos9825")},
	{UTF8STRCPTR("SM-P205"),         "Samsung",     "Galaxy Tab A 8.0",      "SM-P205",      UTF8STRCPTR("Exynos7904")},
	{UTF8STRCPTR("SM-T580"),         "Samsung",     "Galaxy Tab A 10.1 WiFi","SM-T580",      UTF8STRCPTR("Exynos7870")},
	{UTF8STRCPTR("SM-T813"),         "Samsung",     "Galaxy Tab S2 9.7 WiFi","SM-T813",      UTF8STRCPTR("MSM8976")},
	{UTF8STRCPTR("SM801"),           "Smartisan",   "T2",                    "SM801",        UTF8STRCPTR("MSM8992")},
	{UTF8STRCPTR("STF-AL00"),        "Huawei",      "Honor 9",               "STF-AL00",     UTF8STRCPTR("Kirin 960")},
	{UTF8STRCPTR("STK-L22"),         "Huawei",      "Y9 Prime",              "STK-L22",      UTF8STRCPTR("Kirin 710F")},
	{UTF8STRCPTR("STK-LX3"),         "Huawei",      "Y9 Prime",              "STK-LX3",      UTF8STRCPTR("Kirin 710F")},
	{UTF8STRCPTR("SUGAR Y12s"),      "SUGAR",       "Y12s",                  "SUGAR-Y12s",   UTF8STRCPTR("MT6739WW")},
	{UTF8STRCPTR("TA-1003"),         "Nokia",       "Nokia 6 Dual SIM",      "TA-1003",      UTF8STRCPTR("MSM8937")},
	{UTF8STRCPTR("TRT-TL10"),        "Huawei",      "Y7 Prime",              "TRT-TL10",     UTF8STRCPTR("MSM8940")},
	{UTF8STRCPTR("VCE-L22"),         "Huawei",      "nova 4",                "VCE-L22",      UTF8STRCPTR("Kirin 970")},
	{UTF8STRCPTR("VIE-L29"),         "Huawei",      "P9 Plus Dual SIM",      "VIE-L29",      UTF8STRCPTR("Kirin 955")},
	{UTF8STRCPTR("VKY-L29"),         "Huawei",      "P10 Plus Dual SIM",     "VKY-L29",      UTF8STRCPTR("Kirin 960")},
	{UTF8STRCPTR("VNS-L31"),         "Huawei",      "P9 lite",               "VNS-L31",      UTF8STRCPTR("Kirin 650")},
	{UTF8STRCPTR("VOG-L29"),         "Huawei",      "P30 Pro Dual SIM",      "VOG-L29",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("WAS-TL10"),        "Huawei",      "P10 Lite",              "WAS-TL10",     UTF8STRCPTR("Kirin 658")},
	{UTF8STRCPTR("YAL-L21"),         "Huawei",      "Honor 20 Dual SIM",     "YAL-L21",      UTF8STRCPTR("Kirin 980")},
	{UTF8STRCPTR("ZTE BLADE A512"),  "ZTE",         "Blade A512",            "A512",         UTF8STRCPTR("MSM8917")},
	{UTF8STRCPTR("vivo 1801"),       "vivo",        "Y71i",                  "1801",         UTF8STRCPTR("MSM8917")},
	{UTF8STRCPTR("vivo 1807"),       "vivo",        "Y95",                   "1807",         UTF8STRCPTR("SDM439")},
	{UTF8STRCPTR("vivo 1811"),       "vivo",        "Y91",                   "1811",         UTF8STRCPTR("SDM439")},
	{UTF8STRCPTR("vivo 1901"),       "vivo",        "Y17",                   "1901",         UTF8STRCPTR("MT6765")},
	{UTF8STRCPTR("vivo 1904"),       "vivo",        "Y12",                   "1904",         UTF8STRCPTR("MT6762")},
	{UTF8STRCPTR("vivo 1909"),       "vivo",        "V17 Pro",               "1909",         UTF8STRCPTR("SM6150")},
};

const IO::AndroidDB::AndroidInfo *IO::AndroidDB::GetAndroidInfo(Text::CStringNN androidId)
{
	OSInt i = 0;
	OSInt j = (sizeof(androids) / sizeof(androids[0])) - 1;
	OSInt k;
	OSInt l;

	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareFastC(androidId.v, androidId.leng, androids[k].androidId, androids[k].androidIdLen);
		if (l > 0)
		{
			i = k + 1;
		}
		else if (l < 0)
		{
			j = k - 1;
		}
		else
		{
			return &androids[k];
		}
	}
	return 0;
}
