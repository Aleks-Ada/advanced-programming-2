#ifndef SRC_CONFIGURATION_CONFIGURATION_PARSER_H
#define SRC_CONFIGURATION_CONFIGURATION_PARSER_H

#include <string>
#include <vector>

#include "configuration.h"

enum class ConfigurationError {
  MultipleShipsWithSameStartingLetter,
  BoardSizeNotSpecified,
  BoardSizeTooBig,
  BoardSizeTooSmall,
  ShipTooBig,
  NoShips
};

class ConfigurationParser {
public:
  explicit ConfigurationParser(std::string configuration_string)
    : configuration_string(std::move(configuration_string)) {}

  Configuration Parse();
  const std::vector<ConfigurationError>& GetErrors() const;

private:
  void ParseBoard();
  void ParseShips();

  void ReportError(ConfigurationError configuration_error);

  std::string configuration_string;
  Configuration configuration;
  std::vector<ConfigurationError> errors;
};

#endif // SRC_CONFIGURATION_CONFIGURATION_PARSER_H
