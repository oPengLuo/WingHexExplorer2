#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <QDir>
#include <QFileSystemWatcher>
#include <QHash>
#include <QObject>

#include "QWingRibbon/ribbonbuttongroup.h"
#include "control/scriptingconsole.h"
#include "utilities.h"

class ScriptManager : public QObject {
    Q_OBJECT

public:
    struct ScriptDirMeta {
        QString rawName; // a flag
        QString name;
        QString author;
        QString license;
        QString homepage;
        QString comment;
        bool isSys; // a flag
    };

    struct ScriptActionMaps {
        QList<QToolButton *> sysList;
        QList<QToolButton *> usrList;
    };

public:
    static ScriptManager &instance();

    QString userScriptPath() const;

    QString systemScriptPath() const;

    QStringList usrScriptsDbCats() const;

    QStringList sysScriptsDbCats() const;

    QStringList getUsrScriptFileNames(const QString &cat) const;

    QStringList getSysScriptFileNames(const QString &cat) const;

    void refresh();
    void refreshUsrScriptsDbCats();
    void refreshSysScriptsDbCats();

    void attach(ScriptingConsole *console);
    void detach();

    ScriptDirMeta usrDirMeta(const QString &cat) const;
    ScriptDirMeta sysDirMeta(const QString &cat) const;

    static ScriptActionMaps buildUpRibbonScriptRunner(RibbonButtonGroup *group);

private:
    static QToolButton *addPannelAction(RibbonButtonGroup *pannel,
                                        const QString &iconName,
                                        const QString &title,
                                        QMenu *menu = nullptr) {
        return addPannelAction(pannel, ICONRES(iconName), title, menu);
    }

    static QToolButton *addPannelAction(RibbonButtonGroup *pannel,
                                        const QIcon &icon, const QString &title,
                                        QMenu *menu = nullptr) {
        Q_ASSERT(pannel);
        auto a = new QToolButton(pannel);
        a->setText(title);
        a->setIcon(icon);
        a->setMenu(menu);
        if (menu) {
            a->setPopupMode(QToolButton::InstantPopup);
        }
        pannel->addButton(a);
        return a;
    }

public slots:
    void runScript(const QString &filename);

private:
    explicit ScriptManager();
    virtual ~ScriptManager();

    Q_DISABLE_COPY(ScriptManager)

    ScriptDirMeta ensureDirMeta(const QFileInfo &info);

    QStringList getScriptFileNames(const QDir &dir) const;

    QString readJsonObjString(const QJsonObject &jobj, const QString &key);

    static QMenu *buildUpScriptDirMenu(QWidget *parent,
                                       const QStringList &files, bool isSys);

private:
    QString m_sysScriptsPath;
    QString m_usrScriptsPath;
    QStringList m_usrScriptsDbCats;
    QStringList m_sysScriptsDbCats;

    QHash<QString, ScriptDirMeta> _usrDirMetas;
    QHash<QString, ScriptDirMeta> _sysDirMetas;

    ScriptingConsole *_console = nullptr;
};

Q_DECLARE_METATYPE(ScriptManager::ScriptDirMeta)

#endif // SCRIPTMANAGER_H
