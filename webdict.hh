//
// Created by joybin on 2020/3/1.
//

#ifndef GOLDENDICT_WEBDICT_HH
#define GOLDENDICT_WEBDICT_HH

#include "dictionary.hh"
#include "config.hh"
#include <QNetworkAccessManager>
#include "wstring.hh"
#include <QNetworkReply>

#include "forvo.hh"
#include "mediawiki.hh"
#include "programs.hh"
#include "website.hh"

namespace WebDict
{
    using std::vector;
    using std::string;
    using gd::wstring;

class RequestDelegate {
    virtual void requestFinished(QNetworkReply *) {};
    virtual void downloadFinished() {};
    virtual void cancel() {};
};

/// Exposed here for moc
class DataRequest: public Dictionary::DataRequest
{
    Q_OBJECT

public:
    DataRequest( RequestDelegate *delegate ): delegate(delegate) {};

private slots:
    virtual void requestFinished(QNetworkReply*) { delegate->requestFinished(r) };

private:
    RequestDelegate* delegate;
};

/// Exposed here for moc
class WordSearchRequest: public Dictionary::WordSearchRequest
{
    Q_OBJECT

public:
    DataRequest( RequestDelegate *delegate ): delegate(delegate) {};

private slots:
    virtual void downloadFinished() { delegate->downloadFinished() };

private:
    RequestDelegate* delegate;
};


class RunInstance: public QObject
{
    Q_OBJECT
    QProcess process;

public:
    RunInstance();

    // Starts the process. Should only be used once. The finished() signal will
    // be emitted once it finishes. If there's an error, returns false and the
    // description is saved to 'error'.
    bool start( Config::Program const &, QString const & word, QString & error );

signals:
    // Connect to this signal to get run results
    void finished( QByteArray output, QString error );

    // Used internally only
signals:
    void processFinished();

private slots:
    void handleProcessFinished();
};

class ProgramDataRequest: public Dictionary::DataRequest
{
    Q_OBJECT
    Config::Program prg;
    RunInstance instance;

public:
    ProgramDataRequest( QString const & word, Config::Program const & );
    virtual void cancel();

private slots:
    void instanceFinished( QByteArray output, QString error );
};

class ProgramWordSearchRequest: public Dictionary::WordSearchRequest
{
    Q_OBJECT
    Config::Program prg;
    RunInstance instance;

public:
    ProgramWordSearchRequest( QString const & word, Config::Program const & );
    virtual void cancel();

private slots:
    void instanceFinished( QByteArray output, QString error );
};

}

#endif //GOLDENDICT_WEBDICT_HH
