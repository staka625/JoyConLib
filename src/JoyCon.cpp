#include "JoyCon.hpp"
#include <thread>
#include <Siv3D.hpp>

namespace joy {
	using namespace std::chrono_literals;
	using str           = std::string;
	using ll            = long long;
	int JoyCon::devNumL = 0;
	int JoyCon::devNumR = 0;

	JoyCon::JoyCon(JoyType jtype)
	    : packetNum(0), jtype(JoyType::Invalid), dev(nullptr) {
		memset(buff, 0x40, sizeof(buff));
		int cnt = 0;
		hid_device_info *device;
		for(device = hid_enumerate(0, 0); device != nullptr; device = device->next) {
			if(jtype == JoyType::JoyL && device->product_id == JOYCON_L_PRODUCT_ID) {
				this->jtype = JoyType::JoyL;
				cnt++;
			} else if(jtype == JoyType::JoyR && device->product_id == JOYCON_R_PRODUCT_ID) {
				this->jtype = JoyType::JoyR;
				cnt++;
			} else {
				continue;
			}
			if(jtype == JoyType::JoyL) {
				if(cnt == devNumL + 1) {
					dev = hid_open(device->vendor_id, device->product_id, device->serial_number);
					devNumL++;
					break;
				}
			}
			if(jtype == JoyType::JoyR) {
				if(cnt == devNumR + 1) {
					dev = hid_open(device->vendor_id, device->product_id, device->serial_number);
					devNumR++;
					break;
				}
			}
		}
		uint8_t data[0x01];
		data[0] = 0x01;
		SendSubCommand(0x40, data, 2);  //enable 6-axis sensor
		std::this_thread::sleep_for(20ms);
		data[0] = 0x30;
		SendSubCommand(0x03, data, 2);  //get data at 60Hz
		data[0] = 0x00;
		SendSubCommand(0x03, data, 2);
		hid_set_nonblocking(this->dev, 1);
		hid_free_enumeration(device);
	}
	JoyCon::~JoyCon() {
		if(this->jtype == JoyType::JoyL) {
			devNumL--;
		}
		if(this->jtype == JoyType::JoyR) {
			devNumR--;
		}
		hid_close(this->dev);
	}
	bool JoyCon::Update() {
		this->GetRawSensorData();
		this->UpdateStickData();
		this->UpdateAxisData();
		return true;
	}
	void JoyCon::SetPlayerLight(uint8_t led) {
		uint8_t data[0x01];
		data[0] = led;
		this->SendSubCommand(0x30, data, 1);
	}
	void JoyCon::SetHomeLight(uint8_t set[25]) {
		if(jtype != JoyType::JoyR) {
			return;
		}
		this->SendSubCommand(0x38, set, 25);
	}
	void JoyCon::SetHomeLight(bool on) {
		uint8_t data[25];
		data[1] = on ? 0xFF : 0x00;
		SetHomeLight(data);
	}
	JoyType JoyCon::GetJoyType() {
		return this->jtype;
	}
	int JoyCon::GetStickVertical() {
		return stick_vertical;
	}
	int JoyCon::GetStickHorizontal() {
		return stick_horizontal;
	}
	bool JoyCon::isPressingButton(JoyButton btn) {
		if(this->jtype == JoyType::JoyL) {
			switch(btn) {
				using enum joy::JoyButton;
				case Up:
				case Down:
				case Right:
				case Left:
				case SL:
				case SR:
				case R:
				case ZR:
					return this->buff[5] & ButtonBit.at(btn);
				case Minus:
				case LStick:
				case Capture:
					return this->buff[4] & ButtonBit.at(btn);
				default:
					return false;
			}
		}
		if(this->jtype == JoyType::JoyR) {
			switch(btn) {
				using enum joy::JoyButton;
				case Y:
				case X:
				case B:
				case A:
				case SR:
				case SL:
				case R:
				case ZR:
					return this->buff[3] & ButtonBit.at(btn);
				case Plus:
				case RStick:
				case Home:
					return this->buff[4] & ButtonBit.at(btn);
				default:
					return false;
			}
		}
		return false;
	}

	double JoyCon::GetAccX() {
		return acc[0];
	}
	double JoyCon::GetAccY() {
		return acc[1];
	}
	double JoyCon::GetAccZ() {
		return acc[2];
	}
	double JoyCon::GetGyroX() {
		return gyro[0];
	}
	double JoyCon::GetGyroY() {
		return gyro[1];
	}
	double JoyCon::GetGyroZ() {
		return gyro[2];
	}

	//private
	void JoyCon::UpdateStickData() {
		uint8_t *data          = this->buff + (this->jtype == JoyType::JoyL ? 6 : 9);
		this->stick_horizontal = static_cast<int>(data[0]) | ((static_cast<int>(data[1]) & 0xF) << 8);
		this->stick_vertical   = (static_cast<int>(data[1]) >> 4) | (static_cast<int>(data[2]) << 4);
	}
	void JoyCon::UpdateAxisData() {
		int acc_raw[3];
		int gyro_raw[3];
		for(int i = 0; i < 3; i++) {
			acc_raw[i]   = this->buff[2 * i + 14] << 8 | this->buff[2 * i + 13];
			this->acc[i] = acc_raw[i] * 0.000244f;
		}
		for(int i = 0; i < 3; i++) {
			gyro_raw[i]   = this->buff[2 * i + 20] << 8 | this->buff[2 * i + 19];
			this->gyro[i] = gyro_raw[i] * 0.06103f;
		}
	}
	int JoyCon::GetRawSensorData() {
		uint8_t size = 49;
		return hid_read(this->dev, this->buff, size);
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