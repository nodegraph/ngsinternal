#pragma once
#include "o2/src/o2.h"

/// Facebook's dialect of OAuth 2.0
class  O2Nest: public O2 {
    Q_OBJECT

public:
    explicit O2Nest(QObject *parent = 0);

protected Q_SLOTS:
    void onVerificationReceived(QMap<QString, QString>);
    virtual void onTokenReplyFinished();
};

