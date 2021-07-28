

/*================================================================
 *
 *
 *   文件名称：channels_addr_handler.c
 *   创 建 者：肖飞
 *   创建日期：2021年07月16日 星期五 14时03分28秒
 *   修改日期：2021年07月28日 星期三 15时49分41秒
 *   描    述：
 *
 *================================================================*/
#include "modbus_data_value.h"
#include "app.h"
#include "channels.h"
#include "channel.h"
#include "charger.h"
#include "display.h"
#include "display_cache.h"
#include "net_client.h"
#include "card_reader.h"
#include "net_client.h"

#include "log.h"

uint16_t osGetCPUUsage(void);
uint16_t display_cache_get_stop_reason(channel_record_item_stop_reason_t reason, uint8_t channel_id);

typedef enum {
	DISPLAY_FAULT_OK = 0, //(0)系统无故障
	DISPLAY_FAULT_TELEMETER_A,//(1)电表1未连接
	DISPLAY_FAULT_TELEMETER_B,//(2)电表2未连接
	DISPLAY_FAULT_CARD_READER,//(3)刷卡板未连接
	DISPLAY_FAULT_AC_U_OVER,//(4)输入过压
	DISPLAY_FAULT_AC_U_BELOW,//(5)输入欠压
	DISPLAY_FAULT_ESTOP,//(6)急停
	DISPLAY_FAULT_DOOR_OPEN,							//(7)门禁故障
} display_fault_type_t;

typedef enum {
	DISPLAY_TIPS_WIN_NONE = 0, //无提示
	DISPLAY_TIPS_WIN_A_S2,//A枪S2未吸合
	DISPLAY_TIPS_WIN_B_S2,//B枪S2未吸合
	DISPLAY_TIPS_WIN_A_GUN,//A枪未插枪
	DISPLAY_TIPS_WIN_B_GUN,//B枪未插枪
	DISPLAY_TIPS_WIN_SYS_ERR,//系统故障
	DISPLAY_TIPS_WIN_A_SWIPE_CARD,//A枪请刷卡
	DISPLAY_TIPS_WIN_B_SWIPE_CARD,//B枪请刷卡
	DISPLAY_TIPS_WIN_NO_USER,//无此用户
	DISPLAY_TIPS_WIN_CARD_LOCKED,//卡锁定
	DISPLAY_TIPS_WIN_PAYMENT_NO,//(10)尚有账单未结清
	DISPLAY_TIPS_WIN_MONEY_LESS,//余额小于预扣款，不允许充电
	DISPLAY_TIPS_WIN_A_START_SUCCESS,
	DISPLAY_TIPS_WIN_B_START_SUCCESS,
	DISPLAY_TIPS_WIN_A_END_SUCCESS,
	DISPLAY_TIPS_WIN_B_END_SUCCESS,//(15)
	DISPLAY_TIPS_WIN_USER_CHECK,
	DISPLAY_TIPS_WIN_LOCKED,//该枪锁定
	DISPLAY_TIPS_WIN_ORDER_NO_EXIST,//预约号不存在
	DISPLAY_TIPS_WIN_ORDER_OTHER,//预约非本桩
	DISPLAY_TIPS_WIN_PASSWORD_ERROR,//(20)密码错误
	DISPLAY_TIPS_WIN_NO_ENOUGH_MONEY,//余额不足
	DISPLAY_TIPS_WIN_CARD_STOP,//卡被禁用
	DISPLAY_TIPS_WIN_CARD_ABOLISH,//卡废除
	DISPLAY_TIPS_WIN_CARD_OTHER,//非本站卡
	DISPLAY_TIPS_WIN_INPUT_PASSWORD,//(25)请输入密码
	DISPLAY_TIPS_WIN_NET_DISCON,//后台未连接
	DISPLAY_TIPS_WIN_REQUIRED_DATA_ERROR,//数据请求失败
	DISPLAY_TIPS_WIN_A_END_SWIPE_CARD,//A枪刷卡结账
	DISPLAY_TIPS_WIN_B_END_SWIPE_CARD,//B枪刷卡结账
	DISPLAY_TIPS_WIN_START_SUCCESS,//(30)开机成功
	DISPLAY_TIPS_WIN_NO_GUN,//无此充电枪
	DISPLAY_TIPS_WIN_32,//(32)卡片数据不对
	DISPLAY_TIPS_WIN_33,//33用户键盘取消操作
	DISPLAY_TIPS_WIN_34,//34用户取消操作
	DISPLAY_TIPS_WIN_35,//35不支持该卡片
	DISPLAY_TIPS_WIN_36,//36主机拒绝交易
	DISPLAY_TIPS_WIN_37,//37卡片拒绝交易
	DISPLAY_TIPS_WIN_38,//38交易超时
	DISPLAY_TIPS_WIN_39,//39无此交易
	DISPLAY_TIPS_WIN_40,//40错误的ETX数据
	DISPLAY_TIPS_WIN_41,//41错误的LRC数据
	DISPLAY_TIPS_WIN_42,//42交易金额过大
	DISPLAY_TIPS_WIN_43,//43取消授权成功（全取消）
	DISPLAY_TIPS_WIN_44,//44取消预授权成功（扣款）
	DISPLAY_TIPS_WIN_45,//45其他错误
	DISPLAY_TIPS_WIN_46,//46请刷卡
	DISPLAY_TIPS_WIN_47,//47预授权失败，请重新开机
	DISPLAY_TIPS_WIN_48,//48开始充电
	DISPLAY_TIPS_WIN_49,//49预授权成功
	DISPLAY_TIPS_WIN_50,//50预授权失败
	DISPLAY_TIPS_WIN_51,//51预授权超时
	DISPLAY_TIPS_WIN_52,//52预授权成功，等待启动中
	DISPLAY_TIPS_WIN_53,//53预授权失败，请重新开机
	DISPLAY_TIPS_WIN_54,//54预授权超时，请重新开机
	DISPLAY_TIPS_WIN_55,//55枪未插好
} display_tips_type_t;

