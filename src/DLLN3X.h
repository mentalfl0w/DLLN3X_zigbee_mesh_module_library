#ifndef _DLLN3X_H_
#define _DLLN3X_H_

#include <Arduino.h>

typedef struct zigbee_frame
{
	uint8_t length;
	uint8_t src_port;
	uint8_t dis_port;
	uint8_t remote_addrL;
	uint8_t remote_addrH;
	uint8_t data[59];	
}zigbee_frame;

#define new_zigbee_frame(num)	\
struct 						\
{							\
	uint8_t length;			\
	uint8_t src_port;			\
	uint8_t dis_port;			\
	uint8_t remote_addrL;\
	uint8_t remote_addrH;\
	uint8_t data[num];		\
}

class DLLN3X
{
private:
    void _clear();
    bool _online = false, _recv_lock = false;
    int _pack(unsigned char buf[], unsigned char data[], int length);
    int _depack(unsigned char buf[], unsigned char data[], int length);
    HardwareSerial *_DSerial;
    void (*_callback) (unsigned char orig_port,
                    unsigned char dest_port, unsigned short addr,
                    unsigned char data[], int length)=nullptr;

public:
    DLLN3X();
    ~DLLN3X();
    void init(HardwareSerial *DSerial = &Serial1);
    void recv(zigbee_frame *frame);
    bool recv(unsigned char *orig_port,
              unsigned char *dest_port, unsigned short *addr,
              unsigned char data[], int *length);
    bool send(unsigned char orig_port,
              unsigned char dest_port, unsigned short addr,
              unsigned char data[], int length);
    void rled_blink(unsigned char orig_port = 0x80,
                    unsigned short addr = 0x00, unsigned char time = 50);
    bool send_msg(unsigned char dest_port, unsigned short addr,
                  unsigned char data[], int length, unsigned char orig_port = 0x80);
    void loop();
    void setcallback(void (*callback)(unsigned char orig_port,
                    unsigned char dest_port, unsigned short addr,
                    unsigned char data[], int length));
    unsigned short read_addr();
    unsigned short self_addr = 0x0000;
};
#endif