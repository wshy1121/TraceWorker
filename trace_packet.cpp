#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "trace_packet.h"

int CLogDataInf::m_lenSize = 4;

CLogDataInf::CLogDataInf(bool isCopy)
:m_packet(NULL)
,m_packetLen(0)
,m_infsNum(0)
,m_isCopy(true)
{
}


CLogDataInf::~CLogDataInf()
{
	if (m_packet && m_isCopy)
	{
		delete []m_packet;
		m_packet = NULL;
	}
}

void CLogDataInf::I2CLen(int iLen, char *CLen, int CLenSize)
{
	memset(CLen, 0, CLenSize);
	for (int i=0; i<CLenSize; ++i)
	{
		CLen[i] = iLen;
		iLen = iLen >> 8;
		if (iLen == 0)
		{
			CLen[CLenSize-1] = i + 1;
			break;
		}
	}
	return ;
}

void CLogDataInf::C2ILen(char *CLen, int CLenSize, int &iLen)
{
	iLen = 0;
	for (int i=0; i<CLen[CLenSize-1]; ++i)
	{
		iLen += (unsigned char)(CLen[i]) << (i<<3);
	}
	return ;
}


void CLogDataInf::putInf(const char *strdata)
{
	putInf((char *)strdata);
}

void CLogDataInf::putInf(char *strdata)
{
	putInf(strdata, strlen(strdata) + 1);
}

void CLogDataInf::putInf(char *hexData, int dataLen)
{
	if (m_infsNum >= INF_SIZE)
	{
		return ;
	}
	m_infs[m_infsNum] = hexData;
	m_infLens[m_infsNum++] = dataLen;
	m_packetLen += dataLen + m_lenSize;	
}
int CLogDataInf::packet()
{
	return packet(m_packet);
}

int CLogDataInf::packet(char *&packet)
{
	if (m_packet && m_isCopy)
	{
		delete []m_packet;
	}
    m_isCopy = true;
	int mallocLen = m_lenSize + m_packetLen + m_lenSize;
	m_packet = new char[mallocLen];

	int pos = 0;
	I2CLen(mallocLen, m_packet+pos, m_lenSize);
	pos += m_lenSize;

	char *inf = NULL;
	int infLen = 0;
	for (int i=0; i<m_infsNum; ++i)
	{
		inf = m_infs[i];
		infLen = m_infLens[i];
		I2CLen(infLen + m_lenSize, m_packet+pos, m_lenSize);
		pos += m_lenSize;
		memcpy(m_packet+pos, inf, infLen);
		m_infs[i] = m_packet+pos;
		pos += infLen;
	}
	I2CLen(mallocLen, m_packet+pos, m_lenSize);

	packet = m_packet;
	return mallocLen;
}
int CLogDataInf::unPacket(char *packet)
{
    if (m_isCopy)
    {
        if (m_packet)
        {
            delete []m_packet;
        }
        int packetLen = 0;
        C2ILen(packet, m_lenSize, packetLen);
        m_packet = new char[packetLen];
        memcpy(m_packet, packet, packetLen);
    }
    else
    {
        m_packet = packet;
    }
    
	return unPacket(m_packet, m_infs, m_infLens);
}

int CLogDataInf::unPacket(char *packet, char *infs[], int infLens[])
{
	char *inf = NULL;
	int infLen = 0;
	C2ILen(packet, m_lenSize, m_packetLen);
	m_infsNum = 0;
	int i=m_lenSize;
	for (; i<m_packetLen-m_lenSize; )
	{
		C2ILen(packet+i,m_lenSize,infLen);
		inf = packet + i + m_lenSize;
		i += infLen;
		infs[m_infsNum] = inf;
		infLens[m_infsNum++] = infLen - m_lenSize;
		
	}
	C2ILen(packet+i,m_lenSize,infLen);
	if (infLen != m_packetLen)
	{	
		m_packetLen = 0;
		m_infsNum = 0;
	}
	infs[m_infsNum] = NULL;
	infLens[m_infsNum] = 0;
	return m_packetLen;
}

int CLogDataInf::getPacket(char *&packet)
{
	if (m_packet == NULL)
	{
		CLogDataInf::packet();
	}
	packet = m_packet;
	return m_lenSize + m_packetLen + m_lenSize;
}


