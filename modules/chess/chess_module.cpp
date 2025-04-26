#include "chess_module.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <filesystem>
#include <cstdlib>

// Helper function to get current timestamp as a string
std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y%m%d-%H%M%S");
    return ss.str();
}

// Position methods
Position Position::from_algebraic(const std::string& algebraic) {
    if (algebraic.length() != 2) {
        throw std::invalid_argument("Invalid algebraic notation: " + algebraic);
    }
    
    char file_char = algebraic[0];
    char rank_char = algebraic[1];
    
    if (file_char < 'a' || file_char > 'h' || rank_char < '1' || rank_char > '8') {
        throw std::invalid_argument("Invalid algebraic notation: " + algebraic);
    }
    
    int file = file_char - 'a';
    int rank = rank_char - '1';
    
    return Position(file, rank);
}

std::string Position::to_algebraic() const {
    if (!is_valid()) {
        return "invalid";
    }
    
    char file_char = 'a' + file;
    char rank_char = '1' + rank;
    
    return std::string(1, file_char) + std::string(1, rank_char);
}

// Move methods
Move Move::from_uci(const std::string& uci) {
    if (uci.length() < 4) {
        throw std::invalid_argument("Invalid UCI notation: " + uci);
    }
    
    std::string from_str = uci.substr(0, 2);
    std::string to_str = uci.substr(2, 2);
    
    Position from = Position::from_algebraic(from_str);
    Position to = Position::from_algebraic(to_str);
    
    return Move(from, to);
}

std::string Move::to_uci() const {
    return from.to_algebraic() + to.to_algebraic();
}

// ChessBoard implementation
ChessBoard::ChessBoard() : turn(PieceColor::WHITE), fullmove_number(1), game_over(false), result("*") {
    // Initialize an 8x8 board
    board.resize(8, std::vector<ChessPiece>(8, ChessPiece()));
    
    // Set up the pieces
    // Pawns
    for (int file = 0; file < 8; file++) {
        board[file][1] = ChessPiece(PieceType::PAWN, PieceColor::WHITE);
        board[file][6] = ChessPiece(PieceType::PAWN, PieceColor::BLACK);
    }
    
    // Rooks
    board[0][0] = ChessPiece(PieceType::ROOK, PieceColor::WHITE);
    board[7][0] = ChessPiece(PieceType::ROOK, PieceColor::WHITE);
    board[0][7] = ChessPiece(PieceType::ROOK, PieceColor::BLACK);
    board[7][7] = ChessPiece(PieceType::ROOK, PieceColor::BLACK);
    
    // Knights
    board[1][0] = ChessPiece(PieceType::KNIGHT, PieceColor::WHITE);
    board[6][0] = ChessPiece(PieceType::KNIGHT, PieceColor::WHITE);
    board[1][7] = ChessPiece(PieceType::KNIGHT, PieceColor::BLACK);
    board[6][7] = ChessPiece(PieceType::KNIGHT, PieceColor::BLACK);
    
    // Bishops
    board[2][0] = ChessPiece(PieceType::BISHOP, PieceColor::WHITE);
    board[5][0] = ChessPiece(PieceType::BISHOP, PieceColor::WHITE);
    board[2][7] = ChessPiece(PieceType::BISHOP, PieceColor::BLACK);
    board[5][7] = ChessPiece(PieceType::BISHOP, PieceColor::BLACK);
    
    // Queens
    board[3][0] = ChessPiece(PieceType::QUEEN, PieceColor::WHITE);
    board[3][7] = ChessPiece(PieceType::QUEEN, PieceColor::BLACK);
    
    // Kings
    board[4][0] = ChessPiece(PieceType::KING, PieceColor::WHITE);
    board[4][7] = ChessPiece(PieceType::KING, PieceColor::BLACK);
}

ChessPiece ChessBoard::get_piece(const Position& pos) const {
    if (!pos.is_valid()) {
        throw std::out_of_range("Position is out of bounds");
    }
    
    return board[pos.file][pos.rank];
}

