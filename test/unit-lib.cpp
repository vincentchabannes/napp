
#include "lib1.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Test Lib 1", "[lib]" ) {
    REQUIRE( test1a(_first_name="James",_last_name="Bond") == "James Bond" );
    REQUIRE( test1a(_last_name="Bond",_first_name="James") == "James Bond" );

    REQUIRE( test1b(_last_name="Bond",_first_name="James") == "James Bond" );
    REQUIRE( test1b(_first_name="James") == "James default_last_name" );
    REQUIRE( test1b(_last_name="Bond") == "default_first_name Bond" );
    REQUIRE( test1b() == "default_first_name default_last_name" );
}
