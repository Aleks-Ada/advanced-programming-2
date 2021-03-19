#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ostream>

#include "configuration/configuration.h"
#include "configuration/configuration-parser.h"

using ::testing::Contains;
using ::testing::UnorderedElementsAre;

void PrintTo(const ShipType& shipType, std::ostream* os) {
  *os << "ShipType{ " << shipType.name << ", " << shipType.size << " }";
}

TEST(ConfigurationParserTest, SimpleParse) {
  const std::string configuration_string =
      "Board: 80x80\n"
      "Boat: Carrier, 50\n"
      "Boat: Battleship, 4\n"
      "Boat: Destroyer, 75\n"
      "Boat: Submarine, 78\n"
      "Boat: Patrol Boat, 2\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_EQ(80, configuration.board_width);
  EXPECT_EQ(80, configuration.board_height);
  EXPECT_THAT(configuration.ship_types,
              UnorderedElementsAre(
                  ShipType{ "Carrier", 50 },
                  ShipType{ "Battleship", 4 },
                  ShipType{ "Destroyer", 75 },
                  ShipType{ "Submarine", 78 },
                  ShipType{ "Patrol Boat", 2 }));
}

TEST(ConfigurationParserTest, BadFormat) {
  const std::string configuration_string =
      "UktNdboat:Carrier,5F7WSB"
      "bevUIBOat:Battleship,4W7vkj"
      "ICIe2boAT:Destroyer,3uz1xS"
      "O8WP5BoAt:Submarine,3CeiBj"
      "n1sIkbOaRd:40x35oMGEB"
      "c3JnObOaT:Patrol Boat,2bQnFZ";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_EQ(40, configuration.board_width);
  EXPECT_EQ(35, configuration.board_height);
  EXPECT_THAT(configuration.ship_types,
              UnorderedElementsAre(
                  ShipType{ "Carrier", 5 },
                  ShipType{ "Battleship", 4 },
                  ShipType{ "Destroyer", 3 },
                  ShipType{ "Submarine", 3 },
                  ShipType{ "Patrol Boat", 2 }));
}

TEST(ConfigurationParserTest, MultipleBoardsFirstSaved) {
  const std::string configuration_string =
      "Board: 40x35\n"
      "Board: 10x10\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_EQ(40, configuration.board_width);
  EXPECT_EQ(35, configuration.board_height);
}

TEST(ConfigurationParserTest, MultipleShipsWithSameStartLetterRejected) {
  const std::string configuration_string =
      "Boat: Carrier, 5\n"
      "Boat: Cattleship, 4\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(configuration.ship_types, UnorderedElementsAre(ShipType{"Carrier", 5 }));
  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::MultipleShipsWithSameStartingLetter));
}

TEST(ConfigurationParserTest, NoBoardReportsError) {
  const std::string configuration_string =
      "Boat: Carrier, 5\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::BoardSizeNotSpecified));
}

TEST(ConfigurationParserTest, BoardSizeTooBigWidth) {
  const std::string configuration_string =
      "Board: 81x80\n"
      "Boat: Carrier, 5\n"
      "Boat: Battleship, 4\n"
      "Boat: Destroyer, 3\n"
      "Boat: Submarine, 3\n"
      "Boat: Patrol Boat, 2\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::BoardSizeTooBig));
}

TEST(ConfigurationParserTest, BoardSizeTooBigHeight) {
  const std::string configuration_string =
      "Board: 80x81\n"
      "Boat: Carrier, 5\n"
      "Boat: Battleship, 4\n"
      "Boat: Destroyer, 3\n"
      "Boat: Submarine, 3\n"
      "Boat: Patrol Boat, 2\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::BoardSizeTooBig));
}

TEST(ConfigurationParserTest, BoardSizeTooSmall) {
  const std::string configuration_string =
      "Board: 4x4\n"
      "Boat: Carrier, 5\n"
      "Boat: Battleship, 4\n"
      "Boat: Destroyer, 3\n"
      "Boat: Submarine, 3\n"
      "Boat: Patrol Boat, 2\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::BoardSizeTooSmall));
}

TEST(ConfigurationParserTest, ShipIgnoredWhenTooBig) {
  const std::string configuration_string =
      "Board: 20x20\n"
      "Boat: Carrier, 21\n"
      "Boat: Battleship, 4\n"
      "Boat: Destroyer, 3\n"
      "Boat: Submarine, 3\n"
      "Boat: Patrol Boat, 2\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::ShipTooBig));
}

TEST(ConfigurationParserTest, ShipIgnoredWhenTooSmall) {
  const std::string configuration_string =
      "Board: 20x20\n"
      "Boat: Carrier, 0\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::ShipTooSmall));
}

TEST(ConfigurationParserTest, NoShipsDetected) {
  const std::string configuration_string =
      "Board: 20x20\n";
  ConfigurationParser parser = ConfigurationParser(configuration_string);

  Configuration configuration = parser.Parse();

  EXPECT_THAT(parser.GetErrors(),
              Contains(ConfigurationError::NoShips));
}
