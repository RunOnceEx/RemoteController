#pragma once

#include <windows.h>

/************************************************************************
* @class RingBuffer
* @update Ԭ����,2018-10-19
************************************************************************/
class RingBuffer
{
public:
	RingBuffer(unsigned int size);
	~RingBuffer(void);

public:

	inline int PushData(const char *pData, int size) { return AddData(pData, size); }

	inline int PushFrame(const char *pFrame, int size) { return AddData(pFrame, size); }

	inline void Reset() { EnterCriticalSection(&m_section);ResetData();LeaveCriticalSection(&m_section); }

	inline int GetFrameNum() const { return m_frameNum; }

	int PopData(char *pData, unsigned int bufSize);
	
	int PopAllData(char *pData, unsigned int bufSize);

private:

	int AddData(const char *pData, unsigned int length);

	int GetData(char *buf, unsigned int length);

	// ��total>m_maxSizeʱ��������
	int raise(unsigned int total);

	// ��total<m_maxSize/2ʱ��С����
	void reduce(unsigned int size);

	inline void ResetData() { m_totalData = 0; m_writePointer = 0; m_readPointer = 0; m_frameNum = 0; }

private:
	unsigned int m_totalData;			// ��ǰ���ֽ�
	unsigned int m_readPointer;			// ��ָ��
	unsigned int m_writePointer;		// дָ��
	unsigned int m_curSize;				// ��ǰ����
	unsigned int m_minSize;				// ��С��������ʼֵ��
	unsigned int m_frameNum;			// ֡��

	CRITICAL_SECTION m_section;
	char *m_buf;
};
