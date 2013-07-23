//------------------------------------------------------------------------------
/*
    Copyright (c) 2011-2013, OpenCoin, Inc.
*/
//==============================================================================

namespace po = boost::program_options;

void setupServer ()
{
#ifdef RLIMIT_NOFILE
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == 0)
    {
         if (rl.rlim_cur != rl.rlim_max)
         {
             rl.rlim_cur = rl.rlim_max;
             setrlimit(RLIMIT_NOFILE, &rl);
         }
    }
#endif

    getApp().setup ();
}

void startServer ()
{
    //
    // Execute start up rpc commands.
    //
    if (theConfig.RPC_STARTUP.isArray ())
    {
        for (int i = 0; i != theConfig.RPC_STARTUP.size (); ++i)
        {
            const Json::Value& jvCommand    = theConfig.RPC_STARTUP[i];

            if (!theConfig.QUIET)
                Log::out() << "Startup RPC: " << jvCommand;

            RPCHandler  rhHandler (&getApp().getOPs ());

            // VFALCO TODO Clean up this magic number
            LoadType loadType = LT_RPCReference;
            Json::Value jvResult    = rhHandler.doCommand (jvCommand, RPCHandler::ADMIN, &loadType);

            if (!theConfig.QUIET)
                Log::out() << "Result: " << jvResult;
        }
    }

    getApp().run ();                 // Blocks till we get a stop RPC.
}

bool init_unit_test ()
{
    getApp ();

    return true;
}

void printHelp (const po::options_description& desc)
{
    using namespace std;

    cerr << SYSTEM_NAME "d [options] <command> <params>" << endl;

    cerr << desc << endl;

    cerr << "Commands: " << endl;
    cerr << "     account_info <account>|<nickname>|<seed>|<pass_phrase>|<key> [<ledger>] [strict]" << endl;
    cerr << "     account_lines <account> <account>|\"\" [<ledger>]" << endl;
    cerr << "     account_offers <account>|<nickname>|<account_public_key> [<ledger>]" << endl;
    cerr << "     account_tx accountID [ledger_min [ledger_max [limit [offset]]]] [binary] [count] [descending]" << endl;
    cerr << "     book_offers <taker_pays> <taker_gets> [<taker [<ledger> [<limit> [<proof> [<marker>]]]]]" << endl;
    cerr << "     connect <ip> [<port>]" << endl;
    cerr << "     consensus_info" << endl;
#if ENABLE_INSECURE
    cerr << "     data_delete <key>" << endl;
    cerr << "     data_fetch <key>" << endl;
    cerr << "     data_store <key> <value>" << endl;
#endif
    cerr << "     get_counts" << endl;
    cerr << "     json <method> <json>" << endl;
    cerr << "     ledger [<id>|current|closed|validated] [full]" << endl;
    cerr << "     ledger_accept" << endl;
    cerr << "     ledger_closed" << endl;
    cerr << "     ledger_current" << endl;
    cerr << "     ledger_header <ledger>" << endl;
    cerr << "     logrotate " << endl;
    cerr << "     peers" << endl;
    cerr << "     proof_create [<difficulty>] [<secret>]" << endl;
    cerr << "     proof_solve <token>" << endl;
    cerr << "     proof_verify <token> <solution> [<difficulty>] [<secret>]" << endl;
    cerr << "     random" << endl;
    cerr << "     ripple ..." << endl;
    cerr << "     ripple_path_find <json> [<ledger>]" << endl;
    //  cerr << "     send <seed> <paying_account> <account_id> <amount> [<currency>] [<send_max>] [<send_currency>]" << endl;
    cerr << "     stop" << endl;
    cerr << "     tx <id>" << endl;
    cerr << "     unl_add <domain>|<public> [<comment>]" << endl;
    cerr << "     unl_delete <domain>|<public_key>" << endl;
    cerr << "     unl_list" << endl;
    cerr << "     unl_load" << endl;
    cerr << "     unl_network" << endl;
    cerr << "     unl_reset" << endl;
    cerr << "     validation_create [<seed>|<pass_phrase>|<key>]" << endl;
    cerr << "     validation_seed [<seed>|<pass_phrase>|<key>]" << endl;
    cerr << "     wallet_add <regular_seed> <paying_account> <master_seed> [<initial_funds>] [<account_annotation>]" << endl;
    cerr << "     wallet_accounts <seed>" << endl;
    cerr << "     wallet_claim <master_seed> <regular_seed> [<source_tag>] [<account_annotation>]" << endl;
    cerr << "     wallet_seed [<seed>|<passphrase>|<passkey>]" << endl;
    cerr << "     wallet_propose [<passphrase>]" << endl;

    // Transaction helpers (that were removed):
    //  cerr << "     account_domain_set <seed> <paying_account> [<domain>]" << endl;
    //  cerr << "     account_email_set <seed> <paying_account> [<email_address>]" << endl;
    //  cerr << "     account_rate_set <seed> <paying_account> <rate>" << endl;
    //  cerr << "     account_wallet_set <seed> <paying_account> [<wallet_hash>]" << endl;
    //  cerr << "     nickname_info <nickname>" << endl;
    //  cerr << "     nickname_set <seed> <paying_account> <nickname> [<offer_minimum>] [<authorization>]" << endl;
    //  cerr << "     offer_create <seed> <paying_account> <taker_pays_amount> <taker_pays_currency> <taker_pays_issuer> <takers_gets_amount> <takers_gets_currency> <takers_gets_issuer> <expires> [passive]" << endl;
    //  cerr << "     offer_cancel <seed> <paying_account> <sequence>" << endl;
    //  cerr << "     password_fund <seed> <paying_account> [<account>]" << endl;
    //  cerr << "     password_set <master_seed> <regular_seed> [<account>]" << endl;
    //  cerr << "     trust_set <seed> <paying_account> <destination_account> <limit_amount> <currency> [<quality_in>] [<quality_out>]" << endl;
}

