#include <fstream>
#include <iostream>
#include <optional>
#include <regex>

#include "board/auto-placer.h"
#include "board/random-placement-generator.h"
#include "board-renderer/board-renderer.h"
#include "configuration/configuration-parser.h"
#include "computer-ai.h"

void ClearScreen() {
  std::cout << "\033c";
}

template<typename T>
void Print(const T value) {
  std::cout << std::to_string(value);
}

template<>
void Print(const std::string value) {
  std::cout << value;
}

template<>
void Print(const std::string_view value) {
  std::cout << value;
}

template<>
void Print(const char* value) {
  std::cout << value;
}

template<typename T>
void PrintLine(const T value) {
  std::cout << std::to_string(value) << "\n";
}

template<>
void PrintLine(const std::string value) {
  std::cout << value << "\n";
}

template<>
void PrintLine(const char* value) {
  std::cout << value << "\n";
}

void PrintLine() {
  std::cout << "\n";
}

void PrintBoard(const BoardRenderer& board_renderer) {
  ClearScreen();
  PrintLine(board_renderer.Render());
}

std::string GetLine() {
  std::string line;
  std::getline(std::cin, line);
  return line;
}

std::optional<std::string> ReadFile(const char* const name) {
  std::ifstream ifstream(name, std::ios::binary | std::ios::ate);
  const int size = ifstream.tellg();
  if (size < 0) {
    return std::nullopt;
  }
  std::string output(size, '\0');
  ifstream.seekg(0);
  ifstream.read(&output[0], size);
  return output;
}

Configuration ReadConfiguration() {
  const std::optional<std::string> configuration_string = ReadFile("adaship_config.ini");

  if (configuration_string.has_value()) {
    ConfigurationParser configuration_parser =
        ConfigurationParser(configuration_string.value());
    Configuration configuration = configuration_parser.Parse();

    bool can_load = configuration_parser.GetErrors().empty();

    if (can_load) {
      const int board_area = configuration.board_height * configuration.board_width;
      const int reasonable_board_area = 0.75f * board_area;
      const int ship_area = std::accumulate(configuration.ship_types.begin(),
                                            configuration.ship_types.end(),
                                            0,
                                            [](const int sum, const ShipType& current)
                                            {
                                              return sum + current.size;
                                            });

      if (ship_area > reasonable_board_area) {
        const int new_area = (ship_area / 0.75f);

        PrintLine("Ships take up too much of the board.");
        if (new_area > (80 * 80)) {
          PrintLine("There are too many ships. Please remove some from the configuration.");
          can_load = false;
        } else {
          PrintLine("The board will be resized to contain these ships.");

          const int sqrt = std::sqrt(new_area);
          const int new_value = sqrt + 1;

          configuration.board_width = new_value;
          configuration.board_height = new_value;
        }
        PrintLine();
      }

      if (can_load) {
        return configuration;
      }
    }

    PrintLine("Detected errors with configuration file.");
    for (auto error : configuration_parser.GetErrors()) {
      if (error == ConfigurationError::BoardSizeNotSpecified) {
        PrintLine("Board size not specified.");
      } else if (error == ConfigurationError::MultipleShipsWithSameStartingLetter) {
        PrintLine("Multiple boats with the same starting letter.");
      } else if (error == ConfigurationError::BoardSizeTooBig) {
        PrintLine("Board size is too large. (Must be at most 80x80)");
      } else if (error == ConfigurationError::BoardSizeTooSmall) {
        PrintLine("Board size is too small. (Must be at least 5x5)");
      } else if (error == ConfigurationError::ShipTooBig) {
        PrintLine("A boat was ignored because it is bigger than the board's width or height.");
      } else if (error == ConfigurationError::ShipTooSmall) {
        PrintLine("A boat was ignored because it is too small (size 0).");
      } else if (error == ConfigurationError::NoShips) {
        PrintLine("No boats were defined. (Please list at least one in the format "
                  "'Boat: {name}, {size}', e.g. 'Boat: Carrier, 5')");
      }
    }
    PrintLine("Press enter to load defaults for the current session.");
    GetLine();
  } else {
    PrintLine("Configuration file not found. Press enter to load defaults.");
    GetLine();
  }

  Configuration configuration;
  configuration.board_height = 10;
  configuration.board_width = 10;
  configuration.ship_types.emplace_back(ShipType{ "Carrier", 5 });
  configuration.ship_types.emplace_back(ShipType{ "Battleship", 4 });
  configuration.ship_types.emplace_back(ShipType{ "Destroyer", 3 });
  configuration.ship_types.emplace_back(ShipType{ "Submarine", 3 });
  configuration.ship_types.emplace_back(ShipType{ "Patrol Boat", 2 });
  return configuration;
}

