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
    //TODO: problème getNbClient retourne le nombre de client en attente et non le total à gérer.
    //comment savoir s'il en reste ???
    while (_salon->isInService() || _salon->getNbClient() > 0) {
        if (_salon->getNbClient() > 0) {
            _salon->pickNextClient();
        } else {
            _salon->goToSleep();

            //Si on est réveillé et que le salon fermé et en plus qu'il n'y a pas de client à gérer
            //alors on peut s'arrêter tout de suite
            if (!_salon->isInService() && _salon->getNbClient() == 0) break;
        }
        //A cette étape, il y a forcèment un ou des clients en attente
        _salon->waitClientAtChair();
        _salon->beautifyClient();
        _interface->consoleAppendTextBarber("Coupé les cheveux à un client de plus...");
    }
    // _salon->goToSleep();	//TODO:est-ce qu'on veut animer le barbier comme s'il allait s'endormir avant de terminer son thread ?? (sans faire de wait attention)
    _interface->consoleAppendTextBarber("La journée est terminée, à demain !");
}
