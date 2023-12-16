/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Modifications à faire dans le fichier

#include "pcosalon.h"

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcothread.h>

#include <iostream>

PcoSalon::PcoSalon(GraphicSalonInterface *interface, unsigned int capacity)
    : _interface(interface), capacity(capacity) {

	//Init chairs with pointers on PcoConditionVariable objects
	chairs.reserve(capacity);
    for (unsigned i = 0; i < capacity; i++) {
        chairs.push_back(std::make_unique<PcoConditionVariable>());
    }
}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
unsigned int PcoSalon::getNbClient() {
    return nbClients;
}

bool PcoSalon::accessSalon(unsigned clientId) {
    _mutex.lock();
    if (nbClients == capacity) return false;
    animationClientAccessEntrance(clientId);

    if (barberAwake) {
        animationClientSitOnChair(clientId, freeChairIndex);
        auto &chairToUse = chairs.at(freeChairIndex);
        freeChairIndex = freeChairIndex + 1 % capacity;
        chairToUse->wait(&_mutex);
    } else {
        animationWakeUpBarber();
        barberAwake = true;
    }

    _mutex.unlock();
    return true;
}


void PcoSalon::goForHairCut(unsigned clientId) {
    _mutex.lock();
    animationClientSitOnWorkChair(clientId);
    workChairFree = false;
    _mutex.unlock();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId) {
    // TODO
    _mutex.lock();
    _mutex.unlock();
}


void PcoSalon::walkAround(unsigned clientId) {
    // TODO
    _mutex.lock();
    _mutex.unlock();
}


void PcoSalon::goHome(unsigned clientId) {
    // TODO
    _mutex.lock();
    _mutex.unlock();
}


/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
void PcoSalon::goToSleep() {
    // TODO
    _mutex.lock();
    _mutex.unlock();
}


void PcoSalon::pickNextClient() {
    // TODO
    _mutex.lock();
    // chairs.at(nextClientChairIndex).notifyOne();
    _mutex.unlock();
}


void PcoSalon::waitClientAtChair() {
    // TODO
    _mutex.lock();
    _mutex.unlock();
}


void PcoSalon::beautifyClient() {
    // TODO
    _mutex.lock();
    _mutex.unlock();
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService() {
    // TODO
    return true;
}


void PcoSalon::endService() {
    // TODO
    _mutex.lock();
    _mutex.unlock();
}

/********************************************
 *   Méthodes privées pour les animations   *
 *******************************************/
void PcoSalon::animationClientAccessEntrance(unsigned clientId) {
    _mutex.unlock();
    _interface->clientAccessEntrance(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnChair(unsigned clientId, unsigned clientSitNb) {
    _mutex.unlock();
    _interface->clientSitOnChair(clientId, clientSitNb);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnWorkChair(unsigned clientId) {
    _mutex.unlock();
    _interface->clientSitOnWorkChair(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientWaitForHairToGrow(unsigned clientId) {
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, true);
    _mutex.lock();
}

void PcoSalon::animationClientWalkAround(unsigned clientId) {
    _mutex.unlock();
    _interface->clientWalkAround(clientId);
    _mutex.lock();
}

void PcoSalon::animationBarberGoToSleep() {
    _mutex.unlock();
    _interface->barberGoToSleep();
    _mutex.lock();
}

void PcoSalon::animationWakeUpBarber() {
    _mutex.unlock();
    _interface->clientWakeUpBarber();
    _mutex.lock();
}

void PcoSalon::animationBarberCuttingHair() {
    _mutex.unlock();
    _interface->barberCuttingHair();
    _mutex.lock();
}

void PcoSalon::animationClientGoHome(unsigned clientId) {
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, false);
    _mutex.lock();
}
