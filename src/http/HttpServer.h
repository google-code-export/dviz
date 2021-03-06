#ifndef HttpServer_H
#define HttpServer_H

#include <QTcpServer>
#include <QHttpResponseHeader>
#include <QMap>

#ifndef QStringMap
// Just typedefd for ease of use, no other reason.
typedef QMap<QString,QString> QStringMap;
#endif

#ifndef QByteArrayPair
// Define this QPair<> defn as a typedef so that the Q_FOREACH macro wouldn't barf during compile about 3 args
typedef QPair<QByteArray, QByteArray> QByteArrayPair;
#endif

#define Http_Send_Ok(socket) Http_Send_200(socket)
	 
#define Http_Send_200(socket) \
	respond(socket,QString("HTTP/1.0 200 OK")); \
	QTextStream output(socket); \
	output.setAutoDetectUnicode(true); \
	output
	
#define Http_Send_404(socket) \
	respond(socket,QString("HTTP/1.0 404 File Not Found")); \
	QTextStream output(socket); \
	output.setAutoDetectUnicode(true); \
	output 
	
#define Http_Send_500txt(socket, message) \
	respond(socket,QString("HTTP/1.0 400 " message)); \
	QTextStream output(socket); \
	output.setAutoDetectUnicode(true); \
	output << message;

#define Http_Send_Response(socket,string) \
	respond(socket,QString(string)); \
	QTextStream output(socket); \
	output.setAutoDetectUnicode(true); \
	output 
	
	
class HttpUser;

// HttpServer is the the class that implements the simple HTTP server.
class HttpServer : public QTcpServer
{
	Q_OBJECT
public:
	HttpServer(quint16 port, QObject* parent = 0);
	
	// QTcpServer::
	void incomingConnection(int socket);
	
	// Pause/resume accepting connections
	void pause();
	void resume();
	
	// Utility method: Returns an absolute URL string containing the given path and query string, encoding them to percent encoding by default.
	static QString toPathString(const QStringList &pathElements, const QStringMap &query, bool encoded=true);
	
	// Send a generic 404 response to the client for the given path & query string, encoding any user-supplied values with percent encoding to prevent XSS
	void generic404(QTcpSocket *socket, const QStringList &path = QStringList(), const QStringMap & map = QStringMap());
	
	// Simple file serving routine - reads the file, guesses the content type from extension (supports png,jp(e)g,gif,css,js, and html), and sends it to the socket
	bool serveFile(QTcpSocket *socket, const QString &, bool addExpiresHeader = true);
	
	const QHttpRequestHeader & requestHeader() { return m_requestHeader; }
	
	QHash<QString,QString> cookies() { return m_cookies; }
	bool hasCookie(QString key) { return m_cookies.contains(key); }
	QString cookie(QString key) { return m_cookies.value(key); }
	
	void setCookie(QString key, QString value, int expiresDays);
	void setCookie(QString key, QString value, QDateTime expires, QString domain="", QString path="/");
	
	void redirect(QTcpSocket *socket, const QString &, bool addExpiresHeader = false);
	
protected:
	// Make a class that inherits HttpServer and override this function to handle requests yourself.
	virtual void dispatch(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query);
	
	// Send the HTTP response line 
	void respond(QTcpSocket *socket, const QHttpResponseHeader &);
	
	// Overriden for convenience: Send a response and the contents of the byte array to the socket
	void respond(QTcpSocket *socket, const QHttpResponseHeader &,const QByteArray &);
	
	
	// Login/logout page
	virtual void loginPage(QTcpSocket *socket, const QStringList &pathElements, const QStringMap &query, QString loginUrl, QString templateFile = "");
		
private slots:
	void readClient();
	void discardClient();
	
private:
	void loadCurrentUser(const QStringMap &query);
	void setUserCookie(HttpUser *);

	bool m_disabled;
	
	QHash<QString,QString> m_cookies;
	QHash<QString,QString> m_setCookies;
	
	QHttpRequestHeader m_requestHeader;
	
	QTcpSocket *m_socket;
};
 
#endif
