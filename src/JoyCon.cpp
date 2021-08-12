#include "JoyCon.hpp"

namespace joy {
	int JoyCon::devNumL = 0;
	int JoyCon::devNumR = 0;
	JoyCon::JoyCon(JoyType jtype)
	    : packetNum(0), jtype(JoyType::Invalid), dev(nullptr) {
		int cnt = 0;
		hid_device_info *device;
		for(device = hid_enumerate(0, 0); device != nullptr; device = device->next) {
			if(jtype == JoyType::JoyL && device->product_id == JOYCON_L_PRODUCT_ID) {
				this->jtype = JoyType::JoyL;
			} else if(jtype == JoyType::JoyR && device->product_id == JOYCON_R_PRODUCT_ID) {
				this->jtype = JoyType::JoyR;
			} else {
				continue;
			}
			cnt++;
			if(jtype == JoyType::JoyL) {
				if(cnt == this->devNumL + 1) {
					dev = hid_open(device->vendor_id, device->product_id, device->serial_number);
					devNumL++;
					break;
				}
			}
			if(jtype == JoyType::JoyR) {
				if(cnt == this->devNumR + 1) {
					dev = hid_open(device->vendor_id, device->product_id, device->serial_number);
					devNumR++;
					break;
				}
			}
		}
		hid_free_enumeration(device);
	}
	void JoyCon::SetLED(bool first, bool second, bool third, bool fourth) {
		uint8_t led =
		    (static_cast<uint8_t>(fourth) << 3) + (static_cast<uint8_t>(third) << 2) + (static_cast<uint8_t>(second) << 1) + static_cast<uint8_t>(first);
		this->SetLED(led);
	}
	void JoyCon::SetLED(uint8_t led) {
		uint8_t data[0x01];
		data[0] = led;
		this->SendSubCommand(0x30, data, 1);
	}
	JoyType JoyCon::GetJoyType() {
		return this->jtype;
	}
	void JoyCon::SendSubCommand(uint8_t command, uint8_t data[], int len) {
		uint8_t buf[0x40];
		memset(buf, 0x0, size_t(0x40));

		buf[0] = 1;                // 0x10 for rumble only
		buf[1] = this->packetNum;  // Increment by 1 for each packet sent. It loops in 0x0 - 0xF range.

		this->packetNum++;
		this->packetNum %= 0xF;

		buf[10] = command;
		memcpy(buf + 11, data, len);

		hid_write(this->dev, buf, 0x40);
	}
}  // namespace joy