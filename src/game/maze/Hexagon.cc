
# include "Hexagon.hh"

namespace maze {

  HexagonMaze::HexagonMaze(unsigned width, unsigned height):
    Maze(width, height, 6u)
  {}

  unsigned
  HexagonMaze::opposite(unsigned door, bool /*inverted*/) const noexcept {
    // The opposite side is reached by adding 3 to the
    // door's index.
    return (door + 3u) % sides();
  }

  bool
  HexagonMaze::inverted(unsigned /*x*/, unsigned /*y*/) const {
    // A hexagon is never inverted.
    return false;
  }

  void
  HexagonMaze::prepareOpening(Opening& o) const noexcept {
    // Prevent opening of the left border of the maze.
    if (o.x() == 0u) {
      o.close(2u);
      o.close(3u);
    }
    // Prevent opening of the right border of the maze.
    if (o.x() == width() - 1u) {
      o.close(0u);
      o.close(5u);
    }
    // Prevent opening of the bottom border of the maze.
    if (o.y() == 0u) {
      // Hexagons with odd x coordinates will be slightly
      // lower which means more of their doors will have
      // to be closed.
      o.close(1u);
      if (o.x() % 2u == 0u) {
        o.close(0u);
        o.close(2u);
      }
    }
    // Prevent opening of the top border of the maze.
    if (o.y() == height() - 1u) {
      // Hexagons with even x coordinates will be slightly
      // lower which means more of their doors will have
      // to be closed.
      o.close(4u);
      if (o.x() % 2u == 1u) {
        o.close(3u);
        o.close(5u);
      }
    }
  }

  unsigned
  HexagonMaze::idFromDoorAndCell(unsigned x, unsigned y, unsigned door) const {
    unsigned id = linear(x, y);

    if (door == 0u) {
      // Opening the bottom right door. It points to the
      // cell immediately after the cell for odd hexagons
      // and almost one row before otherwise.
      if (x % 2u == 1u) {
        ++id;
      }
      else {
        id -= (width() - 1u);
      }
    }
    else if (door == 1u) {
      // Opening the bottom door, it points to the cell
      // one row before in the array.
      id -= width();
    }
    else if (door == 2u) {
      // Opening the bottom left door. It points to the
      // cell immediately before the cell for odd hexagons
      // and almost one row before otherwise.
      if (x % 2u == 1u) {
        --id;
      }
      else {
        id -= (width() + 1u);
      }
    }
    else if (door == 3u) {
      // Opening the top left door. It points to the cell
      // immediately before the cell for even hexagons and
      // almost one row after otherwise.
      if (x % 2u == 0u) {
        --id;
      }
      else {
        id += (width() - 1u);
      }
    }
    else if (door == 4u) {
      // Opening the top door, it points to the cell one
      // row after in the array.
      id += width();
    }
    else {
      // Opening the top right door. It points to the cell
      // immediately after the cell for even hexagons and
      // almost one row after otherwise.
      if (x % 2u == 0u) {
        ++id;
      }
      else {
        id += (width() + 1u);
      }
    }

    return id;
  }

}
