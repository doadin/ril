/*
 * samsung-ipc-1.0.vapi
 *
 * Copyright (C) 2011-2012 Simon Busch <morphis@gravedo.de>
 *
 * libsamsung-ipc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * libsamsung-ipc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

[CCode (cheader_filename = "radio.h")]
namespace SamsungIpc
{
    [CCode (cname = "int", cprefix = "IPC_CLIENT_TYPE_", has_type_id = false)]
    public enum ClientType
    {
        FMT,
        RFS,
    }

    [CCode (cname = "int", cprefix = "IPC_DEVICE_", has_type_id = false)]
    public enum DeviceType
    {
        CRESPO,
        H1,
    }

    [CCode (cname = "int", cprefix = "IPC_TYPE_", has_type_id = false)]
    public enum RequestType
    {
        EXEC,
        GET,
        SET,
        CFRM,
        EVENT,
    }

    [CCode (cname = "unsigned char", cprefix = "IPC_TYPE_", has_type_id = false)]
    public enum ResponseType
    {
        INDI,
        RESP,
        NOTI,
    }

    [CCode (cname = "int", cprefix = "IPC_GROUP_", has_type_id = false)]
    public enum MessageGroup
    {
        PWR,
        CALL,
        SMS,
        SEC,
        PB,
        DISP,
        NET,
        SND,
        MISC,
        SVC,
        SS,
        GPRS,
        SAT,
        CFG,
        IMEI,
        GPS,
        SAP,
        GEN,
    }

    [CCode (cname = "unsigned short", cprefix = "IPC_", has_type_id = false)]
    public enum MessageType
    {
        PWR_PHONE_PWR_OFF,
        PWR_PHONE_PWR_UP,
        PWR_PHONE_RESET,
        PWR_BATT_STATUS,
        PWR_BATT_TYPE,
        PWR_BATT_COMP,
        PWR_PHONE_STATE,
        PB_ACCESS,
        PB_STORAGE,
        PB_STORAGE_LIST,
        PB_ENTRY_INFO,
        PB_CAPABILITY_INFO,
        SS_WAITING,
        SS_CLI,
        SS_BARRING,
        SS_BARRING_PW,
        SS_FORWARDING,
        SS_INFO,
        SS_MANAGE_CALL,
        SS_USSD,
        SS_AOC,
        SS_RELEASE_COMPLETE,
        GPRS_DEFINE_PDP_CONTEXT,
        GPRS_QOS,
        GPRS_PS,
        GPRS_PDP_CONTEXT,
        GPRS_SHOW_PDP_ADDR,
        GPRS_3G_QUAL_SERVICE_PROFILE,
        GPRS_IP_CONFIGURATION,
        GPRS_DEFINE_SEC_PDP_CONTEXT,
        GPRS_TFT,
        GPRS_HSDPA_STATUS,
        GPRS_CURRENT_SESSION_DATA_COUNT,
        GPRS_DATA_DORMANT,
        GPRS_DUN_PIN_CTRL,
        GPRS_CALL_STATUS,
        GPRS_PORT_LIST,
        SAT_PROFILE_DOWNLOAD,
        SAT_ENVELOPE_CMD,
        SAT_PROACTIVE_CMD,
        SAT_TERMINATE_USAT_SESSION,
        SAT_EVENT_DOWNLOAD,
        SAT_PROVIDE_LOCAL_INFO,
        SAT_POLLING,
        SAT_REFRESH,
        SAT_SETUP_EVENT_LIST,
        SAT_CALL_CONTROL_RESULT,
        SAT_IMAGE_CLUT,
        SAT_CALL_PROCESSING,
        IMEI_START,
        IMEI_CHECK_DEVICE_INFO,
        CALL_OUTGOING,
        CALL_INCOMING,
        CALL_RELEASE,
        CALL_ANSWER,
        CALL_STATUS,
        CALL_LIST,
        CALL_BURST_DTMF,
        CALL_CONT_DTMF,
        CALL_WAITING,
        CALL_LINE_ID,
        DISP_ICON_INFO,
        DISP_HOMEZONE_INFO,
        DISP_RSSI_INFO,
        SEC_SIM_STATUS,
        SEC_PHONE_LOCK,
        SEC_CHANGE_LOCKING_PW,
        SEC_SIM_LANG,
        SEC_RSIM_ACCESS,
        SEC_GSIM_ACCESS,
        SEC_SIM_ICC_TYPE,
        SEC_LOCK_INFO,
        SEC_ISIM_AUTH,
        NET_PREF_PLMN,
        NET_PLMN_SEL,
        NET_CURRENT_PLMN,
        NET_PLMN_LIST,
        NET_REGIST,
        NET_SUBSCRIBER_NUM,
        NET_BAND_SEL,
        NET_SERVICE_DOMAIN_CONFIG,
        NET_POWERON_ATTACH,
        NET_MODE_SEL,
        NET_ACQ_ORDER,
        NET_IDENTITY,
        NET_CURRENT_RRC_STATUS,
        GEN_PHONE_RES,
        MISC_ME_VERSION,
        MISC_ME_IMSI,
        MISC_ME_SN,
        MISC_TIME_INFO,
        SMS_SEND_MSG,
        SMS_INCOMING_MSG,
        SMS_READ_MSG,
        SMS_SAVE_MSG,
        SMS_DEL_MSG,
        SMS_DELIVER_REPORT,
        SMS_DEVICE_READY,
        SMS_SEL_MEM,
        SMS_STORED_MSG_COUNT,
        SMS_SVC_CENTER_ADDR,
        SMS_SVC_OPTION,
        SMS_MEM_STATUS,
        SMS_CBS_MSG,
        SMS_CBS_CONFIG,
        SMS_STORED_MSG_STATUS,
        SMS_PARAM_COUNT,
        SMS_PARAM,
        SND_SPKR_VOLUME_CTRL,
        SND_MIC_MUTE_CTRL,
        SND_AUDIO_PATH_CTRL,
        SND_RINGBACK_TONE_CTRL,
        SND_CLOCK_CTRL,
        RFS_NV_READ_ITEM,
        RFS_NV_WRITE_ITEM,
    }

    /* ******************************************************************************** */

    namespace Power
    {
        [CCode (cname = "gint16", cprefix = "IPC_PWR_PHONE_STATE_", has_type_id = false)]
        public enum PhoneState
        {
            LPM,
            NORMAL,
        }
    }

    /* ******************************************************************************** */

    namespace Security
    {
        [CCode (cname = "gint8", cprefix = "IPC_SEC_SIM_STATUS_", has_type_id = false)]
        public enum SimStatus
        {
            INITIALIZING,
            SIM_LOCK_REQUIRED,
            INSIDE_PF_ERROR,
            LOCK_SC,
            LOCK_FD,
            LOCK_PN,
            LOCK_PU,
            LOCK_PP,
            LOCK_PC,
            CARD_NOT_PRESENT,
            CARD_ERROR,
            INIT_COMPLETE,
            PB_INIT_COMPLETE,
        }

        [CCode (cname = "gint8", cprefix = "IPC_SEC_FACILITY_TYPE_", has_type_id = false)]
        public enum FacilityType
        {
            SC,
            FD,
            PN,
            PU,
            PP,
            PC,
        }

        [CCode (cname = "gint8", cprefix = "IPC_SEC_FACILITY_LOCK_TYPE_", has_type_id = false)]
        public enum FacilityLockType
        {
            SC_UNLOCKED,
            SC_PIN1_REQ,
            SC_PUK_REQ,
            SC_CARD_BLOCKED,
        }

        [CCode (cname = "gint8", cprefix = "IPC_SEC_PIN_TYPE_", has_type_id = false)]
        public enum PinType
        {
            PIN1,
            PIN2,
        }

        [CCode (cname = "gint8", cprefix = "IPC_SEC_SIM_CARD_TYPE_", has_type_id = false)]
        public enum SimCardType
        {
            UNKNOWN,
            SIM,
            USIM,
        }

        [CCode (cname = "gint8", cprefix = "IPC_SEC_RSIM_COMMAND_", has_type_id = false)]
        public enum RSimCommandType
        {
            READ_BINARY,
            READ_RECORD,
            UPDATE_BINARY,
            STATUS,
        }

        [CCode (cname = "struct ipc_sec_sim_status_response", destroy_function = "")]
        public struct SimStatusMessage
        {
            public SimStatus status;
            public FacilityLockType facility_lock;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( SimStatusMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_sec_pin_status_set", destroy_function = "")]
        public struct PinStatusSetMessage
        {
            public PinType type;
            public uint8 length1;
            public uint8 length2;
            [CCode (array_length_cname = "length1")]
            public uint8[] pin1; // size = 8
            [CCode (array_length_cname = "length2")]
            public uint8[] pin2; // size = 8

            [CCode (cname = "ipc_sec_pin_status_set_setup")]
            public void setup(PinType pin_type, string pin1, string pin2);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PinStatusSetMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_sec_phone_lock_get", destroy_function = "")]
        public struct PhoneLockGetMessage
        {
            public FacilityType facility;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PhoneLockGetMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_sec_phone_lock_response", destroy_function = "")]
        public struct PhoneLockGetResponseMessage
        {
            public FacilityType facility;
            public bool status;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PhoneLockGetResponseMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_sec_rsim_access_get", destroy_function = "")]
        public struct RSimAccessRequestMessage
        {
            public RSimCommandType command;
            public uint16 fileid;
            public uint8 p1;
            public uint8 p2;
            public uint8 p3;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( RSimAccessRequestMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_sec_rsim_access_response", destroy_function = "")]
        public struct RSimAccessResponseMessage
        {
            public uint8 sw1;
            public uint8 sw2;
            public uint8 len;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( RSimAccessResponseMessage );
                    return res;
                }
            }

            [CCode (cname = "ipc_sec_rsim_access_response_get_file_data")]
            public static string get_file_data( Response response );
        }

        [CCode (cname = "struct ipc_sec_lock_info_get", destroy_function = "")]
        public struct LockInfoRequestMessage
        {
            public uint8 unk0;
            public PinType pin_type;

            [CCode (cname = "ipc_sec_lock_info_get_setup")]
            public void setup(PinType pin_type);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( LockInfoRequestMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_sec_lock_info_response", destroy_function = "")]
        public struct LockInfoResponseMessage
        {
            public uint8 num;
            public PinType type;
            public uint8 key;
            public uint8 attempts;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( LockInfoResponseMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Display
    {
        [CCode (cname = "ipc_disp_icon_info", destroy_function = "")]
        public struct IconInfoMessage
        {
            public uint8 unk;
            public uint8 rssi;
            public uint8 battery;
            public uint8 act;
            public uint8 reg;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( IconInfoMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_disp_rssi_info", destroy_function = "")]
        public struct RssiInfoMessage
        {
            public uint8 rssi;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( RssiInfoMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Network
    {
        [CCode (cname = "gint8", cprefix = "IPC_NET_ACCESS_TECHNOLOGY_", has_type_id = false)]
        public enum AccessTechnology
        {
            UNKNOWN,
            GSM,
            GSM2,
            GPRS,
            EDGE,
            UMTS,
        }

        [CCode (cname = "gint8", cprefix = "IPC_NET_REGISTRATION_STATE_", has_type_id = false)]
        public enum RegistrationState
        {
            NONE,
            HOME,
            SEARCHING,
            EMERGENCY,
            UNKNOWN,
            ROAMING,
        }

        [CCode (cname = "gint8", cprefix = "IPC_NET_PLMN_STATUS_", has_type_id = false)]
        public enum PlmnStatus
        {
            AVAILABLE,
            CURRENT,
            FORBIDDEN,
        }

        [CCode (cname = "gint8", cprefix = "IPC_NET_PLMN_SEL_", has_type_id = false)]
        public enum PlmnSelectionMode
        {
            MANUAL,
            AUTO,
        }

        [CCode (cname = "gint8", cprefix = "IPC_NET_MODE_SEL_", has_type_id = false)]
        public enum NetworkSelectionMode
        {
            GSM_UMTS,
            GSM_ONLY,
            UMTS_ONLY
        }

        [CCode (cname = "gint8", cprefix = "IPC_NET_SERVICE_DOMAIN_", has_type_id = false)]
        public enum ServiceDomain
        {
            GSM,
            GPRS,
        }

        [CCode (cname = "struct ipc_net_regist_response", destroy_function = "")]
        public struct RegistrationMessage
        {
            public AccessTechnology act;
            public ServiceDomain domain;
            public RegistrationState reg_state;
            public uint8 edge;
            public uint16 lac;
            public uint32 cid;
            public uint8 rej_cause;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( RegistrationMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_net_regist_get", destroy_function = "")]
        public struct RegistrationGetMessage
        {
            public uint8 net;
            public ServiceDomain domain;

            [CCode (cname = "ipc_net_regist_get_setup")]
            public void setup( ServiceDomain domain );

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( RegistrationGetMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_net_current_plmn_response", destroy_function = "")]
        public struct CurrentPlmnMessage
        {
            public uint8 unk0;
            public uint8 slevel;
            public uint8 unk1;
            [CCode (array_length = false)]
            public uint8[] plmn;
            public uint8 type;
            public uint16 lac;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( CurrentPlmnMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_net_plmn_entry", destroy_function = "")]
        public struct PlmnEntryMessage
        {
            public PlmnStatus status;
            [CCode (array_length = false)]
            public uint8[] plmn;
            public uint8 type;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PlmnEntryMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_net_plmn_entries", destroy_function = "")]
        public struct PlmnEntriesMessage
        {
            public uint8 num;

            public unowned PlmnEntryMessage? get_entry( Response response, uint pos )
            {
                unowned PlmnEntryMessage? entry = null;

                if ( pos >= num )
                    return null;

                uint8 *p = ((uint8*) response.data) + sizeof(PlmnEntriesMessage);
                entry = (PlmnEntryMessage?) (p + pos * sizeof(PlmnEntryMessage));

                return entry;
            }

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PlmnEntriesMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_net_plmn_sel_get", destroy_function = "")]
        public struct PlmnSelectionGetMessage
        {
            [CCode (cname = "plmn_sel")]
            public PlmnSelectionMode mode;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PlmnSelectionGetMessage );
                    return res;
                }
            }

        }

        [CCode (cname = "struct ipc_net_plmn_sel_set", destroy_function = "")]
        public struct PlmnSelectionSetMessage
        {
            public PlmnSelectionMode mode;
            public uint8[] plmn;
            public AccessTechnology act;

            [CCode (cname = "ipc_net_plmn_sel_set_setup")]
            public void setup(uint8 mode, string plmn, AccessTechnology act);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PlmnSelectionSetMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Call
    {
        [CCode (cname = "int", cprefix = "IPC_CALL_TYPE_", has_type_id = false)]
        public enum Type
        {
            VOICE,
            DATA,
        }

        [CCode (cname = "gint8", cprefix = "IPC_CALL_IDENTITY_", has_type_id = false)]
        public enum Identity
        {
            DEFAULT,
            HIDE,
            SHOW,
        }

        [CCode (cname = "gint8", cprefix = "IPC_CALL_PREFIX_", has_type_id = false)]
        public enum Prefix
        {
            NONE,
            INTL,
        }

        [CCode (cname = "gint8", cprefix = "IPC_CALL_STATE_", has_type_id = false)]
        public enum State
        {
            DIALING,
            IGNORING_INCOMING_STATUS,
            CONNECTED,
            RELEASED,
            CONNECTING,
        }

        [CCode (cname = "gint8", cprefix = "IPC_CALL_LIST_ENTRY_STATE_", has_type_id = false)]
        public enum State2
        {
            ACTIVE,
            HOLDING,
            DIALING,
            ALERTING,
            INCOMING,
            WAITING
        }

        [CCode (cname = "int", cprefix = "IPC_CALL_TERM_", has_type_id = false)]
        public enum Termination
        {
            MO,
            MT,
        }

        [CCode (cname = "uint8", cprefix = "IPC_CALL_DTMF_STATE_", has_type_id = false)]
        public enum DtmfState
        {
            START,
            STOP
        }

        [CCode (cname = "struct ipc_call_outgoing", destroy_function = "")]
        public struct OutgoingMessage
        {
            public Type type;
            public Identity identity;
            public uint8 length;
            public Prefix prefix;
            public uint8[] number;

            [CCode (cname = "ipc_call_outgoing_setup")]
            public void setup(Type type, Identity identity, Prefix prefix, string number);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( OutgoingMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_call_incoming", destroy_function = "")]
        public struct IncomingMessage
        {
            public uint8 type;
            public uint8 id;
            public uint8 line;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( IncomingMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_message_info", destroy_function = "")]
        public struct ListResponseMessage
        {
            [CCode (cname = "ipc_call_list_response_get_num_entries")]
            public uint get_num_entries();
            [CCode (cname = "ipc_call_list_response_get_entry")]
            public ListEntry* get_entry(uint num);
            [CCode (cname = "ipc_call_list_response_get_entry_number")]
            public string get_entry_number(uint num);
        }

        [SimpleType]
        [CCode (cname = "struct ipc_call_list_entry", destroy_function = "")]
        public struct ListEntry
        {
            public Type type;
            public uint8 idx;
            public Termination term;
            public State2 state;
            public uint8 mpty;
            public uint8 number_len;
            public uint8 unk4;
        }

        [CCode (cname = "struct ipc_call_status", destroy_function = "")]
        public struct StatusMessage
        {
            public uint8 type;
            public uint8 id;
            public State state;
            public uint8 reason;
            public uint8 end_cause;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( StatusMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_call_cont_dtmf", destroy_function = "")]
        public struct ContDtmfMessage
        {
            public DtmfState state;
            public uint8 tone;

            [CCode (cname = "ipc_call_cont_dtmf_burst_pack")]
            public uint8[] pack(uint8[] burst);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( ContDtmfMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Generic
    {
        [CCode (cname = "struct ipc_gen_phone_res", destroy_function = "")]
        public struct PhoneResponseMessage
        {
            public uint8 group;
            public uint8 type;
            public uint16 code;
            public uint8 unk;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PhoneResponseMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Misc
    {
        [CCode (cname = "ipc_parse_misc_me_imsi")]
        public string parse_imsi(uint8[] data);

        [CCode (cname = "struct ipc_misc_me_version", destroy_function = "", free_function = "")]
        public struct VersionMessage
        {
            [CCode (array_length = false)]
            public uint8[] sw_version;
            [CCode (array_length = false)]
            public uint8[] hw_version;
            [CCode (array_length = false)]
            public uint8[] cal_date;
            [CCode (array_length = false)]
            public uint8[] misc;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( VersionMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "gint8", cprefix = "IPC_MISC_ME_SN_SERIAL_NUM_", has_type_id = false)]
        public enum SerialNumberType
        {
            SERIAL,
            MANUFACTURE_DATE,
            BARCODE,
        }

        [CCode (cname = "struct ipc_misc_me_sn")]
        public struct SerialNumberResponseMessage
        {
            public SerialNumberType type;
            public uint8 length;
            public uint8[] data;
        }

        [CCode (cname = "struct ipc_message_info")]
        public struct MeResponseMessage
        {
            [CCode (cname = "ipc_misc_me_imsi_response_get_imsi")]
            public static string get_imsi( Response response );
        }

        [CCode (cname = "struct ipc_misc_time_info", destroy_function = "")]
        public struct TimeInfoMessage
        {
            public uint8 tz_valid;
            public uint8 daylight_valid;
            public uint8 year;
            public uint8 mon;
            public uint8 day;
            public uint8 hour;
            public uint8 min;
            public uint8 sec;
            public uint8 tz;
            public uint8 dl;
            public uint8 dv;
            public uint8[] plmn;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( TimeInfoMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Gprs
    {
        [CCode (cname = "IPC_GPRS_PDP_CONTEXT_GET_DESC_COUNT")]
        public uint MAX_PDP_CONTEXT_COUNT;

        [CCode (cname = "gint8", cprefix = "IPC_GPRS_STATE_", has_type_id = false)]
        public enum State
        {
            NOT_ENABLED,
            ENABLED,
            DISABLED
        }

        [CCode (cname = "gint8", cprefix = "IPC_GPRS_FAIL_", has_type_id = false)]
        public enum Error
        {
            INSUFFICIENT_RESOURCES,
            MISSING_UNKNOWN_APN,
            UNKNOWN_PDP_ADDRESS_TYPE,
            USER_AUTHENTICATION,
            ACTIVATION_REJECT_CGSN,
            ACTIVATION_REJECT_UNSPECIFIED,
            SERVICE_OPTION_NOT_SUPPORTED,
            SERVICE_OPTION_NOT_SUBSCRIBED,
            SERVICE_OPTION_OUT_OF_ORDER,
            NSAPI_IN_USE
        }

        [CCode (cname = "struct ipc_gprs_define_pdp_context", destroy_function = "")]
        public struct DefinePdpContextMessage
        {
            public uint8 enable;
            public uint8 cid;
            public uint8 unk;
            public uint8[] apn;

            [CCode (cname = "ipc_gprs_define_pdp_context_setup")]
            public void setup(uint8 cid, bool enable, string apn);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( DefinePdpContextMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_gprs_ip_configuration", destroy_function = "")]
        public struct IpConfigurationMessage
        {
            public uint8 cid;
            public uint8 field_flag;
            public uint8 unk1;
            [CCode (array_length = false)]
            public uint8[] ip;
            [CCode (array_length = false)]
            public uint8[] dns1;
            [CCode (array_length = false)]
            public uint8[] dns2;
            [CCode (array_length = false)]
            public uint8[] gateway;
            [CCode (array_length = false)]
            public uint8[] subnet_mask;
            [CCode (array_length = false)]
            public uint8[] unk2;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( IpConfigurationMessage );
                    return res;
                }
            }
        }

        [CCode (name = "struct ipc_gprs_pdp_context_get_desc", destroy_function = "")]
        public struct PdpContextGetDescMessage
        {
            public uint8 cid;
            public uint8 state;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PdpContextGetDescMessage );
                    return res;
                }
            }
        }

        [CCode (name = "struct ipc_gprs_pdp_context_get", destroy_function = "")]
        public struct PdpContextGetMessage
        {
            public uint8 unk;
            public PdpContextGetDescMessage[] desc;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PdpContextGetMessage );
                    return res;
                }
            }
        }


        [CCode (name = "struct ipc_gprs_call_status", destroy_function = "")]
        public struct CallStatusMessage
        {
            public uint8 cid;
            public State status;
            public uint16 fail_status;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( CallStatusMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_gprs_hsdpa_status", destroy_function = "")]
        public struct HsdpaStatusMessage
        {
            public uint8 unk;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( HsdpaStatusMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_gprs_pdp_context_set", destroy_function = "")]
        public struct PdpContextSetMessage
        {
            public uint8 enable;
            public uint8 cid;
            public uint8[] unk0;
            public uint8[] username;
            public uint8[] password;
            public uint8[] unk1;
            public uint8 unk2;

            [CCode (cname = "ipc_gprs_pdp_context_setup")]
            public void setup(int cid, bool activate, string? username, string? password);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PdpContextSetMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_gprs_ps", destroy_function = "")]
        public struct PsMessage
        {
            public uint8[] unk;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PsMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_gprs_current_session_data_counter", destroy_function = "")]
        public struct CurrentSessionDataCounterMessage
        {
            public uint8[] cid;
            public uint8[] rx_count;
            public uint8[] tx_count;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( CurrentSessionDataCounterMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_gprs_port_list")]
        public struct PortListMessage
        {
            public uint8[] unk;

            [CCode (cname = "ipc_gprs_port_list_setup")]
            public void setup();

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( PortListMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Sms
    {
        [CCode (cname = "gint8", cprefix = "IPC_SMS_MSG_", has_type_id = false)]
        public enum MessageAmountType
        {
            MULTIPLE,
            SINGLE,
        }

        [CCode (cname = "gint8", cprefix = "IPC_SMS_TYPE_", has_type_id = false)]
        public enum MessageType
        {
            POINT_TO_POINT,
            STATUS_REPORT,
            OUTGOING,
        }

        [CCode (cname = "gint16", cprefix = "IPC_SMS_ACK_", has_type_id = false)]
        public enum AcknowledgeErrorType
        {
            NO_ERROR,
            PDA_FULL_ERROR,
            MAILFORMED_REQ_ERROR,
            UNSPEC_ERROR,
        }

        [CCode (cname = "struct ipc_sms_incoming_msg")]
        public struct IncomingMessage
        {
            public uint8 msg_type;
            public uint8 type;
            public uint16 sim_index;
            public uint8 msg_tpid;
            public uint8 length;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( IncomingMessage );
                    return res;
                }
            }

            public uint8[] unpack_pdu( Response response )
            {
                uint8[] pdu = new uint8[this.length];
                uint8 *pdu_start = ((uint8*) response.data) + sizeof( IncomingMessage );
                GLib.Memory.copy(pdu, pdu_start, this.length);
                return pdu;
            }
        }

        [CCode (cname = "struct ipc_sms_send_msg_request")]
        public struct SendMessage
        {
            public MessageType type;
            public MessageAmountType msg_type;
            public uint8 length;
            public uint8 smsc_len;

            public uint8[] pack(string smsc, uint8[] pdu);

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( SendMessage );
                    return res;
                }
            }
        }

        [CCode (cname = "struct ipc_sms_send_msg_response")]
        public struct DeliverReportMessage
        {
            public MessageType type;
            public AcknowledgeErrorType error;
            public uint8 msg_tpid;
            public uint8 unk;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( DeliverReportMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Sound
    {
        [CCode (cname = "gint8", cprefix = "IPC_SND_VOLUME_TYPE_", has_type_id = false)]
        public enum VolumeType
        {
            VOICE,
            SPEAKER,
            HEADSET,
            BTVOICE,
        }

        [CCode (cname = "gint8", cprefix = "IPC_SND_AUDIO_PATH_", has_type_id = false)]
        public enum AudioPath
        {
            HANDSET,
            HEADSET,
            SPEAKER,
            BLUETOOTH,
            BLUETOOTH_NO_NR,
            HEADPHONE,
        }

        [CCode (cname = "struct ipc_snd_spkr_volume_ctrl")]
        public struct SpeakerVolumeControlMessage
        {
            public VolumeType type;
            public uint8 volume;

            public unowned uint8[] data
            {
                get
                {
                    unowned uint8[] res = (uint8[])(&this);
                    res.length = (int) sizeof( SpeakerVolumeControlMessage );
                    return res;
                }
            }
        }
    }

    /* ******************************************************************************** */

    namespace Rfs
    {
        [CCode (cname = "ipc_rfs_send_io_confirm_for_nv_read_item")]
        public void send_io_confirm_for_nv_read_item(Client client, Response req);
        [CCode (cname = "ipc_rfs_send_io_confirm_for_nv_write_item")]
        public void send_io_confirm_for_nv_write_item(Client client, Response req);
    }

    /* ******************************************************************************** */

    [CCode (cname = "struct ipc_message_info", destroy_function = "", free_function = "")]
    public struct Request
    {
        public uint8 mseq;
        public uint8 aseq;
        public uint8 group;
        public uint8 index;
        public RequestType type;
        public uint32 length;
        [CCode (array_length_cname = "length")]
        public uint8[] data;

        public MessageType command
        {
            get { return (MessageType) ((group << 8) | index); }
            set { group = value >> 8; index = value & 0xff; }
        }
    }

    [CCode (cname = "struct ipc_message_info", destroy_function = "", free_function = "", copy_function = "")]
    public struct Response
    {
        public uint8 mseq;
        public uint8 aseq;
        public uint8 group;
        public uint8 index;
        public ResponseType type;
        public uint32 length;
        [CCode (array_length_cname = "length")]
        public uint8[] data;

        public MessageType command
        {
            get { return (MessageType) ((group << 8) | index); }
            set { group = value >> 8; index = value & 0xff; }
        }
    }

    public delegate int TransportCb(uint8[] data);
    public delegate void LogHandlerCb(string message);

    [Compact]
    [CCode (cname = "struct ipc_client", cprefix = "ipc_client_")]
    public class Client
    {
        public Client(ClientType client_type);
        public Client.for_device(DeviceType device_type, ClientType client_type);
        [CCode (delagate_target_pos = 0.9)]
        public int set_log_handler(LogHandlerCb log_cb);
        public int set_io_handlers(TransportCb write_cb, TransportCb read_cb);
        public int bootstrap_modem();
        public int open();
        public int close();
        public int power_on();
        public int power_off();
        public int recv(out Response response);
        public void send(MessageType command, RequestType type, uint8[] data, uint8 mseq);
        public void send_get(MessageType command, uint8 aseq);
        public void send_exec(MessageType command, uint8 aseq);
        public void set_handlers_common_data_fd(int fd);
        public int get_handlers_common_data_fd();
        public int create_handlers_common_data();
    }
}
