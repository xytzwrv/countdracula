#include "greetings_module.hpp"

GreetingsModule::GreetingsModule(dpp::cluster& bot) {
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "hello") {
            event.reply("Hello world from the greetings module!");
        }
    });

    dpp::slashcommand hello("helloworld", "Say hello, world!", bot.me.id);
    bot.global_command_create(hello);
}
