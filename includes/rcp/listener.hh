#include <netinet/in.h>

namespace rcp {
class connection;
class socket;

class listener {
public:
  connection Accept() const noexcept;
protected:
  listener(rcp::socket& socket, in_port_t port)
    : mSocket(socket)
    , mPort(port)
  {}

  friend class socket;

private:
  socket& mSocket;
  const in_port_t mPort;
};
} // end namespace rcp
