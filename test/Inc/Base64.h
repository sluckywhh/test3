#ifndef __BASE64_H__
#define __BASE64_H__

static const char base64char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const unsigned char b64dec[256] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 0-7 */
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 8-15 */
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 16-23 */
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 24-31 */
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 32-39 */
		0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f, /* 40-47 */
		0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, /* 48-55 */
		0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 56-63 */
		0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, /* 64-71 */
		0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, /* 72-79 */
		0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, /* 80-87 */
		0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff, /* 88-95 */
		0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, /* 96-103 */
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, /* 104-111 */
		0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, /* 112-119 */
		0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff, /* 120-127 */

		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 128-255 */
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

inline char *base64_encode(unsigned char* binData, unsigned long binLength, char* base64, unsigned long* pulbase64Length)
{
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned long current = 0;

	for (i = 0; i < binLength; i += 3) 
	{
		//获取第一个6位
		current = (*(binData+i) >> 2) & 0x3F;
		*(base64 + j++) = base64char[current];

		//获取第二个6位的前两位
		current = (*(binData+i) << 4) & 0x30;

		//如果只有一个字符，那么需要做特殊处理
		if (binLength <= (i+1)) 
		{
			*(base64 + j++) = base64char[current];
			*(base64 + j++) = '=';
			*(base64 + j++) = '=';
			break;
		}

		//获取第二个6位的后四位
		current |= (*(binData+i+1) >> 4 ) & 0xf;
		*(base64 + j++) = base64char[current];
		//获取第三个6位的前四位
		current = (*(binData+i+1) << 2 ) & 0x3c;
		if (binLength <= (i+2)) 
		{
			*(base64 + j++) = base64char[current];
			*(base64 + j++) = '=';
			break;
		}

		//获取第三个6位的后两位
		current |= (*(binData+i+2) >> 6) & 0x03;
		*(base64 + j++) = base64char[current];

		//获取第四个6位
		current = *(binData+i+2) & 0x3F;
		*(base64 + j++) = base64char[current];
	}

	*pulbase64Length = j;
	*(base64+j) = '\0';

	return base64;
}
 

//
inline unsigned char *base64_decode(char const* base64,  unsigned long  base64Length, unsigned char* binData, unsigned long* pulbinLength)
{
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned long k = 0;

	unsigned char temp[4] = "";

	for (i = 0; i < base64Length; i += 4) 
	{
		for (j = 0; j < 64; j++) 
		{
			if (*(base64 + i) == base64char[j]) 
			{
				temp[0] = j;
			}
		}

		for (j = 0; j < 64 ; j++) 
		{
			if (*(base64 + i + 1) == base64char[j]) 
			{
				temp[1] = j;
			}
		}


		for (j = 0; j < 64 ; j++) 
		{
			if (*(base64 + i + 2) == base64char[j]) 
			{
				temp[2] = j;
			}
		}


		for (j = 0; j < 64 ; j++) 
		{
			if (*(base64 + i + 3) == base64char[j]) 
			{
				temp[3] = j;
			}
		}

		*(binData + k++) = ((temp[0] << 2) & 0xFC) | ((temp[1]>>4) & 0x03);
		if ( *(base64 + i + 2)  == '=' )
			break;

		*(binData + k++) = ((temp[1] << 4) & 0xF0) | ((temp[2]>>2) & 0x0F);
		if ( *(base64 + i + 3) == '=' )
			break;

		*(binData + k++) = ((temp[2] << 6) & 0xF0) | (temp[3] & 0x3F);
	}

	*pulbinLength = k;
	return binData;
}
#endif