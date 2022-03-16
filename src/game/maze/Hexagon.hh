#ifndef    HEXAGON_HH
# define   HEXAGON_HH

# include "Maze.hh"

namespace maze {

  class HexagonMaze : public Maze {
    public:

      /**
       * @brief - Build a new maze with hexagonal cells with
       *          the input dimensions.
       * @param width - the width of the maze.
       * @param height - the height of the maze.
       */
      HexagonMaze(unsigned width, unsigned height);

      /**
       * @brief - Implementation of the interface method to create
       *          a new maze.
       */
      void
      generate() override;

      /**
       * @brief - Implementation of the interface method to handle
       *          the opposite doors for a hexagonal maze.
       * @param door - the door for which the opposite door should be
       *               returned.
       * @param inverted - TODO: Handle this.
       * @return - the index of the opposite door.
       */
      unsigned
      opposite(unsigned door, bool inverted) const noexcept override;

      /**
       * @brief - Implementation of the interface method to handle
       *          whether a door is inverted.
       * @param door - the index of the door to return.
       * @return - `true` if the door is inverted.
       */
      bool
      inverted(unsigned door) const noexcept override;
  };

}

#endif    /* HEXAGON_HH */
