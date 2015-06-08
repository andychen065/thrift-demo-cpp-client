/*
 * INCLUDES
 */
// Thrift
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>
// POCO
#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/AutoPtr.h"
#include <iostream>
// others
#include "APIs.h"
#include <iostream>

/*
 * USINGS
 */
// Thrift
using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace ::thriftDemo;
// POCO
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::AutoPtr;

//int main(int argc, char **argv) {
//    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
//    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
//    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
//    APIsClient client(protocol);
//    transport->open();
//    client.ping();
//    transport->close();
//}

class ViewCountClientApp: public Application
	/// Try ViewCountClientApp --help (on Unix platforms) or ViewCountClientApp /help (elsewhere) for
	/// more information.
{
public:
	ViewCountClientApp(): _helpRequested(false)
	{
	}

protected:	
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
		Application::initialize(self);
	}
	
	void uninitialize()
	{
		Application::uninitialize();
	}
	
	void reinitialize(Application& self)
	{
		Application::reinitialize(self);
	}
	
	void defineOptions(OptionSet& options)
	{
		Application::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
				.required(false)
				.repeatable(false)
				.callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleHelp)));
                
                options.addOption(
                        Option("mode", "m", "select a run mode for the ViewCount client\n"
                        "if using the get API => [api]=get\n"
                        "if using the put API => [api]=put\n"
                        "if using the increase API => [api]=increase\n"
                        "if using the ping API => [api]=ping\n")
                            .required(false)
                            .repeatable(false)
                            .argument("[api]")
                            .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleMode)));
		
//                options.addOption(
//			Option("define", "D", "define a configuration property")
//				.required(false)
//				.repeatable(true)
//				.argument("name=value")
//				.callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleDefine)));
//				
//		options.addOption(
//			Option("config-file", "f", "load configuration data from a file")
//				.required(false)
//				.repeatable(true)
//				.argument("file")
//				.callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleConfig)));
//
//		options.addOption(
//			Option("bind", "b", "bind option value to test.property")
//				.required(false)
//				.repeatable(false)
//				.argument("value")
//				.binding("test.property"));
	}
	
	void handleHelp(const std::string& name, const std::string& value)
	{
		_helpRequested = true;
		displayHelp();
		stopOptionsProcessing();
	}
        
        void displayHelp()
	{
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("A sample application that counts the number of viewers of a page.");
		helpFormatter.format(std::cout);
	}
	
        void handleMode(const std::string& name, const std::string& value)
        {
            if (value == "get"){
                doAPIs("get");
            }
            else if (value == "put"){
                doAPIs("put");
            }
            else if (value == "increase"){
                doAPIs("increase");
            }
            else if (value == "ping"){
                doAPIs("ping");
            }
            else {
                logger().error("Please enter 1 of the 4 APIs: put/get/increase/ping !");
            }
        }
        void doAPIs(string api) {
            boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
            boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
            boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
            APIsClient client(protocol);
            transport->open();
            if (api == "get"){
                logger().information("client.get()");
                client.get("username");
            }
            else if (api == "put"){
                logger().information("client.put()");
                client.put("username", 10);
            }
            else if (api == "increase"){
                logger().information("client.increase()");
                client.increase("username");
            }
            else if (api == "ping"){
                logger().information("client.ping()");
                client.ping();
            }
            transport->close();
        }
        
//	void handleDefine(const std::string& name, const std::string& value)
//	{
//		defineProperty(value);
//	}
//	
//	void handleConfig(const std::string& name, const std::string& value)
//	{
//		loadConfiguration(value);
//	}
//	
//	void defineProperty(const std::string& def)
//	{
//		std::string name;
//		std::string value;
//		std::string::size_type pos = def.find('=');
//		if (pos != std::string::npos)
//		{
//			name.assign(def, 0, pos);
//			value.assign(def, pos + 1, def.length() - pos);
//		}
//		else name = def;
//		config().setString(name, value);
//	}

	int main(const std::vector<std::string>& args)
	{
		if (!_helpRequested)
		{       
                        // print out the applied arguments
//			logger().information("Arguments to main():");
//			for (std::vector<std::string>::const_iterator it = args.begin(); it != args.end(); ++it)
//			{
//				logger().information(*it);
//			}
//			logger().information("Application properties:");
//			printProperties("");
		}
		return Application::EXIT_OK;
	}
	
//	void printProperties(const std::string& base)
//	{
//		AbstractConfiguration::Keys keys;
//		config().keys(base, keys);
//		if (keys.empty())
//		{
//			if (config().hasProperty(base))
//			{
//				std::string msg;
//				msg.append(base);
//				msg.append(" = ");
//				msg.append(config().getString(base));
//				logger().information(msg);
//			}
//		}
//		else
//		{
//			for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
//			{
//				std::string fullKey = base;
//				if (!fullKey.empty()) fullKey += '.';
//				fullKey.append(*it);
//				printProperties(fullKey);
//			}
//		}
//	}
	
private:
	bool _helpRequested;
};


POCO_APP_MAIN(ViewCountClientApp)