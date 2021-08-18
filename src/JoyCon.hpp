#pragma once
#include <cstdint>
#include <thread>
#include <unordered_map>
#include "hid/hidapi.h"

namespace joy {
	static const int JOYCON_L_PRODUCT_ID = 8198;
	static const int JOYCON_R_PRODUCT_ID = 8199;
	enum class JoyType {
		Invalid,
		JoyL,
		JoyR,
	};
	enum class JoyButton : int {
		Up,
		Left,
		Right,
		Down,
		X,
		Y,
		A,
		B,
		SL,
		SR,
		Home,
		Capture,
		Minus,
		Plus,
		L,
		R,
		LStick,
		RStick,
		ZL,
		ZR,
		LLeft  = Up,
		LDown  = Left,
		LUP    = Right,
		LRight = Down,
		RLeft  = B,
		RDown  = A,
		RUp    = Y,
		RRight = X,
	};
	static const std::unordered_map<JoyButton, uint8_t> ButtonBit{
	    {JoyButton::Up, 0x02},
	    {JoyButton::Left, 0x08},
	    {JoyButton::Right, 0x04},
	    {JoyButton::Down, 0x01},
	    {JoyButton::X, 0x02},
	    {JoyButton::Y, 0x01},
	    {JoyButton::A, 0x08},
	    {JoyButton::B, 0x04},
	    {JoyButton::SL, 0x20},
	    {JoyButton::SR, 0x10},
	    {JoyButton::Home, 0x10},
	    {JoyButton::Capture, 0x20},
	    {JoyButton::Minus, 0x01},
	    {JoyButton::Plus, 0x02},
	    {JoyButton::L, 0x40},
	    {JoyButton::R, 0x40},
	    {JoyButton::LStick, 0x08},
	    {JoyButton::RStick, 0x04},
	    {JoyButton::ZL, 0x80},
	    {JoyButton::ZR, 0x80},
	};
	class JoyCon {
	  public:
		JoyCon(JoyType jtype);
		~JoyCon();
		void SetPlayerLight(uint8_t led);
		void SetHomeLight(uint8_t set[25]);
		void SetHomeLight(bool on = true);
		bool Update();
		JoyType GetJoyType();
		int GetStickVertical();
		int GetStickHorizontal();
		double GetAccX();
		double GetAccY();
		double GetAccZ();
		double GetGyroX();
		double GetGyroY();
		double GetGyroZ();
		bool isPressingButton(JoyButton btn);
		uint8_t buff[0x40];

	  private:
		int stick_vertical, stick_horizontal;
		double acc[3];
		double gyro[3];
		int GetRawSensorData();
		void UpdateStickData();
		void UpdateAxisData();
		static int devNumL;
		static int devNumR;
		hid_device *dev;
		JoyType jtype;
		uint8_t packetNum;
		void SendSubCommand(uint8_t command, uint8_t data[], int len);
	};
}  // namespace joy