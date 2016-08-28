/**
 * \file main-hdlcd-portkiller.cpp
 * \brief 
 *
 * Additional tools to be used together with the HDLC Daemon.
 * Copyright (C) 2016  Florian Evers, florian-evers@gmx.de
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Config.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include "HdlcdAccessClient.h"
#include "HdlcdPacketCtrl.h"

int main(int argc, char* argv[]) {
    try {
        // Declare the supported options.
        boost::program_options::options_description l_Description("Allowed options");
        l_Description.add_options()
            ("help,h",    "produce this help message")
            ("version,v", "show version information")
            ("connect,c", boost::program_options::value<std::string>(),
                          "connect to a single device via the HDLCd\n"
                          "syntax: SerialPort@IPAddess:PortNbr\n"
                          "  linux:   /dev/ttyUSB0@localhost:5001\n"
                          "  windows: //./COM1@example.com:5001")
        ;

        // Parse the command line
        boost::program_options::variables_map l_VariablesMap;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, l_Description), l_VariablesMap);
        boost::program_options::notify(l_VariablesMap);
        if (l_VariablesMap.count("version")) {
            std::cerr << "HDLCd port killer version " << HDLCD_TOOLS_VERSION_MAJOR << "." << HDLCD_TOOLS_VERSION_MINOR
                      << " built with hdlcd-devel version " << HDLCD_DEVEL_VERSION_MAJOR << "." << HDLCD_DEVEL_VERSION_MINOR << std::endl;
        } // if

        if (l_VariablesMap.count("help")) {
            std::cout << l_Description << std::endl;
            return 1;
        } // if
        
        if (!l_VariablesMap.count("connect")) {
            std::cout << "you have to specify one device to connect to" << std::endl;
            return 1;
        } // if

        // Install signal handlers
        boost::asio::io_service l_IoService;
        boost::asio::signal_set l_Signals(l_IoService);
        l_Signals.add(SIGINT);
        l_Signals.add(SIGTERM);
        l_Signals.async_wait([&l_IoService](boost::system::error_code, int){ l_IoService.stop(); });
        
        // Parse the destination specifier
        static boost::regex s_RegEx("^(.*?)@(.*?):(.*?)$");
        boost::smatch l_Match;
        if (boost::regex_match(l_VariablesMap["connect"].as<std::string>(), l_Match, s_RegEx)) {
            // Resolve destination
            boost::asio::ip::tcp::resolver l_Resolver(l_IoService);
            auto l_EndpointIterator = l_Resolver.resolve({ l_Match[2], l_Match[3] });
            
            // Prepare access protocol entity: 0x10: Port status only, no data exchange, port status read and write
            HdlcdAccessClient l_AccessClient(l_IoService, l_EndpointIterator, l_Match[1], 0x10);
            l_AccessClient.SetOnClosedCallback([&l_IoService](){ l_IoService.stop(); });
            l_AccessClient.Send(std::move(HdlcdPacketCtrl::CreatePortKillRequest()));
            
            // Start event processing
            l_IoService.run();
        } else {
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value);
        } // else
    } catch (std::exception& a_Error) {
        std::cerr << "Exception: " << a_Error.what() << "\n";
    } // catch

    return 0;
}
