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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
 * _db:
 */
BOOST_FIXTURE_TEST_CASE(test_one, database_fixture) {
   std::cout << "TEST_ONE\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   auto session{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   _db.undo();
   _db.write_batch();

   // _state:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());
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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J k:K l:L m:M n:N o:O p:P q:Q r:R s:S t:T
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
 */
BOOST_FIXTURE_TEST_CASE(test_two, database_fixture) {
   std::cout << "TEST_TWO\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   for (size_t i{}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys1[i], values1[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J k:K l:L m:M n:N o:O p:P q:Q r:R s:S t:T
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"},
                                             {"k","K"},{"l","L"},{"m","M"},{"n","N"},{"o","O"},
                                             {"p","P"},{"q","Q"},{"r","R"},{"s","S"},{"t","T"}}, _db.get_as_map());

   _db.undo();
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
}

/**
 * Test #3:
 *
 * Pre-fill `_state`
 * `start_undo_session`
 * Modify `_state`
 * `undo`
 *
 * _db:
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
 * _db: k:A l:B m:C n:D o:E p:F q:G r:H s:I t:J 
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
 */
BOOST_FIXTURE_TEST_CASE(test_three, database_fixture) {
   std::cout << "TEST_THREE\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   for (size_t i{}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values1[i]);
   }
   _db.write_batch();

   // _db: k:A l:B m:C n:D o:E p:F q:G r:H s:I t:J 
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","K"},{"b","L"},{"c","M"},{"d","N"},{"e","O"},
                                             {"f","P"},{"g","Q"},{"h","R"},{"i","S"},{"j","T"}}, _db.get_as_map());

   _db.undo();
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
}

/**
 * Test #4:
 *
 * Pre-fill `_state`
 * `start_undo_session`
 * Remove some `_state`
 * `undo`
 *
 * _db:
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
 * _db:
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
 */
BOOST_FIXTURE_TEST_CASE(test_four, database_fixture) {
   std::cout << "TEST_FOUR\n";

   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   for (size_t i{}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.remove(keys0[i]);
   }
   _db.write_batch();

   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   _db.undo();
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J 
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
}

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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J k:K l:L m:M n:N o:O p:P q:Q r:R s:S t:T
 * _db:
 */
BOOST_FIXTURE_TEST_CASE(test_five, database_fixture) {
   std::cout << "TEST_FIVE\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   auto session0{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session1{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys1[i], values1[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J k:K l:L m:M n:N o:O p:P q:Q r:R s:S t:T
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"},
                                             {"k","K"},{"l","L"},{"m","M"},{"n","N"},{"o","O"},
                                             {"p","P"},{"q","Q"},{"r","R"},{"s","S"},{"t","T"}}, _db.get_as_map());

   _db.undo_all();
   _db.write_batch();

   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());
}

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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J k:K l:L m:M n:N o:O p:P q:Q r:R s:S t:T
 * _db._stack._new_keys<0>: a b c d e f g h i j
 * _db._stack._new_keys<1>: k l m n o p q r s t
 * _db._stack._new_keys<0>: a b c d e f g h i j k l m n o p q r s t
 */
BOOST_FIXTURE_TEST_CASE(test_six, database_fixture) {
   std::cout << "TEST_SIX\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   auto session0{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session1{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys1[i], values1[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J k:K l:L m:M n:N o:O p:P q:Q r:R s:S t:T
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"},
                                             {"k","K"},{"l","L"},{"m","M"},{"n","N"},{"o","O"},
                                             {"p","P"},{"q","Q"},{"r","R"},{"s","S"},{"t","T"}}, _db.get_as_map());

   // _db._stack._new_keys<0>: a b c d e f g h i j
   // _db._stack._new_keys<1>: k l m n o p q r s t
   BOOST_TEST_REQUIRE( (_db.stack()[_db.stack().size()-2].new_keys()) == (std::set<std::vector<uint8_t>>{{'a'},{'b'},{'c'},{'d'},{'e'},{'f'},{'g'},{'h'},{'i'},{'j'}}) );
   BOOST_TEST_REQUIRE( (_db.stack()[_db.stack().size()-1].new_keys()) == (std::set<std::vector<uint8_t>>{{'k'},{'l'},{'m'},{'n'},{'o'},{'p'},{'q'},{'r'},{'s'},{'t'}}) );

   _db.squash();

   // _db._stack._new_keys<0>: a b c d e f g h i j k l m n o p q r s t
   BOOST_TEST_REQUIRE( (_db.stack().back().new_keys()) == (std::set<std::vector<uint8_t>>{ {'a'},{'b'},{'c'},{'d'},{'e'},{'f'},{'g'},{'h'},{'i'},{'j'},
                                                                                           {'k'},{'l'},{'m'},{'n'},{'o'},{'p'},{'q'},{'r'},{'s'},{'t'}}) );
}

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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db: k:A l:B m:C n:D o:E f:F g:G h:H i:I j:J
 * _db: k:A l:B m:C n:D o:E f:F g:G h:H i:I j:J
 */
