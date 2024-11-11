// Unit tests

#include "message.h"
#include "message_serialization.h"
#include "table.h"
#include "value_stack.h"
#include "exceptions.h"
#include "tctest.h"

struct TestObjs
{
  Message m; // default message

  // some valid messages
  Message login_req;
  Message create_req;
  Message push_req;
  Message pop_req;
  Message set_req;
  Message get_req;
  Message add_req;
  Message mul_req;
  Message sub_req;
  Message div_req;
  Message bye_req;
  Message ok_resp;
  Message failed_resp;
  Message error_resp;
  Message data_resp;
  Message long_get_req;
  Message create_req_2;

  // some invalid messages
  // (meaning that the internal data violates the protocol spec)
  Message invalid_login_req;
  Message invalid_create_req;
  Message invalid_data_resp;

  // Message that is too long to encode
  Message invalid_too_long;

  // Some encoded messages to test decoding
  std::string encoded_login_req;
  std::string encoded_create_req;
  std::string encoded_data_resp;
  std::string encoded_get_req;
  std::string encoded_failed_resp;
  std::string encoded_error_resp;
  std::string encoded_bye_req;

  // Invalid encoded messages to test decoding
  std::string encoded_push_req_no_nl;
  std::string encoded_get_req_too_long;

  // Test object for Table unit tests
  Table *invoices;
  Table *line_items;

  // ValueStack object for testing
  ValueStack valstack;

  TestObjs();
  ~TestObjs();
};

// Create and clean up test fixture
TestObjs *setup();
void cleanup( TestObjs *objs );

// Guard object to ensure that Table is locked and unlocked
// in unit tests. This could be used to manage locking and
// unlocking a Table when autocommit (no multi-table transactions)
// is desired.
class TableGuard {
public:
  Table *m_table;

  TableGuard( Table *table )
    : m_table( table )
  {
    m_table->lock();
  }

  ~TableGuard()
  {
    m_table->unlock();
  }
};

// Prototypes of test functions
void test_message_default_ctor( TestObjs *objs );
void test_message_get_message_type( TestObjs *objs );
void test_message_get_username( TestObjs *objs );
void test_message_get_table( TestObjs *objs );
void test_message_get_value( TestObjs *objs );
void test_message_get_key( TestObjs *objs );
void test_message_is_valid( TestObjs *objs );
void test_message_serialization_encode( TestObjs *objs );
void test_message_serialization_encode_long( TestObjs *objs );
void test_message_serialization_encode_too_long( TestObjs *objs );
void test_message_serialization_decode( TestObjs *objs );
void test_message_serialization_decode_invalid( TestObjs *objs );
void test_table_has_key( TestObjs *objs );
void test_table_get( TestObjs *objs );
void test_table_commit_changes( TestObjs *objs );
void test_table_rollback_changes( TestObjs *objs );
void test_table_commit_and_rollback( TestObjs *objs );
void test_value_stack( TestObjs *objs );
void test_value_stack_exceptions( TestObjs *objs );

