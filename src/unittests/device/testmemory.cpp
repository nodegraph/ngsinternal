#include "testmemory.h"
#include <base/device/deviceheadersgl.h>

#include <base/memoryallocator/taggednew.h>

#include <base/device/unpackedbuffers/texture.h>
#include <base/device/unpackedbuffers/renderbuffer.h>
#include <base/device/unpackedbuffers/framebuffer.h>
#include <base/device/devicedebug.h>
#include <base/device/pipelinesetups/quadpipelinesetup.h>

#include <iostream>
#include <limits>
#include <vector>

namespace ngs {

TestMemory::TestMemory(QuadPipelineSetup* pipeline):
    _pipeline(pipeline) {

#if GLES_MAJOR_VERSION >= 3
  rundown_memory_with_depth_textures();
#endif

  rundown_memory_with_depth_rbos();
  rundown_memory_with_textures();
}

TestMemory::~TestMemory() {
}

void TestMemory::draw_quad() {
  _pipeline->draw();
}

RenderBuffer* TestMemory::create_depth_rbo(int width, int height) {
  // Render buffer objects fail during storage creation when the gpu memory is around 300MB.
  // You should prefer to use regular textures instead.

#if GLES_MAJOR_VERSION >= 3
  return new_ff RenderBuffer(GL_DEPTH32F_STENCIL8, width, height);
#elif GLES_MAJOR_VERSION >=2
  return new_ff RenderBuffer(GL_DEPTH_COMPONENT16, width, height);
#endif
}

#if GLES_MAJOR_VERSION >= 3
void TestMemory::rundown_memory_with_depth_textures() {
  int count(0);
  std::vector<FrameBuffer*> fbos;
  std::vector<Texture*> depth_textures;
  std::vector<Texture*> textures;

  // Allocate almost all of the gpu memory.
  do {
    ++count;
    std::cerr<<"DepthTexture: #"<<count<<"\n";

    // Create our depth texture.
    Texture* depth_texture = create_depth_stencil_texture(0, _width, _height);
    depth_textures.push_back(depth_texture);

    // Create our fbo.
    FrameBuffer* fbo = new_ff FrameBuffer();
    fbos.push_back(fbo);
    fbo->bind();

    // Create our texture.
    Texture* texture = new_ff Texture(0, FloatElement, _width, _height, 4, false);
    textures.push_back(texture);

    // Attach our depth buffer.
    //fbo->attach_texture(GL_DEPTH_STENCIL_ATTACHMENT,*depth_texture,0);
    fbo->attach_texture(GL_DEPTH_ATTACHMENT, *depth_texture, 0);

    // Attach our stencil buffer.
    fbo->attach_texture(GL_STENCIL_ATTACHMENT, *depth_texture, 0);

    // Attach the texture to the fbo.
    fbo->attach_texture(GL_COLOR_ATTACHMENT0, *texture, 0);

    assert(fbo->is_complete());

    // Clear the color buffer.
    gpu(glClearColor(1, 0, 0, 1));
    gpu(glClear(GL_COLOR_BUFFER_BIT));

    // Clear depth buffer.
    gpu(glClearDepthf(1.0f));
    gpu(glClearDepthf((float)count/1000000.0f));

    draw_quad();

    // Unbind the fbo.
    fbo->unbind();

  } while (count < 100);

  // Cleanup
  for (size_t i = 0; i < textures.size(); i++) {
    delete_ff(textures[i]);
    delete_ff(depth_textures[i]);
    delete_ff(fbos[i]);
    std::cerr << "DepthTexture #" << i << "\n";
  }
}
#endif

void TestMemory::rundown_memory_with_depth_rbos() {
  int count(0);
  std::vector<FrameBuffer*> fbos;
  std::vector<RenderBuffer*> rbos;
  std::vector<Texture*> textures;

  // Allocate almost all of the gpu memory.
  do {
    ++count;

    // Get available memory.
//    available_mem = Device::get_available_memory_kb();
//    std::cerr<<"DepthRBO #"<<count<<" Total mem: "<<total_mem<<"  Available mem: "<<available_mem<<"\n";

    // Create our depth texture.
    RenderBuffer* rbo = create_depth_rbo(_width, _height);
    rbos.push_back(rbo);

    // Create our fbo.
    FrameBuffer* fbo = new_ff FrameBuffer();
    fbos.push_back(fbo);
    fbo->bind();

    // Attach depth buffer.
    fbo->attach_render_buffer(GL_DEPTH_ATTACHMENT, *rbo);

#if GLES_MAJOR_VERSION >= 3
    // Attach stencil buffer.
    fbo->attach_render_buffer(GL_STENCIL_ATTACHMENT, *rbo);
    // Create our texture.
    Texture* texture = new_ff Texture(0, FloatElement, _width, _height, 4, false);
#elif GLES_MAJOR_VERSION >= 2
    // No stencil buffer attachment support in gles 2.0, (i think?)
    // Create our texture.
    Texture* texture = new_ff Texture(0, UCharElement, _width, _height, 4, false);
#endif
    textures.push_back(texture);

    // Attach the texture to the fbo.
    fbo->attach_texture(GL_COLOR_ATTACHMENT0, *texture, 0);

    assert(fbo->is_complete());

    // Clear the color buffer.
    gpu(glClearColor(1, 0, 0, 1));
    gpu(glClear(GL_COLOR_BUFFER_BIT));

    // Clear depth buffer.
    gpu(glClearDepthf((float)count/1000000.0f));
    gpu(glClear(GL_DEPTH_BUFFER_BIT));

    draw_quad();

    // Unbind the fbo.
    fbo->unbind();
  } while(count<100); //while (available_mem > _min_gpu_mem);

  // Cleanup.
  for (size_t i = 0; i < textures.size(); i++) {
    delete_ff(textures[i]);
    delete_ff(rbos[i]);
    delete_ff(fbos[i]);
    std::cerr << "DepthRBO #" << i << "\n";
  }
}

void TestMemory::rundown_memory_with_textures() {
  int count(0);
  std::vector<Texture*> textures;

  FrameBuffer* fbo = new_ff FrameBuffer();

  // Allocate almost all of the gpu memory.
  // Note gpu won't really allocate unless we use it somehow in fbo.
  do {
    ++count;

    // Get available memory.
//    available_mem = Device::get_available_memory_kb();
//    std::cerr<<"Texture #"<<count<<" Total mem: "<<total_mem<<"  Available mem: "<<available_mem<<"\n";

    fbo->bind();

#if GLES_MAJOR_VERSION >= 3
    // Create our texture.
    Texture* texture = new_ff Texture(0, FloatElement, _width, _height, 4, false);
#elif GLES_MAJOR_VERSION >= 2
    Texture* texture = new_ff Texture(0, UCharElement, _width, _height, 4, false);
#endif
	assert(texture);
    textures.push_back(texture);

    fbo->attach_texture(GL_COLOR_ATTACHMENT0, *texture, 0);

    assert(fbo->is_complete());

    // Clear the color buffer.
    gpu(glClearColor(1, 0, 0, 1));
    gpu(glClear(GL_COLOR_BUFFER_BIT));

    // Clear depth buffer.
    gpu(glClearDepthf((float)count/1000000.0f));
    gpu(glClear(GL_DEPTH_BUFFER_BIT));

    draw_quad();

    fbo->unbind();
  } while(count < 100); //while (available_mem > _min_gpu_mem);

  // Cleanup.
  for (size_t i = 0; i < textures.size(); i++) {
    // Delete our texture.
    delete_ff(textures[i]);
    std::cerr << "Texture #" << i << "\n";
  }
  delete_ff(fbo)
  ;
}

}
