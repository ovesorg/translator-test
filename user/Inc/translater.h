#ifndef __translater_H__
#define __translater_H__




#define num_received_messages 20;
//#define NUM_G_CAN_RECEIVE_MESSAGE_JBD 0x20;


// 定义设备类型的枚举
typedef enum {
    DEVICE_TYPE_CAN_BMS,
    DEVICE_TYPE_CAN_CHARGE_CONTROLLER,
    // 添加其他设备类型...
} DeviceType;


// 定义设备状态枚举
typedef enum {
    STATUS_NORMAL = 0x01,
    STATUS_SLEEP,
    STATUS_WAITING,
    STATUS_PROTECT,
    STATUS_ERROR,
} Status;


// 定义设备初始化函数指针类型
typedef void (*DeviceInitializer)(void*,uint8_t device_id);

// 定义设备操作函数指针类型
typedef void (*DeviceOperation)(void*);
//typedef void (*DeviceOperation)(void);

//定义销毁函数指针类型
typedef void (*DeviceDestroyer)(void*);


/*定义设备的结构体*/
typedef struct {
    uint8_t device_id;          // 设备ID
    uint8_t status;             // 设备系统状态
    DeviceType device_type;            // 设备类型
    uint16_t error_counter_rx;  // 接收错误计数器
    uint16_t error_counter_tx;  // 发送错误计数器
    // void (*initialize)(void* device); // 设备初始化函数指针
    // void (*operate)(void* device);    // 设备操作函数指针
    DeviceInitializer initialize; // 设备初始化函数指针
    DeviceOperation operate;    // 设备操作函数指针
    DeviceDestroyer destroy;    // 设备销毁函数指针

} CanDevice;


// CanBms结构体
typedef struct {
    CanDevice device;
    uint16_t total_voltage;           // BYTE0 ~ 1 for total voltage, unsigned, high byte first, in 10mV
    int16_t current;                  // BYTE2 ~ 3 for current, signed, in 10mA, positive for charging, negative for discharge
    uint16_t u16_current;             // BYTE2 ~ 3 for current, unsigned, in 10mA, positive for charging, negative for discharge
    uint16_t remaining_capacity;      // BYTE4 ~ 5 for remaining capacity, unsigned, in 10mAh
    uint8_t charging_flag;            // BYTE6 for charging flag, 0 is not charging, 1 is charging

    uint16_t full_capacity;           // BYTE8 ~ 9 is full capacity, unsigned, high byte first, in 10mAh
    uint16_t discharge_cycles;        // BYTE10 ~ 11 is the number of discharge cycles, unsigned, unit is 1
    uint16_t rsoc;                    // BYTE12 ~ 13 for remaining capacity (RSOC), unsigned, in units of %

    uint16_t cell_balance_status_1;    // BYTE0 ~ 1 for balance status of cell1 ~ cell16, 1 is open, 0 is not open
    uint16_t cell_balance_status_2;    // BYTE2 ~ 3 for balance status of cell17 ~ cell33, 1 is open, 0 is not open
    uint16_t protection_flags;         // BYTE4 ~ 5 for protection flags, see note 1

    uint16_t mos_status;              // BYTE14 ~ 15 for MOS tube status
    uint16_t production_date;         // BYTE16 ~ 17 for production date
    uint16_t software_version;        // BYTE18 ~ 19 for software version

    uint8_t num_battery_strings;      // BYTE20 for the number of battery strings
    uint8_t num_ntc_probes;           // BYTE21 for the number of NTC probes

    uint16_t ntc_temperature[10];        
    uint16_t cell_voltage[30];

} CanBms;


// CanChargeController
typedef struct {
    CanDevice device;
    uint16_t output_voltage;//100mV
    uint16_t output_current;//100mA
    uint8_t CanChargeController_status;
} CanChargeController;



