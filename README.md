# Pokemon Rumble World - Plugin 3GX (Version Finale)

## Compilation via GitHub Actions
Ce projet compile automatiquement un plugin .3gx pour Luma3DS.

### Etapes
1. Uploade ces fichiers sur ton repo GitHub
2. Va dans Actions → le build se lance auto
3. Attends 5-10 minutes
4. Telecharge l artifact "PokemonRumbleWorld-3GX"

### Installation
- Copie le .3gx dans : `/luma/plugins/0004000000176B00/plugin.3gx`
- Active "Plugin Loader" dans Luma3DS (menu au boot avec Select)
- Lance le jeu, appuie sur Select en jeu

### Fonctionnalites
- Monnaie (Diamants, Pieces, Mine Infinie)
- Pokemon (Changer, Capture Auto)
- Combat (Vie Inf, Mega Inf, Invincible, OHKO)
- Autre (Rang 3GX, Hex Editor, Scan AOB)

### Si le build echoue
Le probleme vient souvent de CTRPluginFramework. Verifie les logs.
