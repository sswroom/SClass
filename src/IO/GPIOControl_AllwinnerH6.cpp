#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

// Allwinner H6
#define GPIO_BASE_MAP  (0x0300B000)
#define GPIOL_BASE_MAP  (0x07022000)

typedef struct
{
	IO::PhysicalMem *mem;
	IO::PhysicalMem *memL;
	volatile UInt32 *gpioPtr;
	volatile UInt32 *gpioLPtr;
} ClassData;

void GPIOControl_GetPortNum(UInt16 pinNum, OSInt *portNum, OSInt *index)
{
	if (pinNum < 17)
	{
		*portNum = 2;
		*index = pinNum;
	}
	else if (pinNum < 44)
	{
		*portNum = 3;
		*index = pinNum - 17;
	}
	else if (pinNum < 51)
	{
		*portNum = 5;
		*index = pinNum - 44;
	}
	else if (pinNum < 66)
	{
		*portNum = 6;
		*index = pinNum - 51;
	}
	else if (pinNum < 77)
	{
		*portNum = 7;
		*index = pinNum - 66;
	}
	else if (pinNum < 88)
	{
		*portNum = 8;
		*index = pinNum - 77;
	}
	else if (pinNum < 93)
	{
		*portNum = 9;
		*index = pinNum - 88;
	}
}

IO::GPIOControl::GPIOControl()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->mem, IO::PhysicalMem(GPIO_BASE_MAP, 0x4000));
	clsData->gpioPtr = (volatile UInt32 *)clsData->mem->GetPointer();
	NEW_CLASS(clsData->memL, IO::PhysicalMem(GPIOL_BASE_MAP, 0x4000));
	clsData->gpioLPtr = (volatile UInt32 *)clsData->memL->GetPointer();
	this->clsData = clsData;
}

IO::GPIOControl::~GPIOControl()
{
	ClassData *clsData = (ClassData*)this->clsData;
	DEL_CLASS(clsData->mem);
	DEL_CLASS(clsData->memL);
	MemFree(clsData);
}

Bool IO::GPIOControl::IsError()
{
	ClassData *clsData = (ClassData*)this->clsData;
	return clsData->mem->IsError() || clsData->memL->IsError();
}

UOSInt IO::GPIOControl::GetPinCount()
{
	return 93;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 93)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	if (portNum >= 8)
	{
		return ((clsData->gpioLPtr[(portNum & 7) * 9 + 4] >> index) & 1) != 0;
	}
	else
	{
		return ((clsData->gpioPtr[portNum * 9 + 4] >> index) & 1) != 0;
	}
}

Bool IO::GPIOControl::IsPinOutput(UOSInt pinNum)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 93)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	if (portNum >= 8)
	{
		return ((clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7) == 1;
	}
	else
	{
		return ((clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7) == 1;
	}
}

UOSInt IO::GPIOControl::GetPinMode(UOSInt pinNum)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 93)
	{
		return 0;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	if (portNum >= 8)
	{
		return (clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7;
	}
	else
	{
		return (clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7;
	}
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 93)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 mask = 7 << ((index & 7) * 4);
	UInt32 v = (isOutput?1:0) << ((index & 7) * 4);
	if (portNum >= 8)
	{
		clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] = (clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] & ~mask) | v;
	}
	else
	{
		clsData->gpioPtr[portNum * 9 + (index >> 3)] = (clsData->gpioPtr[portNum * 9 + (index >> 3)] & ~mask) | v;
	}
	return true;
}

Bool IO::GPIOControl::SetPinState(UOSInt pinNum, Bool isHigh)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 93)
	{
		return false;
	}
	OSInt portNum;
	OSInt index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 v = isHigh?(1 << index):0;
	if (portNum >= 8)
	{
		clsData->gpioLPtr[(portNum & 7) * 9 + 4] = (clsData->gpioLPtr[(portNum & 7) * 9 + 4] & ~(1 << index)) | v;
	}
	else
	{
		clsData->gpioPtr[portNum * 9 + 4] = (clsData->gpioPtr[portNum * 9 + 4] & ~(1 << index)) | v;
	}
	return true;
}

