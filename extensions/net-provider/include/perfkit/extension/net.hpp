#pragma once
#include "perfkit/terminal.h"

namespace perfkit::terminal::net {
/**
 * Create net client session instance
 *
 * @param info
 */
terminal_ptr create(struct terminal_init_info const& info);

/**
 * Create net client from configuration profile
 * @param config_profile_name
 */
terminal_ptr create(std::string config_profile_name);

struct terminal_init_info {
  /** Session name to introduce self */
  std::string const name;

  /** Blocks until the first connection is successfully established. */
  bool wait_connection = false;

  /** Host name to connect with. */
  std::string host_name = "localhost";

  /** Describes this session */
  std::string description;

  /** Host port to connect with. */
  uint16_t host_port = 25572;

  /**
   * Specify if remote connection should be read-only.
   * For security reason, default value is set to true. */
  bool read_only_access = true;

  /**
   * Specify if password is required for access.
   * If password is empty, any access will be allowed. */
  std::string password;

  /**
   * Specify how many milliseconds to wait for host response */
  std::chrono::milliseconds connection_timeout_ms{5'000};

  explicit terminal_init_info(std::string session_name) : name{std::move(session_name)} {};
};

}  // namespace perfkit::terminal::net