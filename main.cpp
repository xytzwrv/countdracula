#include <dpp/dpp.h>
#include "modules/greetings_module.h"
#include <cstdlib>
#include <iostream>

int main() {
    const char* token = std::getenv("DISCORD_BOT_TOKEN");

    if (!token) {
        std::cerr << "DISCORD_BOT_TOKEN environment variable was not set." << std::endl;
        return 1;
    }

    dpp::cluster bot(token);

    GreetingsModule greetings(bot);

    bot.on_ready([&bot](const dpp::ready_t& event) {
        bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_playing, "Modular Bot!"));
    });

    bot.start(dpp::st_wait);
}

