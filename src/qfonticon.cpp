#include <qfonticon.h>

#include <QMap>
#include <QFont>
#include <QIconEngine>
#include <QTimer>
#include <QFontMetrics>
#include <QPainter>
#include <QDebug>
#include <QGuiApplication>
#include <QPalette>
#include <QWidget>
#include <QFontDatabase>
#include <QFile>

template<class T>
class StateMap : public QMap<QPair<QIcon::Mode, QIcon::State>, T>
{
public:
    typedef QPair<QIcon::Mode, QIcon::State> Key;

public:
    StateMap() : QMap<Key, T>() {}
    StateMap(StateMap<T> &&other) : QMap<Key, T>(other) {}
    StateMap(const StateMap<T> &other) : QMap<Key, T>(other) {}
    StateMap(std::initializer_list<std::pair<Key, T>> list)  : QMap<Key, T>(list) {}
    StateMap<T>& operator=(StateMap<T> &&other) noexcept {  QMap<Key, T>::operator=(other); return *this; }
    StateMap<T>& operator=(const StateMap<T> &other) noexcept {  QMap<Key, T>::operator=(other); return *this; }
    ~StateMap() = default;

    T get(const Key& k, const T& defaultValue = {}) const
    {
        auto it = this->find(k);
        if(it != this->end())
            return it.value();

        if(k.second != QIcon::Off)
            return get(k.first, QIcon::Off, defaultValue);

        if(k.first != QIcon::Normal)
            return get(QIcon::Normal, QIcon::Off, defaultValue);

        return defaultValue;
    }

    T get(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off, const T& defaultValue = {}) const
    {
        return get(Key{mode, state}, defaultValue);
    }

    void set(const T& value, const Key& k) { this->insert(k, value); }

    void set(const T& value, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off)
    {
        set(value, {mode, state});
    }
};



// =============================================================================



class QFontIconEnginePrivate
{
public:
    QFontIconEnginePrivate();
    ~QFontIconEnginePrivate();

    void setupTimer();
    QSize actualSize(const QSize& size, QFont& font, qreal scale, const QString& text) const;

    StateMap<int> icons;
    StateMap<int> fonts;
    StateMap<qreal> scales;
    StateMap<QColor> colors;
    StateMap<qreal> speeds;
    StateMap<QEasingCurve> curves;

    QWidget* widget = nullptr;

    QScopedPointer<QTimer> timer;
    StateMap<qreal> progress;
    StateMap<qreal> angles;

    struct FontInfo
    {
        int appId;
        QString family;
        QString style;
    };

    static int defaultFont;
    static QMap<int, FontInfo> availableFonts;
    static QFont getFont(int font);
};

QFontIconEnginePrivate::QFontIconEnginePrivate() {}
QFontIconEnginePrivate::~QFontIconEnginePrivate() {}

void QFontIconEnginePrivate::setupTimer()
{
    timer.reset();

    if(!widget)
        return;

    if(std::none_of(speeds.begin(), speeds.end(),
                    [](qreal s){ return s > 0;}))
        return;

    timer.reset(new QTimer);
    timer->setInterval(20);

    QObject::connect(timer.data(), SIGNAL(timeout()),
                     widget, SLOT(update()));
    QObject::connect(timer.data(), &QTimer::timeout, [this]()
    {
        StateMap<qreal> new_progress;
        StateMap<qreal> new_angles;

        for(auto it = speeds.begin(); it != speeds.end(); ++it)
        {
            if(it.value() == 0)
                continue;

            // Speed is in degrees per seconds
            qreal p = progress.get(it.key(), 0);
            p += (it.value()/1000.0*timer->interval())/360.0;

            if(p >= 1)
                p -= 1;

            new_progress.set(p, it.key());

            auto c = curves.get(it.key());
            qreal a = c.valueForProgress(p) * 360.0;
            new_angles.set(a, it.key());
        }

        angles.swap(new_angles);
        progress.swap(new_progress);
    });

    timer->start();
}

