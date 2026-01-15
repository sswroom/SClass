#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

// Allwinner A64
#define GPIO_BASE_MAP  (0x01C20800)

struct IO::GPIOControl::ClassData
{
	IO::PhysicalMem *mem;
	volatile UInt32 *gpioPtr;
};

void GPIOControl_GetPortNum(UInt16 pinNum, IntOS *portNum, IntOS *index)
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
	DEL_CLASS(this->clsData->mem);
	MemFree(this->clsData);
}

Bool IO::GPIOControl::IsError()
{
	return this->clsData->mem->IsError();
}

UIntOS IO::GPIOControl::GetPinCount()
{
	return 103;
}

Bool IO::GPIOControl::IsPinHigh(UIntOS pinNum)
{
	if (pinNum >= 103)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	return ((this->clsData->gpioPtr[portNum * 9 + 4] >> index) & 1) != 0;
}

Bool IO::GPIOControl::IsPinOutput(UIntOS pinNum)
{
	if (pinNum >= 103)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	return ((this->clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7) == 1;
}

UIntOS IO::GPIOControl::GetPinMode(UIntOS pinNum)
{
	if (pinNum >= 103)
	{
		return 0;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	return (this->clsData->gpioPtr[portNum * 9 + (index >> 3)] >> ((index & 7) * 4)) & 7;
}

Bool IO::GPIOControl::SetPinOutput(UIntOS pinNum, Bool isOutput)
{
	if (pinNum >= 103)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 mask = 7 << ((index & 7) * 4);
	UInt32 v = (isOutput?1:0) << ((index & 7) * 4);
	this->clsData->gpioPtr[portNum * 9 + (index >> 3)] = (this->clsData->gpioPtr[portNum * 9 + (index >> 3)] & ~mask) | v;
	return true;
}

Bool IO::GPIOControl::SetPinState(UIntOS pinNum, Bool isHigh)
{
	if (pinNum >= 103)
	{
		return false;
	}
	IntOS portNum;
	IntOS index;
	GPIOControl_GetPortNum(pinNum, &portNum, &index);

	UInt32 v = isHigh?(1 << index):0;
	this->clsData->gpioPtr[portNum * 9 + 4] = (this->clsData->gpioPtr[portNum * 9 + 4] & ~(1 << index)) | v;
	return true;
}

Bool IO::GPIOControl::SetPullType(UIntOS pinNum, IO::IOPin::PullType pt)
{
	if (pinNum >= 103)
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
	this->clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] = (this->clsData->gpioPtr[portNum * 9 + 7 + (index >> 4)] & ~(3 << ((index & 15) * 2))) | v;
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
	//PortB
	CSTR("UART2_TX"),
	CSTR("UART2_RX"),
	CSTR("UART2_RTS"),
	CSTR("UART2_CTS"),
	CSTR("AIF2_SYNC"),
	CSTR("AIF2_BCLK"),
	CSTR("AIF2_DOUT"),
	CSTR("AIF2_DIN"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortC
	CSTR("NAND_WE"),
	CSTR("NAND_ALE"),
	CSTR("NAND_CLE"),
	CSTR("NAND_CE1"),
	CSTR("NAND_CE0"),
	CSTR("NAND_RE"),
	CSTR("NAND_RB0"),
	CSTR("NAND_RB1"),
	CSTR("NAND_DQ0"),
	CSTR("NAND_DQ1"),

	CSTR("NAND_DQ2"),
	CSTR("NAND_DQ3"),
	CSTR("NAND_DQ4"),
	CSTR("NAND_DQ5"),
	CSTR("NAND_DQ6"),
	CSTR("NAND_DQ7"),
	CSTR("NAND_DQS"),

	//PortD
	CSTR("LCD_D2"),
	CSTR("LCD_D3"),
	CSTR("LCD_D4"),
	CSTR("LCD_D5"),
	CSTR("LCD_D6"),
	CSTR("LCD_D7"),
	CSTR("LCD_D10"),
	CSTR("LCD_D11"),
	CSTR("LCD_D12"),
	CSTR("LCD_D13"),

	CSTR("LCD_D14"),
	CSTR("LCD_D15"),
	CSTR("LCD_D18"),
	CSTR("LCD_D19"),
	CSTR("LCD_D20"),
	CSTR("LCD_D21"),
	CSTR("LCD_D22"),
	CSTR("LCD_D23"),
	CSTR("LCD_CLK"),
	CSTR("LCD_DE"),

	CSTR("LCD_HSYNC"),
	CSTR("LCD_VSYNC"),
	CSTR("PWM0"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortE
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
	CSTR("PLL_LOCK_DBG"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

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

	CSTR("AIF3_SYNC"),
	CSTR("AIF3_BCLK"),
	CSTR("AIF3_DOUT"),
	CSTR("AIF3_DIN"),

	//PortH
	CSTR("TWI0_SCK"),
	CSTR("TWI0_SDA"),
	CSTR("TWI1_SCK"),
	CSTR("TWI1_SDA"),
	CSTR("UART3_TX"),
	CSTR("UART3_RX"),
	CSTR("UART3_RTS"),
	CSTR("UART3_CTS"),
	CSTR("OWA_OUT"),
	CSTR("Reserved"),

	CSTR("MIC_CLK"),
	CSTR("MIC_DATA"),
};

static Text::CString GPIOControl_Func3[] = {
	//PortB
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("I2S0_MCLK"),
	CSTR("PCM0_SYNC"),
	CSTR("PCM0_BCLK"),
	CSTR("PCM0_DOUT"),
	CSTR("PCM0_DIN"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortC
	CSTR("Reserved"),
	CSTR("SDC2_DS"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("SDC2_CLK"),
	CSTR("SDC2_CMD"),
	CSTR("Reserved"),
	CSTR("SDC2_D0"),
	CSTR("SDC2_D1"),

	CSTR("SDC2_D2"),
	CSTR("SDC2_D3"),
	CSTR("SDC2_D4"),
	CSTR("SDC2_D5"),
	CSTR("SDC2_D6"),
	CSTR("SDC2_D7"),
	CSTR("SDC2_RST"),

	//PortD
	CSTR("UART3_TX"),
	CSTR("UART3_RX"),
	CSTR("UART4_TX"),
	CSTR("UART4_RX"),
	CSTR("UART4_RTS"),
	CSTR("UART4_CTS"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("LVDS_VP0"),
	CSTR("LVDS_VN0"),
	CSTR("LVDS_VP1"),
	CSTR("LVDS_VN1"),
	CSTR("LVDS_VP2"),
	CSTR("LVDS_VN2"),
	CSTR("LVDS_VPC"),
	CSTR("LVDS_VNC"),

	CSTR("LVDS_VP3"),
	CSTR("LVDS_VN3"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortE
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
	CSTR("TWI2_SCK"),
	CSTR("TWI2_SDA"),
	CSTR("Reserved"),
	CSTR("Reserved"),

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

	CSTR("PCM1_SYNC"),
	CSTR("PCM1_BCLK"),
	CSTR("PCM1_DOUT"),
	CSTR("PCM1_DIN"),

	//PortH
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
};

static Text::CString GPIOControl_Func4[] = {
	//PortB
	CSTR("JTAG_MS0"),
	CSTR("JTAG_CK0"),
	CSTR("JTAG_DO0"),
	CSTR("JTAG_DI0"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("UART0_TX"),
	CSTR("UART0_RX"),

	//PortC
	CSTR("SPI0_MOSI"),
	CSTR("SPI0_MISO"),
	CSTR("SPI0_CLK"),
	CSTR("SPI0_CS"),
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
	CSTR("SPI1_CS"),
	CSTR("SPI1_CLK"),
	CSTR("SPI1_MOSI"),
	CSTR("SPI1_MISO"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("RGMII_RXD3/MII_RXD3/RMII_NULL"),
	CSTR("RGMII_RXD2/MII_RXD2/RMII_NULL"),

	CSTR("RGMII_RXD1/MII_RXD1/RMII_RXD1"),
	CSTR("RGMII_RXD0/MII_RXD0/RMII_RXD0"),
	CSTR("RGMII_RXCK/MII_RXCK/RMII_NULL"),
	CSTR("RGMII_RXCTL/MII_RXDV/RMII_CRS_DV"),
	CSTR("RGMII_NULL/MII_RXERR/RMII_RXER"),
	CSTR("RGMII_TXD3/MII_TXD3/RMII_NULL"),
	CSTR("RGMII_TXD2/MII_TXD2/RMII_NULL"),
	CSTR("RGMII_TXD1/MII_TXD1/RMII_TXD1"),
	CSTR("RGMII_TXD0/MII_TXD0/RMII_TXD0"),
	CSTR("RGMII_TXCK/MII_TXCK/RMII_TXCK"),

	CSTR("RGMII_TXCTL/MII_TXEN/RMII_TXEN"),
	CSTR("RGMII_CLKIN/MII_COL/RMII_NULL"),
	CSTR("MDC"),
	CSTR("MDIO"),
	CSTR("Reserved"),

	//PortE
	CSTR("TS_CLK"),
	CSTR("TS_ERR"),
	CSTR("TS_SYNC"),
	CSTR("TS_DVLD"),
	CSTR("TS_D0"),
	CSTR("TS_D1"),
	CSTR("TS_D2"),
	CSTR("TS_D3"),
	CSTR("TS_D4"),
	CSTR("TS_D5"),

	CSTR("TS_D6"),
	CSTR("TS_D7"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

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

	//PortH
	CSTR("w_dbgin"),
	CSTR("w_dbgack"),
	CSTR("w_jtclk"),
	CSTR("w_jtdi"),
	CSTR("w_jtdo"),
	CSTR("dbg_uart_txd"),
	CSTR("w_jtms"),
	CSTR("w_jtrst_n"),
	CSTR("jtrstn_k"),
	CSTR("jtclk_k"),

	CSTR("jtms_k"),
	CSTR("jtdi_k"),
};

static Text::CString GPIOControl_Func5[] = {
	//PortB
	CSTR("Reserved"),
	CSTR("SIM_PWREN"),
	CSTR("SIM_VPPEN"),
	CSTR("SIM_VPPPP"),
	CSTR("SIM_CLK"),
	CSTR("SIM_DATA"),
	CSTR("SIM_RST"),
	CSTR("SIM_DET"),
	CSTR("Reserved"),
	CSTR("Reserved"),

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
	CSTR("CCIR_CLK"),
	CSTR("CCIR_DE"),
	CSTR("CCIR_HSYNC"),
	CSTR("CCIR_VSYNC"),
	CSTR("CCIR_D0"),
	CSTR("CCIR_D1"),
	CSTR("CCIR_D2"),
	CSTR("CCIR_D3"),
	CSTR("CCIR_D4"),
	CSTR("CCIR_D5"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("CCIR_D6"),
	CSTR("CCIR_D7"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	//PortE
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

	//PortH
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
};

static Text::CString GPIOControl_Func6[] = {
	//PortB
	CSTR("PB_EINT0"),
	CSTR("PB_EINT1"),
	CSTR("PB_EINT2"),
	CSTR("PB_EINT3"),
	CSTR("PB_EINT4"),
	CSTR("PB_EINT5"),
	CSTR("PB_EINT6"),
	CSTR("PB_EINT7"),
	CSTR("PB_EINT8"),
	CSTR("PB_EINT9"),

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

	//PortE
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
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

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
	CSTR("PH_EINT11"),
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
	else if (pinNum >= 103)
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
