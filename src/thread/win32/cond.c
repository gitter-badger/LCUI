﻿/* ***************************************************************************
 * cond.c -- condition variables, for the mechanism of thread synchronization
 *
 * Copyright (C) 2013-2015 by Liu Chao <lc-soft@live.cn>
 *
 * This file is part of the LCUI project, and may only be used, modified, and
 * distributed under the terms of the GPLv2.
 *
 * (GPLv2 is abbreviation of GNU General Public License Version 2)
 *
 * By continuing to use, modify, or distribute this file you indicate that you
 * have read the license and understand and accept it fully.
 *
 * The LCUI project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GPL v2 for more details.
 *
 * You should have received a copy of the GPLv2 along with this file. It is
 * usually in the LICENSE.TXT file, If not, see <http://www.gnu.org/licenses/>.
 * ***************************************************************************/

/* ****************************************************************************
 * cond.c -- 条件变量，用于进行线程同步的机制
 *
 * 版权所有 (C) 2013-2015 归属于 刘超 <lc-soft@live.cn>
 *
 * 这个文件是LCUI项目的一部分，并且只可以根据GPLv2许可协议来使用、更改和发布。
 *
 * (GPLv2 是 GNU通用公共许可证第二版 的英文缩写)
 *
 * 继续使用、修改或发布本文件，表明您已经阅读并完全理解和接受这个许可协议。
 *
 * LCUI 项目是基于使用目的而加以散布的，但不负任何担保责任，甚至没有适销性或特
 * 定用途的隐含担保，详情请参照GPLv2许可协议。
 *
 * 您应已收到附随于本文件的GPLv2许可协议的副本，它通常在LICENSE.TXT文件中，如果
 * 没有，请查看：<http://www.gnu.org/licenses/>.
 * ***************************************************************************/

#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/thread.h>

#ifdef LCUI_BUILD_IN_WIN32

/** 初始化一个条件变量 */
int LCUICond_Init( LCUI_Cond *cond )
{
	/* 创建一个事件对象：
	 * 使用默认安全符
	 * 在事件被释放后，自动复原为无信号状态
	 * 初始状态为无信号状态
	 * 不指定名字，无名
	 */
	*cond = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( *cond == 0 ) {
		return -1;
	}
	return 0;
}

/** 销毁一个条件变量 */
int LCUICond_Destroy( LCUI_Cond *cond )
{
	CloseHandle(cond);
	return 0;
}

/** 阻塞当前线程，等待条件成立 */
int LCUICond_Wait( LCUI_Cond *cond, LCUI_Mutex *mutex )
{
	int ret;
	LCUIMutex_Unlock( mutex );
	ret = WaitForSingleObject( *cond, INFINITE );
	switch( ret ) {
	case WAIT_TIMEOUT: break;
	case WAIT_OBJECT_0: LCUIMutex_Lock( mutex ); return 0;
	case WAIT_FAILED: 
	default: return -1;
	}
	return 1;
}

/** 计时阻塞当前线程，等待条件成立 */
int LCUICond_TimedWait( LCUI_Cond *cond, LCUI_Mutex *mutex, unsigned int ms )
{
	LCUIMutex_Unlock( mutex );
	switch(  WaitForSingleObject( *cond, ms ) ) {
	case WAIT_TIMEOUT: break;
	case WAIT_OBJECT_0: LCUIMutex_Lock( mutex ); return 0;
	case WAIT_FAILED: 
	default: return -1;
	}
	return 1;
}

/** 唤醒一个阻塞等待条件成立的线程 */
int LCUICond_Signal( LCUI_Cond *cond )
{
	if( PulseEvent( *cond ) ) {
		return 0;
	}
	return -1;
}

/** 唤醒所有阻塞等待条件成立的线程 */
int LCUICond_Broadcast( LCUI_Cond *cond )
{
	if( SetEvent( *cond ) ) {
		return 0;
	}
	return -1;
}

#endif
