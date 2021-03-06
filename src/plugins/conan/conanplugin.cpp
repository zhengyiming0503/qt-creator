/****************************************************************************
**
** Copyright (C) 2018 Jochen Seemann
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "conaninstallstep.h"
#include "conanplugin.h"
#include "conansettings.h"

#include <coreplugin/icore.h>
#include <projectexplorer/projectmanager.h>
#include <projectexplorer/buildmanager.h>

using namespace Core;

namespace ConanPackageManager {
namespace Internal {

class ConanPluginRunData
{
public:
    ConanInstallStepFactory installStepFactory;
};

ConanPlugin::~ConanPlugin()
{
    delete m_runData;
}

void ConanPlugin::extensionsInitialized()
{ }

bool ConanPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    m_runData = new ConanPluginRunData;
    conanSettings()->fromSettings(ICore::settings());

    return true;
}

ConanSettings *ConanPlugin::conanSettings()
{
    static ConanSettings theSettings;
    return &theSettings;
}


} // namespace Internal
} // namespace ConanPackageManager
