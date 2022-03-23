
# include "Generator.hh"
# include <numeric>
# include <unordered_set>
# include "Maze.hh"
# include "Opening.hh"

namespace maze {

  std::string
  strategyToString(const Strategy& st) noexcept {
    switch (st) {
      case Strategy::RandomizedKruskal:
        return "Randomized Kruskal";
      case Strategy::RandomizedPrim:
        return "Randomized Prim";
      case Strategy::AldousBroder:
        return "Aldous-Broder";
      default:
        return "unknown";
    }
  }

  namespace kruksal {

    /// @brief - Convenience structure defining a wall: this helps
    /// speeding up the process of picking a door.
    struct Door {
      // The x coordinate of the starting cell.
      unsigned x;

      // The y coordinate of the starting cell.
      unsigned y;

      // The index of the door.
      unsigned d;
    };

    void
    generate(Maze& m) {
      // Generation variables.
      unsigned size = m.width() * m.height();
      std::vector<unsigned> ids(size, 0u);
      std::vector<Door> doors;

      std::iota(ids.begin(), ids.end(), 0);

      // Prepare the list of doors to consider.
      for (unsigned id = 0u ; id < size ; ++id) {
        unsigned x = id % m.width();
        unsigned y = id / m.width();

        Opening o(x, y, m.sides(), m.inverted(x, y));
        m.prepareOpening(o);

        bool boxed = false;
        o.breach(boxed);
        if (boxed) {
          m.error("Cell " + std::to_string(x) + "x" + std::to_string(y) + " is boxed, can't open wall");
        }

        for (unsigned d = 0u ; d < m.sides() ; ++d) {
          if (!o.canBeOpened(d)) {
            continue;
          }

          doors.push_back(Door{x, y, d});
        }
      }

      // Close all doors in the maze.
      m.close();

      // The algorithm used is taken from this article (in French):
      // https://fr.wikipedia.org/wiki/Mod%C3%A9lisation_math%C3%A9matique_d%27un_labyrinthe#Fusion_al%C3%A9atoire_de_chemins
      unsigned opened = 0u;
      unsigned processed = 0u;
      unsigned walls = size - 1u;

      // As long as we didnt open enough walls.
      while (opened < walls) {
        bool valid = false;

        while (!valid) {
          valid = true;

          // Pick a random wall to open. Once done, move it to the
          // end of the list so that it is not considered anymore.
          unsigned id = std::rand() % (doors.size() - 1u - processed);
          Door door = doors[id];
          std::swap(doors[id], doors[doors.size() - 1u - processed]);
          ++processed;

          unsigned id1 = m.linear(door.x, door.y);
          unsigned inv1 = m.inverted(door.x, door.y);

          // Compute the cell on the other side of the door.
          unsigned id2 = m.idFromDoorAndCell(door.x, door.y, door.d);

          // Consistency check.
          if (id2 > size) {
            m.error(
              "Failed to generate maze",
              "Requested access to cell " + std::to_string(id2) + " when only " +
              std::to_string(size) + " are available"
            );
          }

          // Check if the cell already belongs to the same
          // region: if yes, we have to pick another wall.
          if(ids[id1] == ids[id2]) {
            valid = false;
            continue;
          }

          // Open the wall for both cells.
          m.m_cells[id1].toggle(door.d, true);
          m.m_cells[id2].toggle(m.opposite(door.d, inv1), true);

          // And equalize the identifiers for both regions.
          unsigned toReplace = std::max(ids[id1], ids[id2]);
          unsigned newArea = std::min(ids[id1], ids[id2]);

          for (unsigned id = 0u ; id < size ; ++id) {
            if (ids[id] == toReplace) {
              ids[id] = newArea;
            }
          }
        }

        // Opened one more wall.
        ++opened;
      }
    }

  }

  namespace prim {

    /// @brief - Convenience structure describing a wall.
    struct Wall {
      // The x coordinate of the starting cell.
      unsigned xO;

      // The y coordinate of the starting cell.
      unsigned yO;

      // The x coordinate of the destination cell.
      unsigned xE;

      // The y coordinate of the destination cell.
      unsigned yE;

      // The door connected both cells.
      unsigned door;
    };

    /// @brief - Convenience structure allowing to keep a list
    /// of walls and pick one of them at random while keeping
    /// a reasonable deletion time.
    struct Walls {
      /// @brief - A convenience define to reference a list of
      /// indices.
      using Indices = std::unordered_set<unsigned>;

