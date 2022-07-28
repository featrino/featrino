#include "PointsPage.hpp"

#include "Application.hpp"
#include "providers/twitch/api/GQL.hpp"
#include "util/Helpers.hpp"
#include "util/LayoutCreator.hpp"
#include "util/RemoveScrollAreaBackground.hpp"
#include "widgets/dialogs/LoginDialog.hpp"
#include "widgets/helper/EditableModelView.hpp"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QTableView>
#include <QVBoxLayout>
#include <algorithm>

namespace chatterino {

PointsPage::PointsPage()
{
    auto *app = getApp();

    LayoutCreator<PointsPage> layoutCreator(this);
    auto topArea = layoutCreator.emplace<QVBoxLayout>().withoutMargin();

    auto scroll = layoutCreator.emplace<QScrollArea>();
    auto widget = scroll.emplaceScrollAreaWidget();
    removeScrollAreaBackground(scroll.getElement(), widget.getElement());

    auto layout = widget.setLayoutType<QVBoxLayout>();

    // Description
    auto explanation =
        layout
            .emplace<QLabel>(
                "DANGER! You will need your account oauth cookie to allow full "
                "access to your account if you want to use this point miner. "
                "Channels are prioritize based on tab order (left to right, "
                "top to bottom). Navigate to " +
                formatRichNamedLink("https://www.twitch.tv/", "twitch.tv") +
                +" and view your cookies (lock button). Find the cookie named "
                 "auth-token and paste that below.")
            .getElement();
    explanation->setWordWrap(true);
    explanation->setStyleSheet("color: #bbb");

    // Form to put in token
    // TODO: save the oauth to user settings file!
    auto formLayout = layout.emplace<QFormLayout>().getElement();
    formLayout->addRow("OAuth Cookie", &this->oauthTokenInput);
    oauthTokenInput.setEchoMode(QLineEdit::Password);
    oauthTokenInput.setText(getSettings()->twitchGQLOAuth);
    connect(&this->oauthTokenInput, &QLineEdit::textChanged,
            [=](const QString &text) {
                getSettings()->twitchGQLOAuth = text;
                getGQL()->update(text);
            });
    layout->addSpacing(16);

    // Log table of historical log information
    // TODO: should probably use a SLOT to handle live updating of logs...
    auto tableView = layout.emplace<QTableView>().getElement();
    tableView->setModel(&m_model);
    //tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->horizontalHeader()->setStretchLastSection(true);
}

void PointsPage::onShow()
{
    auto *app = getApp();

    m_model.clear();

    auto logs = app->twitchPoints->getLogs();
    for (const auto &log : logs)
        m_model.append(log);
}

}  // namespace chatterino
