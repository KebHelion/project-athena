//
//  InfoView
//  hifi
//
//  Created by Stojce Slavkovski on 9/7/13.
//  Copyright (c) 2013 HighFidelity, Inc. All rights reserved.
//

#include "InfoView.h"
#include <QApplication>
#include "Application.h"

#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKit/QWebElement>
#include <QDesktopWidget>

#define SETTINGS_VERSION_KEY "info-version"
#define MAX_DIALOG_HEIGHT_RATIO 0.9

InfoView::InfoView(bool forced) {
    _forced = forced;
    settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    
    switchToResourcesParentIfRequired();
    QString absPath = QFileInfo("resources/html/interface-welcome-allsvg.html").absoluteFilePath();
    QUrl url = QUrl::fromLocalFile(absPath);
    
    load(url);
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(loaded(bool)));
}

void InfoView::showFirstTime() {
    new InfoView(false);
}

void InfoView::forcedShow() {
    new InfoView(true);
}

bool InfoView::shouldShow() {
    if (_forced) {
        return true;
    }
    
    QSettings* settings = Application::getInstance()->getSettings();
    
    QString lastVersion = settings->value(SETTINGS_VERSION_KEY).toString();
    
    QWebFrame* mainFrame = page()->mainFrame();
    QWebElement versionTag = mainFrame->findFirstElement("#version");
    QString version = versionTag.attribute("value");
    
    if (lastVersion == QString::null || version == QString::null || lastVersion != version) {
        if (version != QString::null) {
            settings->setValue(SETTINGS_VERSION_KEY, version);
        }
        return true;
    }
    
    return false;
}

void InfoView::loaded(bool ok) {
    if (!ok || !shouldShow()) {
        return;
    }
    
    QDesktopWidget* desktop = Application::getInstance()->desktop();
    QWebFrame* mainFrame = page()->mainFrame();
    
    int height = mainFrame->contentsSize().height() > desktop->height() ?
        desktop->height() * MAX_DIALOG_HEIGHT_RATIO :
        mainFrame->contentsSize().height();
    
    resize(mainFrame->contentsSize().width(), height);
    move(desktop->screen()->rect().center() - rect().center());
    setWindowTitle(title());
    show();
}
