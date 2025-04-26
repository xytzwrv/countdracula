#include <dpp/dpp.h>
#include "modules/greetings_module.hpp"
#include "modules/chess/chess_module.hpp"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <chrono>

int main() {
    std::cout << "=== Count Dracula Bot Starting ===" << std::endl;
    
    // Get token and guild ID from environment variables
    const char* token = std::getenv("DISCORD_BOT_TOKEN");
    const char* guild_id_str = std::getenv("DISCORD_GUILD_ID");

    if (!token) {
        std::cerr << "ERROR: DISCORD_BOT_TOKEN environment variable was not set." << std::endl;
        return 1;
    }
    
    std::cout << "Token loaded successfully" << std::endl;
    
    // Parse guild ID if provided
    dpp::snowflake guild_id = 0;
    if (guild_id_str) {
        try {
            guild_id = std::stoull(guild_id_str);
            std::cout << "Guild ID loaded from environment: " << guild_id << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "WARNING: Could not parse DISCORD_GUILD_ID as a number: " << e.what() << std::endl;
        }
    } else {
        std::cout << "WARNING: DISCORD_GUILD_ID environment variable not set. Guild-specific commands will not be registered." << std::endl;
    }
    
    // Create a timestamp for our logs
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::cout << "Bot startup time: " << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S") << std::endl;

    // Create bot with specific intents, only requesting what we need
    dpp::cluster bot(token, dpp::i_guilds | dpp::i_guild_messages | dpp::i_message_content);
    
    std::cout << "Bot cluster created with minimal required intents" << std::endl;

    // Set up logging with extra error information
    bot.on_log([](const dpp::log_t& event) {
        std::cout << "[LOG] " << event.message << std::endl;
        
        // Check for intent-related errors in the log message
        if (event.message.find("Disallowed intent") != std::string::npos) {
            std::cerr << "\n===== IMPORTANT INTENT ERROR =====" << std::endl;
            std::cerr << "Your bot is trying to use intents that are not enabled in the Discord Developer Portal." << std::endl;
            std::cerr << "Please go to https://discord.com/developers/applications" << std::endl;
            std::cerr << "Select your application, go to the 'Bot' tab, and enable the following:" << std::endl;
            std::cerr << "- MESSAGE CONTENT INTENT" << std::endl;
            std::cerr << "==================================\n" << std::endl;
        }
    });
    
    // Register for guild-specific events
    bot.on_guild_create([](const dpp::guild_create_t& event) {
        std::cout << "Bot added to guild: " << event.created.name << " (ID: " << event.created.id << ")" << std::endl;
        std::cout << "Guild member count: " << event.created.member_count << std::endl;
    });

    // Initialize modules
    GreetingsModule greetings(bot);
    ChessModule chess(bot);
    
    std::cout << "\n=== IMPORTANT INFORMATION ===" << std::endl;
    std::cout << "When inviting your bot to a server, make sure to use an invite URL that includes BOTH the 'bot' and 'applications.commands' scopes." << std::endl;
    std::cout << "Example invite URL format:" << std::endl;
    std::cout << "https://discord.com/api/oauth2/authorize?client_id=YOUR_CLIENT_ID&permissions=8&scope=applications.commands%20bot" << std::endl;
    std::cout << "Replace YOUR_CLIENT_ID with your actual bot's client ID." << std::endl;
    std::cout << "Also make sure to set the environment variables:" << std::endl;
    std::cout << "  export DISCORD_BOT_TOKEN=your_token_here" << std::endl;
    std::cout << "  export DISCORD_GUILD_ID=your_server_id_here" << std::endl;
    std::cout << "==========================\n" << std::endl;

    // Set up ready handler
    bot.on_ready([&bot, guild_id](const dpp::ready_t& event) {
        std::cout << "========================================" << std::endl;
        std::cout << "Bot is ready! Logged in as " << bot.me.username << " (ID: " << bot.me.id << ")" << std::endl;
        std::cout << "Connected to " << event.guilds.size() << " guilds" << std::endl;
        std::cout << "Privileged intents status: " << std::endl;
        std::cout << "  Message Content: " << ((bot.intents & dpp::i_message_content) ? "Enabled" : "Disabled") << std::endl;
        
        // Set presence (activity)
        bot.set_presence(dpp::presence(dpp::ps_online, dpp::at_game, "Chess and more!"));
        std::cout << "Activity status set to: 'Chess and more!'" << std::endl;
        
        // Register guild-specific commands for faster testing
        // This is faster than global commands which can take up to an hour to propagate
        if (guild_id > 0) {
            std::cout << "Creating test commands for guild ID: " << guild_id << std::endl;
            
            dpp::slashcommand test_cmd("test", "A test command", bot.me.id);
            bot.guild_command_create(test_cmd, guild_id);
        } else {
            std::cout << "No guild ID set, skipping guild-specific command registration" << std::endl;
        }
        
        std::cout << "========================================" << std::endl;
    });

    std::cout << "Starting bot..." << std::endl;
    try {
        bot.start(dpp::st_wait);
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Bot crashed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Bot has been stopped." << std::endl;
    return 0;
}
