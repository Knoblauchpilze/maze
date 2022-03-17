
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
  Opening::breach(bool& boxed) const noexcept {
    // In case there are no doors left to open, do
    // nothing and return immediately.
    boxed = (m_opened == 0u);
    if (boxed) {
      return 0u;
    }

    unsigned id = 0u;
    bool valid = false;

    while (!valid) {
      id = std::rand() % m_doors.size();
      valid = (m_doors[id] == true);
    }

    return id;
  }

}
