// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/************************************************************************************
 *
 *  Filename:      btc_util.c
 *
 *  Description:   Miscellaneous helper functions
 *
 *
 ***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "btc/btc_util.h"
#if (BTA_AV_INCLUDED == TRUE)
#include "bta/bta_av_api.h"
#endif  ///BTA_AV_INCLUDED == TRUE
#include "common/bt_defs.h"
#include "stack/btm_api.h"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
#define ISDIGIT(a)  ((a>='0') && (a<='9'))
#define ISXDIGIT(a) (((a>='0') && (a<='9'))||((a>='A') && (a<='F'))||((a>='a') && (a<='f')))

/************************************************************************************
**  Local type definitions
************************************************************************************/

/************************************************************************************
**  Static variables
************************************************************************************/

/************************************************************************************
**  Static functions
************************************************************************************/

/************************************************************************************
**  Externs
************************************************************************************/

/************************************************************************************
**  Functions
************************************************************************************/

/*****************************************************************************
**   Logging helper functions
*****************************************************************************/
#if(BTA_AV_INCLUDED == TRUE)
const char *dump_rc_event(UINT8 event)
{
    switch (event) {
        CASE_RETURN_STR(BTA_AV_RC_OPEN_EVT)
        CASE_RETURN_STR(BTA_AV_RC_CLOSE_EVT)
        CASE_RETURN_STR(BTA_AV_REMOTE_CMD_EVT)
        CASE_RETURN_STR(BTA_AV_REMOTE_RSP_EVT)
        CASE_RETURN_STR(BTA_AV_VENDOR_CMD_EVT)
        CASE_RETURN_STR(BTA_AV_VENDOR_RSP_EVT)
        CASE_RETURN_STR(BTA_AV_META_MSG_EVT)
        CASE_RETURN_STR(BTA_AV_RC_FEAT_EVT)
    default:
        return "UNKNOWN_EVENT";
    }
}

const char *dump_rc_notification_event_id(UINT8 event_id)
{
    switch (event_id) {
        CASE_RETURN_STR(AVRC_EVT_PLAY_STATUS_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_TRACK_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_TRACK_REACHED_END)
        CASE_RETURN_STR(AVRC_EVT_TRACK_REACHED_START)
        CASE_RETURN_STR(AVRC_EVT_PLAY_POS_CHANGED)
        CASE_RETURN_STR(AVRC_EVT_BATTERY_STATUS_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_SYSTEM_STATUS_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_APP_SETTING_CHANGE)
        CASE_RETURN_STR(AVRC_EVT_VOLUME_CHANGE)

    default:
        return "Unhandled Event ID";
    }
}

const char  *dump_rc_pdu(UINT8 pdu)
{
    switch (pdu) {
        CASE_RETURN_STR(AVRC_PDU_LIST_PLAYER_APP_ATTR)
        CASE_RETURN_STR(AVRC_PDU_LIST_PLAYER_APP_VALUES)
        CASE_RETURN_STR(AVRC_PDU_GET_CUR_PLAYER_APP_VALUE)
        CASE_RETURN_STR(AVRC_PDU_SET_PLAYER_APP_VALUE)
        CASE_RETURN_STR(AVRC_PDU_GET_PLAYER_APP_ATTR_TEXT)
        CASE_RETURN_STR(AVRC_PDU_GET_PLAYER_APP_VALUE_TEXT)
        CASE_RETURN_STR(AVRC_PDU_INFORM_DISPLAY_CHARSET)
        CASE_RETURN_STR(AVRC_PDU_INFORM_BATTERY_STAT_OF_CT)
        CASE_RETURN_STR(AVRC_PDU_GET_ELEMENT_ATTR)
        CASE_RETURN_STR(AVRC_PDU_GET_PLAY_STATUS)
        CASE_RETURN_STR(AVRC_PDU_REGISTER_NOTIFICATION)
        CASE_RETURN_STR(AVRC_PDU_REQUEST_CONTINUATION_RSP)
        CASE_RETURN_STR(AVRC_PDU_ABORT_CONTINUATION_RSP)
        CASE_RETURN_STR(AVRC_PDU_SET_ABSOLUTE_VOLUME)
    default:
        return "Unknown PDU";
    }
}
#endif  ///BTA_AV_INCLUDED == TRUE

UINT32 devclass2uint(DEV_CLASS dev_class)
{
    UINT32 cod = 0;

    if (dev_class != NULL) {
        /* if COD is 0, irrespective of the device type set it to Unclassified device */
        cod = (dev_class[2]) | (dev_class[1] << 8) | (dev_class[0] << 16);
    }
    return cod;
}

