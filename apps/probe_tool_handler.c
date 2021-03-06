

/*================================================================
 *
 *
 *   文件名称：probe_tool_handler.c
 *   创 建 者：肖飞
 *   创建日期：2020年03月20日 星期五 12时48分07秒
 *   修改日期：2021年09月11日 星期六 17时55分45秒
 *   描    述：
 *
 *================================================================*/
#include "probe_tool_handler.h"

#include <string.h>

#include "lwip/netdb.h"
#include "lwip/inet.h"

#include "net_client.h"
#include "flash.h"
#include "iap.h"
#include "app.h"
#include "ftp_client.h"
#include "hw_rtc.h"
#include "channels.h"

#include "sal_hook.h"

#define LOG_UDP
#include "log.h"

static void fn1(request_t *request)
{
	probe_server_chunk_sendto(request->payload.fn, (void *)0x8000000, 512);
}

#include "test_event.h"
static void fn2(request_t *request)
{
	try_get_test_event();
}

static void fn3(request_t *request)
{
	static uint32_t file_crc32 = 0;

	uint32_t data_size = request->header.data_size;
	uint32_t data_offset = request->header.data_offset;
	uint32_t total_size = request->header.total_size;
	uint32_t stage = request->payload.stage;
	uint8_t *data = (uint8_t *)(request + 1);
	uint8_t start_upgrade_app = 0;

	if(stage == 0) {
		flash_erase_sector(IAP_CONST_FW_ADDRESS_START_SECTOR, IAP_CONST_FW_ADDRESS_SECTOR_NUMBER);
	} else if(stage == 1) {
		if(data_size == 4) {
			uint32_t *p = (uint32_t *)data;
			file_crc32 = *p;
		}
	} else if(stage == 2) {
		flash_write(IAP_CONST_FW_ADDRESS + data_offset, data, data_size);

		if(data_offset + data_size == total_size) {
			uint32_t read_offset = 0;
			uint32_t crc32 = 0;

			while(read_offset < total_size) {
				uint32_t i;
				uint32_t left = total_size - read_offset;
				uint32_t read_size = (left > 32) ? 32 : left;
				uint8_t *read_buffer = (uint8_t *)(IAP_CONST_FW_ADDRESS + read_offset);

				for(i = 0; i < read_size; i++) {
					crc32 += read_buffer[i];
				}

				read_offset += read_size;
			}

			_printf("crc32:%x, file_crc32:%x\n", crc32, file_crc32);

			if(crc32 == file_crc32) {
				start_upgrade_app = 1;
			}
		}
	}

	loopback(request);

	if(start_upgrade_app != 0) {
		_printf("start upgrade app!\n");

		if(set_firmware_size(total_size) != 0) {
			debug("");
		}

		if(set_firmware_valid(1) != 0) {
			debug("");
		}

		HAL_NVIC_SystemReset();
	}
}

static int p_host(struct hostent *ent)
{
	int ret = 0;
	char **cp;

	if(ent == NULL) {
		ret = -1;
		return ret;
	}

	_printf("\n");

	_printf("h_name:%s\n", ent->h_name);
	_printf("h_aliases:\n");
	cp = ent->h_aliases;

	while(*cp != NULL) {
		_printf("%s\n", *cp);
		cp += 1;

		if(*cp != NULL) {
			//_printf(", ");
		}
	}

	_printf("h_addrtype:%d\n", ent->h_addrtype);

	_printf("h_length:%d\n", ent->h_length);

	_printf("h_addr_list:\n");
	cp = ent->h_addr_list;

	while(*cp != NULL) {
		_printf("%s\n", inet_ntoa(**cp));
		cp += 1;

		if(*cp != NULL) {
			//_printf(", ");
		}
	}

	return ret;
}

static void get_host_by_name(char *content, uint32_t size)
{
	struct hostent *ent;
	char *hostname = (char *)os_alloc(RECV_BUFFER_SIZE);
	int ret;
	int fn;
	int catched;

	//_hexdump("content", (const char *)content, size);

	if(hostname == NULL) {
		return;
	}

	hostname[0] = 0;

	ret = sscanf(content, "%d %s%n", &fn, hostname, &catched);

	if(ret == 2) {
		_printf("hostname:%s!\n", hostname);
		ent = gethostbyname(hostname);
		p_host(ent);
	} else {
		_printf("no hostname!\n");
	}

	os_free(hostname);
}

