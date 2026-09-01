#include <TessesFramework/TessesFramework.hpp>

int main(int argc, char **argv) {
    using namespace Tesses::Framework;
    using namespace Tesses::Framework::TextStreams;
    Tesses::Framework::TF_InitWithConsole();
    StdOut() << "TimeZone Standard Offset: "
             << (int64_t)(Date::GetTimeZone() / 3600) << NewLine();
    StdOut() << (Date::TimeZoneSupportDST() ? "TimeZone Supports DST"
                                            : "TimeZone Doesn't Support DST")
             << NewLine();

    return 0;
}