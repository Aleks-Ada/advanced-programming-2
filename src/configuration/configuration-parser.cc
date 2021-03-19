#include "configuration-parser.h"

#include <regex>
#include <set>

Configuration ConfigurationParser::Parse() {
  ParseBoard();
  ParseShips();

  return configuration;
}

void ConfigurationParser::ParseBoard() {
  const std::regex board_regex("[Bb][Oo][Aa][Rr][Dd]: ?(\\d{0,9})x(\\d{0,9})");

  std::smatch match;
  if (std::regex_search(configuration_string, match, board_regex)) {
    const std::string& width_string = match.str(1);
    const std::string& height_string = match.str(2);

    const int width = std::stoi(width_string);
    const int height = std::stoi(height_string);

    if ((width > 80) || (height > 80)) {
      ReportError(ConfigurationError::BoardSizeTooBig);
    } else if ((width < 5) || (height < 5)) {
      ReportError(ConfigurationError::BoardSizeTooSmall);
    }

    configuration.board_width = width;
    configuration.board_height = height;
  } else {
    ReportError(ConfigurationError::BoardSizeNotSpecified);
    configuration.board_width = 10;
    configuration.board_height = 10;
  }
}

void ConfigurationParser::ParseShips() {
  const std::regex boat_regex("[BbOoAaTt]: ?([A-Za-z ]+), ?(\\d{0,9})");
  std::set<char> ship_starting_letters_found;

  const std::sregex_iterator regex_end;
  for (std::sregex_iterator iterator(configuration_string.begin(),
                                     configuration_string.end(),
                                     boat_regex);
       iterator != regex_end; ++iterator) {
    const std::smatch& match = *iterator;

    const std::string& ship_name = match.str(1);
    const std::string& ship_size_string = match.str(2);

    const char ship_name_start_letter = ship_name.at(0);

    if (ship_starting_letters_found.find(ship_name_start_letter)
        != ship_starting_letters_found.end()) {
      ReportError(ConfigurationError::MultipleShipsWithSameStartingLetter);
    } else {
      const int ship_size = std::stoi(ship_size_string);

      if (ship_size < 1) {
        ReportError(ConfigurationError::ShipTooSmall);
      } else if ((ship_size > configuration.board_height) || (ship_size > configuration.board_width)) {
        ReportError(ConfigurationError::ShipTooBig);
      } else {
        ShipType ship_type;
        ship_type.name = ship_name;
        ship_type.size = ship_size;

        ship_starting_letters_found.emplace(ship_name_start_letter);
        configuration.ship_types.emplace_back(ship_type);
      }
    }
  }

  if (configuration.ship_types.empty()) {
    ReportError(ConfigurationError::NoShips);
  }
}

const std::vector<ConfigurationError>& ConfigurationParser::GetErrors() const {
  return errors;
}

void ConfigurationParser::ReportError(ConfigurationError configuration_error) {
  errors.emplace_back(configuration_error);
}