static void fn4(request_t *request)
{
	const ip_addr_t *local_ip = get_default_ipaddr();
	_printf("local host ip:%s\n", inet_ntoa(*local_ip));

	get_host_by_name((char *)(request + 1), request->header.data_size);
	memset(request, 0, RECV_BUFFER_SIZE);
}

uint16_t osGetCPUUsage(void);
static void fn5(request_t *request)
{
	int size = xPortGetFreeHeapSize();
	uint8_t *os_thread_info;
	uint32_t ticks = osKernelSysTick();
	uint16_t cpu_usage = osGetCPUUsage();
	size_t total_heap_size = get_total_heap_size();
	size_t heap_size;
	size_t heap_count;
	size_t heap_max_size;

	get_mem_info(&heap_size, &heap_count,  &heap_max_size);

	_printf("cpu usage:%d\n", cpu_usage);
	_printf("free os heap size:%d\n", size);
	_printf("total heap size:%d, free heap size:%d, used:%d, heap count:%d, max heap size:%d\n",
	        total_heap_size,
	        total_heap_size - heap_size,
	        heap_size,
	        heap_count,
	        heap_max_size);
	_printf("current ticks:%lu\n", ticks);
	_printf("%lu day %lu hour %lu min %lu sec\n",
	        ticks / (1000 * 60 * 60 * 24),//day
	        (ticks % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60),//hour
	        (ticks % (1000 * 60 * 60)) / (1000 * 60),//min
	        (ticks % (1000 * 60)) / (1000)//sec
	       );

	if(size < 4 * 1024) {
		return;
	}

	size = 1024;

	os_thread_info = (uint8_t *)os_alloc(size);

	if(os_thread_info == NULL) {
		return;
	}

	osThreadList(os_thread_info);

	_puts((const char *)os_thread_info);

	os_free(os_thread_info);

	if(is_app() == 1) {
		_printf("in app!\n");
	} else {
		_printf("in bootloader!\n");
	}
}

static void fn6(request_t *request)
{
	char *content = (char *)(request + 1);
	char protocol[8];
	int fn;
	int catched;
	int ret = 0;
	net_client_info_t *net_client_info = get_net_client_info();

	if(net_client_info == NULL) {
		return;
	}

	set_client_state(net_client_info, CLIENT_SUSPEND);

	ret = sscanf(content, "%d %7s%n", &fn, protocol, &catched);

	if(ret == 2) {
		app_info_t *app_info = get_app_info();

		OS_ASSERT(app_info != NULL);

		_printf("protocol:%s!\n", protocol);

		if(strcmp(protocol, "default") == 0) {
			app_info->mechine_info.request_type = REQUEST_TYPE_DEFAULT;
			app_save_config();
		} else if(strcmp(protocol, "sse") == 0) {
			app_info->mechine_info.request_type = REQUEST_TYPE_SSE;
			app_save_config();
		} else if(strcmp(protocol, "ocpp") == 0) {
			app_info->mechine_info.request_type = REQUEST_TYPE_OCPP_1_6;
			app_save_config();
		} else {
			app_info->mechine_info.request_type = REQUEST_TYPE_NONE;
			app_save_config();
		}
	} else {
		_printf("no protocol!\n");
	}

	set_client_state(net_client_info, CLIENT_REINIT);
}

#include "test_storage.h"
static void fn7(request_t *request)
{
	char *content = (char *)(request + 1);
	int fn;
	int op;
	int start;
	int size;
	int catched;
	int ret;

	ret = sscanf(content, "%d %d %d %d%n", &fn, &op, &start, &size, &catched);

	if(ret == 4) {
		app_info_t *app_info = get_app_info();

		OS_ASSERT(app_info->storage_info != NULL);

		switch(op) {
			case 0: {
				test_storage_check(app_info->storage_info, start, size);
			}
			break;

			case 1: {
				test_storage_read(app_info->storage_info, start, size);
			}
			break;

			case 2: {
				test_storage_write(app_info->storage_info, start, size);
			}
			break;

			default: {
			}
			break;
		}

	}
}

static void fn8(request_t *request)
{
}

static void fn9(request_t *request)
{
}

