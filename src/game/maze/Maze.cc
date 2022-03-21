
# include "Maze.hh"
# include <fstream>

namespace maze {

  Maze::Maze(unsigned width,
             unsigned height,
             const Strategy& strategy,
             unsigned sides):
    utils::CoreObject("maze"),

    m_width(width),
    m_height(height),

    m_strategy(strategy),

    m_cellSides(sides),

    m_cells(m_width * m_height, Cell(m_cellSides, false))
  {
    setService("maze");
  }

  unsigned
  Maze::width() const noexcept {
    return m_width;
  }

  unsigned
  Maze::height() const noexcept {
    return m_height;
  }

  unsigned
  Maze::sides() const noexcept {
    return m_cellSides;
  }

  const Cell&
  Maze::at(unsigned x, unsigned y) const {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch cell at coordinate " + std::to_string(x) + "x" + std::to_string(y),
        "Maze only has dimensions " + std::to_string(m_width) + "x" + std::to_string(m_height)
      );
    }

    return m_cells[linear(x, y)];
  }

  Cell
  Maze::at(unsigned x, unsigned y) {
    if (x >= m_width || y >= m_height) {
      error(
        "Failed to fetch cell at coordinate " + std::to_string(x) + "x" + std::to_string(y),
        "Maze only has dimensions " + std::to_string(m_width) + "x" + std::to_string(m_height)
      );
    }

    return m_cells[linear(x, y)];
  }

  void
  Maze::open() {
    for (unsigned id = 0u ; id < m_cells.size() ; ++id) {
      m_cells[id].open();
    }
  }

  void
  Maze::close() {
    for (unsigned id = 0u ; id < m_cells.size() ; ++id) {
      m_cells[id].close();
    }
  }

  MazeShPtr
  Maze::fromFile(const std::string& /*file*/) {
    /// TODO: Handle loading of maze from file.
    return nullptr;
  }

  void
  Maze::save(const std::string& file) const {
    // Open the file and verify that it is valid.
    std::ofstream out(file.c_str());
    if (!out.good()) {
      error(
        "Failed to save world to \"" + file + "\"",
        "Failed to open file"
      );
    }

    // Save the dimensions of the maze and the type of
    // the cells.
    unsigned size = sizeof(unsigned);
    const char* raw = reinterpret_cast<const char*>(&m_width);
    out.write(raw, size);

    raw = reinterpret_cast<const char*>(&m_height);
    out.write(raw, size);

    raw = reinterpret_cast<const char*>(&m_cellSides);
    out.write(raw, size);

    /// TODO: Handle serialization of maze.

    // Close the file so that we save the data.
    out.close();

    log("Saved maze to \"" + file + "\"", utils::Level::Info);
  }

  void
  Maze::generate() {
    // Nothing to generate if the maze is empty.
    if (m_cells.empty()) {
      return;
    }

    switch (m_strategy) {
      case Strategy::RandomizedKruskal:
        kruksal::generate(*this);
        break;
      case Strategy::RandomizedPrim:
      case Strategy::AldousBroder:
      default:
        warn(
          "Failed to generate maze",
          "Unsupported generation strategy " + strategyToString(m_strategy)
        );
        break;
    }
  }

  unsigned
  Maze::linear(unsigned x, unsigned y) const noexcept {
    return y * m_width + x;
  }

}
