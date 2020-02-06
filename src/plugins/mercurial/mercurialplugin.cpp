#include <vcsbase/vcscommand.h>
class MercurialTopicCache : public Core::IVersionControl::TopicCache
{
public:
    MercurialTopicCache(MercurialClient *client) : m_client(client) {}

protected:
    QString trackFile(const QString &repository) override
    {
        return repository + QLatin1String("/.hg/branch");
    }

    QString refreshTopic(const QString &repository) override
    {
        return m_client->branchQuerySync(repository);
    }

private:
    MercurialClient *m_client;
};

class MercurialPluginPrivate final : public VcsBase::VcsBasePluginPrivate
{
    Q_DECLARE_TR_FUNCTIONS(Mercurial::Internal::MercurialPlugin)

public:
    MercurialPluginPrivate();

    // IVersionControl
    QString displayName() const final;
    Core::Id id() const final;
    bool isVcsFileOrDirectory(const Utils::FilePath &fileName) const final;

    bool managesDirectory(const QString &filename, QString *topLevel = nullptr) const final;
    bool managesFile(const QString &workingDirectory, const QString &fileName) const final;
    bool isConfigured() const final;
    bool supportsOperation(Operation operation) const final;
    bool vcsOpen(const QString &fileName) final;
    bool vcsAdd(const QString &filename) final;
    bool vcsDelete(const QString &filename) final;
    bool vcsMove(const QString &from, const QString &to) final;
    bool vcsCreateRepository(const QString &directory) final;
    bool vcsAnnotate(const QString &file, int line) final;

    Core::ShellCommand *createInitialCheckoutCommand(const QString &url,
                                                     const Utils::FilePath &baseDirectory,
                                                     const QString &localName,
                                                     const QStringList &extraArgs) final;

    bool sccManaged(const QString &filename);

    // To be connected to the HgTask's success signal to emit the repository/
    // files changed signals according to the variant's type:
    // String -> repository, StringList -> files
    void changed(const QVariant&);

private:
    void updateActions(VcsBase::VcsBasePluginPrivate::ActionState) final;
    bool submitEditorAboutToClose() final;

    // File menu action slots
    void addCurrentFile();
    void annotateCurrentFile();
    void diffCurrentFile();
    void logCurrentFile();
    void revertCurrentFile();
    void statusCurrentFile();

    // Directory menu action slots
    void diffRepository();
    void logRepository();
    void revertMulti();
    void statusMulti();

    // Repository menu action slots
    void pull();
    void push();
    void update();
    void import();
    void incoming();
    void outgoing();
    void commit();
    void showCommitWidget(const QList<VcsBase::VcsBaseClient::StatusItem> &status);
    void commitFromEditor() override;
    void diffFromEditorSelected(const QStringList &files);

    void createMenu(const Core::Context &context);
    void createFileActions(const Core::Context &context);
    void createDirectoryActions(const Core::Context &context);
    void createRepositoryActions(const Core::Context &context);

    // Variables
    MercurialSettings m_settings;
    MercurialClient m_client{&m_settings};

    OptionsPage m_optionsPage{[this] { configurationChanged(); }, &m_settings};

    Core::CommandLocator *m_commandLocator = nullptr;
    Core::ActionContainer *m_mercurialContainer = nullptr;

    QList<QAction *> m_repositoryActionList;

    // Menu items (file actions)
    ParameterAction *m_addAction = nullptr;
    ParameterAction *m_deleteAction = nullptr;
    ParameterAction *annotateFile = nullptr;
    ParameterAction *diffFile = nullptr;
    ParameterAction *logFile = nullptr;
    ParameterAction *revertFile = nullptr;
    ParameterAction *statusFile = nullptr;

    QAction *m_createRepositoryAction = nullptr;
    QAction *m_menuAction = nullptr;

    QString m_submitRepository;

    bool m_submitActionTriggered = false;
};
static MercurialPluginPrivate *dd = nullptr;
    : VcsBase::VcsBasePluginPrivate(Core::Context(Constants::MERCURIAL_CONTEXT))
    setTopicCache(new MercurialTopicCache(&m_client));
    Core::Context context(Constants::MERCURIAL_CONTEXT);
    connect(&m_client, &VcsBaseClient::changed, this, &MercurialPluginPrivate::changed);
    connect(&m_client, &MercurialClient::needUpdate, this, &MercurialPluginPrivate::update);
        m_client.view(source, id);
    const auto widgetCreator = [this] { return new MercurialEditorWidget(&m_client); };
    new VcsSubmitEditorFactory(submitEditorParameters, [] { return new CommitEditor; }, this);
    m_client.synchronousAdd(state.currentFileTopLevel(), state.relativeCurrentFile());
    m_client.annotate(state.currentFileTopLevel(), state.relativeCurrentFile(), QString(), currentLine);
    m_client.diff(state.currentFileTopLevel(), QStringList(state.relativeCurrentFile()));
    m_client.log(state.currentFileTopLevel(), QStringList(state.relativeCurrentFile()),
                 QStringList(), true);
    m_client.revertFile(state.currentFileTopLevel(), state.relativeCurrentFile(), reverter.revision());
    m_client.status(state.currentFileTopLevel(), state.relativeCurrentFile());
    m_client.diff(state.topLevel());
    m_client.log(state.topLevel());
    m_client.revertAll(state.topLevel(), reverter.revision());
    m_client.status(state.topLevel());
    SrcDestDialog dialog(state, SrcDestDialog::incoming, Core::ICore::dialogParent());
    m_client.synchronousPull(dialog.workingDir(), dialog.getRepositoryString());
    SrcDestDialog dialog(state, SrcDestDialog::outgoing, Core::ICore::dialogParent());
    m_client.synchronousPush(dialog.workingDir(), dialog.getRepositoryString());
    m_client.update(state.topLevel(), updateDialog.revision());
    m_client.import(state.topLevel(), fileNames);
    SrcDestDialog dialog(state, SrcDestDialog::incoming, Core::ICore::dialogParent());
    m_client.incoming(state.topLevel(), dialog.getRepositoryString());
    m_client.outgoing(state.topLevel());
    connect(&m_client, &MercurialClient::parsedStatus, this, &MercurialPluginPrivate::showCommitWidget);
    m_client.emitParsedStatus(m_submitRepository);
    disconnect(&m_client, &MercurialClient::parsedStatus, this, &MercurialPluginPrivate::showCommitWidget);
    const QString branch = vcsTopic(m_submitRepository);
                            m_settings.stringValue(MercurialSettings::userNameKey),
                            m_settings.stringValue(MercurialSettings::userEmailKey), status);
    m_client.diff(m_submitRepository, files);
        m_client.commit(m_submitRepository, files, editorFile->filePath().toString(),
                        extraOptions);
