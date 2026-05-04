#include "mainwindow.h"
#include "surveydata.h"

#include <QApplication>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFrame>
#include <QSizePolicy>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_questions = defaultQuestions();
    setWindowTitle("Опитування: Смаки та їжа");
    setMinimumSize(680, 560);
    resize(760, 720);
    buildUi();
    applyStyles();
}

void MainWindow::buildUi()
{
    // ── Central scroll area ──────────────────────────────────────────────
    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    setCentralWidget(scrollArea);

    auto *container = new QWidget;
    scrollArea->setWidget(container);

    auto *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(0);

    // ── Header ───────────────────────────────────────────────────────────
    auto *header = new QFrame;
    header->setObjectName("header");
    auto *headerLayout = new QVBoxLayout(header);
    headerLayout->setContentsMargins(24, 20, 24, 20);
    headerLayout->setSpacing(6);

    auto *emoji = new QLabel("🍽️");
    emoji->setObjectName("emoji");
    emoji->setAlignment(Qt::AlignCenter);

    auto *title = new QLabel("Смаки та їжа");
    title->setObjectName("title");
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel("Розкажіть нам про свої гастрономічні вподобання.\n"
                                "Відповіді будуть збережені у файл на вашому комп'ютері.");
    subtitle->setObjectName("subtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);

    headerLayout->addWidget(emoji);
    headerLayout->addWidget(title);
    headerLayout->addWidget(subtitle);
    mainLayout->addWidget(header);
    mainLayout->addSpacing(20);

    // ── Progress bar ─────────────────────────────────────────────────────
    m_progressBar = new QProgressBar;
    m_progressBar->setObjectName("progressBar");
    m_progressBar->setRange(0, m_questions.size());
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(6);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addSpacing(16);

    // ── Questions ────────────────────────────────────────────────────────
    for (const auto &q : m_questions) {
        auto *card = new QFrame;
        card->setObjectName("card");

        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(20, 16, 20, 16);
        cardLayout->setSpacing(8);

        auto *label = new QLabel(q.text);
        label->setObjectName("questionLabel");
        label->setWordWrap(true);

        auto *input = new QTextEdit;
        input->setObjectName("answerInput");
        input->setPlaceholderText(q.placeholder);
        input->setFixedHeight(72);
        input->setAcceptRichText(false);

        connect(input, &QTextEdit::textChanged, this, [this]() {
            int filled = 0;
            for (auto *ed : m_inputs)
                if (!ed->toPlainText().trimmed().isEmpty()) ++filled;
            m_progressBar->setValue(filled);
        });

        cardLayout->addWidget(label);
        cardLayout->addWidget(input);
        mainLayout->addWidget(card);
        mainLayout->addSpacing(10);

        m_inputs.append(input);
    }

    mainLayout->addSpacing(10);

    // ── Status label ──────────────────────────────────────────────────────
    m_statusLabel = new QLabel;
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->hide();
    mainLayout->addWidget(m_statusLabel);

    // ── Buttons ───────────────────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout;
    btnRow->setSpacing(12);

    m_clearBtn = new QPushButton("Очистити форму");
    m_clearBtn->setObjectName("clearBtn");
    m_clearBtn->setCursor(Qt::PointingHandCursor);

    m_submitBtn = new QPushButton("Зберегти відповіді");
    m_submitBtn->setObjectName("submitBtn");
    m_submitBtn->setCursor(Qt::PointingHandCursor);
    m_submitBtn->setDefault(true);

    btnRow->addWidget(m_clearBtn);
    btnRow->addStretch();
    btnRow->addWidget(m_submitBtn);
    mainLayout->addLayout(btnRow);

    connect(m_submitBtn, &QPushButton::clicked, this, &MainWindow::onSubmit);
    connect(m_clearBtn,  &QPushButton::clicked, this, &MainWindow::onClear);
}

void MainWindow::applyStyles()
{
    setStyleSheet(R"(
        QScrollArea, QWidget {
            background-color: #fdf6ee;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        #header {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #e8622a, stop:1 #c0392b);
            border-radius: 14px;
        }

        #emoji {
            font-size: 36px;
            color: #1a1a1a;
        }

        #title {
            font-size: 24px;
            font-weight: 700;
            color: #1a1a1a;
            letter-spacing: 1px;
        }

        #subtitle {
            font-size: 13px;
            color: rgba(0,0,0,0.72);
        }

        #progressBar {
            background: #f0d9c8;
            border-radius: 4px;
            border: none;
        }
        #progressBar::chunk {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #e8622a, stop:1 #f0a500);
            border-radius: 4px;
        }

        #card {
            background: #ffffff;
            border-radius: 12px;
            border: 1px solid #f0ddd0;
        }
        #card:hover {
            border: 1px solid #e8622a;
            background: #fffaf7;
        }

        #questionLabel {
            font-size: 14px;
            font-weight: 600;
            color: #3d1f0d;
        }

        #answerInput {
            border: 1.5px solid #f0ddd0;
            border-radius: 8px;
            padding: 6px 10px;
            font-size: 13px;
            color: #3d1f0d;
            background: #fffaf7;
        }
        #answerInput:focus {
            border: 1.5px solid #e8622a;
            background: #ffffff;
        }

        #submitBtn {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #e8622a, stop:1 #f0a500);
            color: #ffffff;
            border: none;
            border-radius: 10px;
            padding: 11px 30px;
            font-size: 14px;
            font-weight: 700;
            min-width: 190px;
        }
        #submitBtn:hover  {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #cf4f1e, stop:1 #d98e00);
        }
        #submitBtn:pressed {
            background: #b8401a;
        }

        #clearBtn {
            background: transparent;
            color: #a0614a;
            border: 1.5px solid #f0c8b0;
            border-radius: 10px;
            padding: 11px 22px;
            font-size: 14px;
            min-width: 150px;
        }
        #clearBtn:hover  { background: #fdf0e8; color: #3d1f0d; }
        #clearBtn:pressed { background: #f5e0d0; }

        #statusLabel {
            font-size: 13px;
            padding: 10px 14px;
            border-radius: 8px;
        }
    )");
}

