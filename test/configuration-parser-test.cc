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
      "Board: 40x35\n"
      "Boat: Carrier, 5\n"
      "Boat: Battleship, 4\n"
      "Boat: Destroyer, 3\n"
      "Boat: Submarine, 3\n"
      "Boat: Patrol Boat, 2\n";
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

TEST(ConfigurationParserTest, UnixNewlines) {
  const std::string configuration_string =
      "Board: 40x35\r\n"
      "Boat: Carrier, 5\r\n"
      "Boat: Battleship, 4\r\n"
      "Boat: Destroyer, 3\r\n"
      "Boat: Submarine, 3\r\n"
      "Boat: Patrol Boat, 2\r\n";
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