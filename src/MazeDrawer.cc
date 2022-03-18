
# include "MazeDrawer.hh"

namespace triangle {

  bool
  visible(unsigned x,
          unsigned y,
          const pge::CoordinateFrame& frame) noexcept
  {
    // The triangles are 'squashed' as there's more than
    // one triangle per cell.
    return frame.cellsViewport().visible(olc::vf2d(0.5f * x, y), olc::vf2d(1.0f, 1.0f));
  }

  bool
  border(float x,
         float y,
         unsigned door,
         bool /*even*/,
         bool inverted,
         const pge::CoordinateFrame& frame,
         olc::vf2d& p1,
         olc::vf2d& p2) noexcept
  {
    // The cells are 'squashed' together as we can put
    // more than one hexagon per cell. We have to use
    // a compression compared to the canonical index.
    p1 = olc::vf2d(0.5f * x, y);
    p2 = olc::vf2d(0.5f * x, y);

    bool valid = true;
    switch (door) {
      case 0u:
        if (inverted) {
          // Right door.
          p1.x += 1.0f;

          p2.x += 0.5f;
          p2.y += 1.0f;
        }
        else {
          // Right door.
          p1.x += 0.5f;

          p2.x += 1.0f;
          p2.y += 1.0f;
        }
        break;
      case 1u:
        if (inverted) {
          // Left door.
          p2.x += 0.5f;
          p2.y += 1.0f;
        }
        else {
          // Bottom door.
          p1.y += 1.0f;

          p2.x += 1.0f;
          p2.y += 1.0f;
        }
        break;
      case 2u:
        if (inverted) {
          // Top door.
          p2.x += 1.0f;
        }
        else {
          // Left door.
          p1.x += 0.5f;

          p2.y += 1.0f;
        }
        break;
      default:
        valid = false;
        break;
      }

    // In case of an invalid door do not draw anything
    if (!valid) {
      return false;
    }

    p1 = frame.tileCoordsToPixels(p1.x, p1.y, pge::RelativePosition::Center, 1.0f);
    p2 = frame.tileCoordsToPixels(p2.x, p2.y, pge::RelativePosition::Center, 1.0f);

    return true;
  }

  bool
  adjustBorder(unsigned /*x*/,
               unsigned /*y*/,
               unsigned door,
               bool inverted,
               olc::vf2d p1,
               olc::vf2d p2)
  {
    // Adjust the position of a few pixels to make the
    // line representing the door visible.
    bool valid = true;

    switch (door) {
      case 0u:
        if (inverted) {
          // Right door.
          p1.x -= 1.0f;

          p2.x -= 1.0f;
          p2.y -= 1.0f;
        }
        else {
          // Right door.
          p2.x -= 1.0f;
          p2.y -= 1.0f;
        }
        break;
      case 1u:
        if (inverted) {
          // Left door.
          p2.y -= 1.0f;
        }
        else {
          // Bottom door.
          p1.y -= 1.0f;

          p2.x -= 1.0f;
          p2.y -= 1.0f;
        }
        break;
      case 2u:
        if (inverted) {
          // Top door.
          p2.x -= 1.0f;
        }
        else {
          // Left door.
          p2.y -= 1.0f;
        }
        break;
      default:
        valid = false;
        break;
    }

    return valid;
  }

  void
  renderBorder(olc::vf2d p1,
               olc::vf2d p2,
               olc::PixelGameEngine* pge,
               olc::Pixel color)
  {
    // Draw the line with the specified color.
    pge->DrawLine(p1, p2, color);
  }

  void
  draw(unsigned x,
       unsigned y,
       bool inverted,
       const pge::CoordinateFrame& frame,
       olc::PixelGameEngine* pge,
       const olc::Pixel& color)
  {
    olc::vf2d p;
    // The cells are 'squashed' together as we can put
    // more than one triangle per cell. We have to use
    // a compression compared to the canonical index.
    p.x = 0.5f * x;
    p.y = y;

    // Determine the position of the three edges of the
    // triangle. We have to consider inverted triangles.
    olc::vf2d p1, p2, p3;
    if (inverted) {
      // The triangle is upside down.
      p1.x = p.x;
      p1.y = p.y;

      p2.x = p.x + 1.0f;
      p2.y = p.y;

      p3.x = p.x + 0.5f;
      p3.y = p.y + 1.0f;
    }
    else {
      // The triangle is standing.
      p1.x = p.x;
      p1.y = p.y + 1.0f;

      p2.x = p.x + 1.0f;
      p2.y = p.y + 1.0f;

      p3.x = p.x + 0.5f;
      p3.y = p.y;
    }

    // As we're drawing a non-squarish shape we have to
    // use the low-level drawing primitives.
    olc::vf2d tp1 = frame.tileCoordsToPixels(p1.x, p1.y, pge::RelativePosition::Center, 1.0f);
    olc::vf2d tp2 = frame.tileCoordsToPixels(p2.x, p2.y, pge::RelativePosition::Center, 1.0f);
    olc::vf2d tp3 = frame.tileCoordsToPixels(p3.x, p3.y, pge::RelativePosition::Center, 1.0f);

    pge->FillTriangle(tp1, tp2, tp3, color);
  }

}