void ChessBoard::make_move(const Move& move) {
    if (!is_legal_move(move)) {
        throw std::invalid_argument("Illegal move");
    }
    
    // Make the move
    board[move.to.file][move.to.rank] = board[move.from.file][move.from.rank];
    board[move.from.file][move.from.rank] = ChessPiece();
    
    // Update turn
    turn = (turn == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
    
    // Update fullmove number (increments after Black's move)
    if (turn == PieceColor::WHITE) {
        fullmove_number++;
    }
    
    // For now, we'll just implement a simple checkmate detection
    // In a real implementation, we would need much more sophisticated game state checking
    auto legal_moves = get_legal_moves();
    if (legal_moves.empty()) {
        game_over = true;
        result = (turn == PieceColor::WHITE) ? "0-1" : "1-0";
    }
}

// A simplified legal moves function - in a real chess engine this would be much more complex
std::vector<Move> ChessBoard::get_legal_moves() const {
    std::vector<Move> moves;
    
    // This is a very simplified implementation - a real chess engine would need much more
    // For demonstration, we'll just allow basic pawn moves
    for (int file = 0; file < 8; file++) {
        for (int rank = 0; rank < 8; rank++) {
            Position from(file, rank);
            ChessPiece piece = get_piece(from);
            
            // Skip empty squares and pieces of the wrong color
            if (piece.is_empty() || piece.color != turn) {
                continue;
            }
            
            // Handle different piece types
            if (piece.type == PieceType::PAWN) {
                // Direction depends on color
                int direction = (piece.color == PieceColor::WHITE) ? 1 : -1;
                
                // Forward move
                Position to(file, rank + direction);
                if (to.is_valid() && get_piece(to).is_empty()) {
                    moves.push_back(Move(from, to));
                    
                    // Double move from starting position
                    if ((piece.color == PieceColor::WHITE && rank == 1) || 
                        (piece.color == PieceColor::BLACK && rank == 6)) {
                        Position double_to(file, rank + 2 * direction);
                        if (double_to.is_valid() && get_piece(double_to).is_empty()) {
                            moves.push_back(Move(from, double_to));
                        }
                    }
                }
                
                // Captures
                for (int df : {-1, 1}) {
                    Position capture_to(file + df, rank + direction);
                    if (capture_to.is_valid()) {
                        ChessPiece target = get_piece(capture_to);
                        if (!target.is_empty() && target.color != piece.color) {
                            moves.push_back(Move(from, capture_to));
                        }
                    }
                }
            }
            
            // For other pieces, we'd implement their move patterns here
            // This is omitted for brevity, but would be necessary for a complete chess implementation
        }
    }
    
    return moves;
}

bool ChessBoard::is_legal_move(const Move& move) const {
    auto legal_moves = get_legal_moves();
    return is_move_in_vector(move, legal_moves);
}

bool ChessBoard::is_move_in_vector(const Move& move, const std::vector<Move>& moves) {
    return std::find(moves.begin(), moves.end(), move) != moves.end();
}

// SVG generation for the chess board
std::string ChessBoard::to_svg() const {
    std::stringstream svg;
    
    // SVG header
    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
    svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"400\">" << std::endl;
    
    // Board background
    svg << "<rect width=\"400\" height=\"400\" fill=\"#8ca2ad\"/>" << std::endl;
    
    // Draw the board squares
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int x = file * 50;
            int y = (7 - rank) * 50;  // Flip the board so rank 1 is at the bottom
            
            bool is_light = (file + rank) % 2 == 0;
            std::string color = is_light ? "#ffce9e" : "#d18b47";
            
            svg << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"50\" height=\"50\" fill=\"" << color << "\"/>" << std::endl;
            
            // Draw the piece if there is one
            ChessPiece piece = board[file][rank];
            if (!piece.is_empty()) {
                std::string piece_symbol;
                
                // Map piece type to Unicode chess symbol
                switch (piece.type) {
                    case PieceType::PAWN:
                        piece_symbol = (piece.color == PieceColor::WHITE) ? "♙" : "♟";
                        break;
                    case PieceType::KNIGHT:
                        piece_symbol = (piece.color == PieceColor::WHITE) ? "♘" : "♞";
                        break;
                    case PieceType::BISHOP:
                        piece_symbol = (piece.color == PieceColor::WHITE) ? "♗" : "♝";
                        break;
                    case PieceType::ROOK:
                        piece_symbol = (piece.color == PieceColor::WHITE) ? "♖" : "♜";
                        break;
                    case PieceType::QUEEN:
                        piece_symbol = (piece.color == PieceColor::WHITE) ? "♕" : "♛";
                        break;
                    case PieceType::KING:
                        piece_symbol = (piece.color == PieceColor::WHITE) ? "♔" : "♚";
                        break;
                    default:
                        piece_symbol = "";
                }
                
                if (!piece_symbol.empty()) {
                    svg << "<text x=\"" << (x + 25) << "\" y=\"" << (y + 35) 
                        << "\" font-size=\"35\" text-anchor=\"middle\" fill=\"" 
                        << (piece.color == PieceColor::WHITE ? "white" : "black") << "\">" 
                        << piece_symbol << "</text>" << std::endl;
                }
            }
        }
    }
    
    // Draw rank and file labels
    for (int i = 0; i < 8; i++) {
        // Rank labels (1-8)
        svg << "<text x=\"5\" y=\"" << (i * 50 + 25) << "\" font-size=\"12\" text-anchor=\"middle\">" 
            << (8 - i) << "</text>" << std::endl;
        
        // File labels (a-h)
        svg << "<text x=\"" << (i * 50 + 25) << "\" y=\"395\" font-size=\"12\" text-anchor=\"middle\">" 
            << char('a' + i) << "</text>" << std::endl;
    }
    
    // Close the SVG
    svg << "</svg>" << std::endl;
    
    return svg.str();
}

