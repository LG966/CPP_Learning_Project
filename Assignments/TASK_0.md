# Se familiariser avec l'existant

## A- Exécution

Compilez et lancez le programme.

Allez dans le fichier `tower_sim.cpp` et recherchez la fonction responsable de gérer les inputs du programme.
Sur quelle touche faut-il appuyer pour ajouter un avion ?
Comment faire pour quitter le programme ?
A quoi sert la touche 'F' ?

Les touches disponibles :
- `c` spawn un avion
- `x` exit le programme
- `q` exit le programme
- `+` zoommer
- `-` dézoomer
- `F` full screen

Ajoutez un avion à la simulation et attendez.
Que est le comportement de l'avion ?
Quelles informations s'affichent dans la console ?

Lorqu'on ajoute un avion, il spawn quelque part en haut de l'écran, puis atterit. Au bout d'un certain moment, il repart. Ces opérations sont documentés sur la sortie standard du programme.

Ajoutez maintenant quatre avions d'un coup dans la simulation.
Que fait chacun des avions ?

L'aéroport peut accueillir au maximum 3 avions. Les autres avions sont donc en attente et tournent dans le ciel.

## B- Analyse du code

Listez les classes du programme à la racine du dossier src/.
Pour chacune d'entre elle, expliquez ce qu'elle représente et son rôle dans le programme.

Aircraft : C'est un avion. Il tourne dans le ciel, atterit, se ravitaille et repart de l'aéroport.

Aircraft_types : Types d'avion. Il en existe plusieurs.

Airport : Représente un aéroport. Il accueille des avions, possède des terminaux ainsi qu'une tour de contrôle.

Config : Fichier de configuration. On y trouve des constantes qui sont utilisées dans le reste du code.

Geometry : Petite librairie pour la géométrie dans un plan 2D ou 3D.

Main : Permet de lancer le programme

Runway : Piste d'atterrissage

Terminal : Terminal accueillant un avion à la fois. L'avion attend un certain moment dedans, puis repart.

Tower : Tour de contrôle, elle coordonne les avions, en leur donnant des instructions sur quand et où atterrir par exemple.

Tower_sim : C'est le programme global, coordonne la création des avions, les raccourcis clavier l'initialisation de l'airport ...

Pour les classes `Tower`, `Aircaft`, `Airport` et `Terminal`, listez leurs fonctions-membre publiques et expliquez précisément à quoi elles servent.
Réalisez ensuite un schéma présentant comment ces différentes classes intéragissent ensemble.


-> **Tower**
- `Tower(Airport& airport_)` : Une Tower est rattachée à un Airport.
- `WaypointQueue get_instructions(Aircraft& aircraft);` : Donne les instructions à l'avion. Si l'avion est loin, on le rammène près de l'aéroport, s'il est près, on tente de réserver un terminal. Si l'avion a un terminal dedié, alors on lui dit d'y aller.
- `void arrived_at_terminal(const Aircraft& aircraft);` : Commence le service de l'avion au terminal associé.

-> **Aircraft**
- `const std::string& get_flight_num()` : getter sur le numéro de flight
- `float distance_to(const Point3D& p)` : distance de l'avion par rapport à un autre point
- `display()` : affiche l'avion
- `move()` : déplace l'avion à son prochain waypoint

-> **Airport**
- `move()` : avance le service des terminaux
- `display()` : affiche l'airport

-> **Terminal**
- `in_use()` : est ce qu'un avion est stationné dans le terminal
- `is_servicing()` : si un avion est stationné et qu'il est entrain d'être servi
- `assign_craft()` : assigne un avion au terminal, s'il est assez près
- `finish_service()` : fini le service et libère l'avion si il n'est plus servi
- `move()` : fait progresser le service

