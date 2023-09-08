#include "main.h"


thread_safe_queue transmit_buffer;

DeviceList deviceList_translator = {0};


void init_queue(thread_safe_queue* queue, uint8_t capacity) {
    queue->messages = (can_trasnmit_message_struct*)malloc(capacity * sizeof(can_trasnmit_message_struct));
    queue->size = 0;
    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = 0;
}

void enqueue(thread_safe_queue* queue, can_trasnmit_message_struct message) {
    if (queue->size >= queue->capacity) {
        // 队列已满，无法入队
        return;
    }
    queue->messages[queue->rear] = message;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
}

can_trasnmit_message_struct dequeue(thread_safe_queue* queue) 
{
    can_trasnmit_message_struct message;
    if (queue->size <= 0) {
        // 队列为空，无法出队
        can_trasnmit_message_struct empty_message = {0}; // 返回一个空的消息结构体
        return empty_message;
    }
    message = queue->messages[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return message;
}

void send_messages() 
{
    while (transmit_buffer.size>0) 
    {
        
        can_trasnmit_message_struct message = dequeue(&transmit_buffer);
        can_message_transmit(CAN0, &message);
        while (can_transmit_states(CAN0, CAN_FIFO0) != CAN_TRANSMIT_OK) {}

        if (transmit_buffer.size <= 0) 
        {
            // 队列为空，停止发送线程
            break;
        }
        //执行发送消息的操作


    }
}

void  enqueue_message(uint32_t sfid, uint32_t efid, uint8_t ff, uint8_t ft, uint8_t dlen, uint8_t* data) 
{
    uint8_t i = 0;
    can_trasnmit_message_struct message;
    message.tx_sfid = sfid;
    message.tx_efid = efid;
    message.tx_ff = ff;
    message.tx_ft = ft;
    message.tx_dlen = dlen;
    for (i = 0; i < dlen; i++) {
        message.tx_data[i] = data[i];
    }
    enqueue(&transmit_buffer, message);
}

void enqueue_const_message(uint32_t sfid, uint32_t efid, uint8_t ff, uint8_t ft, uint8_t dlen, const uint8_t* data) 
{
    //uint8_t i = 0;
    can_trasnmit_message_struct message;
    message.tx_sfid = sfid;
    message.tx_efid = efid;
    message.tx_ff = ff;
    message.tx_ft = ft;
    message.tx_dlen = dlen;
    memcpy(message.tx_data, data, dlen);
    enqueue(&transmit_buffer, message);
}

// Function to initialize the device list
void initializeDeviceList(DeviceList* deviceList) {
    deviceList->head = NULL;
}

// Function to add a new device to the list
void addDevice(DeviceList* list, void* instance) {
    Device* newDevice = (Device*)malloc(sizeof(Device));
    if (newDevice == NULL) {
        // Handle memory allocation failure
        printf("Memory allocation failed:%s-%u\n", __FUNCTION__, __LINE__);
        return;
    }

    newDevice->instance = instance;
    newDevice->next = list->head;
    list->head = newDevice;
}

// Function to find a device in the list by device ID and device type
void* findDevice(DeviceList* list, DeviceType device_type, uint8_t device_id ) {
    Device* current = list->head;

    if (list == NULL || list->head == NULL) 
    {
        printf("find nothing:%s-%u\n", __FUNCTION__, __LINE__);
        return NULL; // Empty list or invalid list
    }

    while (current != NULL) 
    {
        
        // switch (device_type)
        // {
        // case DEVICE_TYPE_CAN_BMS:
        //     {
        //         CanBms* device = (CanBms*)(current->instance);
        //         if (device != NULL && device->device.device_id == device_id) {
        //             return device;
        //         }
        //         break;
        //     }
        // case DEVICE_TYPE_CAN_CHARGE_CONTROLLER:
        //     {
        //         CanChargeController* device = (CanChargeController*)(current->instance);
        //         if (device != NULL && device->device.device_id == device_id) {
        //             return device;
        //         }
        //         break;
        //     }
        // default:
        //     {
        //         printf("Invalid device type\n");
        //         break;
        //     }
            
        // }

        CanDevice* device = (CanDevice*)(current->instance);
        if (device != NULL && device->device_id == device_id && device->device_type == device_type) {
            return current->instance;
        }
        current = current->next;
    }

    return NULL; // Device not found
}

// Function to destroy the device list and free memory
void destroyDeviceList(DeviceList* list) 
{
    Device* current = list->head;
    while (current != NULL) {
        Device* next = current->next;
        free(current->instance);
        free(current);
        current = next;
    }
    list->head = NULL;
}

// Function to add a single device to the list
void addSingleDevice(DeviceList* list, void* instance) 
{
    Device* newDevice = (Device*)malloc(sizeof(Device));
    if (instance == NULL) 
    {
        // Invalid parameters, return early
        printf("Null parameters %s-%s-%u\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    if (newDevice == NULL) {
        // Handle memory allocation failure
        printf("Memory allocation failed\n");
        return;
    }

    newDevice->instance = instance;
    newDevice->next = list->head;
    list->head = newDevice;
    printDeviceList(list);
}

// Function to remove a single device from the list
void removeSingleDevice(DeviceList* list, void* instance) 
{
    Device* current = list->head;
    Device* prev = NULL;
    if (list == NULL || instance == NULL || list->head == NULL) 
    {
        // Invalid parameters or the list is empty, return early
        return;
    }

    // Traverse the list to find the device
    while (current != NULL) {
        if (current->instance == instance) {
            // Device found, remove it from the list
            if (prev == NULL) {
                // If the device is at the head of the list
                list->head = current->next;
            } else {
                prev->next = current->next;
            }

            // Free the memory allocated for the Device node
            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }

    // Device not found in the list

}


// Function to operate all devices in the list
void operateAllDevices(DeviceList* list) 
{
    Device* current = list->head;
    if (list == NULL || list->head == NULL) {
        // Empty list or invalid list, return early
        printf("Device list is empty.\n");
        return;
    }
    while (current != NULL) {
        CanDevice* device = (CanDevice*)(current->instance);
        if (device != NULL && device->operate != NULL) {
            // 调用设备的操作函数，实际执行的是与设备类型对应的操作函数
            printf("operate device type:%u device id:%u \n",device->device_type, device->device_id);
            device->operate(current->instance);
        }
        current = current->next;
    }
    printf("operate all devices %s-%s-%u\n", __FILE__, __FUNCTION__, __LINE__);
}

// Function to print the device list
void printDeviceList(DeviceList* deviceList) 
{
    Device* current = deviceList->head;
    if (deviceList == NULL || deviceList->head == NULL) 
    {
        printf("Device list is empty.\n");
        return;
    }
    printf("Printing Device List:\n");
    while (current != NULL) {
        CanDevice* device = (CanDevice*)(current->instance);
        printf("Device ID: %d, Device Type: %d\n", device->device_id, device->device_type);
        current = current->next;
    }
}
