#pragma once

/***********************************************************************************************
 ***                                 OR_JAM_6——源代码                                      ***
 ***********************************************************************************************
 *                                                                                             *
 *                       文件名 : ORTimer.h                                                    *
 *                                                                                             *
 *                     编写名单 : IronHammer_Std                                               *
 *                                                                                             *
 *                     新建日期 : 2022/3/23                                                    *
 *                                                                                             *
 *                     最近编辑 : 2024/10/5 IronHammer_Std                                     *
 *                                                                                             *
 *                     单元测试 : 2022/3/30 IronHammer_Std                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * 内容：                                                             
 *   namespace Timer -- 计时器所在命名空间       
 *     inttime_t (在 types.h) -- 时间刻类型
 *     timer_t -- 时间点类型                            
 *     dura_t -- 时间段类型                                                 
 *     ClocksPerSec -- 获得一秒的时间段长度                               
 *     ClocksPerMSec -- 获得一毫秒的时间段长度                               
 *     SecondToTick -- 用秒数生成时间刻
 *     MilliToTick -- 用毫秒数生成时间刻
 *     ToDuration -- 用时间刻生成时间段
 *     ToTimeInt -- 用时间段生成时间刻
 *     ClockNow -- 获取当前时间点
 *     SetSpeedRatio -- 设置速度倍率（> 1 = 加速 ，< 1 = 减速）
 *     mode_t -- 计时器种类（正计时、倒计时）
 *     TimerClass -- 计时器类
 *       Set -- 重设运行参数
 *       Reset -- 计时器复位
 *       Pause -- 暂停计时
 *       Resume -- 继续计时
 *       GetClock -- 获取计时器的时间段（正计时 = 计时时间 ， 倒计时 = 剩余时间）
 *       GetSecond -- 获取计时器秒数（正计时 = 计时时间 ， 倒计时 = 剩余时间）
 *       GetMilli -- 获取计时器毫秒数（正计时 = 计时时间 ， 倒计时 = 剩余时间）
 *       TimeUp -- 是否时间到了
 *       BeginTime -- 获取起始时间点
 *       CountLength -- 获取（倒计时）时间段长度
 *       PauseMode -- 是否正在暂停
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "Minimal.h"
#include <ratio>
#include <chrono>

namespace ETimer
{
	typedef std::chrono::time_point<std::chrono::steady_clock> timer_t;
	typedef std::chrono::steady_clock::duration dura_t;
	const dura_t __DurationZero = dura_t::zero();
	const inttime_t ClockTicksPerSec = std::chrono::steady_clock::duration::period::den / (std::chrono::steady_clock::duration::period::num);
	const inttime_t ClockTicksPerMSec = ClockTicksPerSec / 1000;

    constexpr dura_t ClocksPerSec() { return dura_t((inttime_t)((double)ClockTicksPerSec)); }
    constexpr dura_t ClocksPerMSec() { return dura_t((inttime_t)((double)ClockTicksPerMSec)); }
	constexpr inttime_t SecondToTick(const inttime_t _clock) { return _clock * ClockTicksPerSec; }
	constexpr inttime_t MilliToTick(const inttime_t _clock) { return _clock * ClockTicksPerMSec; }
	constexpr dura_t ToDuration(const inttime_t _clock) { return dura_t(_clock); }
	constexpr inttime_t ToTimeInt(const dura_t& _clock) { return (inttime_t)_clock.count(); }
    inline timer_t ClockNow() { return std::chrono::steady_clock::now(); }

	enum mode_t
	{
		T_COUNT,
		T_COUNTDOWN
	};
	class TimerClass
	{
		timer_t Begin, RBegin;
		timer_t PauseTime;
		dura_t Length;
		mode_t Mode;
		bool InPause;//sizeof TimerClass = 32
		void ReleasePause(timer_t To)
		{
			Begin += To - PauseTime;
			PauseTime = To;
		}
	public:
		//CONSTRUCTOR
		TimerClass(timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNT;
			Length = __DurationZero;
			PauseTime = ClockNow();
			InPause = false;
		}
		TimerClass(dura_t _Length, timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNTDOWN;
			Length = _Length;
			PauseTime = ClockNow();
			InPause = false;
		}
		//ACTION
		void Set(timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNT;
			Length = __DurationZero;
			PauseTime = ClockNow();
			InPause = false;
		}
		void Set(dura_t _Length, timer_t _Begin = ClockNow())
		{
			Begin = RBegin = _Begin;
			Mode = T_COUNTDOWN;
			Length = _Length;
			PauseTime = ClockNow();
			InPause = false;
		}
		void Reset()
		{
			Begin = RBegin = ClockNow();
			PauseTime = ClockNow();
			InPause = false;
		}
		void Pause()
		{
			if (InPause)return;
			PauseTime = ClockNow();
			InPause = true;
		}
		void Resume()
		{
			if (!InPause)return;
			timer_t Now = ClockNow();
			ReleasePause(Now);
			PauseTime = Now;
			InPause = false;
		}
		//QUERY
		dura_t GetClock()
		{
			timer_t Now = ClockNow();
			if (InPause)
			{
				ReleasePause(Now);
			}
			if (Mode == T_COUNT)
			{
				return (Now - Begin);
			}
			else if (Mode == T_COUNTDOWN)
			{
				return (Begin + Length) - Now;
			}
			else return __DurationZero;
		}
		inttime_t GetSecond()
		{
			return (inttime_t)((double)GetClock().count() / (double)ClockTicksPerSec);
		}
		inttime_t GetMilli()
		{
			return (inttime_t)(((double)GetClock().count() - (double)Length.count()) / (double)ClockTicksPerMSec);
		}
		bool TimeUp()
		{
			if (Mode == T_COUNTDOWN)
			{
				return ((inttime_t)((double)GetClock().count() - (double)Length.count()) <= 0);
			}
			else return false;
		}
		timer_t BeginTime() { return RBegin; }
		dura_t CountLength() { return Length; }
		bool PauseMode() { return InPause; }
	};

    class RateClass :private TimerClass
    {
        dura_t Duration;
    public:
        RateClass(inttime_t Rate) :Duration(ClockTicksPerSec / Rate), TimerClass(dura_t(ClockTicksPerSec / Rate))
        {

        }
        bool NextFrame()
        {
            if (TimeUp())
            {
                Set(Duration);
                return true;
            }
            else return false;
        }
    };

    class StrictRateClass
    {
        inttime_t RateInt;
        inttime_t Count;
        timer_t Begin;
    public:
        StrictRateClass(inttime_t Rate) :RateInt(Rate), Count(0), Begin(ClockNow()) {}
        bool NextFrame()
        {
            if (Count * ClockTicksPerSec < (ClockNow() - Begin).count() * RateInt)
            {
                ++Count; return true;
            }
            else return false;
        }
    };
}

using ETimer::TimerClass;
using ETimer::RateClass;
using ETimer::StrictRateClass;
