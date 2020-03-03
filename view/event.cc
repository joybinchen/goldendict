//
// Created by joybin on 2020/3/1.
//

#include "event.hh"

void Config::Events::signalMutedDictionariesChanged()
{
    emit mutedDictionariesChanged();
}

