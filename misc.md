1. multiple window

   按[这里](https://github.com/memononen/nanovg/issues/379)的思路确实可以在多个窗口间正确渲染，但是会出现memory usage直线上升的问题（到128m左右），profile之后发现是NanoVG中glBufferData造成的，多次调试发现使用了不同window使用了同一个vao/vbo/…，vbo还是每帧动态生成的。解决方法：每个window产生独立的vbo等，渲染的时候绑定到NanoVG中去，需要略微改一下NanoVG的代码，两个窗口的memory usage之后18m左右。

   仍然留下一个问题：两个窗口都渲染的时候，第二个窗口正常；只渲染第二个窗口时，第二个窗口排版混乱。

2. 窗口不可见的时候，cpu突然升高

   https://stackoverflow.com/questions/23550423/high-cpu-usage-when-glfw-opengl-window-isnt-visible

   全部不可见的时候main render loop没有阻塞，全速跑的，所以cpu很高。需要在没有窗口focused的时候插一个sleep

2. resize tab