import debug;
import heap;

int main() {
  heap::vbo_allocator heap{};
  heap.init();
  std::vector<heap::vbo_handle> handles;
  handles.emplace_back(heap.get<int>(3));
  handles.emplace_back(heap.get<double>(1));
  handles.emplace_back(heap.get<int>(1));
  heap.print();
  handles.erase(handles.begin() + 1);
  heap.print();
}