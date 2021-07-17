

/*================================================================
 *
 *
 *   文件名称：channels_addr_handler.c
 *   创 建 者：肖飞
 *   创建日期：2021年07月16日 星期五 14时03分28秒
 *   修改日期：2021年07月17日 星期六 13时38分08秒
 *   描    述：
 *
 *================================================================*/
#include "modbus_data_value.h"
#include "app.h"
#include "channels.h"
#include "channel.h"
#include "charger.h"
#include "display.h"
#include "net_client.h"

#include "log.h"

uint16_t osGetCPUUsage(void);

void channels_modbus_data_action(void *fn_ctx, void *chain_ctx)
{
	channels_info_t *channels_info = (channels_info_t *)fn_ctx;
	channels_settings_t *channels_settings = &channels_info->channels_settings;
	channels_config_t *channels_config = channels_info->channels_config;
	modbus_data_ctx_t *modbus_data_ctx = (modbus_data_ctx_t *)chain_ctx;
	app_info_t *app_info = get_app_info();
	mechine_info_t *mechine_info = &app_info->mechine_info;

	switch(modbus_data_ctx->addr) {
		case 5: {//枪数设置	0：单枪 1：双枪
			modbus_data_value_with_base_rw(modbus_data_ctx, channels_settings->channel_number, -1);

			if(channels_settings->channel_number > channels_config->channel_number) {
				channels_settings->channel_number = channels_config->channel_number;
			}

			channels_info->channel_number = channels_settings->channel_number;
		}
		break;

		case 6: {//是否刷卡	0：否 1：是
			modbus_data_value_rw(modbus_data_ctx, channels_settings->authorize);
		}
		break;

		case 7: {//后台设置	0:无 1：SSE 2:OCPP
			modbus_data_value_rw(modbus_data_ctx, channels_settings->request_type);
		}
		break;

		case 8 ... 11: {//IP	ASCII
			modbus_data_buffer_rw(modbus_data_ctx, app_info->display_cache_app.ip, 4, modbus_data_ctx->addr - 8);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				app_info->display_cache_app.ip_sync = 1;
			}
		}
		break;

		case 12: {//端口号	ASCII//?
			modbus_data_value_rw(modbus_data_ctx, app_info->display_cache_app.port);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				app_info->display_cache_app.ip_sync = 1;
			}
		}
		break;

		case 13 ... 17: {//预留IP	ASCII
		}
		break;

		case 18: {//服务费设置	0.01元
			modbus_data_value_rw(modbus_data_ctx, channels_settings->price_info.service_price);
		}
		break;

		//case 19: {//时间小时	BCD
		//}
		//break;

		//case 20: {//时间分钟	BCD
		//}
		//break;

		//case 21: {//电价	0.01元
		//}
		//break;

		case 19 ... 78: {//计费设置	同上
			modbus_data_buffer_rw(modbus_data_ctx, channels_info->display_cache_channels.price_item_cache, 60, modbus_data_ctx->addr - 19);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.price_sync = 1;
			}
		}
		break;

		case 79 ... 94: {//设备编号	ASCII
			modbus_data_buffer_rw(modbus_data_ctx, mechine_info->device_id, 16, modbus_data_ctx->addr - 79);
		}
		break;

		case 95: {//A枪额定电流	 0：16  1：32  2：63
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->channel_settings.ac_current_limit);
		}
		break;

		case 96: {//是否验证标记	0：否 1：是
		}
		break;

		case 97: {//是否锁卡	0：否 1：是
		}
		break;

		case 98: {//预付款高	0.01元
			modbus_data_value_rw(modbus_data_ctx, channels_info->display_cache_channels.withholding_h);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.withholding_sync = 1;
			}
		}
		break;

		case 99: {//预付款低	0.01元
			modbus_data_value_rw(modbus_data_ctx, channels_info->display_cache_channels.withholding_l);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.withholding_sync = 1;
			}
		}
		break;

		case 100: {//B枪额定电流	1A
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 1;
			modbus_data_value_rw(modbus_data_ctx, channel_info->channel_settings.ac_current_limit);
		}
		break;

		case 101: {//联网方式	0：网口 1：DTU
		}
		break;

		case 102: {//刷卡板类型	0:626 1:628 3：ZLG
		}
		break;

		case 103: {//电表设置	0：无 1:645 2：CED
		}
		break;

		case 104: {//HMI 设置	0：DW 2:BK 3:无
		}
		break;

		case 105: {//交流桩类型	0：单相 1：三相
		}
		break;

		case 106: {//预留
		}
		break;

		case 107: {//PE检测	0：是 1：否
			modbus_data_value_rw(modbus_data_ctx, channels_settings->pe_detect_disable);
		}
		break;

		case 108 ... 109: {//标记设置	ASCII
		}
		break;

		case 110 ... 299: {//预留
		}
		break;

		case 300: {//主板版本号	BCD
		}
		break;

		//case 301: {//系统时间 秒	BCD
		//}
		//break;

		//case 302: {//系统时间 分	BCD
		//}
		//break;

		//case 303: {//系统时间 时	BCD
		//}
		//break;

		//case 304: {//系统时间 日	BCD
		//}
		//break;

		//case 305: {//系统时间 月	BCD
		//}
		//break;

		//case 306: {//系统时间 年	BCD
		//}
		//break;

		//case 307: {//系统时间 周	BCD
		//}
		//break;

		case 301 ... 307: {//系统时间 周	BCD
			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_GET) {
				time_t ts = get_time();
				struct tm *tm = localtime(&ts);
				channels_info->display_cache_channels.datetime_cache.year = tm->tm_year + 1900;
				channels_info->display_cache_channels.datetime_cache.mon = tm->tm_mon + 1;
				channels_info->display_cache_channels.datetime_cache.day = tm->tm_mday;
				channels_info->display_cache_channels.datetime_cache.hour = tm->tm_hour;
				channels_info->display_cache_channels.datetime_cache.min = tm->tm_min;
				channels_info->display_cache_channels.datetime_cache.sec = tm->tm_sec;
				channels_info->display_cache_channels.datetime_cache.wday = tm->tm_wday;
			}

			modbus_data_buffer_rw(modbus_data_ctx, &channels_info->display_cache_channels.datetime_cache, 7, modbus_data_ctx->addr - 301);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.datetime_sync = 1;
			}
		}
		break;

		case 308: {//故障状态
			int fault = get_first_fault(channels_info->faults);
			modbus_data_value_r(modbus_data_ctx, fault);
		}
		break;

		case 309: {//输入电压
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_r(modbus_data_ctx, channel_info->va);
		}
		break;

		case 310: {//刷卡板状态
			int fault = get_fault(channels_info->faults, CHANNELS_FAULT_CARD_READER);
			modbus_data_value_r(modbus_data_ctx, fault);
		}
		break;

		case 311: {//网络状态
			int state = 0;
			net_client_info_t *net_client_info = get_net_client_info();

			if(get_client_state(net_client_info) == CLIENT_CONNECTED) {
				state = 1;
			}

			modbus_data_value_r(modbus_data_ctx, state);
		}
		break;

		case 312: {//系统运行时间	1min
			uint32_t up_time = osKernelSysTick() / 60000;
			modbus_data_value_r(modbus_data_ctx, up_time);
		}
		break;

		case 313: {//CPU使用率	%
			uint16_t cpu_usage = osGetCPUUsage();
			modbus_data_value_r(modbus_data_ctx, cpu_usage);
		}
		break;

		case 314: {//提示地址
		}
		break;

		case 315: {//当前电价	0.01元
			price_info_t *price_info = &channels_info->channels_settings.price_info;
			uint16_t price = price_info->price[get_seg_index_by_ts(get_time())];

			modbus_data_value_r(modbus_data_ctx, price);
		}
		break;

		case 316: {//主板上传信息
		}
		break;

		case 317: {//屏下发消息
		}
		break;

		case 318 ... 333: {//密码输入区	ASCII
		}
		break;

		case 334 ... 349: {//账户名输入区	ASCII
		}
		break;

		case 366: {//账户余额高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 367: {//账户余额低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 368: {//控制板温度
			modbus_data_value_r(modbus_data_ctx, channels_info->temperature);
		}
		break;

		case 369: {//显示屏状态
		}
		break;

		case 370: {//时间校正标记
		}
		break;

		case 371 ... 499: {//预留
		}
		break;

		case 500: {//充电状态
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_r(modbus_data_ctx, channel_info->state);
		}
		break;

		case 501: {//充电电压
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_r(modbus_data_ctx, channel_info->voltage);
		}
		break;

		case 502: {//充电电流
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_r(modbus_data_ctx, channel_info->current);
		}
		break;

		case 503: {//充电电量高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.energy);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 504: {//充电电量低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.energy);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 505: {//充电时长
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t duration = (get_time() - channel_info->channel_record_item.start_time) / 60;
			modbus_data_value_r(modbus_data_ctx, duration);
		}
		break;

		case 506 ... 521: {//充电账户
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_buffer_rw(modbus_data_ctx, channel_info->channel_record_item.account, 16, modbus_data_ctx->addr - 506);
		}
		break;

		case 522: {//账户余额高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 523: {//账户余额低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 524: {//消费金额高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.amount);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 525: {//消费金额低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.amount);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 526: {//电量读数低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->total_energy);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 527: {//电量读数高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->total_energy);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 528: {//充电电压B
		}
		break;

		case 529: {//充电电流B
		}
		break;

		case 530: {//充电电压C
		}
		break;

		case 531: {//充电电流C
		}
		break;

		case 532: {//二维码显示
		}
		break;

		case 533: {//充电功率高
		}
		break;

		case 534: {//充电功率低
		}
		break;

		case 544 ... 599: {//预留
		}
		break;

		case 600: {//开关机状态
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->total_energy);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 601: {//插枪状态
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			charger_info_t *charger_info = (charger_info_t *)channel_info->charger_info;
			uint16_t value = charger_info->charger_connect_state;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 602: {//电表状态
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			int fault = get_fault(channel_info->faults, CHANNEL_FAULT_ENERGYMETER);
			modbus_data_value_r(modbus_data_ctx, fault);
		}
		break;

		case 603: {//停止充电原因
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = channel_info->channel_record_item.stop_reason;//todo
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 604: {//开始充电原因
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = channel_info->channel_record_item.start_reason;//todo
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 605: {//枪是否锁定
		}
		break;

		case 606: {//枪头温度
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			charger_info_t *charger_info = (charger_info_t *)channel_info->charger_info;
			uint16_t value = (charger_info->dc_p_temperature > charger_info->dc_n_temperature) ? charger_info->dc_p_temperature : charger_info->dc_n_temperature;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 607: {//CP电压
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = channel_info->cp_ad_voltage;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 608: {//占空比
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = channel_info->cp_pwm_duty;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 609: {//频率
			uint16_t value = 1000;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		//case 610 ... 699: {//预留
		//}
		//break;

		case 650: {//充电模式设置
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.charge_mode);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channel_info->display_cache_channel.charge_sync = 1;
			}
		}
		break;

		case 651: {//设定充电金额
		}
		break;

		case 652: {//开始充电时间时	BCD
		}
		break;

		case 653: {//开始充电时间分	BCD
		}
		break;

		case 654: {//结束充电时间时	BCD
		}
		break;

		case 655: {//结束充电时间分	BCD
		}
		break;

		case 656: {//设定充电电量
		}
		break;

		case 657: {//设定充电时间
		}
		break;

		case 658 ... 699: {//预留
		}
		break;

		case 700 ... 899: {//B枪
		}
		break;

		case 900: {//记录查询 年	BCD
		}
		break;

		case 901: {//记录查询 月	BCD
		}
		break;

		case 902: {//记录查询 日	BCD
		}
		break;

		case 903: {//查询确认	按键 下发1
		}
		break;

		case 904: {//未上传记录数
		}
		break;

		case 905: {//记录总数
		}
		break;

		case 906: {//枪号
		}
		break;

		case 907 ... 922: {//账户号	ASCII
		}
		break;

		case 923: {//开始时间时+分 	BCD
		}
		break;

		case 924: {//结束时间时+分	BCD
		}
		break;

		case 925: {//充电量高
		}
		break;

		case 926: {//充电量低
		}
		break;

		case 927: {//消费金额高
		}
		break;

		case 928: {//消费金额低
		}
		break;

		case 929: {//开始充电时间
		}
		break;

		case 930: {//结束充电时间
		}
		break;

		default:
			debug("error! op:%s, addr:%d",
			      (modbus_data_ctx->action == MODBUS_DATA_ACTION_GET) ? "get" :
			      (modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) ? "set" :
			      "unknow",
			      modbus_data_ctx->addr);
			break;
	}
}
