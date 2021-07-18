

/*================================================================
 *
 *
 *   文件名称：local_time.c
 *   创 建 者：肖飞
 *   创建日期：2021年07月18日 星期日 16时43分41秒
 *   修改日期：2021年07月18日 星期日 19时23分32秒
 *   描    述：
 *
 *================================================================*/
#include "hw_rtc.h"

time_t get_time(void)
{
	struct tm tm;
	rtc_get_datetime(&tm);
	return mktime(&tm);
}

void set_time(time_t ts)
{
	struct tm *tm;
	tm = localtime(&ts);
	rtc_set_datetime(tm);
}
