#include <TessesFramework/TessesFramework.hpp>

using namespace Tesses::Framework;
using namespace Tesses::Framework::Filesystem;
using namespace Tesses::Framework::Filesystem::Literals;

int main(int argc, char **argv) {
    TF_Init();
    if (argc < 2) {
        std::cout << "USAGE " << argv[0] << " <file|dir>" << std::endl;
        return 1;
    }

    auto watcher = FSWatcher::Create(LocalFS, VFSPath{argv[1]});
    watcher->events = FSWatcherEventType::All;
    watcher->event = [](FSWatcherEvent &evt) -> void {
        std::cout << evt.ToString() << std::endl;
    };
    watcher->SetEnabled(true);
    TF_RunEventLoop();
    return 0;
}