//#include "test_https.h"
//void set_connect_enable(uint8_t enable);
static void fn10(request_t *request)
{
	//char *url = "https://httpbin.org/get";
	//char *url = "ws://192.168.41.2:8080/ocpp/";
	//char *url = "ws://47.244.218.210:8080/OCPP/echoSocket/13623";
	//char *url = "wss://35.201.125.176:433/SSECHINAEVSE";
	//char *url = "https://216.58.199.110";
	//char *url = "wss://ocpp-16-json.dev-plugitcloud.com/SSECHINAEVSE";
	//char *url = "wss://iot-ebus-ocpp-v16-server-test.azurewebsites.net/ws/test123";
	//test_https(url);
	//set_connect_enable(1);
}

//http://coolaf.com/tool/chattest
//11 0 ws://82.157.123.54:9010/ajaxchattest
//11 0 wss://echo.websocket.org
//11 0 tcp://112.74.40.227:12345
//11 0 udp://112.74.40.227:12345
static void fn11(request_t *request)
{
	app_info_t *app_info = get_app_info();
	char *content = (char *)(request + 1);
	int fn;
	int catched;
	int ret;
	net_client_info_t *net_client_info = get_net_client_info();
	mechine_info_t *buffer = (mechine_info_t *)os_alloc(sizeof(mechine_info_t));

	if(buffer == NULL) {
		return;
	}

	if(net_client_info != NULL) {
		set_client_state(net_client_info, CLIENT_SUSPEND);
	}

	ret = sscanf(content, "%d %s %s %n", &fn, buffer->device_id, buffer->uri, &catched);

	if(ret == 3) {
		strcpy(app_info->mechine_info.device_id, buffer->device_id);
		strcpy(app_info->mechine_info.uri, buffer->uri);
		app_save_config();
	}

	os_free(buffer);

	debug("device id:\'%s\', server uri:\'%s\'!", app_info->mechine_info.device_id, app_info->mechine_info.uri);

	if(net_client_info != NULL) {
		set_client_state(net_client_info, CLIENT_REINIT);
	}
}

//12 10.42.0.1 2121 /user.mk anonymous
//12 10.42.0.1 2121 /user.mk user pass
//12 ftp.gnu.org 21 /gnu/tar/tar-1.32.tar.gz anonymous
//12 ftp.sjtu.edu.cn 21 /centos/2/centos2-scripts-v1.tar anonymous
static void fn12(request_t *request)
{
	char *content = (char *)(request + 1);
	int fn;
	int catched;
	int ret;
	ftp_server_path_t *ftp_server_path = (ftp_server_path_t *)os_alloc(sizeof(ftp_server_path_t));

	if(ftp_server_path == NULL) {
		return;
	}

	memset(ftp_server_path, 0, sizeof(ftp_server_path_t));

	ret = sscanf(content, "%d %s %s %s %s %s %n", &fn, ftp_server_path->host, ftp_server_path->port, ftp_server_path->path, ftp_server_path->user, ftp_server_path->password, &catched);

	debug("ret:%d", ret);

	if((ret == 6) || (ret == 5)) {
		debug("server host:\'%s\', server port:\'%s\', path\'%s\', user:\'%s\', password\'%s\'", ftp_server_path->host, ftp_server_path->port, ftp_server_path->path, ftp_server_path->user, ftp_server_path->password);
		request_ftp_client_download(ftp_server_path->host, ftp_server_path->port, ftp_server_path->path, ftp_server_path->user, ftp_server_path->password, FTP_CLIENT_ACTION_DOWNLOAD, NULL, NULL);
	}

	os_free(ftp_server_path);
}

static void fn13(request_t *request)
{
	char *content = (char *)(request + 1);
	int fn;
	int catched;
	int ret;
	struct tm tm;
	time_t ts;

	ret = sscanf(content, "%d %04d%02d%02d%02d%02d%02d %n",
	             &fn,
	             &tm.tm_year,
	             &tm.tm_mon,
	             &tm.tm_mday,
	             &tm.tm_hour,
	             &tm.tm_min,
	             &tm.tm_sec,
	             &catched);
	debug("ret:%d", ret);
	tm.tm_year -= 1900;
	tm.tm_mon -= 1;
	ts = mktime(&tm);

	if(ret == 7) {
		if(set_time(ts) == 0) {
			debug("set time successful!");
		} else {
			debug("set time failed!");
		}
	}

	ts = get_time();
	tm = *localtime(&ts);
	debug("tm %04d-%02d-%02d %02d:%02d:%02d",
	      tm.tm_year + 1900,
	      tm.tm_mon + 1,
	      tm.tm_mday,
	      tm.tm_hour,
	      tm.tm_min,
	      tm.tm_sec);
}

