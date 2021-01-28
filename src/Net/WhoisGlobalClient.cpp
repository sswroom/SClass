#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WhoisGlobalClient.h"

Net::WhoisGlobalClient::WhoisGlobalClient(Net::SocketFactory *sockf) : Net::WhoisClient(sockf, 0, 0)
{
}

Net::WhoisGlobalClient::~WhoisGlobalClient()
{
}

Net::WhoisRecord *Net::WhoisGlobalClient::RequestIP(UInt32 ip)
{
	UInt32 ipCmp = Net::SocketUtil::IPv4ToSortable(ip);
	const UTF8Char *whoisHost;
	const Char *prefix = 0;
	switch (ipCmp >> 24)
	{
	case 41:
	case 102:
	case 105:
	case 196:
	case 197:
		whoisHost = (const UTF8Char*)"whois.afrinic.net";
		break;

	case 202:
		if ((ipCmp & 0xFFFFE000) == 0xCA7B0000) // 202.123.0.0/19
		{
			whoisHost = (const UTF8Char*)"whois.afrinic.net";
			break;
		}
	case 1:
	case 14:
	case 27:
	case 36:
	case 39:
	case 42:
	case 49:
	case 58:
	case 59:
	case 60:
	case 61:
	case 101:
	case 103:
	case 106:
	case 110:
	case 111:
	case 112:
	case 113:
	case 114:
	case 115:
	case 116:
	case 117:
	case 118:
	case 119:
	case 120:
	case 121:
	case 122:
	case 123:
	case 124:
	case 125:
	case 126:
	case 175:
	case 180:
	case 182:
	case 183:
	case 203:
	case 210:
	case 211:
	case 218:
	case 219:
	case 220:
	case 221:
	case 222:
	case 223:
		whoisHost = (const UTF8Char*)"whois.apnic.net";
		break;
/* APNIC
43.224.0.0/13
43.236.0.0/14
43.240.0.0/14
43.245.0.0/16
43.246.0.0/15
43.248.0.0/14
43.252.0.0/16
43.254.0.0/15
45.64.0.0/16
45.65.0.0/20
45.65.16.0/20
45.65.32.0/19
45.112.0.0/12
45.248.0.0/13
137.59.0.0/16
139.5.0.0/16
144.48.0.0/16
146.196.32.0/19
146.196.64.0/18
150.107.0.0/16
150.129.0.0/16
150.242.0.0/16
157.119.0.0/16
160.19.20.0/22
160.19.48.0 /21
160.19.64.0/22
160.19.208.0/20
160.19.224.0/22
160.20.0.0/20
160.20.40.0 /21
160.20.48.0/20
160.20.72.0/22
160.20.222.0/23
160.202.8.0 /21
160.202.32.0/19
160.202.128.0/17
160.238.0.0/24
160.238.12.0/22
160.238.16.0/22
160.238.33.0/24
160.238.34.0/23
160.238.58.0/23
160.238.64.0/19
162.12.208.0/21
162.12.240.0/21
163.47.4.0/22
163.47.8.0/21
163.47.16.0/23
163.47.18.0/24
163.47.20.0/23
163.47.32.0/21
163.47.40.0/22
163.47.44.0/23
163.47.47.0/24
163.47.48.0/20
163.47.64.0/18
163.47.128.0/17
163.53.0.0/16
192.12.109.0/24
192.26.110.0/24
192.51.188.0/24
192.75.137.0/24
192.135.90.0/23
192.135.99.0/24
192.140.128.0/17
192.144.78.0/23
192.144.80.0/20
192.145.228.0/23
192.156.144.0/24
192.156.220.0/24
192.188.82.0/23
192.197.113.0/24
199.21.172.0/22
199.212.57.0/24
204.52.191.0/24
216.250.96.0/20
*/
	case 2:
	case 5:
	case 25:
	case 31:
	case 37:
	case 46:
	case 51:
	case 53:
	case 57:
	case 62:
	case 77:
	case 78:
	case 79:
	case 80:
	case 81:
	case 82:
	case 83:
	case 84:
	case 85:
	case 86:
	case 87:
	case 88:
	case 89:
	case 90:
	case 91:
	case 92:
	case 93:
	case 94:
	case 95:
	case 109:
	case 141:
	case 145:
	case 151:
	case 176:
	case 178:
	case 185:
	case 188:
	case 193:
	case 194:
	case 195:
	case 212:
	case 213:
	case 217:
		whoisHost = (const UTF8Char*)"whois.ripe.net";
		break;

	case 177:
	case 179:
	case 181:
	case 186:
	case 187:
	case 189:
	case 190:
	case 191:
	case 200:
	case 201:
		whoisHost = (const UTF8Char*)"whois.lacnic.net";
		break;
	
	case 3:
	case 4:
	case 7:
	case 8:
	case 9:
	case 13:
	case 15:
	case 16:
	case 18:
	case 20:
	case 23:
	case 24:
	case 32:
	case 34:
	case 35:
	case 40:
	case 44:
	case 45:
		if (ipCmp >= 0x2DFA1C00 && ipCmp <= 0x2DFFFFFF) //45.250.28.0 - 45.255.255.255
		{
			whoisHost = (const UTF8Char*)"whois.apnic.net";
			break;
		}
	case 47:
	case 50:
	case 52:
	case 54:
	case 63:
	case 64:
		if (ipCmp >= 0x40785800 && ipCmp <= 0x40785FFF) //64.120.88.0 - 64.120.95.255
		{
			whoisHost = (const UTF8Char*)"whois.apnic.net";
			break;
		}
	case 65:
	case 66:
	case 67:
	case 68:
	case 69:
	case 70:
	case 71:
	case 72:
	case 73:
	case 74:
	case 75:
	case 76:
	case 96:
	case 97:
	case 98:
	case 99:
	case 100:
	case 104:
	case 107:
	case 108:
	case 128:
	case 129:
	case 130:
		if (ipCmp >= 0x82580000 && ipCmp <= 0x8259FFFF) //130.88.0.0 - 130.89.255.255
		{
			whoisHost = (const UTF8Char*)"whois.ripe.net";
			break;
		}
	case 131:
	case 132:
	case 134:
	case 135:
	case 136:
	case 137:
	case 138:
	case 139:
		if (ipCmp >= 0x8BA20000 && ipCmp <= 0x8BA2FFFF) //139.162.0.0 - 139.162.255.255
		{
			whoisHost = (const UTF8Char*)"whois.ripe.net";
			break;
		}
	case 140:
	case 142:
	case 143:
	case 144:
	case 146:
	case 147:
		if (ipCmp >= 0x934B0000 && ipCmp <= 0x934BFFFF) //147.75.0.0 - 147.75.255.255
		{
			whoisHost = (const UTF8Char*)"whois.ripe.net";
			break;
		}
	case 148:
	case 149:
	case 152:
	case 155:
	case 156:
	case 157:
	case 158:
	case 159:
	case 160:
	case 161:
	case 162:
	case 164:
		if (ipCmp >= 0xA4340000 && ipCmp <= 0xA4347FFF) //164.52.0.0 - 164.52.127.255
		{
			whoisHost = (const UTF8Char*)"whois.apnic.net";
			break;
		}
	case 165:
	case 166:
	case 167:
	case 168:
	case 169:
	case 170:
	case 172:
	case 173:
	case 174:
	case 184:
	case 192:
	case 198:
	case 199:
	case 204:
	case 205:
	case 206:
	case 207:
	case 208:
	case 209:
		if (ipCmp >= 0xD13AB000 && ipCmp <= 0xD13ABFFF) //209.58.176.0 - 209.58.191.255
		{
			whoisHost = (const UTF8Char*)"whois.ripe.net";
			break;
		}
	case 214:
	case 215:
	case 216:

	default:
		whoisHost = (const UTF8Char*)"whois.arin.net";
		prefix = "n + ";
		break;
	}

	return Net::WhoisClient::RequestIP(ip, this->sockf->DNSResolveIPv4(whoisHost), prefix);
}
