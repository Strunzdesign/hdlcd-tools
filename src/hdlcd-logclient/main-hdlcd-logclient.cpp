/**
 * \file main-hdlcd-logclient.cpp
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
#include "HdlcdAccessClient.h"
#include "LogClientFormatter.h"

int main(int argc, char* argv[]) {
    try {
        std::cerr << "HDLCd Logclient to dump incoming payload packets together with UTC arrival time v" << HDLCD_TOOLS_VERSION_MAJOR << "." << HDLCD_TOOLS_VERSION_MINOR << std::endl;
        if (argc != 4) {
            std::cerr << "Usage: hdlcd-logclient <host> <port> <usb-device>\n";
            return 1;
        } // if

        // Install signal handlers
        boost::asio::io_service io_service;
        boost::asio::signal_set signals_(io_service);
        signals_.add(SIGINT);
        signals_.add(SIGTERM);
        signals_.async_wait([&io_service](boost::system::error_code a_ErrorCode, int a_SignalNumber){io_service.stop();});
        
        // Resolve destination
        boost::asio::ip::tcp::resolver resolver(io_service);
        auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
        
        // Prepare access protocol entity: 0x21 = Payload Raw RO, RX only, RECV_CTRL
        HdlcdAccessClient l_AccessClient(io_service, endpoint_iterator, argv[3], 0x21);
        l_AccessClient.SetOnDataCallback([](const HdlcdPacketData& a_PacketData){ PrintLogEntry(a_PacketData.GetData()); });
        l_AccessClient.SetOnClosedCallback([&io_service](){io_service.stop();});
        
        // Start event processing
        io_service.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    } // catch

    return 0;
}
