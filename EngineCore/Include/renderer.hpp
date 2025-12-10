#include "vulkanInit.hpp"
#include <vector>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Renderer
{
public:
  GLFWwindow *window;
  VkInstance instance;

  Renderer(GLFWwindow *window);
  void init();
  ~Renderer();

private:
  void cleanup();
};