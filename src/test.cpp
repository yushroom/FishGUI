#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "RFGUI.hpp"
#include "RFFont.hpp"
#include "Bitmap.hpp"

//#include <stdio.h>
//#include <string.h>
//#include <math.h>
//#include <vector>
//using namespace std;
//#include "RFFont.hpp"
//
//#define WIDTH   512
//#define HEIGHT  512
//
//
///* origin is the upper left corner */
//unsigned char image[HEIGHT][WIDTH];
//unsigned char image2[HEIGHT][WIDTH];
//
//
///* Replace this function with something useful. */
//
//void
//draw_bitmap(FT_Bitmap*  bitmap,
//            FT_Int      x,
//            FT_Int      y)
//{
//    FT_Int  i, j, p, q;
//    FT_Int  x_max = x + bitmap->width;
//    FT_Int  y_max = y + bitmap->rows;
//    
//    for ( i = x, p = 0; i < x_max; i++, p++ )
//    {
//        for ( j = y, q = 0; j < y_max; j++, q++ )
//        {
//            if ( i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT )
//                continue;
//            
//            image[j][i] |= bitmap->buffer[q * bitmap->width + p];
//        }
//    }
//}
//
//bool is_contour(int x, int y) {
//    return (image[y][x] != 0 &&
//            (x > 0 && image[y][x-1] != 0) &&
//            (x < WIDTH && image[y][x+1] != 0) &&
//            (y > 0 && image[y-1][x] != 0) &&
//            (y < HEIGHT && image[y+1][x] != 0) &&
//            (x > 0 && y > 0 && image[x-1][y-1] != 0) &&
//            (x > 0 && y < HEIGHT && image[x-1][y+1] != 0) &&
//            (x < WIDTH && y > 0 && image[x+1][y-1] != 0) &&
//            (x < WIDTH && y < HEIGHT && image[x+1][y+1] != 0));
//}
//
//void get_contour() {
//    for (int y = 1; y < HEIGHT-1; ++y)
//        for (int x = 1; x < WIDTH-1; ++x) {
//            if (image[y][x] == 0)
//                image2[y][x] = 0;
//            else
//                image2[y][x] = 1 -  image[y-1][x] * image[y-1][x-1] * image[y-1][x+1] * image[y][x-1] * image[y][x+1] * image[y+1][x-1] * image[y+1][x] * image[y+1][x+1];
//        }
//}
//
//void show_image() {
//    vector<uint8_t> pixels;
//    pixels.reserve(WIDTH * HEIGHT);
//    for (int j = 0; j < HEIGHT; ++j)
//        for (int i = 0; i < WIDTH; ++i) {
//            for (int k = 0; k < 3; ++k)
//                pixels.push_back(image[HEIGHT-j][i] == 0 ? 0 : 255);
//        }
//    //write_bmp("/Users/yushroom/program/graphics/RFGUI/fonts.bmp", WIDTH, HEIGHT, pixels);
//	write_bmp("./fonts.bmp", WIDTH, HEIGHT, pixels);
//}
//
//int main( int argc, char**  argv )
//{
//	RFFont::get_instance();
//
//
//    FT_Library    library;
//    FT_Face       face;
//    
//    FT_GlyphSlot  slot;
//    FT_Matrix     matrix;                 /* transformation matrix */
//    FT_Vector     pen;                    /* untransformed origin  */
//    FT_Error      error;
//    
//    //char*         filename = "/System/Library/Fonts/Menlo.ttc";
//	char* filename = R"(C:\Windows\Fonts\consola.ttf)";
//    char* text = "Ff";
//    
//    double        angle;
//    int           target_height;
//    int           n, num_chars;
//    
//    
////    if ( argc != 3 )
////    {
////        fprintf ( stderr, "usage: %s font sample-text\n", argv[0] );
////        exit( 1 );
////    }
//    
//    if ( argc == 3 )
//    {
//        filename      = argv[1];                           /* first argument     */
//        text          = argv[2];                           /* second argument    */
//    }
//    num_chars     = strlen( text );
//    //angle         = ( 25.0 / 360 ) * 3.14159 * 2;      /* use 25 degrees     */
//    angle = 0;
//    target_height = HEIGHT;
//    
//    error = FT_Init_FreeType( &library );              /* initialize library */
//    /* error handling omitted */
//    
//    error = FT_New_Face( library, filename, 0, &face );/* create face object */
//    /* error handling omitted */
//    
//    /* use 50pt at 100dpi */
//    error = FT_Set_Char_Size( face, 512<<6, 0,
//                             96, 0 );                /* set character size */
//    error = FT_Set_Pixel_Sizes(
//                               face,   /* handle to face object */
//                               512,      /* pixel_width           */
//                               512 );   /* pixel_height          */
//    /* error handling omitted */
//    
//    slot = face->glyph;
//    
//    /* set up matrix */
//    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
//    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
//    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
//    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );
//    
//    /* the pen position in 26.6 cartesian space coordinates; */
//    /* start at (300,200) relative to the upper left corner  */
////    pen.x = 300 * 64;
////    pen.y = ( target_height - 200 ) * 64;
//    pen.x = 0;
//    pen.y = 0;
//    
//    for ( n = 0; n < num_chars; n++ )
//    {
//        /* set transformation */
//        FT_Set_Transform( face, &matrix, &pen );
//        
//        /* load glyph image into the slot (erase previous one) */
//        error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
//        if ( error )
//            continue;                 /* ignore errors */
//        
//        /* now, draw to our target surface (convert position) */
//        draw_bitmap( &slot->bitmap,
//                    slot->bitmap_left,
//                    target_height - slot->bitmap_top );
//        
//        /* increment pen position */
//        pen.x += slot->advance.x;
//        pen.y += slot->advance.y;
//    }
//    
//    //get_contour();
//    show_image();
//    
//    FT_Done_Face    ( face );
//    FT_Done_FreeType( library );
//    
//    return 0;
//}


static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

int main(void)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	RFGUI::init(window, 640, 480);
	glfwSetWindowSizeCallback(window, RFGUI::window_size_callback);
	glfwSetKeyCallback(window, RFGUI::key_callback);
	glfwSetCursorPosCallback(window, RFGUI::cursor_position_callback);
	glfwSetMouseButtonCallback(window, RFGUI::mouse_button_callback);

	float ratio;
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;
	glViewport(0, 0, width, height);

	auto bmp = RFFont::get_instance()->get_bitmap_for_char('B', 512);
	bmp->save("./fonts.bmp");

	/*while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.f, 1.f, 0.f, 1.f, 1.f, -1.f);

		RFGUI::Begin();
		RFGUI::SideBar();

		if (RFGUI::Button("button 1")) {
			printf("button 1 clicked\n");
		}

		if (RFGUI::Button("button 2")) {
			printf("button 2 clicked\n");
		}

		RFGUI::End();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}*/

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