static void fn14(request_t *request)
{
	char *content = (char *)(request + 1);
	int fn;
	int channel_id;
	int type;//channel_event_type_t
	int catched;
	int ret;

	ret = sscanf(content, "%d %d %d %n",
	             &fn,
	             &channel_id,
	             &type,
	             &catched);
	debug("ret:%d", ret);

	if(ret == 3) {
		channel_event_t *channel_event = os_calloc(1, sizeof(channel_event_t));
		channels_event_t *channels_event = os_calloc(1, sizeof(channels_event_t));
		channels_info_t *channels_info = get_channels();
		channel_info_t *channel_info = channels_info->channel_info + channel_id;

		OS_ASSERT(channel_event != NULL);
		OS_ASSERT(channels_event != NULL);

		channel_info->channel_event_start_display.charge_mode = CHANNEL_RECORD_CHARGE_MODE_UNLIMIT;
		channel_info->channel_event_start_display.start_reason = CHANNEL_RECORD_ITEM_START_REASON_BMS;

		channel_event->channel_id = channel_id;
		channel_event->type = type;
		channel_event->ctx = &channel_info->channel_event_start_display;

		channels_event->type = CHANNELS_EVENT_CHANNEL;
		channels_event->event = channel_event;

		if(send_channels_event(channels_info, channels_event, 100) != 0) {
			debug("send channel %d type %d failed!", channel_id, type);
		} else {
			debug("send channel %d type %d successful!", channel_id, type);
		}
	}
}

#include "voice.h"
static void fn15(request_t *request)
{
	char *content = (char *)(request + 1);
	int fn;
	int voice_id;
	int catched;
	int ret;

	ret = sscanf(content, "%d %d %n",
	             &fn,
	             &voice_id,
	             &catched);
	debug("ret:%d", ret);

	if(ret == 2) {
		channels_info_t *channels_info = get_channels();
		channels_config_t *channels_config = channels_info->channels_config;
		voice_info_t *voice_info = get_or_alloc_voice_info(channels_config);

		if(voice_info != NULL) {
			debug("voice_id:%d!", voice_id);
			request_voice(voice_info, voice_id);
		}
	}
}

extern RTC_HandleTypeDef hrtc;
static void fn16(request_t *request)
{
	char *content = (char *)(request + 1);
	int fn;
	int data;
	int catched;
	int ret;

	ret = sscanf(content, "%d %d %n",
	             &fn,
	             &data,
	             &catched);
	debug("ret:%d", ret);

	if(ret == 2) {
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, data);
		HAL_NVIC_SystemReset();
	} else {
		debug("read data:%d", HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0));
	}
}

static void fn17(request_t *request)
{
	char *content = (char *)(request + 1);
	int fn;
	int onoff;
	int catched;
	int ret;

	ret = sscanf(content, "%d %d %n",
	             &fn,
	             &onoff,
	             &catched);
	debug("ret:%d", ret);

	if(ret == 2) {
		if(onoff == 1) {
			HAL_GPIO_WritePin(kl1_GPIO_Port, kl1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(kn1_GPIO_Port, kn1_Pin, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(kl1_GPIO_Port, kl1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(kn1_GPIO_Port, kn1_Pin, GPIO_PIN_RESET);
		}
	} else {
		debug("");
	}
}

static server_item_t server_map[] = {
	{1, fn1},
	{2, fn2},
	{3, fn3},
	{4, fn4},
	{5, fn5},
	{6, fn6},
	{7, fn7},
	{8, fn8},
	{9, fn9},
	{10, fn10},
	{11, fn11},
	{12, fn12},
	{13, fn13},
	{14, fn14},
	{15, fn15},
	{16, fn16},
	{17, fn17},
};

server_map_info_t server_map_info = {
	.server_map = server_map,
	.server_map_size = sizeof(server_map) / sizeof(server_item_t),
};
