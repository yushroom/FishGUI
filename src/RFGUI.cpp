#include "RFGUI.hpp"
#include "RFFont.hpp"

namespace RFGUI {
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        g_renderContext.width = width;
        g_renderContext.height = height;
    }
    
    void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {
        g_renderContext.mouse_position.x = float(xpos);
        g_renderContext.mouse_position.y = float(ypos);
        //printf("mouse position: (%lf, %lf)\n", xpos, ypos);
    }
    
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        //printf("%d, %d\n", button, action);
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            g_renderContext.mouse_left_button_pressed = (action == GLFW_PRESS);
            g_renderContext.mouse_left_button_released = (action == GLFW_RELEASE);
        }
    }
    
    inline bool mouse_in_region(int x, int y, int w, int h) {
        auto mx = g_renderContext.mouse_position.x;
        auto my = g_renderContext.mouse_position.y;
        return (mx > x && mx < x+w && my > y && my < y+h);
    }
    
    
    void init(GLFWwindow * window, const int width, const int height) {
        g_renderContext.width = width;
        g_renderContext.height = height;
        g_renderContext.window = window;
    }
    
    void Begin() {
        RFGUI::g_renderContext.reset();
        RFGUI::g_side_bar_state.reset();
    }
    
    void End() {
        
    }
    
    void fill_rect(int x, int y, int w, int h) {
        float xx = float(x) / g_renderContext.width;
        float yy = 1.f-float(y) / g_renderContext.height;
        float ww = float(w) / g_renderContext.width;
        float hh = -float(h) / g_renderContext.height;
        glBegin(GL_QUADS);
        glVertex2f(xx, yy);
        glVertex2f(xx, yy+hh);
        glVertex2f(xx+ww, yy+hh);
        glVertex2f(xx+ww, yy);
        glEnd();
    }
    
    void SideBar(int width/* = 160*/) {
        glColor3f(0.8f, 0.8f, 0.8f);
        const int x = g_renderContext.width - width;
        fill_rect(x, 0, width, g_renderContext.height);
        g_side_bar_state.x = x;
        g_side_bar_state.y_filled = 0;
        g_side_bar_state.width = width;
    }
    
    bool Button(const char* text) {
        int x, y, w, h;
        g_side_bar_state.get_origin(&x, &y);
        w = g_side_bar_state.get_avaliable_width();
        h = g_side_bar_state.y_cell_height;
        
        bool clicked = false;
        int id = g_side_bar_state.get_current_cell_id();
        bool mouse_inside = mouse_in_region(x, y, w, h);
        if (mouse_inside) {
            g_side_bar_state.hover_cell_id = id;
            if (g_renderContext.mouse_left_button_pressed) {
                glColor3f(0.5f, 0.5f, 0.5f);
                g_side_bar_state.clicked_cell_id = id;
            } else {
                glColor3f(0.9f, 0.9f, 0.9f);
                if (g_side_bar_state.clicked_cell_id == id) {
                    clicked = true;
                    g_side_bar_state.clicked_cell_id = -1;
                }
            }
        }
        else
            glColor3f(1.f, 1.f, 1.f);
        
        fill_rect(x, y, w, h);
        
        g_side_bar_state.add_cell(h);
        return clicked;
    }
    
    void Label(const std::string& text, GUIAlignment alignment) {
        int x, y, w, h;
        g_side_bar_state.get_origin(&x, &y);
        w = g_side_bar_state.get_avaliable_width();
        h = g_side_bar_state.y_cell_height;
        //const int id = g_side_bar_state.get_current_cell_id();
        DrawText(text, x, y, w, h, alignment);
        g_side_bar_state.add_cell(h);
    }
    
    void DrawText(const std::string& text, int x, int y, int w, int h, GUIAlignment alignment)
    {
        auto bmp = RFFont::GetInstance()->GetBitmapForString(text, w, h);
        //bmp->save("fonts.bmp");
    }

}