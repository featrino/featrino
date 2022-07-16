#pragma once

#include "providers/twitch/TwitchPoints.hpp"
#include "widgets/AccountSwitchWidget.hpp"
#include "widgets/settingspages/SettingsPage.hpp"

#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QTableView>

namespace chatterino {

class GeneralPageView;
class DescriptionLabel;
struct DropdownArgs;

class PointsModel : public QAbstractTableModel
{
    QList<PointLog> m_data;

public:
    PointsModel(QObject *parent = {})
        : QAbstractTableModel{parent}
    {
    }
    int rowCount(const QModelIndex &) const override
    {
        return m_data.count();
    }
    int columnCount(const QModelIndex &) const override
    {
        return 3;
    }
    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role != Qt::DisplayRole && role != Qt::EditRole)
            return {};
        const auto &point = m_data[index.row()];
        const QLocale locale("en_US");
        switch (index.column())
        {
            case 0:
                return locale.toString(point.m_time, "hh:mm:ss");
            case 1:
                return point.m_channel;
            case 2:
                return point.m_message;
            default:
                return {};
        };
    }
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const override
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return {};
        switch (section)
        {
            case 0:
                return "Date";
            case 1:
                return "Channel";
            case 2:
                return "Log Message";
            default:
                return {};
        }
    }
    void append(const PointLog &point)
    {
        beginInsertRows({}, m_data.count(), m_data.count());
        m_data.prepend(point);
        endInsertRows();
    }
    void clear()
    {
        beginResetModel();
        m_data.clear();
        endResetModel();
    }
};

class PointsPage : public SettingsPage
{
public:
    PointsPage();

    void onShow() final;

private:
    PointsModel m_model;
    QLineEdit oauthTokenInput;
};

}  // namespace chatterino