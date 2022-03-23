#ifndef    OPENING_HH
# define   OPENING_HH

# include <vector>

namespace maze {

  class Opening {
    public:

      /**
       * @brief - Create a new opening with the specified coordinates
       *          and all doors opened.
       * @param x - the x coordinate of the opening.
       * @param y - the y coordinate of the opening.
       * @param sides - the number of doors for the opening.
       * @param inverted - whether it is an inverted cell.
       */
      Opening(unsigned x,
              unsigned y,
              unsigned sides,
              bool inverted) noexcept;

      /**
       * @brief - The x coordinate of the opening.
       * @return - the x coordinate.
       */
      unsigned
      x() const noexcept;

      /**
       * @brief - The y coordinate of the opening.
       * @return - the y coordinate.
       */
      unsigned
      y() const noexcept;

      /**
       * @brief - Whether the opening concerns an inverted
       *          cell.
       * @return - whether the opening is for an inverted cell.
       */
      bool
      inverted() const noexcept;

      /**
       * @brief - Close the door at the specified index. In case
       *          the index is not valid nothing happens.
       * @param door - the index of the door to close.
       */
      void
      close(unsigned door) noexcept;

      /**
       * @brief - Used to determine whether the door at the input
       *          index is valid to be opened or not.
       * @param door - the door to be opened. In case the door is
       *               not within the admissible range we assume
       *               it can't be opened.
       * @return - `true` if the door can be opened.
       */
      bool
      canBeOpened(unsigned door) const noexcept;

      /**
       * @brief - Pick a random door to open among the ones that
       *          are not yet opened.
       *          In case all the doors are obstructed, the output
       *          value will be set to `true`.
       * @param boxed - `true` in case all doors are not available
       *                to be breached. In this case, ignore the
       *                return value.
       * @return - the index of the door to open.
       */
      unsigned
      breach(bool& boxed) const noexcept;

    private:

      /**
       * @brief - The x coordinate of the opening.
       */
      unsigned m_x;

      /**
       * @brief - The y coordinate of the opening.
       */
      unsigned m_y;

      /**
       * @brief - The status of each door of the opening.
       */
      std::vector<bool> m_doors;

      /**
       * @brief - The amount of doors that are opened.
       */
      unsigned m_opened;

      /**
       * @brief - Whether the opening is an inverted cell
       *          or not.
       */
      bool m_inverted;
  };

}

#endif    /* OPENING_HH */
