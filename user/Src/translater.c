#include "main.h"

//#define NUM_G_CAN_RECEIVE_MESSAGE_JBD 0x20;


/* select CAN baudrate */
/* 1MBps */
/* #define CAN_BAUDRATE  1000 */
/* 500kBps */
#define CAN_BAUDRATE  500 
/* 250kBps */
/* #define CAN_BAUDRATE  250 */
/* 125kBps */
/* #define CAN_BAUDRATE  125 */
/* 100kBps */ 
/* #define CAN_BAUDRATE  100 */
/* 50kBps */ 
/* #define CAN_BAUDRATE  50 */
/* 20kBps */ 
/* #define CAN_BAUDRATE  20 */

/*peripheral definitions */
#define CANX CAN0


uint8_t g_can_buffsize = 0;
FlagStatus g_can0_receive_flag;

can_parameter_struct can_init_parameter;
can_filter_parameter_struct can_filter_parameter;

can_trasnmit_message_struct g_transmit_message;
can_receive_message_struct g_receive_message;
 

CanBmsJbd g_can_bms_jbd1;
CanChargeController g_can_charge_controller1;
ChargingProcess g_charging_process1;


// 定义设备初始化函数指针数组，用于存放不同类型设备的初始化函数
DeviceInitializer deviceInitializers[] = {
    initializeCanBms,              // 初始化 CanBms 设备的函数指针
    initializeCanChargeController, // 初始化 CanChargeController 设备的函数指针
    // 添加其他设备类型的初始化函数指针...
};

// 定义设备操作函数指针数组，用于存放不同类型设备的操作函数
DeviceOperation deviceOperations[] = {
    operateCanBms,              // 操作 CanBms 设备的函数指针
    operateCanChargeController, // 操作 CanChargeController 设备的函数指针
    // 添加其他设备类型的操作函数指针...
};
DeviceDestroyer deviceDestroyers[] = {
    destroyCanBms,              // 销毁 CanBms 设备的函数指针
    destroyCanChargeController, // 销毁 CanChargeController 设备的函数指针
    // 添加其他设备类型的销毁函数指针...
};

/* Confirmed transmit message table */
can_trasnmit_message_struct g_can_transmit_message_JBD[/*NUM_G_CAN_TRANSMIT_MESSAGE_JBD*/] = 
{
    {0x100, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0}, 
    {0x101, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x102, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x103, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x104, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x105, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x106, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x107, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x108, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x109, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x10A, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x10B, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x10C, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x10D, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
    {0x10E, 0, CAN_FF_STANDARD, CAN_FT_REMOTE, 0, 0},
};
uint8_t NUM_G_CAN_TRANSMIT_MESSAGE_TBL = sizeof(g_can_transmit_message_JBD) / sizeof(g_can_transmit_message_JBD[0]);

can_trasnmit_message_struct g_can_transmit_message_CC[/* NUM_G_CAN_TRANSMIT_MESSAGE_CC */] = 
{
    {0, 0x1806E5F4, CAN_FF_EXTENDED, CAN_FT_DATA, 8, {0, 0, 0, 0, 0, 0, 0, 0}},
};
uint8_t NUM_G_CAN_TRANSMIT_MESSAGE_CC = sizeof(g_can_transmit_message_CC) / sizeof(g_can_transmit_message_CC[0]);

/* receive message table*/
can_receive_message_struct g_can_receive_message_tbl[0x20/*NUM_G_CAN_RECEIVE_MESSAGE_JBD*/];
 




uint16_t extract_uint16(uint8_t* data, uint8_t index) 
{
    return (uint16_t)((data[index] << 8) | data[index + 1]);
}

int16_t extract_int16(uint8_t* data, uint8_t index) 
{
    return (int16_t)((data[index] << 8) | data[index + 1]);
}

uint8_t extract_uint8(uint8_t* data, uint8_t index) 
{
    return data[index];
}

uint8_t get_high_byte(uint16_t value) 
{
    return (value >> 8) & 0xFF;
}

uint8_t get_low_byte(uint16_t value) 
{
    return value & 0xFF;
}

void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure CAN0 GPIO */
	gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    //gpio_pin_remap_config(GPIO_CAN0_FULL_REMAP,ENABLE);
    
}

void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter)
{
	
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_INIT_STRUCT, &can_filter);

    /* initialize CAN register */
    can_deinit(CAN0);
    //can_deinit(CAN1);
    
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_3TQ;
    
    /* 1MBps */
#if CAN_BAUDRATE == 1000
    can_parameter.prescaler = 6;
    /* 500KBps */
#elif CAN_BAUDRATE == 500
    can_parameter.prescaler = 12;
    /* 250KBps */
#elif CAN_BAUDRATE == 250
    can_parameter.prescaler = 24;
    /* 125KBps */
#elif CAN_BAUDRATE == 125
    can_parameter.prescaler = 48;
    /* 100KBps */
#elif  CAN_BAUDRATE == 100
    can_parameter.prescaler = 60;
    /* 50KBps */
#elif  CAN_BAUDRATE == 50
    can_parameter.prescaler = 120;
    /* 20KBps */
#elif  CAN_BAUDRATE == 20
    can_parameter.prescaler = 300;
#else
    #error "please select list can baudrate in private defines in main.c "
#endif  
    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    //can_init(CAN1, &can_parameter);
    
    /* initialize filter */ 
    can_filter.filter_number=0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000;
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    
    can_filter_init(&can_filter);
    
    can_filter.filter_number=0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000;
    can_filter.filter_fifo_number = CAN_FIFO1;
    can_filter.filter_enable = ENABLE;

    can_filter_init(&can_filter);

}

void translater_canInit(void)
{
	g_can0_receive_flag = RESET;

	can_gpio_config();
	can_config(can_init_parameter, can_filter_parameter);

    //can_interrupt_enable(CAN0, CAN_INT_RFNE0);
    can_interrupt_enable(CAN0, CAN_INT_RFNE1);

}


