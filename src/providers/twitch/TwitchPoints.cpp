#include "providers/twitch/TwitchPoints.hpp"

#include "Application.hpp"
#include "common/QLogging.hpp"
#include "providers/twitch/TwitchIrcServer.hpp"
#include "providers/twitch/api/GQL.hpp"
#include "singletons/Paths.hpp"
#include "singletons/WindowManager.hpp"
#include "util/PostToThread.hpp"
#include "widgets/Notebook.hpp"
#include "widgets/Window.hpp"
#include "widgets/helper/NotebookTab.hpp"
#include "widgets/splits/Split.hpp"
#include "widgets/splits/SplitContainer.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

namespace chatterino {

void TwitchPoints::run()
{
    this->thread.start();
}

void TwitchPoints::TwitchPointsThread::run()
{
    while (true)
    {
        try
        {
            // Get all tabs the user has open
            // NOTE: these should be in top to bottom order
            QList<SplitContainer *> pages;
            auto &nb = getApp()->getWindows()->getMainWindow().getNotebook();
            for (int i = 0; i < nb.getPageCount(); ++i)
                pages.append(static_cast<SplitContainer *>(nb.getPageAt(i)));

            // For each tab get what channels we have
            std::map<std::string, bool> channels;
            for (auto *sc : pages)
            {
                const QString &tabTitle = sc->getTab()->getTitle();
                const auto splits = sc->getSplits();

                // First, check for splits on this page
                for (auto *split : splits)
                {
                    auto name = split->getChannel()->getName().toStdString();
                    channels[name] = split->getChannel()->isLive();
                }
            }

            // Now for each channel, lets get how many points they have
            for (auto const &chanpair : channels)
            {
                auto chan_name = chanpair.first;
                getGQL()->getUserPoints(
                    QString::fromUtf8(chan_name.c_str()),
                    [chan_name](int points) {
                        getApp()->twitchPoints->appendLog(
                            {QDateTime::currentDateTime(),
                             QString::fromUtf8(chan_name.c_str()),
                             "Total of " + QString::number(points) +
                                 " points"});
                    },
                    [chan_name]() {
                        getApp()->twitchPoints->appendLog(
                            {QDateTime::currentDateTime(),
                             QString::fromUtf8(chan_name.c_str()),
                             "Unable to retrieve points, is your oauth cookie "
                             "right?"});
                    });
            }
        }
        catch (const std::exception &ex)
        {
            qCWarning(chatterinoTwitchPoints)
                << "received error from point update: " << ex.what();
        }

        // Sleep a bit (do points at fixed frequency)
        sleep(10.0);
    }
}

}  // namespace chatterino
