#ifndef _TRACE_PACKET_H
#define _TRACE_PACKET_H



#define INF_SIZE 16 
class CLogDataInf
{
public:
	CLogDataInf();
	~CLogDataInf();
	void putInf(const char *strdata);
	void putInf(char *strdata);	
	void putInf(char *hexData, int dataLen);
	int packet();
	int packet(char *&packet);
	int unPacket(char *packet);
	int unPacket(char *packet, char *infs[], int infLens[]);
	int getPacket(char *&packet);
	void I2CLen(int iLen, char *CLen, int CLenSize);
	void C2ILen(char *CLen, int CLenSize, int &iLen);
public:
	static int m_lenSize;	
	char *m_infs[INF_SIZE];
	int m_infLens[INF_SIZE];
	char *m_packet;
	int m_packetLen;
	int m_infsNum;
};


#endif

