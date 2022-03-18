#ifndef    MAZE_HH
# define   MAZE_HH

# include <memory>
# include <vector>
# include <core_utils/CoreObject.hh>
# include "Cell.hh"
# include "Opening.hh"

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
       *          The generation in and of itself uses a generic idea
       *          and some hooks are provided for inheriting mazes
       *          with different cell types to handle the specific
       *          part of the process.
       */
      void
      generate();

      /**
       * @brief - Whether or not this cell is inverted. This allows to
       *          handle the case where a cell (due to its number of
       *          sides) is not always in the same orientation in the
       *          maze. Typically triangle will have to be upside down
       *          once in a while to generate a triangular maze. This
       *          method determines whether the cell at the specified
       *          coordinates is inverted or not.
       *          In case the coordinate is not valid an error is raised.
       * @param x - the x coordinate of the cell.
       * @param y - the y coordinate of the cell.
       * @return - `true` if the cell is inverted.
       */
      virtual bool
      inverted(unsigned x, unsigned y) const = 0;

    protected:

      /**
       * @brief - A method to transform 2D coordinates to its linear
       *          version.
       */
      unsigned
      linear(unsigned x, unsigned y) const noexcept;

      /**
       * @brief - Used to generate the index of the move going in the
       *          opposite direction based on the geometry of the cells
       *          for this maze.
       * @param door - the door for which the opposite door should be
       *               returned.
       * @param inverted - Whether or not the cell to which the door
       *                   belongs is inverted (as defined in the
       *                   inverted method).
       * @return - the index of the opposite door.
       */
      virtual unsigned
      opposite(unsigned door, bool inverted) const noexcept = 0;

      /**
       * @brief - Interface method allowing to constrain an opening
       *          based on the dimensions of the cells. This is used
       *          whenever a cell is picked for the maze generation.
       * @param o - output argument describing the opening to modify.
       */
      virtual void
      prepareOpening(Opening& o) const noexcept = 0;

      /**
       * @brief - Interface method used to compute the index of the
       *          cell reached by going through the specified door
       *          for the input coordinates.
       *          The coordinates are assumed to be valid and the
       *          door *will* be valid based on the number of cells.
       * @param x - the x coordinate of the starting position.
       * @param y - the y coordinate of the starting position.
       * @param door - the door to go through.
       * @return - the index of the cell (as a linear value) that is
       *           reached when going through the door of the cell.
       */
      virtual unsigned
      idFromDoorAndCell(unsigned x, unsigned y, unsigned door) const = 0;

      /**
       * @brief - Interface method allowing to provide a human readable
       *          name for a door.
       * @param id - the index of the door in the cell.
       * @param inverted - whether the cell to which the door belongs to
       *                   is inverted.
       * @return - a string representing the name of the door.
       */
      virtual
      std::string
      doorName(unsigned id, bool inverted) const noexcept = 0;

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
