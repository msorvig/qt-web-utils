
#include "qtwebutils.h"

#include <QtCore/private/qstdweb_p.h>
#include <QtGui/qfontdatabase.h>

typedef std::multimap<QString, emscripten::val> FontFamiliesMap;
Q_GLOBAL_STATIC(FontFamiliesMap, g_fontFamiliesFonts);
Q_GLOBAL_STATIC(QStringList, g_fontFamilies);

namespace {

bool verifyLocalFontsSupport()
{
    emscripten::val window = emscripten::val::global("window");
    emscripten::val queryFn = window["queryLocalFonts"];
    static bool didwarn = false;
    if (queryFn.isUndefined() && !didwarn) {
        didwarn = true;
        qWarning() << "This browser does not support local font access (window.queryLocalFonts is undefined)";
        return false;
    }
    return true;
}

void queryLocalFonts(std::function<void(const QStringList &)> familiesCallback)
{
    emscripten::val window = emscripten::val::global("window");
    qstdweb::Promise::make(window, "queryLocalFonts", {
        .thenFunc = [familiesCallback](emscripten::val fontArray) {
            const int count = fontArray["length"].as<int>();
            for (int i = 0; i < count; ++i) {
                const emscripten::val font = fontArray.call<emscripten::val>("at", i);
                const QString family = QString::fromStdString(font["family"].as<std::string>());
                g_fontFamiliesFonts->insert(std::make_pair(family, font));
                if (!g_fontFamilies->contains(family))
                    g_fontFamilies->append(family);
                if (i == count - 1 && familiesCallback)
                    familiesCallback(*g_fontFamilies);
            }
        },
        .catchFunc = [familiesCallback](emscripten::val err) {
            qWarning() << "Error while trying to query local-fonts API"
                << QString::fromStdString(err["name"].as<std::string>())
                << QString::fromStdString(err["message"].as<std::string>());
            if (familiesCallback)
                familiesCallback(*g_fontFamilies);
        }
    });
}

void populateFontBlob(emscripten::val fontBlob, std::function<void(int)> populatedCallback)
{
    qstdweb::Promise::make(fontBlob, "arrayBuffer", {
        .thenFunc = [populatedCallback](emscripten::val fontArrayBuffer) {
            QByteArray fontData = qstdweb::Uint8Array(qstdweb::ArrayBuffer(fontArrayBuffer)).copyToQByteArray();
            static quint64 totalSize = 0;
            totalSize += fontData.length();
            // qDebug() << "total font data size" << totalSize;
            int id = QFontDatabase::addApplicationFontFromData(fontData);
            if (populatedCallback)
                populatedCallback(id);
        },
        .catchFunc = [](emscripten::val err) {
            qWarning() << "Error while loading font ArrayBuffer"
                << QString::fromStdString(err["name"].as<std::string>())
                << QString::fromStdString(err["message"].as<std::string>());
        }
    });
}

void populateFontFamily(const QString &familiy, std::function<void(const QList<int> &)> populatedCallback)
{
    auto fontsRange = g_fontFamiliesFonts->equal_range(familiy);
    struct State {
        QList<int> fontIds;
        int fontCounter;
    };
    State *state = new State();
    state->fontCounter = std::distance(fontsRange.first, fontsRange.second);
    for (auto it = fontsRange.first; it != fontsRange.second; ++it) {
        emscripten::val font = it->second;
        qstdweb::Promise::make(font, "blob", {
            .thenFunc = [state, populatedCallback](emscripten::val blob) {
                populateFontBlob(blob, [state, populatedCallback](int id){
                    state->fontIds.append(id);
                    state->fontCounter--;
                    if (state->fontCounter == 0) {
                        if (populatedCallback)
                            populatedCallback(state->fontIds);
                        delete state;
                    }
                });
            },
            .catchFunc = [state, familiy](emscripten::val err) {
                qWarning() << "Error while loading font Blob for family" << familiy
                    << QString::fromStdString(err["name"].as<std::string>())
                    << QString::fromStdString(err["message"].as<std::string>());
                delete state;
            }
        });
    }
}

} // namespace

/*
    Local font support.

    The user must grant permission to access local fonts. The browser will prompt
    the user to grant permission on the first call to window.queryLocalFonts(), or
    in this case on the first call of any of the functions below. The first call
    must happen in response to user input, for example a button click.

    Fonts are loaded fully into memory as freetype memory fonts. The full installed
    local font set will often contain several gigabytes of font data, and loading
    all fonts is not recomended.
*/

/*!
    Queries for available local font families.
*/
void qtwebutils::getFontFamilies(std::function<void(const QStringList &)> familiesCallback)
{
    if (!verifyLocalFontsSupport())
        return;

    if (g_fontFamiliesFonts->empty()) {
        if (familiesCallback)
            familiesCallback(*g_fontFamilies);
    } else {
        queryLocalFonts(familiesCallback);
    }
}

/*!
    Populates the font database with local fonts for the given font family.

    The fonts are installed as application fonts, e.g. added with
    QFontDatabase::addApplicationFontFromData().
*/
void qtwebutils::populateFontFamily(const QString &familiy, std::function<void(const QList<int> &)> populatedCallback)
{
    if (!verifyLocalFontsSupport())
        return;

    if (g_fontFamiliesFonts->empty()) {
        queryLocalFonts([familiy, populatedCallback](const QStringList &){
            // g_fontFamiliesFonts should now be populated
            ::populateFontFamily(familiy, populatedCallback);
        });
    } else {
        ::populateFontFamily(familiy, populatedCallback);
    }
}

/*!
    Populates the font database with local fonts for the given font families.

    The fonts are installed as application fonts, e.g. added with
    QFontDatabase::addApplicationFontFromData().
*/
void qtwebutils::populateFontFamilies(const QStringList &families, std::function<void(const QList<int> &)> populatedCallback)
{
    if (!verifyLocalFontsSupport())
        return;

    if (g_fontFamiliesFonts->empty()) {
        queryLocalFonts([families, populatedCallback](const QStringList &){
            // TODO: combine callbacks into one update for all families
            for (const QString &family : families)
                populateFontFamily(family, populatedCallback);
        });
    } else {
        for (const QString &family : families)
            populateFontFamily(family, populatedCallback);
    }
}

/*!
    Returns a list of "web safe" font families.

*/
QStringList qtwebutils::webSafeFontFamilies()
{
    return {"Arial", "Verdana", "Tahoma", "Trebuchet", "Times New Roman",
            "Georgia", "Garamond", "Courier New"};
}

/*!
    Populates the font database with local fonts for the "web safe" font families.

    The fonts are installed as application fonts, e.g. added with
    QFontDatabase::addApplicationFontFromData().
*/
void qtwebutils::populateWebSafeFamilies(std::function<void(const QList<int> &)> populatedCallback)
{
    populateFontFamilies(webSafeFontFamilies(), populatedCallback);
}
