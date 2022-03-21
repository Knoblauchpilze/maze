
# include "Generator.hh"
# include <numeric>
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

    void
    generate(Maze& m) {
      // Generation variables.
      unsigned size = m.width() * m.height();
      std::vector<unsigned> ids(size, 0u);

      std::iota(ids.begin(), ids.end(), 0);

      // Close all doors in the maze.
      m.close();

      // The algorithm used is taken from this article (in French):
      // https://fr.wikipedia.org/wiki/Mod%C3%A9lisation_math%C3%A9matique_d%27un_labyrinthe#Fusion_al%C3%A9atoire_de_chemins
      unsigned opened = 0u;
      unsigned walls = size - 1u;

      while (opened < walls) {
        bool valid = true;

        // Find a wall to open.
        do {
          valid = true;
          // Pick a random cell.
          unsigned id1 = std::rand() % size;
          unsigned x1 = id1 % m.width();
          unsigned y1 = id1 / m.width();
          bool inv1 = m.inverted(x1, y1);

          Opening o(x1, y1, m.sides(), inv1);
          m.prepareOpening(o);

          // Pick a door to open.
          bool boxed = false;
          unsigned door = o.breach(boxed);

          if (boxed) {
            m.error("Cell " + std::to_string(x1) + "x" + std::to_string(y1) + " is boxes, can't open wall");

            valid = false;
            continue;
          }

          // Compute the cell on the other side of the door.
          unsigned id2 = m.idFromDoorAndCell(x1, y1, door);

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

          m.log(
            "Merging regions " + std::to_string(ids[id1]) +
            " to " + std::to_string(ids[id2]) +
            " after opening door " + m.doorName(door, inv1) +
            " in " + std::to_string(x1) + "x" + std::to_string(y1) +
            " to door " + m.doorName(m.opposite(door, inv1), m.inverted(id2 % m.width(), id2 / m.width())) +
            " in " + std::to_string(id2 % m.width()) + "x" + std::to_string(id2 / m.width()),
            utils::Level::Verbose
          );

          // Open the wall for both cells.
          m.m_cells[id1].toggle(door, true);
          m.m_cells[id2].toggle(m.opposite(door, inv1), true);

          // And equalize the identifiers for both regions.
          unsigned toReplace = std::max(ids[id1], ids[id2]);
          unsigned newArea = std::min(ids[id1], ids[id2]);

          for (unsigned id = 0u ; id < size ; ++id) {
            if (ids[id] == toReplace) {
              ids[id] = newArea;
            }
          }
        } while (!valid);

        // Opened one more wall.
        ++opened;
      }
    }

  }

  namespace prim {

    void
    generate(Maze& m) {
      /// TODO: Handle generation with randomized Prim.
      m.warn("Should handle generation of maze with randomized Prim");
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
