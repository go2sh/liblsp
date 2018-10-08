#ifndef LSP_EVENT_H
#define LSP_EVENT_H

#include <functional>
#include <vector>

namespace lsp {
namespace Event {
template <typename... T> class Emitter {
public:
  typedef std::function<void(T...)> CallbackFunction;
  std::vector<CallbackFunction> Callbacks;
  void on(CallbackFunction Func) { Callbacks.push_back(Func); }

  void emit(T... args) {
    for (auto &Callback : Callbacks) {
      Callback(args...);
    }
  }
};
} // namespace Event
} // namespace lsp
#endif // !LSP_EVENT_H