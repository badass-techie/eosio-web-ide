#include <eosio/eosio.hpp>

using namespace eosio;
using std::vector;
using games = eosio::multi_index<"games"_n, game>;

class[[eosio::contract("tictactoe")]] tictactoe : public contract{
public:
    
    using contract::contract;
    
    // ctor
    tictactoe(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {

    }

    struct [[eosio::table]] game{
        static constexpr uint16_t boardWidth = 3;
        static constexpr uint16_t boardHeight = boardWidth;
        
        game() : board(boardWidth * boardHeight, 0){

        }

        name challenger, host, turn; // = account name of host, challenger and turn to store whose turn it is.
        name winner = none; // = none/ draw/ name of host/ name of challenger

        vector<uint8_t> board;

        // initialize the board with empty cell
        void initializeBoard(){
            board.assign(boardWidth * boardHeight, 0);
        }

        // reset game
        void resetGame(){
            initializeBoard();
            turn = host;
            winner = "none"_n;
        }

        //
		auto primary_key() const { 
            return challenger.value; 
        }

        // EOSLIB_SERIALIZE macro defining how the abi serializes / deserializes  
		EOSLIB_SERIALIZE( game, (challenger)(host)(turn)(winner)(board))
    };

    //prototypes

    [[eosio::action]]
    void create(const name &challenger, name &host);
    
    [[eosio::action]]
    void restart(const name &challenger, const name &host, const name &by);

    [[eosio::action]]
    void close(const name &challenger, const name &host);
    
    [[eosio::action]]   
    void move(const name &challenger, const name &host, const name &by, const uint16_t &row, const uint16_t &column);

private:
    bool isEmptyCell(const uint8_t &cell);
    bool isValidMove(const uint16_t &row, const uint16_t &column, const std::vector<uint8_t> &board);
    name getWinner(const game &currentGame);

};