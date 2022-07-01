#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QStringList>
#include <memory>

#include "BrowserExtension.hpp"
#include "RunGui.hpp"
#include "common/Args.hpp"
#include "common/Modes.hpp"
#include "common/QLogging.hpp"
#include "common/Version.hpp"
#include "providers/IvrApi.hpp"
#include "providers/twitch/api/GQL.hpp"
#include "providers/twitch/api/Helix.hpp"
#include "singletons/Paths.hpp"
#include "singletons/Settings.hpp"
#include "util/AttachToConsole.hpp"
#include "util/IncognitoBrowser.hpp"


using namespace chatterino;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("featrino");
    QCoreApplication::setApplicationVersion(CHATTERINO_VERSION);
    QCoreApplication::setOrganizationDomain("https://featrino.com");

    Paths *paths{};

    try
    {
        paths = new Paths;
    }
    catch (std::runtime_error &error)
    {
        QMessageBox box;
        if (Modes::instance().isPortable)
        {
            box.setText(
                error.what() +
                QStringLiteral(
                    "\n\nInfo: Portable mode requires the application to "
                    "be in a writeable location. If you don't want "
                    "portable mode reinstall the application. "
                    "https://chatterino.com."));
        }
        else
        {
            box.setText(error.what());
        }
        box.exec();
        return 1;
    }

    initArgs(a);

    // run in gui mode or browser extension host mode
    if (getArgs().shouldRunBrowserExtensionHost)
    {
        runBrowserExtensionHost();
    }
    else if (getArgs().printVersion)
    {
        attachToConsole();

        auto version = Version::instance();
        qInfo().noquote() << QString("%1 (commit %2%3)")
                                 .arg(version.fullVersion())
                                 .arg(version.commitHash())
                                 .arg(Modes::instance().isNightly
                                          ? ", " + version.dateOfBuild()
                                          : "");
    }
    else
    {
        if (getArgs().verbose)
        {
            attachToConsole();
        }

        IvrApi::initialize();
        Helix::initialize();
        GQL::initialize();

        Settings settings(paths->settingsDirectory);

        runGui(a, *paths, settings);
    }
    return 0;
}