struct ShipChoice {
  explicit ShipChoice(ShipType ship_type) : ship_type(std::move(ship_type)), is_placed(false) {};
  ShipType ship_type;
  bool is_placed;
};

struct ShipPlacement {
  ShipChoice* ship_choice;
  Orientation orientation;
  Location location;
};

std::optional<ShipChoice*> ChooseShipType(std::vector<ShipChoice>& ship_choices) {
  PrintLine("Choose a ship to place:");

  for (int index = 0; index < ship_choices.size(); ++index) {
    const ShipChoice& ship_choice = ship_choices.at(index);

    if (!ship_choice.is_placed) {
      Print("(");
      Print(index + 1);
      Print(") ");
      Print("[size: ");
      Print(ship_choice.ship_type.size);
      Print("] ");
      Print(ship_choice.ship_type.name);
      PrintLine();
    }
  }
  PrintLine();
  PrintLine("(0) Cancel");
  Print("[0]: ");

  const std::string choice = GetLine();

  if (std::regex_match(choice, std::regex("\\d+"))) {
    const int choice_int = std::stoi(choice);

    if ((choice_int > 0) && (choice_int <= ship_choices.size())) {
      ShipChoice& ship_choice = ship_choices.at(choice_int - 1);

      if (!ship_choice.is_placed) {
        return &ship_choice;
      }
    }
  }

  return std::nullopt;
}

std::optional<Orientation> ChooseShipOrientation() {
  PrintLine("Please choose an orientation:");
  PrintLine("(1) Horizontal");
  PrintLine("(2) Vertical");
  PrintLine();
  PrintLine("(0) Cancel");
  Print("[0]: ");

  const std::string choice = GetLine();

  if (choice == "1") {
    return Orientation::Horizontal;
  } else if (choice == "2") {
    return Orientation::Vertical;
  }

  return std::nullopt;
}

std::optional<Location> ChooseLocation(const Configuration& configuration) {
  Print("Please enter a column-row index (e.g. A2, cancel if invalid): ");

  const std::string choice = GetLine();

  std::string column;
  int row;

  std::smatch regex_match;
  if (std::regex_match(choice, regex_match, std::regex(R"(^([A-Za-z]{1,2})(\d{1,2})$)"))) {
    column = regex_match.str(1);
    row = std::stoi(regex_match.str(2));
  } else if (std::regex_match(choice, regex_match, std::regex(R"(^(\d{1,2})([A-Za-z]{1,2})$)"))) {
    row = std::stoi(regex_match.str(1));
    column = regex_match.str(2);
  } else {
    return std::nullopt;
  }

  Location location;

  try {
    location = BoardLocation(column, row);
  } catch (const std::runtime_error&) {
    return std::nullopt;
  }

  if ((location.x <= 0) ||
      (location.y <= 0) ||
      (location.x > configuration.board_width) ||
      (location.y > configuration.board_height)) {
    return std::nullopt;
  }

  return location;
}

