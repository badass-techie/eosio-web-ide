#include <eosio/eosio.hpp>
#include "abcounter.cpp"

using namespace eosio;
using std::string;
using std::make_tuple;

class [[eosio::contract("addressbook")]] addressbook : public eosio::contract {
  public:
    //ctor
    addressbook(name receiver, name code, datastream<const char*> ds):contract(receiver, code, ds) {

    }

    //update a row or insert a new row into the table
    [[eosio::action]] void upsert(name user, string first_name, string last_name, uint64_t age, string street, string city, string state) {
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
          row.age = age;
          row.street = street;
          row.city = city;
          row.state = state;
        }); //change existing record
        send_summary(user, "successfully emplaced record to addressbook");
        increment_counter(user, "modify");

      }
      else {
        //user not found

        addresses.emplace(user, [&]( auto& row ) {
          row.key = user;
          row.first_name = first_name;
          row.last_name = last_name;
          row.age = age;
          row.street = street;
          row.city = city;
          row.state = state;
        }); //create new record
        send_summary(user, "successfully emplaced record to addressbook");
        increment_counter(user, "emplace");
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
      send_summary(user, " successfully emplaced record to addressbook");
      increment_counter(user, "erase");
    }

    //sends transaction receipt to user
    [[eosio::action]] void notify(name user, string msg) {
      require_auth(get_self());
      require_recipient(user);
    }

  private:
    struct [[eosio::table]] person {
        name key;
        string first_name;
        string last_name;
        uint64_t age;
        string street;
        string city;
        string state;

        uint64_t primary_key() const {
          return key.value;
        }

        uint64_t get_secondary_1() const { 
          return age;
        }
    };
    
    void send_summary(name user, string message){
      action(
        permission_level{get_self(),"active"_n},
        get_self(),
        "notify"_n,
        make_tuple(user, name{user}.to_string() + message)
      ).send();  
    }

    void increment_counter(name user, string type) {
      abcounter::count_action count("abcounter"_n, {get_self(), "active"_n});
      count.send(user, type);
    }

    using address_index = eosio::multi_index<"people"_n, person, indexed_by<"byage"_n, const_mem_fun<person, uint64_t, &person::get_secondary_1>>>;

};