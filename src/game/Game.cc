
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

    // Default maze is a 50x50 square one with Kruksal algorithm.
    m_width(50u),
    m_height(50u),
    m_strategy(maze::Strategy::DepthFirst),
    m_sides(4u),
    m_maze(std::make_shared<maze::SquareMaze>(m_width, m_height, m_strategy))
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
    std::string str = "Dims: " + std::to_string(m_width) + "x" + std::to_string(m_height);
    m_menus.dims = generateMenu(pos, dims, str, "dims");
    m_menus.strategy = generateMenu(pos, dims, "Strategy: " + maze::strategyToString(m_strategy), "strat");
    str = (m_sides == 3u ? "triangle" : m_sides == 4u ? "square" : "hexagon");
    m_menus.sides = generateMenu(pos, dims, "Cell kind: " + str, "sides");
    MenuShPtr gen = generateMenu(pos, dims, "Generate !", "generate", true);
    gen->setSimpleAction(
      [this](Game& g) {
        g.generateMaze();
      }
    );

    // Register menus in the parent.
    status->addMenu(m_menus.dims);
    status->addMenu(m_menus.strategy);
    status->addMenu(m_menus.sides);
    status->addMenu(gen);

    // Generate the menu for the generation properties.
    MenuShPtr props = generateMenu(olc::vi2d(0, height - SIDES_MENU_HEIGHT), olc::vi2d(width, SIDES_MENU_HEIGHT), "", "sides");

    dims = olc::vi2d(50, SIDES_MENU_HEIGHT);

    MenuShPtr triangle = generateMenu(pos, dims, "Triangle", "triangle", true, true);
    MenuShPtr square = generateMenu(pos, dims, "Square", "square", true, true);
    MenuShPtr hexagon = generateMenu(pos, dims, "Hexagon", "hexagon", true, true);

    // Register menus in the parent.
    props->addMenu(triangle);
    props->addMenu(square);
    props->addMenu(hexagon);
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

    MenuShPtr kruskal = generateMenu(pos, dims, "Kruskal", "kruskal", true, true);
    MenuShPtr prim = generateMenu(pos, dims, "Prim", "prim", true, true);
    MenuShPtr aldousBroder = generateMenu(pos, dims, "Depth-first", "depthfirst", true, true);

    // Register menus in the parent.
    props->addMenu(kruskal);
    props->addMenu(prim);
    props->addMenu(aldousBroder);
    kruskal->setSimpleAction(
      [this](Game& g) {
        g.setGenerationStrategy(maze::Strategy::RandomizedKruskal);
      }
    );
    prim->setSimpleAction(
      [this](Game& g) {
        g.setGenerationStrategy(maze::Strategy::RandomizedPrim);
      }
    );
    aldousBroder->setSimpleAction(
      [this](Game& g) {
        g.setGenerationStrategy(maze::Strategy::DepthFirst);
      }
    );

    // Package menus for output.
    std::vector<MenuShPtr> menus;

    menus.push_back(status);
    menus.push_back(props);

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
  Game::setGenerationStrategy(const maze::Strategy& strategy) noexcept {
    // Only available when the game is not paused.
    if (m_state.paused) {
      return;
    }

    // Prevent updates with no changes.
    if (m_strategy == strategy) {
      return;
    }

    m_strategy = strategy;
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
      " and " + std::to_string(m_sides) + " side(s) with strategy " +
      maze::strategyToString(m_strategy),
      utils::Level::Info
    );

    {
      utils::ChronoMilliseconds c("Maze generated", "maze");
      m_maze->generate();
    }
  }

  void
  Game::load(const std::string& file) {

    // Generate a new maze.
    maze::MazeShPtr m = maze::Maze::fromFile(file);
    if (m == nullptr) {
      error("Failed to load maze from file \"" + file + "\"");

      return;
    }

    // In case the loading succeeded, reset the internal
    // attribute.
    m_maze = m;

    log(
      "Loaded maze from file \"" + file + "\" with dimensions " +
      std::to_string(m_maze->width()) + "x" + std::to_string(m_maze->height()),
      utils::Level::Info
    );

    // Update internal properties.
    m_width = m_maze->width();
    m_height = m_maze->height();
    m_sides = m_maze->sides();
  }

  void
  Game::save(const std::string& file) const {
    if (m_maze == nullptr) {
      return;
    }

    m_maze->save(file);
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
    std::string str = "Dims: " + std::to_string(m_width) + "x" + std::to_string(m_height);
    m_menus.dims->setText(str);

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

    m_menus.strategy->setText("Strategy: " + maze::strategyToString(m_strategy));

    m_menus.sides->setText("Cell kind: " + text);
  }

  void
  Game::resetMaze() noexcept {
    // Generate a new maze.
    m_maze.reset();

    switch (m_sides) {
      case 3u:
        m_maze = std::make_shared<maze::TriangleMaze>(m_width, m_height, m_strategy);
        break;
      case 4u:
        m_maze = std::make_shared<maze::SquareMaze>(m_width, m_height, m_strategy);
        break;
      case 6u:
        m_maze = std::make_shared<maze::HexagonMaze>(m_width, m_height, m_strategy);
        break;
      default:
        break;
    }

    if (m_maze == nullptr) {
      error(
        "Failed to generate new maze",
        "Unsupported configuration with dimensions " +
        std::to_string(m_width) + "x" + std::to_string(m_height) +
        " and " + std::to_string(m_sides) + " side(s) with strategy " +
        maze::strategyToString(m_strategy)
      );
    }

    log(
      "Created new maze with dimensions " +
      std::to_string(m_width) + "x" + std::to_string(m_height) +
      " and " + std::to_string(m_sides) + " side(s) with strategy " +
      maze::strategyToString(m_strategy),
      utils::Level::Info
    );
  }

}
