
# include "Triangle.hh"

namespace maze {

  TriangleMaze::TriangleMaze(unsigned width, unsigned height):
    Maze(width, height, 3u)
  {}

  unsigned
  TriangleMaze::opposite(unsigned door, bool inverted) const noexcept {
    // The opposite move is different in case we have an
    // inverted cell or not.
    // Drawing the situation helps here.
    if (inverted) {
      return (door + sides() - 1u) % sides();
    }

    return (door + 1u) % sides();
  }

  bool
  TriangleMaze::inverted(unsigned x, unsigned y) const {
    // Invalid cell coordinates.
    if (x >= width() || y >= height()) {
      error(
        "Failed to determine inverted status of " + std::to_string(x) + "x" + std::to_string(y),
        "Maze has a size of " + std::to_string(width()) + "x" + std::to_string(height())
      );
    }

    // In a triangle, we start the first row by an inverted
    // triangle, and then pursue with a regular one. The row
    // on top is then starting with a regular triangle, and
    // then an inverted one. And so on.
    if (y % 2u == 0u) {
      // In an even row every even triangle is inverted.
      return (x % 2u == 0u);
    }

    // In an odd row every odd triangle is inverted.
    return (x % 2u == 1u);
  }

  void
  TriangleMaze::prepareOpening(Opening& o) const noexcept {
    // Prevent opening of the left border of the maze.
    if (o.x() == 0u) {
      o.close(o.y() == 0u ? 1u : 2u);
    }
    // Prevent opening of the right border of the maze.
    if (o.x() == width() - 1u) {
      o.close(0u);
    }
    // Prevent opening of the bottom border of the maze.
    // Note that only the non inverted triangles will be
    // affected by an opening that can't be performed.
    if (o.y() == 0u && !o.inverted()) {
      o.close(1u);
    }
    // Prevent opening of the top border of the maze.
    // Note that only the inverted triangles will be
    // affected by an opening that can't be performed.
    if (o.y() == height() - 1u && o.inverted()) {
      o.close(2u);
    }
  }

  unsigned
  TriangleMaze::idFromDoorAndCell(unsigned x, unsigned y, unsigned door) const {
    unsigned id = linear(x, y);

    if (door == 0u) {
      // Opening the right door, it always points to the
      // cell right after the initial one.
      ++id;
    }
    else if (door == 1u) {
      // Opening either the left door (if the triangle is
      // inverted) or the bottom door (if the triangle is
      // not inverted).
      if (inverted(x, y)) {
        --id;
      }
      else {
        id -= width();
      }
    }
    else {
      // Opening either the left door (if the triangle is
      // not inverted) or the top door (if the triangle is
      // inverted).
      if (inverted(x, y)) {
        id += width();
      }
      else {
        --id;
      }
    }

    return id;
  }

}