![NSv12W8n38NXjJd5eqkne5iNCjx12uJQqS8qaJI3U7ihCeAppfylsH0DpTbyhXETYptK7DiO4Gu15trcmn59nRb8-mygowitsMeHdbPPCXuTCVwC4y8jE_hORKvUL1f6qnfe4RxgVjSFBq3VktEZ8SlryzyR](https://user-images.githubusercontent.com/79719146/163730013-138b75c2-5b9e-47e6-bfe2-e245f867f191.png)

Quelles classes et fonctions sont impliquées dans la génération du chemin d'un avion ?

Le chemin d'un avion est représenté par une suite d'instructions, c'est à dire de `Waypoint`.
Un `Waypoint` n'est rien d'autre qu'une position dans l'espace (vertex) ainsi qu'un type (aérien, aéroport, terminal).

C'est la `Tower` qui génére les instructions aux avions, pour les diriger vers la piste d'atterrissage, les terminaux ou pour les maintenir en l'air.
Les avions gérent leur déplacement en se déplaçant vers leur prochain `Waypoint`.

Quel conteneur de la librairie standard a été choisi pour représenter le chemin ?
Expliquez les intérêts de ce choix.

```cpp
using WaypointQueue = std::deque<Waypoint>;
```

Un chemin est donc une file et c'est logique; on exécute les instructions des plus anciennes aux plus récentes.

## C- Bidouillons !

1) Déterminez à quel endroit du code sont définies les vitesses maximales et accélération de chaque avion.
Le Concorde est censé pouvoir voler plus vite que les autres avions.
Modifiez le programme pour tenir compte de cela.

```cpp
    // Dans aircraft_types.cpp 
    aircraft_types[2] = new AircraftType { .02f, .45f, .45f, MediaPath { "concorde_af.png" } };
```

L'avion se crash !

2) Identifiez quelle variable contrôle le framerate de la simulation.
Ajoutez deux nouveaux inputs au programme permettant d'augmenter ou de diminuer cette valeur.
Essayez maintenant de mettre en pause le programme en manipulant ce framerate. Que se passe-t-il ?\
Ajoutez une nouvelle fonctionnalité au programme pour mettre le programme en pause, et qui ne passe pas par le framerate.

```cpp
    // Dans config.cpp, on modifie cette valeur
    constexpr unsigned int DEFAULT_TICKS_PER_SEC = 16u;
```

On ajoute une fonction dans `opengl_interface.cpp` pour modifier le framerate :

```cpp
void increase_ticks(const int n)
{
    ticks_per_sec += n;
}
```

On ajoute des fonctions dans `tower_sim.cpp` qui décrémente/incrémente la framerate.
```cpp
    GL::keystrokes.emplace('i', []() { GL::increase_ticks(1); });
    GL::keystrokes.emplace('o', []() { GL::increase_ticks(-1); });
```

Lorque'on passe à une framerate de 0, le programme crash (Exception en point flottant).

Pour mettre en pause le programme, on crée une constante qui régule la boucle principale du programme :

```cpp
    void timer(const int step)
    {
        if (pause){
            for (auto& item : move_queue)
            {
                item->move();
            }
        }
        glutPostRedisplay();
        glutTimerFunc(1000u / ticks_per_sec, timer, step + 1);
    }
```


3) Identifiez quelle variable contrôle le temps de débarquement des avions et doublez-le.

```cpp
    constexpr unsigned int SERVICE_CYCLES = 40u;
```

Cette constante représente le nombre de tick nécessaire pour service un avion. On double donc cette constante. 
(Move() de terminal incrémente cette valeur)


4) Lorsqu'un avion a décollé, il réattérit peu de temps après.
Faites en sorte qu'à la place, il soit retiré du programme.\
Indices :\
A quel endroit pouvez-vous savoir que l'avion doit être supprimé ?\
Pourquoi n'est-il pas sûr de procéder au retrait de l'avion dans cette fonction ?
A quel endroit de la callstack pourriez-vous le faire à la place ?\
Que devez-vous modifier pour transmettre l'information de la première à la seconde fonction ?

L'avion peut être supprimé au moment du lift-off, dans la fonction `Aircraft::operate_landing_gear()`.
Seulement si on essaie de le supprimer au sein-même de la fonction, alors le programme plante, ce qui est logique.

```cpp
    // Cette solution barbare ne marche pas !
    if (ground_before && !ground_after)
    {
        std::cout << flight_number << " lift off" << std::endl;
        delete this;
    }
```

Une solution consiste à ajouter un waypoint de type `destroy` au niveau du lift-off, de sorte que si le prochain waypoint dans move est de type destroy, alors set un flag `_to_delete` à true sur l'objet `displayable`. Dans la fonction qui appelle `move()` sur chaque objet,
on supprime tous les objets avec ce flag de la queue, en appelant leur destructeur. Ce destructeur se charge de retirer l'objet de la display_queue.

