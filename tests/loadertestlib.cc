#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>         // GENERATE
#include <catch2/catch_test_macros.hpp> // TEST_CASE, SECTION, REQUIRE, REQUIRE_THROWS_AS
#include <filesystem>                   // std::filesystem
#include <fstream>                      // nlohmann::json
#include <nlohmann/json.hpp>            // nlohmann::json
#include <string>                       // std::string

#include "loader/loaderlib.hpp" // ValidateSudokuPuzzle, PuzzleTraits, GetTestDataPath, etc.

namespace fs = std::filesystem;

namespace
{
SudokuPuzzleData MakeValidSudoku()
{
    const SudokuPuzzleData puzzle {.N = 9,
                                   .board = {{5, 3, 0, 0, 7, 0, 0, 0, 0},
                                             {6, 0, 0, 1, 9, 5, 0, 0, 0},
                                             {0, 9, 8, 0, 0, 0, 0, 6, 0},
                                             {8, 0, 0, 0, 6, 0, 0, 0, 3},
                                             {4, 0, 0, 8, 0, 3, 0, 0, 1},
                                             {7, 0, 0, 0, 2, 0, 0, 0, 6},
                                             {0, 6, 0, 0, 0, 0, 2, 8, 0},
                                             {0, 0, 0, 4, 1, 9, 0, 0, 5},
                                             {0, 0, 0, 0, 8, 0, 0, 7, 9}},
                                   .label = "valid"};

    return puzzle;
}

SudokuPuzzleData MakeInvalidSudoku()
{

    SudokuPuzzleData puzzle {.N = 9,
                             .board = {{5, 3, 0, 0, 7, 0, 0, 0, 0},
                                       {6, 0, 0, 1, 9, 5, 0, 0, 0},
                                       {0, 9, 8, 0, 0, 0, 0, 6, 0},
                                       {8, 0, 0, 0, 6, 0, 0, 0, 3},
                                       {4, 0, 0, 8, 0, 3, 0, 0, 1},
                                       {7, 0, 0, 0, 2, 0, 0, 0, 6},
                                       {0, 6, 0, 0, 0, 0, 2, 8, 0},
                                       {0, 0, 0, 4, 1, 9, 0, 0, 5},
                                       {0, 0, 0, 0, 8, 0, 5, 7, 10}},
                             .label = "invalid"};

    return puzzle;
}

class TempDirectory
{
  public:
    TempDirectory()
        : path_(fs::temp_directory_path() / ("puzzle_tests_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
    {
        fs::create_directories(path_);
    }

    ~TempDirectory()
    {
        std::error_code ec;
        fs::remove_all(path_, ec);
    }

    TempDirectory(const TempDirectory&) = delete;
    TempDirectory& operator=(const TempDirectory&) = delete;
    TempDirectory(TempDirectory&& other) noexcept;
    TempDirectory& operator=(TempDirectory&& other) noexcept;

    [[nodiscard]] const fs::path& path() const
    {
        return path_;
    }

  private:
    fs::path path_;
};

template <typename T>
void WriteJson(const fs::path& path, const T& puzzle)
{
    std::ofstream file(path);
    REQUIRE(file);

    nlohmann::json json = puzzle;
    file << json.dump(4);
}

} // namespace

TEST_CASE("ValidateSudokuPuzzle", "[ValidateSudokuPuzzle]")
{
    SECTION("Valid puzzles")
    {
        const SudokuPuzzleData puzzle = MakeValidSudoku();

        const auto result = ValidateSudokuPuzzle(puzzle);

        REQUIRE(result.has_value());
    }

    SECTION("Invalid puzzles")
    {
        const SudokuPuzzleData puzzle = MakeInvalidSudoku();

        const auto result = ValidateSudokuPuzzle(puzzle);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }
}

TEST_CASE("PuzzleTraits", "[PuzzleTraits][Sudoku]")
{
    SECTION("Valid puzzles")
    {
        const auto valid = MakeValidSudoku();

        REQUIRE(PuzzleTraits<SudokuPuzzleData>::validate(valid).has_value());
        REQUIRE(PuzzleTraits<SudokuPuzzleData>::label(valid) == "valid");
    }

    SECTION("Invalid puzzles")
    {
        const auto invalid = MakeInvalidSudoku();

        REQUIRE_FALSE(PuzzleTraits<SudokuPuzzleData>::validate(invalid).has_value());
    }
}

TEST_CASE("LoadPuzzle returns nullopt for a nonexistent file", "[LoadPuzzle]")
{
    TempDirectory temp;

    SECTION("Non-existed path")
    {
        const auto puzzle = LoadPuzzle<SudokuPuzzleData>("/this/file/does/not/exist.json");

        REQUIRE_FALSE(puzzle.has_value());
    }

    SECTION("Valid puzzles", "[LoadPuzzle][Sudoku]")
    {
        auto expected = MakeValidSudoku();
        expected.label = "valid sudoku";

        const auto path = temp.path() / "valid.json";
        WriteJson(path, expected);

        const auto actual = LoadPuzzle<SudokuPuzzleData>(path.string());

        REQUIRE(actual.has_value());
        REQUIRE(actual->label == expected.label);
    }

    SECTION("Invalid puzzles", "[LoadPuzzle][Sudoku]")
    {
        auto puzzle = MakeInvalidSudoku();
        puzzle.label = "invalid sudoku";

        const fs::path path = temp.path() / "invalid.json";
        WriteJson(path, puzzle);

        const auto result = LoadPuzzle<SudokuPuzzleData>(path.string());

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Malformed JSON files", "[LoadPuzzle]")
    {
        const fs::path path = temp.path() / "malformed.json";

        {
            std::ofstream file(path);
            REQUIRE(file);

            file << "{ this is not valid JSON";
        }

        const auto result = LoadPuzzle<SudokuPuzzleData>(path.string());

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Invalid puzzle structure", "[LoadPuzzle]")
    {
        const auto path = temp.path() / "wrong_structure.json";

        {
            std::ofstream file(path);
            REQUIRE(file);

            file << R"({
                "something": "completely different"
            })";
        }

        const auto result = LoadPuzzle<SudokuPuzzleData>(path.string());

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("LoadAllPuzzles returns empty for nonexistent directory", "[LoadAllPuzzles]")
{
    const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>("/does/not/exist");

    REQUIRE(puzzles.empty());
}

TEST_CASE("LoadAllPuzzles returns empty for a file path", "[LoadAllPuzzles]")
{
    TempDirectory temp;

    auto valid = MakeValidSudoku();
    valid.label = "valid";

    auto invalid = MakeInvalidSudoku();
    invalid.label = "invalid";

    SECTION("Single JSON file")
    {
        auto first = MakeValidSudoku();
        first.label = "first";

        WriteJson(temp.path() / "first.json", first);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path().string());

        REQUIRE(puzzles.size() == 1);
    }

    SECTION("Two JSON files")
    {
        auto first = MakeValidSudoku();
        first.label = "first";

        auto second = MakeValidSudoku();
        second.label = "second";

        WriteJson(temp.path() / "first.json", first);
        WriteJson(temp.path() / "second.json", second);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path().string());

        REQUIRE(puzzles.size() == 2);
    }

    SECTION("Not a directory")
    {
        const auto file = temp.path() / "not_a_directory.json";

        {
            std::ofstream output(file);
            REQUIRE(output);
            output << "{}";
        }

        const std::vector<SudokuPuzzleData> puzzles = LoadAllPuzzles<SudokuPuzzleData>(file.string());

        REQUIRE(puzzles.empty());
    }

    SECTION("Files w/ uppercase JSON extension")
    {
        auto puzzle = MakeValidSudoku();
        puzzle.label = "uppercase extension";

        WriteJson(temp.path() / "puzzle.JSON", puzzle);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path().string());

        REQUIRE(puzzles.size() == 1);
        REQUIRE(puzzles.front().label == "uppercase extension");
    }

