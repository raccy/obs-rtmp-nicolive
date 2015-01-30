#include <QtCore>
#include <QtNetwork>

class NicoLive : public QObject
{
// Q_OBJECT
public:
        static const QUrl LOGIN_URL;
        static const QUrl PUBSTAT_URL;
        static const QString FMEPROF_URL_PRE;
private:
        QString session;
        QString mail;
        QString password;
        QString live_id;
        QString live_url;
        QString live_key;
        QNetworkAccessManager* qnam;
        char *buff = NULL;
        size_t buff_size = 256;
        const char *buff_str(const char * str);
public:
        NicoLive();
        void setSession(const char *session);
        void setAccount(const char *mail, const char *password);
        const char *getSession();
        const char *getLiveId();
        const char *getLiveUrl(const char *live_id);
        const char *getLiveKey(const char *live_id);
        bool checkSession();
        QVariant makeCookieData(const QString &session_id);
        QByteArray getWeb(const QUrl);
        // Access Niconico Site
        bool siteLogin();
        bool sitePubStat();
        bool siteLiveProf();
};
