add_test([=[UserTest.AuthenticationAndRolePermissions]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=UserTest.AuthenticationAndRolePermissions]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UserTest.AuthenticationAndRolePermissions]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:323]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[UserTest.RegistrationValidationRejectsEmptyOrDuplicateUsername]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=UserTest.RegistrationValidationRejectsEmptyOrDuplicateUsername]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[UserTest.RegistrationValidationRejectsEmptyOrDuplicateUsername]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:343]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[BookTest.BorrowAndReturnUpdatesAvailableCopies]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=BookTest.BorrowAndReturnUpdatesAvailableCopies]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[BookTest.BorrowAndReturnUpdatesAvailableCopies]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:353]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[BookTest.OutOfStockBookCannotBeBorrowedAndSearchMatchesCaseInsensitive]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=BookTest.OutOfStockBookCannotBeBorrowedAndSearchMatchesCaseInsensitive]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[BookTest.OutOfStockBookCannotBeBorrowedAndSearchMatchesCaseInsensitive]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:364]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[FineTest.OverdueDaysAndFineCalculationBoundaries]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=FineTest.OverdueDaysAndFineCalculationBoundaries]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[FineTest.OverdueDaysAndFineCalculationBoundaries]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:378]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[BorrowTransactionTest.ReturnOverdueBookCalculatesFineAndMarksFinePaid]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=BorrowTransactionTest.ReturnOverdueBookCalculatesFineAndMarksFinePaid]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[BorrowTransactionTest.ReturnOverdueBookCalculatesFineAndMarksFinePaid]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:393]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[ReservationTest.QueueEligibilityAndCancellation]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=ReservationTest.QueueEligibilityAndCancellation]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[ReservationTest.QueueEligibilityAndCancellation]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:413]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
add_test([=[NotificationTest.BuildsExpectedPickupAndOverdueMessages]=]  C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe [==[--gtest_filter=NotificationTest.BuildsExpectedPickupAndOverdueMessages]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[NotificationTest.BuildsExpectedPickupAndOverdueMessages]=]
  PROPERTIES
    
    _CMAKE_TEST_BUILD_DEPENDS [==[C:/LibraryManagementApp/Code/tests/build/run_unit_tests.exe]==]
    DEF_SOURCE_LINE [==[C:\LibraryManagementApp\Code\tests\test_core_methods.cpp:432]==]
    WORKING_DIRECTORY [==[C:/LibraryManagementApp/Code/tests/build]==]
    SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==]
    
)
set(run_unit_tests_TESTS [==[UserTest.AuthenticationAndRolePermissions]==] [==[UserTest.RegistrationValidationRejectsEmptyOrDuplicateUsername]==] [==[BookTest.BorrowAndReturnUpdatesAvailableCopies]==] [==[BookTest.OutOfStockBookCannotBeBorrowedAndSearchMatchesCaseInsensitive]==] [==[FineTest.OverdueDaysAndFineCalculationBoundaries]==] [==[BorrowTransactionTest.ReturnOverdueBookCalculatesFineAndMarksFinePaid]==] [==[ReservationTest.QueueEligibilityAndCancellation]==] [==[NotificationTest.BuildsExpectedPickupAndOverdueMessages]==])
