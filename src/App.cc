
# include "App.hh"
# include <cstdlib>
# include <maths_utils/ComparisonUtils.hh>

namespace pge {

  App::App(const AppDesc& desc):
    PGEApp(desc),

    m_game(nullptr),
    m_state(nullptr),
    m_menus(),

    m_packs(std::make_shared<TexturePack>())
  {}

  bool
  App::onFrame(float fElapsed) {
    // Handle case where no game is defined.
    if (m_game == nullptr) {
      return false;
    }

    if (!m_game->step(fElapsed)) {
      log("This is game over", utils::Level::Info);
    }

    return m_game->terminated();
  }

  void
  App::onInputs(const controls::State& c,
                const CoordinateFrame& cf)
  {
    // Handle case where no game is defined.
    if (m_game == nullptr) {
      return;
    }

    // Handle menus update and process the
    // corresponding actions.
    std::vector<ActionShPtr> actions;
    bool relevant = false;

    for (unsigned id = 0u ; id < m_menus.size() ; ++id) {
      menu::InputHandle ih = m_menus[id]->processUserInput(c, actions);
      relevant = (relevant || ih.relevant);
    }

    if (m_state != nullptr) {
      menu::InputHandle ih = m_state->processUserInput(c, actions);
      relevant = (relevant || ih.relevant);
    }

    for (unsigned id = 0u ; id < actions.size() ; ++id) {
      actions[id]->apply(*m_game);
    }

    bool lClick = (c.buttons[controls::mouse::Left] == controls::ButtonState::Released);
    if (lClick && !relevant) {
      olc::vf2d it;
      olc::vi2d tp = cf.pixelCoordsToTiles(olc::vi2d(c.mPosX, c.mPosY), &it);

      m_game->performAction(tp.x + it.x, tp.y + it.y);
    }

    if (c.keys[controls::keys::P]) {
      m_game->togglePause();
    }

    if (c.keys[controls::keys::G]) {
      m_game->generateMaze();
    }
  }

  void
  App::loadData() {
    // Create the game and its state.
    m_game = std::make_shared<Game>();
  }

  void
  App::loadResources() {
    // Assign a specific tint to the regular
    // drawing layer so that we have a built
    // in transparency.
    // We can't do it directly when drawing
    // in the rendering function because as
    // the whole layer will be drawn as one
    // quad in opengl with an opaque alpha,
    // we will lose this info.
    // This means that everything is indeed
    // transparent but that's the only way
    // for now to achieve it.
    setLayerTint(Layer::Draw, olc::Pixel(255, 255, 255, alpha::SemiOpaque));

    log("Load app resources in the 'm_packs' attribute", utils::Level::Info);
  }

  void
  App::loadMenuResources() {
    // Generate the game state.
    m_state = std::make_shared<GameState>(
      olc::vi2d(ScreenWidth(), ScreenHeight()),
      Screen::Game
    );

    m_menus = m_game->generateMenus(ScreenWidth(), ScreenHeight());
  }

  void
  App::cleanResources() {
    if (m_packs != nullptr) {
      m_packs.reset();
    }
  }

  void
  App::cleanMenuResources() {
    m_menus.clear();
  }

