call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
msbuild ".\\"%1".vcxproj"
cd App
".\\"%1"(debug).exe"