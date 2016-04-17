# cef-gl-example
Chromium Embedded Framework + OpenGL

![screenshot](https://raw.githubusercontent.com/if1live/cef-gl-example/master/document/screenshot.png)

## Build
### Required
* Visual Studio 2015
* Chromium Embedded Framework 3 Build
	* https://cefbuilds.com/
	* Windows 64bit
	
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

## Note
### Chromium Embedded Framework 3 Build version
* Branch 2623, Windows 64bit, 2016-04-07, CEF 3.2623.1397.gaf139d7 (123MB)
	* **DO NOT USE**
	* http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=13997
* Branch 2526, Windows 64bit, 2016-03-16, CEF 3.2526.1373.gb660893 (113MB)
	* Working. **Recommended**
	
### Cross-platform
I use only Windows 64bit. 
I don't care other platform.

## Similar Projects & Reference
* [cefgui](https://github.com/andmcgregor/cefgui)
* [cef_osr](https://github.com/qwertzui11/cef_osr)
