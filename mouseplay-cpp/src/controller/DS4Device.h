#pragma once

#include <hidapi/hidapi.h>

class DS4Device {
public:
    DS4Device();
    ~DS4Device();

    bool open();
    void close();
    bool sendStickState(unsigned char lx, unsigned char ly, unsigned char rx, unsigned char ry);
    bool isOpen() const;

private:
    hid_device* m_device;
};
