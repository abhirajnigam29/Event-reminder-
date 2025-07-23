#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QLabel>
#include <QDateEdit>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QDate>

class Event {
public:
    std::string title;
    std::string description;
    std::string date;
    bool status;

    Event(std::string title, std::string description, std::string date, bool status = false)
        : title(title), description(description), date(date), status(status) {}

    std::string getTitle() const {
        return title;
    }

    std::string getDescription() const {
        return description;
    }

    std::string getDate() const {
        return date;
    }

    bool getStatus() const {
        return status;
    }

    void changeStatus(bool newStatus = true) {
        status = newStatus;
    }

    QString toQString() const {
        return QString("%1 (Date: %2, Status: %3)")
               .arg(QString::fromStdString(title))
               .arg(QString::fromStdString(date))
               .arg(status ? "Completed" : "Incomplete");
    }
};

class EventManager {
public:
    std::vector<Event> eventList;

    void addEvent(const Event& event) {
        eventList.push_back(event);
    }

    bool removeEvent(const std::string& title) {
        auto it = std::remove_if(eventList.begin(), eventList.end(),
                                 [&](const Event& e) { return e.getTitle() == title; });

        if (it != eventList.end()) {
            eventList.erase(it, eventList.end());
            return true;
        } else {
            return false;
        }
    }

    bool markEventAsDone(const std::string& title) {
        for (auto& event : eventList) {
            if (event.getTitle() == title) {
                event.changeStatus(true);
                return true;
            }
        }
        return false;
    }
};

class EventAppWindow : public QMainWindow {
    Q_OBJECT

public:
    EventAppWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        eventManager = new EventManager();
        setupUI();
        loadEventsIntoList();
    }

    ~EventAppWindow() {
        delete eventManager;
    }

private slots:
    void onAddEventClicked() {
        QString title = titleInput->text();
        QString description = descriptionInput->text();
        QString date = dateInput->date().toString("yyyy/MM/dd");

        if (title.isEmpty() || description.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Title and Description cannot be empty.");
            return;
        }

        eventManager->addEvent(Event(title.toStdString(), description.toStdString(), date.toStdString()));
        QMessageBox::information(this, "Success", "Event added successfully!");
        clearInputFields();
        loadEventsIntoList();
    }

    void onRemoveEventClicked() {
        QString title = removeTitleInput->text();

        if (title.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please enter a title to remove.");
            return;
        }

        if (eventManager->removeEvent(title.toStdString())) {
            QMessageBox::information(this, "Success", "Event removed successfully!");
            removeTitleInput->clear();
            loadEventsIntoList();
        } else {
            QMessageBox::warning(this, "Error", "Event not found.");
        }
    }

    void onMarkEventDoneClicked() {
        QString title = markDoneTitleInput->text();

        if (title.isEmpty()) {
            QMessageBox::warning(this, "Input Error", "Please enter a title to mark as done.");
            return;
        }

        if (eventManager->markEventAsDone(title.toStdString())) {
            QMessageBox::information(this, "Success", "Event marked as completed!");
            markDoneTitleInput->clear();
            loadEventsIntoList();
        } else {
            QMessageBox::warning(this, "Error", "Event not found.");
        }
    }

    void onViewEventsClicked() {
        loadEventsIntoList();
        if (eventListWidget->count() == 0) {
            QMessageBox::information(this, "No Events", "No events to display.");
        }
    }

private:
    EventManager* eventManager;

    QLineEdit *titleInput;
    QLineEdit *descriptionInput;
    QDateEdit *dateInput;
    QPushButton *addEventButton;

    QLineEdit *removeTitleInput;
    QPushButton *removeEventButton;

    QLineEdit *markDoneTitleInput;
    QPushButton *markDoneButton;

    QListWidget *eventListWidget;
    QPushButton *viewEventsButton;

    void setupUI() {
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        mainLayout->addWidget(new QLabel("--- Add New Event ---"));
        QHBoxLayout *addLayout = new QHBoxLayout();
        titleInput = new QLineEdit();
        titleInput->setPlaceholderText("Event Title");
        descriptionInput = new QLineEdit();
        descriptionInput->setPlaceholderText("Event Description");
        dateInput = new QDateEdit();
        dateInput->setCalendarPopup(true);
        dateInput->setDate(QDate::currentDate());

        addEventButton = new QPushButton("Add Event");
        connect(addEventButton, &QPushButton::clicked, this, &EventAppWindow::onAddEventClicked);

        addLayout->addWidget(new QLabel("Title:"));
        addLayout->addWidget(titleInput);
        addLayout->addWidget(new QLabel("Description:"));
        addLayout->addWidget(descriptionInput);
        addLayout->addWidget(new QLabel("Date:"));
        addLayout->addWidget(dateInput);
        addLayout->addWidget(addEventButton);
        mainLayout->addLayout(addLayout);

        mainLayout->addWidget(new QLabel("\n--- Remove Event ---"));
        QHBoxLayout *removeLayout = new QHBoxLayout();
        removeTitleInput = new QLineEdit();
        removeTitleInput->setPlaceholderText("Title to Remove");
        removeEventButton = new QPushButton("Remove Event");
        connect(removeEventButton, &QPushButton::clicked, this, &EventAppWindow::onRemoveEventClicked);
        removeLayout->addWidget(new QLabel("Title:"));
        removeLayout->addWidget(removeTitleInput);
        removeLayout->addWidget(removeEventButton);
        mainLayout->addLayout(removeLayout);

        mainLayout->addWidget(new QLabel("\n--- Mark Event as Done ---"));
        QHBoxLayout *markDoneLayout = new QHBoxLayout();
        markDoneTitleInput = new QLineEdit();
        markDoneTitleInput->setPlaceholderText("Title to Mark Done");
        markDoneButton = new QPushButton("Mark as Done");
        connect(markDoneButton, &QPushButton::clicked, this, &EventAppWindow::onMarkEventDoneClicked);
        markDoneLayout->addWidget(new QLabel("Title:"));
        markDoneLayout->addWidget(markDoneTitleInput);
        markDoneLayout->addWidget(markDoneButton);
        mainLayout->addLayout(markDoneLayout);

        mainLayout->addWidget(new QLabel("\n--- All Events ---"));
        eventListWidget = new QListWidget();
        eventListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        viewEventsButton = new QPushButton("Refresh Events List");
        connect(viewEventsButton, &QPushButton::clicked, this, &EventAppWindow::onViewEventsClicked);
        mainLayout->addWidget(eventListWidget);
        mainLayout->addWidget(viewEventsButton);

        setWindowTitle("Event Management System");
        resize(800, 600);
    }

    void loadEventsIntoList() {
        eventListWidget->clear();
        if (eventManager->eventList.empty()) {
            eventListWidget->addItem("No events for viewing.");
        } else {
            for (const auto& event : eventManager->eventList) {
                eventListWidget->addItem(event.toQString());
            }
        }
    }

    void clearInputFields() {
        titleInput->clear();
        descriptionInput->clear();
        dateInput->setDate(QDate::currentDate());
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    EventAppWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
