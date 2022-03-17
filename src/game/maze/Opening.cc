
# include "Opening.hh"
# include <cstdlib>

namespace maze {

  Opening::Opening(unsigned x,
                   unsigned y,
                   unsigned sides,
                   bool inverted) noexcept:
    m_x(x),
    m_y(y),

    m_doors(sides, true),
    m_opened(sides),

    m_inverted(inverted)
  {}

  unsigned
  Opening::x() const noexcept {
    return m_x;
  }

  unsigned
  Opening::y() const noexcept {
    return m_y;
  }

  bool
  Opening::inverted() const noexcept {
    return m_inverted;
  }

  void
  Opening::close(unsigned door) noexcept {
    if (door < m_doors.size()) {
      // Keep track of the number of opened doors.
      if (m_doors[door]) {
        --m_opened;
      }

      m_doors[door] = false;
    }
  }

  unsigned
  Opening::breach(bool& open) const noexcept {
    // In case there are no doors left to open, do
    // nothing and return immediately.
    open = (m_opened == m_doors.size());
    if (open) {
      return 0u;
    }

    return std::rand() % m_doors.size();
  }

}