int main(int argc, char **argv)
{
  // Allow test name to be specified on the command line
  if ( argc >= 2 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST( test_message_default_ctor );
  TEST( test_message_get_message_type );
  TEST( test_message_get_username );
  TEST( test_message_get_table );
  TEST( test_message_get_value );
  TEST( test_message_get_key );
  TEST( test_message_is_valid );
  TEST( test_message_serialization_encode );
  TEST( test_message_serialization_encode_long );
  TEST( test_message_serialization_encode_too_long );
  TEST( test_message_serialization_decode );
  TEST( test_message_serialization_decode_invalid );
  TEST( test_table_has_key );
  TEST( test_table_get );
  TEST( test_table_commit_changes );
  TEST( test_table_rollback_changes );
  TEST( test_table_commit_and_rollback );
  TEST( test_value_stack );
  TEST( test_value_stack_exceptions );

  TEST_FINI();
}

// Constructor for TestObjs.
// All test fixture objects should be initialized here.
TestObjs::TestObjs()
  : m()

  // Valid messages
  , login_req( MessageType::LOGIN, { "alice" } )
  , create_req( MessageType::CREATE, { "accounts" } )
  , push_req( MessageType::PUSH, { "47374" } )
  , pop_req( MessageType::POP )
  , set_req( MessageType::SET, { "accounts", "acct123" } )
  , get_req( MessageType::GET, { "accounts", "acct123" } )
  , add_req( MessageType::ADD )
  , mul_req( MessageType::MUL )
  , sub_req( MessageType::SUB )
  , div_req( MessageType::DIV )
  , bye_req( MessageType::BYE )
  , ok_resp( MessageType::OK )
  // Note that the quoted_text argument in FAILED and ERROR responses
  // have the quotes stripped off before being incorporated into the
  // enclosing Message object
  , failed_resp( MessageType::FAILED, { "The operation failed" } )
  , error_resp( MessageType::ERROR, { "An error occurred" } )
  , data_resp( MessageType::DATA, { "10012" } )
  , long_get_req( MessageType::GET )
  // underscores in identifiers are legal (if not the first character)
  , create_req_2( MessageType::CREATE, { "line_items" } )

  // Invalid (non-protocol-conforming) messages
  , invalid_login_req( MessageType::LOGIN, { "bob", "extra" } ) // too many args
  , invalid_create_req( MessageType::CREATE, { "8foobar" } ) // arg is not an identifier
  , invalid_data_resp( MessageType::DATA ) // missing argument
  , invalid_too_long( MessageType::SET )

  // Encoded messages (for testing decoding)
  , encoded_login_req( "LOGIN alice\n" )
  , encoded_create_req( "     CREATE   invoices  \n" ) // unusual whitespace, but this is legal
  , encoded_data_resp( "DATA 90125\n" )
  , encoded_get_req( "GET lineitems foobar\n" )
  , encoded_failed_resp( "FAILED \"Something went wrong, shucks!\"\n" )
  , encoded_error_resp( " ERROR \"Wow, something really got messed up\"\n"  )
  , encoded_bye_req( "BYE\n" )

  // Invalid encoded messages
  , encoded_push_req_no_nl( "PUSH 91025" )
  , encoded_get_req_too_long( "GET foo " + std::string( Message::MAX_ENCODED_LEN, 'x' ) )

  // Table objects
  , invoices( new Table( "invoices" ) )
  , line_items( new Table( "line_items" ) )
{
  // This GET request message is just barely small enough to encode
  // (with no room to spare)
  long_get_req.push_arg( std::string( 509, 'y' ) );
  long_get_req.push_arg( std::string( 509, 'y' ) );

  // This SET message is (just barely by 1 character) too large to encode
  invalid_too_long.push_arg( std::string( 509, 'x' ) );
  invalid_too_long.push_arg( std::string( 510, 'x' ) );
}

// Destructor for TestObjs
TestObjs::~TestObjs()
{
  delete invoices;
  delete line_items;
}

TestObjs *setup()
{
  return new TestObjs;
}

void cleanup(TestObjs *objs)
{
  delete objs;
}

void test_message_default_ctor(TestObjs *objs)
{
  ASSERT( MessageType::NONE == objs->m.get_message_type() );
  ASSERT( objs->m.get_num_args() == 0 );
}

void test_message_get_message_type( TestObjs *objs )
{
  ASSERT( MessageType::LOGIN == objs->login_req.get_message_type() );
  ASSERT( MessageType::CREATE == objs->create_req.get_message_type() );
  ASSERT( MessageType::PUSH == objs->push_req.get_message_type() );
  ASSERT( MessageType::POP == objs->pop_req.get_message_type() );
  ASSERT( MessageType::SET == objs->set_req.get_message_type() );
  ASSERT( MessageType::GET == objs->get_req.get_message_type() );
  ASSERT( MessageType::ADD == objs->add_req.get_message_type() );
  ASSERT( MessageType::MUL == objs->mul_req.get_message_type() );
  ASSERT( MessageType::SUB == objs->sub_req.get_message_type() );
  ASSERT( MessageType::DIV == objs->div_req.get_message_type() );
  ASSERT( MessageType::BYE == objs->bye_req.get_message_type() );

  ASSERT( MessageType::OK == objs->ok_resp.get_message_type() );
  ASSERT( MessageType::FAILED == objs->failed_resp.get_message_type() );
  ASSERT( MessageType::ERROR == objs->error_resp.get_message_type() );
  ASSERT( MessageType::DATA == objs->data_resp.get_message_type() );

  ASSERT( MessageType::GET == objs->long_get_req.get_message_type() );
  ASSERT( MessageType::CREATE == objs->create_req_2.get_message_type() );
}

void test_message_get_username( TestObjs *objs )
{
  ASSERT( "alice" == objs->login_req.get_username() );
}

void test_message_get_table( TestObjs *objs )
{
  ASSERT( "accounts" == objs->create_req.get_table() );
  ASSERT( "accounts" == objs->set_req.get_table() );
  ASSERT( "accounts" == objs->get_req.get_table() );
  ASSERT( std::string( 509, 'y' ) == objs->long_get_req.get_table() );
  ASSERT( "line_items" == objs->create_req_2.get_table() );
}

void test_message_get_value( TestObjs *objs )
{
  ASSERT( "47374" == objs->push_req.get_value() );
  ASSERT( "10012" == objs->data_resp.get_value() );
}

void test_message_get_key( TestObjs *objs )
{
  ASSERT( "acct123" == objs->set_req.get_key() );
  ASSERT( "acct123" == objs->get_req.get_key() );
}

void test_message_is_valid( TestObjs *objs )
{
  ASSERT( objs->login_req.is_valid() );
  ASSERT( objs->create_req.is_valid() );
  ASSERT( objs->push_req.is_valid() );
  ASSERT( objs->pop_req.is_valid() );
  ASSERT( objs->set_req.is_valid() );
  ASSERT( objs->get_req.is_valid() );
  ASSERT( objs->add_req.is_valid() );
  ASSERT( objs->mul_req.is_valid() );
  ASSERT( objs->sub_req.is_valid() );
  ASSERT( objs->div_req.is_valid() );
  ASSERT( objs->bye_req.is_valid() );
  ASSERT( objs->ok_resp.is_valid() );
  ASSERT( objs->failed_resp.is_valid() );
  ASSERT( objs->error_resp.is_valid() );
  ASSERT( objs->data_resp.is_valid() );
  ASSERT( objs->long_get_req.is_valid() );
  ASSERT( objs->create_req_2.is_valid() );

  ASSERT( !objs->invalid_login_req.is_valid() );
  ASSERT( !objs->invalid_create_req.is_valid() );
  ASSERT( !objs->invalid_data_resp.is_valid() );
}

void test_message_serialization_encode( TestObjs *objs )
{
  std::string s;

  MessageSerialization::encode( objs->login_req, s );
  ASSERT( "LOGIN alice\n" == s );

  MessageSerialization::encode( objs->create_req, s );
  ASSERT( "CREATE accounts\n" == s );

  MessageSerialization::encode( objs->push_req, s );
  ASSERT( "PUSH 47374\n" == s );

  MessageSerialization::encode( objs->pop_req, s );
  ASSERT( "POP\n" == s );

  MessageSerialization::encode( objs->set_req, s );
  ASSERT( "SET accounts acct123\n" == s );

  MessageSerialization::encode( objs->data_resp, s );
  ASSERT( "DATA 10012\n" == s );
}

void test_message_serialization_encode_long( TestObjs *objs )
{
  std::string expected_str = "GET " + std::string(509, 'y') + " " + std::string(509, 'y') + "\n";
  std::string actual_str;
  MessageSerialization::encode( objs->long_get_req, actual_str );
  ASSERT( expected_str == actual_str );
}

void test_message_serialization_encode_too_long( TestObjs *objs )
{
  try {
    std::string s;
    MessageSerialization::encode( objs->invalid_too_long, s );
    FAIL( "exception was not thrown for too-long encoded message" );
  } catch (InvalidMessage &ex) {
    // Good
  }
}

void test_message_serialization_decode( TestObjs *objs )
{
  Message msg;

  MessageSerialization::decode( objs->encoded_login_req, msg );
  ASSERT( MessageType::LOGIN == msg.get_message_type() );
  ASSERT( 1 == msg.get_num_args() );
  ASSERT( "alice" == msg.get_username() );

  MessageSerialization::decode( objs->encoded_create_req, msg );
  ASSERT( MessageType::CREATE == msg.get_message_type() );
  ASSERT( 1 == msg.get_num_args() );
  ASSERT( "invoices" == msg.get_table() );

  MessageSerialization::decode( objs->encoded_data_resp, msg );
  ASSERT( MessageType::DATA == msg.get_message_type() );
  ASSERT( 1 == msg.get_num_args() );
  ASSERT( "90125" == msg.get_value() );

  MessageSerialization::decode( objs->encoded_get_req, msg );
  ASSERT( MessageType::GET == msg.get_message_type() );
  ASSERT( 2 == msg.get_num_args() );
  ASSERT( "lineitems" == msg.get_table() );
  ASSERT( "foobar" == msg.get_key() );

  MessageSerialization::decode( objs->encoded_failed_resp, msg );
  ASSERT( MessageType::FAILED == msg.get_message_type() );
  ASSERT( 1 == msg.get_num_args() );
  ASSERT( "Something went wrong, shucks!" == msg.get_quoted_text() );

  MessageSerialization::decode( objs->encoded_error_resp, msg );
  ASSERT( MessageType::ERROR == msg.get_message_type() );
  ASSERT( 1 == msg.get_num_args() );
  ASSERT( "Wow, something really got messed up" == msg.get_quoted_text() );

  MessageSerialization::decode( objs->encoded_bye_req, msg );
  ASSERT( MessageType::BYE == msg.get_message_type() );
  ASSERT( 0 == msg.get_num_args() );
}

void test_message_serialization_decode_invalid( TestObjs *objs )
{
  Message msg;

  try {
    MessageSerialization::decode( objs->encoded_push_req_no_nl, msg );
    FAIL( "No exception thrown decoding message lacking terminating newline" );
  } catch ( InvalidMessage &ex ) {
    // Good
  }

  try {
    MessageSerialization::decode( objs->encoded_get_req_too_long, msg );
    FAIL( "No exception thrown decoding message that is too long" );
  } catch ( InvalidMessage &ex ) {
    // Good
  }
}

void test_table_has_key( TestObjs *objs )
{
  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    objs->invoices->set( "abc123", "1000" );
    objs->invoices->set( "xyz456", "1318" );
  }

  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    // Changes should be visible even though we haven't committed them
    ASSERT( objs->invoices->has_key( "abc123" ) );
    ASSERT( objs->invoices->has_key( "xyz456" ) );

    ASSERT( !objs->invoices->has_key( "nonexistent" ) );
  }
}

