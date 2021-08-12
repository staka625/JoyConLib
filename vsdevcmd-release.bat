call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat"
msbuild ".\\"%1".vcxproj" /p:configuration=release
cd App
".\\"%1".exe"