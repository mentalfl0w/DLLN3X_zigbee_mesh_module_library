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

int DLLN3X::_pack(uint8_t buf[], uint8_t data[], int length)
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

int DLLN3X::_depack(uint8_t buf[], uint8_t data[], int length)
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

bool DLLN3X::recv(uint8_t *orig_port,
                  uint8_t *dest_port, uint16_t *addr,
                  uint8_t data[], int *length)
{
    _recv_lock = true;
    uint8_t buf[200] = "";
    uint8_t head = 0, tail = 0;
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

bool DLLN3X::send(uint8_t orig_port,
                  uint8_t dest_port, uint16_t addr,
                  uint8_t data[], int length)
{
    if (orig_port<0x80)
        return false;
    
    uint8_t send_buf[208] = {0xFE}, buf[200] = "";
    uint8_t head = 0, buf_length = 0;
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

bool DLLN3X::send(zigbee_frame *frame)
{
    return send(frame->src_port, frame->des_port, (frame->remote_addrH << 8) | (frame->remote_addrL & 0xFF),
     frame->data, frame->length);
}

void DLLN3X::rled_blink(uint8_t orig_port, uint16_t addr, uint8_t time)
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

uint16_t DLLN3X::read_addr()
{
    if (self_addr!=0)
        return self_addr;
    uint8_t orig_port = 0xFF, dest_port = 0xFF;
    uint8_t arg = 0x01;
    uint16_t addr = 0xFFFF;
    uint8_t data[60] = "";
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

void DLLN3X::loop()
{
    uint8_t orig_port = 0xFF, dest_port = 0xFF;
    uint16_t addr = 0xFFFF;
    uint8_t data[200] = "";
    int length = 0;
    if (_DSerial->available()>0&&!_recv_lock)
    {
        recv(&orig_port,&dest_port,&addr,data,&length);
        printf("Message: ");
        for (int i = 0; i < length;i++)
        {
            printf("%X ", data[i]);
        }
        printf("at port %X from %X:%X\n", dest_port, addr, orig_port);
        if (_callback!=nullptr)
            _callback(orig_port, dest_port, addr, data, length);
    }
}

void DLLN3X::setcallback(void (*callback)(uint8_t orig_port,
                    uint8_t dest_port, uint16_t addr,
                    uint8_t data[], int length))
{
    _callback = callback;
}