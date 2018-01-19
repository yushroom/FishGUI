TODO

底层部分

- [x] 鼠标事件在不同widget之间的分发

- [ ] 把MainWindow变成单例？

- [x] 提取TreeView，最好是模板

- [ ] theme里面去掉NVGcolor

- [ ] widget::OnResized

- [x] KeyEvent 以及触发区域

- [ ] 从FishGUI.cpp中移出来

- [ ] SelectionModel中的鼠标事件

- [ ] 使用系统字体

- [ ] 什么时候需要nvgbeginpath，nvgrestore，nvgsave，nvgfill

- [ ] nvgtext的位置

- [ ] 清理一下变量名，ctx，vg，theme

- [ ] treeView singleSelection的回调做好了，multi的没做

- [ ] 降低cpu usage[instance?batch?]看一下imgui的做法

- [x] 触摸板的鼠标事件少一个[press和release一起触发]

- [ ] 配色

- [ ] 用ID标示每个widget，鼠标事件的时候要用

- [x] theme取消static

- [x] rect完全在外面的话不用绘制

- [ ] 清理CPU/GPU，delete

- [x] 每个窗口独立input

- [ ] drag event要重新设计

- [x] 渲染状态context

- [x] 窗口resize的时候scroll bar的问题

- [ ] scroll bar鼠标drag事件

- [ ] button的鼠标事件[鼠标点击事件锁定对象]

- [ ] float box

- [ ] edit box鼠标事件和编辑

- [x] checkbox鼠标事件

- [ ] slider鼠标drag

- [ ] combox选择

- [ ] hierarchy层次结构的收起/选中高亮/**重命名**

- [ ] tab移动

- [x] segmented button的绘制和鼠标事件

- [ ] status bar文本的显示

- [ ] application menu, popup menu, dropdown list[使用系统自己的？还是自己画？]

      如果自己画会有overlay的遮挡问题，这样鼠标事件很难处理

demo部分

- [ ] 主场景绘制
- [x] project view，文件夹
- [ ] console view



https://icomoon.io/app/#/select




1. multiple window

   按[这里](https://github.com/memononen/nanovg/issues/379)的思路确实可以在多个窗口间正确渲染，但是会出现memory usage直线上升的问题（到128m左右），profile之后发现是NanoVG中glBufferData造成的，多次调试发现使用了不同window使用了同一个vao/vbo/…，vbo还是每帧动态生成的。~~解决方法：每个window产生独立的vbo等，渲染的时候绑定到NanoVG中去，需要略微改一下NanoVG的代码，两个窗口的memory usage之后18m左右。~~

   ~~仍然留下一个问题：两个窗口都渲染的时候，第二个窗口正常；只渲染第二个窗口时，第二个窗口排版混乱。~~

   最终的方案：目前看来是[buffer object streaming](https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming)，所以加了动态调整buffer大小（只变大，不变小）

   2018/1/19更新：由于windows下opengl shared context不共享vao, 所以这个做法失效。新的方法是所有的buffer，texture，vao，fbo之类的全部在mainwindow的context下产生，其他window全部off  screen渲染到各自的framebuffer（注意这些framebuffer和attachment要在mainWindow的context下创建）。之后再把framebuffer的colorbuffer贴到窗口上，注意这一步中需要一个quad，这个quad的vao和vbo要在这个window的context下独立创建，讲道理vbo可以共享，但是为简单期间就这么做了。

   ​

2. 窗口不可见的时候，cpu突然升高

   https://stackoverflow.com/questions/23550423/high-cpu-usage-when-glfw-opengl-window-isnt-visible

   全部不可见的时候main render loop没有阻塞，全速跑的，所以cpu很高。~~需要在没有窗口focused的时候插一个sleep。~~ 全局加一个sleep控制帧率。

3. resize tab 

4. 处理鼠标事件时注意：触摸板的轻点的鼠标事件中press和release是一起触发的，button的click事件在release的时候触发

5. OpenGL shared context的坑：

   windows下不共享vao，fbo，跨context bind的时候会报INVALID_OPERATION