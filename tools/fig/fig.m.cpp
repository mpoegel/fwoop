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
    fwoop::DNS::Question question(url, fwoop::DNS::RecordType::A, fwoop::DNS::ClassValue::IN);
    fwoop::Log::Debug("question: ", question);
    auto record = fwoop::DNS::Query::GetRecord(question);

    if (record == nullptr) {
        std::cerr << "record not found\n";
    } else {
        fwoop::Log::Debug("record: ", record->toString());
        std::cout << record->IP() << "\n";
    }

    return 0;
}
