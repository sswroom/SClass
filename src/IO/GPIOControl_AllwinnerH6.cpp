#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

// Allwinner H6
#define GPIO_BASE_MAP  (0x0300B000)
#define GPIOL_BASE_MAP  (0x07022000)

struct IO::GPIOControl::ClassData
{
	IO::PhysicalMem *mem;
	IO::PhysicalMem *memL;
	volatile UInt32 *gpioPtr;
	volatile UInt32 *gpioLPtr;
};

void GPIOControl_GetPortNum(UInt16 pinNum, IntOS *portNum, IntOS *index)
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
	DEL_CLASS(this->clsData->mem);
	DEL_CLASS(this->clsData->memL);
	MemFree(this->clsData);
}

Bool IO::GPIOControl::IsError()
{
	return this->clsData->mem->IsError() || this->clsData->memL->IsError();
}

UIntOS IO::GPIOControl::GetPinCount()
{
	return 93;
}

Bool IO::GPIOControl::IsPinHigh(UIntOS pinNum)
{
	if (pinNum >= 93)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	if (portNum >= 8)
	{
		return ((this->clsData->gpioLPtr[(portNum & 7) * 9 + 4] >> index) & 1) != 0;
	}
	else
	{
		return ((this->clsData->gpioPtr[portNum * 9 + 4] >> index) & 1) != 0;
	}
}

