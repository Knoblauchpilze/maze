
# include "Cell.hh"

namespace maze {

  Cell::Cell(unsigned doors, bool open):
    utils::CoreObject("cell"),

    m_doors(doors, open)
  {
    setService("maze");
  }

  unsigned
  Cell::doors() const noexcept {
    return m_doors.size();
  }

  bool
  Cell::operator()(unsigned id) const {
    if (id >= m_doors.size()) {
      error(
        "Failed to access door " + std::to_string(id),
        "Cell only defines " + std::to_string(m_doors.size())
      );
    }

    return m_doors[id];
  }

  void
  Cell::open() {
    for (unsigned id = 0u ; id < m_doors.size() ; ++id) {
      m_doors[id] = true;
    }
  }

  void
  Cell::close() {
    for (unsigned id = 0u ; id < m_doors.size() ; ++id) {
      m_doors[id] = false;
    }
  }

  void
  Cell::toggle(unsigned id, bool open) {
    if (id >= m_doors.size()) {
      error(
        "Failed to toggle door " + std::to_string(id),
        "Cell only defines " + std::to_string(m_doors.size())
      );
    }

    m_doors[id] = open;
  }

}
