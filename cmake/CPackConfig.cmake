if(PROJECT_IS_TOP_LEVEL)

  # The name of the package <PROJECT_NAME>-<PROJECT_VERSION>.tar.gz
  set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

  # The location of LICENSE
  set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

  # The location of README
  set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")

  # Bundle the package into a compressed tarball
  set(CPACK_GENERATOR "TGZ")

  # Generate SHA256 for users to verify the integrity of the package
  set(CPACK_PACKAGE_CHECKSUM "SHA256")

  # Install LICENSE and README into the package
  install(FILES
    "${PROJECT_SOURCE_DIR}/LICENSE"
    "${PROJECT_SOURCE_DIR}/README.md"
    DESTINATION .
  )

  # CPack helps
  include(CPack)

endif()
