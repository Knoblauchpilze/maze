
# include "Game.hh"
# include <cxxabi.h>
# include <core_utils/Chrono.hh>
# include "Menu.hh"
# include "Triangle.hh"
# include "Square.hh"
# include "Hexagon.hh"

/// @brief - The height of the status menu in pixels.
# define STATUS_MENU_HEIGHT 50

/// @brief - The height of the sides menu in pixels.
# define SIDES_MENU_HEIGHT 50

/// @brief - The default dimensions of the maze.
# define MAZE_DEFAULT_DIMENSIONS 50

namespace {

  pge::MenuShPtr
  generateMenu(const olc::vi2d& pos,
               const olc::vi2d& size,
               const std::string& text,
               const std::string& name,
               bool clickable = false,
               bool selectable = false)
  {
    pge::menu::MenuContentDesc fd = pge::menu::newMenuContent(text, "", size);
    fd.color = olc::WHITE;
    fd.hColor = olc::GREY;
    fd.align = pge::menu::Alignment::Center;

    return std::make_shared<pge::Menu>(
      pos,
      size,
      name,
      pge::menu::newColoredBackground(olc::VERY_DARK_ORANGE),
      fd,
      pge::menu::Layout::Horizontal,
      clickable,
      selectable
    );
  }

}

namespace pge {

  Game::Game():
    utils::CoreObject("game"),

    m_state(
      State{
        true,  // paused
        true,  // disabled
        false, // terminated
      }
    ),

    m_menus(),

    m_width(MAZE_DEFAULT_DIMENSIONS),
    m_height(MAZE_DEFAULT_DIMENSIONS),
    m_sides(3u),
    m_maze(std::make_shared<maze::TriangleMaze>(m_width, m_height))
  {
    setService("game");
  }

  Game::~Game() {}

  const maze::Maze&
  Game::maze() const noexcept {
    return *m_maze;
  }

  std::vector<MenuShPtr>
  Game::generateMenus(float width,
                      float height)
  {
    // Generate the status menu.
    MenuShPtr status = generateMenu(olc::vi2d(), olc::vi2d(width, STATUS_MENU_HEIGHT), "", "status");

    olc::vi2d pos;
    olc::vi2d dims(50, STATUS_MENU_HEIGHT);
    m_menus.width = generateMenu(pos, dims, "Width: " + std::to_string(m_width), "width");
    m_menus.height = generateMenu(pos, dims, "Height: " + std::to_string(m_height), "height");
    m_menus.sides = generateMenu(pos, dims, "Sides: " + std::to_string(m_sides), "sides");
    MenuShPtr gen = generateMenu(pos, dims, "Generate !", "generate", true);
    gen->setSimpleAction(
      [this](Game& g) {
        g.generateMaze();
      }
    );

    // Register menus in the parent.
    status->addMenu(m_menus.width);
    status->addMenu(m_menus.height);
    status->addMenu(m_menus.sides);
    status->addMenu(gen);

    // Generate the menu for the type of cells.
    MenuShPtr cells = generateMenu(olc::vi2d(0, height - SIDES_MENU_HEIGHT), olc::vi2d(width, SIDES_MENU_HEIGHT), "", "sides");

    dims = olc::vi2d(50, SIDES_MENU_HEIGHT);

    MenuShPtr triangle = generateMenu(pos, dims, "Triangle", "triangle", true, true);
    MenuShPtr square = generateMenu(pos, dims, "Square", "square", true, true);
    MenuShPtr hexagon = generateMenu(pos, dims, "Hexagon", "hexagon", true, true);

    // Register menus in the parent.
    cells->addMenu(triangle);
    cells->addMenu(square);
    cells->addMenu(hexagon);
    triangle->setSimpleAction(
      [this](Game& g) {
        g.setCellSidesCount(3u);
      }
    );
    square->setSimpleAction(
      [this](Game& g) {
        g.setCellSidesCount(4u);
      }
    );
    hexagon->setSimpleAction(
      [this](Game& g) {
        g.setCellSidesCount(6u);
      }
    );

    // Package menus for output.
    std::vector<MenuShPtr> menus;

    menus.push_back(status);
    menus.push_back(cells);

    return menus;
  }

