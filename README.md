# Programmation orienté objet : Equation de la chaleur 1D

L'objectif de ce projet, est d'implémenter la résolution de l'équation de la chaleur, notamment par les méthodes de Résolution Jacobi et Gauss-Seidel.  
Ce projet permet de comparer les vitesses d'executions de ces deux méthodes, en parallèlisme et en séquentiel. On peut également lancer l'execution du programme en précisant les paramètres de maillage que l'on veut utiliser ou bien executer directement ./simulator pour utiliser les paramètres par défaut (0.0 1.0 0.01 uniform). Le programme résout également l'équation avec plusieurs initialisations (initialisation deux zones(T1 pour la première moitié du maillage et T2 pour la seconde), ou bien uniform (la même température partout, ici (T1+T2)/2)).  

Le programme permet aussi de générer des tableaux de données pour les variables Gauss-Seidel ainsi que Jacobi. Ce qui permet une visualisation `gnuplot` grâce aux fichiers prévus pour afficher les graphiques.  
Il permet aussi de générer un fichier .vtk, ce qui permet d'ouvrir le fichier sur paraview et de bénéficier d'une visualisation plus élaborée. 


### Compilation : 
Pour compiler et exécuter le projet, suivez ces étapes :

1. **Créer un répertoire build** :
    - Exécutez la commande suivante pour créer un répertoire `build` :
    ```bash
    mkdir build
    ```

2. **Se rendre dans le répertoire build** :
    ```bash
    cd build
    ```

3. **Construire le Programme** :
    - Exécutez une des commandes suivantes pour avoir le mode DEBUG ou RELEASE :
    ```bash
    cmake -DCMAKE_BUILD_TYPE=Debug ..  
    cmake -DCMAKE_BUILD_TYPE=Release ..
    ```

4. **Construire le Programme** :
    - Exécutez la commande suivante pour compiler le programme et générer l'exécutable principal (`simulator`) :
    ```bash
    make ..  
    ```

5. **Exécuter le Programme** :
    - Une fois la compilation réussie, vous pouvez exécuter le programme avec :
    ```bash
    ./simulator
    ```
    - Ou bien en utilisant des paramètres choisis comme par exemple : 
    ```bash
    ./simulator 0.0 2.0 0.001 uniform
    ```
    - Cela exécutera le programme principal qui résout l'équation de la chaleur.

6. **Exécuter les Tests** :
    - Pour exécuter les tests unitaires avec `Gtest`, utilisez la commande suivante :
    ```bash
    ctest
    ```

7. **Afficher les graphs avec `gnuplot`** : 
    - Exécutez les commandes suivantes pour afficher les graphs `gnuplot`. Vous devez vous trouver dans la racine du projet pour lancer gnuplot.
    ```bash
    cd ..  
    gnuplot plot-Jacobi.gp  
    gnuplot plot-GaussSeidel.gp  
    ```