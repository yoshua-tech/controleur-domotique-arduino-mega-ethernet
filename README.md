# Réalisation d'un contrôleur domotique intelligent basé sur Arduino MEGA et Ethernet

## Description
Projet de fin de licence en Télécommunication et Réseauxinformatiques - ESMICOM 2026.
Ce système permet de contrôler à distance 5 appareils électriques via une interface web et de surveiller la température et l'humidité et aussi renvoi une alarme (LED+Buzzer actifs, background de la température change en rouge) si la température devient >= 35.
La communication se fait par Ethernet avec un shield Ethernet. L'interface utilise AJAX pour la mise à jour en temps réel sans recharger la page.

**Mots-clés** : `Domotique`, `IoT`, `Arduino Mega`, `Ethernet `, `AJAX`, `DHT11`, `RDC`

## Démonstration Vidéo
Vidéo du système en fonctionnement :
[[Voir la démo](https://youtu.be/hfYGUMELLjc?si=uoAAo8H8xn04PRKs)]

## Matériel utilisé
- **Carte principale** : Arduino Mega 2560
- **Module réseau** : Ethernet Shield V2
- **Actionneurs** : 5 Relais 12V pour contrôle des appareils 220V
- **Capteur** : 1x DHT11 - Température et Humidité
- **Signalisation** : 1x Buzzer actif + LEDs de statut
- **Autres** : Alimentation 12V, Boîtier

## Fonctionnalités principales
1. **Contrôle à distance** : Allumer/Éteindre 5 appareils depuis PC ou Smartphone
2. **Monitoring** : Lecture de la Température et Humidité en temps réel
3. **Alertes sonores et visuelles** : Enclenchement de l'alarme selon les conditions
4. **Interface Web** : Page HTML/CSS/JS responsive sur réseau local
5. **Fiabilité** : Connexion Ethernet filaire, pas de dépendance au Wifi

## Schéma de câblage
[Schéma de câblage](/shc_sch_schéma.png)

## Nous résumons l'intelligence du système

         CAPTER
          ↓
          DHT11
          ↓
       ANALYSER
          ↓
       Comparaison avec
       seuil 35 °C
          ↓
       DÉCIDER
       ↙     ↘
    Température Température
    normale       excessive
    ↓              ↓
   Surveillance   ALARME
                  ↓
          LED + BUZZER       
            ↑
            │
         COMMUNIQUER
            │
        Shield Ethernet
            │
            ↓
        Interface Web AJAX
            │
            ↓
         UTILISATEUR
            │
            ↓
         COMMANDER
            │
            ↓
         5 relais
  
       .
##

## Installation et Utilisation
1. **Prérequis** : Arduino IDE 1.8+
2. **Librairies** : Installer `Ethernet` et `DHT sensor library` via le gestionnaire de librairies
3. **Câblage** : Suivre le schéma dans `/main` et prière de bien vouloir enlever le PIR car non pris en charge par le code dans [Code](/domotique.ino)
4. **Configuration** : Modifier l'adresse IP statique dans [Code](/domotique.ino) selon votre réseau
5. **Téléversement** : Charger le code sur l'Arduino Mega
6. **Accès** : Ouvrir un navigateur et taper l'IP de l'Arduino. Ex: `http://192.168.1.177`

## Auteur
**MADINDA KILOLO Josué**
Licencié en Télécommunication et Réseaux informatiques - ESMICOM 2026
Kinshasa, République Démocratique du Congo
 [madindajosue7@gmail.com]

## Licence
Ce projet est sous licence MIT. Vous êtes libre de l'utiliser, le modifier et le distribuer.

## Remerciements
Encadreur : Mr LUKELE KALUNGA Patrick - ESMICOM
