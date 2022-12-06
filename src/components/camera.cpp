#include <simplege/simplege.h>

using json = nlohmann::json;

#ifdef WIN32
extern "C"
{
  _declspec(dllexport) DWORD NvOptimusEnablement = 1;
  _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace SimpleGE
{
  namespace Graphic::GL
  {
    static void from_json(const json& j, ColorFloat& desc)
    {
      j.at("r").get_to(desc.red);
      j.at("g").get_to(desc.green);
      j.at("b").get_to(desc.blue);
      j.at("a").get_to(desc.alpha);
    }
  } // namespace Graphic::GL

  static void from_json(const json& j, CameraComponent::Description& desc)
  {
    j.at("title").get_to(desc.title);
    j.at("viewHeight").get_to(desc.viewHeight);
    j.at("viewWidth").get_to(desc.viewWidth);
    j.at("color").get_to(desc.color);
    j.at("height").get_to(desc.height);
    j.at("near").get_to(desc.nearClip);
    j.at("far").get_to(desc.farClip);
    j.at("compositors").get_to(desc.compositors);
  }

  static void InitDisplaySystem()
  {
    static bool initialized = false;
    if (initialized)
    {
      return;
    }

    if (glfwInit() != GLFW_TRUE)
    {
      fmt::print(stderr, "Échec à l'initialisation de GLFW\n");
      std::terminate();
    }

    initialized = true;
  }

  static void GLAPIENTRY OnGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                          const GLchar* message, const void* userParam)
  {
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    {
      return;
    }

    fmt::print(stderr, "GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
               (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    Expects(type != GL_DEBUG_TYPE_ERROR);
  }

  CameraComponent::CameraComponent(Entity& owner) : Component(owner) { DisplaySystem::Instance().Register(this); }

  CameraComponent::~CameraComponent() { DisplaySystem::Instance().Unregister(this); }

  Component::SetupResult CameraComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult CameraComponent::Setup(const Description& descr)
  {
    current = this;

    InitDisplaySystem();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    viewWidth = descr.viewWidth;
    viewHeight = descr.viewHeight;
    height = descr.height;
    nearClip = descr.nearClip;
    farClip = descr.farClip;
    clearColor = descr.color;
    window = glfwCreateWindow(ViewWidth(), ViewHeight(), descr.title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
      fmt::print(stderr, "Cannot get an OpenGL-compatible window, check support for OpenGL 4.3 with https://www.realtech-vr.com/home/glview");
      std::terminate();
    }

    glfwMakeContextCurrent(window);

    fmt::print("Vendor: {} Renderer: {}\n", reinterpret_cast<const char*>(glGetString(GL_VENDOR)), reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    if (auto err = glewInit(); err != GLEW_OK)
    {
      fmt::print(stderr, "Erreur: {}\n", reinterpret_cast<const char*>(glewGetErrorString(err)));
      glfwTerminate();
      std::terminate();
    }

    Graphic::GL::Enable(Graphic::GL::Capabilities::DebugOutput);
    Graphic::GL::Enable(Graphic::GL::Capabilities::DebugOutputSynchronous);

    glDebugMessageCallback(&OnGLDebugMessage, nullptr);

    Graphic::GL::Enable(Graphic::GL::Capabilities::DepthTest);
    Graphic::GL::DepthFunc(Graphic::GL::DepthFuncT::LEqual);

    Graphic::GL::SetViewport(0, 0, ViewWidth(), ViewHeight());

    framebuffer = &Graphic::GL::Framebuffer::GetDefault();
    framebuffer->ClearDepth();
    framebuffer->ClearColor(clearColor);

    {
      using namespace Graphic;

      renderTexture =
          RenderTexture::Create({ViewWidth(), ViewHeight(), GL::TextureFilter::Nearest, GL::TextureFilter::Nearest,
                                 GL::TextureWrap::Clamp, GL::TextureWrap::Clamp});
    }

    renderTexture->ClearDepth();
    renderTexture->ClearColor(clearColor);

    compositors = std::move(descr.compositors);

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    return {this, {}};
  }

  void CameraComponent::Render(const Timing& timing)
  {
    current = this;

    auto rt = renderTexture;

    for (const auto& comp : compositors)
    {
      if (comp->Enabled())
      {
        rt = comp->Compose(rt);
      }
    }

    renderTexture->BindFramebuffer();

    glfwSwapBuffers(window);
    glfwPollEvents();

    renderTexture->ClearDepth();
    renderTexture->ClearColor(clearColor);

    if (glfwWindowShouldClose(window) == GLFW_TRUE)
    {
      Game::Close();
    }
  }

  const Matrix<4, 4>& CameraComponent::Projection()
  {
    gsl::not_null<PositionComponent*> posComp = Owner().GetComponent<PositionComponent>();
    auto position = posComp->WorldPosition();
    float x = position[0];
    float y = position[1];
    float z = position[2];
    float ratio = float(ViewWidth()) / float(ViewHeight());
    float width = ratio * height;

    return projection.Ortho(x - width, x + width, -y + height, -y - height, z + nearClip, z + farClip);
  }
} // namespace SimpleGE