#include <boost/asio.hpp>
#include <optional>
#include <queue>
#include <unordered_set>
#include <cstddef>
#include <iostream>

namespace io = boost::asio;
using tcp = io::ip::tcp;
using error_code = boost::system::error_code;
using namespace std::placeholders;

using message_handler = std::function<void(std::string)>;
using error_handler = std::function<void()>;