void test_table_get( TestObjs *objs )
{
  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    objs->invoices->set( "abc123", "1000" );
    objs->invoices->set( "xyz456", "1318" );
  }

  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    // Changes should be visible even though we haven't committed them
    ASSERT( "1000" == objs->invoices->get( "abc123" ) );
    ASSERT( "1318" == objs->invoices->get( "xyz456" ) );

    ASSERT( !objs->invoices->has_key( "nonexistent" ) );
  }
}

void test_table_commit_changes( TestObjs *objs )
{
  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    objs->invoices->set( "abc123", "1000" );
    objs->invoices->set( "xyz456", "1318" );
  }

  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    // Changes should be visible even though we haven't committed them
    ASSERT( "1000" == objs->invoices->get( "abc123" ) );
    ASSERT( "1318" == objs->invoices->get( "xyz456" ) );

    ASSERT( !objs->invoices->has_key( "nonexistent" ) );
  }

  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    // Commit changes
    objs->invoices->commit_changes();

    // Changes should still be visible
    ASSERT( "1000" == objs->invoices->get( "abc123" ) );
    ASSERT( "1318" == objs->invoices->get( "xyz456" ) );

    ASSERT( !objs->invoices->has_key( "nonexistent" ) );
  }
}

void test_table_rollback_changes( TestObjs *objs )
{
  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    objs->invoices->set( "abc123", "1000" );
    objs->invoices->set( "xyz456", "1318" );
  }

  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    // Changes should be visible even though we haven't committed them
    ASSERT( "1000" == objs->invoices->get( "abc123" ) );
    ASSERT( "1318" == objs->invoices->get( "xyz456" ) );

    ASSERT( !objs->invoices->has_key( "nonexistent" ) );
  }

  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    // Rollback changes
    objs->invoices->rollback_changes();
  }

  {
    TableGuard g( objs->invoices ); // ensure table is locked and unlocked

    // Table should be empty again!
    ASSERT( !objs->invoices->has_key( "abc123" ) );
    ASSERT( !objs->invoices->has_key( "xyz456" ) );
    ASSERT( !objs->invoices->has_key( "nonexistent" ) );
  }
}