std::optional<ShipPlacement> ChooseShipWithoutType(
    const BoardRenderer& board_renderer,
    const Configuration& configuration,
    ShipChoice* ship_choice) {
  PrintBoard(board_renderer);
  Print("Ship type: ");
  Print(ship_choice->ship_type.name);
  PrintLine();
  PrintLine();
  const auto orientation = ChooseShipOrientation();

  if (!orientation.has_value()) {
    return std::nullopt;
  }

  PrintBoard(board_renderer);
  Print("Ship type: ");
  Print(ship_choice->ship_type.name);
  PrintLine();
  Print("Ship orientation: ");
  Print(orientation == Orientation::Vertical ? "vertical" : "horizontal");
  PrintLine();
  PrintLine();
  const auto location = ChooseLocation(configuration);

  if (!location.has_value()) {
    return std::nullopt;
  }

  return ShipPlacement{ ship_choice, orientation.value(), location.value() };
}

std::optional<ShipPlacement> ChooseShipPlacement(
    const BoardRenderer& board_renderer,
    const Configuration& configuration,
    std::vector<ShipChoice>& ship_choices) {
  PrintBoard(board_renderer);
  const auto ship_choice = ChooseShipType(ship_choices);

  if (!ship_choice.has_value()) {
    return std::nullopt;
  }

  return ChooseShipWithoutType(board_renderer, configuration, ship_choice.value());
}

struct ShipMovement {
  Location source_location;
  ShipPlacement placement;
};

std::optional<ShipMovement> ChooseShipMovement(
    const Board& board,
    const BoardRenderer& renderer,
    const Configuration& configuration) {
  PrintBoard(renderer);

  const auto source_location = ChooseLocation(configuration);

  if (!source_location.has_value()) {
    return std::nullopt;
  }

  const auto boat = board.GetBoat(source_location.value());

  if (boat.has_value()) {
    ShipChoice choice = ShipChoice(boat->GetShipType());
    const auto placement = ChooseShipWithoutType(renderer, configuration, &choice);

    if (placement.has_value()) {
      return ShipMovement{ source_location.value(), placement.value() };
    }
  }

  return std::nullopt;
}

bool InitializeBoard(const Configuration& configuration,
                     Board& user_board,
                     BoardRenderer& user_board_renderer,
                     PlacementGenerator& placement_generator) {
  std::vector<ShipChoice> ship_choices;
  std::transform(configuration.ship_types.begin(),
                 configuration.ship_types.end(),
                 std::back_inserter(ship_choices),
                 [](const ShipType& ship_type) {
                   return ShipChoice(ship_type);
                 });

  while (true) {
    PrintLine("Please choose:");

    const bool can_place_ship = user_board.PlacedBoatsCount() < configuration.ship_types.size();
    const bool can_move_ship = user_board.PlacedBoatsCount() > 0;
    const bool can_continue = !can_place_ship;

    std::string default_choice = "0";
    if (can_place_ship) {
      PrintLine("(1) Place ship");
      default_choice = "1";
      PrintLine("(2) Auto-place remaining ships");
    }
    if (can_move_ship) {
      PrintLine("(3) Move ship");
      PrintLine("(4) Reset board");
    }
    if (can_continue) {
      PrintLine("(5) Continue");
      default_choice = "5";
    }
    PrintLine();
    PrintLine("(0) Quit");
    Print("[");
    Print(default_choice);
    Print("]: ");

    std::string choice = GetLine();

    if (choice.empty() ||
        (choice != "0" && choice != "1" && choice != "2" && choice != "3" && choice != "4" &&
         choice != "5")) {
      choice = default_choice;
    }

    if (choice == "1") {
      if (can_place_ship) {
        std::optional<ShipPlacement> ship_to_place =
            ChooseShipPlacement(user_board_renderer, configuration, ship_choices);

        if (ship_to_place.has_value()) {
          if (user_board.AddBoat(
              ship_to_place->ship_choice->ship_type,
              ship_to_place->location,
              ship_to_place->orientation)) {
            ship_to_place->ship_choice->is_placed = true;
          }
        }
      } else if (can_continue) {
        break;
      }
    } else if (choice == "2") {
      if (can_place_ship) {
        AutoPlacer auto_placer(user_board, placement_generator);

        std::vector<ShipType> remaining_boats;

        for (const ShipChoice& ship_choice : ship_choices) {
          if (!ship_choice.is_placed)  {
            remaining_boats.emplace_back(ship_choice.ship_type);
          }
        }

        auto_placer.AutoPlace(remaining_boats);
      }
    } else if (choice == "3") {
      if (can_move_ship) {
        std::optional<ShipMovement> ship_to_move =
            ChooseShipMovement(user_board, user_board_renderer, configuration);

        if (ship_to_move.has_value()) {
          const bool success = user_board.MoveBoat(
              user_board.GetBoat(ship_to_move->source_location)->GetShipType(),
              ship_to_move->placement.location,
              ship_to_move->placement.orientation);

          if (!success) {
            ship_to_move->placement.ship_choice->is_placed = false;
          }
        }
      }
    } else if (choice == "4") {
      if (can_move_ship) {
        user_board.Reset();

        for (ShipChoice& ship_choice : ship_choices) {
          ship_choice.is_placed = false;
        }
      }
    } else if (choice == "5") {
      if (can_continue) {
        break;
      }
    } else if (choice == "0") {
      return false;
    }

    PrintBoard(user_board_renderer);
  }

  return true;
}

