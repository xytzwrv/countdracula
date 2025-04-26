#pragma once
#include <dpp/dpp.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <utility>
#include <ctime>
#include <iomanip>
#include <sstream>

// Forward declaration for the Chess board class
class ChessBoard;

class ChessModule {
private:
    dpp::cluster& bot;
    
    // Game state
    std::unique_ptr<ChessBoard> current_game;
    std::pair<dpp::snowflake, dpp::snowflake> current_players;
    std::vector<std::string> board_images;
    bool game_in_progress;
    
    // Helper methods
    std::string board_to_image(const ChessBoard& board, const std::string& white_player, 
                              const std::string& black_player, int move_number);
    void register_commands();
    
    // Command handlers
    void handle_start_chess(const dpp::slashcommand_t& event);
    void handle_move(const dpp::slashcommand_t& event);
    
public:
    ChessModule(dpp::cluster& bot);
    ~ChessModule();
};

// Chess piece type
enum class PieceType {
    NONE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};

// Chess piece color
enum class PieceColor {
    NONE,
    WHITE,
    BLACK
};

// Chess piece representation
struct ChessPiece {
    PieceType type;
    PieceColor color;
    
    ChessPiece() : type(PieceType::NONE), color(PieceColor::NONE) {}
    ChessPiece(PieceType t, PieceColor c) : type(t), color(c) {}
    
    bool is_empty() const { return type == PieceType::NONE; }
};

// Position on the chess board
struct Position {
    int file; // a-h (0-7)
    int rank; // 1-8 (0-7)
    
    Position() : file(0), rank(0) {}
    Position(int f, int r) : file(f), rank(r) {}
    
    bool is_valid() const {
        return file >= 0 && file < 8 && rank >= 0 && rank < 8;
    }
    
    // Create position from algebraic notation (e.g., "e4")
    static Position from_algebraic(const std::string& algebraic);
    
    // Convert position to algebraic notation
    std::string to_algebraic() const;
};

// Move representation
struct Move {
    Position from;
    Position to;
    
    Move() {}
    Move(const Position& f, const Position& t) : from(f), to(t) {}
    
    // Create a move from UCI string (e.g., "e2e4")
    static Move from_uci(const std::string& uci);
    
    // Convert to UCI string
    std::string to_uci() const;
    
    bool operator==(const Move& other) const {
        return from.file == other.from.file && from.rank == other.from.rank &&
               to.file == other.to.file && to.rank == other.to.rank;
    }
};

// Chess board representation
class ChessBoard {
private:
    std::vector<std::vector<ChessPiece>> board;
    PieceColor turn;
    int fullmove_number;
    bool game_over;
    std::string result;
    
public:
    ChessBoard();
    
    // Board state getters
    ChessPiece get_piece(const Position& pos) const;
    PieceColor get_turn() const { return turn; }
    int get_fullmove_number() const { return fullmove_number; }
    bool is_game_over() const { return game_over; }
    std::string get_result() const { return result; }
    
    // Game actions
    void make_move(const Move& move);
    std::vector<Move> get_legal_moves() const;
    bool is_legal_move(const Move& move) const;
    
    // SVG generation
    std::string to_svg() const;
    
    // Other helpers
    static bool is_move_in_vector(const Move& move, const std::vector<Move>& moves);
};