  void
  App::drawDecal(const RenderDesc& /*res*/) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::VERY_DARK_GREY);

    // In case we're not in the game screen, do nothing.
    if (m_state->getScreen() != Screen::Game) {
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::draw(const RenderDesc& res) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    const maze::Maze& m = m_game->maze();
    switch (m.sides()) {
      case 3u:
        drawTriangularMaze(res, m);
        break;
      case 4u:
        drawSquareMaze(res, m);
        break;
      case 5u:
        drawPentagonalMaze(res, m);
        break;
      case 6u:
        drawHexagonalMaze(res, m);
        break;
      default:
        error(
          "Failed to render maze",
          "Cells with " + std::to_string(m.sides()) + " side(s) are not supported"
        );
        break;
    }

    drawOverlays(res);

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawUI(const RenderDesc& /*res*/) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    // Render the game menus.
    for (unsigned id = 0u ; id < m_menus.size() ; ++id) {
      m_menus[id]->render(this);
    }

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawDebug(const RenderDesc& res) {
    // Clear rendering target.
    SetPixelMode(olc::Pixel::ALPHA);
    Clear(olc::Pixel(255, 255, 255, alpha::Transparent));

    // In case we're not in game mode, just render
    // the state.
    if (m_state->getScreen() != Screen::Game) {
      m_state->render(this);
      SetPixelMode(olc::Pixel::NORMAL);
      return;
    }

    // Draw cursor's position.
    olc::vi2d mp = GetMousePos();
    olc::vf2d it;
    olc::vi2d mtp = res.cf.pixelCoordsToTiles(mp, &it);

    int h = GetDrawTargetHeight();
    int dOffset = 15;
    DrawString(olc::vi2d(0, h / 2), "Mouse coords      : " + toString(mp), olc::CYAN);
    DrawString(olc::vi2d(0, h / 2 + 1 * dOffset), "World cell coords : " + toString(mtp), olc::CYAN);
    DrawString(olc::vi2d(0, h / 2 + 2 * dOffset), "Intra cell        : " + toString(it), olc::CYAN);

    SetPixelMode(olc::Pixel::NORMAL);
  }

  void
  App::drawOverlays(const RenderDesc& res) noexcept {
    SpriteDesc sd = {};
    sd.loc = pge::RelativePosition::Center;
    sd.radius = 1.0f;

    olc::vi2d mp = GetMousePos();
    olc::vi2d mtp = res.cf.pixelCoordsToTiles(mp, nullptr);

    sd.x = 1.0f * mtp.x;
    sd.y = 1.0f * mtp.y;

    sd.sprite.tint = olc::Pixel(0, 255, 0, pge::alpha::AlmostTransparent);
    drawRect(sd, res.cf);
  }

  void
  App::drawTriangularMaze(const RenderDesc& /*res*/, const maze::Maze& /*maze*/) noexcept {
    /// TODO: Handle this.
    DrawStringDecal(olc::vi2d(200, 300), "Unsupported triangular rendering !", olc::RED);
  }

  void
  App::drawSquareMaze(const RenderDesc& res, const maze::Maze& maze) noexcept {
    // Colors for the cells and their borders.
    olc::Pixel cell(13, 2, 8);
    olc::Pixel border(0, 143, 17);
    // olc::Pixel border(0, 255, 65);
    // olc::Pixel border(0, 59, 0);

    SpriteDesc sd = {};
    sd.loc = pge::RelativePosition::Center;

    // Assume that the tile size is a square and scale
    // the cells so that they occupy one tile.
    sd.radius = 1.0f;

    // A convenience lambda to draw a door.
    auto drawDoor = [this, &border, &res](unsigned x, unsigned y, unsigned door) {
      // Note that as we want to draw a rectangle and
      // not a square, we have to rely to the low-level
      // drawing primitives.
      float dx = x;
      float dy = y;

      // The dimensions of the doors.
      float doorPerc = 0.05f;

      olc::vf2d size = res.cf.tileSize();

      bool valid = true;
      switch (door) {
        case 0u:
          // Right door.
          dx += (1.0f - doorPerc);
          size.x *= doorPerc;
          break;
        case 1u:
          // Bottom door.
          dy += (1.0f - doorPerc);
          size.y *= doorPerc;
          break;
        case 2u:
          // Left door.
          size.x *= doorPerc;
          break;
        case 3u:
          // Top door.
          size.y *= doorPerc;
          break;
        default:
          valid = false;
          warn("Requesting to draw door " + std::to_string(door) + " at " + std::to_string(x) + "x" + std::to_string(y));
          break;
      }

      // In case of an invalid door do not draw anything
      if (!valid) {
        return;
      }

      // Clamp the size of a door to always be at least one
      // pixel wide so that it is visible.
      size.x = std::max(size.x, 1.0f);
      size.y = std::max(size.y, 1.0f);

      olc::vf2d p = res.cf.tileCoordsToPixels(dx, dy, pge::RelativePosition::Center, 1.0f);
      FillRectDecal(p, size, border);
    };

    unsigned count = 0u;

    // We assume the cell `0, 0` of the maze has the same
    // world coordinates.
    for (unsigned y = 0u ; y < maze.height() ; ++y) {
      for (unsigned x = 0u ; x < maze.width() ; ++x) {
        sd.x = x;
        // The maze is generated upside down.
        sd.y = maze.height() - 1u - y;

        // Discard invisible cells.
        if (!res.visible(olc::vf2d(sd.x, sd.y))) {
          ++count;
          continue;
        }

        sd.sprite.tint = cell;

        drawRect(sd, res.cf);

        // Draw each closed door of the cell.
        const maze::Cell& c = maze.at(x, y);

        if (!c(0u)) {
          drawDoor(sd.x, sd.y, 0u);
        }
        if (!c(1u)) {
          drawDoor(sd.x, sd.y, 1u);
        }
        if (!c(2u)) {
          drawDoor(sd.x, sd.y, 2u);
        }
        if (!c(3u)) {
          drawDoor(sd.x, sd.y, 3u);
        }
      }
    }
  }

  void
  App::drawPentagonalMaze(const RenderDesc& /*res*/, const maze::Maze& /*maze*/) noexcept {
    /// TODO: Handle this.
    DrawStringDecal(olc::vi2d(200, 300), "Unsupported pentagonal rendering !", olc::RED);
  }

  void
  App::drawHexagonalMaze(const RenderDesc& /*res*/, const maze::Maze& /*maze*/) noexcept {
    /// TODO: Handle this.
    DrawStringDecal(olc::vi2d(200, 300), "Unsupported hexagonal rendering !", olc::RED);
  }

}