      /// @brief- Iterating over the indices.
      using IndicesIt = Indices::const_iterator;

      // The walls' data.
      std::vector<Wall> data;

      // The status of elements that are used.
      Indices used;

      // The list of free indices.
      Indices free;

      /**
       * @brief - Push back an element in the walls' data.
       * @param w - the wall to push.
       */
      void
      push_back(const Wall& w) noexcept {
        // If no indices are free, create a new one.
        if (free.empty()) {
          used.insert(data.size());
          data.push_back(w);

          return;
        }

        // Otherwise, use a free index.
        unsigned id = *free.begin();
        free.erase(id);

        used.insert(id);
        data[id] = w;
      }

      /**
       * @brief - Whether there are no more walls in the structure.
       * @return - `true` if no more walls are defined.
       */
      bool
      empty() const noexcept {
        return data.size() == free.size();
      }

      /**
       * @brief - The current size of the list of walls.
       * @return - the size of the list walls.
       */
      std::size_t
      size() const noexcept {
        return used.size();
      }

      /**
       * @brief - Select a random wall from the ones registered.
       * @return - the randomly picked wall.
       */
      Wall
      pick() {
        // In case no element are available, this is a problem.
        if (empty()) {
          throw utils::CoreException("Unable to pick wall", "prim", "maze", "No wall left");
        }

        // Pick a random element among the used ones.
        unsigned id = std::rand() % used.size();

        // Find the corresponding index in the data structure.
        IndicesIt it = used.cbegin();
        unsigned c = 0u;
        while (c < id && it != used.cend()) {
          ++c;
          ++it;
        }

        if (c < id) {
          throw utils::CoreException(
            "Unable to pick wall",
            "prim",
            "maze",
            "Requested index " + std::to_string(id) + " but could only reach " + std::to_string(c)
          );
        }

        // Select the wall.
        Wall w = data[*it];

        // Mark the spot as free again.
        free.insert(*it);
        used.erase(it);

        return w;
      }
    };

    std::string
    hash(unsigned x, unsigned y) noexcept {
      return std::to_string(x) + "x" + std::to_string(y);
    }

    void
    generate(Maze& m) {
      // The algorithm is taken from here:
      // https://en.wikipedia.org/wiki/Maze_generation_algorithm#Randomized_Prim's_algorithm

      // We start with a grid full of walls.
      m.close();

      Walls walls;
      std::unordered_set<std::string> visited;

      // Convenience lambda to generate the neihbors.
      auto generateNeighbors = [&m, &visited, &walls](unsigned x, unsigned y) {
        Opening o(x, y, m.sides(), m.inverted(x, y));
        m.prepareOpening(o);

        for (unsigned d = 0u ; d < m.sides() ; ++d) {
          if (!o.canBeOpened(d)) {
            continue;
          }

          unsigned id = m.idFromDoorAndCell(x, y, d);
          unsigned xn = id % m.width();
          unsigned yn = id / m.width();

          // Prevent cells to be generated if they already exist.
          if (visited.count(hash(xn, yn)) > 0) {
            continue;
          }

          walls.push_back(Wall{x, y, xn, yn, d});
        }
      };

      // Pick a random cell and initialize the list of walls.
      unsigned size = m.width() * m.height();

      unsigned id = std::rand() % size;
      unsigned x1 = id % m.width();
      unsigned y1 = id / m.width();

      generateNeighbors(x1, y1);
      visited.insert(hash(x1, y1));

      // Continue processing while there are walls to analyze.
      while (!walls.empty()) {
        // Pick a random wall.
        Wall w = walls.pick();

        // Check whether the destination cell is visited.
        if (visited.count(hash(w.xE, w.yE)) > 0) {
          continue;
        }

        visited.insert(hash(w.xE, w.yE));

        // Open the wall for both cells.
        bool inv1 = m.inverted(w.xO, w.yO);
        m.m_cells[m.linear(w.xO, w.yO)].toggle(w.door, true);
        m.m_cells[m.linear(w.xE, w.yE)].toggle(m.opposite(w.door, inv1), true);

        // Generate the neighbors of the cell.
        generateNeighbors(w.xE, w.yE);
      }
    }

  }

  namespace aldousbroder {

    void
    generate(Maze& m) {
      /// TODO: Handle generation with Aldous-Broder.
      m.warn("Should handle generation of maze with Aldous-Broder");
    }

  }

}
