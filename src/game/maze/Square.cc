
# include "Square.hh"

namespace maze {

  SquareMaze::SquareMaze(unsigned width, unsigned height):
    Maze(width, height, 4u)
  {}

  unsigned
  SquareMaze::opposite(unsigned door, bool /*inverted*/) const noexcept {
    // A drawing helps as well, but it's pretty simple.
    return (door + 2u) % sides();
  }

  bool
  SquareMaze::inverted(unsigned /*x*/, unsigned /*y*/) const {
    // A square is never inverted.
    return false;
  }

  void
  SquareMaze::prepareOpening(Opening& o) const noexcept {
    // Prevent opening of the left border of the maze.
    if (o.x() == 0u) {
      o.close(2u);
    }
    // Prevent opening of the right border of the maze.
    if (o.x() == width() - 1u) {
        o.close(0u);
    }
    // Prevent opening of the bottom border of the maze.
    if (o.y() == 0u) {
      o.close(1u);
    }
    // Prevent opening of the top border of the maze.
    if (o.y() == height() - 1u) {
      o.close(3u);
    }
  }

  unsigned
  SquareMaze::idFromDoorAndCell(unsigned x, unsigned y, unsigned door) const {
    unsigned id = linear(x, y);

    if(door == 0) {
      // Opening the right door, it always points to the
      // cell right after the initial one.
      ++id;
    }
    else if (door == 1u) {
      // Opening the bottom door, it points to the cell
      // one row before in the array.
      id -= width();
    }
    else if (door == 2u) {
      // Opening the left door, it always points to the
      // cell right before the initial one.
      --id;
    }
    else {
      // Opening the top door, it points to the cell one
      // row after in the array.
      id += width();
    }

    return id;
  }

  std::string
  SquareMaze::doorName(unsigned id, bool /*inverted*/) const noexcept {
    switch (id) {
      case 0u:
        return "right";
      case 1u:
        return "bottom";
      case 2u:
        return "left";
      case 3u:
        return "top";
      default:
        return "unknown";
    }
  }

}
