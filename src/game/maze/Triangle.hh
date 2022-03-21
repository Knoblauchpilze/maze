#ifndef    TRIANGLE_HH
# define   TRIANGLE_HH

# include "Maze.hh"

namespace maze {

  class TriangleMaze : public Maze {
    public:

      /**
       * @brief - Build a new maze with triangle cells with the
       *          input dimensions.
       * @param width - the width of the maze.
       * @param height - the height of the maze.
       * @param strategy - the strategy to generate the maze.
       */
      TriangleMaze(unsigned width, unsigned height, const Strategy& strategy);

      /**
       * @brief - Implementation of the interface method to handle
       *          whether a cell is inverted.
       * @param x - the x coordinate of the cell.
       * @param y - the y coordinate of the cell.
       * @return - `true` if the cell is inverted.
       */
      bool
      inverted(unsigned x, unsigned y) const override;

    protected:

      /**
       * @brief - Implementation of the interface method to handle
       *          the opposite doors for a triangular maze.
       * @param door - the door for which the opposite door should be
       *               returned.
       * @param inverted - Whether or not the cell to which the door
       *                   belongs is inverted (as defined in the
       *                   inverted method).
       * @return - the index of the opposite door.
       */
      unsigned
      opposite(unsigned door, bool inverted) const noexcept override;

      /**
       * @brief - Implementation of the interface method to handle
       *          the adding of constraints to an opening based on
       *          the number of sides of each cell.
       * @param o - output argument describing the opening to modify.
       */
      void
      prepareOpening(Opening& o) const noexcept override;

      /**
       * @brief - Implementation of the interface method to determine
       *          the index of the cell reached by going through the
       *          door of the input coordinates.
       * @param x - the x coordinate of the starting position.
       * @param y - the y coordinate of the starting position.
       * @param door - the door to go through.
       * @return - the index of the cell (as a linear value) that is
       *           reached when going through the door of the cell.
       */
      unsigned
      idFromDoorAndCell(unsigned x, unsigned y, unsigned door) const override;

      /**
       * @brief - Implementation of the interface method to name a door.
       * @param id - the index of the door in the cell.
       * @param inverted - whether the cell of the door is inverted.
       * @return - a string representing the name of the door.
       */
      std::string
      doorName(unsigned id, bool inverted) const noexcept override;
  };

}

#endif    /* TRIANGLE_HH */
