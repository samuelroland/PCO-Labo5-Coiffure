/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
// Modifications à faire dans le fichier

#include "pcosalon.h"
#include "utils/display.h"

#include <pcosynchro/pcoconditionvariable.h>
#include <pcosynchro/pcomutex.h>
#include <pcosynchro/pcothread.h>

#include <iostream>
#include <string>

PcoSalon::PcoSalon(GraphicSalonInterface *interface, unsigned int capacity)
    : _interface(interface), capacity(capacity) {
    //Initialiser les chaises en tant que liste de PcoConditionVariable, chacune servant à faire attendre ou libérer une client
    chairs.reserve(capacity);
    chairsUsed.resize(capacity, false);
    for (unsigned i = 0; i < capacity; i++) {
        chairs.push_back(std::make_unique<PcoConditionVariable>());
    }
}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
unsigned int PcoSalon::getNbClient() {
    _mutex.lock();
    auto local = nbUncutClients;
    _mutex.unlock();
    return local;
}

bool PcoSalon::accessSalon(unsigned clientId) {
    _mutex.lock();
    _interface->consoleAppendTextClient(clientId, "Tentons d'entrer dans le salon...");
    animationClientAccessEntrance(clientId);
    //Bloquer l'entrée au salon s'il est fermé ou qu'il n'y a pas assez de place en salle d'attente
    if (!isOpen || nbWaitingClients >= capacity) {
        _mutex.unlock();
        _interface->consoleAppendTextClient(clientId, "Pas possible de rentrer...");
        return false;
    }

    auto clientNumero = ++nbUncutClients;
    ++nbWaitingClients;
    //Si le barbier est réveillé on s'assied en salle d'attente
    if (barberAwake) {
        _interface->consoleAppendTextClient(clientId, QString("Barbier réveillé: Je me pose sur la chaise %1, je suis client n %2").arg(freeChairIndex).arg(clientId));
        auto &chairToUse = chairs.at(freeChairIndex);
        auto localChairIndex = freeChairIndex;
        freeChairIndex = (freeChairIndex + 1) % capacity;
        animationClientSitOnChair(clientId, localChairIndex);

        _interface->consoleAppendTextClient(clientId, QString("pose sur la chaise"));

        chairsUsed.at(localChairIndex) = true;//le barbier peut ainsi savoir qu'on est arrivé sur cette chaise
        clientSitWaiting.notifyOne();         //on notifie le barbier qu'un nouveau client s'est assis (qu'il puisse checker si c'est le prochain client)
        chairToUse->wait(&_mutex);            //on attend que le barbier nous signale quand c'est notre tour

        chairsUsed.at(localChairIndex) = false;//la chaise n'est plus occupée
        _interface->consoleAppendTextClient(clientId, QString("leve de la chaise"));
        nbWaitingClients--;//la chaise est libre le client n'est plus en attente
    } else {
        //Sinon on réveille le barbier et on y va direct
        _interface->consoleAppendTextClient(clientId, QString("Barbier endormi: Je le réveille et vais directement sur la working chair"));

        //On réveille le barbier ici, parce que le prochain client ne doit pas faire de même et passer tout droit sans attendre
        barberAwake = true;
        barberSleeping.notifyOne();
        //On décrémente nbWaitingClients car le client n'attend pas, 2 autres clients peuvent rentrer
        nbWaitingClients--;
        barberReady.wait(&_mutex);
    }

    _mutex.unlock();
    return true;
}


void PcoSalon::goForHairCut(unsigned clientId) {
    _mutex.lock();
    animationClientSitOnWorkChair(clientId);
    _interface->consoleAppendTextClient(clientId, "Arrivé sur la working chair");
    workChairFree = false;
    barberWaiting.notifyOne();
    clientCutWaiting.wait(&_mutex);

    workChairFree = true;
    _interface->consoleAppendTextClient(clientId, "La coupe est terminée");
    //Quitter le salon
    _mutex.unlock();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId) {
    _mutex.lock();
    animationClientWaitForHairToGrow(clientId);//animation de sortie du salon
    _mutex.unlock();
}


void PcoSalon::walkAround(unsigned clientId) {
    _mutex.lock();
    animationClientWalkAround(clientId);
    _mutex.unlock();
}


void PcoSalon::goHome(unsigned clientId) {
    _mutex.lock();
    _interface->consoleAppendTextClient(clientId, QString("Je vais à la maison"));
    animationClientGoHome(clientId);
    _interface->consoleAppendTextClient(clientId, QString("Je suis arrivé à la maison"));
    _mutex.unlock();
}


/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
void PcoSalon::goToSleep() {
    _mutex.lock();
    _interface->consoleAppendTextBarber("Je vais au lit");
    barberAwake = false;
    animationBarberGoToSleep();
    _interface->consoleAppendTextBarber("Je m'endors");
    barberSleeping.wait(&_mutex);
    _interface->consoleAppendTextBarber("Je me réveille");

    //Animer le réveil du barbier seulement si le salon ne doit pas fermer
    if (isInService() || nbUncutClients > 0) {
        _interface->consoleAppendTextBarber("Je me déplace vers la chaise");
        animationWakeUpBarber();

        //Le barbier est réveillé par le premier client qui entre
        //et qui voit qu'il dort. barberAwake est déjà true.
    }
    _mutex.unlock();
}


void PcoSalon::pickNextClient() {
    _mutex.lock();
    _interface->consoleAppendTextBarber("Prenons le client suivant");
    //TODO: vraiment utile ?
    // if (nbWaitingClients == 0) {
    //     _mutex.unlock();
    //     return;
    // }
    _interface->consoleAppendTextBarber(QString("Appeler le client en chaise %1 ").arg(nextClientChairIndex));

    //Tant que le chaise du prochain client n'est pas occupée, on attend que le client s'assoit dessus
    while (chairsUsed.at(nextClientChairIndex) == false) {
        _interface->consoleAppendTextBarber(QString("Attente sur un client en route vers sa chaise"));
        clientSitWaiting.wait(&_mutex);
        _interface->consoleAppendTextBarber(QString("Un nouveau client assis, est-ce que c'est le suivant ?"));
    }
    _interface->consoleAppendTextBarber(QString("Client assis trouvé et libéré"));
    chairs.at(nextClientChairIndex)->notifyOne();
    nextClientChairIndex = (nextClientChairIndex + 1) % capacity;

    _mutex.unlock();
}


void PcoSalon::waitClientAtChair() {
    _mutex.lock();
    barberReady.notifyOne();
    _interface->consoleAppendTextBarber("Attendons le client appelé");
    //On attend que le client arrive à la chaise de travail
    //on est sûr qu'il n'arrivera pas avant nous car il ne peut venir que si on est devant la chaise
    barberWaiting.wait(&_mutex);
    _mutex.unlock();
}


void PcoSalon::beautifyClient() {
    _mutex.lock();
    _interface->consoleAppendTextBarber("Faisons lui une belle coupe.");
    animationBarberCuttingHair();
    nbUncutClients--;
    clientCutWaiting.notifyOne();//notifier le client sur la working chair qu'on a terminé la coupe
    _interface->consoleAppendTextBarber("Coupe terminée");
    _mutex.unlock();
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService() {
    return isOpen;
}


void PcoSalon::endService() {
    _mutex.lock();
    isOpen = false;
    _interface->consoleAppendTextBarber("Le salon doit fermer, c'est la fin de service.");
    //Si le barbier dort, il faut le réveiller pour que son thread puisse se terminer
    if (!barberAwake) {
        barberSleeping.notifyOne();
    }
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
