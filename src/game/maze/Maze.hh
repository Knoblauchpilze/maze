#ifndef    MAZE_HH
# define   MAZE_HH

# include <memory>
# include <vector>
# include <core_utils/CoreObject.hh>
# include "Cell.hh"

namespace maze {

  class Maze: public utils::CoreObject {
    public:

      /**
       * @brief - Create a new maze with the specified dimensions.
       * @param width - the width of the maze in cells.
       * @param height - the height of the maze in cells.
       * @param sides - the number of sides of the cells for this
       *                maze.
       */
      Maze(unsigned width,
           unsigned height,
           unsigned sides = 4u);

      /**
       * @brief - The width of this maze in cells.
       * @return - the width of the maze.
       */
      unsigned
      width() const noexcept;

      /**
       * @brief - The height of this maze in cells.
       * @return - the height of the maze.
       */
      unsigned
      height() const noexcept;

      /**
       * @brief - The number of sides each cell of the maze has.
       * @return - the number of sides of each cell.
       */
      unsigned
      sides() const noexcept;

      /**
       * @brief - Return the cell at the specified index. In case
       *          the coordinates are not valid an error is raised.
       * @param x - the x coordinate of the cell to fetch.
       * @param y - the y coordinate of the cell to fetch.
       * @return - the cell at the specified coordinates.
       */
      const Cell&
      at(unsigned x, unsigned y) const;

      /**
       * @brief - Constant version of accessing a cell.
       * @param x - the x coordinate of the cell to fetch.
       * @param y - the y coordinate of the cell to fetch.
       * @return - the cell at the specified coordinates.
       */
      Cell
      at(unsigned x, unsigned y);

      /**
       * @brief - Opens all the doors of all the cells defined in
       *          the maze.
       */
      void
      open();

      /**
       * @brief - Close all the doors of all the cells defined in
       *          the maze.
       */
      void
      close();

      /**
       * @brief - Interface method allowing to generate a new maze.
       */
      virtual void
      generate() = 0;

      /**
       * @brief - Used to generate the index of the move going in the
       *          opposite direction based on the geometry of the cells
       *          for this maze.
       * @param door - the door for which the opposite door should be
       *               returned.
       * @param inverted - TODO: Handle this.
       * @return - the index of the opposite door.
       */
      virtual unsigned
      opposite(unsigned door, bool inverted) const noexcept = 0;

      /**
       * @brief - Whether or not this cell is inverted. TODO: Refine.
       * @param door - the index of the door to return.
       * @return - `true` if the door is inverted.
       */
      virtual bool
      inverted(unsigned door) const noexcept = 0;

    private:

      /**
       * @brief - A method to transform 2D coordinates to its linear
       *          version.
       */
      unsigned
      linear(unsigned x, unsigned y) const noexcept;

    protected:

      /**
       * @brief - The width of the maze in cells.
       */
      unsigned m_width;

      /**
       * @brief - The height of the maze in cells.
       */
      unsigned m_height;

      /**
       * @brief - The number of sides on each cell.s
       */
      unsigned m_cellSides;

      /**
       * @brief - The cells for this maze.
       */
      std::vector<Cell> m_cells;
  };

  using MazeShPtr = std::shared_ptr<Maze>;
}

#endif    /* MAZE_HH */
