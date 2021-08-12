#pragma once
#include <cstdint>
#include "hid/hidapi.h"

namespace joy {
	static const int JOYCON_L_PRODUCT_ID = 8198;
	static const int JOYCON_R_PRODUCT_ID = 8199;
	enum class JoyType {
		Invalid,
		JoyL,
		JoyR,
	};
	class JoyCon {
	  public:
		JoyCon(JoyType jtype);
		~JoyCon(){};
		void SetLED(bool first, bool second, bool third, bool fourth);
		void SetLED(uint8_t led);
		JoyType GetJoyType();

	  private:
		static int devNumL;
		static int devNumR;
		hid_device *dev;
		JoyType jtype;
		uint8_t packetNum;
		void SendSubCommand(uint8_t command, uint8_t data[], int len);
	};
}  // namespace joy