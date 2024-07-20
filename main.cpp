import debug;
import heap;

int main() {
  heap::allocator heap{};
  heap.init();
  //{
  std::vector<heap::handle> handles{};
  handles.reserve(10);
  handles.emplace_back(heap.get(15));
  heap.print();
  handles.emplace_back(heap.get(2));
  heap.print();

  //}
  // heap.print();
}