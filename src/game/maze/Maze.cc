
# include "Maze.hh"

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

    m_cells(width * height, Cell(sides, false))
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
