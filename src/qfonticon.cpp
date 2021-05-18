#include <qfonticon.h>

#include <QMap>
#include <QRawFont>
#include <QIconEngine>
#include <QTimer>
#include <QFontMetrics>
#include <QPainter>
#include <QDebug>
#include <QGuiApplication>
#include <QPalette>
#include <QWidget>
#include <QFontDatabase>
#include <QPainterPath>
#include <QFile>

template<class T>
class StateMap : public QMap<QPair<QIcon::Mode, QIcon::State>, T>
{
public:
    typedef QPair<QIcon::Mode, QIcon::State> Key;

public:
    StateMap() : QMap<Key, T>() {}
    StateMap(StateMap<T> &&other) noexcept : QMap<Key, T>(other) {}
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

        if(k.first != QIcon::Normal)
            return get(QIcon::Normal, k.second, defaultValue);

        if(k.second != QIcon::Off)
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
    QSizeF resizeFont(const QSizeF& size, QRawFont& font, qreal scale, quint32 glyphIndex) const;

    StateMap<int> icons;
    StateMap<int> fonts;
    StateMap<qreal> scales;
    StateMap<QColor> colors;
    StateMap<qreal> speeds;
    StateMap<QEasingCurve> curves;

    QWidget* widget = nullptr;

    bool badge = false;

    QScopedPointer<QTimer> timer;
    StateMap<qreal> progress;
    StateMap<qreal> angles;

    struct FontInfo
    {
        int     appId = -1;
        QString family;
        QString style;
        QFont   font;
    };

    static int defaultFont;
    static QMap<int, QRawFont> availableFonts;
    static QRawFont& getFont(int font);

