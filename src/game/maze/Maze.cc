
# include "Maze.hh"
# include <fstream>
# include <core_utils/CoreException.hh>
# include <core_utils/BitReader.hh>
# include <core_utils/BitWriter.hh>
# include "Triangle.hh"
# include "Square.hh"
# include "Hexagon.hh"

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
  Maze::fromFile(const std::string& file) {
    // Open the file and verify that it is valid.
    std::ifstream out(file.c_str());
    if (!out.good()) {
      throw utils::CoreException(
        "Failed to load maze from file \"" + file + "\"",
        "maze",
        "maze",
        "No such file"
      );
    }

    // Read the dimensions and general properties.
    unsigned w = 0u, h = 0u, sides = 0u;

    out.read(reinterpret_cast<char*>(&w), sizeof(unsigned));
    out.read(reinterpret_cast<char*>(&h), sizeof(unsigned));
    out.read(reinterpret_cast<char*>(&sides), sizeof(unsigned));

    // Consistency check.
    if (w == 0u || h == 0u || (sides != 3u && sides != 4u && sides != 6u)) {
      throw utils::CoreException(
        "Failed to load maze from file \"" + file + "\"",
        "maze",
        "maze",
        "Invalid maze definition " + std::to_string(w) + "x" +
        std::to_string(h) + " sides: " + std::to_string(sides)
      );
    }

    MazeShPtr mz;
    switch (sides) {
      case 3u:
        mz = std::make_shared<TriangleMaze>(w, h, Strategy::RandomizedKruskal);
        break;
      case 4u:
        mz = std::make_shared<SquareMaze>(w, h, Strategy::RandomizedKruskal);
        break;
      case 6u:
      default:
        mz = std::make_shared<HexagonMaze>(w, h, Strategy::RandomizedKruskal);
        break;
    }

    // Read the cells.
    utils::BitReader br(out);
    for (unsigned y = 0u ; y < h ; ++y) {
      for (unsigned x = 0u ; x < w ; ++x) {
        // Reset the doors of the cell.
        Cell& c = mz->m_cells[mz->linear(x, y)];
        c.close();

        // Update doors based on the content of the file.
        for (unsigned d = 0u ; d < c.doors() ; ++d) {
          c.toggle(d, br.read());
        }
      }
    }

    return mz;
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

    // NOTE: we want to store a single bit for each door.
    // This is not possible out of the box from the tools
    // provided by the fstream but we can accumulate the
    // bits and flush them when needed.
    utils::BitWriter bw(out);
    for (unsigned id = 0u ; id < m_cells.size() ; ++id) {
      const Cell& c = m_cells[id];
      for (unsigned d = 0u ; d < c.doors() ; ++d) {
        bw.push(c(d));
      }
    }

    bw.flush(false);

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
        prim::generate(*this);
        break;
      case Strategy::DepthFirst:
        depthfirst::generate(*this);
        break;
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
