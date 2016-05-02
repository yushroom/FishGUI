#include <glfw3.h>

namespace RFGUI {
    struct Position {
        float x, y;
    };
    
    struct RenderContext {
        GLFWwindow* window;
        int width;
        int height;
        Position mouse_position;
        bool mouse_left_button_pressed = false; // down
        bool mouse_left_button_released = false; // up
        
        void reset() {
        }
    };
    static RenderContext g_renderContext;
    
    enum SideBarPosition {
        SBP_LEFT,
        SBP_RIGHT,
        SBP_TOP,
        SBP_BOTTOM,
        SBP_FLOATING,
    };
    
    struct SideBarState {
        int x;
        int y;
        int width;
        const int x_margin_left = 10;
        int x_margin_right = 10; // or 18
        const int y_margin = 5;
        const int y_cell_height = 15;
        int y_start = 0;
        int y_filled = 0;
        int cell_count = 0;
        int hover_cell_id = -1;
        int clicked_cell_id = -1;
        
        void reset() {
            y_filled = 0;
            cell_count = 0;
        }
        
        void get_origin(int *px, int *py) {
            *px = x + x_margin_left;
            *py = y_filled + y_margin;
        }
        
        int get_avaliable_width() {
            return width - x_margin_left - x_margin_right;
        }
        
        int get_current_cell_id() {
            return cell_count;
        }
        
        // cell height without margin
        void add_cell(const int height) {
            y_filled += height + y_margin;
            cell_count ++;
        }
    };
    static SideBarState g_side_bar_state;
    
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    void window_size_callback(GLFWwindow* window, int width, int height);
    
    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    
    void init(GLFWwindow * window, const int width, const int height);
    
    void Begin();
    
    void End();
    
    void SideBar(int width = 160);
    
    bool Button(const char* text);
}