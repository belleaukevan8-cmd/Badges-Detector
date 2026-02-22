# Badge Detector v1.2

App externe Flipper Zero — Momentum mntm-012

## Fichiers
- `application.fam` — manifest
- `badge_types.h` — struct BadgeInfo partagée
- `rfid_reader.c/h` — RFID 125kHz (LFRFIDWorker)
- `nfc_reader.c/h` — NFC 13.56MHz (NfcScanner)
- `storage_manager.c/h` — log CSV sur SD
- `ui_main.c/h` — menu principal
- `ui_result.c/h` — affichage résultat
- `badge_detector.c/h` — point d'entrée

## Log CSV
`SD:/badge_detector/badge_log.csv`

## Compilation FZOC
1. Mettre le dossier sur GitHub
2. FZOC → URL repo → Momentum mntm-012 → Compiler
3. Copier le `.fap` dans `SD:/apps/Tools/`