    static QMap<QString, int> iconNames;
    static QMap<QString, int> fontNames;
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

QSizeF QFontIconEnginePrivate::resizeFont(const QSizeF& size, QRawFont& font, qreal scale, quint32 glyphIndex) const
{
    qreal drawSize = qMax(size.width(), size.height())*scale;
    font.setPixelSize(drawSize);

    auto rect = font.boundingRect(glyphIndex);

    auto rsize = rect.size();

    if(rsize.width() > size.width() || rsize.height() > size.height())
    {
        auto nsize = rsize.scaled(size, Qt::KeepAspectRatio);
        qreal ratio = nsize.height() / rsize.height();
        font.setPixelSize(drawSize * ratio);
        return nsize;
    }
    else
        return size;
}

int QFontIconEnginePrivate::defaultFont = 0;
QMap<int, QRawFont> QFontIconEnginePrivate::availableFonts;

QRawFont& QFontIconEnginePrivate::getFont(int font)
{
    Q_ASSERT(availableFonts.contains(font));
    return availableFonts[font];
}

QMap<QString, int> QFontIconEnginePrivate::iconNames;
QMap<QString, int> QFontIconEnginePrivate::fontNames;



// =============================================================================

/**
 * @class QFontIconEngine
 * @brief QIconEngine subclass to handle font glyphs as icons.
 *
 * QFontIconEngine can be used with any fonts. You presumably have embedded your
 * favourite glyph font in a @c .qrc file. The first thing you need to do is
 * loading you font using the loadFont() static function:
 *
 * @code
 * QFontIconEngine::loadFont(":/path/to/my_font.ttf");
 * @endcode
 *
 * Then you can use the icon() static helper function to easily create icons from
 * the font you just loaded:
 * @code
 * QIcon icon = QFontIconEngine::icon(0xf82b) // This is the code point of the glyph
 *
 * // Alternatively, you can make an enumeration and / or register string base names
 * enum glyph_code {
 *     super_glyph = 0xf82b
 * };
 *
 * QFontIconEngine::registerIconName("super-glyph", super_glyph);
 *
 * // And then use names instead:
 * QIcon otherIcon = QFontIconEngine::icon("super-glyph");
 * @endcode
 *
 * QFontIconEngine supports stateful icons. That means you can set different
 * glyphs and various other properties on a per-state basis, depending on
 * @c QIcon::Mode and @c QIcon::State . For example:
 *
 * @code
 * // Assuming you have registered icon names
 * // create the engine and then set properties
 * auto engine = new QFontIconEngine();
 * engine->setIcon("toggle-on",  QIcon::Normal, QIcon::On);
 * engine->setIcon("toggle-off", QIcon::Normal, QIcon::Off);
 *
 * engine->setColor(Qt::green, QIcon::Normal, QIcon::On);
 * engine->setColor(Qt::red,   QIcon::Normal, QIcon::Off);
 *
 * // Pass it to a QIcon. Note that QIcon takes the ownership of the engine so.
 * // Therefore it's unsafe to reuse if afterward.
 * QIcon icon(engine);
 * @endcode
 *
 * The default state is { QIcon::Normal, QIcon::Off }  It is the
 * one set by default when using helper fuctions. If you create and engine from
 * scratch, this state must be set.
 *
 * If a value is not available in a given state { M, S } , it will
 * first try to find a { QIcon::Normal, S } and if still not
 * successful, will ultimately default to { QIcon::Normal, QIcon::Off }.
 *
 * Properties also include a spinning animation. (Yep)
 * @code
 * // Assuming you'll put it on a button
 * auto btn = new QPushButton();
 *
 * auto engine = new QFontIconEngine();
 * engine->setIcon("spinner");
 *
 * engine->setWidget(btn); // You need to tell the engine which widget to update
 * engine->setSpeed(180); // speed is in degree per second. 180 <=> 1 turn in 2s
 * engine->setCurve(QEasingCurve::InOutSine); // You can even set non-linear rotation.
 *
 * // Pass it to a QIcon and profits
 * QIcon icon(engine);
 * @endcode
 *
 * You can load multiple fonts and use them seamlessly.
 * When loading a font you can specify a arbitrary font id and name which will
 * help you differenciate the fonts.
 * @code
 * enum icon_set
 * {
 *     primary_font,
 *     secondary_font
 * };
 *
 * QFontIconEngine::loadFont(":/path/to/my_font.ttf", primary_font, "primary");
 * QFontIconEngine::loadFont(":/path/to/my_font.ttf", secondary_font, "secondary");
 *
 * // Set a default
 * QFontIconEngine::setDefaultFont(primary_font);
 *
 * // Then you can pass either the font id of the font name depending on the
 * // overload you use:
 * QIcon icon = QFontIconEngine::icon("super-glyph", "secondary");
 *
 * // This one will use the default font set earlier
 * QIcon icon2 = QFontIconEngine::icon("super-glyph");
 * @endcode
 */

/**
 * Construct and engine with the defaultFont() set but no icon. It is therefore
 * invalid.
 *
 * @see isValid()
 */
QFontIconEngine::QFontIconEngine() :
    d(new QFontIconEnginePrivate)
{
    setFont(defaultFont());
}

/**
 * @brief Copy constructor.
 */
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

/**
 * @brief Construct and engine using @a icon and @a font.
 *
 * @a icon and @a font will set the { QIcon::Normal, QIcon::Off } state and
 * therefore be used by default.
 *
 * @param icon the code point fo the icon to use
 * @param font the font id to use
 */
QFontIconEngine::QFontIconEngine(int icon, int font) :
    QFontIconEngine()
{
    setIcon(icon);
    setFont(font);
}

/**
 * @brief Construct and engine using @a icon and @a font.
 *
 * @overload
 *
 * @param icon the glyph name fo the icon to use
 * @param font the font name to use
 */
QFontIconEngine::QFontIconEngine(const QString& icon, const QString& font) :
    QFontIconEngine()
{
    setIcon(icon);
    if(font.isEmpty())
        setFont(defaultFont());
    else
        setFont(font);
}

QFontIconEngine::~QFontIconEngine() {}

/**
 * @brief Whether the engine is valid.
 *
 * Invalid engine results in null icon and wont draw.
 *
 * An engine is considered valid if:
 * - it has an default and valid icon (valid code point or name)
 * - it has a default and valid font (loaded font and valid name)
 */
bool QFontIconEngine::isValid() const
{
    if(d->icons.empty())
        return false;

    if(d->fonts.empty())
        return false;

    if(std::any_of(d->icons.begin(), d->icons.end(),
                   [](int i){ return i == InvalidIcon; }))
        return false;

    if(std::any_of(d->fonts.begin(), d->fonts.end(),
                   [](int f){ return !QFontIconEnginePrivate::availableFonts.contains(f); }))
        return false;

    return true;
}

/**
 * @brief Returns the icon code point set for the given state.
 */
int QFontIconEngine::icon(QIcon::Mode mode, QIcon::State state) const
{
    return d->icons.get(mode, state, InvalidIcon);
}

/**
 * @brief Returns the default icon name.
 */
QString QFontIconEngine::iconName() const
{
    return iconName(QIcon::Normal, QIcon::Off);
}

/**
 * @brief Returns the icon name set for the given state.
 */
QString QFontIconEngine::iconName(QIcon::Mode mode, QIcon::State state) const
{
    int i = icon(mode, state);
    return QFontIconEnginePrivate::iconNames.key(i);
}

/**
 * @brief Returns the icon code point as a text string for the given state.
 */
QString QFontIconEngine::text(QIcon::Mode mode, QIcon::State state) const
{
    return { QChar(icon(mode, state)) };
}

/**
 * @brief Returns the icon glyph index in the font. (which is different from the codepoint)
 */
quint32 QFontIconEngine::glyphIndex(QIcon::Mode mode, QIcon::State state) const
{
    auto& f = QFontIconEnginePrivate::getFont(font(mode, state));
    auto v = f.glyphIndexesForString(text(mode, state));
    Q_ASSERT(v.count() == 1);
    return v.first();
}

/**
 * @brief Returns the font id set for the given state.
 */
int QFontIconEngine::font(QIcon::Mode mode, QIcon::State state) const
{
    return d->fonts.get(mode, state, defaultFont());
}

/**
 * @brief Returns the font name set for the given state.
 */
QString QFontIconEngine::fontName(QIcon::Mode mode, QIcon::State state) const
{
    int f = font(mode, state);
    return QFontIconEnginePrivate::fontNames.key(f);
}

/**
 * @brief Returns the scale factor set for the given state.
 */
qreal QFontIconEngine::scaleFactor(QIcon::Mode mode, QIcon::State state) const
{
    return d->scales.get(mode, state, 0.9);
}

/**
 * @brief Returns the color set for the given state.
 */
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

/**
 * @brief Returns the rotation speed set for the given state.
 */
qreal QFontIconEngine::speed(QIcon::Mode mode, QIcon::State state) const
{
    return d->speeds.get(mode, state, 0);
}

/**
 * @brief Returns the easing curve set for the given state.
 */
QEasingCurve QFontIconEngine::curve(QIcon::Mode mode, QIcon::State state) const
{
    return d->curves.get(mode, state);
}

/**
 * @brief Returns the widget the rotation animation displays on.
 */
QWidget* QFontIconEngine::widget() const
{
    return d->widget;
}

/**
 * @brief Returns whether to display a red badge on the icon
 */
bool QFontIconEngine::badgeEnabled() const
{
    return d->badge;
}

/**
 * @brief Set the icon code point for the given state.
 */
void QFontIconEngine::setIcon(int icon, QIcon::Mode mode, QIcon::State state)
{
    d->icons.set(icon, mode, state);
}

/**
 * @brief Set the icon name for the given state.
 */
void QFontIconEngine::setIcon(const QString& name, QIcon::Mode mode, QIcon::State state)
{
    auto it = QFontIconEnginePrivate::iconNames.find(name);
    if(it == QFontIconEnginePrivate::iconNames.end())
    {
        qWarning() << "QFontIcon: Invalid icon name";
        return;
    }

    setIcon(it.value(), mode, state);
}

/**
 * @brief Set the font id for the given state.
 */
void QFontIconEngine::setFont(int font, QIcon::Mode mode, QIcon::State state)
{
    d->fonts.set(font, mode, state);
}

/**
 * @brief Set the font name for the given state.
 */
void QFontIconEngine::setFont(const QString& name, QIcon::Mode mode, QIcon::State state)
{
    auto it = QFontIconEnginePrivate::fontNames.find(name);
    if(it == QFontIconEnginePrivate::fontNames.end())
    {
        qWarning() << "QFontIcon: Invalid font name";
        return;
    }

    setFont(it.value(), mode, state);
}

/**
 * @brief Set the scale factor for the given state.
 */
void QFontIconEngine::setScaleFactor(qreal scale, QIcon::Mode mode, QIcon::State state)
{
    d->scales.set(scale, mode, state);
}

/**
 * @brief Set the color for the given state.
 */
void QFontIconEngine::setColor(const QColor& color, QIcon::Mode mode, QIcon::State state)
{
    d->colors.set(color, mode, state);
}

/**
 * @brief Set the rotation speed for the given state.
 */
void QFontIconEngine::setSpeed(qreal speed, QIcon::Mode mode, QIcon::State state)
{
    d->speeds.set(speed, mode, state);
    d->setupTimer();
}

/**
 * @brief Set the easing curve point for the given state.
 */
void QFontIconEngine::setCurve(const QEasingCurve& curve, QIcon::Mode mode, QIcon::State state)
{
    d->curves.set(curve, mode, state);
}

/**
 * @brief Set the widget the rotation animation displays on.
 */
void QFontIconEngine::setWidget(QWidget* widget)
{
    d->widget = widget;
    d->setupTimer();
}

/**
 * @brief Set if the red badge should be enabled
 */
void QFontIconEngine::setBadgeEnabled(bool en)
{
    d->badge = en;
}

QFontIconEngine* QFontIconEngine::QFontIconEngine::clone() const
{
    return new QFontIconEngine(*this);
}

QSize QFontIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode);
    Q_UNUSED(state);

    if(!isValid())
    {
        qWarning() << "QFontIconEngine: Invalid object";
        return {};
    }

    return size;
}

void QFontIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    if(!isValid())
    {
        qWarning() << "QFontIcon: Invalid QFontIcon object";
        return;
    }

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    auto r  = QRectF(rect); // Use floating for more precision
    auto s  = r.size();
    auto g  = glyphIndex(mode, state);
    int id  = font(mode, state);
    auto f  = QFontIconEnginePrivate::getFont(id);
    auto sf = scaleFactor(mode, state);
    auto c  = color(mode, state);

    d->resizeFont(s, f, sf, g);

    auto a = d->angles.get(mode, state);

    if(a != 0)
    {
        auto center = r.center();
        painter->translate(center.x(), center.y());
        painter->rotate(a);
        painter->translate(-center.x(), -center.y());
    }

    auto bounds = f.boundingRect(g);
    auto glyph = f.pathForGlyph(g);

    painter->translate(r.center() - bounds.center());
    painter->setPen(Qt::NoPen);
    painter->setBrush(c);
    painter->drawPath(glyph);

    painter->restore();

    if(d->badge)
    {
        painter->save();

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(255, 0, 0, 200));

        auto bs = r.size() / 3.0;

        if(bs.width() < 8 || bs.height() < 8)
            bs = r.size() / 2.0;

        QRectF badgeRect(r.right()-bs.width(), r.top(), bs.width(), bs.height());

        painter->drawEllipse(badgeRect);

        painter->restore();
    }
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

