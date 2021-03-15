#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <queue>

#include "board/auto-placer.h"
#include "board/board.h"

using ::testing::Optional;
using ::testing::UnorderedElementsAre;

void PrintTo(const Boat& boat, std::ostream* os) {
  *os << "Boat{ "
      << boat.GetName()
      << ", " << boat.GetSize()
      << ", " << (boat.GetOrientation() == Orientation::Vertical ? "Vertical" : "Horizontal")
      << " }";
}

void PrintTo(const Location location, std::ostream* os) {
  *os << "Location{ " << location.x << ", " << location.y << " }";
}

TEST(BoardTest, PlaceShip) {
  const ShipType cattleship = ShipType{"Cattleship", 4 };
  Board board(10, 10);

  board.AddBoat(cattleship, BoardLetterIndex(A, 1), Orientation::Vertical);

  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 1))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 2))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 3))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 4))->GetShipType(), cattleship);
}

TEST(BoardTest, CountsPlacedShips) {
  ShipType battleship = ShipType{ "Battleship", 5 };
  ShipType submarine = ShipType{ "Submarine", 3 };
  ShipType patrol_boat = ShipType{ "Patrol Boat", 2 };
  Board board(10, 10);

  board.AddBoat(battleship, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.AddBoat(submarine, BoardLetterIndex(B, 1), Orientation::Vertical);
  board.AddBoat(patrol_boat, BoardLetterIndex(C, 1), Orientation::Vertical);

  EXPECT_EQ(3, board.PlacedBoatsCount());
}

TEST(BoardTest, OverlapPlacementDoesntWork) {
  const ShipType cattleship = ShipType{"Cattleship", 4 };
  const ShipType battleship = ShipType{"Battleship", 4 };
  Board board(10, 10);

  const bool placement1 = board.AddBoat(cattleship, BoardLetterIndex(A, 1), Orientation::Vertical);
  // Overlap 1 cell
  const bool placement2 = board.AddBoat(battleship, BoardLetterIndex(A, 4), Orientation::Vertical);

  EXPECT_TRUE(placement1);
  EXPECT_FALSE(placement2);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 1))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 2))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 3))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 4))->GetShipType(), cattleship);
}

TEST(BoardTest, EdgeOfBoardPlacementDoesntWork) {
  const ShipType cattleship = ShipType{"Cattleship", 4 };
  Board board(10, 10);

  const bool placement = board.AddBoat(cattleship, BoardLetterIndex(A, 10), Orientation::Vertical);

  EXPECT_FALSE(placement);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 10)), std::nullopt);
}

TEST(BoardTest, OutOfBoardPlacementDoesntWork) {
  const ShipType cattleship = ShipType{"Cattleship", 4 };
  Board board(10, 10);

  const bool placement = board.AddBoat(cattleship, BoardLetterIndex(Z, 50), Orientation::Vertical);

  EXPECT_FALSE(placement);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(Z, 50)), std::nullopt);
}

TEST(BoardTest, MoveShip) {
  const ShipType cattleship = ShipType{"Cattleship", 4 };
  Board board(10, 10);

  board.AddBoat(cattleship, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.MoveBoat(cattleship, BoardLetterIndex(C, 7), Orientation::Vertical);
  board.MoveBoat(cattleship, BoardLetterIndex(E, 7), Orientation::Horizontal);

  EXPECT_THAT(board.GetBoat(BoardLetterIndex(E, 7))->GetShipType(), cattleship);
  EXPECT_THAT(board.GetBoat(BoardLetterIndex(F, 7))->GetShipType(), cattleship);
  EXPECT_THAT(board.GetBoat(BoardLetterIndex(G, 7))->GetShipType(), cattleship);
  EXPECT_THAT(board.GetBoat(BoardLetterIndex(H, 7))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 7)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 8)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 9)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 10)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 1)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 2)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 3)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 4)), std::nullopt);
}

TEST(BoardTest, MoveToInvalidLocationDoesntMove) {
  const ShipType cattleship = ShipType{ "Cattleship", 4 };
  Board board(10, 10);

  board.AddBoat(cattleship, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.MoveBoat(cattleship, BoardLetterIndex(C, 7), Orientation::Vertical);
  const bool final_move = board.MoveBoat(cattleship, BoardLetterIndex(A, 10), Orientation::Vertical);

  EXPECT_FALSE(final_move);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 10)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 7))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 8))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 9))->GetShipType(), cattleship);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 10))->GetShipType(), cattleship);
}

class PredefinedPlacementGenerator : public PlacementGenerator {
private:
  Board& board;
  std::queue<Orientation>& orientations;
  std::queue<Location>& locations;

public:
  PredefinedPlacementGenerator(
      Board& board,
      std::queue<Orientation>& orientations,
      std::queue<Location>& locations)
      : board(board), orientations(orientations), locations(locations) {}

