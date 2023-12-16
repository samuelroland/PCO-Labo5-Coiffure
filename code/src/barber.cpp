/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Modifications à faire dans le fichier

#include "barber.h"
#include <unistd.h>

#include <iostream>

Barber::Barber(GraphicSalonInterface *interface, std::shared_ptr<SalonBarberInterface> salon)
    : _interface(interface), _salon(salon) {
    _interface->consoleAppendTextBarber("Salut, prêt à travailler !");
}

void Barber::run() {
    while (true) {
        if (_salon->getNbClient() > 0) {
            _salon->pickNextClient();
        } else {
            _salon->goToSleep();
        }
        _salon->waitClientAtChair();
        _salon->beautifyClient();
        _interface->consoleAppendTextBarber("Coupé les cheveux à un client de plus...");
    }

    _interface->consoleAppendTextBarber("La journée est terminée, à demain !");
}
