#ifndef    MAZE_DRAWER_HH
# define   MAZE_DRAWER_HH

# include <core_utils/CoreObject.hh>
# include "olcEngine.hh"
# include "CoordinateFrame.hh"
# include "Maze.hh"

namespace maze {

  class MazeDrawer: public utils::CoreObject {
    public:

      /**
       * @brief - Create a new cell drawer where cells will be assumed
       *          to have the input number of doors.
       * @param frame - the coordinate frame to use to convert cells to
       *                pixels.
       * @param maze - the maze defining the cells to be rendered.
       * @param pge - the pixel game engine allowing to render the cell.
       */
      MazeDrawer(const pge::CoordinateFrame& frame,
                 const Maze& maze,
                 olc::PixelGameEngine* pge);

      /**
       * @brief - Perform the drawing of the maze wrapperd by this
       *          object.
       */
      void
      draw() const noexcept;

    private:

      /**
       * @brief - Draw the cell with the specified index. Discard
       *          it in case it is not currently visible.
       * @param x - the x coordinate of the cell.
       * @param y - the y coordinate of the cell.
       */
      void
      drawCell(unsigned x, unsigned y) const noexcept;

      /**
       * @brief - Draw the door with the specified index for the
       *          input cell.
       * @param x - the x coordinate of the cell.
       * @param y - the y coordinate of the cell.
       * @param door - the index of the door to draw.
       */
      void
      drawDoor(unsigned x, unsigned y, unsigned door) const noexcept;

    private:

      /**
       * @brief - The coordinate frame to use to convert cells to
       *          pixels.
       */
      const pge::CoordinateFrame& m_frame;

      /**
       * @brief - The maze attached to the cells that will be rendered.
       */
      const Maze& m_maze;

      /**
       * @brief - The pixel game engine to use to perform the rendering.
       */
      olc::PixelGameEngine* m_pge;
  };

}

#endif    /* MAZE_DRAWER_HH */
