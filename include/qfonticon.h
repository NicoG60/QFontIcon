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

    ~QFontIconEngine() override;

    // ======

    bool isValid() const;

    int icon(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QString text(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    int font(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    qreal scaleFactor(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QColor color(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    qreal speed(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QEasingCurve curve(QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off) const;
    QWidget* widget() const;

    void setIcon(int icon, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setFont(int font, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setScaleFactor(qreal scale, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setColor(const QColor& color, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setSpeed(qreal speed, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setCurve(const QEasingCurve& curve, QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
    void setWidget(QWidget* widget);

    // ======

    QFontIconEngine* clone() const override;

    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state) override;

    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

    void virtual_hook(int id, void* data) override;

public:
    static bool loadFont(const QString& filename, int font = defaultFont());
    static QIcon icon(int icon, int font = defaultFont());
    static void setDefaultFont(int font);
    static int defaultFont();

protected:
    QScopedPointer<QFontIconEnginePrivate> d;
};

#endif // QFONTICON_H