void init_transmit_message(can_trasnmit_message_struct* transmit_message, uint32_t tx_sfid, uint32_t tx_efid,  uint8_t tx_ff, uint8_t tx_ft, uint8_t tx_dlen, uint8_t* data)
{
    uint8_t i = 0;
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, transmit_message);

    transmit_message->tx_sfid = tx_sfid;
    transmit_message->tx_efid = tx_efid;

    transmit_message->tx_ft = tx_ft;
    transmit_message->tx_ff = tx_ff;
    transmit_message->tx_dlen = tx_dlen;
    if( tx_ft == CAN_FT_DATA)
    {   
        for(i = 0; i < tx_dlen; i++)
        {
            transmit_message->tx_data[i] = data[i];
        }
    }
}

void init_receive_message(can_receive_message_struct* receive_message, uint32_t rx_sfid, uint32_t rx_efid,  uint8_t rx_ff, uint8_t rx_ft, uint8_t rx_dlen, uint8_t* data)
{
    uint8_t i = 0;
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, receive_message);

    receive_message->rx_sfid = rx_sfid;
    receive_message->rx_efid = rx_efid;

    receive_message->rx_ft = rx_ft;
    receive_message->rx_ff = rx_ff;
    receive_message->rx_dlen = rx_dlen;
    if( rx_ft == CAN_FT_DATA)
    {   
        for(i = 0; i < rx_dlen; i++)
        {
            data[i] = receive_message->rx_data[i];
        }
    }
}

void translater_can_sendMessage(uint32_t canId, uint8_t* data, uint8_t length)
{
	uint8_t i = 0;
	can_trasnmit_message_struct transmit_message;

	if (data == NULL || length > 8) 
	{
    // 参数错误处理，可以输出错误信息或执行其他操作
    printf("Error: Invalid parameters\n");
    return;
    }

    /* 配置发送消息结构体 */
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &transmit_message);

    /* 设置发送消息的标识符 */
    transmit_message.tx_sfid = canId;
	//transmit_message.tx_efid = canId;
    /* 设置发送消息的数据长度 */
    transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_STANDARD;
    transmit_message.tx_dlen = length;

    /* 设置发送消息的数据 */
    for (i = 0; i < length; i++) 
	{
        transmit_message.tx_data[i] = data[i];
    }

    /* 发送消息 */
    can_message_transmit(CAN0, &transmit_message);
}


void translater_can_receiveMessage(uint32_t canId, uint8_t* data, uint8_t length)
{
	uint8_t i = 0;
	can_receive_message_struct receive_message;

	if(!(0x100 == receive_message.rx_sfid)&&(CAN_FF_STANDARD == receive_message.rx_ff)&&(8 == receive_message.rx_dlen))
	{
    	// 参数错误处理，可以输出错误信息或执行其他操作
    	printf("Error: Invalid parameters\n");
    	return;
    }

    /* 配置收到消息结构体 */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &receive_message);

    /* 设置收到消息的标识符 */
    receive_message.rx_sfid = canId;
	//receive_message.rx_efid = canId;
    /* 设置收到消息的数据长度 */
    receive_message.rx_ft = CAN_FT_DATA;
    receive_message.rx_ff = CAN_FF_STANDARD;
    receive_message.rx_dlen = length;

    /* 设置收到消息的数据 */
	for(i = 0; i < receive_message.rx_dlen; i++)
	{
	    data[i] = receive_message.rx_data[i];
    }
}



// CanBms设备的初始化函数
void initializeCanBms(void* device,uint8_t device_id) 
{
    CanBms* bms = (CanBms*)device;

    if (device == NULL) 
    {
        printf("pointer is null. Initialization failed. %s-%u\n", __FUNCTION__, __LINE__);
        return;
    }
    // 使用 memset 函数将整个结构体的值设置为零
    memset(bms, 0, sizeof(CanBms));
    // 初始化 CanBms 的属性...
    bms->device.device_id = device_id; 
    bms->device.device_type = DEVICE_TYPE_CAN_BMS; // 设备类型赋值
    bms->device.operate = operateCanBms; // 设备操作函数赋值

    // 其他初始化操作...
}

// CanChargeController设备的初始化函数
void initializeCanChargeController(void* device,uint8_t device_id) 
{
    CanChargeController* chargeController = (CanChargeController*)device;

    if (device == NULL) 
    {
        printf("pointer is null. Initialization failed. %s-%u\n", __FUNCTION__, __LINE__);
        return;
    }
    memset(chargeController, 0, sizeof(CanChargeController));
    // 初始化 CanChargeController 的属性...
    chargeController->device.device_id = device_id; 
    chargeController->device.device_type = DEVICE_TYPE_CAN_CHARGE_CONTROLLER; // 设备类型赋值
    // 其他初始化操作...
    chargeController->device.operate = operateCanChargeController; // 设备操作函数赋值
}

// CanBms设备的销毁函数
void destroyCanBms(void* device) 
{
    CanBms* bms = (CanBms*)device;
    // 执行 CanBms 设备的销毁操作...
    memset(bms, 0, sizeof(CanBms));
    // 释放 CanBms 设备动态分配的内存

}

// CanChargeController设备的销毁函数
void destroyCanChargeController(void* device) 
{
    CanChargeController* chargeController = (CanChargeController*)device;
    // 执行 CanChargeController 设备的销毁操作...
    memset(chargeController, 0, sizeof(CanChargeController));

}

// CanBms设备的操作函数
void operateCanBms(void* device) 
{
    CanBms* bms = (CanBms*)device;
    // 执行CanBms设备的操作
    uint8_t i = 0;
    for (i = 0; i < NUM_G_CAN_TRANSMIT_MESSAGE_TBL; i++) 
    {
        can_message_transmit(CAN0, &g_can_transmit_message_JBD[i]);

        while (can_transmit_states(CAN0, CAN_FIFO0) != CAN_TRANSMIT_OK) {}
        delay_1ms(5);
    }
}

