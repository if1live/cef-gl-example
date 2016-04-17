# cef-gl-example
Chromium Embedded Framework + OpenGL

## Build
### Required
* Visual Studio 2015
* Chromium Embedded Framework 3 Build
	* https://cefbuilds.com/
	* Branch 2623
	* Windows 64bit
	* 2016-04-07
	* CEF 3.2623.1397.gaf139d7 (123MB)
	
### Progress
```bash
# Unarchive those directories from CEF.
# * `Debug`
# * `Release`
# * `libcef_dll`
# * `Resources`
# * `include`

mkdir build && cd build
cmake -G "Visual Studio 14 2015 Win64" ..
```
