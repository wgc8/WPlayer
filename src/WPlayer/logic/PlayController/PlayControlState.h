#pragma once
// 定义当前播放状态
enum PlayControlStatus
{
	PCS_UNINIT = -1,	// 未初始化
	PCS_IDLE,			// 空闲，已初始化
	PCS_PLAYING,		// 播放中
	PCS_PAUSE,			// 暂停
	PCS_STOP,			// 停止
	PCS_SEEK			// 跳转
};