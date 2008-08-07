/***************************************************************************
 *   Copyright (C) 2005-08 by the Quassel Project                          *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QDebug>

#ifdef Q_WS_QWS
#include <Qtopia>
#endif

#include "settings.h"

static QHash<QString, QHash<QString, QVariant> > __settingsCache__;

Settings::Settings(QString g, QString applicationName)

#ifdef Q_WS_MAC
  : QSettings(QCoreApplication::organizationDomain(), applicationName),
#else
  : QSettings(QCoreApplication::organizationName(), applicationName),
#endif
    group(g)
{

/* we need to call the constructor immediately in order to set the path...
#ifndef Q_WS_QWS
  QSettings(QCoreApplication::organizationName(), applicationName);
#else
  // FIXME sandboxDir() is not currently working correctly...
  //if(Qtopia::sandboxDir().isEmpty()) QSettings();
  //else QSettings(Qtopia::sandboxDir() + "/etc/QuasselIRC.conf", QSettings::NativeFormat);
  // ...so we have to use a workaround:
  QString appPath = QCoreApplication::applicationFilePath();
  if(appPath.startsWith(Qtopia::packagePath())) {
    QString sandboxPath = appPath.left(Qtopia::packagePath().length() + 32);
    QSettings(sandboxPath + "/etc/QuasselIRC.conf", QSettings::IniFormat);
    qDebug() << sandboxPath + "/etc/QuasselIRC.conf";
  } else {
    QSettings(QCoreApplication::organizationName(), applicationName);
  }
#endif
*/
}

QStringList Settings::allLocalKeys() {
  beginGroup(group);
  QStringList res = allKeys();
  endGroup();
  return res;
}

QStringList Settings::localChildKeys(const QString &rootkey) {
  QString g;
  if(rootkey.isEmpty()) g = group;
  else g = QString("%1/%2").arg(group, rootkey);
  beginGroup(g);
  QStringList res = childKeys();
  endGroup();
  return res;
}

QStringList Settings::localChildGroups(const QString &rootkey) {
  QString g;
  if(rootkey.isEmpty()) g = group;
  else g = QString("%1/%2").arg(group, rootkey);
  beginGroup(g);
  QStringList res = childGroups();
  endGroup();
  return res;
}

void Settings::setLocalValue(const QString &key, const QVariant &data) {
  beginGroup(group);
  setValue(key, data);
  setCacheValue(group, key, data);
  endGroup();
}

const QVariant &Settings::localValue(const QString &key, const QVariant &def) {
  if(!isCached(group, key)) {
    beginGroup(group);
    setCacheValue(group, key, value(key, def));
    endGroup();
  }
  return cacheValue(group, key);
}

void Settings::removeLocalKey(const QString &key) {
  beginGroup(group);
  remove(key);
  endGroup();
}


void Settings::setCacheValue(const QString &group, const QString &key, const QVariant &data) {
  ::__settingsCache__[group][key] = data;
}

const QVariant &Settings::cacheValue(const QString &group, const QString &key) {
  return ::__settingsCache__[group][key];
}

bool Settings::isCached(const QString &group, const QString &key) {
  return ::__settingsCache__.contains(group) && ::__settingsCache__[group].contains(key);
}