void uint2devclass(UINT32 cod, DEV_CLASS dev_class)
{
    dev_class[2] = (UINT8)cod;
    dev_class[1] = (UINT8)(cod >> 8);
    dev_class[0] = (UINT8)(cod >> 16);
}

static const UINT8  base_uuid_be[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                                      0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};

void uuid128_be_to_esp_uuid(esp_bt_uuid_t *u, uint8_t* uuid128)
{
    if (memcmp(base_uuid_be+4, uuid128 + 4, 12) != 0) {
        u->len = ESP_UUID_LEN_128;
        uint8_t *p_i = uuid128 + ESP_UUID_LEN_128 - 1;
        uint8_t *p_o = u->uuid.uuid128;
        uint8_t *p_end = p_o + ESP_UUID_LEN_128;
        for (; p_o != p_end; *p_o++ = *p_i--)
            ;
    } else if (uuid128[0] == 0 && uuid128[1] == 0) {
        u->len = 2;
        u->uuid.uuid16 = (uuid128[2] << 8) + uuid128[3];
    } else {
        u->len = 4;
        u->uuid.uuid32 = (uuid128[2] << 8) + uuid128[3];
        u->uuid.uuid32 += (uuid128[0] << 24) + (uuid128[1] << 16);
    }

    return;
}

void uuid_to_string_legacy(bt_uuid_t *p_uuid, char *str)
{
    uint32_t uuid0, uuid4;
    uint16_t uuid1, uuid2, uuid3, uuid5;

    memcpy(&uuid0, &(p_uuid->uu[0]), 4);
    memcpy(&uuid1, &(p_uuid->uu[4]), 2);
    memcpy(&uuid2, &(p_uuid->uu[6]), 2);
    memcpy(&uuid3, &(p_uuid->uu[8]), 2);
    memcpy(&uuid4, &(p_uuid->uu[10]), 4);
    memcpy(&uuid5, &(p_uuid->uu[14]), 2);

    sprintf((char *)str, "%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
            ntohl(uuid0), ntohs(uuid1),
            ntohs(uuid2), ntohs(uuid3),
            ntohl(uuid4), ntohs(uuid5));
    return;
}

esp_bt_status_t btc_hci_to_esp_status(uint8_t hci_status)
{
    esp_bt_status_t esp_status = ESP_BT_STATUS_FAIL;
    switch(hci_status) {
        case HCI_SUCCESS:
            esp_status = ESP_BT_STATUS_SUCCESS;
            break;
        case HCI_ERR_HOST_TIMEOUT:
            esp_status = ESP_BT_STATUS_TIMEOUT;
            break;
        case HCI_ERR_ILLEGAL_COMMAND:
            esp_status = ESP_BT_STATUS_PENDING;
            break;
        case HCI_ERR_UNACCEPT_CONN_INTERVAL:
            esp_status = ESP_BT_STATUS_UNACCEPT_CONN_INTERVAL;
            break;
        case HCI_ERR_PARAM_OUT_OF_RANGE:
            esp_status = ESP_BT_STATUS_PARAM_OUT_OF_RANGE;
            break;
        case HCI_ERR_ILLEGAL_PARAMETER_FMT:
            esp_status = ESP_BT_STATUS_ERR_ILLEGAL_PARAMETER_FMT;
            break;
        default:
            esp_status = ESP_BT_STATUS_FAIL;
            break;
    }

    return esp_status;
}

esp_bt_status_t btc_btm_status_to_esp_status (uint8_t btm_status)
{
    esp_bt_status_t esp_status = ESP_BT_STATUS_FAIL;
    switch(btm_status){
        case BTM_SUCCESS:
            esp_status = ESP_BT_STATUS_SUCCESS;
            break;
        case BTM_BUSY:
            esp_status = ESP_BT_STATUS_BUSY;
            break;
        case BTM_NO_RESOURCES:
            esp_status = ESP_BT_STATUS_NOMEM;
            break;
        case BTM_ERR_PROCESSING:
            esp_status = ESP_BT_STATUS_PENDING;
            break;
        case BTM_PEER_LE_DATA_LEN_UNSUPPORTED:
            esp_status = ESP_BT_STATUS_PEER_LE_DATA_LEN_UNSUPPORTED;
            break;
        case BTM_CONTROL_LE_DATA_LEN_UNSUPPORTED:
            esp_status = ESP_BT_STATUS_CONTROL_LE_DATA_LEN_UNSUPPORTED;
            break;
        case BTM_SET_PRIVACY_SUCCESS:
            esp_status = ESP_BT_STATUS_SUCCESS;
            break;
        case BTM_SET_PRIVACY_FAIL:
            esp_status = ESP_BT_STATUS_FAIL;
            break;
        default:
            esp_status = ESP_BT_STATUS_FAIL;
            break;
    }

    return esp_status;
}
