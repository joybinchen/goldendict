//
// Created by joybin on 2020/3/1.
//

#ifndef GOLDENDICT_EVENT_HH
#define GOLDENDICT_EVENT_HH

#include <QObject>

namespace Config {

/// Configuration-specific events. Some parts of the program need to react
/// to specific changes in configuration. The object of this class is used
/// to emit signals when such events happen -- and the listeners connect to
/// them to be notified of them.
/// This class is separate from the main Class since QObjects can't be copied.
class Events : public QObject {
Q_OBJECT

public:

    /// Signals that the value of the mutedDictionaries has changed.
    /// This emits mutedDictionariesChanged() signal, so the subscribers will
    /// be notified.
    void signalMutedDictionariesChanged();

signals:

    /// THe value of the mutedDictionaries has changed.
    void mutedDictionariesChanged();

private:
};

}

#endif //GOLDENDICT_EVENT_HH
