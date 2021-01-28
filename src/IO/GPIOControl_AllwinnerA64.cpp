#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

// Allwinner A64
#define GPIO_BASE_MAP  (0x01C20800)

typedef struct
{
	IO::PhysicalMem *mem;
	volatile UInt32 *gpioPtr;
} ClassData;

void GPIOControl_GetPortNum(UInt16 pinNum, OSInt *portNum, OSInt *index)
{
	if (pinNum < 10)
	{
		*portNum = 1;
		*index = pinNum;
	}
	else if (pinNum < 27)
	{
		*portNum = 2;
		*index = pinNum - 10;
	}
	else if (pinNum < 52)
	{
		*portNum = 3;
		*index = pinNum - 27;
	}
	else if (pinNum < 70)
	{
		*portNum = 4;
		*index = pinNum - 52;
	}
	else if (pinNum < 77)
	{
		*portNum = 5;
		*index = pinNum - 70;
	}
	else if (pinNum < 91)
	{
		*portNum = 6;
		*index = pinNum - 77;
	}
	else if (pinNum < 103)
	{
		*portNum = 7;
		*index = pinNum - 91;
	}
}

IO::GPIOControl::GPIOControl()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->mem, IO::PhysicalMem(GPIO_BASE_MAP, 0x4000));
	clsData->gpioPtr = (volatile UInt32 *)clsData->mem->GetPointer();
	this->clsData = clsData;
}

IO::GPIOControl::~GPIOControl()
{
	ClassData *clsData = (ClassData*)this->clsData;
	DEL_CLASS(clsData->mem);
	MemFree(clsData);
}

Bool IO::GPIOControl::IsError()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->mem->IsError();
}

UOSInt IO::GPIOControl::GetPinCount()
{
	return 103;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 103)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	return ((clsData->gpioPtr[portNum * 9 + 4] >> index) & 1) != 0;
}

Bool IO::GPIOControl::IsPinOutput(UOSInt pinNum)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 103)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	return ((clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7) == 1;
}

UOSInt IO::GPIOControl::GetPinMode(UOSInt pinNum)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 103)
	{
		return 0;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	return (clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7;
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 103)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 mask = 7 << ((index & 7) * 4);
	UInt32 v = (isOutput?1:0) << ((index & 7) * 4);
	clsData->gpioPtr[portNum * 9 + (index >> 3)] = (clsData->gpioPtr[portNum * 9 + (index >> 3)] & ~mask) | v;
	return true;
}

Bool IO::GPIOControl::SetPinState(UOSInt pinNum, Bool isHigh)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 103)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 v = isHigh?(1 << index):0;
	clsData->gpioPtr[portNum * 9 + 4] = (clsData->gpioPtr[portNum * 9 + 4] & ~(1 << index)) | v;
	return true;
}

Bool IO::GPIOControl::SetPullType(UOSInt pinNum, IO::IOPin::PullType pt)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 103)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 v;
	if (pt == IO::IOPin::PT_DISABLE)
	{
		v = 0;
	}
	else if (pt == IO::IOPin::PT_UP)
	{
		v = 1 << ((index & 15) * 2);
	}
	else if (pt == IO::IOPin::PT_DOWN)
	{
		v = 2 << ((index & 15) * 2);
	}
	clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] = (clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] & ~(3 << ((index & 15) * 2))) | v;
	return true;
}

