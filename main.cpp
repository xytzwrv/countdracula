#include <dpp/dpp.h>
#include "modules/greetings_module.hpp"
#include "modules/chess/chess_module.hpp"
#include <cstdlib>
#include <iostream>

int main() {
    const char* token = std::getenv("DISCORD_BOT_TOKEN");

    if (!token) {
        std::cerr << "DISCORD_BOT_TOKEN environment variable was not set." << std::endl;
        return 1;
    }

    dpp::cluster bot(token);

    // Initialize modules
    GreetingsModule greetings(bot);
    ChessModule chess(bot);

    bot.on_ready([&bot](const dpp::ready_t& event) {
        std::cout << "Bot is ready! Logged in as " << bot.me.username << std::endl;
        bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_playing, "Chess and more!"));
    });

    bot.start(dpp::st_wait);
}
