#pragma once

#include "common/Singleton.hpp"

#include <QMutex>
#include <QString>
#include <QThread>
#include <boost/optional.hpp>
#include <common/Atomic.hpp>

namespace chatterino {

class PointLog
{
public:
    QDateTime m_time;
    QString m_channel, m_message;

    PointLog(const QDateTime &time, const QString &channel,
             const QString &message)
        : m_time(time)
        , m_channel{channel}
        , m_message{message}
    {
    }
};

class TwitchIrcServer;

class TwitchPoints final : public Singleton
{
public:
    void run();

    QList<PointLog> getLogs()
    {
        QMutexLocker ml(&mtxLogs);
        return logs;
    }

    void appendLog(const PointLog &log)
    {
        QMutexLocker ml(&mtxLogs);
        if (logs.size() >= 1000)
            logs.erase(logs.begin());
        logs.append(log);
    }

private:
    class TwitchPointsThread : public QThread
    {
    public:
        void run() override;

    private:
        std::map<std::string, int> num_points;
    };

    TwitchIrcServer *twitch;
    TwitchPointsThread thread;

    QMutex mtxLogs;
    QList<PointLog> logs;
};

}  // namespace chatterino
