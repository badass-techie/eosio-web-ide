#include <eosio/eosio.hpp>

using namespace eosio;
using std::string;

class [[eosio::contract("addressbook")]] addressbook : public eosio::contract {
  public:
    //ctor
    addressbook(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {

    }

    //update a row or insert a new row into the table
    [[eosio::action]] void upsert(name user, string first_name, string last_name, string street, string city, string state) {
      require_auth( user ); //make sure this record belongs to the account carrying out this transaction
      address_index addresses(get_self(), get_first_receiver().value);  //instantiate table
      
      //look up user in "addresses" table
      auto iterator = addresses.find(user.value);
      if( iterator != addresses.end() ){
        //user found

        addresses.modify(iterator, user, [&]( auto& row ) {
          row.key = user;
          row.first_name = first_name;
          row.last_name = last_name;
          row.street = street;
          row.city = city;
          row.state = state;
        }); //change existing record

      }
      else {
        //user not found

        addresses.emplace(user, [&]( auto& row ) {
          row.key = user;
          row.first_name = first_name;
          row.last_name = last_name;
          row.street = street;
          row.city = city;
          row.state = state;
        }); //create new record
      }
    }

    //remove row
    [[eosio::action]] void erase(name user){
      require_auth(user);
      address_index addresses(get_self(), get_first_receiver().value);
      
      //check if this record exists in table
      auto iterator = addresses.find(user.value);
      check(iterator != addresses.end(), "Record does not exist");
      addresses.erase(iterator);
    }

  private:
    struct [[eosio::table]] person {
        name key;
        string first_name;
        string last_name;
        string street;
        string city;
        string state;

        uint64_t primary_key() const {
          return key.value;
        }
    };

  using address_index = eosio::multi_index<"people"_n, person>;

};