    SECTION("Non-JSON files")
    {
        WriteJson(temp.path() / "valid.json", valid);

        {
            std::ofstream textFile(temp.path() / "ignored.txt");
            REQUIRE(textFile);
            textFile << "not a puzzle";
        }

        {
            std::ofstream cppFile(temp.path() / "ignored.cpp");
            REQUIRE(cppFile);
            cppFile << "not a puzzle";
        }

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path().string());

        REQUIRE(puzzles.size() == 1);
        REQUIRE(puzzles.front().label == "valid");
    }

    SECTION("Invalid puzzles")
    {
        WriteJson(temp.path() / "valid.json", valid);
        WriteJson(temp.path() / "invalid.json", invalid);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path().string());

        REQUIRE(puzzles.size() == 1);
        REQUIRE(puzzles.front().label == "valid");
    }

    SECTION("Malformed JSON files")
    {
        WriteJson(temp.path() / "valid.json", valid);

        {
            std::ofstream malformed(temp.path() / "malformed.json");
            REQUIRE(malformed);
            malformed << "{ invalid json";
        }

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path().string());

        REQUIRE(puzzles.size() == 1);
        REQUIRE(puzzles.front().label == "valid");
    }

    SECTION("LoadAllPuzzles sorts puzzles by label")
    {
        auto zebra = MakeValidSudoku();
        zebra.label = "zebra";

        auto apple = MakeValidSudoku();
        apple.label = "apple";

        auto banana = MakeValidSudoku();
        banana.label = "banana";

        // Write them in a non-sorted order
        WriteJson(temp.path() / "zebra.json", zebra);
        WriteJson(temp.path() / "apple.json", apple);
        WriteJson(temp.path() / "banana.json", banana);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path().string());

        REQUIRE(puzzles.size() == 3);

        REQUIRE(puzzles[0].label == "apple");
        REQUIRE(puzzles[1].label == "banana");
        REQUIRE(puzzles[2].label == "zebra");
    }
}

TEST_CASE("GetTestDataPath", "[GetTestDataPath]")
{
    SECTION("Valid path")
    {
        const auto path = GetTestDataPath();

        REQUIRE_FALSE(path.empty());
        REQUIRE(fs::exists(path));
        REQUIRE(fs::is_directory(path));
    }
}
