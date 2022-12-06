#include <simplege/simplege.h>

using json = nlohmann::json;

namespace SimpleGE
{
  static void from_json(const json& j, SpriteComponent::Description& desc)
  {
    if (j.contains("frameSkip"))
    {
      j.at("frameSkip").get_to(desc.frameSkip);
    }
    if (j.contains("isAnimated"))
    {
      j.at("isAnimated").get_to(desc.isAnimated);
    }
    if (j.contains("animWait"))
    {
      j.at("animWait").get_to(desc.animWait);
    }
    if (j.contains("spriteName"))
    {
      j.at("spriteName").get_to(desc.spriteName);
    }
    j.at("spriteSheet").get_to(desc.spriteSheet);
  }

  Component::SetupResult SpriteComponent::Setup(const nlohmann::json& descr) { return Setup(descr.get<Description>()); }

  Component::SetupResult SpriteComponent::Setup(const Description& descr)
  {
    frameSkip = descr.frameSkip;
    isAnimated = descr.isAnimated;
    animWait = descr.animWait;
    spriteName = descr.spriteName;
    animationFrame = 1;
    animWaitCounter = animWait;
    spriteSheet = descr.spriteSheet;

    return {this,
            {{[this]() { return spriteSheet.Ready(); },
              [this]()
              {
                // On crée ici un tableau de 4 vertices permettant de représenter
                // le rectangle à afficher.
                vertexBuffer = Graphic::CreateVertexBuffer<Vertex, Graphic::BufferUsage::Dynamic>(vertices.size());

                // On crée ici un tableau de 6 indices, soit 2 triangles, pour
                // représenter quels vertices participent à chaque triangle:
                // ```
                // 0    1
                // +----+
                // |\   |
                // | \  |
                // |  \ |
                // |   \|
                // +----+
                // 3    2
                // ```
                indexBuffer = Graphic::CreateIndexBuffer<std::uint16_t, Graphic::BufferUsage::Dynamic>(Indices.size());

                // Et on initialise le contenu des vertices
                UpdateMesh();
                UpdateComponents();
              }}}};
  }

  // Cette méthode met à jour l'animation de la sprite, si il
  // y a lieu, et met à jour le contenu des vertices afin de tenir
  // compte des changements de position et autres.
  void SpriteComponent::UpdateLogic(const Timing& timing)
  {
    if (isAnimated)
    {
      if (animWaitCounter > 0)
      {
        animWaitCounter--;
      }
      else if ((timing.frame % frameSkip) == 0)
      {
        UpdateMesh();
      }
    }

    UpdateComponents();
  }

  // La méthode *display* choisit le shader et la texture appropriée
  // via la méthode *bind* de la feuille de sprite, sélectionne le
  // tableau de vertices et d'indices et fait l'appel de rendu.
  void SpriteComponent::Display(const Timing& timing)
  {
    spriteSheet->Bind(vertexBuffer, indexBuffer);
    Graphic::GL::DrawElements(Indices);
    spriteSheet->Unbind();
  }

  // Cette méthode met à jour les informations relatives à la sprite
  // à afficher.
  void SpriteComponent::UpdateMesh()
  {
    const auto& newSpriteName = isAnimated ? FindNextFrameName() : spriteName;
    Expects(spriteSheet != nullptr);
    descr = spriteSheet->GetSprite(newSpriteName);
    if (!descr)
      return; // HACK
    Ensures(descr);
    spriteSize = descr->sourceSize;
  }

  void SpriteComponent::SetSpriteName(std::string_view spriteName)
  {
    this->spriteName = spriteName;
    UpdateComponents();
  }

  // La fonction *FindNextFrameName* détermine le nom de la sprite
  // à afficher dans une animation, et déclenche des événements
  // enregistrés si on atteint la fin de l'animation.
  std::string SpriteComponent::FindNextFrameName()
  {
    auto animationSprite = fmt::format("{}{}", spriteName, animationFrame);
    if (spriteSheet->GetSprite(animationSprite) != nullptr)
    {
      animationFrame++;
      return animationSprite;
    }
    if (animationFrame == 1)
    {
      return spriteName;
    }
    else
    {
      animationFrame = 1;
      animWaitCounter = animWait;
      animationEndedEvent.Trigger();
      return FindNextFrameName();
    }
  }

  // Cette méthode met à jour le contenu de chaque vertex, soient
  // leurs position et les coordonnées de texture, en tenant compte
  // des transformations et de la sprite courante.
  void SpriteComponent::UpdateComponents()
  {
    if (!descr)
      return; // HACK
    gsl::not_null<PositionComponent*> posComp = Owner().GetComponent<PositionComponent>();
    const auto position = posComp->WorldPosition();

    const auto z = position[2];
    const auto xMin = position[0];
    const auto xMax = xMin + float(descr->frame.width());
    const auto yMax = position[1];
    const auto yMin = yMax - float(descr->frame.height());
    const auto uMin = descr->uv.x();
    const auto uMax = uMin + float(descr->uv.width());
    const auto vMin = descr->uv.y();
    const auto vMax = vMin + float(descr->uv.height());

    std::array<Vertex, 4> v{{
        {xMin, yMin, z, uMin, vMin},
        {xMax, yMin, z, uMax, vMin},
        {xMax, yMax, z, uMax, vMax},
        {xMin, yMax, z, uMin, vMax},
    }};

    std::copy(v.begin(), v.end(), vertices.begin());
    vertexBuffer->Update(gsl::span<const Vertex>(vertices), 0);
    indexBuffer->Update(gsl::span<const std::uint16_t>(Indices), 0);
  }
} // namespace SimpleGE