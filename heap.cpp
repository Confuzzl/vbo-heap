module heap;

using namespace heap;

smart_handle::~smart_handle() {
  if (h.parent)
    h.parent->free(h);
}