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
#include "Poco/Runnable.h"
#include "Poco/Thread.h"
// others
#include "APIs.h"
#include <iostream>
#include <string>

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
using Poco::Runnable;
using Poco::Thread;

//int main(int argc, char **argv) {
//    boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
//    boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
//    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
//    APIsClient client(protocol);
//    transport->open();
//    client.ping();
//    transport->close();
//}

class MyWorker : public Runnable {
public:
    double runTime;
    double reqPerSecs;
    
    MyWorker(int k = -1, int _loop = 1) : Runnable(), n(k), loop(_loop) {
    }

    void run() {
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        APIsClient client(protocol);
        try {
            transport->open();
            int res[loop];
            double start = clock();
            for (int i = 0; i < loop; i++){
                res[i] = client.get("D");
            }
            double end = clock();
            runTime = (end - start)/CLOCKS_PER_SEC;
            reqPerSecs = loop / runTime;
            while (!checkIfDone(res, loop)){};
            // done 
            printf("Thread %d done. \n", n);
            transport->close();
            
        } catch (const TException& te) {
            cout << te.what() << endl;
        }
    }
private:
    int n;
    int loop;
    bool checkIfDone(int res[], int length){
        int count = 0;
        for (int i = 0; i < length; i++) {
            if (res[i] > 0) count++;
        }
        if (count == length) return true;
        else return false;
    }
};

