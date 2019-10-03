#define BOOST_TEST_MODULE chainrocks_test

#include <boost/test/unit_test.hpp>

#include <chainrocks/database.hpp>

/**
 * Test data.
 */
static const std::vector<uint8_t> keys0[10]  { {'a'}, {'b'}, {'c'}, {'d'}, {'e'}, {'f'}, {'g'}, {'h'}, {'i'}, {'j'}};
static const std::vector<uint8_t> keys1[10]  { {'k'}, {'l'}, {'m'}, {'n'}, {'o'}, {'p'}, {'q'}, {'r'}, {'s'}, {'t'}};
static const std::vector<uint8_t> values0[10]{ {'A'}, {'B'}, {'C'}, {'D'}, {'E'}, {'F'}, {'G'}, {'H'}, {'I'}, {'J'}};
static const std::vector<uint8_t> values1[10]{ {'K'}, {'L'}, {'M'}, {'N'}, {'O'}, {'P'}, {'Q'}, {'R'}, {'S'}, {'T'}};

/**
 * Fixture to set up and tear down per test.
 */
struct database_fixture {
   database_fixture()
      : _db{boost::filesystem::unique_path()}
   {
   }

   chainrocks::database _db;
};

template <typename Map>
bool vector_compare (Map const &lhs, Map const &rhs) {
   return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

void compare(const std::map<std::string,std::string>& correct_vec, const std::map<std::string,std::string>& database_data_vec) {
   BOOST_TEST_REQUIRE( vector_compare(correct_vec, database_data_vec) );
}

/**
 * Testing `database` functionality:
 * Creating/modifying/removing.
 */

/**
 * Test #1:
 * `start undo session`
 * Fill `_db` with new values
 * `undo`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j 
 * _db:
 */
BOOST_FIXTURE_TEST_CASE(test_one, database_fixture) {
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   _db.start_undo_session(true);
   for (size_t i{0}; i < 10; ++i) {
      _db.put_batch(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   _db.undo();
   _db.write_batch();

   // _state:
   _db.print_state();
   // compare(std::map<std::string,std::string>{}, _db.get_as_map());
}

/**
 * Test #2:
 *
 * Pre-fill `_state`
 * `start_undo_session`
 * Fill `_state` with new values
 * `undo`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j 
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j K:k L:l M:m N:n O:o P:p Q:q R:r S:s T:t
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j 
 */
// BOOST_FIXTURE_TEST_CASE(test_two, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    for (size_t i{}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys1[i], values1[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j 10k 11l 12m 13n 14o 15p 16q 17r 18s 19t
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{ 0ULL,"a"},{ 1ULL,"b"},{ 2ULL,"c"},{ 3ULL,"d"},{ 4ULL,"e"},
//                                                                                { 5ULL,"f"},{ 6ULL,"g"},{ 7ULL,"h"},{ 8ULL,"i"},{ 9ULL,"j"},
//                                                                                {10ULL,"k"},{11ULL,"l"},{12ULL,"m"},{13ULL,"n"},{14ULL,"o"},
//                                                                                {15ULL,"p"},{16ULL,"q"},{17ULL,"r"},{18ULL,"s"},{19ULL,"t"}}) );

//    _db.undo();

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
// }

/**
 * Test #3:
 *
 * Pre-fill `_state`
 * `start_undo_session`
 * Modify `_state`
 * `undo`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j 
 * _db: A:k B:l C:m D:n E:o F:p G:q H:r I:s J:t 
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j 
 */
// BOOST_FIXTURE_TEST_CASE(test_three, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    for (size_t i{}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0k 1l 2m 3n 4o 5p 6q 7r 8s 9t
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values1[i]);
//    }

//    // _state: 0k 1l 2m 3n 4o 5p 6q 7r 8s 9t
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"k"},{1ULL,"l"},{2ULL,"m"},{3ULL,"n"},{4ULL,"o"},
//                                                                                {5ULL,"p"},{6ULL,"q"},{7ULL,"r"},{8ULL,"s"},{9ULL,"t"}}) );

//    _db.undo();

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
// }

/**
 * Test #4:
 *
 * Pre-fill `_state`
 * `start_undo_session`
 * Remove some `_state`
 * `undo`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j 
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j 
 */
// BOOST_FIXTURE_TEST_CASE(test_four, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    for (size_t i{}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.remove(keys0[i]);
//    }

//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    _db.undo();

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
// }

/**
 * Test #5:
 *
 * `start_undo_session`
 * Fill `_state` with new values
 * `start_undo_session`
 * Fill `_state` with more new values
 * `undo_all`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j K:k L:l M:m N:n O:o P:p Q:q R:r S:s T:t
 * _db:
 */
// BOOST_FIXTURE_TEST_CASE(test_five, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    auto session0{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session1{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys1[i], values1[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j 10k 11l 12m 13n 14o 15p 16q 17r 18s 19t
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{ 0ULL,"a"},{ 1ULL,"b"},{ 2ULL,"c"},{ 3ULL,"d"},{ 4ULL,"e"},
//                                                                                { 5ULL,"f"},{ 6ULL,"g"},{ 7ULL,"h"},{ 8ULL,"i"},{ 9ULL,"j"},
//                                                                                {10ULL,"k"},{11ULL,"l"},{12ULL,"m"},{13ULL,"n"},{14ULL,"o"},
//                                                                                {15ULL,"p"},{16ULL,"q"},{17ULL,"r"},{18ULL,"s"},{19ULL,"t"}}) );

//    _db.undo_all();

//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );
// }

/**
 * Testing `database` functionality:
 * Squash tests.
 */

/**
 * Test #6:
 *
 * `start_undo_session`
 * Add some keys
 * `start_undo_session`
 * Add some keys
 * `squash`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j K:k L:l M:m N:n O:o P:p Q:q R:r S:s T:t
 * _new_keys<0>: A B C D E F G H I J
 * _new_keys<1>: K L M N O P Q R S T
 * _new_keys<0>: A B C D E F G H I J K L M N O P Q R S T
 */
// BOOST_FIXTURE_TEST_CASE(test_six, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    auto session0{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session1{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys1[i], values1[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j 10k 11l 12m 13n 14o 15p 16q 17r 18s 19t
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{ 0ULL,"a"},{ 1ULL,"b"},{ 2ULL,"c"},{ 3ULL,"d"},{ 4ULL,"e"},
//                                                                                { 5ULL,"f"},{ 6ULL,"g"},{ 7ULL,"h"},{ 8ULL,"i"},{ 9ULL,"j"},
//                                                                                {10ULL,"k"},{11ULL,"l"},{12ULL,"m"},{13ULL,"n"},{14ULL,"o"},
//                                                                                {15ULL,"p"},{16ULL,"q"},{17ULL,"r"},{18ULL,"s"},{19ULL,"t"}}) );

//    // new_keys: 0 1 2 3 4 5 6 7 8 9
//    // new_keys: 10 11 12 13 14 15 16 17 18 19
//    _db.print_keys();
//    BOOST_TEST_REQUIRE( (_db.stack()[_db.stack().size()-2]._new_keys) == (std::set<uint64_t>{0ULL,1ULL,2ULL,3ULL,4ULL,5ULL,6ULL,7ULL,8ULL,9ULL}) );

//    _db.squash();

//    // new_keys: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19
//    _db.print_keys();
//    BOOST_TEST_REQUIRE( (_db.stack().back()._new_keys) == (std::set<uint64_t>{ 0ULL, 1ULL, 2ULL, 3ULL, 4ULL, 5ULL, 6ULL, 7ULL, 8ULL, 9ULL,
//                                                                                    10ULL,11ULL,12ULL,13ULL,14ULL,15ULL,16ULL,17ULL,18ULL,19ULL}) );
// }

/**
 * Test #7:
 *
 * `start_undo_session`
 * Add some keys
 * `start_undo_session`
 * Modify some keys
 * `squash`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db: A:k B:l C:m D:n E:o F:f G:g H:h I:i J:j
 * _db: A:k B:l C:m D:n E:o F:f G:g H:h I:i J:j
 */
// BOOST_FIXTURE_TEST_CASE(test_seven, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    auto session0{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session1{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 5; ++i) {
//       _db.put(keys0[i], values1[i]);
//    }

//    // _state: 0k 1l 2m 3n 4o 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"k"},{1ULL,"l"},{2ULL,"m"},{3ULL,"n"},{4ULL,"o"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    _db.squash();

//    // _state: 0k 1l 2m 3n 4o 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"k"},{1ULL,"l"},{2ULL,"m"},{3ULL,"n"},{4ULL,"o"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
// }

/**
 * Test #8:
 *
 * Pre-filled state
 * `start_undo_session`
 * Remove some `_state`
 * `start_undo_session`
 * Remove some `_state`
 * `squash`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db: F:f G:g H:h I:i J:j
 * _db: J:j
 * _db: J:j
 */
// BOOST_FIXTURE_TEST_CASE(test_eight, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    for (size_t i{}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session0{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 5; ++i) {
//       _db.remove(keys0[i]);
//    }

//    // _state: 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session1{_db.start_undo_session(true)};
//    for (size_t i{5}; i < 9; ++i) {
//       _db.remove(keys0[i]);
//    }

//    // _state: 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{9ULL,"j"}}) );

//    _db.squash();

//    // _state: 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{9ULL,"j"}}) );
// }

/**
 * Testing `database` functionality:
 * RAII/push tests.
 */

/**
 * Test #9:
 *
 * Initiate RAII `{`
 * `start_undo_session`
 * Fill `_state` with new values
 * End RAII `}`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db:
 */
// BOOST_FIXTURE_TEST_CASE(test_nine, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    {
//    auto session{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
//    }

//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );
// }

/**
 * Test #10:
 *
 * `start_undo_session`
 * Initiate RAII `{`
 * Fill `_state` with new values
 * End RAII `}`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 */
// BOOST_FIXTURE_TEST_CASE(test_ten, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    auto session{_db.start_undo_session(true)};
//    {
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
// }

/**
 * Test #11:
 *
 * Initiate RAII `{`
 * `start_undo_session`
 * Fill `_state` with new values
 * `push`
 * End RAII `}`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 */
// BOOST_FIXTURE_TEST_CASE(test_eleven, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    {
//    auto session{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
//    session.push();
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
// }

/**
 * Test #12:
 *
 * Initiate RAII `{`
 * `start_undo_session`
 * Fill `_state` with new values
 * `push`
 * End RAII `}`
 *
 * _db:
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 * _db: A:a B:b C:c D:d E:e F:f G:g H:h I:i J:j
 */
// BOOST_FIXTURE_TEST_CASE(test_twelve, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    {
//    auto session{_db.start_undo_session(true)};
//    for (size_t i{0}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
//    session.push();
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    _db.undo();

//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );
// }

// /**
//  * Other tests.
//  */

// /**
//  * Test #13:
//  *
//  * Pre-fill state
//  * `start_undo_session`
//  * Remove `_state` 0a
//  * `start_undo_session`
//  * Put `_state` 0a
//  * `squash`
//  * Remove some `_state`
//  */
// BOOST_FIXTURE_TEST_CASE(test_thirteen, database_fixture) {
//    // _state:
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{}) );

