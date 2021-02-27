#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <QFontIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // === Trick to get dark mode working on macOS
    auto updatePalette = []()
    {
        QSignalBlocker blocker(QApplication::instance());
        auto p = QApplication::palette();

        auto cn = p.color(QPalette::Normal, QPalette::Text);
        auto ca = p.color(QPalette::Active, QPalette::Text);
        auto cd = p.color(QPalette::Disabled, QPalette::Text);
        auto ci = p.color(QPalette::Inactive, QPalette::Text);

        p.setColor(QPalette::Normal, QPalette::ButtonText, cn);
        p.setColor(QPalette::Active, QPalette::ButtonText, ca);
        p.setColor(QPalette::Disabled, QPalette::ButtonText, cd);
        p.setColor(QPalette::Inactive, QPalette::ButtonText, ci);

        QApplication::setPalette(p);
    };

    updatePalette();

    QObject::connect(&app, &QApplication::paletteChanged, updatePalette);
    // === end of the trick

    QFontIconEngine::loadFont(":/fonts/fa-solid-900.ttf");

    QMainWindow w;

    QVBoxLayout* layout = new QVBoxLayout();

    // a simple animated beer button
    //=====================
    {
        QPushButton* beerButton = new QPushButton( "Cheers!");

        auto engine = new QFontIconEngine(fa::icons::beer);
        engine->setSpeed(180);
        engine->setWidget(beerButton);
        engine->setCurve(QEasingCurve::InOutCubic);

        beerButton->setIcon( QIcon(engine) );

        layout->addWidget(beerButton);
    }

    // a simple toggle button
    //==============================
    {
        QPushButton* toggleButton = new QPushButton("Toggle Me");
        toggleButton->setCheckable(true);

        auto engine = new QFontIconEngine();
        engine->setIcon(fa::icons::toggle_on, QIcon::Normal, QIcon::On);
        engine->setColor(Qt::green, QIcon::Normal, QIcon::On);

        engine->setIcon(fa::icons::toggle_off, QIcon::Normal, QIcon::Off);
        engine->setColor(Qt::red, QIcon::Normal, QIcon::Off);

        toggleButton->setIcon( QIcon(engine) );

        layout->addWidget(toggleButton);
    }


    // add the samples
    QWidget* samples = new QWidget();
    samples->setLayout(layout);
    w.setCentralWidget(samples);

    w.show();

    return app.exec();
}