5) Lorsqu'un objet de type `Displayable` est créé, il faut ajouter celui-ci manuellement dans la liste des objets à afficher.
Il faut également penser à le supprimer de cette liste avant de le détruire.
Faites en sorte que l'ajout et la suppression de `display_queue` soit "automatiquement gérée" lorsqu'un `Displayable` est créé ou détruit.
Pourquoi n'est-il pas spécialement pertinent d'en faire de même pour `DynamicObject` ?

C'est en itérant sur la move_queue qu'on détecte les objets à détruire, on peut donc retirer l'objet à ce moment là au lieu de re-parcourir la move_queue avec un destructeur.

6) La tour de contrôle a besoin de stocker pour tout `Aircraft` le `Terminal` qui lui est actuellement attribué, afin de pouvoir le libérer une fois que l'avion décolle.
Cette information est actuellement enregistrée dans un `std::vector<std::pair<const Aircraft*, size_t>>` (size_t représentant l'indice du terminal).
Cela fait que la recherche du terminal associé à un avion est réalisée en temps linéaire, par rapport au nombre total de terminaux.
Cela n'est pas grave tant que ce nombre est petit, mais pour préparer l'avenir, on aimerait bien remplacer le vector par un conteneur qui garantira des opérations efficaces, même s'il y a beaucoup de terminaux.\
Modifiez le code afin d'utiliser un conteneur STL plus adapté. Normalement, à la fin, la fonction `find_craft_and_terminal(const Aicraft&)` ne devrait plus être nécessaire.

On peut utiliser une unordered_map. La clé sera l'avion, la valeur sera le terminal.
On utilise alors la méthode `find()` sur la map qui a le même comportement que `find_craft_and_terminal`.


## D- Théorie

1) Comment a-t-on fait pour que seule la classe `Tower` puisse réserver un terminal de l'aéroport ?

La structure associant les avions aux terminaux est privée. Cela signifie donc que seul la classe `Tower` peut la modifier directement.


2) En regardant le contenu de la fonction `void Aircraft::turn(Point3D direction)`, pourquoi selon-vous ne sommes-nous pas passer par une réference ?
Pensez-vous qu'il soit possible d'éviter la copie du `Point3D` passé en paramètre ?

On modifie le paramètre direction en appelant la méthode `cap_length` dessus. Ainsi, si c'est une référence, on va modifier l'objet original. Or `turn` est appelé avec l'argument `target - pos - speed` dans `aircraft.cpp`. Ici ça ne poserait donc pas problème de passer une référence et de la modifier. 

Toutefois, la nature de la fonction n'est pas de modifier la direction qui lui est passé en paramètre, il est donc préférable ici de faire une copie. Une autre solution consiste à effectivement passer une référence constante en paramètre, et d'en faire une copie non constante, ce qui revient à la même chose au final.

## E- Bonus

Le temps qui s'écoule dans la simulation dépend du framerate du programme.
La fonction move() n'utilise pas le vrai temps. Faites en sorte que si.
Par conséquent, lorsque vous augmentez le framerate, la simulation s'exécute plus rapidement, et si vous le diminuez, celle-ci s'exécute plus lentement.

Dans la plupart des jeux ou logiciels que vous utilisez, lorsque le framerate diminue, vous ne le ressentez quasiment pas (en tout cas, tant que celui-ci ne diminue pas trop).
Pour avoir ce type de résultat, les fonctions d'update prennent généralement en paramètre le temps qui s'est écoulé depuis la dernière frame, et l'utilise pour calculer le mouvement des entités.

Recherchez sur Internet comment obtenir le temps courant en C++ et arrangez-vous pour calculer le dt (delta time) qui s'écoule entre deux frames.
Lorsque le programme tourne bien, celui-ci devrait être quasiment égale à 1/framerate.
Cependant, si le programme se met à ramer et que la callback de glutTimerFunc est appelée en retard (oui oui, c'est possible), alors votre dt devrait être supérieur à 1/framerate.

Passez ensuite cette valeur à la fonction `move` des `DynamicObject`, et utilisez-la pour calculer les nouvelles positions de chaque avion.
Vérifiez maintenant en exécutant le programme que, lorsque augmentez le framerate du programme, vous n'augmentez pas la vitesse de la simulation.

Ajoutez ensuite deux nouveaux inputs permettant d'accélérer ou de ralentir la simulation.