Bool IO::GPIOControl::SetPullType(UOSInt pinNum, IO::IOPin::PullType pt)
{
	ClassData *clsData = (ClassData*)this->clsData;
	if (pinNum >= 93)
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
	if (portNum >= 8)
	{
		clsData->gpioLPtr[(portNum & 7) * 9 + 7 + (index >> 4)] = (clsData->gpioLPtr[(portNum & 7) * 9 + 7 + (index >> 4)] & ~(3 << ((index & 15) * 2))) | v;
	}
	else
	{
		clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] = (clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] & ~(3 << ((index & 15) * 2))) | v;
	}
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
	//PortC
	"NAND_WE",
	"NAND_ALE",
	"NAND_CLE",
	"NAND_CE0",
	"NAND_RE",
	"NAND_RB0",
	"NAND_DQ0",
	"NAND_DQ1",
	"NAND_DQ2",
	"NAND_DQ3",

	"NAND_DQ4",
	"NAND_DQ5",
	"NAND_DQ6",
	"NAND_DQ7",
	"NAND_DQS",
	"NAND_CE1",
	"NAND_RB1",

	//PortD
	"LCD0_D2",
	"LCD0_D3",
	"LCD0_D4",
	"LCD0_D5",
	"LCD0_D6",
	"LCD0_D7",
	"LCD0_D10",
	"LCD0_D11",
	"LCD0_D12",
	"LCD0_D13",

	"LCD0_D14",
	"LCD0_D15",
	"LCD0_D18",
	"LCD0_D19",
	"LCD0_D20",
	"LCD0_D21",
	"LCD0_D22",
	"LCD0_D23",
	"LCD0_CLK",
	"LCD0_DE",

	"LCD0_HSYNC",
	"LCD0_VSYNC",
	"PWM0",
	"TWI2_SCK",
	"TWI2_SDA",
	"TWI0_SCK",
	"TWI0_SDA",

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

	"PCM2_SYNC",
	"PCM2_CLK",
	"PCM2_DOUT",
	"PCM2_DIN",
	"PCM2_MCLK",

	//PortH
	"UART0_TX",
	"UART0_RX",
	"CIR_TX",
	"SPI1_CS",
	"SPI1_CLK",
	"SPI1_MOSI",
	"SPI1_MISO",
	"Reserved",
	"HSCL",
	"HSDA",

	"HCEC",

	//PortL
	"Reserved",
	"Reserved",
	"S_UART_TX",
	"S_UART_RX",
	"S_JTAG_MS",
	"S_JTAG_CK",
	"S_JTAG_DO",
	"S_JTAG_DI",
	"S_PWM0",
	"S_CIR_RX",

	"S_OWC",

	//PortM
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