//    for (size_t i{}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session0{_db.start_undo_session(true)};
//    _db.remove(keys0[0]);

//    // _state: 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"}, {5ULL,"f"},
//                                                                                {6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    auto session1{_db.start_undo_session(true)};
//    _db.put(keys0[0], values0[0]);

//    // _state: 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{0ULL,"a"},{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"},
//                                                                                {5ULL,"f"},{6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );

//    _db.squash();

//    // Under my previous implementation this would throw, which is incorrect behavior.
//    // Now, under the fixed implementation, this should now throw.
//    _db.remove(keys0[0]);

//    // _state: 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    _db.print_state();
//    BOOST_TEST_REQUIRE( (_db.state()) == (std::map<uint64_t, std::string>{{1ULL,"b"},{2ULL,"c"},{3ULL,"d"},{4ULL,"e"}, {5ULL,"f"},
//                                                                                {6ULL,"g"},{7ULL,"h"},{8ULL,"i"},{9ULL,"j"}}) );
   
// BOOST_AUTO_TEST_SUITE_END()







// /// Test data.
// static const uint64_t keys0[10]{ 0ULL, 1ULL, 2ULL, 3ULL, 4ULL, 5ULL, 6ULL, 7ULL, 8ULL, 9ULL};
// static const std::string values0[10]{"a","b","c","d","e","f","g","h","i","j"};

