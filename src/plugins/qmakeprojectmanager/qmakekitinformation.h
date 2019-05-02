/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#pragma once

#include <projectexplorer/kitmanager.h>

namespace QmakeProjectManager {
namespace Internal {

class QmakeKitAspect : public ProjectExplorer::KitAspect
{
    Q_OBJECT

public:
    QmakeKitAspect();

    QList<ProjectExplorer::Task> validate(const ProjectExplorer::Kit *k) const override;
    void setup(ProjectExplorer::Kit *k) override;

    ProjectExplorer::KitAspectWidget *createConfigWidget(ProjectExplorer::Kit *k) const override;

    ItemList toUserOutput(const ProjectExplorer::Kit *k) const override;

    void addToMacroExpander(ProjectExplorer::Kit *kit, Utils::MacroExpander *expander) const override;

    static Core::Id id();
    enum class MkspecSource { User, Code };
    static void setMkspec(ProjectExplorer::Kit *k, const Utils::FileName &fn, MkspecSource source);
    static Utils::FileName mkspec(const ProjectExplorer::Kit *k);
    static Utils::FileName effectiveMkspec(const ProjectExplorer::Kit *k);
    static Utils::FileName defaultMkspec(const ProjectExplorer::Kit *k);
};

} // namespace Internal
} // namespace QmakeProjectManager