// CanChargeController设备的操作函数
void operateCanChargeController(void* device) 
{
    //CanChargeController* controller = (CanChargeController*)device;
    uint16_t request_voltage = 0, request_current = 0, device_id = 0;
    CanBms* bms = NULL;
    CanChargeController* Controller = (CanChargeController*)device; 

    device_id = Controller->device.device_id;
    bms = findDevice(&deviceList_translator, DEVICE_TYPE_CAN_BMS, device_id);//find bms device
    if(bms == NULL)
    {
        printf("find bms device failed %s-%s-%u\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    if(bms->total_voltage < 5600u||bms->total_voltage > 8400u||bms->ntc_temperature[0] > 3281)
    {
        printf("CC failed %s-%s-%u\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }
     
    if(bms->rsoc < 10u&&bms->rsoc >= 0u)
    {
        request_voltage = 840u;
        //request_current = bms->full_capacity*2/100u;
        request_current = 84u;
    }
    else if (bms->rsoc < 20)
    {
        request_voltage = 840u;
        //request_current = bms->full_capacity*5/100u;
        request_current = 210u;
    }
    else if (bms->rsoc < 80)
    {
        request_voltage = 840u;
        request_current = 300u;
    }
    else if (bms->rsoc < 100)
    {
        request_voltage = 840u;
        //request_current = bms->full_capacity*2/100u;
        request_current = 84u;
    }
    else
    {
        printf("RSOC is false %s-%s-%u\n", __FILE__, __FUNCTION__, __LINE__);
    }

    if(request_current>300)
    {
        request_current = 300;
    }

    g_can_transmit_message_CC[device_id].tx_data[0] = get_high_byte(request_voltage);
    g_can_transmit_message_CC[device_id].tx_data[1] = get_low_byte(request_voltage);
    g_can_transmit_message_CC[device_id].tx_data[2] = get_high_byte(request_current);
    g_can_transmit_message_CC[device_id].tx_data[3] = get_low_byte(request_current);
    g_can_transmit_message_CC[device_id].tx_data[4] = 0x00; //0：open 1:close
    g_can_transmit_message_CC[device_id].tx_data[5] = 0x00; //0: normal other: error

    can_message_transmit(CAN0, &g_can_transmit_message_CC[device_id]);
    
}


// 处理CAN消息
void processCANReceiveMessage(can_receive_message_struct* message) 
{
    if (message->rx_ff == CAN_FF_STANDARD) 
    {
        if(message->rx_ft == CAN_FT_DATA)
        {
        switch (message->rx_sfid) 
        {
            case 0x100:
                // 处理总电压、电流、剩余容量的数据
                {
                    uint16_t total_voltage = extract_uint16(message->rx_data, 0);
                    int16_t current = extract_int16(message->rx_data, 2);
                    uint16_t u16_current = extract_uint16(message->rx_data, 2);
                    uint16_t remaining_capacity = extract_uint16(message->rx_data, 4);
                    uint8_t charging_flag = extract_uint8(message->rx_data, 6);

                    g_can_bms_jbd1.bms.total_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.current = extract_int16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.u16_current = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.remaining_capacity = extract_uint16(message->rx_data, 4);
                    g_can_bms_jbd1.bms.charging_flag = extract_uint8(message->rx_data, 6);
                    printf("Received CAN message with ID 0x100\n");
                    printf("Total Voltage: %u mV\n", total_voltage * 10);
                    printf("Current: %d mA\n", current * 10);
                    printf("Current: %u mA\n", u16_current * 10);
                    printf("Remaining Capacity: %u mAh\n", remaining_capacity * 10);
                    printf("Charging Flag: %u\n", charging_flag);
                    break;
                }
            case 0x101:
                {   
                    uint16_t full_capacity = extract_uint16(message->rx_data, 0);
                    uint16_t discharge_cycles = extract_uint16(message->rx_data, 2);
                    uint16_t rsoc = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.bms.full_capacity = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.discharge_cycles = extract_uint16(message->rx_data, 2);  
                    g_can_bms_jbd1.bms.rsoc = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x101\n");
                    printf("Full Capacity: %u mAh\n", full_capacity * 10);
                    printf("Discharge Cycles: %u\n", discharge_cycles);
                    printf("RSOC: %u\n", rsoc);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x102:
                {
                    uint16_t cell_balance_status_1 = extract_uint16(message->rx_data, 0);
                    uint16_t cell_balance_status_2 = extract_uint16(message->rx_data, 2);
                    uint16_t protection_flags = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.bms.cell_balance_status_1 = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.cell_balance_status_2 = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.protection_flags = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x102\n");
                    printf("Cell Balance Status 1: %u\n", cell_balance_status_1);
                    printf("Cell Balance Status 2: %u\n", cell_balance_status_2);
                    printf("the protection sign: %u\n", protection_flags);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x103:
                {
                    uint16_t mos_status = extract_uint16(message->rx_data, 0);
                    uint16_t production_date = extract_uint16(message->rx_data, 2);
                    uint16_t software_version = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);


                    g_can_bms_jbd1.bms.mos_status = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.production_date = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.software_version = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x103\n");
                    printf("MOS Status: %u\n", mos_status);
                    printf("Production Date: %u\n", production_date);
                    printf("Software Version: %u\n", software_version);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;

                }
            case 0x104:
                {
                    uint8_t num_battery_strings = extract_uint8(message->rx_data, 0);
                    uint8_t num_ntc_probes = extract_uint8(message->rx_data, 1);
                    uint16_t crc = Check_CRC16(message->rx_data, 2);
                    uint8_t status_CRC = CRC16Verification(message->rx_data,2);

                    g_can_bms_jbd1.bms.num_battery_strings = extract_uint8(message->rx_data, 0);
                    g_can_bms_jbd1.bms.num_ntc_probes = extract_uint8(message->rx_data, 1);

                    printf("Received CAN message with ID 0x104\n");
                    printf("Number of Battery Strings: %u\n", num_battery_strings);
                    printf("Number of NTC Probes: %u\n", num_ntc_probes);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x105:
                {
                    uint16_t ntc1_temperature = extract_uint16(message->rx_data, 0);
                    uint16_t ntc2_temperature = extract_uint16(message->rx_data, 2);
                    uint16_t ntc3_temperature = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.ntc1_temperature = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.ntc2_temperature = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.ntc3_temperature = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x105\n");
                    printf("NTC1 Temperature: %u\n", ntc1_temperature);
                    printf("NTC2 Temperature: %u\n", ntc2_temperature);
                    printf("NTC3 Temperature: %u\n", ntc3_temperature);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;

                }
            case 0x106:
                {
                    uint16_t ntc4_temperature = extract_uint16(message->rx_data, 0);
                    uint16_t ntc5_temperature = extract_uint16(message->rx_data, 2);
                    //uint16_t ntc6_temperature = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 4);
                    uint8_t status_CRC = CRC16Verification(message->rx_data, 4);

                    g_can_bms_jbd1.ntc4_temperature = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.ntc5_temperature = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.ntc6_temperature = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x106\n");
                    printf("NTC4 Temperature: %u\n", ntc4_temperature);
                    printf("NTC5 Temperature: %u\n", ntc5_temperature);
                    //printf("NTC6 Temperature: %u\n", ntc6_temperature);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x107:
                {
                    uint16_t cell1_voltage = extract_uint16(message->rx_data, 0);
                    uint16_t cell2_voltage = extract_uint16(message->rx_data, 2);
                    uint16_t cell3_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.cell1_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.cell2_voltage = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.cell3_voltage = extract_uint16(message->rx_data, 4);

                    g_can_bms_jbd1.bms.cell_voltage[0] = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.cell_voltage[1] = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.cell_voltage[2] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x107\n");
                    printf("Cell1 Voltage: %u\n", cell1_voltage);
                    printf("Cell2 Voltage: %u\n", cell2_voltage);
                    printf("Cell3 Voltage: %u\n", cell3_voltage);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x108:
                {
                    uint16_t cell4_voltage = extract_uint16(message->rx_data, 0);
                    uint16_t cell5_voltage = extract_uint16(message->rx_data, 2);
                    uint16_t cell6_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.cell4_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.cell5_voltage = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.cell6_voltage = extract_uint16(message->rx_data, 4);

                    g_can_bms_jbd1.bms.cell_voltage[3] = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.cell_voltage[4] = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.cell_voltage[5] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x108\n");
                    printf("Cell4 Voltage: %u\n", cell4_voltage);
                    printf("Cell5 Voltage: %u\n", cell5_voltage);
                    printf("Cell6 Voltage: %u\n", cell6_voltage);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            
            case 0x109:
                {
                    uint16_t cell7_voltage = extract_uint16(message->rx_data, 0);
                    uint16_t cell8_voltage = extract_uint16(message->rx_data, 2);
                    uint16_t cell9_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.cell7_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.cell8_voltage = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.cell9_voltage = extract_uint16(message->rx_data, 4);

                    g_can_bms_jbd1.bms.cell_voltage[6] = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.cell_voltage[7] = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.cell_voltage[8] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x109\n");
                    printf("Cell7 Voltage: %u\n", cell7_voltage);
                    printf("Cell8 Voltage: %u\n", cell8_voltage);
                    printf("Cell9 Voltage: %u\n", cell9_voltage);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x10A:
                {
                    uint16_t cell10_voltage = extract_uint16(message->rx_data, 0);
                    uint16_t cell11_voltage = extract_uint16(message->rx_data, 2);
                    uint16_t cell12_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.cell10_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.cell11_voltage = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.cell12_voltage = extract_uint16(message->rx_data, 4);

                    g_can_bms_jbd1.bms.cell_voltage[9] = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.cell_voltage[10] = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.cell_voltage[11] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x10A\n");
                    printf("Cell10 Voltage: %u\n", cell10_voltage);
                    printf("Cell11 Voltage: %u\n", cell11_voltage);
                    printf("Cell12 Voltage: %u\n", cell12_voltage);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x10B:
                {
                    uint16_t cell13_voltage = extract_uint16(message->rx_data, 0);
                    uint16_t cell14_voltage = extract_uint16(message->rx_data, 2);
                    uint16_t cell15_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.cell13_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.cell14_voltage = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.cell15_voltage = extract_uint16(message->rx_data, 4);

                    g_can_bms_jbd1.bms.cell_voltage[12] = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.cell_voltage[13] = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.cell_voltage[14] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x10B\n");
                    printf("Cell13 Voltage: %u\n", cell13_voltage);
                    printf("Cell14 Voltage: %u\n", cell14_voltage);
                    printf("Cell15 Voltage: %u\n", cell15_voltage);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x10C:
                {
                    uint16_t cell16_voltage = extract_uint16(message->rx_data, 0);
                    uint16_t cell17_voltage = extract_uint16(message->rx_data, 2);
                    uint16_t cell18_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    g_can_bms_jbd1.cell16_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.cell17_voltage = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.cell18_voltage = extract_uint16(message->rx_data, 4);

                    g_can_bms_jbd1.bms.cell_voltage[15] = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.cell_voltage[16] = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.cell_voltage[17] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x10C\n");
                    printf("Cell16 Voltage: %u\n", cell16_voltage);
                    printf("Cell17 Voltage: %u\n", cell17_voltage);
                    printf("Cell18 Voltage: %u\n", cell18_voltage);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
                case 0x10D:
                    {
                        uint16_t cell19_voltage = extract_uint16(message->rx_data, 0);
                        uint16_t cell20_voltage = extract_uint16(message->rx_data, 2);
                        uint16_t cell21_voltage = extract_uint16(message->rx_data, 4);
                        uint16_t crc = Check_CRC16(message->rx_data, 6);
                        uint8_t status_CRC = CRCVerification(message);

                        g_can_bms_jbd1.cell19_voltage = extract_uint16(message->rx_data, 0);
                        g_can_bms_jbd1.cell20_voltage = extract_uint16(message->rx_data, 2);
                        g_can_bms_jbd1.cell21_voltage = extract_uint16(message->rx_data, 4);

                        g_can_bms_jbd1.bms.cell_voltage[18] = extract_uint16(message->rx_data, 0);
                        g_can_bms_jbd1.bms.cell_voltage[19] = extract_uint16(message->rx_data, 2);
                        g_can_bms_jbd1.bms.cell_voltage[20] = extract_uint16(message->rx_data, 4);

                        printf("Received CAN message with ID 0x10D\n");
                        printf("Cell19 Voltage: %u\n", cell19_voltage);
                        printf("Cell20 Voltage: %u\n", cell20_voltage);
                        printf("Cell21 Voltage: %u\n", cell21_voltage);
                        //printf("CRC: %u\n", crc);
                        printf("CRCJBD: %u\n", status_CRC);
                        break;
                    }
                case 0x10E:
                    {
                        uint16_t cell22_voltage = extract_uint16(message->rx_data, 0);
                        uint16_t cell23_voltage = extract_uint16(message->rx_data, 2);
                        //uint16_t cell24_voltage = extract_uint16(message->rx_data, 4);
                        //uint16_t crc = Check_CRC16(message->rx_data, 4);
                        uint8_t status_CRC = CRC16Verification(message->rx_data,4);

                        g_can_bms_jbd1.cell22_voltage = extract_uint16(message->rx_data, 0);
                        g_can_bms_jbd1.cell23_voltage = extract_uint16(message->rx_data, 2);
                        g_can_bms_jbd1.cell24_voltage = extract_uint16(message->rx_data, 4);

                        g_can_bms_jbd1.bms.cell_voltage[21] = extract_uint16(message->rx_data, 0);
                        g_can_bms_jbd1.bms.cell_voltage[22] = extract_uint16(message->rx_data, 2);
                        g_can_bms_jbd1.bms.cell_voltage[23] = extract_uint16(message->rx_data, 4);

                        printf("Received CAN message with ID 0x10E\n");
                        printf("Cell22 Voltage: %u\n", cell22_voltage);
                        printf("Cell23 Voltage: %u\n", cell23_voltage);
                        //printf("Cell24 Voltage: %u\n", cell24_voltage);
                        //printf("CRC: %u\n", crc);
                        printf("CRCJBD: %u\n", status_CRC);
                        break;
                    }        
            // 添加其他标准帧的处理分支
            default:
                printf("Received unknown standard data frame with ID: 0x%X\n", message->rx_sfid);
                break;
        }

        }
        else
        {
            printf("Received unknown standard remote frame with ID: 0x%X\n", message->rx_sfid);            
        }


    } 
    else if(message->rx_ff == CAN_FF_EXTENDED)
    {
        if(message->rx_ft == CAN_FT_DATA)
        {
            switch (message->rx_efid)
            {
                case 0x18FF50E5:
                {   
                    g_can_charge_controller1.output_voltage = extract_uint16(message->rx_data, 0);
                    g_can_charge_controller1.output_current = extract_uint16(message->rx_data, 2);
                    g_can_charge_controller1.CanChargeController_status = extract_uint8(message->rx_data, 4);
                    printf("Received CAN message with ID 0x18FF50E5\n");
                    printf("Output Voltage: %u mV\n", g_can_charge_controller1.output_voltage * 100);
                    printf("Output Current: %u mA\n", g_can_charge_controller1.output_current * 100);
                    printf("Status: %u\n", g_can_charge_controller1.CanChargeController_status);
                    break;
                }
            } 
        }

    }
    else
    {
        printf("Received unknown frame");
    }
}


void processDeviceCANReceiveMessage(can_receive_message_struct* message)
{
    CanBms *bms_ptr = 0;
    CanChargeController *chargeController_ptr = 0;

    // Find devices by device ID and device type
    bms_ptr = (CanBms*)findDevice(&deviceList_translator, DEVICE_TYPE_CAN_BMS, 0);
    chargeController_ptr = (CanChargeController*)findDevice(&deviceList_translator, DEVICE_TYPE_CAN_CHARGE_CONTROLLER, 0);

    if (message->rx_ff == CAN_FF_STANDARD) 
    {
        if(message->rx_ft == CAN_FT_DATA)
        {
        switch (message->rx_sfid) 
        {
            case 0x100:
                // 处理总电压、电流、剩余容量的数据
                {
                    uint16_t total_voltage = extract_uint16(message->rx_data, 0);
                    int16_t current = extract_int16(message->rx_data, 2);
                    uint16_t u16_current = extract_uint16(message->rx_data, 2);
                    uint16_t remaining_capacity = extract_uint16(message->rx_data, 4);
                    uint8_t charging_flag = extract_uint8(message->rx_data, 6);

                    bms_ptr->total_voltage = extract_uint16(message->rx_data, 0);
                    bms_ptr->current = extract_int16(message->rx_data, 2);
                    bms_ptr->u16_current = extract_uint16(message->rx_data, 2);
                    bms_ptr->remaining_capacity = extract_uint16(message->rx_data, 4);
                    bms_ptr->charging_flag = extract_uint8(message->rx_data, 6);

                    g_can_bms_jbd1.bms.total_voltage = extract_uint16(message->rx_data, 0);
                    g_can_bms_jbd1.bms.current = extract_int16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.u16_current = extract_uint16(message->rx_data, 2);
                    g_can_bms_jbd1.bms.remaining_capacity = extract_uint16(message->rx_data, 4);
                    g_can_bms_jbd1.bms.charging_flag = extract_uint8(message->rx_data, 6);

                    // printf("Received CAN message with ID 0x100\n");
                    // printf("Total Voltage: %u mV\n", total_voltage * 10);
                    // printf("Current: %d mA\n", current * 10);
                    // printf("Current: %u mA\n", u16_current * 10);
                    // printf("Remaining Capacity: %u mAh\n", remaining_capacity * 10);
                    // printf("Charging Flag: %u\n", charging_flag);

                    printf("Received CAN message with ID 0x100\n");
                    printf("Total Voltage: %u mV\n", g_can_bms_jbd1.bms.total_voltage * 10);
                    printf("Current: %d mA\n", g_can_bms_jbd1.bms.current * 10);
                    printf("Current: %u mA\n", g_can_bms_jbd1.bms.u16_current * 10);
                    printf("Remaining Capacity: %u mAh\n", g_can_bms_jbd1.bms.remaining_capacity * 10);
                    printf("Charging Flag: %u\n", g_can_bms_jbd1.bms.charging_flag);

                    printf("Received CAN message with ID 0x100\n");
                    printf("Total Voltage: %u mV\n", bms_ptr->total_voltage * 10);
                    printf("Current: %d mA\n", bms_ptr->current * 10);
                    printf("Current: %u mA\n", bms_ptr->u16_current * 10);
                    printf("Remaining Capacity: %u mAh\n", bms_ptr->remaining_capacity * 10);
                    printf("Charging Flag: %u\n", bms_ptr->charging_flag);

                    break;
                }
            case 0x101:
                {   
                    // uint16_t full_capacity = extract_uint16(message->rx_data, 0);
                    // uint16_t discharge_cycles = extract_uint16(message->rx_data, 2);
                    // uint16_t rsoc = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->full_capacity = extract_uint16(message->rx_data, 0);
                    bms_ptr->discharge_cycles = extract_uint16(message->rx_data, 2);
                    bms_ptr->rsoc = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x101\n");
                    printf("Full Capacity: %u mAh\n", bms_ptr->full_capacity * 10);
                    printf("Discharge Cycles: %u\n", bms_ptr->discharge_cycles);
                    printf("RSOC: %u\n", bms_ptr->rsoc);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x102:
                {
                    // uint16_t cell_balance_status_1 = extract_uint16(message->rx_data, 0);
                    // uint16_t cell_balance_status_2 = extract_uint16(message->rx_data, 2);
                    // uint16_t protection_flags = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->cell_balance_status_1 = extract_uint16(message->rx_data, 0);
                    bms_ptr->cell_balance_status_2 = extract_uint16(message->rx_data, 2);
                    bms_ptr->protection_flags = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x102\n");
                    printf("Cell Balance Status 1: %u\n", bms_ptr->cell_balance_status_1);
                    printf("Cell Balance Status 2: %u\n", bms_ptr->cell_balance_status_2);
                    printf("the protection sign: %u\n", bms_ptr->protection_flags);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x103:
                {
                    // uint16_t mos_status = extract_uint16(message->rx_data, 0);
                    // uint16_t production_date = extract_uint16(message->rx_data, 2);
                    // uint16_t software_version = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->mos_status = extract_uint16(message->rx_data, 0);
                    bms_ptr->production_date = extract_uint16(message->rx_data, 2);
                    bms_ptr->software_version = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x103\n");
                    printf("MOS Status: %u\n", bms_ptr->mos_status);
                    printf("Production Date: %u\n", bms_ptr->production_date);
                    printf("Software Version: %u\n", bms_ptr->software_version);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;

                }
            case 0x104:
                {
                    // uint8_t num_battery_strings = extract_uint8(message->rx_data, 0);
                    // uint8_t num_ntc_probes = extract_uint8(message->rx_data, 1);
                    uint16_t crc = Check_CRC16(message->rx_data, 2);
                    uint8_t status_CRC = CRC16Verification(message->rx_data,2);

                    bms_ptr->num_battery_strings = extract_uint8(message->rx_data, 0);
                    bms_ptr->num_ntc_probes = extract_uint8(message->rx_data, 1);

                    printf("Received CAN message with ID 0x104\n");
                    printf("Number of Battery Strings: %u\n", bms_ptr->num_battery_strings);
                    printf("Number of NTC Probes: %u\n", bms_ptr->num_ntc_probes);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x105:
                {
                    // uint16_t ntc1_temperature = extract_uint16(message->rx_data, 0);
                    // uint16_t ntc2_temperature = extract_uint16(message->rx_data, 2);
                    // uint16_t ntc3_temperature = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->ntc_temperature[0] = extract_uint16(message->rx_data, 0);
                    bms_ptr->ntc_temperature[1] = extract_uint16(message->rx_data, 2);
                    bms_ptr->ntc_temperature[2] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x105\n");
                    printf("NTC1 Temperature: %u\n", bms_ptr->ntc_temperature[0]);
                    printf("NTC2 Temperature: %u\n", bms_ptr->ntc_temperature[1]);
                    printf("NTC3 Temperature: %u\n", bms_ptr->ntc_temperature[2]);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;

                }
            case 0x106:
                {
                    // uint16_t ntc4_temperature = extract_uint16(message->rx_data, 0);
                    // uint16_t ntc5_temperature = extract_uint16(message->rx_data, 2);
                    //uint16_t ntc6_temperature = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 4);
                    uint8_t status_CRC = CRC16Verification(message->rx_data, 4);

                    bms_ptr->ntc_temperature[3] = extract_uint16(message->rx_data, 0);
                    bms_ptr->ntc_temperature[4] = extract_uint16(message->rx_data, 2);

                    printf("Received CAN message with ID 0x106\n");
                    printf("NTC4 Temperature: %u\n", bms_ptr->ntc_temperature[3]);
                    printf("NTC5 Temperature: %u\n", bms_ptr->ntc_temperature[4]);
                    
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x107:
                {
                    // uint16_t cell1_voltage = extract_uint16(message->rx_data, 0);
                    // uint16_t cell2_voltage = extract_uint16(message->rx_data, 2);
                    // uint16_t cell3_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);



                    bms_ptr->cell_voltage[0] = extract_uint16(message->rx_data, 0);
                    bms_ptr->cell_voltage[1] = extract_uint16(message->rx_data, 2);
                    bms_ptr->cell_voltage[2] = extract_uint16(message->rx_data, 4);


                    printf("Received CAN message with ID 0x107\n");
                    printf("Cell1 Voltage: %u\n", bms_ptr->cell_voltage[0]);
                    printf("Cell2 Voltage: %u\n", bms_ptr->cell_voltage[1]);
                    printf("Cell3 Voltage: %u\n", bms_ptr->cell_voltage[2]);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x108:
                {
                    // uint16_t cell4_voltage = extract_uint16(message->rx_data, 0);
                    // uint16_t cell5_voltage = extract_uint16(message->rx_data, 2);
                    // uint16_t cell6_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->cell_voltage[3] = extract_uint16(message->rx_data, 0);
                    bms_ptr->cell_voltage[4] = extract_uint16(message->rx_data, 2);
                    bms_ptr->cell_voltage[5] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x108\n");
                    printf("Cell4 Voltage: %u\n", bms_ptr->cell_voltage[3]);
                    printf("Cell5 Voltage: %u\n", bms_ptr->cell_voltage[4]);
                    printf("Cell6 Voltage: %u\n", bms_ptr->cell_voltage[5]);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            
            case 0x109:
                {
                    // uint16_t cell7_voltage = extract_uint16(message->rx_data, 0);
                    // uint16_t cell8_voltage = extract_uint16(message->rx_data, 2);
                    // uint16_t cell9_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->cell_voltage[6] = extract_uint16(message->rx_data, 0);
                    bms_ptr->cell_voltage[7] = extract_uint16(message->rx_data, 2);
                    bms_ptr->cell_voltage[8] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x109\n");
                    printf("Cell7 Voltage: %u\n", bms_ptr->cell_voltage[6]);
                    printf("Cell8 Voltage: %u\n", bms_ptr->cell_voltage[7]);
                    printf("Cell9 Voltage: %u\n", bms_ptr->cell_voltage[8]);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x10A:
                {
                    // uint16_t cell10_voltage = extract_uint16(message->rx_data, 0);
                    // uint16_t cell11_voltage = extract_uint16(message->rx_data, 2);
                    // uint16_t cell12_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->cell_voltage[9] = extract_uint16(message->rx_data, 0);
                    bms_ptr->cell_voltage[10] = extract_uint16(message->rx_data, 2);
                    bms_ptr->cell_voltage[11] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x10A\n");
                    printf("Cell10 Voltage: %u\n", bms_ptr->cell_voltage[9]);
                    printf("Cell11 Voltage: %u\n", bms_ptr->cell_voltage[10]);
                    printf("Cell12 Voltage: %u\n", bms_ptr->cell_voltage[11]);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x10B:
                {
                    // uint16_t cell13_voltage = extract_uint16(message->rx_data, 0);
                    // uint16_t cell14_voltage = extract_uint16(message->rx_data, 2);
                    // uint16_t cell15_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->cell_voltage[12] = extract_uint16(message->rx_data, 0);
                    bms_ptr->cell_voltage[13] = extract_uint16(message->rx_data, 2);
                    bms_ptr->cell_voltage[14] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x10B\n");
                    printf("Cell13 Voltage: %u\n", bms_ptr->cell_voltage[12]);
                    printf("Cell14 Voltage: %u\n", bms_ptr->cell_voltage[13]);
                    printf("Cell15 Voltage: %u\n", bms_ptr->cell_voltage[14]);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
            case 0x10C:
                {
                    // uint16_t cell16_voltage = extract_uint16(message->rx_data, 0);
                    // uint16_t cell17_voltage = extract_uint16(message->rx_data, 2);
                    // uint16_t cell18_voltage = extract_uint16(message->rx_data, 4);
                    uint16_t crc = Check_CRC16(message->rx_data, 6);
                    uint8_t status_CRC = CRCVerification(message);

                    bms_ptr->cell_voltage[15] = extract_uint16(message->rx_data, 0);
                    bms_ptr->cell_voltage[16] = extract_uint16(message->rx_data, 2);
                    bms_ptr->cell_voltage[17] = extract_uint16(message->rx_data, 4);

                    printf("Received CAN message with ID 0x10C\n");
                    printf("Cell16 Voltage: %u\n", bms_ptr->cell_voltage[15]);
                    printf("Cell17 Voltage: %u\n", bms_ptr->cell_voltage[16]);
                    printf("Cell18 Voltage: %u\n", bms_ptr->cell_voltage[17]);
                    //printf("CRC: %u\n", crc);
                    printf("CRCJBD: %u\n", status_CRC);
                    break;
                }
                case 0x10D:
                    {
                        uint16_t cell19_voltage = extract_uint16(message->rx_data, 0);
                        uint16_t cell20_voltage = extract_uint16(message->rx_data, 2);
                        uint16_t cell21_voltage = extract_uint16(message->rx_data, 4);
                        uint16_t crc = Check_CRC16(message->rx_data, 6);
                        uint8_t status_CRC = CRCVerification(message);

                        bms_ptr->cell_voltage[18] = extract_uint16(message->rx_data, 0);
                        bms_ptr->cell_voltage[19] = extract_uint16(message->rx_data, 2);
                        bms_ptr->cell_voltage[20] = extract_uint16(message->rx_data, 4);

                        printf("Received CAN message with ID 0x10D\n");
                        printf("Cell19 Voltage: %u\n", bms_ptr->cell_voltage[18]);
                        printf("Cell20 Voltage: %u\n", bms_ptr->cell_voltage[19]);
                        printf("Cell21 Voltage: %u\n", bms_ptr->cell_voltage[20]);
                        //printf("CRC: %u\n", crc);
                        printf("CRCJBD: %u\n", status_CRC);
                        break;
                    }
                case 0x10E:
                    {
                        uint16_t cell22_voltage = extract_uint16(message->rx_data, 0);
                        uint16_t cell23_voltage = extract_uint16(message->rx_data, 2);
                        //uint16_t cell24_voltage = extract_uint16(message->rx_data, 4);
                        //uint16_t crc = Check_CRC16(message->rx_data, 4);
                        uint8_t status_CRC = CRC16Verification(message->rx_data,4);

                        bms_ptr->cell_voltage[21] = extract_uint16(message->rx_data, 0);
                        bms_ptr->cell_voltage[22] = extract_uint16(message->rx_data, 2);

                        printf("Received CAN message with ID 0x10E\n");
                        printf("Cell22 Voltage: %u\n", bms_ptr->cell_voltage[21]);
                        printf("Cell23 Voltage: %u\n", bms_ptr->cell_voltage[22]);

                        //printf("CRC: %u\n", crc);
                        printf("CRCJBD: %u\n", status_CRC);
                        break;
                    }        
            // 添加其他标准帧的处理分支
            default:
                printf("Received unknown standard data frame with ID: 0x%X\n", message->rx_sfid);
                break;
        }

        }
        else
        {
            printf("Received unknown standard remote frame with ID: 0x%X\n", message->rx_sfid);            
        }


    } 
    else if(message->rx_ff == CAN_FF_EXTENDED)
    {
        if(message->rx_ft == CAN_FT_DATA)
        {
            switch (message->rx_efid)
            {
                case 0x18FF50E5:
                {   

                    chargeController_ptr->output_voltage = extract_uint16(message->rx_data, 0);
                    chargeController_ptr->output_current = extract_uint16(message->rx_data, 2);
                    chargeController_ptr->CanChargeController_status = extract_uint8(message->rx_data, 4);

                    printf("Received CAN message with ID 0x18FF50E5\n");
                    printf("Output Voltage: %u mV\n", chargeController_ptr->output_voltage * 100);
                    printf("Output Current: %u mA\n", chargeController_ptr->output_current * 100);
                    printf("Status: %u\n", chargeController_ptr->CanChargeController_status);
                    break;
                }
            } 
        }

    }
    else
    {
        printf("Received unknown frame");
    }

}



// void processCANTtransmitMessage(uint8_t device_id)
// {
//     switch (device_id)
//     {
//     case DEVICE_CHARGER1:
//         {
//         can_trasnmit_message_struct transmit_message;
//         // 设置 transmit_message 的成员值
//         transmit_message.tx_sfid = 0x1806E5F4;
//         transmit_message.tx_efid = 0;
//         transmit_message.tx_ff = CAN_FF_EXTENDED;
//         transmit_message.tx_ft = CAN_FT_DATA;
//         transmit_message.tx_dlen = 8;
//         transmit_message.tx_data[0] = get_high_byte(g_charging_process1.maxVoltage);
//         transmit_message.tx_data[1] = get_low_byte(g_charging_process1.maxVoltage);
//         transmit_message.tx_data[2] = get_high_byte(g_charging_process1.maxCurrent);
//         transmit_message.tx_data[3] = get_low_byte(g_charging_process1.maxCurrent);
//         transmit_message.tx_data[4] = g_charging_process1.chargerControl;
//         transmit_message.tx_data[5] = g_charging_process1.stopReason;      
//         printf("DEVICE_CHARGER1 send message1\n");
//         can_message_transmit(CAN0, &transmit_message);
//         while (can_transmit_states(CAN0, CAN_FIFO0) != CAN_TRANSMIT_OK) {}



//         transmit_message.tx_sfid = 0x1800FFF4;
//         transmit_message.tx_efid = 0;
//         transmit_message.tx_ff = CAN_FF_EXTENDED;
//         transmit_message.tx_ft = CAN_FT_DATA;
//         transmit_message.tx_dlen = 8;
//         transmit_message.tx_data[0] = get_high_byte(g_charging_process1.highestCellVoltage);
//         transmit_message.tx_data[1] = get_low_byte(g_charging_process1.highestCellVoltage);
//         transmit_message.tx_data[2] = get_high_byte(g_charging_process1.lowestCellVoltage);
//         transmit_message.tx_data[3] = get_low_byte(g_charging_process1.lowestCellVoltage);
//         transmit_message.tx_data[4] = g_charging_process1.stateOfCharge;
//         transmit_message.tx_data[5] = g_charging_process1.highestTemperature;
//         transmit_message.tx_data[6] = get_high_byte(g_charging_process1.batteryVoltage);
//         transmit_message.tx_data[7] = get_low_byte(g_charging_process1.batteryVoltage);
//         printf("DEVICE_CHARGER1 send message2\n");
//         can_message_transmit(CAN0, &transmit_message);
//         while (can_transmit_states(CAN0, CAN_FIFO0) != CAN_TRANSMIT_OK) {}

//         break;
//         }


//     case DEVICE_CHARGER2:

//         break;
//     case DEVICE_BMS_JBD1:
//         {
//             uint8_t i = 0;
//             for (i = 0; i < NUM_G_CAN_TRANSMIT_MESSAGE_TBL; i++) 
//             {
//                 can_message_transmit(CAN0, &g_can_transmit_message_JBD[i]);
//                 while (can_transmit_states(CAN0, CAN_FIFO0) != CAN_TRANSMIT_OK) {}
//             }
//         }
//         break;  

//     case DEVICE_BMS_JBD2:
//         break;

//     default:
//         break;
//     }
// }

void translater_task_init(void)
{ 
    CanBms* bms1 = (CanBms*)malloc(sizeof(CanBms));
    CanChargeController* chargeController1 = (CanChargeController*)malloc(sizeof(CanChargeController));

    translater_canInit();
    
    initializeDeviceList(&deviceList_translator);

    printf("Before initialization - bms1 address: %p\n", bms1);
    printf("Before initialization - chargeController1 address: %p\n", chargeController1);

    // 初始化设备
    deviceInitializers[DEVICE_TYPE_CAN_BMS](bms1, 0);
    deviceInitializers[DEVICE_TYPE_CAN_CHARGE_CONTROLLER](chargeController1, 0);

    printf("After initialization - bms1 address: %p\n", bms1);
    printf("After initialization - chargeController1 address: %p\n", chargeController1);

    //添加设备到设备列表
    addSingleDevice(&deviceList_translator, bms1);
    addSingleDevice(&deviceList_translator, chargeController1);

    //bms1->device.operate(bms1);
    //bms1->device.operate(bms1);

    //operateAllDevices(&deviceList_translator);

    // free(bms1);
    // bms1 = NULL;
    // free(chargeController1);
    // chargeController1 = NULL;
    printf("translater_task_init %s-%s-%u\n", __FILE__,__FUNCTION__, __LINE__);
}



void translater_task_run(void)
{
    // CanBms bms1;
    // CanChargeController chargeController1;
    // CanBms *bms_ptr = NULL;
    // CanChargeController *chargeController_ptr = NULL;
    
    // // 初始化设备
    // deviceInitializers[DEVICE_TYPE_CAN_BMS](&bms1, 1);
    // deviceInitializers[DEVICE_TYPE_CAN_CHARGE_CONTROLLER](&chargeController1, 1);

    // //添加设备到设备列表
    // initializeDeviceList(&deviceList_translator);
    // addSingleDevice(&deviceList_translator, &bms1);
    // addSingleDevice(&deviceList_translator, &chargeController1);

    // printDeviceList(&deviceList_translator);
    
    // Find devices by device ID and device type 
    // bms_ptr = (CanBms*)findDevice(&deviceList_translator, DEVICE_TYPE_CAN_BMS, 1);
    // chargeController_ptr = (CanChargeController*)findDevice(&deviceList_translator, DEVICE_TYPE_CAN_CHARGE_CONTROLLER, 1);


    // 执行设备操作

    while (1)
    {
        if(g_TimerFlag[TIMER_BMS_COM] == TRUE)
        {
            //printf("bms_ptr->device.operate(bms_ptr)\n");
            //bms_ptr->device.operate(bms_ptr);  

            //operateAllDevices(&deviceList_translator);

            g_TimerFlag[TIMER_BMS_COM] = FALSE;
        }
        //bms_ptr->device.operate(bms_ptr);
        //delay_1ms(1000);
    }
}
