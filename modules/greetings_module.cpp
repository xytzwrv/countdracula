#include "greetings_module.hpp"
#include <iostream>

GreetingsModule::GreetingsModule(dpp::cluster& bot) {
    std::cout << "Initializing Greetings Module..." << std::endl;
    
    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "hello") {
            std::cout << "Received /hello command from user: " << event.command.get_issuing_user().username << std::endl;
            event.reply("Hello world from the greetings module!");
        }
    });

    // Create the hello command
    try {
        dpp::slashcommand hello("helloworld", "Say hello, world!", bot.me.id);
        
        // Try to get guild ID from environment
        const char* guild_id_str = std::getenv("DISCORD_GUILD_ID");
        if (guild_id_str) {
            try {
                dpp::snowflake guild_id = std::stoull(guild_id_str);
                bot.guild_command_create(hello, guild_id);
                std::cout << "Registered command 'helloworld' for guild ID: " << guild_id << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "WARNING: Could not parse DISCORD_GUILD_ID for greetings module: " << e.what() << std::endl;
            }
        } else {
            std::cout << "No guild ID available for guild-specific command registration" << std::endl;
        }
        
        // Also register globally (takes up to an hour to propagate)
        bot.global_command_create(hello);
        std::cout << "Registered global command: helloworld (may take up to an hour to appear)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception when creating hello command: " << e.what() << std::endl;
    }
    
    std::cout << "Greetings Module initialized successfully!" << std::endl;
}
