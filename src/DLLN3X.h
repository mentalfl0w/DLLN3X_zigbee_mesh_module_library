#ifndef _DLLN3X_H_
#define _DLLN3X_H_

#include <Arduino.h>
#include <SoftwareSerial.h>

typedef struct zigbee_frame
{
	uint8_t length;
	uint8_t src_port;
	uint8_t des_port;
	uint8_t remote_addrL;
	uint8_t remote_addrH;
	uint8_t data[59];	
}zigbee_frame;

#define new_zigbee_frame(num)	\
struct 						\
{							\
	uint8_t length;			\
	uint8_t src_port;			\
	uint8_t des_port;			\
	uint8_t remote_addrL;\
	uint8_t remote_addrH;\
	uint8_t data[num];		\
}

class DLLN3X
{
private:
    void _clear();
    bool _online = false, _recv_lock = false;
    int _pack(uint8_t buf[], uint8_t data[], int length);
    int _depack(uint8_t buf[], uint8_t data[], int length);
    Stream *_DSerial;
    void (*_callback) (uint8_t orig_port,
                    uint8_t dest_port, uint16_t addr,
                    uint8_t data[], int length)=nullptr;

public:
    DLLN3X();
    ~DLLN3X();
    void init(HardwareSerial *DSerial);
    void init(SoftwareSerial *DSerial);
    void recv(zigbee_frame *frame);
    bool recv(uint8_t *orig_port,
              uint8_t *dest_port, uint16_t *addr,
              uint8_t data[], int *length);
    bool send(uint8_t orig_port,
              uint8_t dest_port, uint16_t addr,
              uint8_t data[], int length);
    bool send(zigbee_frame *frame);
    void rled_blink(uint8_t orig_port = 0x80,
                    uint16_t addr = 0x00, uint8_t time = 50);
    void loop();
    void setcallback(void (*callback)(uint8_t orig_port,
                    uint8_t dest_port, uint16_t addr,
                    uint8_t data[], int length));
    uint16_t read_addr();
    uint16_t self_addr = 0x0000;
};
#endif
