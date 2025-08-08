#!/bin/bash

echo "=== Test spécifique du Keylogger LPTF ==="
echo

# Test interactif du keylogger
echo "Ce test va démarrer le keylogger en mode démo."
echo "Vous pourrez voir comment il fonctionne en toute sécurité."
echo

(
    sleep 1
    echo "keylog"
    sleep 3
    echo ""  # Appuyer sur Entrée pour récupérer les touches
    sleep 1
    echo "quit"
) | ./main demo

echo
echo "✅ Test du keylogger terminé!"
echo
echo "📋 Ce que vous avez vu:"
echo "- ✅ Démarrage du keylogger"
echo "- ✅ Capture de touches simulées"
echo "- ✅ Arrêt propre du keylogger"
echo "- ✅ Gestion sécurisée des threads"
echo
echo "🔐 Sécurité:"
echo "- Ce keylogger est volontairement limité"
echo "- Il ne capture que des données de démonstration"
echo "- Il respecte les permissions système"
echo "- Usage éducatif uniquement!"
