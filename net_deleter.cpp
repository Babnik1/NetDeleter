#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <array>
#include <tuple>
#include <windows.h>
#include <sstream>

#include "logging.h"
#include "type.h"


const std::tuple< ResultCommand, std::string > ExecCommand( std::string& command )
{
    std::array< char, 128 > buffer{};
    std::string result{};

    std::unique_ptr< FILE, decltype( &_pclose ) > pipe( _popen( command.c_str(), "r" ), _pclose );
    if ( !pipe )
    {
        return std::make_tuple( ResultCommand::Error, "" );
    }

    while ( fgets( buffer.data(), buffer.size(), pipe.get() ) != nullptr )
    {
        result += buffer.data();
    }
    return std::make_tuple( ResultCommand::Success, result );
}

std::vector< DeviceInfo > ParsePnputResult( const std::string& output )
{
    std::vector< DeviceInfo > devices{};
    std::stringstream ss( output );
    std::string line{};
    DeviceInfo currentDevice{};

    auto getValue = []( const std::string& l )
    {
        size_t pos = l.find( ":" );
        if ( pos == std::string::npos )
        {
            return std::string{ "" };
        }

        std::string val = l.substr( pos + 1 );
        val.erase( 0, val.find_first_not_of( " \t" ) );
        return val; 
    };

    while( std::getline( ss, line ) )
    {
        if ( !line.empty() && line.back() == '\r' )
        {
            line.pop_back();
        }

        if ( line.find( "Instance ID:" )  != std::string::npos )
        {
            if ( !currentDevice.instanceID.empty() ) 
            {
                devices.push_back( currentDevice );
            }

            currentDevice = {};
            currentDevice.instanceID = getValue( line );
        }
        else if ( line.find( "Class Name:" )  != std::string::npos )
        {
            currentDevice.className = getValue( line );
        }
        else if ( line.find( "Manufacturer Name:" ) != std::string::npos )
        {
            currentDevice.manufacturer = getValue( line );
        }
        else if ( line.find( "Device Description:" ) != std::string::npos )
        {
            currentDevice.devDescription = getValue( line );
        }
    }

    if ( !currentDevice.instanceID.empty() ) 
    {
        devices.push_back( currentDevice );
    }
    return devices;
}


int main( int argc, char* argv[] ) 
{
    try 
    {
        const std::string dataCommand = "pnputil /enum-devices /class Net";
        const std::string deleteCommand = "pnputil /remove-device";
        const std::string localeCommand = "chcp 65001";
        const std::string tunnelMarker = "ROOT\\WINTUN\\";

        std::string command = localeCommand + " && " + dataCommand;
        auto [ status, output ] = ExecCommand( command );
        if ( status == ResultCommand::Error )
        {
            ERROR_LOG( "Read execution error" );
        }
        DEBUG_LOG( "List: " << "\n" << output );

        std::vector< DeviceInfo > devices = ParsePnputResult( output );
        INFO_LOG( "Found " << devices.size() << " devices" );

        command = localeCommand + " && " + deleteCommand;
        int count = 0;
        for ( const auto& dev : devices ) 
        {
            DEBUG_LOG( "InstanceID: " << dev.instanceID << "\n"
                    << "ClassName: " << dev.className << "\n"
                    << "Manufacturer: " << dev.manufacturer << "\n"
                    << "Dev Description: " << dev.devDescription << "\n"
                    << "---------------------------\n" );

            if ( dev.instanceID.find( tunnelMarker ) != std::string::npos )
            {
                std::string fullDeleteCommand = command + " " + dev.instanceID;
                auto [ status, output ] = ExecCommand( fullDeleteCommand );
                INFO_LOG( "Result: " << output );
                if ( status == ResultCommand::Error || output.find( "Failed" ) )
                {
                    ERROR_LOG( "Failed to remove: " << dev.instanceID );
                    continue;
                }
                count++;
            }
        }

         INFO_LOG( "Deleted " << count << " tunnel devices" );
    } 
    catch ( const std::exception& e ) 
    {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}