QSize QFontIconEnginePrivate::actualSize(const QSize& size, QFont& font, qreal scale, const QString& text) const
{
    int drawSize = qRound(size.height()*scale);
    font.setPixelSize(drawSize);

    auto metrics = QFontMetrics(font);
    auto rect = metrics.boundingRect(text);

    auto rsize = rect.size();

    if(rsize.width() > size.width() || rsize.height() > size.height())
    {
        rsize.scale(size, Qt::KeepAspectRatio);
        font.setPixelSize(rsize.height());
        return rsize;
    }
    else
        return size;
}

int QFontIconEnginePrivate::defaultFont = 0;
QMap<int, QFontIconEnginePrivate::FontInfo> QFontIconEnginePrivate::availableFonts;

QFont QFontIconEnginePrivate::getFont(int font)
{
    static QFontDatabase db;

    auto it = availableFonts.find(font);
    if(it != availableFonts.end())
        return db.font(it.value().family, it.value().style, 16);
    else
        return{};
}



// =============================================================================



QFontIconEngine::QFontIconEngine() :
    d(new QFontIconEnginePrivate)
{}

QFontIconEngine::QFontIconEngine(const QFontIconEngine& other) :
    d(new QFontIconEnginePrivate)
{
    auto& od = other.d;

    d->icons = od->icons;
    d->fonts = od->fonts;
    d->colors = od->colors;
    d->speeds = od->speeds;
    d->curves = od->curves;

    d->widget = od->widget;

    d->setupTimer();
}

QFontIconEngine::QFontIconEngine(int icon, int font) :
    QFontIconEngine()
{
    setIcon(icon);
    setFont(font);
}

QFontIconEngine::~QFontIconEngine() {}

bool QFontIconEngine::isValid() const
{
    if(std::any_of(d->icons.begin(), d->icons.end(),
                   [](int i){ return i == InvalidIcon; }))
        return false;

    if(std::any_of(d->fonts.begin(), d->fonts.end(),
                   [](int f){ return !QFontIconEnginePrivate::availableFonts.contains(f); }))
        return false;

    return true;
}

int QFontIconEngine::icon(QIcon::Mode mode, QIcon::State state) const
{
    return d->icons.get(mode, state, InvalidIcon);
}

QString QFontIconEngine::text(QIcon::Mode mode, QIcon::State state) const
{
    return { QChar(icon(mode, state)) };
}

int QFontIconEngine::font(QIcon::Mode mode, QIcon::State state) const
{
    return d->fonts.get(mode, state, defaultFont());
}

qreal QFontIconEngine::scaleFactor(QIcon::Mode mode, QIcon::State state) const
{
    return d->scales.get(mode, state, 0.9);
}

QColor QFontIconEngine::color(QIcon::Mode mode, QIcon::State state) const
{
    auto c = d->colors.get(mode, state);

    if(!c.isValid())
    {
        auto p = QGuiApplication::palette();

        switch (mode)
        {
        case QIcon::Active:
            c = p.color(QPalette::Active, QPalette::ButtonText);
            break;

        case QIcon::Normal:
            c = p.color(QPalette::Normal, QPalette::ButtonText);
            break;

        case QIcon::Disabled:
            c = p.color(QPalette::Disabled, QPalette::ButtonText);
            break;

        case QIcon::Selected:
            c = p.color(QPalette::Active, QPalette::ButtonText);
            break;
        }
    }

    return c;
}

qreal QFontIconEngine::speed(QIcon::Mode mode, QIcon::State state) const
{
    return d->speeds.get(mode, state, 0);
}

QEasingCurve QFontIconEngine::curve(QIcon::Mode mode, QIcon::State state) const
{
    return d->curves.get(mode, state);
}

QWidget* QFontIconEngine::widget() const
{
    return d->widget;
}

void QFontIconEngine::setIcon(int icon, QIcon::Mode mode, QIcon::State state)
{
    d->icons.set(icon, mode, state);
}

void QFontIconEngine::setFont(int font, QIcon::Mode mode, QIcon::State state)
{
    d->fonts.set(font, mode, state);
}