void PressEnterToContinue() {
  Print("Press enter to continue. ");
  GetLine();
}

enum FireMode {
  NORMAL,
  SALVO,
  HIDDEN_MINES
};

bool UserTurn(const std::string_view name,
              const Configuration& configuration,
              RandomPlacementGenerator& placement_generator,
              Board& user_board,
              BoardRenderer& user_board_renderer,
              Board& opponent_board,
              BoardRenderer& opponent_board_renderer,
              const FireMode fire_mode) {
  int shots = 1;

  if  (fire_mode == SALVO)  {
    shots = user_board.GetRemainingShips().size();
  }

  for (int shot = 1; shot <= shots; ++shot) {
    while (true) {
      ClearScreen();
      Print("It's ");
      Print(name);
      PrintLine("'s turn.");
      PrintLine();
      PrintLine("Your board:");
      user_board_renderer.SetMode(SELF);
      PrintLine(user_board_renderer.Render());
      PrintLine("Your opponent's board:");
      opponent_board_renderer.SetMode(TARGET);
      PrintLine(opponent_board_renderer.Render());

      if (shots == 1) {
        PrintLine("Please choose:");
      } else {
        Print("Please choose (shot ");
        Print(shot);
        Print(" / ");
        Print(shots);
        PrintLine("):");
      }
      PrintLine("(1) Fire at chosen location");
      PrintLine("(2) Fire at a random location");
      PrintLine();
      PrintLine("(0) Quit");
      Print("[1]: ");

      std::string choice = GetLine();
      Location fire_location;

      if (choice != "0" && choice != "1" && choice != "2") {
        choice = "1";
      }

      if (choice == "1") {
        const auto location = ChooseLocation(configuration);

        if (location.has_value()) {
          fire_location = location.value();
        }
      } else if (choice == "2") {
        fire_location = placement_generator.ChooseLocation(opponent_board.NotFiredLocations());
      } else {
        return false;
      }

      if (opponent_board.Shoot(fire_location)) {
        ClearScreen();
        Print("It's ");
        Print(name);
        PrintLine("'s turn.");
        PrintLine();
        PrintLine("Your board:");
        user_board_renderer.SetMode(SELF);
        PrintLine(user_board_renderer.Render());
        PrintLine("Your opponent's board:");
        opponent_board_renderer.SetMode(TARGET);
        PrintLine(opponent_board_renderer.Render());
        Print("You shot at ");
        Print(fire_location.ToString());
        PrintLine(".");
        if (opponent_board.IsMine(fire_location)) {
          PrintLine("You hit a mine!");
        } else if (opponent_board.IsHit(fire_location)) {
          PrintLine("You scored a hit! Well done!");
        } else {
          PrintLine("You missed. Better luck next time!");
        }
        PressEnterToContinue();
        break;
      } else {
        PrintLine("Invalid shot. Try again.");
      }
    }
  }

  return true;
}