// CanJbdBms结构体
typedef struct 
{
    CanBms bms;
    uint16_t ntc1_temperature;        // BYTE22 ~ 23 for temperature value of NTC1, unsigned, in 0.1K
    uint16_t ntc2_temperature;        // BYTE24 ~ 25 for temperature value of NTC2, unsigned, in 0.1K
    uint16_t ntc3_temperature;        // BYTE26 ~ 27 for temperature value of NTC3, unsigned, in 0.1K
    uint16_t ntc4_temperature;        // BYTE28 ~ 29 for temperature value of NTC4, unsigned, in 0.1K
    uint16_t ntc5_temperature;        // BYTE30 ~ 31 for temperature value of NTC5, unsigned, in 0.1K
    uint16_t ntc6_temperature;        // BYTE32 ~ 33 for temperature value of NTC6, unsigned, in 0.1K
    uint16_t cell1_voltage;           // BYTE34 ~ 35 for Cell1 voltage, unsigned, high byte first, in mV
    uint16_t cell2_voltage;           // BYTE36 ~ 37 for Cell2 voltage, unsigned, high byte first, in mV
    uint16_t cell3_voltage;           // BYTE38 ~ 39 for Cell3 voltage, unsigned, high byte first, in mV
    uint16_t cell4_voltage;           // BYTE40 ~ 41 for Cell4 voltage, unsigned, high byte first, in mV
    uint16_t cell5_voltage;           // BYTE42 ~ 43 for Cell5 voltage, unsigned, high byte first, in mV
    uint16_t cell6_voltage;           // BYTE44 ~ 45 for Cell6 voltage, unsigned, high byte first, in mV
    uint16_t cell7_voltage;           // BYTE46 ~ 47 for Cell7 voltage, unsigned, high byte first, in mV
    uint16_t cell8_voltage;           // BYTE48 ~ 49 for Cell8 voltage, unsigned, high byte first, in mV
    uint16_t cell9_voltage;           // BYTE50 ~ 51 for Cell9 voltage, unsigned, high byte first, in mV
    uint16_t cell10_voltage;          // BYTE52 ~ 53 for Cell10 voltage, unsigned, high byte first, in mV
    uint16_t cell11_voltage;          // BYTE54 ~ 55 for Cell11 voltage, unsigned, high byte first, in mV
    uint16_t cell12_voltage;          // BYTE56 ~ 57 for Cell12 voltage, unsigned, high byte first, in mV
    uint16_t cell13_voltage;          // BYTE58 ~ 59 for Cell13 voltage, unsigned, high byte first, in mV
    uint16_t cell14_voltage;          // BYTE60 ~ 61 for Cell14 voltage, unsigned, high byte first, in mV
    uint16_t cell15_voltage;          // BYTE62 ~ 63 for Cell15 voltage, unsigned, high byte first, in mV
    uint16_t cell16_voltage;          // BYTE64 ~ 65 for Cell16 voltage, unsigned, high byte first, in mV
    uint16_t cell17_voltage;          // BYTE66 ~ 67 for Cell17 voltage, unsigned, high byte first, in mV
    uint16_t cell18_voltage;          // BYTE68 ~ 69 for Cell18 voltage, unsigned, high byte first, in mV
    uint16_t cell19_voltage;          // BYTE70 ~ 71 for Cell19 voltage, unsigned, high byte first, in mV
    uint16_t cell20_voltage;          // BYTE72 ~ 73 for Cell20 voltage, unsigned, high byte first, in mV
    uint16_t cell21_voltage;          // BYTE74 ~ 75 for Cell21 voltage, unsigned, high byte first, in mV
    uint16_t cell22_voltage;          // BYTE76 ~ 77 for Cell22 voltage, unsigned, high byte first, in mV
    uint16_t cell23_voltage;          // BYTE78 ~ 79 for Cell23 voltage, unsigned, high byte first, in mV
    uint16_t cell24_voltage;          // BYTE80 ~ 81 for Cell24 voltage, unsigned, high byte first, in mV
    uint16_t cell25_voltage;          // BYTE82 ~ 83 for Cell25 voltage, unsigned, high byte first, in mV
    
} CanBmsJbd;


typedef struct {
    uint16_t maxVoltage;    // 最高允许充电端电压
    uint16_t maxCurrent;    // 最高允许充电电流
    uint8_t chargerControl;    // 充电机控制
    uint8_t stopReason;     // BMS 发送停止命令原因
    uint16_t highestCellVoltage;    // 单体最高电压
    uint16_t lowestCellVoltage;     // 单体最低电压
    uint8_t stateOfCharge;          // SOC
    int8_t highestTemperature;      // 最高温度
    uint16_t batteryVoltage;        // 电池组电压
} ChargingProcess;


typedef struct LinkedList {
    CanDevice* head;
} LinkedList;






uint16_t extract_uint16(uint8_t* data, uint8_t index);
int16_t extract_int16(uint8_t* data, uint8_t index);
uint32_t extract_uint32(uint8_t* data, uint8_t index);



void can_gpio_config(void);
void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter);
void translater_canInit(void);


void init_transmit_message(can_trasnmit_message_struct* transmit_message, uint32_t tx_sfid, uint32_t tx_efid,  uint8_t tx_ff, uint8_t tx_ft, uint8_t tx_dlen, uint8_t* data);
void init_receive_message(can_receive_message_struct* receive_message, uint32_t rx_sfid, uint32_t rx_efid,  uint8_t rx_ff, uint8_t rx_ft, uint8_t rx_dlen, uint8_t* data);

void translater_can_sendMessage(uint32_t canId, uint8_t* data, uint8_t length);
void translater_can_receiveMessage(uint32_t canId, uint8_t* data, uint8_t length);


// CanBms设备的初始化函数
void initializeCanBms(void* device,uint8_t device_id); 
// CanChargeController设备的初始化函数
void initializeCanChargeController(void* device,uint8_t device_id);
// CanBms设备的销毁函数
void destroyCanBms(void* device);
// CanChargeController设备的销毁函数
void destroyCanChargeController(void* device);
// CanBms设备的操作函数
void operateCanBms(void* device);
// CanChargeController设备的操作函数
void operateCanChargeController(void* device);

void processCANReceiveMessage(can_receive_message_struct* message);
void processCANTtransmitMessage(uint8_t device_id);

void processDeviceCANReceiveMessage(can_receive_message_struct* message);



void translater_task_init(void);
void translater_task_run(void);




#endif /* __translater_H__ */

