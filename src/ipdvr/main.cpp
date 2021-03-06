/* ipdvr - DVR for IPTV streams
 *
 * Copyright (C) 2015  Peter Helbing
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <contrib/cxxopts/cxxopts.hpp>
#include <ipdvr/config.h>
#include <ipdvr/listingdb.h>
#include <ipdvr/listingupdater.h>

#include <curl/curl.h>
#include <memory>

int main(int argc, char* argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);

    // setup options parser
    cxxopts::Options options("ipdvr", "ipdvr -- A digital video recorder for IPTV streams");
    options.add_options()
      ("c,config", "Configuration file", cxxopts::value<std::string>())
      ("h,help", "Print this help")
      ;
    try
    {
        options.parse(argc, argv);
    }
    catch (cxxopts::OptionParseException & e)
    {
        std::cout << "Error parsing command-line options: " << e.what() << std::endl;
        return 1;
    }

    // on --help / -h, print help and exit
    if (options.count("help"))
    {
        std::cout << options.help();
        return 0;
    }

    // try to parse config, exit if fail
    std::shared_ptr<ipdvr::Config> spConfig;
    if (options.count("config"))
    {
        spConfig = std::make_shared<ipdvr::Config>(options["config"].as<std::string>());
    }
    else
    {
        spConfig = std::make_shared<ipdvr::Config>();
    }
    if (spConfig->parse() == false)
    {
        return 1;
    }

    auto spListingDb = std::make_shared<ipdvr::ListingDb>();

    auto spListingUpdater = std::make_shared<ipdvr::ListingUpdater>(spConfig, spListingDb);
    spListingUpdater->updateAll();

    curl_global_cleanup();

    return 0;
}