// ChessModule implementation
ChessModule::ChessModule(dpp::cluster& bot) : bot(bot), game_in_progress(false) {
    std::cout << "ChessModule loaded!" << std::endl;
    
    // Register slash commands
    register_commands();
    
    // Set up command handlers
    bot.on_slashcommand([this](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "start_chess") {
            handle_start_chess(event);
        }
        else if (event.command.get_command_name() == "move") {
            handle_move(event);
        }
    });
}

ChessModule::~ChessModule() {
    // Clean up any temporary files
    for (const auto& image_path : board_images) {
        try {
            std::filesystem::remove(image_path);
        } catch (const std::exception& e) {
            std::cerr << "Error removing temp file: " << e.what() << std::endl;
        }
    }
}

void ChessModule::register_commands() {
    // Start chess command
    dpp::slashcommand start_cmd("start_chess", "Start a new chess game with another user", bot.me.id);
    start_cmd.add_option(
        dpp::command_option(dpp::co_user, "opponent", "The user to play against", true)
    );
    bot.global_command_create(start_cmd);
    
    // Move command
    dpp::slashcommand move_cmd("move", "Make a chess move in standard UCI notation (e.g., e2e4)", bot.me.id);
    move_cmd.add_option(
        dpp::command_option(dpp::co_string, "move", "The move in UCI notation (e.g., e2e4)", true)
    );
    bot.global_command_create(move_cmd);
}

std::string ChessModule::board_to_image(const ChessBoard& board, const std::string& white_player, 
                                      const std::string& black_player, int move_number) {
    std::string svg_data = board.to_svg();
    std::string timestamp = get_timestamp();
    std::string image_path = "/tmp/chess_board_" + timestamp + "_" + white_player + "_vs_" + 
                             black_player + "_move_" + std::to_string(move_number) + ".svg";
    
    // Write SVG data to file
    std::ofstream svg_file(image_path);
    if (svg_file.is_open()) {
        svg_file << svg_data;
        svg_file.close();
        board_images.push_back(image_path);
        return image_path;
    } else {
        throw std::runtime_error("Failed to write SVG to file");
    }
}