/**
 * @brief load the font located at @a filename.
 *
 * You can pass an arbitrary font id and font name if you're using multiple
 * fonts.
 */
bool QFontIconEngine::loadFont(const QString& filename, int font, const QString& name)
{
    // Open it
    QRawFont rawFont(filename, 32);

    QFontIconEnginePrivate::availableFonts[font] = rawFont;

    if(!name.isEmpty())
        registerFontName(name, font);

    return true;
}

/**
 * @brief Convenience function that returns an icon.
 */
QIcon QFontIconEngine::icon(int icon, int font)
{
    return QIcon(new QFontIconEngine(icon, font));
}

/**
 * @brief Convenience function that returns an icon.
 */
QIcon QFontIconEngine::icon(const QString& icon, const QString& font)
{
    return QIcon(new QFontIconEngine(icon, font));
}

/**
 * @brief Set the default font to use.
 *
 * Before calling this function, the default font is set to 0.
 */
void QFontIconEngine::setDefaultFont(int font)
{
    QFontIconEnginePrivate::defaultFont = font;
}

/**
 * @brief Returns the currently set default font.
 */
int QFontIconEngine::defaultFont()
{
    return QFontIconEnginePrivate::defaultFont;
}

/**
 * @brief Register a name for the given code point
 */
bool QFontIconEngine::registerIconName(QString name, int code)
{
    name = name.trimmed();
    if(name.isEmpty())
    {
        qWarning() << "QFontIcon: Invalid icon name";
        return false;
    }

    QFontIconEnginePrivate::iconNames[name] = code;
    return true;
}

/**
 * @brief Register all the names / code points provided
 */
bool QFontIconEngine::registerIconName(const QMap<QString, int>& names)
{
    bool r = true;
    for(auto it = names.begin(); it != names.end(); ++it)
        r &= registerIconName(it.key(), it.value());
    return r;
}

/**
 * @brief Register a font name for the given font id
 */
bool QFontIconEngine::registerFontName(QString name, int font)
{
    name = name.trimmed();
    if(name.isEmpty())
    {
        qWarning() << "QFontIcon: Invalid font name";
        return false;
    }

    QFontIconEnginePrivate::fontNames[name] = font;
    return true;
}

/**
 * @brief Register all the names / font ids provided
 */
bool QFontIconEngine::registerFontName(const QMap<QString, int>& names)
{
    bool r = true;
    for(auto it = names.begin(); it != names.end(); ++it)
        r &= registerFontName(it.key(), it.value());
    return r;
}
