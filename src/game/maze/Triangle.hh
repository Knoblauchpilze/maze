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
       */
      TriangleMaze(unsigned width, unsigned height);

      /**
       * @brief - Implementation of the interface method to create
       *          a new maze.
       */
      void
      generate() override;

      /**
       * @brief - Implementation of the interface method to handle
       *          the opposite doors for a triangular maze.
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

#endif    /* TRIANGLE_HH */