namespace square {

  bool
  visible(unsigned x,
          unsigned y,
          const pge::CoordinateFrame& frame) noexcept
  {
    return frame.cellsViewport().visible(olc::vf2d(x, y), olc::vf2d(1.0f, 1.0f));
  }

  bool
  border(float x,
         float y,
         unsigned door,
         bool /*even*/,
         bool /*inverted*/,
         const pge::CoordinateFrame& frame,
         // Note that this will correspond to the position of
         // the square to draw.
         olc::vf2d& p1,
         // Note that this will correspond to the size of the
         // border to draw.
         olc::vf2d& p2) noexcept
  {
    // Note that as we want to draw a rectangle and
    // not a square, we have to rely to the low-level
    // drawing primitives.
    float dx = x;
    float dy = y;

    // The dimensions of the doors in cells.
    float doorPerc = 0.05f;

    p2 = frame.tileSize();

    bool valid = true;
    switch (door) {
      case 0u:
        // Right door.
        dx += (1.0f - doorPerc);
        p2.x *= doorPerc;
        break;
      case 1u:
        // Bottom door.
        dy += (1.0f - doorPerc);
        p2.y *= doorPerc;
        break;
      case 2u:
        // Left door.
        p2.x *= doorPerc;
        break;
      case 3u:
        // Top door.
        p2.y *= doorPerc;
        break;
      default:
        valid = false;
        break;
    }

    // In case of an invalid door do not draw anything
    if (!valid) {
      return false;
    }

    // Clamp the size of a door to always be at least one
    // pixel wide so that it is visible.
    p2.x = std::max(p2.x, 1.0f);
    p2.y = std::max(p2.y, 1.0f);

    p1 = frame.tileCoordsToPixels(dx, dy, pge::RelativePosition::Center, 1.0f);

    return true;
  }

  void
  renderBorder(olc::vf2d p1,
               olc::vf2d p2,
               olc::PixelGameEngine* pge,
               olc::Pixel color)
  {
    // Draw the border as a rectangle with the specified
    // color.
    pge->FillRectDecal(p1, p2, color);
  }

  void
  draw(unsigned x,
       unsigned y,
       bool /*inverted*/,
       const pge::CoordinateFrame& frame,
       olc::PixelGameEngine* pge,
       const olc::Pixel& color)
  {
    // A sqaure can be rendered very efficiently.
    olc::vf2d p = frame.tileCoordsToPixels(x, y, pge::RelativePosition::Center, 1.0f);
    pge->FillRectDecal(p, frame.tileSize(), color);
  }

}

namespace hexagon {

  bool
  visible(unsigned x,
          unsigned y,
          const pge::CoordinateFrame& frame) noexcept
  {
    return frame.cellsViewport().visible(olc::vf2d(x, y), olc::vf2d(1.0f, 1.0f));
  }