// Test that changes can be committed, then more modifications can be
// done and then rolled back, and the originally committed data is still
// there.
void test_table_commit_and_rollback( TestObjs *objs )
{
  // Add some data
  {
    TableGuard g( objs->line_items );

    objs->line_items->set( "apples", "100" );
    objs->line_items->set( "bananas", "150" );
  }

  // Commit changes
  {
    TableGuard g( objs->line_items );

    objs->line_items->commit_changes();
  }

  // Ensure that data is there
  {
    TableGuard g( objs->line_items );

    ASSERT( "100" == objs->line_items->get( "apples" ) );
    ASSERT( "150" == objs->line_items->get( "bananas" ) );
  }

  // Add more data
  {
    TableGuard g( objs->line_items );

    objs->line_items->set( "oranges", "220" );
  }

  // Ensure that data is there
  {
    TableGuard g( objs->line_items );

    ASSERT( "100" == objs->line_items->get( "apples" ) );
    ASSERT( "150" == objs->line_items->get( "bananas" ) );
    ASSERT( "220" == objs->line_items->get( "oranges" ) );
  }

  // Rollback most recent change
  {
    TableGuard g( objs->line_items );

    objs->line_items->rollback_changes();
  }

  // Original data should still be there (since it was committed),
  // but pending change shouldn't be there
  {
    TableGuard g( objs->line_items );

    ASSERT( "100" == objs->line_items->get( "apples" ) );
    ASSERT( "150" == objs->line_items->get( "bananas" ) );
    ASSERT( !objs->line_items->has_key( "oranges" ) );
  }
}

