#include "DS4Device.h"

#include <array>
#include <vector>

namespace {
constexpr unsigned short kSonyVendorId = 0x054C;
}

DS4Device::DS4Device() : m_device(nullptr) {
    hid_init();
}

DS4Device::~DS4Device() {
    close();
    hid_exit();
}

bool DS4Device::open() {
    close();

    hid_device_info* devices = hid_enumerate(kSonyVendorId, 0x0);
    hid_device_info* current = devices;

    while (current != nullptr) {
        if (current->vendor_id == kSonyVendorId) {
            m_device = hid_open_path(current->path);
            if (m_device != nullptr) {
                break;
            }
        }
        current = current->next;
    }

    hid_free_enumeration(devices);

    if (m_device != nullptr) {
        hid_set_nonblocking(m_device, 1);
        return true;
    }

    return false;
}

void DS4Device::close() {
    if (m_device != nullptr) {
        hid_close(m_device);
        m_device = nullptr;
    }
}

bool DS4Device::sendStickState(unsigned char lx, unsigned char ly, unsigned char rx, unsigned char ry) {
    if (m_device == nullptr) {
        return false;
    }

    std::array<unsigned char, 64> report{};
    report[0] = 0x01;
    report[1] = lx;
    report[2] = ly;
    report[3] = rx;
    report[4] = ry;

    return hid_write(m_device, report.data(), report.size()) >= 0;
}

bool DS4Device::isOpen() const {
    return m_device != nullptr;
}
