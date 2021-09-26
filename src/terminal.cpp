//
// Created by Seungwoo on 2021-09-25.
//
#include <filesystem>

#include <perfkit/detail/base.hpp>
#include <perfkit/detail/commands.hpp>
#include <perfkit/terminal.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

#include "detail/basic_interactive_terminal.hpp"

namespace perfkit {
auto create_basic_interactive_terminal() -> std::shared_ptr<if_terminal> {
  return std::make_shared<basic_interactive_terminal>();
}
}  // namespace perfkit

namespace perfkit::terminal {
using commands::args_view;
using commands::command_already_exist_exception;
using commands::command_name_invalid_exception;
using commands::string_set;

class _config_saveload_manager {
 public:
  bool load_from(args_view args = {}) {
    auto path = args.empty() ? _latest : args.front();
    _latest   = path;
    return perfkit::import_options(path);
  }

  bool save_to(args_view args = {}) {
    auto path = args.empty() ? _latest : args.front();
    _latest   = path;
    return perfkit::export_options(path);
  }

  void retrieve_filenames(args_view args, string_set& cands) {
    namespace fs = std::filesystem;

    fs::path path;
    if (!args.empty()) { path = args[0]; }
    if (path.empty()) { path = "./"; }

    if (!is_directory(path)) { path = path.parent_path(); }
    if (path.empty()) { path = "./"; }

    fs::directory_iterator it{path}, end{};
    std::transform(
            it, end, std::inserter(cands, cands.end()),
            [](auto&& p) {
              fs::path path = p.path();
              auto&& str    = path.string();

              return fs::is_directory(path) ? str.append("/") : str;
            });
  }

 public:
  std::string _latest = {};
};

void register_conffile_io_commands(
        perfkit::if_terminal* ref,
        std::string_view cmd_load,
        std::string_view cmd_store,
        std::string_view initial_path) {
  auto manip     = std::make_shared<_config_saveload_manager>();
  manip->_latest = initial_path;

  auto rootnode  = ref->commands()->root();
  auto node_load = rootnode->add_subcommand(
          cmd_load,
          [manip](auto&& tok) { return manip->load_from(tok); },
          [manip](auto&& tok, auto&& set) { return manip->retrieve_filenames(tok, set); });

  auto node_save = rootnode->add_subcommand(
          cmd_store,
          [manip](auto&& tok) { return manip->save_to(tok); },
          [manip](auto&& tok, auto&& set) { return manip->retrieve_filenames(tok, set); });

  if (!node_load || !node_save) { throw command_already_exist_exception{}; }
}

void register_logging_manip_command(if_terminal* ref, std::string_view cmd) {
  std::string cmdstr{cmd};
  auto logging = ref->commands()->root()->add_subcommand(
          cmd,
          [ref, cmdstr](args_view args) -> bool {
            if (args.empty() || args.size() > 2) {
              glog()->error("usage: {} <'_global_'|'_default_'|logger> [loglevel]", cmdstr);
              return false;
            }

            if (args.size() == 1) {
              spdlog::string_view_t sv = {};
              if (args[0] == "_global_") {
                sv = to_string_view(spdlog::get_level());
              } else if (args[0] == "_default_") {
                sv = to_string_view(spdlog::default_logger()->level());
              } else if (auto logger = spdlog::get(std::string{args[0]})) {
                sv = to_string_view(logger->level());
              } else {
                glog()->error("logger {} not found.", args[0]);
                return false;
              }

              ref->puts(fmt::format("{}={}", args[0], sv));
            } else {
              auto lv = spdlog::level::from_str(std::string{args[1]});

              if (args[0] == "_global_") {
                spdlog::set_level(lv);
              } else if (args[0] == "_default_") {
                spdlog::default_logger()->set_level(lv);
              } else if (auto logger = spdlog::get(std::string{args[0]})) {
                logger->set_level(lv);
              } else {
                glog()->error("logger {} not found.", args[0]);
                return false;
              }
            }

            return true;
          },
          [ref, cmdstr](auto&&, auto&& cands) -> bool {
            auto node = ref->commands()->root()->find_subcommand(cmdstr);
            spdlog::details::registry::instance().apply_all(
                    [&cands](const std::shared_ptr<spdlog::logger>& logger) {
                      if (logger->name().empty()) { return; }
                      cands.insert(logger->name());
                    });
            cands.insert("_global_");
            cands.insert("_default_");

            return true;
          });
}

void register_trace_manip_command(if_terminal* ref, std::string_view cmd) {
}

void register_config_manip_command(if_terminal* ref, std::string_view cmd) {
}

void initialize_with_basic_commands(if_terminal* ref) {
  register_logging_manip_command(ref);
  register_trace_manip_command(ref);
  register_conffile_io_commands(ref);
  register_config_manip_command(ref);
}

}  // namespace perfkit::terminal
