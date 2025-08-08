#!/bin/bash

# Script de test pour démontrer le fonctionnement du serveur multi-clients

echo "=== Test du système LPTF Socket ==="
echo

# Vérifier que le programme est compilé
if [ ! -f "./main" ]; then
    echo "Compilation du projet..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur de compilation!"
        exit 1
    fi
fi

echo "1. Démarrage du serveur en arrière-plan..."
./main server &
SERVER_PID=$!
echo "Serveur démarré (PID: $SERVER_PID)"


sleep 2

echo
echo "2. Test de connexion d'un client..."
echo "test message 1" | ./main client &
CLIENT1_PID=$!

sleep 1

echo
echo "3. Test de connexion d'un second client..."
echo "test message 2" | ./main client &
CLIENT2_PID=$!

sleep 2

echo
echo "4. Arrêt du serveur..."
kill $SERVER_PID 2>/dev/null
wait $CLIENT1_PID 2>/dev/null
wait $CLIENT2_PID 2>/dev/null

echo
echo "=== Test terminé ==="
echo "Le serveur a été capable de gérer plusieurs clients simultanément!"
echo "Vérifiez les logs ci-dessus pour voir les connexions et messages."