  bool
  border(float x,
         float y,
         unsigned door,
         bool even,
         bool /*inverted*/,
         const pge::CoordinateFrame& frame,
         olc::vf2d& p1,
         olc::vf2d& p2) noexcept
  {
    p1 = olc::vf2d(x, y);
    p2 = olc::vf2d(x, y);

    bool valid = true;
    switch (door) {
      case 0u:
        if (even) {
          p1.x += 1.333f;
          p1.y += 0.5f;

          p2.x += 1.0f;
          p2.y += 1.0f;
        }
        else {
          p1.x += 1.333f;

          p2.x += 1.0f;
          p2.y += 0.5f;
        }
        break;
      case 1u:
        if (even) {
          p1.x += 0.333f;
          p1.y += 1.0f;

          p2.x += 1.0f;
          p2.y += 1.0f;
        }
        else {
          p1.x += 0.333f;
          p1.y += 0.5f;

          p2.x += 1.0f;
          p2.y += 0.5f;
        }
        break;
      case 2u:
        if (even) {
          p1.y += 0.5f;

          p2.x += 0.333f;
          p2.y += 1.0f;
        }
        else {
          p2.x += 0.333f;
          p2.y += 0.5f;
        }
        break;
      case 3u:
        if (even) {
          p1.y += 0.5f;

          p2.x += 0.333f;
        }
        else {
          p2.x += 0.333f;
          p2.y -= 0.5f;
        }
        break;
      case 4u:
        if (even) {
          p1.x += 0.333f;

          p2.x += 1.0f;
        }
        else {
          p1.x += 0.333f;
          p1.y -= 0.5f;

          p2.x += 1.0f;
          p2.y -= 0.5f;
        }
        break;
      case 5u:
        if (even) {
          p1.x += 1.0f;

          p2.x += 1.333f;
          p2.y += 0.5f;
        }
        else {
          p1.x += 1.0f;
          p1.y -= 0.5f;

          p2.x += 1.333f;
        }
        break;
      default:
        valid = false;
        break;
    }

    // In case of an invalid door do not draw anything
    if (!valid) {
      return false;
    }

    p1 = frame.tileCoordsToPixels(p1.x, p1.y, pge::RelativePosition::Center, 1.0f);
    p2 = frame.tileCoordsToPixels(p2.x, p2.y, pge::RelativePosition::Center, 1.0f);

    return true;
  }

  bool
  adjustBorder(unsigned x,
               unsigned /*y*/,
               unsigned door,
               bool /*inverted*/,
               olc::vf2d p1,
               olc::vf2d p2)
  {
    // Adjust the position of a few pixels to make the
    // line representing the door visible.
    bool valid = true;

    bool even = (x % 2u == 0u);

    switch (door) {
      case 0u:
        if (!even) {
          p1.x -= 1.0f;
          p2.x -= 1.0f;
        }
        break;
      case 1u:
        break;
      case 2u:
        break;
      case 3u:
        if (!even) {
          p1.x += 1.0f;
          p2.x += 1.0f;
        }
        break;
      case 4u:
        p1.y += 1.0f;
        p2.y += 1.0f;
        break;
      case 5u:
        if (even) {
          p1.x -= 1.0f;
          p2.x -= 1.0f;
        }
        else {
          p1.x -= 1.0f;
          p2.x -= 1.0f;
        }
        break;
      default:
        valid = false;
        break;
    }

    return valid;
  }

  void
  renderBorder(olc::vf2d p1,
               olc::vf2d p2,
               olc::PixelGameEngine* pge,
               olc::Pixel color)
  {
    // Draw the line with the specified color.
    pge->DrawLine(p1, p2, color);
  }

  void
  draw(unsigned x,
       unsigned y,
       bool /*inverted*/,
       const pge::CoordinateFrame& frame,
       olc::PixelGameEngine* pge,
       const olc::Pixel& color)
  {
    olc::vf2d p;
    // The cells are 'squashed' together as we can put
    // more than one hexagon per cell. We have to use
    // a compression compared to the canonical index.
    p.x = x;
    p.y = y;

    // Determine the position of the six vertices of the
    // hexagon.
    olc::vf2d p1, p2, p3, p4, p5, p6;
    if (x % 2u == 0u) {
      p1.x = p.x + 0.333f;
      p1.y = p.y;

      p2.x = p.x + 1.0f;
      p2.y = p.y;

      p3.x = p.x + 1.333f;
      p3.y = p.y + 0.5f;

      p4.x = p.x + 1.0f;
      p4.y = p.y + 1.0f;

      p5.x = p.x + 0.333f;
      p5.y = p.y + 1.0f;

      p6.x = p.x;
      p6.y = p.y + 0.5f;
    }
    else {
      p1.x = p.x + 0.333f;
      p1.y = p.y - 0.5f;

      p2.x = p.x + 1.0f;
      p2.y = p.y - 0.5f;

      p3.x = p.x + 1.333f;
      p3.y = p.y;

      p4.x = p.x + 1.0f;
      p4.y = p.y + 0.5f;

      p5.x = p.x + 0.333f;
      p5.y = p.y + 0.5f;

      p6 = p;
    }

    // As we're drawing a non-squarish shape we have to
    // use the low-level drawing primitives.
    olc::vf2d tp1 = frame.tileCoordsToPixels(p1.x, p1.y, pge::RelativePosition::Center, 1.0f);
    olc::vf2d tp2 = frame.tileCoordsToPixels(p2.x, p2.y, pge::RelativePosition::Center, 1.0f);
    olc::vf2d tp3 = frame.tileCoordsToPixels(p3.x, p3.y, pge::RelativePosition::Center, 1.0f);
    olc::vf2d tp4 = frame.tileCoordsToPixels(p4.x, p4.y, pge::RelativePosition::Center, 1.0f);
    olc::vf2d tp5 = frame.tileCoordsToPixels(p5.x, p5.y, pge::RelativePosition::Center, 1.0f);
    olc::vf2d tp6 = frame.tileCoordsToPixels(p6.x, p6.y, pge::RelativePosition::Center, 1.0f);

    pge->FillTriangle(tp1, tp5, tp6, color);
    pge->FillTriangle(tp1, tp2, tp5, color);
    pge->FillTriangle(tp2, tp4, tp5, color);
    pge->FillTriangle(tp2, tp3, tp4, color);
  }

}

