#pragma once
// ���嵱ǰ����״̬
enum PlayControlStatus
{
	PCS_UNINIT = -1,	// δ��ʼ��
	PCS_IDLE,			// ���У��ѳ�ʼ��
	PCS_PLAYING,		// ������
	PCS_PAUSE,			// ��ͣ
	PCS_STOP,			// ֹͣ
	PCS_SEEK			// ��ת
};