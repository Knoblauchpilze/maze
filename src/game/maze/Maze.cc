
# include "Maze.hh"
# include <numeric>

namespace maze {

  Maze::Maze(unsigned width,
             unsigned height,
             unsigned sides):
    utils::CoreObject("maze"),

    m_width(width),
    m_height(height),

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

    // Generation variables.
    unsigned size = width() * height();
    std::vector<unsigned> ids(size, 0u);

    std::iota(ids.begin(), ids.end(), 0);

    // Close all doors in the maze.
    close();

    // The algorithm used is taken from this article (in French):
    // https://fr.wikipedia.org/wiki/Mod%C3%A9lisation_math%C3%A9matique_d%27un_labyrinthe#Fusion_al%C3%A9atoire_de_chemins
    unsigned opened = 0u;
    unsigned walls = width() * height() - 1;

    while (opened < walls) {
      bool valid = true;

      // Find a wall to open.
      do {
        valid = true;
        // Pick a random cell.
        unsigned id1 = std::rand() % size;
        unsigned x1 = id1 % width();
        unsigned y1 = id1 / width();
        bool inv1 = inverted(x1, y1);

        Opening o(x1, y1, sides(), inv1);
        prepareOpening(o);

        // Pick a door to open.
        bool boxed = false;
        unsigned door = o.breach(boxed);

        if (boxed) {
          error("Cell " + std::to_string(x1) + "x" + std::to_string(y1) + " is boxes, can't open wall");

          valid = false;
          continue;
        }

        // Compute the cell on the other side of the door.
        unsigned id2 = idFromDoorAndCell(x1, y1, door);

        // Consistency check.
        if (id2 > size) {
          error(
            "Failed to generate maze",
            "Requested access to cell " + std::to_string(id2) + " when only " +
            std::to_string(size) + " are available"
          );
        }

        // Check if the cell already belongs to the same
        // region: if yes, we have to pick another wall.
        if(ids[id1] == ids[id2]) {
          valid = false;
          continue;
        }

        log(
          "Merging regions " + std::to_string(ids[id1]) +
          " to " + std::to_string(ids[id2]) +
          " after opening door " + doorName(door, inv1) +
          " in " + std::to_string(x1) + "x" + std::to_string(y1) +
          " to door " + doorName(opposite(door, inv1), inverted(id2 % width(), id2 / width())) +
          " in " + std::to_string(id2 % width()) + "x" + std::to_string(id2 / width()),
          utils::Level::Verbose
        );

        // Open the wall for both cells.
        m_cells[id1].toggle(door, true);
        m_cells[id2].toggle(opposite(door, inv1), true);

        // And equalize the identifiers for both regions.
        unsigned toReplace = std::max(ids[id1], ids[id2]);
        unsigned newArea = std::min(ids[id1], ids[id2]);

        for (unsigned id = 0u ; id < size ; ++id) {
          if (ids[id] == toReplace) {
            ids[id] = newArea;
          }
        }
      } while (!valid);

      // Opened one more wall.
      ++opened;
    }
  }

  unsigned
  Maze::linear(unsigned x, unsigned y) const noexcept {
    return y * m_width + x;
  }

}