void ComputerTurn(const std::string_view name,
                  RandomPlacementGenerator& placement_generator,
                  ComputerAi& computer_ai,
                  Board& computer_board,
                  BoardRenderer& computer_board_renderer,
                  Board& opponent_board,
                  BoardRenderer& opponent_board_renderer,
                  const FireMode fire_mode = NORMAL) {
  int shots = 1;

  if (fire_mode == SALVO) {
    shots = computer_board.GetRemainingShips().size();
  }

  for (int shot = 0; shot < shots; ++shot) {
    const Location fire_location = computer_ai.ChooseNextShot();

    if (opponent_board.Shoot(fire_location)) {
      ClearScreen();
      Print("It's ");
      Print(name);
      PrintLine("'s turn.");
      PrintLine();
      PrintLine("The computer's board:");
      computer_board_renderer.SetMode(SELF);
      PrintLine(computer_board_renderer.Render());
      PrintLine("The computer's opponent board:");
      opponent_board_renderer.SetMode(TARGET);
      PrintLine(opponent_board_renderer.Render());
      Print("The computer shot at ");
      Print(fire_location.ToString());
      PrintLine(".");

      if (opponent_board.IsMine(fire_location)) {
        PrintLine("The computer hit a mine!");
      } else if (opponent_board.IsHit(fire_location)) {
        PrintLine("The computer hit the opponent!");
      } else {
        PrintLine("The computer missed. Whew!");
      }
    }

    PressEnterToContinue();
  }
}

bool UserVsComputer(const Configuration& configuration,
                    RandomPlacementGenerator& placement_generator,
                    const FireMode fire_mode = NORMAL) {
  Board user_board(configuration.board_width, configuration.board_height);
  BoardRenderer user_board_renderer(user_board);
  PrintBoard(user_board_renderer);
  PrintLine();

  if (fire_mode == HIDDEN_MINES) {
    user_board.AddRandomMines(placement_generator);
  }

  bool success = InitializeBoard(configuration,
                                 user_board,
                                 user_board_renderer,
                                 placement_generator);

  if (!success) {
    return false;
  }

  Board computer_board(configuration.board_width, configuration.board_height);
  {
    AutoPlacer computer_auto_placer(computer_board, placement_generator);
    computer_auto_placer.AutoPlace(configuration.ship_types);
  }
  BoardRenderer computer_board_renderer(computer_board);
  ComputerAi computer_ai(user_board, placement_generator);

  if (fire_mode == HIDDEN_MINES) {
    computer_board.AddRandomMines(placement_generator);
  }

  while (true) {
    success = UserTurn("the player", configuration, placement_generator,
                       user_board, user_board_renderer,
                       computer_board, computer_board_renderer,
                       fire_mode);

    if (!success) {
      return false;
    }

    if (computer_board.AreAllShipsSunk()) {
      PrintLine("The player won!");
      break;
    }

    ComputerTurn("the computer", placement_generator, computer_ai,
                 computer_board, computer_board_renderer,
                 user_board, user_board_renderer,
                 fire_mode);

    if (user_board.AreAllShipsSunk()) {
      PrintLine("The computer won!");
      break;
    }
  }

  PressEnterToContinue();

  return true;
}

bool UserVsUser(const Configuration& configuration,
                RandomPlacementGenerator& placement_generator,
                const FireMode fire_mode = NORMAL) {
  Board user_1_board(configuration.board_width, configuration.board_height);
  BoardRenderer user_1_board_renderer(user_1_board);
  PrintBoard(user_1_board_renderer);
  PrintLine();

  if (fire_mode == HIDDEN_MINES) {
    user_1_board.AddRandomMines(placement_generator);
  }

  bool success = InitializeBoard(configuration,
                                 user_1_board,
                                 user_1_board_renderer,
                                 placement_generator);

  if (!success) {
    return false;
  }

  Board user_2_board(configuration.board_width, configuration.board_height);
  BoardRenderer user_2_board_renderer(user_2_board);
  PrintBoard(user_2_board_renderer);
  PrintLine();

  if (fire_mode == HIDDEN_MINES) {
    user_2_board.AddRandomMines(placement_generator);
  }

  success = InitializeBoard(configuration,
                            user_2_board,
                            user_2_board_renderer,
                            placement_generator);

  if (!success) {
    return false;
  }

  while (true) {
    success = UserTurn("player 1", configuration, placement_generator,
                       user_1_board, user_1_board_renderer,
                       user_2_board, user_2_board_renderer,
                       fire_mode);

    if (!success) {
      return false;
    }

    if (user_2_board.AreAllShipsSunk()) {
      PrintLine("Player 1 won!");
      break;
    }

    success = UserTurn("player 2", configuration, placement_generator,
                       user_2_board, user_2_board_renderer,
                       user_1_board, user_1_board_renderer,
                       fire_mode);

    if (!success) {
      return false;
    }

    if (user_1_board.AreAllShipsSunk()) {
      PrintLine("Player 2 won!");
      break;
    }
  }

  PressEnterToContinue();

  return true;
}