//------------------------------------------------------------------------------

// OUr custom unit test runner
class RippleUnitTests : public UnitTests
{
public:
    void logMessage (String const& message)
    {
        Log::out () << message.toStdString ();
    }
};

//------------------------------------------------------------------------------

/** Run the Beast unit tests.
*/
static void runBeastUnitTests ()
{
    RippleUnitTests tr;

    tr.setAssertOnFailure (false);
    tr.setPassesAreLogged (false);

    tr.runAllTests ();

    // Report
    for (int i = 0; i < tr.getNumResults (); ++i)
    {
        UnitTests::TestResult const& r (*tr.getResult (i));
                
        for (int j = 0; j < r.messages.size (); ++i)
            Log::out () << r.messages [j].toStdString ();
    }
}

//------------------------------------------------------------------------------

/** Run the Boost unit tests.

    @note These are deprecated. We want to migrate to using only
          the Beast unit testing framework. Please do not add more
          Boost based unit tests.
*/
// VFALCO NOTE What are argc and argv for?
//             Where does the boost unit test framework write its output?
//
static void runBoostUnitTests (int argc, char* argv [])
{
    // DEPRECATED
    boost::unit_test::unit_test_main (init_unit_test, argc, argv);
}

//------------------------------------------------------------------------------

int rippleMain (int argc, char** argv)
{
    //
    // These debug heap calls do nothing in release or non Visual Studio builds.
    //

    // Checks the heap at every allocation and deallocation (slow).
    //
    //Debug::setAlwaysCheckHeap (false);

    // Keeps freed memory blocks and fills them with a guard value.
    //
    //Debug::setHeapDelayedFree (false);

    // At exit, reports all memory blocks which have not been freed.
    //
#if 1
    Debug::setHeapReportLeaks (false);

#else
    // This is some temporary leak checking test code
    //
    Debug::setHeapReportLeaks (false);

    //malloc (512); // Any leaks before this line in the output are from static initializations.

    ThreadWithCallQueue t ("test");
    GlobalPagedFreeStore::getInstance ();
    t.start ();

    return 0;
#endif

    using namespace std;

    setCallingThreadName ("main");
    int iResult = 0;
    po::variables_map   vm;                                     // Map of options.

    // VFALCO TODO Replace boost program options with something from Beast.
    //
    // Set up option parsing.
    //
    po::options_description desc ("General Options");
    desc.add_options ()
    ("help,h", "Display this message.")
    ("conf", po::value<std::string> (), "Specify the configuration file.")
    ("rpc", "Perform rpc command (default).")
    ("rpc_ip", po::value <std::string> (), "Specify the IP address for RPC command. Format: <ip-address>[':'<port-number>]")
    ("rpc_port", po::value <int> (), "Specify the port number for RPC command.")
    ("standalone,a", "Run with no peers.")
    ("testnet,t", "Run in test net mode.")
    ("unittest,u", "Perform unit tests.")
    ("unittest2", "Perform new unit tests.")
    ("parameters", po::value< vector<string> > (), "Specify comma separated parameters.")
    ("quiet,q", "Reduce diagnotics.")
    ("verbose,v", "Verbose logging.")
    ("load", "Load the current ledger from the local DB.")
    ("replay","Replay a ledger close.")
    ("ledger", po::value<std::string> (), "Load the specified ledger and start from .")
    ("start", "Start from a fresh Ledger.")
    ("net", "Get the initial ledger from the network.")
    ("fg", "Run in the foreground.")
    ("import", po::value<std::string> (), "Import old DB into new DB.")
    ;

    // Interpret positional arguments as --parameters.
    po::positional_options_description p;
    p.add ("parameters", -1);

    // These must be added before the Application object is created
    NodeStore::addBackendFactory (SqliteBackendFactory::getInstance ());
    NodeStore::addBackendFactory (LevelDBBackendFactory::getInstance ());
#if RIPPLE_HYPERLEVELDB_AVAILABLE
    NodeStore::addBackendFactory (HyperLevelDBBackendFactory::getInstance ());
#endif
#if RIPPLE_MDB_AVAILABLE
    NodeStore::addBackendFactory (MdbBackendFactory::getInstance ());
#endif

    if (! RandomNumbers::getInstance ().initialize ())
    {
        Log::out() << "Unable to add system entropy";
        iResult = 2;
    }

    if (iResult)
    {
        nothing ();
    }
    else
    {
        // Parse options, if no error.
        try
        {
            po::store (po::command_line_parser (argc, argv)
                       .options (desc)                                         // Parse options.
                       .positional (p)                                         // Remainder as --parameters.
                       .run (),
                       vm);
            po::notify (vm);                                            // Invoke option notify functions.
        }
        catch (...)
        {
            iResult = 1;
        }
    }

    if (iResult)
    {
        nothing ();
    }
    else if (vm.count ("help"))
    {
        iResult = 1;
    }

    if (HaveSustain () &&
            !iResult && !vm.count ("parameters") && !vm.count ("fg") && !vm.count ("standalone") && !vm.count ("unittest"))
    {
        std::string logMe = DoSustain (theConfig.DEBUG_LOGFILE.c_str());

        if (!logMe.empty ())
            Log (lsWARNING) << logMe;
    }

    if (vm.count ("quiet"))
    {
        Log::setMinSeverity (lsFATAL, true);
    }
    else if (vm.count ("verbose"))
    {
        Log::setMinSeverity (lsTRACE, true);
    }
    else
    {
        Log::setMinSeverity (lsINFO, true);
    }

    // Run the unit tests if requested.
    //
    if (vm.count ("unittest"))
    {
        runBeastUnitTests ();

        // DEPRECATED
        runBoostUnitTests (argc, argv);
        return 0;
    }

    if (vm.count ("unittest2"))
    {
        runBeastUnitTests ();
        return 0;
    }

    if (!iResult)
    {
        theConfig.setup (
            vm.count ("conf") ? vm["conf"].as<std::string> () : "", // Config file.
            !!vm.count ("testnet"),                                 // Testnet flag.
            !!vm.count ("quiet"));                                  // Quiet flag.

        if (vm.count ("standalone"))
        {
            theConfig.RUN_STANDALONE = true;
            theConfig.LEDGER_HISTORY = 0;
        }
    }

    if (vm.count ("start")) theConfig.START_UP = Config::FRESH;

    if (vm.count ("import"))
        theConfig.DB_IMPORT = vm["import"].as<std::string> ();

    if (vm.count ("ledger"))
    {
        theConfig.START_LEDGER = vm["ledger"].as<std::string> ();
        if (vm.count("replay"))
            theConfig.START_UP = Config::REPLAY;
        else
            theConfig.START_UP = Config::LOAD;
    }
    else if (vm.count ("load"))
    {
        theConfig.START_UP = Config::LOAD;
    }
    else if (vm.count ("net"))
    {
        theConfig.START_UP = Config::NETWORK;

        if (theConfig.VALIDATION_QUORUM < 2)
            theConfig.VALIDATION_QUORUM = 2;
    }

    if (iResult == 0)
    {
        // These overrides must happen after the config file is loaded.

        // Override the RPC destination IP address
        //
        if (vm.count ("rpc_ip"))
        {
            theConfig.setRpcIpAndOptionalPort (vm ["rpc_ip"].as <std::string> ());
        }

        // Override the RPC destination port number
        //
        if (vm.count ("rpc_port"))
        {
            // VFALCO TODO This should be a short.
            theConfig.setRpcPort (vm ["rpc_port"].as <int> ());
        }
    }

    if (iResult)
    {
        nothing ();
    }
    else if (!vm.count ("parameters"))
    {
        // No arguments. Run server.
        setupServer ();
        setCallingThreadName ("io");
        startServer ();
    }
    else
    {
        // Have a RPC command.
        setCallingThreadName ("rpc");
        std::vector<std::string> vCmd   = vm["parameters"].as<std::vector<std::string> > ();

        iResult = commandLineRPC (vCmd);
    }

    if (1 == iResult && !vm.count ("quiet"))
        printHelp (desc);

    return iResult;
}
// vim:ts=4