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
    const SudokuPuzzleData sudoku {.N = 9,
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

    return sudoku;
}

SudokuPuzzleData MakeInvalidSudoku()
{

    SudokuPuzzleData sudoku {.N = 9,
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

    return sudoku;
}

SumokuPuzzleData MakeValidSumoku()
{
    SumokuPuzzleData sumoku {.N = 3,
                             .boxes = {{{.x = 0, .y = 0}, {.x = 1, .y = 0}},
                                       {{.x = 0, .y = 1}, {.x = 0, .y = 2}},
                                       {{.x = 1, .y = 1}, {.x = 1, .y = 2}},
                                       {{.x = 2, .y = 0}, {.x = 2, .y = 1}},
                                       {{.x = 2, .y = 2}}},
                             .sums = {3, 5, 4, 5, 1},
                             .label = "valid"};

    return sumoku;
}

SumokuPuzzleData MakeInvalidSumoku()
{

    SumokuPuzzleData sumoku {.N = 3,
                             .boxes = {{{.x = 0, .y = 0}, {.x = 1, .y = 0}},
                                       {{.x = 0, .y = 1}, {.x = 0, .y = 2}},
                                       {{.x = 1, .y = 1}, {.x = 1, .y = 2}},
                                       {{.x = 2, .y = 0}, {.x = 2, .y = 1}},
                                       {{.x = 2, .y = 2}}},
                             .sums = {3, 5, 5, 5, 1},
                             .label = "invalid"};

    return sumoku;
}

struct TempDirectory
{
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

TEST_CASE("ValidateSumokuPuzzle", "[ValidateSumokuPuzzle]")
{
    SumokuPuzzleData sumoku = MakeValidSumoku();
    sumoku.label = "test";

    SECTION("Valid puzzles")
    {
        const SumokuPuzzleData puzzle = MakeValidSumoku();

        const auto result = ValidateSumokuPuzzle(puzzle);

        REQUIRE(result.has_value());
    }

    SECTION("Invalid puzzles - N is 0")
    {
        sumoku.N = 0;

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - the number of boxes and sums does not match")
    {
        sumoku.sums = {3, 5, 4}; // missing 2 sums

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - incorrect sums")
    {
        sumoku.sums = {3, 10, 4, 5, 1}; // the second sum is incorrect

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - elements out of bound")
    {
        sumoku.boxes = {{{.x = 10, .y = 0}, {.x = 1, .y = 0}},
                        {{.x = 0, .y = 1}, {.x = 0, .y = 2}},
                        {{.x = 1, .y = 1}, {.x = 1, .y = 2}},
                        {{.x = 2, .y = 0}, {.x = 2, .y = 1}},
                        {{.x = 2, .y = 2}}};

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - boxes appear twice")
    {
        sumoku.boxes = {{{.x = 0, .y = 0}, {.x = 1, .y = 0}},
                        {{.x = 0, .y = 0}, {.x = 0, .y = 2}},
                        {{.x = 1, .y = 1}, {.x = 1, .y = 2}},
                        {{.x = 2, .y = 0}, {.x = 2, .y = 1}},
                        {{.x = 2, .y = 2}}};

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - empty boxes")
    {
        sumoku.boxes = {};

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - empty sums")
    {
        sumoku.sums = {};

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - empty sums")
    {
        sumoku.sums = {};

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - contraints contract with each others")
    {
        sumoku.boxes = {{{.x = 0, .y = 0}, {.x = 1, .y = 0}},
                        {{.x = 0, .y = 1}, {.x = 0, .y = 2}},
                        {{.x = 1, .y = 1}, {.x = 1, .y = 2}},
                        {{.x = 2, .y = 0}, {.x = 2, .y = 1}},
                        {{.x = 2, .y = 2}},
                        {{.x = 3, .y = 3}}};

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }

    SECTION("Invalid puzzles - contraints contract with each others")
    {
        const SumokuPuzzleData sumoku {.N = 3,
                                       .boxes = {{{.x = 0, .y = 0}, {.x = 1, .y = 0}},
                                                 {{.x = 0, .y = 1}, {.x = 0, .y = 2}},
                                                 {{.x = 1, .y = 1}, {.x = 1, .y = 2}},
                                                 {{.x = 2, .y = 0}, {.x = 2, .y = 1}},
                                                 {{.x = 2, .y = 2}}},
                                       .sums = {3, 5, 5, 5, 1},
                                       .label = "invalid"};

        const auto result = ValidateSumokuPuzzle(sumoku);

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().empty());
    }
}

TEST_CASE("PuzzleTraits", "[PuzzleTraits][Sudoku][Sumoku]")
{
    SECTION("Valid Sudoku puzzles")
    {
        const auto valid = MakeValidSudoku();

        REQUIRE(PuzzleTraits<SudokuPuzzleData>::validate(valid).has_value());
        REQUIRE(PuzzleTraits<SudokuPuzzleData>::label(valid) == "valid");
    }

    SECTION("Invalid Sudoku puzzles")
    {
        const auto invalid = MakeInvalidSudoku();

        REQUIRE_FALSE(PuzzleTraits<SudokuPuzzleData>::validate(invalid).has_value());
    }

    SECTION("Valid Sumoku puzzles")
    {
        const auto valid = MakeValidSumoku();

        REQUIRE(PuzzleTraits<SumokuPuzzleData>::validate(valid).has_value());
        REQUIRE(PuzzleTraits<SumokuPuzzleData>::label(valid) == "valid");
    }

    SECTION("Invalid Sumoku puzzles")
    {
        const auto invalid = MakeInvalidSumoku();

        REQUIRE_FALSE(PuzzleTraits<SumokuPuzzleData>::validate(invalid).has_value());
    }
}

TEST_CASE("LoadPuzzle returns nullopt for a nonexistent file", "[LoadPuzzle]")
{
    TempDirectory temp;

    SECTION("Non-existed path")
    {
        const auto puzzle1 = LoadPuzzle<SudokuPuzzleData>("/this/file/does/not/exist.json");
        const auto puzzle2 = LoadPuzzle<SumokuPuzzleData>("/this/file/does/not/exist.json");

        REQUIRE_FALSE(puzzle1.has_value());
        REQUIRE_FALSE(puzzle2.has_value());
    }

    SECTION("Valid puzzles", "[LoadPuzzle][Sudoku]")
    {
        auto expected = MakeValidSudoku();
        expected.label = "valid sudoku";

        const auto path = temp.path_ / "valid.json";
        WriteJson(path, expected);

        const auto actual = LoadPuzzle<SudokuPuzzleData>(path.string());

        REQUIRE(actual.has_value());
        REQUIRE(actual->label == expected.label);
    }

    SECTION("Valid puzzles", "[LoadPuzzle][Sudoku]")
    {
        auto expected = MakeValidSumoku();
        expected.label = "valid sumoku";

        const auto path = temp.path_ / "valid.json";
        WriteJson(path, expected);

        const auto actual = LoadPuzzle<SumokuPuzzleData>(path.string());

        REQUIRE(actual.has_value());
        REQUIRE(actual->label == expected.label);
    }

    SECTION("Invalid puzzles", "[LoadPuzzle][Sudoku]")
    {
        auto sudoku = MakeInvalidSudoku();
        sudoku.label = "invalid sudoku";

        const fs::path path = temp.path_ / "invalid.json";
        WriteJson(path, sudoku);

        const auto result = LoadPuzzle<SudokuPuzzleData>(path.string());

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Invalid puzzles", "[LoadPuzzle][Sumoku]")
    {
        auto sumoku = MakeInvalidSumoku();
        sumoku.label = "invalid sumoku";

        const fs::path path = temp.path_ / "invalid.json";
        WriteJson(path, sumoku);

        const auto result = LoadPuzzle<SumokuPuzzleData>(path.string());

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Malformed JSON files", "[LoadPuzzle]")
    {
        const fs::path path = temp.path_ / "malformed.json";

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
        const auto path = temp.path_ / "wrong_structure.json";

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

        WriteJson(temp.path_ / "first.json", first);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path_.string());

        REQUIRE(puzzles.size() == 1);
    }

    SECTION("Two JSON files")
    {
        auto first = MakeValidSudoku();
        first.label = "first";

        auto second = MakeValidSudoku();
        second.label = "second";

        WriteJson(temp.path_ / "first.json", first);
        WriteJson(temp.path_ / "second.json", second);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path_.string());

        REQUIRE(puzzles.size() == 2);
    }

    SECTION("Not a directory")
    {
        const auto file = temp.path_ / "not_a_directory.json";

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
        auto sudoku = MakeValidSudoku();
        sudoku.label = "uppercase extension";

        WriteJson(temp.path_ / "puzzle.JSON", sudoku);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path_.string());

        REQUIRE(puzzles.size() == 1);
        REQUIRE(puzzles.front().label == "uppercase extension");
    }

    SECTION("Non-JSON files")
    {
        WriteJson(temp.path_ / "valid.json", valid);

        {
            std::ofstream textFile(temp.path_ / "ignored.txt");
            REQUIRE(textFile);
            textFile << "not a puzzle";
        }

        {
            std::ofstream cppFile(temp.path_ / "ignored.cpp");
            REQUIRE(cppFile);
            cppFile << "not a puzzle";
        }

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path_.string());

        REQUIRE(puzzles.size() == 1);
        REQUIRE(puzzles.front().label == "valid");
    }

    SECTION("Invalid puzzles")
    {
        WriteJson(temp.path_ / "valid.json", valid);
        WriteJson(temp.path_ / "invalid.json", invalid);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path_.string());

        REQUIRE(puzzles.size() == 1);
        REQUIRE(puzzles.front().label == "valid");
    }

    SECTION("Malformed JSON files")
    {
        WriteJson(temp.path_ / "valid.json", valid);

        {
            std::ofstream malformed(temp.path_ / "malformed.json");
            REQUIRE(malformed);
            malformed << "{ invalid json";
        }

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path_.string());

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
        WriteJson(temp.path_ / "zebra.json", zebra);
        WriteJson(temp.path_ / "apple.json", apple);
        WriteJson(temp.path_ / "banana.json", banana);

        const auto puzzles = LoadAllPuzzles<SudokuPuzzleData>(temp.path_.string());

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
