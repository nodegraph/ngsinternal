#include <base/device/deviceheadersgl.h>
#include <base/device/devicedebug.h>
#include <base/device/transforms/glmhelper.h>
#include <base/device/transforms/mouseinfo.h>
#include <base/device/transforms/viewparams.h>

#include <iostream>


namespace ngs {


// Update the point in other space from the screen_pos.
// Note the screen_pos variable is untouched.
void MouseInfo::update_transforms(const ViewportParams& viewport, const glm::mat4& model_view, const glm::mat4 &projection, float background_z) {
  screen_pos_gl = get_screen_click_gl(viewport, screen_pos);
  object_space_pos = get_object_space_click(model_view,projection,viewport,screen_pos_gl,background_z);
  camera_space_pos = get_camera_space_click(object_space_pos,model_view);
}


// OpenGL Transformation Pipeline
// 1) object coordinates = point
// 2) eye coordinates = point*model_view
// 3) clip coordinates = eye*projection
// 4) normalized device coordinates --> eye[0]/=eye[3], eye[1]/=eye[3], eye[2]/=eye[3]
// 5) screen/window coordinates = ndc * view_transform --> 0<=x<=width, 0<=y<=height, 0<=z<=1

// Converts a screen space click to object space.
glm::vec4 get_object_space_click(const glm::mat4 &model_view,
                                 const glm::mat4 &projection,
                                 const ViewportParams& viewport,
                                 const glm::vec2 &screen_click_gl,
                                 float background_screen_z) {

    // Get the depth value.
    // Note that the depth value is not the real distance in world space.
    // It is a post perspective warped value from 0 to 1.
    float screen_z;
//#if GLES_MAJOR_VERSION >= 3
//    gpu(glReadPixels(screen_click_gl[0], screen_click_gl[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &screen_z));
//#elif GLES_MAJOR_VERSION >= 1
//    // We can't read the depth buffer on gles 2.0 and below.
//    screen_z = 1.0;
//#endif

    // Since this can get called outside of a current gl context, we'll stick to 1.0 for now.
    screen_z = 1.0;

    // If the background is hit, then change the depth to the default background depth.
    if (screen_z==1.0) {
        // An object was not not hit, so we assume it hits a plane in the middle of the normalized space.
        screen_z = background_screen_z;
    }

    // Convert screen point to normalized point in (-1,-1,-1) to (1,1,1).
    glm::vec4 normalized(2.0*(screen_click_gl.x-viewport.x)/viewport.width-1.0,
                         2.0*(screen_click_gl.y-viewport.y)/viewport.height-1.0,
                         2.0*screen_z-1.0,
                         1.0);

    // Calculate the unprojection matrix.
    glm::mat4 pmv = projection *model_view;
    glm::mat4 unproject= glm::inverse(pmv);

    // Unproject the window co-ordinates to find the object coordinates.
    glm::vec4 click = unproject*normalized;
    click /= click[3];
    return click;
}


// Converts a object space click to camera space.
glm::vec4 get_camera_space_click(const glm::vec4 &object_space_click, const glm::mat4 &model_view) {
    return model_view *object_space_click;
}


// Converts a screen space click to opengl screen space, where y increases from bottom to top.
glm::vec2 get_screen_click_gl(const ViewportParams &viewport, const glm::vec2 &screen_click) {
    return glm::vec2(screen_click.x,viewport.height-screen_click.y);
}

MouseInfo get_vec2_info(const glm::vec2& screen_pos) {
    MouseInfo info;
    info.screen_pos = screen_pos;
    // The touch is always considered to be a left button mouse click.
    info.left_button=false;
    info.middle_button=false;
    info.right_button=false;
    // Touch are always considered to be modifier free.
    info.control_modifier=false;
    info.alt_modifier=false;
    info.shift_modifier=false;
    return info;
}

}
