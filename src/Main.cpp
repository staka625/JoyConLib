#include <Siv3D.hpp>  // OpenSiv3D v0.4.3
#include "hid/hidapi.h"
#include "JoyCon.hpp"

void Main() {
	bool work = true;
	joy::JoyCon joyL(joy::JoyType::JoyL);
	joy::JoyCon joyR(joy::JoyType::JoyR);
	joyL.SetPlayerLight(0b10000000);
	joyR.SetPlayerLight(0b00000001);
	auto thL = std::thread([&joyL](bool& work) { while(work){joyL.Update();} }, std::ref(work));
	auto thR = std::thread([&joyR](bool& work) { while(work){joyR.Update();} }, std::ref(work));
	Font font(24);
	while(work) {
		work = System::Update();
		font(U"Stick L:({},{})"_fmt(joyL.GetStickHorizontal(), joyL.GetStickVertical())).draw(0, 0);
		font(U"Stick R:({},{})"_fmt(joyR.GetStickHorizontal(), joyR.GetStickVertical())).draw(0, 50);
		font(U"Button A:{}"_fmt(joyR.isPressingButton(joy::JoyButton::A) ? U"Pressed" : U"Released")).draw(0, 100);
		font(U"LAcc(x,y,z):({:.2f},{:.2f},{:.2f})"_fmt(
		         joyL.GetAccX(),
		         joyL.GetAccY(),
		         joyL.GetAccZ()))
		    .draw(0, 150);
		font(U"LGyro(x,y,z):({:.2f},{:.2f},{:.2f})"_fmt(
		         joyL.GetGyroX(),
		         joyL.GetGyroY(),
		         joyL.GetGyroZ()))
		    .draw(0, 200);
	}
	thL.join();
	thR.join();
}