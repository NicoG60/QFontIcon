# QFontIconEngine



QIconEngine subclass to handle font glyphs as icons.  [More...](#detailed-description)

Inherits from QIconEngine

## Public Functions

|                       | Name                                                         |
| --------------------- | ------------------------------------------------------------ |
|                       | **[QFontIconEngine](#function-qfonticonengine)**()           |
|                       | **[QFontIconEngine](#function-qfonticonengine)**(const [QFontIconEngine]() & other)<br>Copy contructor. |
|                       | **[QFontIconEngine](#function-qfonticonengine)**(int icon, int font =[defaultFont](#function-defaultfont)()) =default<br>Construct and engine using _icon_ and _font_. |
|                       | **[QFontIconEngine](#function-qfonticonengine)**(const QString & icon, const QString & font ={})<br>Construct and engine using _icon_ and _font_. |
|                       | **[~QFontIconEngine](#function-~qfonticonengine)**() override =default |
| bool                  | **[isValid](#function-isvalid)**() const<br>Whether the engine is valid. |
| int                   | **[icon](#function-icon)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the icon code point set for the given state. |
| QString               | **[iconName](#function-iconname)**() const override<br>Returns the default icon name. |
| QString               | **[iconName](#function-iconname)**(QIcon::Mode mode, QIcon::State state) const<br>Returns the icon name set for the given state. |
| QString               | **[text](#function-text)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the icon code point as a text string for the given state. |
| int                   | **[font](#function-font)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the font id set for the given state. |
| QString               | **[fontName](#function-fontname)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the font name set for the given state. |
| qreal                 | **[scaleFactor](#function-scalefactor)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the scale factor set for the given state. |
| QColor                | **[color](#function-color)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the color set for the given state. |
| qreal                 | **[speed](#function-speed)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the rotation speed set for the given state. |
| QEasingCurve          | **[curve](#function-curve)**(QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off) const<br>Returns the easing curve set for the given state. |
| QWidget *             | **[widget](#function-widget)**() const<br>Returns the widget the rotation animation displays on. |
| void                  | **[setIcon](#function-seticon)**(int icon, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the icon code point for the given state. |
| void                  | **[setIcon](#function-seticon)**(const QString & name, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the icon name for the given state. |
| void                  | **[setFont](#function-setfont)**(int font, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the font id for the given state. |
| void                  | **[setFont](#function-setfont)**(const QString & name, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the font name for the given state. |
| void                  | **[setScaleFactor](#function-setscalefactor)**(qreal scale, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the scale factor for the given state. |
| void                  | **[setColor](#function-setcolor)**(const QColor & color, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the color for the given state. |
| void                  | **[setSpeed](#function-setspeed)**(qreal speed, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the rotation speed for the given state. |
| void                  | **[setCurve](#function-setcurve)**(const QEasingCurve & curve, QIcon::Mode mode =QIcon::Normal, QIcon::State state =QIcon::Off)<br>Set the easing curve point for the given state. |
| void                  | **[setWidget](#function-setwidget)**(QWidget * widget)<br>Set the widget the rotation animation displays on. |
| [QFontIconEngine]() * | **[clone](#function-clone)**() const override                |
| QSize                 | **[actualSize](#function-actualsize)**(const QSize & size, QIcon::Mode mode, QIcon::State state) override |
| void                  | **[paint](#function-paint)**(QPainter * painter, const QRect & rect, QIcon::Mode mode, QIcon::State state) override |
| QPixmap               | **[pixmap](#function-pixmap)**(const QSize & size, QIcon::Mode mode, QIcon::State state) override |
| void                  | **[virtual_hook](#function-virtual_hook)**(int id, void * data) override |
| bool                  | **[loadFont](#function-loadfont)**(const QString & filename, int font =[defaultFont](#function-defaultfont)(), const QString & name ={}) =default<br>load the font located at _filename_. |
| QIcon                 | **[icon](#function-icon)**(int icon, int font =[defaultFont](#function-defaultfont)()) =default<br>Convenience function that returns an icon. |
| QIcon                 | **[icon](#function-icon)**(const QString & icon, const QString & font ={})<br>Convenience function that returns an icon. |
| void                  | **[setDefaultFont](#function-setdefaultfont)**(int font)<br>Set the default font to use. |
| int                   | **[defaultFont](#function-defaultfont)**()<br>Returns the currently set default font. |
| bool                  | **[registerIconName](#function-registericonname)**(QString name, int code)<br>Register a name for the given code point. |
| bool                  | **[registerIconName](#function-registericonname)**(const QMap< QString, int > & names)<br>Register all the names / code points provided. |
| bool                  | **[registerFontName](#function-registerfontname)**(QString name, int font)<br>Register a font name for the given font id. |
| bool                  | **[registerFontName](#function-registerfontname)**(const QMap< QString, int > & names)<br>Register all the names / font ids provided. |

## Detailed Description

```cpp
class QFontIconEngine;
```

QIconEngine subclass to handle font glyphs as icons. 

[QFontIconEngine]() can be used with any fonts. You presumably have embedded your favourite glyph font in a `.qrc` file. The first thing you need to do is loading you font using the [loadFont()](#function-loadfont) static function:



```cpp
QFontIconEngine::loadFont(":/path/to/my_font.ttf");
```

Then you can use the [icon()](#function-icon) static helper function to easily create icons from the font you just loaded: 

```cpp
QIcon icon = QFontIconEngine::icon(0xf82b) // This is the code point of the glyph

// Alternatively, you can make an enumeration and / or register string base names
enum glyph_code {
    super_glyph = 0xf82b
};

QFontIconEngine::registerIconName("super-glyph", super_glyph);

// And then use names instead:
QIcon otherIcon = QFontIconEngine::icon("super-glyph");
```

[QFontIconEngine]() supports stateful icons. That means you can set different glyphs and various other properties on a per-state basis, depending on `QIcon::Mode` and `QIcon::State` . For example:



```cpp
// Assuming you have registered icon names
// create the engine and then set properties
auto engine = new QFontIconEngine();
engine->setIcon("toggle-on",  QIcon::Normal, QIcon::On);
engine->setIcon("toggle-off", QIcon::Normal, QIcon::Off);

engine->setColor(Qt::green, QIcon::Normal, QIcon::On);
engine->setColor(Qt::red,   QIcon::Normal, QIcon::Off);

// Pass it to a QIcon. Note that QIcon takes the ownership of the engine so.
// Therefore it's unsafe to reuse if afterward.
QIcon icon(engine);
```

The default state is `{ QIcon::Normal, QIcon::Off }` It is the one set by default when using helper fuctions. If you create and engine from scratch, this state must be set.

If a value is not available in a given state `{ M, S }` , it will first try to find a `{ QIcon::Normal, S }` and if still not successful, will ultimately default to `{ QIcon::Normal, QIcon::Off }`.

Properties also include a spinning animation. (Yep) 

```cpp
// Assuming you'll put it on a button
auto btn = new QPushButton();

auto engine = new QFontIconEngine();
engine->setIcon("spinner");

engine->setWidget(btn); // You need to tell the engine which widget to update
engine->setSpeed(180); // speed is in degree per second. 180 <=> 1 turn in 2s
engine->setCurve(QEasingCurve::InOutSine); // You can even set non-linear rotation.

// Pass it to a QIcon and profits
QIcon icon(engine);
```

You can load multiple fonts and use them seamlessly. When loading a font you can specify a arbitrary font id and name which will help you differenciate the fonts. 

```cpp
enum icon_set
{
    primary_font,
    secondary_font
};

QFontIconEngine::loadFont(":/path/to/my_font.ttf", primary_font, "primary");
QFontIconEngine::loadFont(":/path/to/my_font.ttf", secondary_font, "secondary");

// Set a default
QFontIconEngine::setDefaultFont(primary_font);

// Then you can pass either the font id of the font name depending on the
// overload you use:
QIcon icon = QFontIconEngine::icon("super-glyph", "secondary");

// This one will use the default font set earlier
QIcon icon2 = QFontIconEngine::icon("super-glyph");
```


## Public Functions Documentation

### function QFontIconEngine

```cpp
QFontIconEngine()
```


**See**: [isValid()](#function-isvalid)

Construct and engine with the [defaultFont()](#function-defaultfont) set but no icon. It is therefore invalid.


### function QFontIconEngine

```cpp
QFontIconEngine(
    const QFontIconEngine & other
)
```

Copy constructor. 

### function QFontIconEngine

```cpp
QFontIconEngine(
    int icon,
    int font =defaultFont()
) =default
```

Construct and engine using _icon_ and _font_. 

**Parameters**: 

  * **icon** the code point fo the icon to use 
  * **font** the font id to use 


_icon_ and _font_ will set the { QIcon::Normal, QIcon::Off } state and therefore be used by default.


### function QFontIconEngine

```cpp
QFontIconEngine(
    const QString & icon,
    const QString & font ={}
)
```

Construct and engine using _icon_ and _font_. 

**Parameters**: 

  * **icon** the glyph name fo the icon to use 
  * **font** the font name to use 


This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.


### function ~QFontIconEngine

```cpp
~QFontIconEngine() override =default
```


### function isValid

```cpp
bool isValid() const
```

Whether the engine is valid. 

Invalid engine results in null icon and wont draw.

An engine is considered valid if:

* it has an default and valid icon (valid code point or name)
* it has a default and valid font (loaded font and valid name) 


### function icon

```cpp
int icon(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the icon code point set for the given state. 

### function iconName

```cpp
QString iconName() const override
```

Returns the default icon name. 

### function iconName

```cpp
QString iconName(
    QIcon::Mode mode,
    QIcon::State state
) const
```

Returns the icon name set for the given state. 

### function text

```cpp
QString text(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the icon code point as a text string for the given state. 

### function font

```cpp
int font(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the font id set for the given state. 

### function fontName

```cpp
QString fontName(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the font name set for the given state. 

### function scaleFactor

```cpp
qreal scaleFactor(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the scale factor set for the given state. 

### function color

```cpp
QColor color(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the color set for the given state. 

### function speed

```cpp
qreal speed(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the rotation speed set for the given state. 

### function curve

```cpp
QEasingCurve curve(
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
) const
```

Returns the easing curve set for the given state. 

### function widget

```cpp
QWidget * widget() const
```

Returns the widget the rotation animation displays on. 

### function setIcon

```cpp
void setIcon(
    int icon,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the icon code point for the given state. 

### function setIcon

```cpp
void setIcon(
    const QString & name,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the icon name for the given state. 

### function setFont

```cpp
void setFont(
    int font,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the font id for the given state. 

### function setFont

```cpp
void setFont(
    const QString & name,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the font name for the given state. 

### function setScaleFactor

```cpp
void setScaleFactor(
    qreal scale,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the scale factor for the given state. 

### function setColor

```cpp
void setColor(
    const QColor & color,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the color for the given state. 

### function setSpeed

```cpp
void setSpeed(
    qreal speed,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the rotation speed for the given state. 

### function setCurve

```cpp
void setCurve(
    const QEasingCurve & curve,
    QIcon::Mode mode =QIcon::Normal,
    QIcon::State state =QIcon::Off
)
```

Set the easing curve point for the given state. 

### function setWidget

```cpp
void setWidget(
    QWidget * widget
)
```

Set the widget the rotation animation displays on. 

### function clone

```cpp
QFontIconEngine * clone() const override
```


### function actualSize

```cpp
QSize actualSize(
    const QSize & size,
    QIcon::Mode mode,
    QIcon::State state
) override
```


### function paint

```cpp
void paint(
    QPainter * painter,
    const QRect & rect,
    QIcon::Mode mode,
    QIcon::State state
) override
```


### function pixmap

```cpp
QPixmap pixmap(
    const QSize & size,
    QIcon::Mode mode,
    QIcon::State state
) override
```


### function virtual_hook

```cpp
void virtual_hook(
    int id,
    void * data
) override
```


### function loadFont

```cpp
static bool loadFont(
    const QString & filename,
    int font =defaultFont(),
    const QString & name ={}
) =default
```

load the font located at _filename_. 

You can pass an arbitrary font id and font name if you're using multiple fonts. 


### function icon

```cpp
static QIcon icon(
    int icon,
    int font =defaultFont()
) =default
```

Convenience function that returns an icon. 

### function icon

```cpp
static QIcon icon(
    const QString & icon,
    const QString & font ={}
)
```

Convenience function that returns an icon. 

### function setDefaultFont

```cpp
static void setDefaultFont(
    int font
)
```

Set the default font to use. 

Before calling this function, the default font is set to 0. 


### function defaultFont

```cpp
static int defaultFont()
```

Returns the currently set default font. 

### function registerIconName

```cpp
static bool registerIconName(
    QString name,
    int code
)
```

Register a name for the given code point. 

### function registerIconName

```cpp
static bool registerIconName(
    const QMap< QString, int > & names
)
```

Register all the names / code points provided. 

### function registerFontName

```cpp
static bool registerFontName(
    QString name,
    int font
)
```

Register a font name for the given font id. 

### function registerFontName

```cpp
static bool registerFontName(
    const QMap< QString, int > & names
)
```

Register all the names / font ids provided. 