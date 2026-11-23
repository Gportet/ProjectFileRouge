# Spécification du Format Binaire — Asset Engine

Ce document définit le contrat binaire de l'archive d'assets, conçu pour être indépendant de la représentation mémoire C++ et portable entre plateformes.

## 1. Conventions Générales & Encodage

* **Ordre des octets (Endianness) :** Toutes les valeurs numériques multi-octets (entiers et flottants) sont encodées en **Little-Endian**.


* **Alignement :** Aucun octet de remplissage (*padding*) n'est inséré entre les champs. Les données sont écrites de manière continue.

* **Base des offsets :** Tous les offsets sont exprimés en octets depuis le début absolu du fichier (`offset 0`).

### Encodage des Types Primitifs

| Type logique | Taille | Encodage binaire | 
| ----- | ----- | ----- | 
| `bool` | 1 octet | `0x00` pour `false`, `0x01` pour `true` (toute autre valeur est rejetée). | 
| `int8` / `uint8` | 1 octet | Entier 8 bits (complément à deux pour le signé). | 
| `int16` / `uint16` | 2 octets | Entier 16 bits Little-Endian. | 
| `int32` / `uint32` | 4 octets | Entier 32 bits Little-Endian. | 
| `int64` / `uint64` | 8 octets | Entier 64 bits Little-Endian. | 
| `float` | 4 octets | Norme IEEE 754 simple précision (32 bits), écrit en Little-Endian. | 
| `double` | 8 octets | Norme IEEE 754 double précision (64 bits), écrit en Little-Endian. | 
| `UTF-8 String` | $4 + N$ octets | Longueur $N$ en octets (`uint32`) suivie de $N$ octets bruts UTF-8 (sans caractère nul `\0`). | 
| `Collection / Vector` | $4 + \sum \text{éléments}$ | Nombre d'éléments $K$ (`uint32`) suivi des $K$ éléments sérialisés séquentiellement. | 

## 2. Architecture Globale du Fichier

Le fichier binaire est découpé en **trois sections contiguës** :

```
+-----------------------------------+  <-- Offset 0
| 1. HEADER (Taille fixe : 32 B)    |
+-----------------------------------+  <-- Offset 32 (DirectoryOffset)
| 2. ASSET DIRECTORY                |
|    (N entrées × 32 B)             |
+-----------------------------------+  <-- Offset 32 + (N × 32)
| 3. DATA PAYLOADS                  |
|    [Payload #1] [Payload #2] ...  |
+-----------------------------------+  <-- TotalFileSize

```

## 3. Détail des Sections

### Section 1 : Header (Taille fixe : 32 octets)

L'en-tête permet d'identifier le fichier et de valider son intégrité globale avant toute lecture ou allocation lourde.

| Offset | Champ | Type      | Taille   | Description | 
| ----- | ----- |-----------|----------| ----- | 
| `0` | **Magic Number** | `char[4]` | 5 octets | Signature fixe identifiant le format (`SINGE`). | 
| `4` | **Format Version** | `uint32`  | 4 octets | Version de la structure de l'archive. | 
| `8` | **Flags** | `uint32`  | 4 octets | Options globales de l'archive (`0` par défaut, bits réservés pour extensions). | 
| `12` | **Total File Size** | `uint64`  | 8 octets | Taille totale attendue du fichier en octets. | 
| `20` | **Checksum (CRC-32)** | `uint32`  | 4 octets | Empreinte CRC-32 couvrant l'intégralité du fichier (avec ce champ mis à `0` lors du calcul). | 
| `24` | **Asset Count** | `uint32`  | 4 octets | Nombre total d'assets ($N$) enregistrés dans l'archive. | 
| `28` | **Directory Offset** | `uint32`  | 4 octets | Position du début de l'Asset Directory (valeur fixe : `32`). | 

> **Note sur le Checksum :** Le CRC-32 protège à la fois les métadonnées (Header + Asset Directory) et l'ensemble des Payloads contre les corruptions accidentelles.


### Section 2 : Asset Directory (Taille : $N \times 32$ octets)

Situé immédiatement après le Header (`offset 32`), l'annuaire liste tous les assets présents pour permettre un accès direct par ID sans désérialiser les autres ressources.

Chaque entrée occupe **32 octets** :

| Offset relatif | Champ | Type | Taille | Description | 
| ----- | ----- | ----- | ----- | ----- | 
| `+0` | **Asset ID** | `uint64` | 8 octets | Identifiant unique et persistant de l'asset. | 
| `+8` | **Type ID** | `uint32` | 4 octets | Identifiant du type de donnée (ex: `1 = Player`, `2 = PhysicsScene`). | 
| `+12` | **Schema Version** | `uint32` | 4 octets | Version du schéma spécifique à cet asset (pour la migration V1 $\rightarrow$ V2). | 
| `+16` | **Data Offset** | `uint64` | 8 octets | Position exacte (en octets depuis le début du fichier) du payload de cet asset. | 
| `+24` | **Data Size** | `uint64` | 8 octets | Taille exacte en octets du payload sérialisé. | 

### Section 3 : Data Payloads (Taille variable)

Contient les blocs d'octets bruts de chaque asset sérialisé via son adaptateur dédié.

* Aucun séparateur n'est nécessaire entre deux payloads : leurs frontières sont strictement définies par `Data Offset` et `Data Size` dans l'annuaire.

* Les références entre objets au sein d'un payload (ex: un joint physique reliant deux corps) utilisent exclusivement leurs **IDs persistants** (`uint64`).

## 4. Exemple Concret de Calcul d'Offsets

Supposons une archive contenant $N = 2$ **assets** :

* **Asset #1 :** Taille sérialisée = `120 octets`

* **Asset #2 :** Taille sérialisée = `350 octets`

| Section / Élément | Offset de début | Taille | Offset de fin | 
| ----- | ----- | ----- | ----- | 
| **Header** | `0` | `32` octets | `32` | 
| **Directory — Entrée #1** | `32` | `32` octets | `64` | 
| **Directory — Entrée #2** | `64` | `32` octets | `96` | 
| **Payload — Asset #1** | `96` | `120` octets | `216` | 
| **Payload — Asset #2** | `216` | `350` octets | `566` | 

* **Total File Size :** `566 octets` (stocké dans le Header).

* **Accès direct à l'Asset #2 :** Le lecteur lit l'entrée #2 du Directory (`offset 64`), récupère `Data Offset = 216` et `Data Size = 350`, et lit directement la plage `[216, 566[` sans toucher aux octets `[96, 216[` de l'Asset #1.

## 5. Règles de Validation à la Lecture

Avant toute allocation mémoire ou désérialisation, le lecteur doit vérifier dans l'ordre :

1. `FileSize >= 32` (le fichier contient au moins un Header complet).

2. `Magic Number` correspond à la signature attendue.

3. `Format Version` est supportée (rejet explicite sinon).

4. `Total File Size` correspond à la taille réelle du fichier sur disque.

5. `Checksum` calculé correspond au `Checksum` enregistré dans le Header.

6. `Directory Offset + (Asset Count * 32) <= Total File Size` (sans dépassement d'entier).

7. Pour chaque entrée du Directory : `Data Offset <= Total File Size` et `Data Size <= Total File Size - Data Offset`, et aucun `Asset ID` n'est dupliqué.