QString MercurialPluginPrivate::displayName() const
{
    return tr("Mercurial");
}

Core::Id MercurialPluginPrivate::id() const
{
    return {VcsBase::Constants::VCS_ID_MERCURIAL};
}

bool MercurialPluginPrivate::isVcsFileOrDirectory(const Utils::FilePath &fileName) const
{
    return m_client.isVcsDirectory(fileName);
}

bool MercurialPluginPrivate::managesDirectory(const QString &directory, QString *topLevel) const
{
    QFileInfo dir(directory);
    const QString topLevelFound = m_client.findTopLevelForFile(dir);
    if (topLevel)
        *topLevel = topLevelFound;
    return !topLevelFound.isEmpty();
}

bool MercurialPluginPrivate::managesFile(const QString &workingDirectory, const QString &fileName) const
{
    return m_client.managesFile(workingDirectory, fileName);
}

bool MercurialPluginPrivate::isConfigured() const
{
    const Utils::FilePath binary = m_settings.binaryPath();
    if (binary.isEmpty())
        return false;
    QFileInfo fi = binary.toFileInfo();
    return fi.exists() && fi.isFile() && fi.isExecutable();
}

bool MercurialPluginPrivate::supportsOperation(Operation operation) const
{
    bool supported = isConfigured();
    switch (operation) {
    case Core::IVersionControl::AddOperation:
    case Core::IVersionControl::DeleteOperation:
    case Core::IVersionControl::MoveOperation:
    case Core::IVersionControl::CreateRepositoryOperation:
    case Core::IVersionControl::AnnotateOperation:
    case Core::IVersionControl::InitialCheckoutOperation:
        break;
    case Core::IVersionControl::SnapshotOperations:
        supported = false;
        break;
    }
    return supported;
}

bool MercurialPluginPrivate::vcsOpen(const QString &filename)
{
    Q_UNUSED(filename)
    return true;
}

bool MercurialPluginPrivate::vcsAdd(const QString &filename)
{
    const QFileInfo fi(filename);
    return m_client.synchronousAdd(fi.absolutePath(), fi.fileName());
}

bool MercurialPluginPrivate::vcsDelete(const QString &filename)
{
    const QFileInfo fi(filename);
    return m_client.synchronousRemove(fi.absolutePath(), fi.fileName());
}

bool MercurialPluginPrivate::vcsMove(const QString &from, const QString &to)
{
    const QFileInfo fromInfo(from);
    const QFileInfo toInfo(to);
    return m_client.synchronousMove(fromInfo.absolutePath(),
                                            fromInfo.absoluteFilePath(),
                                            toInfo.absoluteFilePath());
}

bool MercurialPluginPrivate::vcsCreateRepository(const QString &directory)
{
    return m_client.synchronousCreateRepository(directory);
}

bool MercurialPluginPrivate::vcsAnnotate(const QString &file, int line)
{
    const QFileInfo fi(file);
    m_client.annotate(fi.absolutePath(), fi.fileName(), QString(), line);
    return true;
}

Core::ShellCommand *MercurialPluginPrivate::createInitialCheckoutCommand(const QString &url,
                                                                   const Utils::FilePath &baseDirectory,
                                                                   const QString &localName,
                                                                   const QStringList &extraArgs)
{
    QStringList args;
    args << QLatin1String("clone") << extraArgs << url << localName;
    auto command = new VcsBase::VcsCommand(baseDirectory.toString(),
                                           m_client.processEnvironment());
    command->addJob({m_settings.binaryPath(), args}, -1);
    return command;
}

bool MercurialPluginPrivate::sccManaged(const QString &filename)
{
    const QFileInfo fi(filename);
    QString topLevel;
    const bool managed = managesDirectory(fi.absolutePath(), &topLevel);
    if (!managed || topLevel.isEmpty())
        return false;
    const QDir topLevelDir(topLevel);
    return m_client.manifestSync(topLevel, topLevelDir.relativeFilePath(filename));
}

void MercurialPluginPrivate::changed(const QVariant &v)
{
    switch (v.type()) {
    case QVariant::String:
        emit repositoryChanged(v.toString());
        break;
    case QVariant::StringList:
        emit filesChanged(v.toStringList());
        break;
    default:
        break;
    }
}
