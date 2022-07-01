#pragma once

#include "common/Singleton.hpp"

#include <QString>
#include <QThread>
#include <boost/optional.hpp>
#include <common/Atomic.hpp>

namespace chatterino {

class TwitchIrcServer;

class TwitchPoints final : public Singleton
{
public:
    void run();

private:
    class TwitchPointsThread : public QThread
    {
    public:
        void run() override;

    private:
        //void handleMessage(const QJsonObject &root);
    };

    TwitchIrcServer *twitch;
    TwitchPointsThread thread;
};

}  // namespace chatterino