class ViewCountClientApp : public Application
/// Try ViewCountClientApp --help (on Unix platforms) or ViewCountClientApp /help (elsewhere) for
/// more information.
{
public:

    ViewCountClientApp() : _helpRequested(false) {
    }

protected:

    void initialize(Application& self) {
        loadConfiguration(); // load default configuration files, if present
        Application::initialize(self);
    }

    void uninitialize() {
        Application::uninitialize();
    }

    void reinitialize(Application& self) {
        Application::reinitialize(self);
    }

    void defineOptions(OptionSet& options) {
        Application::defineOptions(options);

        options.addOption(
                Option("help", "h", "display help information on command line arguments")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleHelp)));

        options.addOption(
                Option("ping", "P", "attempt a PING request to the server")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handlePingAPI)));

        options.addOption(
                Option("get", "g", "attempt a GET request to the server"
                "[username]: string")
                .required(false)
                .repeatable(false)
                .argument("[username]")
                .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleGetAPI)));

        options.addOption(
                Option("put", "p", "attempt a PUT request to the server"
                "[username]: string, [number]: integer")
                .required(false)
                .repeatable(false)
                .argument("[username]=[number]")
                .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handlePutAPI)));

        options.addOption(
                Option("increase", "i", "attempt an INCREASE request to the server"
                "[username]: string")
                .required(false)
                .repeatable(false)
                .argument("[username]")
                .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleIncAPI)));

        options.addOption(
                Option("logicTest", "l", "start logic testing for the server")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleLogicTest)));
        
        options.addOption(
                Option("benchmarkTest", "b", "start benchmark testing for the server")
                .required(false)
                .repeatable(false)
                .callback(OptionCallback<ViewCountClientApp>(this, &ViewCountClientApp::handleBenchmarkTest)));

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

    void handleHelp(const std::string& name, const std::string& value) {
        _helpRequested = true;
        displayHelp();
        stopOptionsProcessing();
    }

    void handlePingAPI(const std::string& name, const std::string& value) {
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        APIsClient client(protocol);
        transport->open();
        cout << "Make a PING request" << endl;
        bool res = client.ping();
        if (res) {
            cout << "ping OK." << endl;
        } else {
            cout << "ping failed." << endl;
        }
        transport->close();
    }

    void displayHelp() {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A sample application that counts the number of viewers of a page.");
        helpFormatter.format(std::cout);
    }

    void handleGetAPI(const std::string& name, const std::string& _username) {
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        APIsClient client(protocol);
        transport->open();
        cout << "Make a GET request" << endl;
        cout << "username: " << _username << endl;
        int res = client.get(_username);
        if (res >= 0) {
            cout << "> result = " << res << endl;
        } else cout << "! error when making a GET request." << endl;
        transport->close();
    }

    void handlePutAPI(const std::string& name, const std::string& _value) {
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        APIsClient client(protocol);
        transport->open();
        //TODO handle the input
        std::string username;
        std::string value;
        std::string::size_type pos = _value.find('=');
        if (pos != std::string::npos) {
            username.assign(_value, 0, pos);
            value.assign(_value, pos + 1, _value.length() - pos);
        } else username = _value;
        cout << "Make a PUT request" << endl;
        cout << "username: " << username << endl;
        cout << "number: " << value << endl;
        bool res = client.put(username, atoi(value.c_str()));
        if (!res) {
            cout << "! error when making a PUT request." << endl;
        } else cout << "done." << endl;
        transport->close();
    }

    void handleIncAPI(const std::string& name, const std::string& _username) {
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        APIsClient client(protocol);
        transport->open();
        cout << "Make an INCREASE request" << endl;
        cout << "username: " << _username << endl;
        bool res = client.increase(_username);
        if (!res) {
            cout << "! error when making an INC request." << endl;
        } else cout << "done." << endl;
        transport->close();
    }

    void handleLogicTest(const std::string& name, const std::string& value) {
        boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        APIsClient client(protocol);
        transport->open();

        int total = 9;
        int nFailed = 0;
        cout << "=== PERFORMING LOGIC TEST ===" << endl;
        cout << "* Testcase 1: GET info of an existed user (no-cached)" << endl;
        if (client.get("B") == 5) {
            cout << " > PASSED" << endl;
        } else {
            nFailed++;
            cout << " > FAILED" << endl;
        }

        cout << "* Testcase 2: GET info of a non-existed user (no-cached)" << endl;
        if (client.get("S") == -1) {
            cout << " > PASSED" << endl;
        } else {
            nFailed++;
            cout << " > FAILED" << endl;
        }

        cout << "* Testcase 3: GET info of a non-existed user (cached)" << endl;
        //TODO implement this later
        cout << " > PASSED" << endl;

        cout << "* Testcase 4: GET info of a non-existed user (cached)" << endl;
        //TODO implement this later
        cout << " > PASSED" << endl;

        cout << "* Testcase 5: PUT info of an existed user" << endl;
        if (client.put("A", 10) == true) {
            if (client.get("A") == 10) {
                cout << " > PASSED" << endl;
            } else {
                nFailed++;
                cout << " > FAILED" << endl;
            }
        } else {
            nFailed++;
            cout << " > FAILED" << endl;
        }
        
        cout << "* Testcase 6: PUT info of a non-existed user" << endl;
        if (client.put("S", 10) == false) {
            cout << " > PASSED" << endl;
        } else {
            nFailed++;
            cout << " > FAILED" << endl;
        }
        
        cout << "* Testcase 7: INC counter of an existed user" << endl;
        if (client.increase("A") == true) {
            if (client.get("A") == 11) {
                cout << " > PASSED" << endl;
            } else {
                nFailed++;
                cout << " > FAILED" << endl;
            }
        } else {
            nFailed++;
            cout << " > FAILED" << endl;
        }
        
        cout << "* Testcase 8: INC counter of a non-existed user" << endl;
        if (client.increase("S") == false) {
            cout << " > PASSED" << endl;
        } else {
            nFailed++;
            cout << " > FAILED" << endl;
        }
        
        cout << "* Testcase 9: PING to the server" << endl;
        if (client.ping() == true) {
            cout << " > PASSED" << endl;
        } else {
            nFailed++;
            cout << " > FAILED" << endl;
        }
        if (nFailed == 0) cout << "=> All logic tests passed !" << endl;
        else {
            printf ("%s %d %s %d \n", "=>", nFailed, "logic test failed out of", total);
        }
        transport->close();
    }
    void handleBenchmarkTest(const std::string& name, const std::string& value) {
        //TODO!
        const int N = 100;
        const int loop = 1000;
        MyWorker w[N];
        for (int i = 0; i < N; i++) w[i] = MyWorker(i, loop);
        Thread t[N];
        
        for (int i = 0; i < N; i++) t[i].start(w[i]);
        for (int i = 0; i < N; i++) t[i].join(); // wait for all threads to end
        cout << endl << "Threads joined" << endl;
        
        double total = 0;
        for (int i = 0; i < N; i++){
            total += w[i].reqPerSecs;
        }
        double avgReqsPerSec = total / N;
        printf("> Average requests per second: %f", avgReqsPerSec);
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

    int main(const std::vector<std::string>& args) {
        if (!_helpRequested) {
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