void ComputerVsComputerHiddenMines(const Configuration& configuration,
                                   RandomPlacementGenerator& placement_generator) {
  Board computer_1_board(configuration.board_width, configuration.board_height);
  computer_1_board.AddRandomMines(placement_generator);
  BoardRenderer computer_1_board_renderer(computer_1_board);

  Board computer_2_board(configuration.board_width, configuration.board_height);
  computer_2_board.AddRandomMines(placement_generator);
  BoardRenderer computer_2_board_renderer(computer_2_board);

  {
    AutoPlacer computer_1_auto_placer(computer_1_board, placement_generator);
    computer_1_auto_placer.AutoPlace(configuration.ship_types);
    AutoPlacer computer_2_auto_placer(computer_2_board, placement_generator);
    computer_2_auto_placer.AutoPlace(configuration.ship_types);
  }

  ComputerAi computer_1_ai(computer_2_board, placement_generator);
  ComputerAi computer_2_ai(computer_1_board, placement_generator);

  while (true) {
    ComputerTurn("computer 1", placement_generator, computer_1_ai,
                 computer_1_board, computer_1_board_renderer,
                 computer_2_board, computer_2_board_renderer,
                 NORMAL);

    if (computer_2_board.AreAllShipsSunk()) {
      PrintLine("Computer 1 won!");
      break;
    }

    ComputerTurn("computer 2", placement_generator, computer_2_ai,
                 computer_2_board, computer_2_board_renderer,
                 computer_1_board, computer_1_board_renderer,
                 NORMAL);

    if (computer_1_board.AreAllShipsSunk()) {
      PrintLine("Computer 2 won!");
      break;
    }
  }

  PressEnterToContinue();
}

int main()
{
  RandomPlacementGenerator placement_generator;
  Configuration configuration = ReadConfiguration();

  while (true) {
    ClearScreen();
    PrintLine("Please choose:");
    PrintLine("(1) one player vs computer game");
    PrintLine("(2) two player game");
    PrintLine("(3) one player vs computer (salvo) game");
    PrintLine("(4) two player (salvo) game");
    PrintLine("(5) one player vs computer (hidden mines) game");
    PrintLine("(6) two player (hidden mines) game");
    PrintLine("(7) computer vs computer (hidden mines) game");
    PrintLine();
    PrintLine("(0) Quit");
    Print("[0]: ");

    const std::string game_choice = GetLine();

    if (game_choice == "1") {
      UserVsComputer(configuration, placement_generator);
    } else if (game_choice == "2") {
      UserVsUser(configuration, placement_generator);
    } else if (game_choice == "3") {
      UserVsComputer(configuration, placement_generator, SALVO);
    } else if (game_choice == "4") {
      UserVsUser(configuration, placement_generator, SALVO);
    } else if (game_choice == "5") {
      UserVsComputer(configuration, placement_generator, HIDDEN_MINES);
    } else if (game_choice == "6") {
      UserVsUser(configuration, placement_generator, HIDDEN_MINES);
    } else if (game_choice == "7") {
      ComputerVsComputerHiddenMines(configuration, placement_generator);
    } else {
      return 0;
    }
  }
}
