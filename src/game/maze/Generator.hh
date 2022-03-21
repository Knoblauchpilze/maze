#ifndef    GENERATOR_HH
# define   GENERATOR_HH

# include <string>
# include <vector>
# include "Cell.hh"

namespace maze {

  /// @brief - The strategies available to generate a maze. For more
  /// information about the strategies, see here:
  /// https://en.wikipedia.org/wiki/Maze_generation_algorithm
  enum class Strategy {
    RandomizedKruskal,
    RandomizedPrim,
    AldousBroder
  };

  /**
   * @brief - Convert a strategy to a human readable string.
   * @param st - the strategy to translate to a string.
   * @return - the name of the strategy.
   */
  std::string
  strategyToString(const Strategy& st) noexcept;

  /// @brief - Convenience define for a list of cells.
  using Cells = std::vector<Cell>;

  /// @brief - Forward declaration of the maze class.
  class Maze;

  namespace kruksal {

    /**
     * @brief - Generate a maze with a random Kruksal algorithm.
     * @param m - the maze: used to query properties about cells.
     */
    void
    generate(Maze& m);

  }

  namespace prim {

    /**
     * @brief - Generate a maze with a randomized Prim algorithm.
     * @param m - the maze: used to query properties about cells.
     */
    void
    generate(Maze& m);

  }

  namespace aldousbroder {

    /**
     * @brief - Generate a maze with the Aldous-Broder algorithm.
     * @param m - the maze: used to query properties about cells.
     */
    void
    generate(Maze& m);

  }
}

#endif    /* GENERATOR_HH */
