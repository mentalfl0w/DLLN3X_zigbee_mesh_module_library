#include "DLLN3X.h"

DLLN3X::DLLN3X(){}
DLLN3X::~DLLN3X() {}

void DLLN3X::init(HardwareSerial *DSerial)
{
    _DSerial = DSerial;
    _DSerial->begin(115200);
    rled_blink();
    _clear();
    read_addr();
}

void DLLN3X::_clear()
{
    while (_DSerial->available())
        _DSerial->read();
}

int DLLN3X::_pack(unsigned char buf[], unsigned char data[], int length)
{
    int count = 0;
    
    for (int i = 0; i < length; i++)
    {
        if (data[i] == 0xFE)
        {
            buf[count++] = data[i];
            buf[count++] = 0xFC;
        }
        else if (data[i] == 0xFF)
        {
            buf[count++] = 0xFE;
            buf[count++] = 0xFD;
        }
        else
        {
            buf[count++] = data[i];
        }
    }
    return count;
}

int DLLN3X::_depack(unsigned char buf[], unsigned char data[], int length)
{
    int count = 0;
    
    for (int i = 0; i < length; i++)
    {
        if (buf[i] != 0xFE)
        {
            data[count++] = buf[i];
        }
        else if (buf[i + 1] == 0xFD)
        {
            data[count++] = 0xFF;
            i++;
        }
        else if (buf[i + 1] == 0xFC)
        {
            data[count++] = 0xFE;
            i++;
        }
    }
    return count;
}

void DLLN3X::recv(zigbee_frame *frame)
{
    _recv_lock = true;
    _DSerial->readBytesUntil(0xFF, (char *)frame, 64);
    _DSerial->read();
    _recv_lock = false;
}

bool DLLN3X::recv(unsigned char *orig_port,
                  unsigned char *dest_port, unsigned short *addr,
                  unsigned char data[], int *length)
{
    _recv_lock = true;
    unsigned char buf[200] = "";
    unsigned char head = 0, tail = 0;
    head = _DSerial->read();
    if (head == 0xFE)
    {
        *length = _DSerial->read();
        *length -= 4;
        *orig_port = _DSerial->read();
        *dest_port = _DSerial->read();
        *addr = _DSerial->read();
        *addr += _DSerial->read() << 8;
        int count = 0;
        for (int i = 0; i < *length; i++)
        {
            buf[count++] = _DSerial->read();
            if (buf[count-1]==0xFE)
            {
                buf[count++]=_DSerial->read();
            }
        }
        *length = count;
        _DSerial->read(); // read pkg tail
        *length = _depack(buf, data, *length);
        _recv_lock = false;
        return true;
    }
    else
    {
        _recv_lock = false;
        return false;
    }
}

bool DLLN3X::send(unsigned char orig_port,
                  unsigned char dest_port, unsigned short addr,
                  unsigned char data[], int length)
{
    if (orig_port<0x80)
        return false;
    
    unsigned char send_buf[208] = {0xFE}, buf[200] = "";
    unsigned char head = 0, buf_length = 0;
    buf_length = _pack(buf, data, length);
    send_buf[1] = length+4;
    send_buf[2] = orig_port;
    send_buf[3] = dest_port;
    send_buf[4] = addr & 0xFF;
    send_buf[5] = (addr & 0xFF00) >> 8;
    memcpy(send_buf + 6, buf, buf_length);
    send_buf[6 + buf_length] = 0xFF;
    return _DSerial->write(send_buf, buf_length + 7);
}

void DLLN3X::rled_blink(unsigned char orig_port, unsigned short addr, unsigned char time)
{
    uint8_t gap = 5;
    for (int i = 0; i < time; i += gap)
    {
        if (i%2!=0)
            send(orig_port, 0x20, addr, &gap, 1);
        else
            delay(gap*100);
    }
}

unsigned short DLLN3X::read_addr()
{
    if (self_addr!=0)
        return self_addr;
    unsigned char orig_port = 0xFF, dest_port = 0xFF;
    unsigned char arg = 0x01;
    unsigned short addr = 0xFFFF;
    unsigned char data[60] = "";
    int length = 0;
    if (!_online)
    {
        _clear();
        _online = true;
    }
    send(0x80, 0x21, 0x0000, &arg, 1);
    recv(&orig_port, &dest_port, &addr, data, &length);
    if (orig_port!=0x21||dest_port!=0x80||addr !=0x0000||length!=3||data[0]!=0x21)
    {
        return 0;
    }
    else
    {
        this->self_addr = data[1] + (data[2] << 8);
        return this->self_addr;
    }
}

bool DLLN3X::send_msg(unsigned char dest_port, unsigned short addr,
                  unsigned char data[], int length, unsigned char orig_port)
{
    return send(orig_port, dest_port, addr, data, length);
}

void DLLN3X::loop()
{
    unsigned char orig_port = 0xFF, dest_port = 0xFF;
    unsigned short addr = 0xFFFF;
    unsigned char data[200] = "";
    int length = 0;
    if (_DSerial->available()>0&&!_recv_lock)
    {
        recv(&orig_port,&dest_port,&addr,data,&length);
        Serial.print("Message: ");
        for (int i = 0; i < length;i++)
        {
            Serial.printf("%X ", data[i]);
        }
        Serial.printf("at port %X from %X:%X\n", dest_port, addr, orig_port);
        if (_callback!=nullptr)
            _callback(orig_port, dest_port, addr, data, length);
    }
}

void DLLN3X::setcallback(void (*callback)(unsigned char orig_port,
                    unsigned char dest_port, unsigned short addr,
                    unsigned char data[], int length))
{
    _callback = callback;
}