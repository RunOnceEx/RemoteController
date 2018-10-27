
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
	_file_size, // �ļ���С
	_version,	// �汾
	_keep_ver,  // �ػ�����汾
	_cmd_line,	// �����У�����λ�ã�
	_disk_info, // ����ʣ��ռ䣨Gb��
	COLUMNS, 
};

// App ��Ϣ
class AppInfo
{
public:
	char ip[64];		// IP
	char name[64];		// ����
	char cpu[64];		// CPU
	char mem[64];		// �ڴ棨��λ:M��
	char threads[64];	// �߳���
	char handles[64];	// �����
	char run_log[64];	// ����ʱ��
	char run_times[64];	// ��������
	char create_time[64];// ����ʱ��
	char mod_time[64];	// �޸�����
	char file_size[64]; // �ļ���С
	char version[64];	// �汾
	char keep_ver[64];	// �ػ�����汾
	char cmd_line[_MAX_PATH]; // ������
	char status[64];	// ����״̬
	char disk_info[64]; // ����״̬

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
