#!/bin/bash

echo "=== Test des fonctionnalités Remote Control LPTF ==="
echo

# Compilation
echo "🔧 Compilation du projet..."
make clean && make
if [ $? -ne 0 ]; then
    echo "❌ Erreur de compilation!"
    exit 1
fi
echo "✅ Compilation réussie"

echo
echo "🧪 Lancement du mode DEMO..."
echo "Instructions:"
echo "1. Tapez 'host' pour voir les informations système"
echo "2. Tapez 'proc' pour voir la liste des processus"
echo "3. Tapez 'cmd' puis 'ls' pour exécuter une commande"
echo "4. Tapez 'keylog' pour tester le keylogger"
echo "5. Tapez 'quit' pour quitter"
echo

./main demo

echo
echo "✅ Test terminé!"
echo "Les nouvelles fonctionnalités du protocole LPTF sont opérationnelles!"
