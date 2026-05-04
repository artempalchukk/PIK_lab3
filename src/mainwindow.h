#pragma once
#include <QMainWindow>
#include <QVector>
#include <QMap>
#include "surveydata.h"

class QLabel;
class QTextEdit;
class QPushButton;
class QScrollArea;
class QWidget;
class QVBoxLayout;
class QProgressBar;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onSubmit();
    void onClear();

private:
    void buildUi();
    void applyStyles();
    bool validateAnswers();
    bool saveToFile(const QMap<QString, QString> &answers);
    void showResult(const QString &filePath);

    QVector<Question>          m_questions;
    QVector<QTextEdit *>       m_inputs;
    QPushButton               *m_submitBtn;
    QPushButton               *m_clearBtn;
    QProgressBar              *m_progressBar;
    QLabel                    *m_statusLabel;
};
