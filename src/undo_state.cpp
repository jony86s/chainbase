#include "undo_state.hpp" // chainrocks::undo_state

namespace chainrocks {
   undo_state::undo_state()
      : _modified_values{}
      , _removed_values{}
      , _new_keys{}
   {
   }
}