void QFontIconEngine::setScaleFactor(qreal scale, QIcon::Mode mode, QIcon::State state)
{
    d->scales.set(scale, mode, state);
}

void QFontIconEngine::setColor(const QColor& color, QIcon::Mode mode, QIcon::State state)
{
    d->colors.set(color, mode, state);
}

void QFontIconEngine::setSpeed(qreal speed, QIcon::Mode mode, QIcon::State state)
{
    d->speeds.set(speed, mode, state);
    d->setupTimer();
}

void QFontIconEngine::setCurve(const QEasingCurve& curve, QIcon::Mode mode, QIcon::State state)
{
    d->curves.set(curve, mode, state);
}

void QFontIconEngine::setWidget(QWidget* widget)
{
    d->widget = widget;
    d->setupTimer();
}

QFontIconEngine* QFontIconEngine::QFontIconEngine::clone() const
{
    return new QFontIconEngine(*this);
}

QSize QFontIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    if(!isValid())
    {
        qWarning() << "QFontIconEngine: Invalid object";
        return {};
    }

    int id = font(mode, state);
    auto f = QFontIconEnginePrivate::getFont(id);
    auto t = text(mode, state);
    auto s = scaleFactor(mode, state);

    return d->actualSize(size, f, s, t);
}

void QFontIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    if(!isValid())
    {
        qWarning() << "QFontIcon: Invalid QFontIcon object";
        return;
    }

    painter->save();

    auto rf = QRectF(rect); // Use floating for more precision

    auto t  = text(mode, state);
    int id  = font(mode, state);
    auto f  = QFontIconEnginePrivate::getFont(id);
    auto sf = scaleFactor(mode, state);
    auto c  = color(mode, state);

    d->actualSize(rect.size(), f, sf, t);

    auto a = d->angles.get(mode, state);

    if(a != 0)
    {
        auto center = rf.center();
        painter->translate(center.x(), center.y());
        painter->rotate(a);
        painter->translate(-center.x(), -center.y());
    }

    painter->setPen(c);
    painter->setFont(f);
    painter->drawText(rf, text(mode, state), QTextOption( Qt::AlignCenter ));

    painter->restore();
}

QPixmap QFontIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    QPixmap pm(size);
    pm.fill( Qt::transparent ); // we need transparency
    {
        QPainter p(&pm);
        paint(&p, QRect(QPoint(0,0),size), mode, state);
    }
    return pm;
}

void QFontIconEngine::virtual_hook(int id, void* data)
{
    if(id == QIconEngine::IsNullHook)
        *static_cast<bool*>(data) = !isValid();
    else
        QIconEngine::virtual_hook(id, data);
}

bool QFontIconEngine::loadFont(const QString& filename, int font)
{
    static QFontDatabase db;
    QFontIconEnginePrivate::FontInfo info;

    // Open it
    QFile fontFile(filename);

    if(!fontFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "QFontIcon: Unable to open font file.";
        return false;
    }

    QByteArray fontData(fontFile.readAll());

    // Load it
    info.appId = QFontDatabase::addApplicationFontFromData(fontData);

    // Retrieve it
    auto families = QFontDatabase::applicationFontFamilies(info.appId);
    if(families.empty())
    {
        qWarning() << "QFontIcon: Font file is empty.";
        return false;
    }

    info.family = families.first();

    auto styles = db.styles(info.family);

    for(const auto& other : qAsConst(QFontIconEnginePrivate::availableFonts))
    {
        if(info.family == other.family)
            styles.removeAll(other.style);
    }

    if(styles.empty())
    {
        qWarning() << "QFontIcon: Already registered.";
        return false;
    }

    info.style = styles.first();

    // Save it
    QFontIconEnginePrivate::availableFonts[font] = info;
    return true;
}

QIcon QFontIconEngine::icon(int icon, int font)
{
    return QIcon(new QFontIconEngine(icon, font));
}

void QFontIconEngine::setDefaultFont(int font)
{
    QFontIconEnginePrivate::defaultFont = font;
}

int QFontIconEngine::defaultFont()
{
    return QFontIconEnginePrivate::defaultFont;
}
