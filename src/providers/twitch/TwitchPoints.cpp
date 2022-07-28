#include "providers/twitch/TwitchPoints.hpp"

#include "Application.hpp"
#include "common/QLogging.hpp"
#include "messages/MessageBuilder.hpp"
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
            std::map<std::string, ChannelPtr> channel_ptrs;
            for (auto *sc : pages)
            {
                const QString &tabTitle = sc->getTab()->getTitle();
                const auto splits = sc->getSplits();

                // First, check for splits on this page
                for (auto *split : splits)
                {
                    if (!split->getChannel()->isTwitchChannel())
                        continue;
                    auto name = split->getChannel()->getName().toStdString();
                    channels[name] = split->getChannel()->isLive();
                    channel_ptrs[name] = split->getChannel();
                }
            }

            // Now for each channel, lets get how many points they have
            for (auto const &chanpair : channels)
            {
                std::string chan_name = chanpair.first;
                bool is_live = chanpair.second;
                ChannelPtr chan_ptr = channel_ptrs[chan_name];
                if (!is_live)
                {
                    if (this->num_points.find(chan_name) !=
                        this->num_points.end())
                        this->num_points.erase(chan_name);
                    continue;
                }

                // TODO: send current watch history amount to the api
                // TODO: https://github.com/lay295/TwitchChannelPoints/blob/e9062eae9cc0aa7923216477e319adbc7f9ae76f/TwitchChannelPoints/frmMain.cs#L221-L230
                // https://static.twitchcdn.net/config/settings.js
                // JObject data = new JObject();
                // data["channel_id"] = streamData["user_id"].ToString();
                // data["broadcast_id"] = streamData["id"].ToString();
                // data["player"] = "site";
                // data["user_id"] = userId;
                // JObject data_root = new JObject();
                // data_root["event"] = "minute-watched";
                // data_root["properties"] = data;
                // string payload = Convert.ToBase64String(Encoding.UTF8.GetBytes(data_root.ToString(Newtonsoft.Json.Formatting.None)));
                // await clientTwitch.UploadStringTaskAsync(streamerList[i].SpadeUrl, payload);

                // User is live, thus query the current set of points
                getGQL()->getUserPoints(
                    QString::fromUtf8(chan_name.c_str()),
                    [this, chan_name, chan_ptr](std::pair<int, QString> data) {
                        int points = data.first;
                        QString claimid = data.second;

                        // First ever time we will just append the current number of points
                        if (this->num_points.find(chan_name) ==
                            this->num_points.end())
                        {
                            this->num_points[chan_name] = points;
                            QString msg("Total of " + QString::number(points) +
                                        " channel points currently");
                            getApp()->twitchPoints->appendLog(
                                {QDateTime::currentDateTime(),
                                 QString::fromUtf8(chan_name.c_str()), msg});
                            if (chan_ptr != nullptr)
                                chan_ptr->addMessage(makeSystemMessage(msg));
                            return;
                        }

                        // TODO: claim the reward here if we have it
                        // https://github.com/lay295/TwitchChannelPoints/blob/e9062eae9cc0aa7923216477e319adbc7f9ae76f/TwitchChannelPoints/frmMain.cs#L208-L212

                        // Else lets print the difference!
                        int point_diff = points - this->num_points[chan_name];
                        if (point_diff > 0)
                        {
                            QString msg("Redeemed " +
                                        QString::number(point_diff) +
                                        " points (" + QString::number(points) +
                                        " total)");
                            getApp()->twitchPoints->appendLog(
                                {QDateTime::currentDateTime(),
                                 QString::fromUtf8(chan_name.c_str()), msg});
                            if (chan_ptr != nullptr)
                                chan_ptr->addMessage(makeSystemMessage(msg));
                            this->num_points[chan_name] = points;
                        }
                    },
                    [chan_name]() {
                        getApp()->twitchPoints->appendLog(
                            {QDateTime::currentDateTime(),
                             QString::fromUtf8(chan_name.c_str()),
                             "Unable to retrieve points, is your oauth "
                             "cookie correct?"});
                    });
            }
        }
        catch (const std::exception &ex)
        {
            qCWarning(chatterinoTwitchPoints)
                << "received error from point update: " << ex.what();
        }

        // Sleep a bit (do points at fixed frequency)
        sleep(60.0);
    }
}

}  // namespace chatterino