typedef enum {
	DISPLAY_CHARGE_STATUS_IDLE,//待机状态 未插枪
	DISPLAY_CHARGE_STATUS_READY,//就绪状态 插枪未充电
	DISPLAY_CHARGE_STATUS_RUNNING,//正在充电 插枪充电
	DISPLAY_CHARGE_STATUS_FINISH,//充电完成 插枪充完电
} display_charge_status_type_t;

void sync_channel_display_cache(channel_info_t *channel_info);
static void account_request_cb(void *fn_ctx, void *chain_ctx)
{
	channel_info_t *channel_info = (channel_info_t *)fn_ctx;
	account_response_info_t *account_response_info = (account_response_info_t *)chain_ctx;

	switch(account_response_info->code) {
		case ACCOUNT_STATE_CODE_OK: {
			channel_info->display_cache_channel.account_balance = account_response_info->balance;
			channel_info->display_cache_channel.start_reason = CHANNEL_RECORD_ITEM_START_REASON_CARD;
			channel_info->display_cache_channel.charger_start_sync = 1;
			sync_channel_display_cache(channel_info);
		}
		break;

		default: {
		}
		break;
	}
}

static void card_reader_cb_fn(void *fn_ctx, void *chain_ctx)
{
	channel_info_t *channel_info = (channel_info_t *)fn_ctx;
	card_reader_data_t *card_reader_data = (card_reader_data_t *)chain_ctx;

	if(card_reader_data != NULL) {
		net_client_info_t *net_client_info = get_net_client_info();
		account_request_info_t account_request_info = {0};

		if(net_client_info == NULL) {
			return;
		}

		channel_info->display_cache_channel.card_id = card_reader_data->id;

		account_request_info.account_type = ACCOUNT_TYPE_CARD;
		account_request_info.card_id = card_reader_data->id;
		account_request_info.password = (char *)channel_info->display_cache_channel.password;
		account_request_info.fn = account_request_cb;
		account_request_info.fn_ctx = channel_info;
		net_client_net_client_ctrl_cmd(net_client_info, NET_CLIENT_CTRL_CMD_QUERY_ACCOUNT, &account_request_info);
	}
}

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

		case 8 ... 12: {//(8-12) IP
			modbus_data_buffer_rw(modbus_data_ctx, app_info->display_cache_app.ip, 5 * 2, modbus_data_ctx->addr - 8);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				app_info->display_cache_app.ip_sync = 1;
			}
		}
		break;

		case 13 ... 17: {//(13-17)预留IP
		}
		break;

		case 18: {//(18)服务费                             0.01元
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
			modbus_data_buffer_rw(modbus_data_ctx, channels_info->display_cache_channels.price_item_cache, 3 * 20 * 2, modbus_data_ctx->addr - 19);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.price_sync = 1;
			}
		}
		break;

		case 79 ... 94: {//设备编号	ASCII
			modbus_data_buffer_rw(modbus_data_ctx, mechine_info->device_id, 16 * 2, modbus_data_ctx->addr - 79);
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

		case 98: {//预付款	0.01元
			modbus_data_value_rw(modbus_data_ctx, channels_info->display_cache_channels.withholding_l);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.withholding_sync = 1;
			}
		}
		break;

		case 99: {//预付款	0.01元
			modbus_data_value_rw(modbus_data_ctx, channels_info->display_cache_channels.withholding_h);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.withholding_sync = 1;
			}
		}
		break;

		case 100: {//B枪额定电流
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 1;
			modbus_data_value_rw(modbus_data_ctx, channel_info->channel_settings.ac_current_limit);
		}
		break;

		case 101: {//联网方式	0：网口 1：DTU
		}
		break;

		case 102: {//刷卡板类型	0:626 1:628 3：ZLG
			modbus_data_value_rw(modbus_data_ctx, channels_info->channels_settings.card_reader_settings.type);
		}
		break;

		case 103: {//电表设置	0：无 1:645 2：CED
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->channel_settings.energy_meter_settings.type);
		}
		break;

		case 104: {//ocpp  预扣款金额
		}
		break;

		case 105: {//是否有门禁检测,需配合直流桩使用
		}
		break;

		case 106: {//是否需要故障灯；复用2号通道的输出接触器当故障灯使用
		}
		break;

		case 107: {//电表倍率
		}
		break;

		case 108: {//pe检测
			modbus_data_value_rw(modbus_data_ctx, channels_settings->pe_detect_disable);
		}
		break;

		case 110 ... 299: {//预留
		}
		break;

		case 300: {//主板版本号	BCD
			uint16_t value = get_u16_from_u8_lh(get_bcd_from_u8(VER_MINOR), get_bcd_from_u8(VER_MAJOR));
			modbus_data_value_r(modbus_data_ctx, value);
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

		case 301 ... 307: {//系统时间	BCD //todo
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

			modbus_data_buffer_rw(modbus_data_ctx, &channels_info->display_cache_channels.datetime_cache, 7 * 2, modbus_data_ctx->addr - 301);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.datetime_sync = 1;
			}
		}
		break;

		case 308: {//故障状态 display_fault_type_t
			int fault = DISPLAY_FAULT_OK;

			if(fault == DISPLAY_FAULT_OK) {
				channel_info_t *channel_info;
				channel_info = (channel_info_t *)channels_info->channel_info + 0;

				if(get_fault(channel_info->faults, CHANNEL_FAULT_ENERGYMETER) == 1) {
					fault = DISPLAY_FAULT_TELEMETER_A;
				}
			}

			if(fault == DISPLAY_FAULT_OK) {
				channel_info_t *channel_info;
				channel_info = (channel_info_t *)channels_info->channel_info + 1;

				if(get_fault(channel_info->faults, CHANNEL_FAULT_ENERGYMETER) == 1) {
					fault = DISPLAY_FAULT_TELEMETER_B;
				}
			}

			if(fault == DISPLAY_FAULT_OK) {
				if(get_fault(channels_info->faults, CHANNELS_FAULT_CARD_READER) == 1) {
					fault = DISPLAY_FAULT_CARD_READER;
				}
			}

			if(fault == DISPLAY_FAULT_OK) {
				if(get_fault(channels_info->faults, CHANNELS_FAULT_INPUT_OVER_VOLTAGE) == 1) {
					fault = DISPLAY_FAULT_AC_U_OVER;
				}
			}

			if(fault == DISPLAY_FAULT_OK) {
				if(get_fault(channels_info->faults, CHANNELS_FAULT_INPUT_LOW_VOLTAGE) == 1) {
					fault = DISPLAY_FAULT_AC_U_BELOW;
				}
			}

			if(fault == DISPLAY_FAULT_OK) {
				if(get_fault(channels_info->faults, CHANNELS_FAULT_FORCE_STOP) == 1) {
					fault = DISPLAY_FAULT_ESTOP;
				}
			}

			modbus_data_value_r(modbus_data_ctx, fault);
		}
		break;

		case 309: {//输入电压
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_r(modbus_data_ctx, channel_info->va);
		}
		break;

		case 310: {//刷卡板状态
			uint16_t status = 1;

			if(get_fault(channels_info->faults, CHANNELS_FAULT_CARD_READER) == 1) {
				status = 0;
			}

			modbus_data_value_r(modbus_data_ctx, status);
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
			modbus_data_value_rw(modbus_data_ctx, channels_info->display_cache_channels.tips);
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
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_buffer_rw(modbus_data_ctx, channel_info->display_cache_channel.password, 16 * 2, modbus_data_ctx->addr - 318);
		}
		break;

		case 334 ... 349: {//账户名输入区	ASCII
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_buffer_rw(modbus_data_ctx, channel_info->display_cache_channel.account, 16 * 2, modbus_data_ctx->addr - 334);
		}
		break;

		case 366: {//账户余额
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 367: {//账户余额
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 368: {//控制板温度
			int16_t value = channels_info->temperature * 10;
			modbus_data_value_r(modbus_data_ctx, value);
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
			charger_info_t *charger_info = (charger_info_t *)channel_info->charger_info;
			uint8_t charging = 0;
			uint8_t charger_connected = 0;
			uint16_t state = DISPLAY_CHARGE_STATUS_IDLE;

			if(channel_info->state != CHANNEL_STATE_IDLE) {
				charging = 1;
			}

			if(charger_info->charger_connect_state == 1) {
				charger_connected = 1;
			}

			if(charger_connected == 1) {
				if(charging == 1) {
					state = DISPLAY_CHARGE_STATUS_RUNNING;
				} else {
					if(channel_info->state == CHANNEL_STATE_STOPPING) {
						state = DISPLAY_CHARGE_STATUS_FINISH;
					} else {
						state = DISPLAY_CHARGE_STATUS_READY;
					}
				}
			} else {
			}

			modbus_data_value_r(modbus_data_ctx, state);
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

		case 503: {//充电电量低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.energy);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 504: {//充电电量高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.energy);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 505: {//充电时长
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t duration = 0;

			duration = channel_info->channel_record_item.stop_time - channel_info->channel_record_item.start_time;
			modbus_data_value_r(modbus_data_ctx, duration);
		}
		break;

		case 506 ... 521: {//充电账户
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_buffer_rw(modbus_data_ctx, channel_info->channel_record_item.account, 16 * 2, modbus_data_ctx->addr - 506);
		}
		break;

		case 522: {//账户余额低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 523: {//账户余额高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.account_balance);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 524: {//消费金额
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->channel_record_item.amount);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 525: {//消费金额
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->channel_record_item.amount);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 526: {//电量读数低
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_0_from_u32(channel_info->total_energy / 100);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 527: {//电量读数高
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = get_u16_1_from_u32(channel_info->total_energy / 100);
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 528: {//充电电压                   0.1V
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 1;
			modbus_data_value_r(modbus_data_ctx, channel_info->voltage);
		}
		break;

		case 529: {//充电电流                   0.1A
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 1;
			modbus_data_value_r(modbus_data_ctx, channel_info->current);
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

		case 600: {//开关机 0-关机, 1-开机
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.onoff);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				if(channels_settings->authorize == 0) {
					channel_info->display_cache_channel.charger_start_sync = 1;
					channel_info->display_cache_channel.start_reason = CHANNEL_RECORD_ITEM_START_REASON_MANUAL;
				} else {
					card_reader_info_t *card_reader_info = (card_reader_info_t *)channels_info->card_reader_info;
					card_reader_cb_t card_reader_cb;

					card_reader_cb.fn = card_reader_cb_fn;
					card_reader_cb.fn_ctx = channel_info;
					card_reader_cb.timeout = 5000;

					start_card_reader_cb(card_reader_info, &card_reader_cb);
				}
			}
		}
		break;

		case 601: {//插枪状态
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			charger_info_t *charger_info = (charger_info_t *)channel_info->charger_info;
			uint16_t value = 0;

			if(charger_info->charger_connect_state == 1) {
				if(charger_info->vehicle_relay_state == 0) {
					value = 1;
				} else {
					value = 2;
				}
			}

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
			uint16_t value;

			if(channel_info->state != CHANNEL_STATE_IDLE) {
				value = display_cache_get_stop_reason(channel_info->channel_record_item.stop_reason, 0);
			} else {
				value = 0;
			}

			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 604: {//开始充电原因
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = channel_info->channel_record_item.start_reason;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 605: {//枪是否锁定//xiaofei todo
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			uint16_t value = (channel_info->state == CHANNEL_STATE_IDLE) ? 0 : 1;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		case 606: {//枪头温度
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			charger_info_t *charger_info = (charger_info_t *)channel_info->charger_info;
			int16_t value = (charger_info->dc_p_temperature > charger_info->dc_n_temperature) ? charger_info->dc_p_temperature : charger_info->dc_n_temperature;

			if(channel_info->temperature > value) {
				value = channel_info->temperature;
			}

			if(channels_info->temperature > value) {
				value = channels_info->temperature;
			}

			value = value * 10;

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

		//case 610 ... 649: {//预留
		//}
		//break;

		case 650: {//充电模式设置
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.charge_mode);
		}
		break;

		case 651: {//设定充电金额
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.charge_amount);
		}
		break;

		case 652: {//开始充电时间时	BCD
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.start_hour);
		}
		break;

		case 653: {//开始充电时间分	BCD
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.start_min);
		}
		break;

		case 654: {//结束充电时间时	BCD
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.stop_hour);
		}
		break;

		case 655: {//结束充电时间分	BCD
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.stop_min);
		}
		break;

		case 656: {//设定充电电量
			channel_info_t *channel_info = (channel_info_t *)channels_info->channel_info + 0;
			modbus_data_value_rw(modbus_data_ctx, channel_info->display_cache_channel.charge_energy);
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

		//case 900: {//记录查询 年	BCD
		//}
		//break;

		//case 901: {//记录查询 月	BCD
		//}
		//break;

		//case 902: {//记录查询 日	BCD
		//}
		//break;

		case 900 ... 902: {//记录查询 	BCD 年 月 日
			modbus_data_buffer_rw(modbus_data_ctx, &channels_info->display_cache_channels.record_dt_cache, 3 * 2, modbus_data_ctx->addr - 900);
		}
		break;

		case 903: {//查询确认	按键 下发1
			modbus_data_value_rw(modbus_data_ctx, channels_info->display_cache_channels.record_load_cmd);

			if(modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) {
				channels_info->display_cache_channels.record_sync = 1;
			}
		}
		break;

		case 904: {//未上传记录数
			channel_record_task_info_t *channel_record_task_info = get_or_alloc_channel_record_task_info(0);
			modbus_data_value_r(modbus_data_ctx, channel_record_task_info->finish_state_count);
		}
		break;

		case 905: {//记录总数
			channel_record_task_info_t *channel_record_task_info = get_or_alloc_channel_record_task_info(0);
			uint16_t end = channel_record_task_info->channel_record_info.end;
			uint16_t start = channel_record_task_info->channel_record_info.start;
			uint16_t value;

			if(end < start) {
				end += CHANNEL_RECORD_NUMBER;
			}

			value = end - start;
			modbus_data_value_r(modbus_data_ctx, value);
		}
		break;

		//case 906: {//枪号
		//}
		//break;

		//case 907 ... 922: {//账户号	ASCII
		//}
		//break;

		//case 923: {//开始时间时+分 	BCD
		//}
		//break;

		//case 924: {//结束时间时+分	BCD
		//}
		//break;

		//case 925: {//充电量高
		//}
		//break;

		//case 926: {//充电量低
		//}
		//break;

		//case 927: {//消费金额高
		//}
		//break;

		//case 928: {//消费金额低
		//}
		//break;

		//case 929: {//开始充电原因
		//}
		//break;

		//case 930: {//结束充电原因
		//}
		//break;

		case 906 ... 1155: {
			modbus_data_buffer_rw(modbus_data_ctx, channels_info->display_cache_channels.record_item_cache, 25 * 10 * 2, modbus_data_ctx->addr - 906);
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

	//debug("op:%s, addr:%d, value:%d",
	//      (modbus_data_ctx->action == MODBUS_DATA_ACTION_GET) ? "get" :
	//      (modbus_data_ctx->action == MODBUS_DATA_ACTION_SET) ? "set" :
	//      "unknow",
	//      modbus_data_ctx->addr,
	//      modbus_data_ctx->value);
}
