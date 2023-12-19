# Labo 5 - Modélisation par moniteur de Mesa

Auteur·ices: Vitória Oliveira et Samuel Roland

## Description des fonctionnalités du logiciel

Le but de ce laboratoire est de modéliser un salon de barbier et guider correctement les clients, le barbier et la fermeture du salon à l'aide d'un moniteur de Mesa.

## Choix d'implémentation

Pour gérer l'attente des clients du barbier aux différentes étapes, nous avons utilisés des variables de condition (`PcoConditionVariable`), voici leur but et utilisation:

- `barberSleeping` : Elle nous permet de bloquer le thread du barbier quand il dort, une fois l'animation de sortie vers sa chambre, dans la méthode `goToSleep()`. Lorsqu'un client arrive dans le salon et que le barbier dort (`barberAwake` à `false`), le client réveille le barbier avec `barberSleeping.notifyOne()` (il n'y a qu'un seul barbier et il n'est pas demandé d'être évolutif et d'en supporter plus).  

- `barberWaiting` : permet au barbier de ne pas commencer son travail alors que le client n'est pas encore sur la chaise de travail. Le wait se fait par le barbier dans `pickNextClient()`. Le notify se fait par le client qui arrive sur la chaise de travail dans `goForHairCut()` une fois l'animation est terminée.  

- `clientCutWaiting` :  dès que le client est sur la chaise de travail dans `goForHairCut()`, il attend que le barbier sa coupe. Le barbier notifie le client lorsque la coupe est terminée dans `beautifyClient()` après l'animation liée.

- `chairs` : ce vecteur de variables de condition permet de faire attendre et appeler les clients dans la salle d'attente du salon. Afin de l'utiliser comme un buffer circulaire, nous gérons deux variables `freeChairIndex` et `nextClientChairIndex` initialisées à 0 qui désignent respectivement l'index d'écriture et de lecture dans ce buffer. C'est à dire l'index de chaise où nous plaçons le client qui vient d'arriver et du prochain qui sera appelé par le barbier, respectant ainsi l'ordre d'arrivée des clients. Les clients vont donc toujours se poser dans l'ordre des index des chaises (0,1,2,3,0,1,2,... pour 3 chaises par ex.). Ces index sont incrémentés de 1 et modulo le nombre de chaises afin de rester dans le range du vecteur. Nous n'avons pas besoin de savoir quelles chaises sont occupées et par quels clients, leur nombre (`nbWaitingClients`) et ces 2 index suffisent.  
Chaque client nécessitant d'attendre dans le salon effectue un wait sur la prochaine chaise disponible `freeChairIndex` dans `accessSalon()`. Le client est débloqué dans `pickNextClient()` par le barbier, lorsque celui-ci notifie un client sur la chaise à l'index `nextClientChairIndex`. 


En plus, nous avons les variables suivantes:
- `bool barberAwake` : indique si le barbier est réveillé.
- `bool isOpen` : indique si le salon est en service. 
- `std::vector<bool> chairsUsed` : vecteur de boolean qui stocke l'état d'occupation des chaises en salle d'attente. 
- `unsigned nbWaitingClients` : compte le nombre de client en salle d'attente. 
- `unsigned nbUncutClients` : compte le nombre de clients dont la coupe n'a pas encore été faite.
- `_mutex` : ce mutex fourni est utilisé dans toutes les méthodes (lock au début et unlock à la fin) afin de garantir qu'il n'y a qu'un seul thread à la fois dans le moniteur de Mesa. 

## Fin de service

La méthode `endService()` met la variable `isOpen` à faux et notifie le barbier sur `barberSleeping` lorsque celui-ci dort afin que la fin de son thread puisse être gérée correctement. 


### Animations


## Tests effectués

### Test de bon fonctionnement général

| Vérification | Status |
|---|---|
| Premier client réveille le barbier | **OK** |
| Client qui réveille  le barbier attend que celui-ci arrive à la chaise de travail | **OK** |
| Client va sur les chaises de la salle d'attente si la chaise du barbier est occupée | **OK** |
| Nombre de clients en attente ne dépasse pas la capacité des chaises disponible | **OK** |
| Clients traités dans l'ordre| **OK** |
| Pas de chevauchement entre les clients| **OK** |
| Client attend chaise libre pour aller sur la chaise du barbier | **OK** |
| Barbier attend que le client soit sur la chaise pour couper les cheveux | **OK** |
| Client attend la fin de la coupe pour partir | **OK** |

### Tests de bon fonctionnement selon nombre de clients et nombre de chaises dans la salle d'attente

| Nombre de clients | Nombre de chaises | Status | 
| --- | --- | --- |
| 0 | 2 | **OK** | 
| 10 | 1 | **OK** |
| 20 | 20 | **OK** | 
| 30 | 2 | **OK** |

Lorsqu'il n'y a pas de chaises en salle d'attente, aucun client n'entre dans le salon et le barbier va se coucher. 

### Test de terminaison 

| Vérification | Status |
|---|---|
| Clients dans le salon sont pris en charge avant la terminaison | **OK** |
| Toutes les threads se terminent | **OK** |
| Thread du barbier est la dernière terminer | **OK** |

### Tests de terminaison spécifiques

| Test | Comportement | Status | 
| --- | --- | --- |
| Terminaison lorsque le barbier dort | Le barbier se réveille brièvement pour que son thread puissent se terminer. Les clients en attente dehors du salon partent progressivement. Les threads sont terminées correctement. | **OK** |
| Terminaison lorsque pas de clients dans le salon | Le barbier va dormir. Il se réveille brièvement pour que son thread puissent se terminer. Les clients en attente dehors du salon partent progressivement. Les threads sont terminées correctement.| **OK** |
| Terminaison lorsque le dernier client est sur la chaise de travail et pas de clients en attente | Le barbier traite le dernier client puis il ferme le salon. Les threads sont terminées correctement. | **OK** |
| Terminaison lorsque barbier est entre deux clients  | Le barbier traite le dernier client puis il ferme le salon. Les threads sont terminées correctement. | **OK** |

