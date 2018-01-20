# FishGUI

FIshGUI is a immediate mode GUI([IMGUI](http://sol.gfxile.net/imgui/)) library, based on [NanoVG](https://github.com/memononen/nanovg).

![ScreenShot](./doc/ScreenShot.png)

with [FishEngine](https://github.com/yushroom/FishEngine)

![ScreenShot_FishEditor](./doc/ScreenShot_FishEditor.jpg)



## How to Build

### Linux / macOS

Tested on Ubuntu 17.10 and macOS 10.13.2

```shell
# ubuntu
# if you do not have opengl dev environment
sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev 

# glfw requires xorg
sudo apt-get xorg-dev

# boost
sudo apt-get install libboost-all-dev
```

```shell
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





