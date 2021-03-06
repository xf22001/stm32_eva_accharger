#
#
#================================================================
#   
#   
#   文件名称：user.mk
#   创 建 者：肖飞
#   创建日期：2019年10月25日 星期五 13时04分38秒
#   修改日期：2021年10月10日 星期日 18时10分26秒
#   描    述：
#
#================================================================

include sal/sal.mk
include config.mk

ifndef_any_of = $(filter undefined,$(foreach v,$(1),$(origin $(v))))
ifdef_any_of = $(filter-out undefined,$(foreach v,$(1),$(origin $(v))))

USER_C_INCLUDES += -Iapps
USER_C_INCLUDES += -Iapps/modules
USER_C_INCLUDES += -Iapps/modules/os
USER_C_INCLUDES += -Iapps/modules/drivers
USER_C_INCLUDES += -Iapps/modules/hardware
USER_C_INCLUDES += -Iapps/modules/app
USER_C_INCLUDES += -Iapps/modules/app/charger
USER_C_INCLUDES += -Iapps/modules/app/power_modules
USER_C_INCLUDES += -Iapps/modules/app/vfs_disk
USER_C_INCLUDES += -Iapps/modules/app/net_client
USER_C_INCLUDES += -Iapps/modules/tests
USER_C_INCLUDES += -IcJSON

USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/system
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/netif/ppp
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/lwip
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/lwip/apps
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/lwip/priv
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/lwip/prot
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/netif
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/compat/posix
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/compat/posix/arpa
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/compat/posix/net
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/compat/posix/sys
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/src/include/compat/stdc
USER_C_INCLUDES += -IMiddlewares/Third_Party/LwIP/system/arch

C_INCLUDES += $(USER_C_INCLUDES)

USER_C_SOURCES += apps/os_memory.c
USER_C_SOURCES += apps/os_random.c
USER_C_SOURCES += apps/early_sys_callback.c
USER_C_SOURCES += apps/usbh_user_callback.c
USER_C_SOURCES += apps/app.c
USER_C_SOURCES += apps/uart_debug_handler.c
USER_C_SOURCES += apps/probe_tool_handler.c
USER_C_SOURCES += apps/channels_config.c
USER_C_SOURCES += apps/can_config.c
USER_C_SOURCES += apps/wiznet_spi.c
USER_C_SOURCES += apps/storage_config.c
USER_C_SOURCES += apps/channels_addr_handler.c
USER_C_SOURCES += apps/display_cache.c
USER_C_SOURCES += apps/local_time.c
USER_C_SOURCES += apps/channels_notify_voice.c

