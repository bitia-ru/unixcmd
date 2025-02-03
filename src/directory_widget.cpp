#include "directory_widget.h"

#include "directory_status_bar.h"
#include "directory_view.h"

#include <QDir>
#include <QLabel>
#include <QVBoxLayout>


struct PathComponent
{
    QString text;
    bool isHome = false;
    bool isShortened = false;
};

Q_DECLARE_METATYPE(PathComponent)


struct DirectoryWidget::Private
{
    QLabel* title = nullptr;
    DirectoryView* view = nullptr;
    DirectoryStatusBar* statusBar = nullptr;

    void setTitle(const QDir& dir)
    {
        QVariantList resultComponents;

        for (const auto& entry : dir.absolutePath().split(dir.separator()))
            resultComponents << entry;

        const auto home = QDir::home();
        const auto homeComponents = home.absolutePath().split(home.separator());

        if (homeComponents.count() < resultComponents.count()) {
            QStringList componentStrings;
            for (const auto& component : resultComponents.first(homeComponents.count()))
                componentStrings << component.toString();

            if (componentStrings == home.absolutePath().split(home.separator())) {
                resultComponents.remove(0, homeComponents.count());
                resultComponents.prepend(QVariant::fromValue(PathComponent{
                    .text = "~",
                    .isHome = true,
                }));
            }
        }

        const QFontMetrics metrics(title->font());

        const auto resultPathPlainText = [&resultComponents] -> QString {
            QStringList resultPathComponents;

            for (const auto& component : resultComponents) {
                if (component.typeId() == QMetaType::QString) {
                    resultPathComponents << component.toString();
                } else if (component.userType() == qMetaTypeId<PathComponent>()) {
                    resultPathComponents << component.value<PathComponent>().text;
                } else {
                    Q_ASSERT(false);
                }
            }

            return resultPathComponents.join("/");
        };

        const auto textRect = [this, &resultPathPlainText, &metrics] -> QRect {
            return metrics.boundingRect(
                QRect(0, 0, title->width(), title->height()),
                Qt::TextSingleLine,
                resultPathPlainText()
            );
        };

         while (textRect().width() > title->width() - 10) {
            const auto findComponentToShorten = [&resultComponents] -> int {
                int i = 0;
                int middleIndex = resultComponents.count() / 2;
                const auto component = resultComponents.at(middleIndex);

                const auto componentAlreadyShortened = [](const QVariant& component) -> bool {
                    return component.userType() == qMetaTypeId<PathComponent>()
                        && component.value<PathComponent>().isShortened;
                };

                do {
                    if (
                        componentAlreadyShortened(resultComponents.at(middleIndex + i)) &&
                        componentAlreadyShortened(resultComponents.at(middleIndex - i))
                    ) {
                        i++;
                        continue;
                    }

                    if (componentAlreadyShortened(resultComponents.at(middleIndex + i)))
                        return middleIndex - i;

                    return middleIndex + i;
                } while (middleIndex + i < resultComponents.count() && middleIndex - i >= 0);

                return -1;
            };

            const int i = findComponentToShorten();
            if (i < 0 || i >= resultComponents.count()-1)
                break;

            const auto componentToShorten = resultComponents.at(i);

            if (componentToShorten.typeId() == QMetaType::QString) {
                resultComponents.replace(i, QVariant::fromValue(PathComponent{
                    .text = componentToShorten.toString().first(1),
                    .isShortened = true,
                }));
            } else if (componentToShorten.userType() == qMetaTypeId<PathComponent>()) {
                auto pathComponent = componentToShorten.value<PathComponent>();
                pathComponent.isShortened = true;
                resultComponents.replace(i, QVariant::fromValue(pathComponent));
            } else {
                Q_ASSERT(false);
            }
        }

        QStringList resultPathComponents;

        for (const auto& component : resultComponents) {
            if (component.typeId() == QMetaType::QString) {
                resultPathComponents << component.toString();
            } else if (component.userType() == qMetaTypeId<PathComponent>()) {
                const auto pathComponent = component.value<PathComponent>();

                if (pathComponent.isHome) {
                    resultPathComponents << QString("<span style=\"color:blue;font-weight:900;\">%1</span>")
                        .arg(pathComponent.text);
                } else if (pathComponent.isShortened) {
                    resultPathComponents << QString("<span style=\"color:grey;font-weight:100;\">%1</span>")
                        .arg(pathComponent.text);
                }
            } else {
                Q_ASSERT(false);
            }
        }

        title->setText(
            resultPathComponents.join("<font color='blue'>/</font>")
        );
    }
};

DirectoryWidget::DirectoryWidget(QWidget* parent)
    : QWidget(parent)
    , d(new Private{
        .title = new QLabel("", this),
        .view = new DirectoryView(this),
        .statusBar = new DirectoryStatusBar(this),
    })
{
    d->title->setTextFormat(Qt::MarkdownText);
    d->title->setContentsMargins(4, 0, 0, 0);

    auto layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->addWidget(d->title);
    layout->addWidget(d->view);
    layout->addWidget(d->statusBar);
    layout->setContentsMargins(0, 5, 0, 0);
    setLayout(layout);

    connect(d->view, &DirectoryView::directoryChanged, [this](const QDir& directory) {
        d->setTitle(directory);
    });
    d->setTitle(d->view->directory());

    connect(
        d->view->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        [this] {
            const int selectedFilesCount = selectedFiles().count();

            if (selectedFilesCount > 0) {
                d->statusBar->showMessage(
                    QString("Selected %1 file(s) of %2")
                        .arg(selectedFilesCount)
                        .arg(filesCount())
                );
            } else {
                d->statusBar->clearMessage();
            }
        }
    );
}

DirectoryWidget::~DirectoryWidget() = default;

DirectoryView* DirectoryWidget::view() const
{
    return d->view;
}

void DirectoryWidget::toggleShowHiddenFiles()
{
    const bool newState = !d->view->hiddenFilesVisible();

    d->view->setShowHiddenFiles(newState);
    d->statusBar->setHiddenFilesVisible(newState);
}

QList<QFileInfo> DirectoryWidget::selectedFiles() const
{
    QList<QFileInfo> files;

    for (const auto& index : view()->selectionModel()->selectedRows()) {
        if (index.data(Qt::UserRole + 1).toBool())
            continue;

        files.append(index.data(Qt::UserRole).value<QFileInfo>());
    }

    return files;
}

std::optional<QFileInfo> DirectoryWidget::currentFile() const
{
    const auto currentIndex = view()->currentIndex();

    if (!currentIndex.isValid() || currentIndex.data(Qt::UserRole + 1).toBool())
        return {};

    return currentIndex.data(Qt::UserRole).value<QFileInfo>();
}

int DirectoryWidget::filesCount() const
{
    // Non-root directories have [..].
    return view()->model()->rowCount() - (view()->directory().isRoot() ? 0 : 1);
}

void DirectoryWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    d->setTitle(d->view->directory());
}