  Orientation GenerateOrientation() override {
    Orientation orientation = orientations.front();
    orientations.pop();
    return orientation;
  }

  Location GenerateLocation(const int width, const int height) override {
    if ((width != board.GetWidth()) || (height != board.GetHeight())) {
      throw std::runtime_error("Incorrect board size specified");
    }

    Location location = locations.front();
    locations.pop();
    return location;
  }
};

TEST(BoardTest, AutoPlace) {
  Board board(10, 10);
  std::queue<Orientation> orientations;
  orientations.push(Orientation::Vertical);
  orientations.push(Orientation::Horizontal);
  orientations.push(Orientation::Vertical);
  orientations.push(Orientation::Horizontal);
  orientations.push(Orientation::Vertical);
  std::queue<Location> locations;
  locations.push(BoardLetterIndex(A, 1));
  locations.push(BoardLetterIndex(D, 1));
  locations.push(BoardLetterIndex(B, 1));
  locations.push(BoardLetterIndex(D, 2));
  locations.push(BoardLetterIndex(C, 1));
  PredefinedPlacementGenerator placement_generator(board, orientations, locations);
  AutoPlacer auto_placer(board, placement_generator);
  std::vector<ShipType> boats;
  const ShipType carrier = ShipType{ "Carrier", 5 };
  const ShipType battleship = ShipType{ "Battleship", 4 };
  const ShipType destroyer = ShipType{ "Destroyer", 3 };
  const ShipType submarine = ShipType{ "Submarine", 3 };
  const ShipType patrol = ShipType{ "Patrol Boat", 2 };
  boats.emplace_back(carrier);
  boats.emplace_back(battleship);
  boats.emplace_back(destroyer);
  boats.emplace_back(submarine);
  boats.emplace_back(patrol);

  const bool auto_place_success = auto_placer.AutoPlace(boats);

  EXPECT_TRUE(auto_place_success);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 1)), Boat(carrier, Orientation::Vertical));
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(D, 1)), Boat(battleship, Orientation::Horizontal));
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(B, 1)), Boat(destroyer, Orientation::Vertical));
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(D, 2)), Boat(submarine, Orientation::Horizontal));
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 1)), Boat(patrol, Orientation::Vertical));
}

TEST(BoardTest, AutoPlaceNotEnoughSpaceRetriesButFails) {
  static constexpr int retries = 10000;
  static constexpr int boat_count = 2;
  static constexpr int entries = retries * boat_count;
  Board board(5, 1);
  std::queue<Orientation> orientations;
  for (int index = 0; index < entries; ++index) {
    orientations.push(Orientation::Horizontal);
  }
  std::queue<Location> locations;
  for (int index = 0; index < entries; ++index) {
    locations.push(BoardLetterIndex(A, 1));
  }
  PredefinedPlacementGenerator placement_generator(board, orientations, locations);
  AutoPlacer auto_placer(board, placement_generator);
  std::vector<ShipType> boats;
  boats.emplace_back(ShipType{ "Battleship", 5 });
  boats.emplace_back(ShipType{ "Carrier", 5 });

  const bool auto_place_success = auto_placer.AutoPlace(boats);

  EXPECT_FALSE(auto_place_success);
  EXPECT_EQ(orientations.size(), 0);
  EXPECT_EQ(locations.size(), 0);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 1)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(B, 1)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(C, 1)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(D, 1)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(E, 1)), std::nullopt);
}