namespace maze {

  MazeDrawer::MazeDrawer(const pge::CoordinateFrame& frame,
                         const Maze& maze,
                         olc::PixelGameEngine* pge):
    utils::CoreObject("drawer"),

    m_frame(frame),
    m_maze(maze),
    m_pge(pge)
  {
    setService("maze");
  }

  void
  MazeDrawer::draw() const noexcept {
    // We assume the cell `0, 0` of the maze has the same
    // world coordinates.
    for (unsigned y = 0u ; y < m_maze.height() ; ++y) {
      for (unsigned x = 0u ; x < m_maze.width() ; ++x) {
        drawCell(x, y);
      }
    }
  }

  void
  MazeDrawer::drawCell(unsigned x, unsigned y) const noexcept {
    // The color of the cell.
    olc::Pixel cell(13, 2, 8);

    // The maze is drawn upside down.
    unsigned dy = m_maze.height() - 1u - y;

    olc::vf2d p;
    p.x = x;
    p.y = dy;

    // Draw the cell.
    switch (m_maze.sides()) {
      case 3u:
        if (triangle::visible(x, dy, m_frame)) {
          triangle::draw(x, dy, m_maze.inverted(x, y), m_frame, m_pge, cell);
        }
        break;
      case 4u:
        if (square::visible(x, dy, m_frame)) {
          square::draw(x, dy, m_maze.inverted(x, y), m_frame, m_pge, cell);
        }
        break;
      case 6u:
        if (hexagon::visible(x, dy, m_frame)) {
          hexagon::draw(x, dy, m_maze.inverted(x, y), m_frame, m_pge, cell);
        }
        break;
      default:
        warn(
          "Failed to draw cell " + std::to_string(x) + "x" + std::to_string(y),
          "Cell having " + std::to_string(m_maze.sides()) + " doors is not a supported"
        );
        return;
    }

    // And draw each door.
    const maze::Cell& c = m_maze.at(x, y);
    for (unsigned id = 0u ; id < c.doors() ; ++id) {
      if (!c(id)) {
        drawDoor(x, y, id);
      }
    }
  }

  void
  MazeDrawer::drawDoor(unsigned x, unsigned y, unsigned door) const noexcept {
    // Generate the two endpoints of the border and draw the
    // corresponding line.
    olc::Pixel border(0, 143, 17);

    olc::vf2d p1, p2;
    bool even = (x % 2u == 0u);
    // The maze is drawn upside down.
    unsigned dy = m_maze.height() - 1u - y;

    bool valid = true;
    switch (m_maze.sides()) {
      case 3u:
        valid = triangle::border(x, dy, door, even, m_maze.inverted(x, y), m_frame, p1, p2);
        valid = triangle::adjustBorder(x, dy, door, m_maze.inverted(x, y), p1, p2);
        if (valid) {
          triangle::renderBorder(p1, p2, m_pge, border);
        }
        break;
      case 4u:
        valid = square::border(x, dy, door, even, m_maze.inverted(x, y), m_frame, p1, p2);
        if (valid) {
          square::renderBorder(p1, p2, m_pge, border);
        }
        break;
      case 6u:
        valid = hexagon::border(x, dy, door, even, m_maze.inverted(x, y), m_frame, p1, p2);
        valid = hexagon::adjustBorder(x, dy, door, m_maze.inverted(x, y), p1, p2);
        if (valid) {
          hexagon::renderBorder(p1, p2, m_pge, border);
        }
        break;
      default:
        valid = false;
        break;
    }

    if (!valid) {
      warn(
        "Failed to render border at " + std::to_string(x) + "x" + std::to_string(y),
        "Unsupported number of doors (" + std::to_string(m_maze.sides())
      );
    }
  }

}
