# FishGUI

FIshGUI is a immediate mode GUI([IMGUI](http://sol.gfxile.net/imgui/)) library, based on [NanoVG](https://github.com/memononen/nanovg).

![ScreenShot](./doc/ScreenShot.png)

with [FishEngine](https://github.com/yushroom/FishEngine)

![ScreenShot_FishEditor](./doc/ScreenShot_FishEditor.jpg)



## How to Build

### Linux / macOS

Tested on Ubuntu16.04 and macOS 10.13.2

- If you don't have **Boost**(1.58 or above is required) installed:

```shell
# Ubuntu
sudo apt-get install libboost-all-dev

# macOS
brew install boost
```

- Build:

```shell
git clone https://github.com/yushroom/FishGUI.git
cd FishGUI
mkdir build && cd build
cmake ..
make
```

build result can be found under **FishGUI/bin**

### Windows

Tested on Windows10 with VS2017

Since **filesystem** is shipped with Visual Studio(2015/2017), boost is not required.





