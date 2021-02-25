# QFontIcon

## Description

QFontIcon is a simple library that enables you to load one or several icon/glyph fonts and use them.

Inspired by [QtAwesome](https://github.com/gamecreature/QtAwesome).

## CMake

It's a cmake based project you can add as a subdirectory.

## Usage

Here is a simple example using Font Awesome.
> Note: The library provides convenient enumerations for Font Awesome under the `fa` namespace.

```cpp
#include <QApplication>
#include <QPushButton>

#include <QFontIcon> // Include it obviously

int main(int argc, char*[] argv)
{
    // Create the application first
    QApplication app(argc, argv);

    // Then load the fonts you want to use.
    // Here we load the free Font Awesome fonts.
    QFontIconEngine::loadFont("path/to/fa-solid-900.ttf",   fa::solid);
    QFontIconEngine::loadFont("path/to/fa-regular-400.ttf", fa::regular);
    QFontIconEngine::loadFont("path/to/fa-brands-400.ttf",  fa::brands);

    // Create the icon you want
    auto beer = QFontIconEngine::icon(fa::beer, fa::solid);

    // And use it
    QPushButton* btn = new QPushButton(beer, "cheers");

    btn->show();

    return app.exec();
}
```

The icon colour is derived from the QApplication palette to better match the
theme of your system.

You can override this and even creating animations and stateful icons by
creating the engine yourself and pass it to a QIcon. For example:
```cpp
// Assuming you still have font awesome loaded
// create the engine and then set properties
auto engine = new QFontIconEngine();
engine->setIcon(fa::toggle_on, QIcon::Normal, QIcon::On);
engine->setIcon(fa::toggle_off, QIcon::Normal, QIcon::Off);

engine->setColor(Qt::green, QIcon::Normal, QIcon::On);
engine->setColor(Qt::red, QIcon::Normal, QIcon::Off);

// Pass it to a QIcon. Note that QIcon takes the ownership of the engine so.
// Therefore it's unsafe to reuse if afterward.
QIcon icon(engine);

btn->setIcon(icon);
```