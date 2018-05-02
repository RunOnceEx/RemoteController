
#pragma once

// ������
enum _Column
{
	_id = 0,
	_no,		// �˿�
	_ip,		// IP
	_name,		// ����
	_cpu,		// CPUռ��
	_mem,		// �ڴ�ռ��
	_threads,	// �߳���
	_handles,	// �����
	_runlog,	// ����ʱ��
	_runtime,	// ��������
	_create_time, // ��������
	_mod_time,	// �޸�����
	_version,	// �汾
	_cmd_line,	// ������
	COLUMNS, 
};

// App ��Ϣ
class AppInfo
{
public:
	char ip[64];		// IP
	char name[64];		// ����
	char cpu[64];		// CPU
	char mem[64];		// �ڴ�
	char threads[64];	// �߳���
	char handles[64];	// �����
	char run_log[64];	// ����ʱ��
	char run_times[64];	// ��������
	char create_time[64];// ����ʱ��
	char mod_time[64];	// �޸�����
	char version[64];	// �汾
	char cmd_line[_MAX_PATH]; // ������

public:
	AppInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	AppInfo(const char *__ip, const char *__name)
	{
		memset(this, 0, sizeof(*this));
		strcpy_s(ip, __ip);
		strcpy_s(name, __name);
	}
};
