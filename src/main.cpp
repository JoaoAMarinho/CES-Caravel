#include "../include/main.h"

namespace po = boost::program_options;
using namespace std;

#define RECEIVE_CMD 1
#define SEND_CMD 0

int main(int argc, char* argv[])
{

    //sudo ip link set can0 type can bitrate 500000
    //sudo ifconfig can0 up

    try {
        string file_path;

        // Declare command line (generic) options 
        po::options_description generic("Generic options");
        generic.add_options()
            ("help,h", "Produce help message")
            ("file,f", po::value<string>(&file_path), "File path")  
        ;

        // Declare the supported options on the command line and file
        po::options_description config("Configuration");
        config.add_options()
            ("command", po::value<string>(), "Desired command (send, receive, interactive)")
            ("id", po::value<int>(), "Message id (eg: 123)")
            ("message,m", po::value<string>() ,"Message data in hex (eg: \"DEADBEEF\")")
            ("cyclic,C", po::value<double>()->notifier([](double v){ 
                if(v < 1.0 || v > 5.0){ 
                    throw po::validation_error(po::validation_error::invalid_option_value, "cyclic", to_string(v));
                }
            }) , "[opt] Cyclic messaging [1,5]")
        ;
        
        // Declare the positional options
        po::positional_options_description p;
        p.add("command", 1);


        po::options_description visible("Allowed options");
        visible.add(generic).add(config);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).
                    options(visible).positional(p).run(), vm);
        po::notify(vm);
        if(!file_path.empty()){
            vm.clear();
            po::store(po::parse_config_file(file_path.c_str(), visible), vm);
            po::notify(vm);
        }
    
        if (vm.count("help")) {
            cout << visible << endl;
            return 0;
        }
        if (vm.count("command")) {
            string cmd = vm["command"].as<string>();

            if(cmd=="send"){
                 
                if(vm.count("id")&&vm.count("message")){
                    run(SEND_CMD, vm["id"].as<int>(), vm["message"].as<string>(), vm.count("cyclic") ? vm["cyclic"].as<double>() : -1);
                    return 0;
                }
                cout << "Missing id or message data parameter." << endl;
                return 1;
            }

            if(cmd=="receive"){                
                run(RECEIVE_CMD);
                return 0;
            }

            if(cmd=="interactive"){                
                cout << "Interactive task!\n" << endl;
                return run_interactive();
            }
            
            cout << "Invalid command option" << endl;
            return 1;
        }
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }

    return 0;

    //sudo ifconfig can0 down
}