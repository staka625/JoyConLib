#include <Siv3D.hpp>  // OpenSiv3D v0.4.3
#include "hid/hidapi.h"
#include "JoyCon.hpp"

void Main() {
	joy::JoyCon joyL(joy::JoyType::JoyL);
	joy::JoyCon joyR(joy::JoyType::JoyR);
}