static const Char *GPIOControl_Func3[] = {
	//PortC
	"Reserved",
	"SDC2_DS",
	"Reserved",
	"Reserved",
	"SDC2_CLK",
	"SDC2_CMD",
	"SDC2_D0",
	"SDC2_D1",
	"SDC2_D2",
	"SDC2_D3",

	"SDC2_D4",
	"SDC2_D5",
	"SDC2_D6",
	"SDC2_D7",
	"SDC2_RST",
	"Reserved",
	"Reserved",

	//PortD
	"TS0_CLK",
	"TS0_ERR",
	"TS0_SYNC",
	"TS0_DVLD",
	"TS0_D0",
	"TS0_D1",
	"TS0_D2",
	"TS0_D3",
	"TS0_D4",
	"TS0_D5",

	"TS0_D6",
	"TS0_D7",
	"TS1_CLK",
	"TS1_ERR",
	"TS1_SYNC",
	"TS1_DVLD",
	"TS1_D0",
	"TS2_CLK",
	"TS2_ERR",
	"TS2_SYNC",

	"TS2_DVLD",
	"TS2_D0",
	"TS3_CLK",
	"TS3_ERR",
	"TS3_SYNC",
	"TS3_DVLD",
	"TS3_D0",

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

	"H_PCM2_SYNC",
	"H_PCM2_CLK",
	"H_PCM2_DOUT",
	"H_PCM2_DIN",
	"H_PCM2_MCLK",

	//PortH
	"PCM0_SYNC",
	"PCM0_CLK",
	"PCM0_DOUT",
	"PCM0_DIN",
	"PCM0_MCLK",
	"OWA_MCLK",
	"OWA_IN",
	"OWA_OUT",
	"Reserved",
	"Reserved",

	"Reserved",

	//PortL
	"S_TWI_SCK",
	"S_TWI_SDA",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"S_PWM1",

	//PortM
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

static const Char *GPIOControl_Func4[] = {
	//PortC
	"SPI0_CLK",
	"Reserved",
	"SPI0_MOSI",
	"SPI0_MISO",
	"Reserved",
	"SPI0_CS",
	"SPI0_HOLD",
	"SPI0_WP",
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
	"DMIC_CLK",
	"DMIC_DATA0",
	"DMIC_DATA1",
	"DMIC_DATA2",
	"DMIC_DATA3",
	"UART2_TX",

	"UART2_RX",
	"UART2_RTS",
	"UART2_CTS",
	"UART3_TX",
	"UART3_RX",
	"UART3_RTS",
	"UART3_CTS",

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
	"SIM0_VPPEN",
	"SIM0_VPPPP",

	"SIM0_PWREN",
	"SIM0_CLK",
	"SIM0_DATA",
	"SIM0_RST",
	"SIM0_DET",

	//PortH
	"H_PCM0_SYNC",
	"H_PCM0_CLK",
	"H_PCM0_DOUT",
	"H_PCM0_DIN",
	"H_PCM0_MCLK",
	"TWI1_SCK",
	"TWI1_SDA",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",

	//PortL
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

	//PortM
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

static const Char *GPIOControl_Func5[] = {
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
	"RGMII_RXD3/RMII_NULL",
	"RGMII_RXD2/RMII_NULL",
	"RGMII_RXD1/RMII_RXD1",
	"RGMII_RXD0/RMII_RXD0",
	"RGMII_RXCK/RMII_NULL",
	"RGMII_RXCTL/RMII_CRS_DV",
	"RGMII_NULL/RMII_RXER",
	"RGMII_TXD3/RMII_NULL",
	"RGMII_TXD2/RMII_NULL",
	"RGMII_TXD1/RMII_TXD1",

	"RGMII_TXD0/RMII_TXD0",
	"RGMII_TXCK/RMII_TXCK",
	"RGMII_TXCTL/RMII_TXEN",
	"RGMII_CLKIN/RMII_NULL",
	"CSI_D8",
	"CSI_D9",
	"Reserved",
	"Reserved",
	"Reserved",
	"MDC",

	"MDIO",
	"Reserved",
	"Reserved",
	"JTAG_MS",
	"JTAG_CK",
	"JTAG_DO",
	"JTAG_DI",

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
	"Reserved",

	//PortH
	"SIM1_VPPEN",
	"SIM1_VPPPP",
	"SIM1_PWREN",
	"SIM1_CLK",
	"SIM1_DATA",
	"SIM1_RST",
	"SIM1_DET",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",

	//PortL
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

	//PortM
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
};

static const Char *GPIOControl_Func6[] = {
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
	"Reserved",
	"Reserved",

	//PortF
	"PF_EINT0",
	"PF_EINT1",
	"PF_EINT2",
	"PF_EINT3",
	"PF_EINT4",
	"PF_EINT5",
	"PF_EINT6",

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
	"PG_EINT14",

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

	//PortL
	"S_PL_EINT0",
	"S_PL_EINT1",
	"S_PL_EINT2",
	"S_PL_EINT3",
	"S_PL_EINT4",
	"S_PL_EINT5",
	"S_PL_EINT6",
	"S_PL_EINT7",
	"S_PL_EINT8",
	"S_PL_EINT9",

	"S_PL_EINT10",

	//PortM
	"S_PM_EINT0",
	"S_PM_EINT1",
	"S_PM_EINT2",
	"S_PM_EINT3",
	"S_PM_EINT4",
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
	else if (pinNum >= 93)
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