USER_C_SOURCES += apps/modules/app/config_utils.c
USER_C_SOURCES += apps/modules/app/poll_loop.c
USER_C_SOURCES += apps/modules/app/request.c
USER_C_SOURCES += apps/modules/app/probe_tool.c
USER_C_SOURCES += apps/modules/app/uart_debug.c
USER_C_SOURCES += apps/modules/app/file_log.c
USER_C_SOURCES += apps/modules/app/net_client/net_client.c
ifdef CONFIG_NET_CLIENT_PROTOCOL_UDP
USER_C_SOURCES += apps/modules/app/net_client/net_protocol_udp.c
endif
ifdef CONFIG_NET_CLIENT_PROTOCOL_TCP
USER_C_SOURCES += apps/modules/app/net_client/net_protocol_tcp.c
endif
ifdef CONFIG_NET_CLIENT_PROTOCOL_TLS
USER_C_SOURCES += apps/modules/app/net_client/net_protocol_tls.c
endif
ifdef CONFIG_NET_CLIENT_REQUEST_DEFAULT
USER_C_SOURCES += apps/modules/app/net_client/request_default.c
endif
ifdef CONFIG_NET_CLIENT_REQUEST_SSE
USER_C_SOURCES += apps/modules/app/net_client/request_sse.c
endif
ifdef CONFIG_NET_CLIENT_REQUEST_OCCP_1_6
USER_C_SOURCES += apps/modules/app/net_client/test_https.c
USER_C_SOURCES += apps/modules/app/net_client/https.c
USER_C_SOURCES += apps/modules/app/net_client/websocket.c
USER_C_SOURCES += apps/modules/app/net_client/request_ocpp_1_6.c
endif
USER_C_SOURCES += apps/modules/app/ftp_client.c
USER_C_SOURCES += apps/modules/app/vfs_disk/vfs.c
USER_C_SOURCES += apps/modules/app/mt_file.c
USER_C_SOURCES += apps/modules/app/can_data_task.c
USER_C_SOURCES += apps/modules/app/uart_data_task.c
USER_C_SOURCES += apps/modules/app/duty_cycle_pattern.c
USER_C_SOURCES += apps/modules/app/usbh_user_callback.c
USER_C_SOURCES += apps/modules/app/early_sys_callback.c
USER_C_SOURCES += apps/modules/app/connect_state.c
USER_C_SOURCES += apps/modules/app/can_command.c
USER_C_SOURCES += apps/modules/app/usb_upgrade.c
USER_C_SOURCES += apps/modules/app/ntc_temperature.c
USER_C_SOURCES += apps/modules/app/display.c
USER_C_SOURCES += apps/modules/app/voice.c
USER_C_SOURCES += apps/modules/app/charger/channels.c
USER_C_SOURCES += apps/modules/app/charger/channel.c
USER_C_SOURCES += apps/modules/app/charger/channel_handler_native.c
ifdef CONFIG_CHARGER_CHANNEL_PROXY_REMOTE
USER_C_SOURCES += apps/modules/app/charger/channel_handler_proxy_remote.c
endif
ifdef CONFIG_CHARGER_CHANNEL_PROXY_LOCAL
USER_C_SOURCES += apps/modules/app/charger/channel_handler_proxy_local.c
endif
ifneq ($(call ifdef_any_of,CONFIG_CHARGER_CHANNEL_PROXY_REMOTE CONFIG_CHARGER_CHANNEL_PROXY_LOCAL),)
USER_C_SOURCES += apps/modules/app/charger/channels_comm_proxy.c
endif
USER_C_SOURCES += apps/modules/app/charger/charger.c
USER_C_SOURCES += apps/modules/app/charger/charger_bms.c
ifdef CONFIG_CHARGER_BMS_HANDLER_GB
USER_C_SOURCES += apps/modules/app/charger/charger_bms_gb.c
endif
ifdef CONFIG_CHARGER_BMS_HANDLER_AC
USER_C_SOURCES += apps/modules/app/charger/charger_bms_ac.c
endif
USER_C_SOURCES += apps/modules/app/charger/channels_power_module.c
ifdef CONFIG_CHANNELS_POWER_MODULES_NATIVE
USER_C_SOURCES += apps/modules/app/charger/channels_power_module_native.c
endif
USER_C_SOURCES += apps/modules/app/charger/energy_meter.c
USER_C_SOURCES += apps/modules/app/charger/energy_meter_handler_dc.c
USER_C_SOURCES += apps/modules/app/charger/energy_meter_handler_ac.c
USER_C_SOURCES += apps/modules/app/charger/energy_meter_handler_ac_hlw8032.c
USER_C_SOURCES += apps/modules/app/charger/energy_meter_handler_ac_sdm_220.c
USER_C_SOURCES += apps/modules/app/charger/energy_meter_handler_ac_sdm_630.c
USER_C_SOURCES += apps/modules/app/charger/channel_record.c
USER_C_SOURCES += apps/modules/app/charger/card_reader.c
USER_C_SOURCES += apps/modules/app/charger/card_reader_handler_pseudo.c
USER_C_SOURCES += apps/modules/app/charger/card_reader_handler_zlg.c
USER_C_SOURCES += apps/modules/app/charger/card_reader_handler_mt_318_626.c
USER_C_SOURCES += apps/modules/app/charger/card_reader_handler_mt_318_628.c
ifdef CONFIG_CHARGER_CHANNEL_PROXY_REMOTE
USER_C_SOURCES += apps/modules/app/charger/channels_comm_proxy_remote.c
endif
ifdef CONFIG_CHARGER_CHANNEL_PROXY_LOCAL
USER_C_SOURCES += apps/modules/app/charger/channels_comm_proxy_local.c
endif
USER_C_SOURCES += apps/modules/hardware/flash.c
USER_C_SOURCES += apps/modules/hardware/dlt_645_master_txrx.c
USER_C_SOURCES += apps/modules/hardware/hw_rtc.c
USER_C_SOURCES += apps/modules/hardware/hw_adc.c
USER_C_SOURCES += apps/modules/hardware/modbus_slave_txrx.c
USER_C_SOURCES += apps/modules/hardware/modbus_master_txrx.c
USER_C_SOURCES += apps/modules/hardware/modbus_spec.c
USER_C_SOURCES += apps/modules/hardware/storage.c
ifdef CONFIG_STORAGE_25LC1024
USER_C_SOURCES += apps/modules/hardware/storage_25lc1024.c
endif
ifdef CONFIG_STORAGE_24LC128
USER_C_SOURCES += apps/modules/hardware/storage_24lc128.c
endif
ifdef CONFIG_STORAGE_W25Q256
USER_C_SOURCES += apps/modules/hardware/storage_w25q256.c
endif
USER_C_SOURCES += apps/modules/drivers/spi_txrx.c
USER_C_SOURCES += apps/modules/drivers/can_txrx.c
USER_C_SOURCES += apps/modules/drivers/can_ops_hal.c
USER_C_SOURCES += apps/modules/drivers/usart_txrx.c
USER_C_SOURCES += apps/modules/os/event_helper.c
USER_C_SOURCES += apps/modules/os/callback_chain.c
USER_C_SOURCES += apps/modules/os/bitmap_ops.c
USER_C_SOURCES += apps/modules/os/iap.c
USER_C_SOURCES += apps/modules/os/os_utils.c
USER_C_SOURCES += apps/modules/os/net_utils.c
USER_C_SOURCES += apps/modules/os/cpu_utils.c
USER_C_SOURCES += apps/modules/os/log.c
USER_C_SOURCES += apps/modules/os/object_class.c
USER_C_SOURCES += apps/modules/os/soft_timer.c
USER_C_SOURCES += apps/modules/tests/test_serial.c
USER_C_SOURCES += apps/modules/tests/test_event.c
USER_C_SOURCES += apps/modules/tests/test_storage.c
USER_C_SOURCES += Middlewares/Third_Party/LwIP/src/core/def.c
USER_C_SOURCES += Middlewares/Third_Party/LwIP/src/core/ipv4/ip4_addr.c
USER_C_SOURCES += Src/net_sockets.c
USER_C_SOURCES += cJSON/cJSON.c

