#pragma once
#include "../stdafx.h"

/// ����������
#define BUFFER_LENGTH 4096

/// SIPͷ�����
#define MAX_SIPLENGTH 256

#define XML_TAG			"<?xml"			/**< XML��� */

#define CALLID_TAG		"Call-ID"		/**< Callid��� */

#define CSEQ_TAG		"CSeq"			/**< Cseq��� */

#define XML_LEN_TAG		"Content-Length" /**< XML���ȱ�� */

typedef char str32[32];

// ��ȡseq���м�CallId
const char* GetSeqAndCallId(const char *buffer, str32 &seq, str32 &cid);

class RingBuffer
{
public:
	RingBuffer( int size );
	~RingBuffer(void);
public:

	int PushData(const char *pData, int size);
	int PopData(char *pData, int bufSize);
	int PopDataEx(char *pData, int bufSize);
	void Reset( );
	
private:
	int AddData(const char *pData, unsigned int length);
	int GetData(char *buf, unsigned int max);
	int GetDataEx(char *buf, unsigned int nLen);

public:

	// ���ص�ǰ�������е�����
	int Size() const { return m_totalData; };
	// ���ص�ǰ�������п��пռ�Ĵ�С
	int Space() const { return m_maxSize-m_totalData; };
	//////////////////////////////////////////////////////////////////////////
	// ����һ��������SipXml����
	bool PopXml(char *pDst, const bool * = 0);// �ú�������PopXml Sipͷ�����Ϲ淶
	// ���һ��������SipXml����
	bool GetXml(char *pDst, const bool *bExitFlag);
	// ���SipXml����
	inline int GetSipXmlLength() const
	{
		return m_nSipXmlLen;
	}

private:
	unsigned int m_totalData;
	unsigned int m_readPointer;
	unsigned int m_writePointer;
	unsigned int m_maxSize;

	CRITICAL_SECTION m_section;
	char *m_buf;

	//////////////////////////////////////////////////////////////////////////
	static char m_xml[BUFFER_LENGTH];	/**< ��ǰSipXml[���̲߳���ȫ] */
	int m_nSipXmlLen;					/**< ��ǰSipXml���� */

	bool m_bGotSip;						/**< �Ƿ���ȡ��Sipͷ */
	char *m_pos;						/**< ָ��ǰsipXml�ַ��� */
	// ����һ��������sipXML
	const char* _PopXml(int nSipLength = MAX_SIPLENGTH, int nStep = 1);
	int getXmlLenFromSip(char* cHead);
};