bool MainWindow::validateAnswers()
{
    for (int i = 0; i < m_inputs.size(); ++i) {
        if (m_inputs[i]->toPlainText().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Незаповнене поле",
                QString("Будь ласка, дайте відповідь на питання %1.").arg(i + 1));
            m_inputs[i]->setFocus();
            return false;
        }
    }
    return true;
}

bool MainWindow::saveToFile(const QMap<QString, QString> &answers)
{
    QString defaultName = QString("survey_%1.txt")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));

    QString path = QFileDialog::getSaveFileName(
        this,
        "Зберегти відповіді",
        QDir::homePath() + "/" + defaultName,
        "Текстовий файл (*.txt);;Усі файли (*.*)");

    if (path.isEmpty()) return false;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Помилка",
            "Не вдалося відкрити файл для запису:\n" + file.errorString());
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << "=== РЕЗУЛЬТАТИ ОПИТУВАННЯ ===\n";
    out << "Дата та час: "
        << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") << "\n";
    out << QString(40, '=') << "\n\n";

    for (int i = 0; i < m_questions.size(); ++i) {
        out << m_questions[i].text << "\n";
        out << "Відповідь: " << answers.value(m_questions[i].id) << "\n\n";
    }

    file.close();
    showResult(path);
    return true;
}

void MainWindow::showResult(const QString &filePath)
{
    m_statusLabel->setStyleSheet(
        "background:#e8f5e9; color:#2e7d32; border:1px solid #a5d6a7;");
    m_statusLabel->setText("Відповіді успішно збережено у файл:\n" + filePath);
    m_statusLabel->show();

    auto *msgBox = new QMessageBox(this);
    msgBox->setWindowTitle("Збережено");
    msgBox->setIcon(QMessageBox::Information);
    msgBox->setText("<b>Відповіді успішно збережено!</b>");
    msgBox->setInformativeText("Файл: " + filePath);
    msgBox->addButton("Відкрити файл", QMessageBox::AcceptRole);
    msgBox->addButton("Закрити",       QMessageBox::RejectRole);

    if (msgBox->exec() == 0)
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void MainWindow::onSubmit()
{
    if (!validateAnswers()) return;

    QMap<QString, QString> answers;
    for (int i = 0; i < m_questions.size(); ++i)
        answers[m_questions[i].id] = m_inputs[i]->toPlainText().trimmed();

    saveToFile(answers);
}

void MainWindow::onClear()
{
    auto reply = QMessageBox::question(this, "Очистити форму",
        "Ви дійсно хочете очистити всі відповіді?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        for (auto *ed : m_inputs) ed->clear();
        m_progressBar->setValue(0);
        m_statusLabel->hide();
    }
}
