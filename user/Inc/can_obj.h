#ifndef __can_obj_h__
#define __can_obj_h__

#define MAX_BUFFER_SIZE 0x12

typedef struct {
    can_trasnmit_message_struct* messages;
    uint8_t size;
    uint8_t capacity;
    uint8_t front;
    uint8_t rear;
} thread_safe_queue;

// Define the Device structure that holds a pointer to the specific device structure
typedef struct Device {
    void* instance;  // Pointer to the specific device structure (CanBms, CanChargeController, etc.)
    struct Device* next;
} Device;


// Define the DeviceList structure
typedef struct DeviceList {
    Device* head;
} DeviceList;







void init_queue(thread_safe_queue* queue, uint8_t capacity);
void enqueue(thread_safe_queue* queue, can_trasnmit_message_struct message);
can_trasnmit_message_struct dequeue(thread_safe_queue* queue);
void send_messages(void);
void enqueue_message(uint32_t sfid, uint32_t efid, uint8_t ff, uint8_t ft, uint8_t dlen, uint8_t* data);
void enqueue_const_message(uint32_t sfid, uint32_t efid, uint8_t ff, uint8_t ft, uint8_t dlen, const uint8_t* data); 

void initializeDeviceList(DeviceList* deviceList);
void addDevice(DeviceList* list, void* instance);
void* findDevice(DeviceList* list, DeviceType type, uint8_t device_id);
void destroyDeviceList(DeviceList* list);
void addSingleDevice(DeviceList* list, void* instance);
void removeSingleDevice(DeviceList* list, void* instance);
void operateAllDevices(DeviceList* list);


void printDeviceList(DeviceList* deviceList);







#endif /* __can_obj_h__ */

