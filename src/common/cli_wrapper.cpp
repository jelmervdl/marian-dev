#include "common/cli_wrapper.h"

namespace marian {
namespace cli {

CLIWrapper::CLIWrapper() : app_(std::make_shared<CLI::App>()) {
  app_->failure_message(failureMessage);
}

bool CLIWrapper::has(const std::string &key) const {
  return vars_.count(key) > 0;
}

std::string CLIWrapper::failureMessage(const CLI::App *app,
                                       const CLI::Error &e) {
  std::string header = "Error: " + std::string(e.what()) + "\n";
  if(app->get_help_ptr() != nullptr)
    header += "Run with " + app->get_help_ptr()->get_name()
              + " for more information.\n";
  return header;
}

}  // namespace cli
}  // namespace marian
