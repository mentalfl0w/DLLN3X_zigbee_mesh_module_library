#ifndef _ZIGBEEFRAME_H_
#define _ZIGBEEFRAME_H_
#include <Arduino.h>
#include <cstring>
#include <iostream>
#include <vector>

namespace zigbee_protocol {
class ZigbeeFrame {
private:
    const uint8_t head = 0xFE, tail = 0xFF;
    uint8_t _length = 0, _src_port = 0, _des_port = 0;
    uint8_t _pack_len = 0;
    uint16_t _remote_addr = 0;
    std::vector<uint8_t> _data, _packed_data, _package;

public:
    ZigbeeFrame(char* data) { setData((uint8_t*)data, strlen(data)); };
    ZigbeeFrame(char* data, uint8_t length) { setData((uint8_t*)data, length); };
    ZigbeeFrame(uint8_t* data, uint8_t length) { setData(data, length); };
    ZigbeeFrame(uint8_t src_port, uint8_t des_port, uint16_t remote_addr, uint8_t* data, uint8_t data_length)
    {
        _length = data_length + 4;
        _src_port = src_port;
        _des_port = des_port;
        _remote_addr = remote_addr;
        setData(data, data_length);
    };
    ZigbeeFrame(uint8_t src_port, uint8_t des_port, uint16_t remote_addr, char* data, uint8_t data_length)
    {
        _length = data_length + 4;
        _src_port = src_port;
        _des_port = des_port;
        _remote_addr = remote_addr;
        setData((uint8_t*)data, data_length);
    };
    ZigbeeFrame(uint8_t src_port, uint8_t des_port, uint16_t remote_addr)
    {
        _src_port = src_port;
        _des_port = des_port;
        _remote_addr = remote_addr;
    };
    ZigbeeFrame(const ZigbeeFrame& zf)
    {
        _length = zf._length;
        _src_port = zf._src_port;
        _des_port = zf._des_port;
        _pack_len = zf._pack_len;
        _remote_addr = zf._remote_addr;
        _data = zf._data;
        _packed_data = zf._packed_data;
        _package = zf._package;
    };
    ZigbeeFrame(const ZigbeeFrame* zf)
    {
        _length = zf->_length;
        _src_port = zf->_src_port;
        _des_port = zf->_des_port;
        _pack_len = zf->_pack_len;
        _remote_addr = zf->_remote_addr;
        _data = zf->_data;
        _packed_data = zf->_packed_data;
        _package = zf->_package;
    };
    ZigbeeFrame() {};
    ~ZigbeeFrame() {};
    void setDesPort(uint8_t des_port) { _des_port = des_port; };
    void setSrcPort(uint8_t src_port) { _src_port = src_port; };
    void setRemoteAddr(uint16_t remote_addr) { _remote_addr = remote_addr; };
    uint8_t getDesPort() { return _des_port; };
    uint8_t getSrcPort() { return _src_port; };
    uint16_t getRemoteAddr() { return _remote_addr; };
    uint8_t getLength(){ return _data.size() + 4;};
    uint8_t getDataLength(){ return _data.size();};
    std::vector<uint8_t>& getData(){ return _data; };
    void setData(uint8_t* data, uint8_t length)
    {
        _data.clear();
        addData(data, length);
    };
    void setData(char* data) { setData((uint8_t*)data, strlen(data)); };
    void setData(char* data, uint8_t length) { setData((uint8_t*)data, length); };
    void setData(std::vector<uint8_t> data) { _data = data; };
    void addData(char* data) { addData(data, strlen(data)); };
    void addData(uint8_t* data, uint8_t length)
    {
        for (uint8_t i = 0; i < length; i++) {
            _data.push_back(data[i]);
        }
    };
    void addData(char* data, uint8_t length) { addData((uint8_t*)data, length); };
    void addData(std::vector<uint8_t> data) { _data.insert(_data.end(), data.begin(), data.end()); };
    void clear()
    {
        _package.clear();
        _packed_data.clear();
        _data.clear();
    };
    uint8_t* data()
    {
        get_package();
        return _package.data();
    };
    uint8_t data_size() { return _data.size(); };
    uint8_t size()
    {
        get_package();
        return _package.size();
    };
    void append(uint8_t byte)
    {
        _data.push_back(byte);
        _length = _data.size() + 4;
    };
    void pack() { pack(_data, _packed_data); };
    void pack(std::vector<uint8_t> data, std::vector<uint8_t>& pack_data)
    {
        pack_data.clear();
        for (int i = 0; i < data.size(); i++) {
            if (data[i] == 0xFE) {
                pack_data.push_back(data[i]);
                pack_data.push_back(0xFC);
            } else if (data[i] == 0xFF) {
                pack_data.push_back(0xFE);
                pack_data.push_back(0xFD);
            } else {
                pack_data.push_back(data[i]);
            }
        }
    };
    void depack() { depack(_data, _packed_data); };
    void depack(std::vector<uint8_t>& data, std::vector<uint8_t> pack_data)
    {
        data.clear();
        for (int i = 0; i < pack_data.size(); i++) {
            if (pack_data[i] != 0xFE) {
                data.push_back(pack_data[i]);
            } else if (pack_data[i + 1] == 0xFD) {
                data.push_back(0xFF);
                i++;
            } else if (pack_data[i + 1] == 0xFC) {
                data.push_back(0xFE);
                i++;
            }
        }
    };
    void make_package(uint8_t src_port, uint8_t des_port, uint16_t remote_addr, std::vector<uint8_t>& package,
        std::vector<uint8_t> data, std::vector<uint8_t>& pack_data)
    {
        pack(data, pack_data);
        package.clear();
        package.push_back(head);
        package.push_back(data.size() + 4);
        package.push_back(src_port);
        package.push_back(des_port);
        package.push_back(remote_addr & 0xFF);
        package.push_back((remote_addr >> 8) & 0xFF);
        package.insert(package.end(), pack_data.begin(), pack_data.end());
        package.push_back(tail);
    };
    std::vector<uint8_t> get_package()
    {
        make_package(_src_port, _des_port, _remote_addr, _package, _data, _packed_data);
        return _package;
    };
    void load_package(std::vector <uint8_t> &buf)
    {
        load_package(buf.data(),buf.size());
    };
    void load_package(uint8_t *buf, uint8_t length)
    {
        _packed_data.clear();
        for (uint8_t i=0;i<length;i++)
        {
            _package.push_back(buf[i]);
            if (i>5 && i<length-1)
                _packed_data.push_back(buf[i]);
        }
        _length = _package[1];
        _src_port = _package[2];
        _des_port = _package[3];
        _remote_addr = _package[4] | (_package[5]<<8);
        depack(_data, _packed_data);
    };
    void print()
    {
        get_package();
        Serial.print("src_port:");
        Serial.println(_src_port, HEX);
        Serial.print("des_port:");
        Serial.println(_des_port, HEX);
        Serial.print("remote_addr:");
        Serial.println(_remote_addr, HEX);
        Serial.print("data:");
        for (auto byte : _data) {
            char buf[4];
            sprintf(buf, "%02X ", byte);
            Serial.print(buf);
        }
        Serial.println();
        Serial.print("packed_data:");
        for (auto byte : _packed_data) {
            char buf[4];
            sprintf(buf, "%02X ", byte);
            Serial.print(buf);
        }
        Serial.println();
        Serial.print("package:");
        for (auto byte : _package) {
            char buf[4];
            sprintf(buf, "%02X ", byte);
            Serial.print(buf);
        }
        Serial.println();
    }
    ZigbeeFrame operator+(const ZigbeeFrame& zf)
    {
        ZigbeeFrame temp(this);
        temp.addData(zf._data);
        return temp;
    };
    void operator+=(const ZigbeeFrame& zf) { addData(zf._data); };
    void operator=(const ZigbeeFrame& zf)
    {
        _length = zf._length;
        _src_port = zf._src_port;
        _des_port = zf._des_port;
        _pack_len = zf._pack_len;
        _remote_addr = zf._remote_addr;
        _data = zf._data;
        _packed_data = zf._packed_data;
        _package = zf._package;
    };
    ZigbeeFrame operator+(char* data)
    {
        ZigbeeFrame temp(this);
        temp.addData(data);
        return temp;
    };
    uint8_t& operator[](uint8_t i)
    {
        get_package();
        return _package[i];
    };
    void operator+=(char* data) { addData(data, (uint8_t)strlen(data)); };
    friend std::ostream& operator<<(std::ostream& os, const ZigbeeFrame& zf)
    {
        os << "src_port:" << zf._src_port << '\n'
           << "des_port:" << zf._des_port << '\n'
           << "remote_addr:" << zf._remote_addr << '\n'
           << "packed_data:";
        for (auto byte : zf._packed_data)
            os << std::hex << std::uppercase << byte << ' ';
        os << '\n';
        return os;
    }
};
}
#endif