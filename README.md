:fr:

# Moniteur Système - README

Ce projet est un moniteur système de bureau conçu pour surveiller les ressources et les performances du système d'exploitation de votre ordinateur. Il utilise le langage de programmation C++ et la bibliothèque d'interface utilisateur Dear ImGui. 

**ATTENTION** : Ce programme n'est compatible qu'avec Linux pour le moment. Lancé sur tout autre OS, il ne fonctionnera pas.

## Installation

- **Récupération du Projet** : Clonez ou téléchargez ce projet depuis le référentiel Git.

## Utilisation

Après avoir récupéré le projet, suivez ces étapes pour compiler et utiliser le projet :

1. **Compilation** : Naviguez dans le répertoire racine du projet, où se trouve le fichier `Makefile`. Le projet est accompagné d'un Makefile qui simplifie la compilation. Exécutez la commande `make` pour compiler le projet.

2. **Exécution** : Une fois que le projet a été compilé avec succès, exécutez l'application en utilisant la commande suivante : ```./monitor```

3. **Interface Utilisateur** : L'interface utilisateur Dear ImGui s'ouvrira, affichant différentes sections de surveillance. Vous pouvez explorer les onglets CPU, Mémoire et Réseau pour obtenir des informations détaillées sur les ressources système.

4. **Fonctionnalités Principales** : Voici un aperçu des fonctionnalités principales du projet :

- **Moniteur Système** : Affiche des informations telles que le type de système d'exploitation, l'utilisateur connecté, le nom de l'ordinateur, le nombre total de tâches/processus en cours d'exécution, le type de CPU, etc.

- **Moniteur de Mémoire et de Processus** : Affiche l'utilisation de la mémoire physique (RAM), de la mémoire virtuelle (SWAP) et de l'espace disque. Vous pouvez également filtrer et sélectionner des processus dans un tableau détaillé.

- **Moniteur Réseau** : Affiche des informations sur les interfaces réseau IPv4, avec des tableaux de données pour la réception (RX) et la transmission (TX). Les données sont également affichées sous forme de barres de progression.



-----------------------------------------------------------------------------------------------------------------------------------------------------------------------



:uk:

# System Monitor - README

This project is a desktop system monitor designed to monitor the resources and performance of your computer's operating system. It uses the C++ programming language and the Dear ImGui user interface library.

**WARNING**: This program is currently only compatible with Linux. Running it on any other operating system will not work.

## Installation

- **Project Retrieval**: Clone or download this project from the Git repository.

## Usage

After retrieving the project, follow these steps to compile and use the project:

1. **Compilation**: Navigate to the project's root directory where the `Makefile` is located. The project comes with a Makefile that simplifies compilation. Execute the `make` command to compile the project.

2. **Execution**: Once the project has been successfully compiled, run the application using the following command: ```./monitor```

3. **User Interface**: The Dear ImGui user interface will open, displaying various monitoring sections. You can explore the CPU, Memory, and Network tabs to get detailed information about system resources.

4. **Key Features**: Here is an overview of the project's main features:

- **System Monitor**: Displays information such as the type of operating system, the logged-in user, the computer's name, the total number of running tasks/processes, the type of CPU, and more.

- **Memory and Process Monitor**: Shows the usage of physical memory (RAM), virtual memory (SWAP), and disk space. You can also filter and select processes in a detailed table.

- **Network Monitor**: Provides information about IPv4 network interfaces, with data tables for receiving (RX) and transmitting (TX). Data is also displayed as progress bars.