// ///////////////////////////////////
// /// Testing `rocksdb` functionality

// BOOST_FIXTURE_TEST_CASE(test_one, database_fixture) {
//    // _state:
//    database.print_state();
//    for (size_t i{0}; i < 10; ++i) {
//       database.rocksdb_put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    database.print_state();

//    for (size_t i{}; i < 10; ++i) {
//       BOOST_TEST_REQUIRE( (database.rocksdb_does_key_exist(keys0[i])) == (true) );
//    }
// }

// BOOST_FIXTURE_TEST_CASE(test_two, database_fixture) {
//    // _state:
//    database.print_state();
//    for (size_t i{0}; i < 10; ++i) {
//       database.rocksdb_put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    database.print_state();

//    for (size_t i{0}; i < 10; ++i) {
//       database.rocksdb_remove(keys0[i]);
//    }

//    // _state:
//    database.print_state();

//    for (size_t i{}; i < 10; ++i) {
//       BOOST_TEST_REQUIRE( (database.rocksdb_does_key_exist(keys0[i])) == (false) );
//    }
// }

// BOOST_FIXTURE_TEST_CASE(test_three, database_fixture) {
//    // _state:
//    database.print_state();
//    for (size_t i{0}; i < 10; ++i) {
//       database.rocksdb_put(keys0[i], values0[i]);
//    }

//    // _state: 0a 1b 2c 3d 4e 5f 6g 7h 8i 9j
//    database.print_state();

//    {
//    std::string value;
//    for (size_t i{}; i < 10; ++i) {
//       database.rocksdb_get(keys0[i], value);
//       BOOST_TEST_REQUIRE( (value) == (values0[i]) );
//    }
//    }

//    for (size_t i{0}; i < 10; ++i) {
//       database.rocksdb_remove(keys0[i]);
//    }

//    // _state:
//    database.print_state();

//    // Note that according to the current implementation the string
//    // "Encountered error: 1" will print out due to the checking of the
//    // internal `_status` code when performing a `Get` in rocksDB; in
//    // this case the `_status` code is a 1 because the value will not
//    // be found in the database.
//    {
//    std::string value;
//    for (size_t i{}; i < 10; ++i) {
//       database.rocksdb_get(keys0[i], value);
//       BOOST_TEST_REQUIRE( (value) == ("") );
//    }
//    }
// BOOST_AUTO_TEST_SUITE_END()
