#
# Component Makefile
#
ifdef CONFIG_BT_ENABLED

COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := include

LIBS := btdm_app

COMPONENT_ADD_LDFLAGS     := -lbt -L $(COMPONENT_PATH)/lib \
                           $(addprefix -l,$(LIBS))

# re-link program if BT binary libs change
COMPONENT_ADD_LINKER_DEPS := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))

COMPONENT_SUBMODULES += lib

ifeq ($(GCC_NOT_5_2_0), 1)
# TODO: annotate fallthroughs in Bluedroid code with comments
CFLAGS += -Wno-implicit-fallthrough
endif

endif


ifdef CONFIG_BLUEDROID_ENABLED

COMPONENT_PRIV_INCLUDEDIRS +=   bluedroid/bta/include                   \
                                bluedroid/bta/ar/include                \
                                bluedroid/bta/av/include                \
                                bluedroid/bta/hf_client/include         \
                                bluedroid/bta/dm/include                \
                                bluedroid/bta/gatt/include              \
                                bluedroid/bta/hh/include                \
                                bluedroid/bta/jv/include                \
                                bluedroid/bta/sdp/include               \
                                bluedroid/bta/sys/include               \
                                bluedroid/btcore/include                \
                                bluedroid/device/include                \
                                bluedroid/gki/include                   \
                                bluedroid/hci/include                   \
                                bluedroid/osi/include                   \
                                bluedroid/utils/include                 \
                                bluedroid/external/sbc/decoder/include  \
                                bluedroid/external/sbc/encoder/include  \
                                bluedroid/btc/core/include              \
                                bluedroid/btc/profile/esp/blufi/include \
                                bluedroid/btc/profile/esp/include       \
                                bluedroid/btc/profile/std/gatt/include  \
                                bluedroid/btc/profile/std/gap/include   \
                                bluedroid/btc/profile/std/a2dp/include  \
                                bluedroid/btc/profile/std/include       \
                                bluedroid/btc/include                   \
                                bluedroid/btif/include                  \
                                bluedroid/stack/btm/include             \
                                bluedroid/stack/btu/include             \
                                bluedroid/stack/gap/include             \
                                bluedroid/stack/gatt/include            \
                                bluedroid/stack/hcic/include            \
                                bluedroid/stack/l2cap/include           \
                                bluedroid/stack/sdp/include             \
                                bluedroid/stack/smp/include             \
                                bluedroid/stack/avct/include            \
                                bluedroid/stack/avrc/include            \
                                bluedroid/stack/avdt/include            \
                                bluedroid/stack/a2dp/include            \
                                bluedroid/stack/rfcomm/include          \
                                bluedroid/stack/include                 \
                                bluedroid/utils/include                 \
                                bluedroid/common/include

COMPONENT_ADD_INCLUDEDIRS +=    bluedroid/api/include/api

COMPONENT_SRCDIRS +=    bluedroid/bta/dm                      \
                        bluedroid/bta/gatt                    \
                        bluedroid/bta/hh                      \
                        bluedroid/bta/sdp                     \
                        bluedroid/bta/av                      \
                        bluedroid/bta/ar                      \
                        bluedroid/bta/sys                     \
                        bluedroid/bta/jv                      \
                        bluedroid/bta/hf_client               \
                        bluedroid/bta                         \
                        bluedroid/btcore                      \
                        bluedroid/btif                        \
                        bluedroid/device                      \
                        bluedroid/gki                         \
                        bluedroid/hci                         \
                        bluedroid/main                        \
                        bluedroid/osi                         \
                        bluedroid/external/sbc/decoder/srce   \
                        bluedroid/external/sbc/encoder/srce   \
                        bluedroid/btc/core                    \
                        bluedroid/btc/profile/esp/blufi       \
                        bluedroid/btc/profile/std/gap         \
                        bluedroid/btc/profile/std/gatt        \
                        bluedroid/btc/profile/std/a2dp        \
                        bluedroid/btc/profile/std/avrc        \
                        bluedroid/btc/profile/std/spp         \
                        bluedroid/btc/profile/std/hf_client   \
                        bluedroid/btc/profile                 \
                        bluedroid/stack/btm                   \
                        bluedroid/stack/btu                   \
                        bluedroid/stack/gap                   \
                        bluedroid/stack/gatt                  \
                        bluedroid/stack/hcic                  \
                        bluedroid/stack/include               \
                        bluedroid/stack/l2cap                 \
                        bluedroid/stack/sdp                   \
                        bluedroid/stack/smp                   \
                        bluedroid/stack/avct                  \
                        bluedroid/stack/avrc                  \
                        bluedroid/stack/avdt                  \
                        bluedroid/stack/a2dp                  \
                        bluedroid/stack/rfcomm                \
                        bluedroid/stack                       \
                        bluedroid/utils                       \
                        bluedroid/api                         \
                        bluedroid

ifeq ($(GCC_NOT_5_2_0), 1)
bluedroid/bta/sdp/bta_sdp_act.o: CFLAGS += -Wno-unused-const-variable
bluedroid/btc/core/btc_config.o: CFLAGS += -Wno-unused-const-variable
bluedroid/stack/btm/btm_sec.o: CFLAGS += -Wno-unused-const-variable
bluedroid/stack/smp/smp_keys.o: CFLAGS += -Wno-unused-const-variable
endif

endif