  void
  Game::performAction(float /*x*/, float /*y*/) {
    // Only handle actions when the game is not disabled.
    if (m_state.disabled) {
      log("Ignoring action while menu is disabled");
      return;
    }
  }

  bool
  Game::step(float /*tDelta*/) {
    // When the game is paused it is not over yet.
    if (m_state.paused) {
      return true;
    }

    updateUI();

    return true;
  }

  void
  Game::togglePause() {
    if (m_state.paused) {
      resume();
    }
    else {
      pause();
    }

    enable(!m_state.paused);
  }

  void
  Game::setMazeWidth(unsigned width) noexcept {
    // Only available when the game is not paused.
    if (m_state.paused) {
      return;
    }

    // Prevent updates with no changes.
    if (m_width == width) {
      return;
    }

    m_width = width;
    resetMaze();
  }

  void
  Game::setMazeHeight(unsigned height) noexcept {
    // Only available when the game is not paused.
    if (m_state.paused) {
      return;
    }

    // Prevent updates with no changes.
    if (m_height == height) {
      return;
    }

    m_height = height;
    resetMaze();
  }

  void
  Game::setCellSidesCount(unsigned sides) noexcept {
    // Only available when the game is not paused.
    if (m_state.paused) {
      return;
    }

    // Prevent updates with no changes.
    if (m_sides == sides) {
      return;
    }

    m_sides = sides;
    resetMaze();
  }

  void
  Game::generateMaze() {
    // Only available when the game is not paused.
    if (m_state.paused) {
      return;
    }

    // Only available if a maze is defined.
    if (m_maze == nullptr) {
      warn("Failed to generate new maze", "No maze defined");
      return;
    }

    log(
      "Generating maze with dimensions " +
      std::to_string(m_width) + "x" + std::to_string(m_height) +
      " and " + std::to_string(m_sides) + " side(s)",
      utils::Level::Info
    );

    {
      utils::ChronoMilliseconds c("Maze generated", "maze");
      m_maze->generate();
    }
  }

  void
  Game::enable(bool enable) {
    m_state.disabled = !enable;

    if (m_state.disabled) {
      log("Disabled game UI", utils::Level::Verbose);
    }
    else {
      log("Enabled game UI", utils::Level::Verbose);
    }
  }

  void
  Game::updateUI() {
    // Update the dimensions of the maze.
    m_menus.width->setText("Width: " + std::to_string(m_width));
    m_menus.height->setText("Height: " + std::to_string(m_height));

    // Update the number of sides of each cell.
    std::string text = "Unknown";
    switch (m_sides) {
      case 3u:
        text = "triangle";
        break;
      case 4u:
        text = "square";
        break;
      case 6u:
        text = "hexagon";
        break;
      default:
        break;
    }

    m_menus.sides->setText("Cell kind: " + text);
  }

  void
  Game::resetMaze() noexcept {
    // Generate a new maze.
    m_maze.reset();

    switch (m_sides) {
      case 3u:
        m_maze = std::make_shared<maze::TriangleMaze>(m_width, m_height);
        break;
      case 4u:
        m_maze = std::make_shared<maze::SquareMaze>(m_width, m_height);
        break;
      case 6u:
        m_maze = std::make_shared<maze::HexagonMaze>(m_width, m_height);
        break;
      default:
        break;
    }

    if (m_maze == nullptr) {
      error(
        "Failed to generate new maze",
        "Unsupported configuration with dimensions " +
        std::to_string(m_width) + "x" + std::to_string(m_height) +
        " and " + std::to_string(m_sides) + " side(s)"
      );
    }

    log(
      "Created new maze with dimensions " +
      std::to_string(m_width) + "x" + std::to_string(m_height) +
      " and " + std::to_string(m_sides) + " side(s)",
      utils::Level::Info
    );
  }

}
