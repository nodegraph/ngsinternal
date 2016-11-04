#ifndef FBDEMO_H
#define FBDEMO_H

#include <QObject>

#include "o2/src/o2facebook.h"
#include "o2nest.h"

class FBDemo : public QObject
{
    Q_OBJECT

public:
    explicit FBDemo(QObject *parent = 0);

signals:
    void extraTokensReady(const QVariantMap &extraTokens);
    void linkingFailed();
    void linkingSucceeded();

public slots:
    void doOAuth(O2::GrantFlow grantFlowType);
    void validateToken();

private slots:
    void onLinkedChanged();
    void onLinkingSucceeded();
    void onOpenBrowser(const QUrl &url);
    void onCloseBrowser();
    void onFinished();

private:
    O2Nest *o2Facebook_;
};

#endif // FBDEMO_H
