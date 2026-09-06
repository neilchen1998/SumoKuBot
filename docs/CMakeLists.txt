set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

doxygen_add_docs(docs
    ${PROJECT_SOURCE_DIR}/README.md
    ${PROJECT_SOURCE_DIR}/benchmarks
    ${PROJECT_SOURCE_DIR}/modules/board
    ${PROJECT_SOURCE_DIR}/modules/loader
    ${PROJECT_SOURCE_DIR}/modules/math
    ${PROJECT_SOURCE_DIR}/modules/solvers/sudoku
    ${PROJECT_SOURCE_DIR}/modules/solvers/sumoku
    ${PROJECT_SOURCE_DIR}/tests
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