BOOST_FIXTURE_TEST_CASE(test_seven, database_fixture) {
   std::cout << "TEST_SEVEN\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   auto session0{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session1{_db.start_undo_session(true)};
   for (size_t i{0}; i < 5; ++i) {
      _db.put(keys0[i], values1[i]);
   }
   _db.write_batch();

   // _db: k:A l:B m:C n:D o:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","K"},{"b","L"},{"c","M"},{"d","N"},{"e","O"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   _db.squash();

   // _db: k:A l:B m:C n:D o:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","K"},{"b","L"},{"c","M"},{"d","N"},{"e","O"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
}

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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db: f:F g:G h:H i:I j:J
 * _db: j:J
 * _db: j:J
 */
BOOST_FIXTURE_TEST_CASE(test_eight, database_fixture) {
   std::cout << "TEST_EIGHT\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   for (size_t i{}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session0{_db.start_undo_session(true)};
   for (size_t i{0}; i < 5; ++i) {
      _db.remove(keys0[i]);
   }
   _db.write_batch();

   // _db: f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   auto session1{_db.start_undo_session(true)};
   for (size_t i{5}; i < 9; ++i) {
      _db.remove(keys0[i]);
   }
   _db.write_batch();

   // _db: j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"j","J"}}, _db.get_as_map());

   _db.squash();

   // _db: j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"j","J"}}, _db.get_as_map());
}

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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db:
 */
BOOST_FIXTURE_TEST_CASE(test_nine, database_fixture) {
   std::cout << "TEST_NINE\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   {
   auto session{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
   }
   _db.write_batch();

   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());
}

/**
 * Test #10:
 *
 * `start_undo_session`
 * Initiate RAII `{`
 * Fill `_state` with new values
 * End RAII `}`
 *
 * _db:
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 */
BOOST_FIXTURE_TEST_CASE(test_ten, database_fixture) {
   std::cout << "TEST_TEN\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   auto session{_db.start_undo_session(true)};
   {
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
   }

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
}

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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 */
BOOST_FIXTURE_TEST_CASE(test_eleven, database_fixture) {
   std::cout << "TEST_ELEVEN\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   {
   auto session{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
   session.push();
   }

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
}

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
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
 * _db:
 */
BOOST_FIXTURE_TEST_CASE(test_twelve, database_fixture) {
   std::cout << "TEST_TWELVE\n";
   
   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());

   {
   auto session{_db.start_undo_session(true)};
   for (size_t i{0}; i < 10; ++i) {
      _db.put(keys0[i], values0[i]);
   }
   _db.write_batch();

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
   session.push();
   }

   // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
   _db.print_state();
   compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
                                             {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

   _db.undo();
   _db.write_batch();

   // _db:
   _db.print_state();
   compare(std::map<std::string,std::string>{}, _db.get_as_map());
}

/**
 * Other tests.
 */

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
//  *
//  * _db:
//  * _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
//  * _db: b:B c:C d:D e:E f:F g:G h:H i:I j:J
//  * _db: j:J
//  * _db: b:B c:C d:D e:E f:F g:G h:H i:I j:J
//  */
// BOOST_FIXTURE_TEST_CASE(test_thirteen, database_fixture) {
//    std::cout << "TEST_THIRTEEN\n";
   
//    // _db:
//    _db.print_state();
//    compare(std::map<std::string,std::string>{}, _db.get_as_map());

//    for (size_t i{}; i < 10; ++i) {
//       _db.put(keys0[i], values0[i]);
//    }
//    _db.write_batch();

//    // _db: a:A b:B c:C d:D e:E f:F g:G h:H i:I j:J
//    _db.print_state();
//    compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
//                                              {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

//    auto session0{_db.start_undo_session(true)};
//    _db.remove(keys0[0]);
//    _db.write_batch();

//    // _db: b:B c:C d:D e:E f:F g:G h:H i:I j:J
//    _db.print_state();
//    compare(std::map<std::string,std::string>{{"b","B"},{"c","C"},{"d","D"},{"e","E"},{"f","F"},
//                                              {"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

//    auto session1{_db.start_undo_session(true)};
//    _db.put(keys0[0], values0[0]);
//    _db.write_batch();

//    // _db: j:J
//    _db.print_state();
//    compare(std::map<std::string,std::string>{{"a","A"},{"b","B"},{"c","C"},{"d","D"},{"e","E"},
//                                              {"f","F"},{"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());

//    _db.squash();

//    // Under my previous implementation this would throw, which is incorrect behavior.
//    // Now, under the fixed implementation, this should now throw.
//    // Update: ^ What?
//    _db.remove(keys0[0]);
//    _db.write_batch();

//    // // _db: b:B c:C d:D e:E f:F g:G h:H i:I j:J
//    // _db.print_state();
//    // compare(std::map<std::string,std::string>{{"b","B"},{"c","C"},{"d","D"},{"e","E"},{"f","F"},
//    //                                           {"g","G"},{"h","H"},{"i","I"},{"j","J"}}, _db.get_as_map());
   
BOOST_AUTO_TEST_SUITE_END()
