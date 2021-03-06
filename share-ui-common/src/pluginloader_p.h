 
/*
 * share-ui -- Handset UX Share user interface
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Jukka Tiihonen <jukka.tiihonen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _PLUGIN_LOADER_PRIVATE_H_
#define _PLUGIN_LOADER_PRIVATE_H_

#include "ShareUI/pluginloader.h"
#include <QObject>
#include <QString>
#include <QPluginLoader>
#include <QDir>
#include <ShareUI/PluginInterface>
#include <ShareUI/MethodBase>
#include <ShareUI/PluginBase>
#include <QMap>
#include <QSettings>

namespace ShareUI {

    /*!
       \class PluginLoaderPrivate
       \brief Private class for PluginLoader
       \author Jukka Tiihonen <jukka.t.tiihonen@nokia.com>
     */
    class PluginLoaderPrivate : public QObject {
    Q_OBJECT
    public:
        /*!
          \brief Constructor
          \param pluginDir Directory where plugins are loaded
          \param confFile Plugin configuration file with path
          \param parent QObject parent
         */
        PluginLoaderPrivate (const QString & pluginDir, const QString & confFile,
            PluginLoader * parent);
            
        virtual ~PluginLoaderPrivate ();
        
        /*!
          \brief Get plugin name for loader
          \parem loader Loader which name needs to be resolved
          \return Name of plugin behind loader
         */
        static QString loaderToName (QPluginLoader * loader);
        
        /*!
          \brief Get suborder value for given method
          \param method Method
          \param type Type is read from this value not from method
          \return suborder value
         */
        int subOrderValue (ShareUI::MethodBase * base,
            ShareUI::MethodBase::Type type);
        
        /*!
          \brief See PluginLoader::pluginNameForMethod
         */
        QString pluginNameForMethod (ShareUI::MethodBase * method) const;
        
        /*!
          \brief Return order value for method
          \return -1 if method can not be promoted or top level order value if it
                  can be.
         */
        int promotedOrderValue (ShareUI::MethodBase * method);
        
    public Q_SLOTS:

        /*!
          \brief Slot for plugin's newMethod signal
         */
        void newMethodFromPlugin (ShareUI::MethodBase * method);
        
        /*!
          \brief Slot for ShareUI::MethodBase::visible
         */
        void methodVisible (bool visible);

    public:
            
        QDir m_pluginDir; //!< Plugin directory
        
        QList <QPluginLoader *> m_loaders; //!< One loaded for each plugin

        //! Methods received from plugins
        QList <ShareUI::MethodBase *> m_loadedMethods;
        
        /*! Method to plugin map (use loadedMethods to index methods, this
            does not provide static ordering */
        QMap <ShareUI::MethodBase*, ShareUI::PluginBase*> m_methodPluginMap;
        
        //! Plugin to loader map
        QMap <ShareUI::PluginBase*, QPluginLoader *> m_pluginLoaderMap;
        
        //! Plugin config (ordering, promoted plugins)
        QSettings m_pluginConfig;

        QStringList m_promotedPlugins; //!< List of promoted plugins
        QStringList m_serviceOrder; //!< Order list for web services
        QStringList m_otherOrder; //!< Order list for others

    Q_SIGNALS:

        /*!
          \brief See PluginLoader::newMethod
         */
        void newMethod (ShareUI::MethodBase * method);
        
        /*!
          \brief See PluginLoader::methodVisible
         */
        void methodVisible (ShareUI::MethodBase * method, bool visible);

    };
}

#endif