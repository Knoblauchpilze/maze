#ifndef    CELL_HH
# define   CELL_HH

# include <vector>
# include <memory>
# include <core_utils/CoreObject.hh>

namespace maze {

  class Cell: public utils::CoreObject {
    public:

      /**
       * @brief - Generate a new cell with the specified number of
       *          doors.
       * @param doors - the numebr of doors for this cell.
       * @param open - whether or not the cell is open at the start.
       */
      Cell(unsigned doors, bool open = false);

      /**
       * @brief - The number of doors for this cell.
       * @return - the number of doors for this cell.
       */
      unsigned
      doors() const noexcept;

      /**
       * @brief - Return whether the door at the input index is
       *          opened or not. In case the door is out of the
       *          bounds for the cell an error is raised.
       *          NOTE: the index starts at `0` and will be set
       *          to the right door (or the cloest approaching
       *          direction), and then clockwise listing all the
       *          doors.
       * @param id - the index of the door.
       * @return - `true` if the cell is opened.
       */
      bool
      operator()(unsigned id) const;

      /**
       * @brief - Opens all the doors of this cell.
       */
      void
      open();

      /**
       * @brief - Close all the doors of this cell.
       */
      void
      close();

      /**
       * @brief - Toggle the status of the door at the specified
       *          index. In case the index is not valid, we raise
       *          an error.
       * @param id - the index of the door.
       * @param open - whether the door is open.
       */
      void
      toggle(unsigned id, bool open);

    private:

      /**
       * @brief - The list of doors of this cell.
       *          NOTE: We use a vector of bools and not a bitset
       *          as the bitset requires to know the size of the
       *          'vector' at compile time. We could probably use
       *          a specialzied version of the cell class but it
       *          seems like overkill.
       *          As per this thread though, it seems unclear if
       *          this is actually a performance tradeof:
       *          https://stackoverflow.com/questions/36932684/c11-vectorbool-performance-issue-with-code-example
       *          So for now it serves our purposes.
       */
      std::vector<bool> m_doors;
  };

  using CellShPtr = std::shared_ptr<Cell>;
}

#endif    /* CELL_HH */
