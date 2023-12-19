# Labo 5 - Modélisation par moniteur de Mesa

Auteur·ices: Vitória Oliveira et Samuel Roland

## Description des fonctionnalités du logiciel
Le but de ce laboratoire est de modéliser une boutique de barbier et gérer les interactions entre les clients, le barbier et le salon à l'aide d'un moniteur de Mesa. 

__TODO__ Expliquer fonctionnement des clients? Barbier? Salon? Ou c'est trop car c'est un peu comme expliquer la machine d'états/dans la consigne du labo? Est-ce qu'on fait aussi un diagramme car ca facilite la compréhension?

## Choix d'implémentation

Des variables de condition ont été employées pour synchroniser les actions des clients et du barbier, qui permet que ce dernier puisse se reposer lorsqu'il n'y a pas de clients et assurant un accès contrôlé aux chaises dans la salle d'attente et à la chaise de travail du barbier. 

- `barberSleeping` : lorsque le barbier va dormir (__TODO__ citer les cas où celui-ci va dormir?), il attend sur cette variable jusqu'à ce que quelqu'un vienne le réveiller. Lorsqu'un client arrive dans le salon et que le barbier dort (boolean `barberAwake` à faux), le client notifie le barbier. 
Le barbier peut aussi être notifié lorsque nous souhaitons finir le programme en appelant la méthode `endService()` afin que la fin de son thread puisse être gérée correctement. 

- `barberWaiting` : lorsque le barbier appelle le prochain client dans `pickNextClient`, il attend que ce dernier arrive sur la `workingChair`. Le client notifie le barbier lorsqu'il arrive sur la chaise de travail dans `goForHairCut()`.  

- `clientCutWaiting` :  dès que le client est sur la `workingChair` dans `goForHairCut()`, il attend que le barbier ait fini de lui couper les cheveux. Le barbier notifie le client lorsque la coupe est faite dans `beautifyClient()`.

- `chairs` : ce vecteur de variables de condition permet de gérer les clients dans la salle d'attente du salon.  
Ce vecteur agit comme un buffer circulaire. Nous utilisons deux variables `freeChairIndex` et `nextClientChairIndex` qui désignent respectivement l'index d'écriture et de lecture dans ce buffer, c-à-d, où nous plaçons le client qui vient d'arriver et quel client sera appelé par le barbier pour aller se couper les cheveux, respectant ainsi l'ordre d'arrivée des clients. 
Chaque client nécessitant d'attendre dans le salon effectue un wait sur la prochaine chaise disponible `freeChairIndex` dans `accessSalon()`. Le client est débloqué dans `pickNextClient()` par le barbier, lorsque celui-ci notifie un client sur la chaise à l'index `nextClientChairIndex`. 

- `_mutex` : ce mutex qui nous a été fourni est utilisé 

En plus, nous avons les variables suivantes:
- bool `barberAwake` : indique si le barbier est réveillé.
- bool `isOpen` : indique si le salon est en service. 
- unsigned `nbWaitingClients` : qui compte le nombre de client en salle d'attente. 

## Fin de service
__TODO__

**// A enlever?**
(__TODO__ j'avais commencé comme ci-dessous, mais j'ai changé afin d'expliquer le projet en fonction des variables de condition. Qu'est-ce que tu en penses?)

### Premier client

Lorsque qu'un client arrive et qu'il n'y a personne dans le salon (à part le barbier), celui-ci passe directement à la chaise de travail. 
Comme dans ce cas, le barbier serait en train de dormir, il notifie le barbier sur la variable de condition `barberSleeping` qui va réveiller le barbier qui attend sur cette variable dans `goToSleep()`.

### Gestion des chaises dans la salle d'attente

Afin de gérer les clients dans la salle d'attente, nous avons crée un vecteur de PcoConditionVariable `chairs` que nous initialisons dans le constructeur de `PcoSalon` et qui prend comme taille la `capacity` du salon qui correspond au nombre de chaises dans la salle d'attente. 

Ce vecteur agit comme un buffer circulaire. Nous utilisons deux variables `freeChairIndex` et `nextClientChairIndex` qui désignent respectivement l'index d'écriture et de lecture dans ce buffer, c-à-d, où nous plaçons le client qui vient d'arriver et quel client sera appelé par le barbier pour aller se couper les cheveux, respectant ainsi l'ordre d'arrivée des clients. 

Chaque client nécessitant d'attendre dans le salon effectue un wait sur la prochaine chaise disponible `freeChairIndex` dans `accessSalon()`. Le client est débloqué dans `pickNextClient()` par le barbier, lorsque celui-ci notifie un client sur la chaise à l'index `nextClientChairIndex`. 

**// Fin A enlever?**

### Animations
-__TODO__ Dire quando nous faisons les animations?

### Diagramme
__TODO__ Faire diagramme car ca facilite la compréhension?


## Tests effectués

__TODO__ reformuler + ajouter des tests

- Simulation d'un Salon Plein : Vérification de la capacité du salon à gérer un nombre maximal de clients, en s'assurant qu'ils entrent, attendent, se font coiffer, et quittent correctement. **OK**. 

- Test de la Synchronisation Client-Barbier : Validation de la synchronisation entre les clients et le barbier, en s'assurant que le barbier ne coiffe que les clients présents sur la chaise de travail.**OK**.

- Test de la Salle d'Attente : Vérification du bon fonctionnement de la salle d'attente, en s'assurant que le nombre de clients en attente ne dépasse pas la capacité des chaises disponibles.**OK**.

- Scénario de Fermeture du Salon : Test de la gestion de la fermeture du salon, en vérifiant que les clients déjà présents dans le salon sont correctement pris en charge avant la fermeture.**OK**.
    - Test lorsque le barbier dort. 
    - Test à d'autres moments. 

- Test client plus rapide que barbier: teste lorsque le client arrive sur la chaise de travail avant le barbier. **NOT OK**.

- Test différent nombre de chaises dans salle d'attente. 

- Test différent nombre de clients. (5 à 8) 

- Test cas spécial: 1 chaise et 30 personnes.



## Conclusion

__TODO__ Citer problèmes




# LISTE DE QUESTIONS 
- Expliquer fonctionnement des clients? Barbier? Salon? Ou c'est trop car c'est un peu comme expliquer la machine d'états/dans la consigne du labo? Est-ce qu'on fait aussi un diagramme car ca facilite la compréhension?
- Expliquer run client/barbier?
- Citer les cas où le barbier va dormir?
- Enlever partie // A enlever   ??
- Expliquer gestion des animations?
- Ajouter diagramme?