TEST(BoardTest, ShootTorpedoes) {
  const ShipType cattleship = ShipType{ "Cattleship", 4 };
  const ShipType battleship = ShipType{ "Battleship", 4 };
  Board board(4, 4);
  board.AddBoat(cattleship, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.AddBoat(battleship, BoardLetterIndex(B, 1), Orientation::Vertical);

  const bool shot1_sucess = board.Shoot(BoardLetterIndex(A, 1));
  const bool shot2_sucess = board.Shoot(BoardLetterIndex(B, 1));
  const bool shot3_sucess = board.Shoot(BoardLetterIndex(C, 1));

  EXPECT_TRUE(shot1_sucess);
  EXPECT_TRUE(board.HasShot(BoardLetterIndex(A, 1)));
  EXPECT_TRUE(board.IsHit(BoardLetterIndex(A, 1)));
  EXPECT_TRUE(shot2_sucess);
  EXPECT_TRUE(board.HasShot(BoardLetterIndex(B, 1)));
  EXPECT_TRUE(board.IsHit(BoardLetterIndex(B, 1)));
  EXPECT_TRUE(shot3_sucess);
  EXPECT_TRUE(board.HasShot(BoardLetterIndex(C, 1)));
  EXPECT_FALSE(board.IsHit(BoardLetterIndex(C, 1)));
  EXPECT_FALSE(board.HasShot(BoardLetterIndex(D, 1)));
  EXPECT_FALSE(board.IsHit(BoardLetterIndex(D, 1)));
}

TEST(BoardTest, ShootPreviouslyTargetedLocationInvalid) {
  Board board(4, 4);

  board.Shoot(BoardLetterIndex(A, 1));
  const bool duplicate_shot_success = board.Shoot(BoardLetterIndex(A, 1));

  EXPECT_FALSE(duplicate_shot_success);
}

TEST(BoardTest, ShootOutOfRangeLocationInvalid) {
  Board board(1, 1);

  const bool shot_success = board.Shoot(BoardLetterIndex(B, 2));

  EXPECT_FALSE(shot_success);
}

TEST(BoardTest, NotFiredLocationNotHit) {
  Board board(10, 10);
  board.AddBoat(ShipType{ "Cattleship", 5 }, BoardLetterIndex(A, 1), Orientation::Horizontal);

  EXPECT_FALSE(board.IsHit(BoardLetterIndex(A, 1)));
}

TEST(BoardTest, AllShipsSunk) {
  Board board(10, 10);
  board.AddBoat(ShipType{ "Cattleship", 3 }, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.AddBoat(ShipType{ "Patrol Boat", 2 }, BoardLetterIndex(B, 1), Orientation::Vertical);

  board.Shoot(BoardLetterIndex(A, 1));
  board.Shoot(BoardLetterIndex(A, 2));
  board.Shoot(BoardLetterIndex(A, 3));
  board.Shoot(BoardLetterIndex(B, 1));
  board.Shoot(BoardLetterIndex(B, 2));

  EXPECT_TRUE(board.AreAllShipsSunk());
}

TEST(BoardTest, AllShipsNotSunk) {
  Board board(10, 10);
  board.AddBoat(ShipType{ "Cattleship", 3 }, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.Shoot(BoardLetterIndex(A, 1));
  board.Shoot(BoardLetterIndex(A, 2));
  board.Shoot(BoardLetterIndex(B, 3));

  EXPECT_FALSE(board.AreAllShipsSunk());
}

TEST(BoardTest, ResetBoard) {
  Board board(4, 1);
  board.AddBoat(ShipType{ "Cattleship", 4 }, BoardLetterIndex(A, 1), Orientation::Horizontal);

  board.Reset();

  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 1)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 2)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 3)), std::nullopt);
  EXPECT_EQ(board.GetBoat(BoardLetterIndex(A, 4)), std::nullopt);
}

TEST(BoardTest, MoveShipSafeAfterResetting) {
  Board board(4, 1);
  board.AddBoat(ShipType{ "Cattleship", 4 }, BoardLetterIndex(A, 1), Orientation::Horizontal);

  board.Reset();
  board.MoveBoat(ShipType{ "Cattleship", 4 }, BoardLetterIndex(A, 1), Orientation::Horizontal);
}

TEST(BoardTest, NotFiredLocations) {
  Board board(2, 2);
  board.Shoot(BoardLetterIndex(A, 1));
  board.Shoot(BoardLetterIndex(A, 2));

  const std::vector<Location> not_fired_locations = board.NotFiredLocations();

  EXPECT_THAT(not_fired_locations, UnorderedElementsAre(BoardLetterIndex(B, 1),
                                                        BoardLetterIndex(B, 2)));
}

TEST(BoardTest, RemainingShips) {
  const ShipType cattleship = ShipType{ "Cattleship", 5 };
  const ShipType battleship = ShipType{ "Battleship", 4 };
  const ShipType other = ShipType{ "Other Ship", 3 };
  const ShipType patrol = ShipType{ "Patrol Boat", 2 };
  Board board(4, 5);
  board.AddBoat(cattleship, BoardLetterIndex(A, 1), Orientation::Vertical);
  board.AddBoat(battleship, BoardLetterIndex(B, 1), Orientation::Vertical);
  board.AddBoat(other, BoardLetterIndex(C, 1), Orientation::Vertical);
  board.AddBoat(patrol, BoardLetterIndex(D, 1), Orientation::Vertical);
  board.Shoot(BoardLetterIndex(A, 1));
  board.Shoot(BoardLetterIndex(A, 2));
  board.Shoot(BoardLetterIndex(A, 3));
  board.Shoot(BoardLetterIndex(A, 4));
  board.Shoot(BoardLetterIndex(A, 5));
  board.Shoot(BoardLetterIndex(B, 1));
  board.Shoot(BoardLetterIndex(B, 2));
  board.Shoot(BoardLetterIndex(B, 3));
  board.Shoot(BoardLetterIndex(B, 4));
  board.Shoot(BoardLetterIndex(D, 1));

  const std::vector<ShipType> remaining_ships = board.GetRemainingShips();

  EXPECT_THAT(remaining_ships, UnorderedElementsAre(other, patrol));
}

TEST(BoardTest, ShootMineExplodesAdjacentShips) {
}
