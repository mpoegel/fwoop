#include <fwoop_argparser.h>
#include <fwoop_dnsquery.h>
#include <fwoop_log.h>

#include <system_error>

int main(int argc, const char *argv[])
{
    auto args = fwoop::ArgParser(argv, argc);
    args.addPositionalArg("url", "", "URL of request");
    args.addNamedArg("verbose", "v", false, "enable verbose logs");

    std::error_code ec = args.parse();
    if (ec) {
        fwoop::Log::Error("invalid argument: ", ec.message());
        return 1;
    }
    bool isVerbose = args.getNamedArg<bool>("verbose");
    if (isVerbose) {
        fwoop::Log::SetThreshold(fwoop::Log::e_Debug);
    } else {
        fwoop::Log::SetThreshold(fwoop::Log::e_Info);
    }

    fwoop::Log::Debug("arguments", args);

    std::string url = args.getPositionalArg<std::string>("url");
    auto record = fwoop::DNS::Query::getRecord(url);

    if (record == nullptr) {
        std::cerr << "record not found\n";
    } else {
        std::cout << record->IP() << "\n";
    }

    return 0;
}
