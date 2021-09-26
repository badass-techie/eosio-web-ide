#include <eosio/eosio.hpp>
class [[eosio::contract]] hello : public eosio::contract {
  public:
      using eosio::contract::contract;
      [[eosio::action]] void what( eosio::name user ) {
         print( "hi, what do you want ", user);
      }

      [[eosio::action]] void why( eosio::name user ) {
         print( "why not ", user);
      }

      [[eosio::action]] void how( eosio::name user ) {
         print( "how are you ", user);
      }
};