C_SOURCES += $(USER_C_SOURCES)

USER_CFLAGS += -DtraceTASK_SWITCHED_IN=StartIdleMonitor -DtraceTASK_SWITCHED_OUT=EndIdleMonitor
USER_CFLAGS += -DSAL_HOOK
USER_CFLAGS += -DLOG_CONFIG_FILE=\"log_config.h\"
USER_CFLAGS += -DCJSON_API_VISIBILITY -DCJSON_EXPORT_SYMBOLS -DENABLE_LOCALES -Dcjson_EXPORTS

#USER_CFLAGS += -DLOG_DISABLE
#USER_CFLAGS += -DALLOC_TRACE_DISABLE

CFLAGS += $(USER_CFLAGS) $(CONFIG_CFLAGS)

#LDFLAGS += -u _printf_float -Wl,--wrap=srand  -Wl,--wrap=rand
LDFLAGS += -u _printf_float

default: all

IAP_FILE := apps/modules/os/iap.h

define update-iap-include
	if [ -f $(IAP_FILE) ]; then
		touch $(IAP_FILE);
	fi
endef

ifeq ("$(origin APP)", "command line")
build-type := .app.stamps
build-type-invalid := .bootloader.stamps
CFLAGS += -DUSER_APP
LDSCRIPT = STM32F407VGTx_FLASH_APP.ld
#$(info $(shell $(update-iap-include)))
$(info "build app!")
else
build-type := .bootloader.stamps
build-type-invalid := .app.stamps
LDSCRIPT = STM32F407VGTx_FLASH.ld
#$(info $(shell $(update-iap-include)))
$(info "build bootloader!")
endif

$(build-type) :
	-rm $(build-type-invalid)
	$(shell $(update-iap-include))
	touch $@


PHONY += all
PHONY += default

USER_DEPS := sal/sal.mk config.mk $(build-type)

cscope: all
	rm cscope e_cs -rf
	mkdir -p cscope
	#$(silent)tags.sh prepare;
	$(silent)touch dep_files;
	$(silent)for f in $$(find . -type f -name "*.d" 2>/dev/null); do \
		for i in $$(cat "$$f" | sed 's/^.*://g' | sed 's/[\\ ]/\n/g' | sort -h | uniq); do \
			if test "$${i:0:1}" = "/";then \
				echo "$$i" >> dep_files; \
			else \
				readlink -f "$$i" >> dep_files; \
			fi; \
		done; \
	done;
	$(silent)cat dep_files | sort | uniq | sed 's/^\(.*\)$$/\"\1\"/g' >> cscope/cscope.files;
	$(silent)cat dep_files | sort | uniq >> cscope/ctags.files;
	$(silent)rm dep_files
	$(silent)tags.sh cscope;
	$(silent)tags.sh tags;
	$(silent)tags.sh env;

clean: clean-cscope
clean-cscope:
	rm cscope e_cs -rf

firmware:
	python apps/modules/fw.py -f build/eva.bin