Bool IO::GPIOControl::IsPinOutput(UIntOS pinNum)
{
	if (pinNum >= 93)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	if (portNum >= 8)
	{
		return ((this->clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7) == 1;
	}
	else
	{
		return ((this->clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7) == 1;
	}
}

UIntOS IO::GPIOControl::GetPinMode(UIntOS pinNum)
{
	if (pinNum >= 93)
	{
		return 0;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	if (portNum >= 8)
	{
		return (this->clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7;
	}
	else
	{
		return (this->clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7;
	}
}

Bool IO::GPIOControl::SetPinOutput(UIntOS pinNum, Bool isOutput)
{
	if (pinNum >= 93)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 mask = 7 << ((index & 7) * 4);
	UInt32 v = (isOutput?1:0) << ((index & 7) * 4);
	if (portNum >= 8)
	{
		this->clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] = (this->clsData->gpioLPtr[(portNum & 7) * 9 + (index >> 3)] & ~mask) | v;
	}
	else
	{
		this->clsData->gpioPtr[portNum * 9 + (index >> 3)] = (this->clsData->gpioPtr[portNum * 9 + (index >> 3)] & ~mask) | v;
	}
	return true;
}

Bool IO::GPIOControl::SetPinState(UIntOS pinNum, Bool isHigh)
{
	if (pinNum >= 93)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 v = isHigh?(1 << index):0;
	if (portNum >= 8)
	{
		this->clsData->gpioLPtr[(portNum & 7) * 9 + 4] = (this->clsData->gpioLPtr[(portNum & 7) * 9 + 4] & ~(1 << index)) | v;
	}
	else
	{
		this->clsData->gpioPtr[portNum * 9 + 4] = (this->clsData->gpioPtr[portNum * 9 + 4] & ~(1 << index)) | v;
	}
	return true;
}

Bool IO::GPIOControl::SetPullType(UIntOS pinNum, IO::IOPin::PullType pt)
{
	if (pinNum >= 93)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
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
		this->clsData->gpioLPtr[(portNum & 7) * 9 + 7 + (index >> 4)] = (this->clsData->gpioLPtr[(portNum & 7) * 9 + 7 + (index >> 4)] & ~(3 << ((index & 15) * 2))) | v;
	}
	else
	{
		this->clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] = (this->clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] & ~(3 << ((index & 15) * 2))) | v;
	}
	return true;
}

void IO::GPIOControl::SetEventOnHigh(UIntOS pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnLow(UIntOS pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnRaise(UIntOS pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnFall(UIntOS pinNum, Bool enable)
{
}

Bool IO::GPIOControl::HasEvent(UIntOS pinNum)
{
	return false;
}

void IO::GPIOControl::ClearEvent(UIntOS pinNum)
{
}

static Text::CString GPIOControl_Func2[] = {
	//PortC
	CSTR("NAND_WE"),
	CSTR("NAND_ALE"),
	CSTR("NAND_CLE"),
	CSTR("NAND_CE0"),
	CSTR("NAND_RE"),
	CSTR("NAND_RB0"),
	CSTR("NAND_DQ0"),
	CSTR("NAND_DQ1"),
	CSTR("NAND_DQ2"),
	CSTR("NAND_DQ3"),

	CSTR("NAND_DQ4"),
	CSTR("NAND_DQ5"),
	CSTR("NAND_DQ6"),
	CSTR("NAND_DQ7"),
	CSTR("NAND_DQS"),
	CSTR("NAND_CE1"),
	CSTR("NAND_RB1"),

	//PortD
	CSTR("LCD0_D2"),
	CSTR("LCD0_D3"),
	CSTR("LCD0_D4"),
	CSTR("LCD0_D5"),
	CSTR("LCD0_D6"),
	CSTR("LCD0_D7"),
	CSTR("LCD0_D10"),
	CSTR("LCD0_D11"),
	CSTR("LCD0_D12"),
	CSTR("LCD0_D13"),

	CSTR("LCD0_D14"),
	CSTR("LCD0_D15"),
	CSTR("LCD0_D18"),
	CSTR("LCD0_D19"),
	CSTR("LCD0_D20"),
	CSTR("LCD0_D21"),
	CSTR("LCD0_D22"),
	CSTR("LCD0_D23"),
	CSTR("LCD0_CLK"),
	CSTR("LCD0_DE"),

	CSTR("LCD0_HSYNC"),
	CSTR("LCD0_VSYNC"),
	CSTR("PWM0"),
	CSTR("TWI2_SCK"),
	CSTR("TWI2_SDA"),
	CSTR("TWI0_SCK"),
	CSTR("TWI0_SDA"),

	//PortF
	CSTR("SDC0_D1"),
	CSTR("SDC0_D0"),
	CSTR("SDC0_CLK"),
	CSTR("SDC0_CMD"),
	CSTR("SDC0_D3"),
	CSTR("SDC0_D2"),
	CSTR("Reserved"),

	//PortG
	CSTR("SDC1_CLK"),
	CSTR("SDC1_CMD"),
	CSTR("SDC1_D0"),
	CSTR("SDC1_D1"),
	CSTR("SDC1_D2"),
	CSTR("SDC1_D3"),
	CSTR("UART1_TX"),
	CSTR("UART1_RX"),
	CSTR("UART1_RTS"),
	CSTR("UART1_CTS"),

	CSTR("PCM2_SYNC"),
	CSTR("PCM2_CLK"),
	CSTR("PCM2_DOUT"),
	CSTR("PCM2_DIN"),
	CSTR("PCM2_MCLK"),

	//PortH
	CSTR("UART0_TX"),
	CSTR("UART0_RX"),
	CSTR("CIR_TX"),
	CSTR("SPI1_CS"),
	CSTR("SPI1_CLK"),
	CSTR("SPI1_MOSI"),
	CSTR("SPI1_MISO"),
	CSTR("Reserved"),
	CSTR("HSCL"),
	CSTR("HSDA"),

	CSTR("HCEC"),

	//PortL
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("S_UART_TX"),
	CSTR("S_UART_RX"),
	CSTR("S_JTAG_MS"),
	CSTR("S_JTAG_CK"),
	CSTR("S_JTAG_DO"),
	CSTR("S_JTAG_DI"),
	CSTR("S_PWM0"),
	CSTR("S_CIR_RX"),

	CSTR("S_OWC"),

	//PortM
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
};

static Text::CString GPIOControl_Func3[] = {
	//PortC
	CSTR("Reserved"),
	CSTR("SDC2_DS"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("SDC2_CLK"),
	CSTR("SDC2_CMD"),
	CSTR("SDC2_D0"),
	CSTR("SDC2_D1"),
	CSTR("SDC2_D2"),
	CSTR("SDC2_D3"),

	CSTR("SDC2_D4"),
	CSTR("SDC2_D5"),
	CSTR("SDC2_D6"),
	CSTR("SDC2_D7"),
	CSTR("SDC2_RST"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortD
	CSTR("TS0_CLK"),
	CSTR("TS0_ERR"),
	CSTR("TS0_SYNC"),
	CSTR("TS0_DVLD"),
	CSTR("TS0_D0"),
	CSTR("TS0_D1"),
	CSTR("TS0_D2"),
	CSTR("TS0_D3"),
	CSTR("TS0_D4"),
	CSTR("TS0_D5"),

	CSTR("TS0_D6"),
	CSTR("TS0_D7"),
	CSTR("TS1_CLK"),
	CSTR("TS1_ERR"),
	CSTR("TS1_SYNC"),
	CSTR("TS1_DVLD"),
	CSTR("TS1_D0"),
	CSTR("TS2_CLK"),
	CSTR("TS2_ERR"),
	CSTR("TS2_SYNC"),

	CSTR("TS2_DVLD"),
	CSTR("TS2_D0"),
	CSTR("TS3_CLK"),
	CSTR("TS3_ERR"),
	CSTR("TS3_SYNC"),
	CSTR("TS3_DVLD"),
	CSTR("TS3_D0"),

	//PortF
	CSTR("JTAG_MS1"),
	CSTR("JTAG_DI1"),
	CSTR("UART0_TX"),
	CSTR("JTAG_DO1"),
	CSTR("UART0_RX"),
	CSTR("JTAG_CK1"),
	CSTR("Reserved"),

	//PortG
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("H_PCM2_SYNC"),
	CSTR("H_PCM2_CLK"),
	CSTR("H_PCM2_DOUT"),
	CSTR("H_PCM2_DIN"),
	CSTR("H_PCM2_MCLK"),

	//PortH
	CSTR("PCM0_SYNC"),
	CSTR("PCM0_CLK"),
	CSTR("PCM0_DOUT"),
	CSTR("PCM0_DIN"),
	CSTR("PCM0_MCLK"),
	CSTR("OWA_MCLK"),
	CSTR("OWA_IN"),
	CSTR("OWA_OUT"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),

	//PortL
	CSTR("S_TWI_SCK"),
	CSTR("S_TWI_SDA"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("S_PWM1"),

	//PortM
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
};

static Text::CString GPIOControl_Func4[] = {
	//PortC
	CSTR("SPI0_CLK"),
	CSTR("Reserved"),
	CSTR("SPI0_MOSI"),
	CSTR("SPI0_MISO"),
	CSTR("Reserved"),
	CSTR("SPI0_CS"),
	CSTR("SPI0_HOLD"),
	CSTR("SPI0_WP"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortD
	CSTR("CSI_PCLK"),
	CSTR("CSI_MCLK"),
	CSTR("CSI_HSYNC"),
	CSTR("CSI_VSYNC"),
	CSTR("CSI_D0"),
	CSTR("CSI_D1"),
	CSTR("CSI_D2"),
	CSTR("CSI_D3"),
	CSTR("CSI_D4"),
	CSTR("CSI_D5"),

	CSTR("CSI_D6"),
	CSTR("CSI_D7"),
	CSTR("CSI_SCK"),
	CSTR("CSI_SDA"),
	CSTR("DMIC_CLK"),
	CSTR("DMIC_DATA0"),
	CSTR("DMIC_DATA1"),
	CSTR("DMIC_DATA2"),
	CSTR("DMIC_DATA3"),
	CSTR("UART2_TX"),

	CSTR("UART2_RX"),
	CSTR("UART2_RTS"),
	CSTR("UART2_CTS"),
	CSTR("UART3_TX"),
	CSTR("UART3_RX"),
	CSTR("UART3_RTS"),
	CSTR("UART3_CTS"),

	//PortF
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortG
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("SIM0_VPPEN"),
	CSTR("SIM0_VPPPP"),

	CSTR("SIM0_PWREN"),
	CSTR("SIM0_CLK"),
	CSTR("SIM0_DATA"),
	CSTR("SIM0_RST"),
	CSTR("SIM0_DET"),

	//PortH
	CSTR("H_PCM0_SYNC"),
	CSTR("H_PCM0_CLK"),
	CSTR("H_PCM0_DOUT"),
	CSTR("H_PCM0_DIN"),
	CSTR("H_PCM0_MCLK"),
	CSTR("TWI1_SCK"),
	CSTR("TWI1_SDA"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),

	//PortL
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),

	//PortM
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
};

static Text::CString GPIOControl_Func5[] = {
	//PortC
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortD
	CSTR("RGMII_RXD3/RMII_NULL"),
	CSTR("RGMII_RXD2/RMII_NULL"),
	CSTR("RGMII_RXD1/RMII_RXD1"),
	CSTR("RGMII_RXD0/RMII_RXD0"),
	CSTR("RGMII_RXCK/RMII_NULL"),
	CSTR("RGMII_RXCTL/RMII_CRS_DV"),
	CSTR("RGMII_NULL/RMII_RXER"),
	CSTR("RGMII_TXD3/RMII_NULL"),
	CSTR("RGMII_TXD2/RMII_NULL"),
	CSTR("RGMII_TXD1/RMII_TXD1"),

	CSTR("RGMII_TXD0/RMII_TXD0"),
	CSTR("RGMII_TXCK/RMII_TXCK"),
	CSTR("RGMII_TXCTL/RMII_TXEN"),
	CSTR("RGMII_CLKIN/RMII_NULL"),
	CSTR("CSI_D8"),
	CSTR("CSI_D9"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("MDC"),

	CSTR("MDIO"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("JTAG_MS"),
	CSTR("JTAG_CK"),
	CSTR("JTAG_DO"),
	CSTR("JTAG_DI"),

	//PortF
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortG
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortH
	CSTR("SIM1_VPPEN"),
	CSTR("SIM1_VPPPP"),
	CSTR("SIM1_PWREN"),
	CSTR("SIM1_CLK"),
	CSTR("SIM1_DATA"),
	CSTR("SIM1_RST"),
	CSTR("SIM1_DET"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),

	//PortL
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),

	//PortM
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
};

static Text::CString GPIOControl_Func6[] = {
	//PortC
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortD
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortF
	CSTR("PF_EINT0"),
	CSTR("PF_EINT1"),
	CSTR("PF_EINT2"),
	CSTR("PF_EINT3"),
	CSTR("PF_EINT4"),
	CSTR("PF_EINT5"),
	CSTR("PF_EINT6"),

	//PortG
	CSTR("PG_EINT0"),
	CSTR("PG_EINT1"),
	CSTR("PG_EINT2"),
	CSTR("PG_EINT3"),
	CSTR("PG_EINT4"),
	CSTR("PG_EINT5"),
	CSTR("PG_EINT6"),
	CSTR("PG_EINT7"),
	CSTR("PG_EINT8"),
	CSTR("PG_EINT9"),

	CSTR("PG_EINT10"),
	CSTR("PG_EINT11"),
	CSTR("PG_EINT12"),
	CSTR("PG_EINT13"),
	CSTR("PG_EINT14"),

	//PortH
	CSTR("PH_EINT0"),
	CSTR("PH_EINT1"),
	CSTR("PH_EINT2"),
	CSTR("PH_EINT3"),
	CSTR("PH_EINT4"),
	CSTR("PH_EINT5"),
	CSTR("PH_EINT6"),
	CSTR("PH_EINT7"),
	CSTR("PH_EINT8"),
	CSTR("PH_EINT9"),

	CSTR("PH_EINT10"),

	//PortL
	CSTR("S_PL_EINT0"),
	CSTR("S_PL_EINT1"),
	CSTR("S_PL_EINT2"),
	CSTR("S_PL_EINT3"),
	CSTR("S_PL_EINT4"),
	CSTR("S_PL_EINT5"),
	CSTR("S_PL_EINT6"),
	CSTR("S_PL_EINT7"),
	CSTR("S_PL_EINT8"),
	CSTR("S_PL_EINT9"),

	CSTR("S_PL_EINT10"),

	//PortM
	CSTR("S_PM_EINT0"),
	CSTR("S_PM_EINT1"),
	CSTR("S_PM_EINT2"),
	CSTR("S_PM_EINT3"),
	CSTR("S_PM_EINT4"),
};

Text::CString IO::GPIOControl::PinModeGetName(UIntOS pinNum, UIntOS pinMode)
{
	if (pinMode == 0)
	{
		return CSTR("Input");
	}
	else if (pinMode == 1)
	{
		return CSTR("Output");
	}
	else if (pinNum >= 93)
	{
		return CSTR("Unknown");
	}
	if (pinMode == 2)
	{
		return GPIOControl_Func2[pinNum];	
	}
	else if (pinMode == 3)
	{
		return GPIOControl_Func3[pinNum];	
	}
	else if (pinMode == 4)
	{
		return GPIOControl_Func4[pinNum];	
	}
	else if (pinMode == 5)
	{
		return GPIOControl_Func5[pinNum];	
	}
	else if (pinMode == 6)
	{
		return GPIOControl_Func6[pinNum];	
	}
	else if (pinMode == 7)
	{
		return CSTR("IO Disable");
	}
	return CSTR("Unknown");
}