void ChessModule::handle_start_chess(const dpp::slashcommand_t& event) {
    if (game_in_progress) {
        event.reply("A game is already in progress. Finish it first or wait.");
        return;
    }
    
    // Get opponent from parameters
    auto opponent_param = event.get_parameter("opponent");
    dpp::snowflake opponent_id = std::get<dpp::snowflake>(opponent_param);
    
    // Check if opponent is a bot
    auto cache_guild = dpp::find_guild(event.command.guild_id);
    if (cache_guild) {
        auto members = cache_guild->members;
        auto opponent_member = members.find(opponent_id);
        if (opponent_member != members.end() && opponent_member->second.is_bot()) {
            event.reply("You cannot play against a bot in two-player mode.");
            return;
        }
    }
    
    // Start a new game
    current_game = std::make_unique<ChessBoard>();
    current_players = std::make_pair(event.command.get_issuing_user().id, opponent_id);
    game_in_progress = true;
    
    // Get user names
    std::string white_player_name = event.command.get_issuing_user().username;
    dpp::user opponent_user = bot.user_get_sync(opponent_id);
    std::string black_player_name = opponent_user.username;
    
    // Create board image
    std::string image_path;
    try {
        image_path = board_to_image(*current_game, white_player_name, black_player_name, 0);
    } catch (const std::exception& e) {
        event.reply("Error generating board image: " + std::string(e.what()));
        return;
    }
    
    // Send start message
    std::string response = "New chess game started between <@" + 
                           std::to_string(event.command.get_issuing_user().id) + 
                           "> (White) and <@" + std::to_string(opponent_id) + 
                           "> (Black)! Use `/move e2e4` to move.";
    
    // Reply with message and file
    event.thinking(true);
    dpp::message msg(event.command.channel_id, response);
    msg.add_file("chessboard.svg", dpp::utility::read_file(image_path));
    bot.message_create(msg, [event](const dpp::confirmation_callback_t& callback) {
        if (callback.is_error()) {
            event.edit_response("Error sending board image");
        }
    });
    event.edit_response("Game started!");
}

void ChessModule::handle_move(const dpp::slashcommand_t& event) {
    if (!game_in_progress || !current_game) {
        event.reply("No game in progress. Use `/start_chess @user` to begin.");
        return;
    }
    
    dpp::snowflake user_id = event.command.get_issuing_user().id;
    if (user_id != current_players.first && user_id != current_players.second) {
        event.reply("You are not a player in the current chess game.");
        return;
    }
    
    // Check whose turn it is
    bool is_white_turn = current_game->get_turn() == PieceColor::WHITE;
    dpp::snowflake current_turn_player = is_white_turn ? current_players.first : current_players.second;
    
    if (user_id != current_turn_player) {
        event.reply("It's not your turn.");
        return;
    }
    
    // Get the move from parameters
    auto move_param = event.get_parameter("move");
    std::string move_str = std::get<std::string>(move_param);
    
    try {
        // Parse the move
        Move chess_move = Move::from_uci(move_str);
        
        // Check if the move is legal
        if (current_game->is_legal_move(chess_move)) {
            // Make the move
            current_game->make_move(chess_move);
            
            // Get player names
            std::string white_player_name = bot.user_get_sync(current_players.first).username;
            std::string black_player_name = bot.user_get_sync(current_players.second).username;
            
            // Create board image
            int move_number = current_game->get_fullmove_number();
            event.thinking(true);
            
            std::string image_path;
            try {
                image_path = board_to_image(*current_game, white_player_name, black_player_name, move_number);
            } catch (const std::exception& e) {
                event.edit_response("Error generating board image: " + std::string(e.what()));
                return;
            }
            
            // Send move message
            std::string response = "Move made: " + move_str;
            
            // Add game over info if applicable
            if (current_game->is_game_over()) {
                response += "\nGame over! Result: " + current_game->get_result();
                game_in_progress = false;
                current_game.reset();
            }
            
            // Reply with message and file
            dpp::message msg(event.command.channel_id, response);
            msg.add_file("chessboard.svg", dpp::utility::read_file(image_path));
            
            bot.message_create(msg, [event](const dpp::confirmation_callback_t& callback) {
                if (callback.is_error()) {
                    event.edit_response("Error sending board image");
                }
            });
            
            event.edit_response("Move processed!");
            
        } else {
            event.reply("Illegal move. Try again.");
        }
    } catch (const std::exception& e) {
        event.reply("Invalid move format. Use standard UCI (e.g., e2e4). Error: " + std::string(e.what()));
    }
}
