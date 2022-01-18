#ifndef QFONTICON_H
#define QFONTICON_H

#include <QIconEngine>
#include <QVariant>
#include <QEasingCurve>

class QFontIconEnginePrivate;
class QFontIconEngine : public QIconEngine
{
public:
    enum { InvalidIcon = -1 };

public:
    QFontIconEngine();
    QFontIconEngine(const QFontIconEngine& other);

    QFontIconEngine(int icon, int font = defaultFont());
    QFontIconEngine(const QString& icon, const QString& font = {});

    ~QFontIconEngine() override;

    // ======

    bool isValid() const;

    int icon(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QString iconName() override;
    QString iconName(QIcon::Mode mode, QIcon::State state) const;
    QString text(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    quint32 glyphIndex(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    int font(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QString fontName(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    qreal scaleFactor(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QColor color(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    qreal speed(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QEasingCurve curve(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QWidget* widget() const;
    bool badgeEnabled() const;

    void setIcon(int icon, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setIcon(const QString& name, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setFont(int font, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setFont(const QString& name, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setScaleFactor(qreal scale, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setColor(const QColor& color, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setSpeed(qreal speed, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setCurve(const QEasingCurve& curve, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setWidget(QWidget* widget);
    void setBadgeEnabled(bool en);

    // ======

    QFontIconEngine* clone() const override;

    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

    void virtual_hook(int id, void* data) override;

public:
    static bool loadFont(const QString& filename, int font = defaultFont(), const QString& name = {});
    static QIcon icon(int icon, int font = defaultFont());
    static QIcon icon(const QString& icon, const QString& font = {});
    static void setDefaultFont(int font);
    static int defaultFont();

    static bool registerIconName(QString name, int code);
    static bool registerIconName(const QMap<QString, int>& names);

    static bool registerFontName(QString name, int font);
    static bool registerFontName(const QMap<QString, int>& names);

protected:
    QScopedPointer<QFontIconEnginePrivate> d;
};

#endif // QFONTICON_H
