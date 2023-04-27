#include <QtWidgets>
#include <QtGui>
#include <QtDBus>

static QGuiApplication *app;
static int state = -1;

static int dell_monitor_connected(void)
{
    static QString dell("Dell Inc.");
    QList<QScreen *> scrs = app->screens();
    int len = scrs.size();
    for (int i = 0; i < len; i++) {
	QScreen *scr = scrs[i];
	if (scr->manufacturer() == dell)
	    return 1;
    }
    return 0;
}

static void inhibit(int do_inhibit)
{
    static unsigned int cookie = 0;
    QDBusInterface *iface = new QDBusInterface(
	    "org.freedesktop.ScreenSaver",
	    "/ScreenSaver",
	    "org.freedesktop.ScreenSaver",
	    QDBusConnection::sessionBus());
    if (do_inhibit) {
	QDBusReply<uint> reply = iface->call("Inhibit", "Inhibit", "External monitor is connected.");
	if (reply.isValid()) {
	    cookie = reply.value();
	    printf("inhibited. cookie=%u\n", cookie);
	} else {
	    const QDBusError err = reply.error();
	    printf("inhibition failed.\n");
	    printf("%s\n", qPrintable(err.message()));
	}
    } else {
	if (cookie != 0) {
	    QDBusReply<void> reply = iface->call("UnInhibit", cookie);
	    if (reply.isValid()) {
		printf("uninhibited.\n");
	    } else {
		const QDBusError err = reply.error();
		printf("uninhibition failed.\n");
		printf("%s\n", qPrintable(err.message()));
	    }
	}
	cookie = 0;
    }
}

static void update(void)
{
    int new_state = dell_monitor_connected();
    if (new_state != state) {
	state = new_state;
	printf("state: %d\n", state);
	inhibit(state);
    }
}

int main(int argc, char* argv[])
{
    app = new QGuiApplication(argc, argv);

    QObject::connect(app, &QGuiApplication::screenAdded, update);
    QObject::connect(app, &QGuiApplication::screenRemoved, update);

    update();

    return app->exec();
}