void test_value_stack( TestObjs *objs )
{
  // stack should be empty initially
  ASSERT( objs->valstack.is_empty() );

  // push some values
  objs->valstack.push( "foo" );
  ASSERT( !objs->valstack.is_empty() );
  objs->valstack.push( "bar" );
  ASSERT( !objs->valstack.is_empty() );
  objs->valstack.push( "12345" );
  ASSERT( !objs->valstack.is_empty() );

  // verify that pushed values can be accessed and popped off
  ASSERT( "12345" == objs->valstack.get_top() );
  objs->valstack.pop();
  ASSERT( !objs->valstack.is_empty() );
  ASSERT( "bar" == objs->valstack.get_top() );
  objs->valstack.pop();
  ASSERT( !objs->valstack.is_empty() );
  ASSERT( "foo" == objs->valstack.get_top() );
  objs->valstack.pop();

  // stack should be empty now
  ASSERT( objs->valstack.is_empty() );
}

void test_value_stack_exceptions( TestObjs *objs )
{
  ASSERT( objs->valstack.is_empty() );

  try {
    objs->valstack.get_top();
    FAIL( "ValueStack didn't throw exception for get_top() on empty stack" );
  } catch ( OperationException &ex ) {
    // good
  }

  try {
    objs->valstack.pop();
    FAIL( "ValueStack didn't throw exception for pop() on empty stack" );
  } catch ( OperationException &ex ) {
    // good
  }
}