void IO::GPIOControl::SetEventOnHigh(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnLow(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnRaise(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnFall(UOSInt pinNum, Bool enable)
{
}

Bool IO::GPIOControl::HasEvent(UOSInt pinNum)
{
	return false;
}

void IO::GPIOControl::ClearEvent(UOSInt pinNum)
{
}

static const Char *GPIOControl_Func2[] = {
	//PortB
	"UART2_TX",
	"UART2_RX",
	"UART2_RTS",
	"UART2_CTS",
	"AIF2_SYNC",
	"AIF2_BCLK",
	"AIF2_DOUT",
	"AIF2_DIN",
	"Reserved",
	"Reserved",

	//PortC
	"NAND_WE",
	"NAND_ALE",
	"NAND_CLE",
	"NAND_CE1",
	"NAND_CE0",
	"NAND_RE",
	"NAND_RB0",
	"NAND_RB1",
	"NAND_DQ0",
	"NAND_DQ1",

	"NAND_DQ2",
	"NAND_DQ3",
	"NAND_DQ4",
	"NAND_DQ5",
	"NAND_DQ6",
	"NAND_DQ7",
	"NAND_DQS",

	//PortD
	"LCD_D2",
	"LCD_D3",
	"LCD_D4",
	"LCD_D5",
	"LCD_D6",
	"LCD_D7",
	"LCD_D10",
	"LCD_D11",
	"LCD_D12",
	"LCD_D13",

	"LCD_D14",
	"LCD_D15",
	"LCD_D18",
	"LCD_D19",
	"LCD_D20",
	"LCD_D21",
	"LCD_D22",
	"LCD_D23",
	"LCD_CLK",
	"LCD_DE",

	"LCD_HSYNC",
	"LCD_VSYNC",
	"PWM0",
	"Reserved",
	"Reserved",

	//PortE
	"CSI_PCLK",
	"CSI_MCLK",
	"CSI_HSYNC",
	"CSI_VSYNC",
	"CSI_D0",
	"CSI_D1",
	"CSI_D2",
	"CSI_D3",
	"CSI_D4",
	"CSI_D5",

	"CSI_D6",
	"CSI_D7",
	"CSI_SCK",
	"CSI_SDA",
	"PLL_LOCK_DBG",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortF
	"SDC0_D1",
	"SDC0_D0",
	"SDC0_CLK",
	"SDC0_CMD",
	"SDC0_D3",
	"SDC0_D2",
	"Reserved",

	//PortG
	"SDC1_CLK",
	"SDC1_CMD",
	"SDC1_D0",
	"SDC1_D1",
	"SDC1_D2",
	"SDC1_D3",
	"UART1_TX",
	"UART1_RX",
	"UART1_RTS",
	"UART1_CTS",

	"AIF3_SYNC",
	"AIF3_BCLK",
	"AIF3_DOUT",
	"AIF3_DIN",

	//PortH
	"TWI0_SCK",
	"TWI0_SDA",
	"TWI1_SCK",
	"TWI1_SDA",
	"UART3_TX",
	"UART3_RX",
	"UART3_RTS",
	"UART3_CTS",
	"OWA_OUT",
	"Reserved",

	"MIC_CLK",
	"MIC_DATA",
};

static const Char *GPIOControl_Func3[] = {
	//PortB
	"Reserved",
	"Reserved",
	"Reserved",
	"I2S0_MCLK",
	"PCM0_SYNC",
	"PCM0_BCLK",
	"PCM0_DOUT",
	"PCM0_DIN",
	"Reserved",
	"Reserved",

	//PortC
	"Reserved",
	"SDC2_DS",
	"Reserved",
	"Reserved",
	"Reserved",
	"SDC2_CLK",
	"SDC2_CMD",
	"Reserved",
	"SDC2_D0",
	"SDC2_D1",

	"SDC2_D2",
	"SDC2_D3",
	"SDC2_D4",
	"SDC2_D5",
	"SDC2_D6",
	"SDC2_D7",
	"SDC2_RST",

	//PortD
	"UART3_TX",
	"UART3_RX",
	"UART4_TX",
	"UART4_RX",
	"UART4_RTS",
	"UART4_CTS",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"LVDS_VP0",
	"LVDS_VN0",
	"LVDS_VP1",
	"LVDS_VN1",
	"LVDS_VP2",
	"LVDS_VN2",
	"LVDS_VPC",
	"LVDS_VNC",

	"LVDS_VP3",
	"LVDS_VN3",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortE
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"TWI2_SCK",
	"TWI2_SDA",
	"Reserved",
	"Reserved",

	//PortF
	"JTAG_MS1",
	"JTAG_DI1",
	"UART0_TX",
	"JTAG_DO1",
	"UART0_RX",
	"JTAG_CK1",
	"Reserved",

	//PortG
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"PCM1_SYNC",
	"PCM1_BCLK",
	"PCM1_DOUT",
	"PCM1_DIN",

	//PortH
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
};

static const Char *GPIOControl_Func4[] = {
	//PortB
	"JTAG_MS0",
	"JTAG_CK0",
	"JTAG_DO0",
	"JTAG_DI0",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"UART0_TX",
	"UART0_RX",

	//PortC
	"SPI0_MOSI",
	"SPI0_MISO",
	"SPI0_CLK",
	"SPI0_CS",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortD
	"SPI1_CS",
	"SPI1_CLK",
	"SPI1_MOSI",
	"SPI1_MISO",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"RGMII_RXD3/MII_RXD3/RMII_NULL",
	"RGMII_RXD2/MII_RXD2/RMII_NULL",

	"RGMII_RXD1/MII_RXD1/RMII_RXD1",
	"RGMII_RXD0/MII_RXD0/RMII_RXD0",
	"RGMII_RXCK/MII_RXCK/RMII_NULL",
	"RGMII_RXCTL/MII_RXDV/RMII_CRS_DV",
	"RGMII_NULL/MII_RXERR/RMII_RXER",
	"RGMII_TXD3/MII_TXD3/RMII_NULL",
	"RGMII_TXD2/MII_TXD2/RMII_NULL",
	"RGMII_TXD1/MII_TXD1/RMII_TXD1",
	"RGMII_TXD0/MII_TXD0/RMII_TXD0",
	"RGMII_TXCK/MII_TXCK/RMII_TXCK",

	"RGMII_TXCTL/MII_TXEN/RMII_TXEN",
	"RGMII_CLKIN/MII_COL/RMII_NULL",
	"MDC",
	"MDIO",
	"Reserved",

	//PortE
	"TS_CLK",
	"TS_ERR",
	"TS_SYNC",
	"TS_DVLD",
	"TS_D0",
	"TS_D1",
	"TS_D2",
	"TS_D3",
	"TS_D4",
	"TS_D5",

	"TS_D6",
	"TS_D7",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortF
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortG
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortH
	"w_dbgin",
	"w_dbgack",
	"w_jtclk",
	"w_jtdi",
	"w_jtdo",
	"dbg_uart_txd",
	"w_jtms",
	"w_jtrst_n",
	"jtrstn_k",
	"jtclk_k",

	"jtms_k",
	"jtdi_k",
};

static const Char *GPIOControl_Func5[] = {
	//PortB
	"Reserved",
	"SIM_PWREN",
	"SIM_VPPEN",
	"SIM_VPPPP",
	"SIM_CLK",
	"SIM_DATA",
	"SIM_RST",
	"SIM_DET",
	"Reserved",
	"Reserved",

	//PortC
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortD
	"CCIR_CLK",
	"CCIR_DE",
	"CCIR_HSYNC",
	"CCIR_VSYNC",
	"CCIR_D0",
	"CCIR_D1",
	"CCIR_D2",
	"CCIR_D3",
	"CCIR_D4",
	"CCIR_D5",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"CCIR_D6",
	"CCIR_D7",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortE
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortF
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortG
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortH
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
};

static const Char *GPIOControl_Func6[] = {
	//PortB
	"PB_EINT0",
	"PB_EINT1",
	"PB_EINT2",
	"PB_EINT3",
	"PB_EINT4",
	"PB_EINT5",
	"PB_EINT6",
	"PB_EINT7",
	"PB_EINT8",
	"PB_EINT9",

	//PortC
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortD
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortE
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortF
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	//PortG
	"PG_EINT0",
	"PG_EINT1",
	"PG_EINT2",
	"PG_EINT3",
	"PG_EINT4",
	"PG_EINT5",
	"PG_EINT6",
	"PG_EINT7",
	"PG_EINT8",
	"PG_EINT9",

	"PG_EINT10",
	"PG_EINT11",
	"PG_EINT12",
	"PG_EINT13",

	//PortH
	"PH_EINT0",
	"PH_EINT1",
	"PH_EINT2",
	"PH_EINT3",
	"PH_EINT4",
	"PH_EINT5",
	"PH_EINT6",
	"PH_EINT7",
	"PH_EINT8",
	"PH_EINT9",

	"PH_EINT10",
	"PH_EINT11",
};

const UTF8Char *IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinMode)
{
	if (pinMode == 0)
	{
		return (const UTF8Char*)"Input";
	}
	else if (pinMode == 1)
	{
		return (const UTF8Char*)"Output";
	}
	else if (pinNum >= 103)
	{
		return (const UTF8Char*)"Unknown";
	}
	if (pinMode == 2)
	{
		return (const UTF8Char*)GPIOControl_Func2[pinNum];	
	}
	else if (pinMode == 3)
	{
		return (const UTF8Char*)GPIOControl_Func3[pinNum];	
	}
	else if (pinMode == 4)
	{
		return (const UTF8Char*)GPIOControl_Func4[pinNum];	
	}
	else if (pinMode == 5)
	{
		return (const UTF8Char*)GPIOControl_Func5[pinNum];	
	}
	else if (pinMode == 6)
	{
		return (const UTF8Char*)GPIOControl_Func6[pinNum];	
	}
	else if (pinMode == 7)
	{
		return (const UTF8Char*)"IO Disable";	
	}
	return (const UTF8Char*)"Unknown";
}
